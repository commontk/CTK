/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#include "ctkPluginTracker.h"

#include "ctkPluginTrackerPrivate.h"
#include "ctkTrackedPlugin_p.h"

#include <QDebug>

ctkPluginTracker::~ctkPluginTracker()
{

}

ctkPluginTracker::ctkPluginTracker(ctkPluginContext* context, ctkPlugin::States stateMask,
                 ctkPluginTrackerCustomizer* customizer)
  : d_ptr(new ctkPluginTrackerPrivate(this, context, stateMask, customizer))
{

}

void ctkPluginTracker::open()
{
  Q_D(ctkPluginTracker);
  QSharedPointer<ctkTrackedPlugin> t;
  {
    QMutexLocker lock(&d->mutex);
    if (d->trackedPlugin)
    {
      return;
    }

    if (d->DEBUG)
    {
      qDebug() << "ctkPluginTracker::open";
    }

    t = QSharedPointer<ctkTrackedPlugin>(new ctkTrackedPlugin(this, d->customizer));
    {
      QMutexLocker lockT(t.data());
      d->context->connectPluginListener(t.data(), SLOT(pluginChanged(ctkPluginEvent)), Qt::DirectConnection);
      QList<ctkPlugin*> plugins = d->context->getPlugins();
      int length = plugins.size();
      for (int i = 0; i < length; i++)
      {
        ctkPlugin::State state = plugins[i]->getState();
        if ((d->mask & state) == 0)
        {
          /* null out plugins whose states are not interesting */
          plugins[i] = 0;
        }
      }
      plugins.removeAll(0);
      /* set tracked with the initial bundles */
      t->setInitial(plugins);
    }
    d->trackedPlugin = t;
  }
  /* Call tracked outside of synchronized region */
  t->trackInitial(); /* process the initial references */
}

void ctkPluginTracker::close()
{
  Q_D(ctkPluginTracker);
  QList<ctkPlugin*> plugins;
  QSharedPointer<ctkTrackedPlugin> outgoing;
  {
    QMutexLocker lock(&d->mutex);
    outgoing = d->trackedPlugin;
    if (outgoing.isNull())
    {
      return;
    }

    if (d->DEBUG)
    {
      qDebug() << "ctkPluginTracker::close";
    }

    outgoing->close();
    plugins = getPlugins();
    d->trackedPlugin.clear();;

    outgoing->disconnect(SLOT(pluginChanged(ctkPluginEvent)));
  }

  foreach (ctkPlugin* plugin, plugins)
  {
    outgoing->untrack(plugin, ctkPluginEvent());
  }
}

QList<ctkPlugin*> ctkPluginTracker::getPlugins() const
{
  Q_D(const ctkPluginTracker);
  QSharedPointer<ctkTrackedPlugin> t = d->tracked();
  if (t.isNull())
  { /* if ctkPluginTracker is not open */
    return QList<ctkPlugin*>();
  }

  {
    QMutexLocker lock(t.data());
    return t->getTracked();
  }
}

QVariant ctkPluginTracker::getObject(ctkPlugin* plugin) const
{
  Q_D(const ctkPluginTracker);
  QSharedPointer<ctkTrackedPlugin> t = d->tracked();
  if (t.isNull())
  {
    return QVariant();
  }

  {
    QMutexLocker lock(t.data());
    return t->getCustomizedObject(plugin);
  }
}

void ctkPluginTracker::remove(ctkPlugin* plugin)
{
  Q_D(ctkPluginTracker);
  QSharedPointer<ctkTrackedPlugin> t = d->tracked();
  if (t.isNull())
  {
    return;
  }
  t->untrack(plugin, ctkPluginEvent());
}

int ctkPluginTracker::size() const
{
  Q_D(const ctkPluginTracker);
  QSharedPointer<ctkTrackedPlugin> t = d->tracked();
  if (t.isNull())
  {
    return 0;
  }

  {
    QMutexLocker lock(t.data());
    return t->size();
  }
}

int ctkPluginTracker::getTrackingCount() const
{
  Q_D(const ctkPluginTracker);
  QSharedPointer<ctkTrackedPlugin> t = d->tracked();
  if (t.isNull())
  {
    return -1;
  }

  {
    QMutexLocker lock(t.data());
    return t->getTrackingCount();
  }
}

QVariant ctkPluginTracker::addingPlugin(ctkPlugin* plugin, const ctkPluginEvent& event)
{
  Q_UNUSED(event)

  QVariant var;
  var.setValue(plugin);
  return var;
}

void ctkPluginTracker::modifiedPlugin(ctkPlugin* plugin, const ctkPluginEvent& event, QVariant object)
{
  Q_UNUSED(plugin)
  Q_UNUSED(event)
  Q_UNUSED(object)
  /* do nothing */
}

void ctkPluginTracker::removedPlugin(ctkPlugin* plugin, const ctkPluginEvent& event, QVariant object)
{
  Q_UNUSED(plugin)
  Q_UNUSED(event)
  Q_UNUSED(object)
  /* do nothing */
}
