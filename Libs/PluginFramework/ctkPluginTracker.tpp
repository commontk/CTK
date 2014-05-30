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


#include "ctkPluginContext.h"
#include "ctkPluginTracker_p.h"
#include "ctkTrackedPlugin_p.h"

#include <QDebug>

//----------------------------------------------------------------------------
template<class T>
ctkPluginTracker<T>::~ctkPluginTracker()
{

}

//----------------------------------------------------------------------------
template<class T>
ctkPluginTracker<T>::ctkPluginTracker(ctkPluginContext* context, ctkPlugin::States stateMask,
                                      PluginTrackerCustomizer* customizer)
  : d_ptr(new PluginTrackerPrivate(this, context, stateMask, customizer))
{

}

//----------------------------------------------------------------------------
template<class T>
void ctkPluginTracker<T>::open()
{
  Q_D(PluginTracker);
  QSharedPointer<TrackedPlugin> t;
  {
    QMutexLocker lock(&d->mutex);
    if (d->trackedPlugin)
    {
      return;
    }

    if (d->DEBUG)
    {
      qDebug() << "ctkPluginTracker<T>::open";
    }

    t = QSharedPointer<TrackedPlugin>(new TrackedPlugin(this, d->customizer));
    {
      QMutexLocker lockT(t.data());
      d->context->connectPluginListener(t.data(), SLOT(pluginChanged(ctkPluginEvent)), Qt::DirectConnection);
      QList<QSharedPointer<ctkPlugin> > plugins = d->context->getPlugins();
      int length = plugins.size();
      for (int i = 0; i < length; i++)
      {
        ctkPlugin::State state = plugins[i]->getState();
        if ((d->mask & state) == 0)
        {
          /* null out plugins whose states are not interesting */
          plugins[i].clear();
        }
      }
      plugins.removeAll(QSharedPointer<ctkPlugin>());
      /* set tracked with the initial bundles */
      t->setInitial(plugins);
    }
    d->trackedPlugin = t;
  }
  /* Call tracked outside of synchronized region */
  t->trackInitial(); /* process the initial references */
}

//----------------------------------------------------------------------------
template<class T>
void ctkPluginTracker<T>::close()
{
  Q_D(PluginTracker);
  QList<QSharedPointer<ctkPlugin> > plugins;
  QSharedPointer<TrackedPlugin> outgoing;
  {
    QMutexLocker lock(&d->mutex);
    outgoing = d->trackedPlugin;
    if (outgoing.isNull())
    {
      return;
    }

    if (d->DEBUG)
    {
      qDebug() << "ctkPluginTracker<T>::close";
    }

    outgoing->close();
    plugins = getPlugins();
    d->trackedPlugin.clear();;

    outgoing->disconnect(SLOT(pluginChanged(ctkPluginEvent)));
  }

  foreach (QSharedPointer<ctkPlugin> plugin, plugins)
  {
    outgoing->untrack(plugin, ctkPluginEvent());
  }
}

//----------------------------------------------------------------------------
template<class T>
QList<QSharedPointer<ctkPlugin> > ctkPluginTracker<T>::getPlugins() const
{
  Q_D(const PluginTracker);
  QSharedPointer<TrackedPlugin> t = d->tracked();
  if (t.isNull())
  { /* if ctkPluginTracker is not open */
    return QList<QSharedPointer<ctkPlugin> >();
  }

  {
    QMutexLocker lock(t.data());
    return t->getTracked();
  }
}

//----------------------------------------------------------------------------
template<class T>
T ctkPluginTracker<T>::getObject(QSharedPointer<ctkPlugin> plugin) const
{
  Q_D(const PluginTracker);
  QSharedPointer<TrackedPlugin> t = d->tracked();
  if (t.isNull())
  {
    return T();
  }

  {
    QMutexLocker lock(t.data());
    return t->getCustomizedObject(plugin);
  }
}

//----------------------------------------------------------------------------
template<class T>
void ctkPluginTracker<T>::remove(QSharedPointer<ctkPlugin> plugin)
{
  Q_D(PluginTracker);
  QSharedPointer<TrackedPlugin> t = d->tracked();
  if (t.isNull())
  {
    return;
  }
  t->untrack(plugin, ctkPluginEvent());
}

//----------------------------------------------------------------------------
template<class T>
int ctkPluginTracker<T>::size() const
{
  Q_D(const PluginTracker);
  QSharedPointer<TrackedPlugin> t = d->tracked();
  if (t.isNull())
  {
    return 0;
  }

  {
    QMutexLocker lock(t.data());
    return t->size();
  }
}

//----------------------------------------------------------------------------
template<class T>
int ctkPluginTracker<T>::getTrackingCount() const
{
  Q_D(const PluginTracker);
  QSharedPointer<TrackedPlugin> t = d->tracked();
  if (t.isNull())
  {
    return -1;
  }

  {
    QMutexLocker lock(t.data());
    return t->getTrackingCount();
  }
}

//----------------------------------------------------------------------------
template<class T>
QMap<QSharedPointer<ctkPlugin>, T> ctkPluginTracker<T>::getTracked() const
{
  QMap<QSharedPointer<ctkPlugin>, T> map;
  Q_D(const PluginTracker);
  QSharedPointer<TrackedPlugin> t = d->tracked();
  if (t.isNull())
  { /* if PluginTracker is not open */
    return map;
  }
  {
    QMutexLocker lock(t.data());
    return t->copyEntries(map);
  }
}

//----------------------------------------------------------------------------
template<class T>
bool ctkPluginTracker<T>::isEmpty() const
{
  Q_D(const PluginTracker);
  QSharedPointer<TrackedPlugin> t = d->tracked();
  if (t.isNull())
  { /* if PluginTracker is not open */
    return true;
  }
  {
    QMutexLocker lock(t.data());
    return t->isEmpty();
  }
}

//----------------------------------------------------------------------------
template<>
inline QSharedPointer<ctkPlugin> ctkPluginTracker<QSharedPointer<ctkPlugin> >::addingPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event)
{
  Q_UNUSED(event)

  return plugin;
}

//----------------------------------------------------------------------------
template<class T>
T ctkPluginTracker<T>::addingPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event)
{
  Q_UNUSED(plugin)
  Q_UNUSED(event)

  return 0;
}

//----------------------------------------------------------------------------
template<class T>
void ctkPluginTracker<T>::modifiedPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event, T object)
{
  Q_UNUSED(plugin)
  Q_UNUSED(event)
  Q_UNUSED(object)
  /* do nothing */
}

//----------------------------------------------------------------------------
template<class T>
void ctkPluginTracker<T>::removedPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event, T object)
{
  Q_UNUSED(plugin)
  Q_UNUSED(event)
  Q_UNUSED(object)
  /* do nothing */
}
