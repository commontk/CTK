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

#include "ctkPlugin_p.h"
#include "ctkPluginContext.h"
#include "ctkPluginContext_p.h"
#include "ctkPluginFrameworkContext_p.h"

#include "ctkServices_p.h"
#include "ctkServiceRegistration.h"
#include "ctkServiceReference.h"
#include "ctkServiceReference_p.h"

#include <stdexcept>

//----------------------------------------------------------------------------
ctkPluginContextPrivate::ctkPluginContextPrivate(ctkPluginPrivate* plugin)
  : plugin(plugin)
{}

//----------------------------------------------------------------------------
void ctkPluginContextPrivate::isPluginContextValid() const
{
  if (!plugin) {
    throw ctkIllegalStateException("This plugin context is no longer valid");
  }
}

//----------------------------------------------------------------------------
void ctkPluginContextPrivate::invalidate()
{
  plugin = 0;
}

//----------------------------------------------------------------------------
ctkPluginContext::ctkPluginContext(ctkPluginPrivate* plugin)
  : d_ptr(new ctkPluginContextPrivate(plugin))
{}

//----------------------------------------------------------------------------
ctkPluginContext::~ctkPluginContext()
{
  Q_D(ctkPluginContext);
  delete d;
}

//----------------------------------------------------------------------------
QVariant ctkPluginContext::getProperty(const QString& key) const
{
  Q_D(const ctkPluginContext);
  d->isPluginContextValid();
  return d->plugin->fwCtx->props.value(key);
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPluginContext::getPlugin() const
{
  Q_D(const ctkPluginContext);
  d->isPluginContextValid();
  return d->plugin->q_func();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPluginContext::getPlugin(long id) const
{
  Q_D(const ctkPluginContext);
  return d->plugin->fwCtx->plugins->getPlugin(id);
}

//----------------------------------------------------------------------------
QList<QSharedPointer<ctkPlugin> > ctkPluginContext::getPlugins() const
{
  Q_D(const ctkPluginContext);
  d->isPluginContextValid();
  return d->plugin->fwCtx->plugins->getPlugins();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPluginContext::installPlugin(const QUrl& location, QIODevice* in)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  return d->plugin->fwCtx->plugins->install(location, in);
}

//----------------------------------------------------------------------------
QFileInfo ctkPluginContext::getDataFile(const QString& filename)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  QDir dataRoot(d->plugin->getDataRoot().absolutePath());
  if (!dataRoot.exists())
  {
    if (!dataRoot.mkpath(dataRoot.absolutePath()))
    {
      qWarning() << "Could not create persistent storage area:" << dataRoot.absolutePath();
    }
  }
  return QFileInfo(dataRoot.absolutePath() + '/' + filename);
}

//----------------------------------------------------------------------------
ctkServiceRegistration ctkPluginContext::registerService(const QStringList& clazzes, QObject* service, const ctkDictionary& properties)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  return d->plugin->fwCtx->services->registerService(d->plugin, clazzes, service, properties);
}

//----------------------------------------------------------------------------
ctkServiceRegistration ctkPluginContext::registerService(const char* clazz, QObject* service, const ctkDictionary& properties)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  QStringList clazzes;
  clazzes.append(clazz);
  return d->plugin->fwCtx->services->registerService(d->plugin, clazzes, service, properties);
}

//----------------------------------------------------------------------------
QList<ctkServiceReference> ctkPluginContext::getServiceReferences(const QString& clazz, const QString& filter)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  return d->plugin->fwCtx->services->get(clazz, filter, 0);
}

//----------------------------------------------------------------------------
ctkServiceReference ctkPluginContext::getServiceReference(const QString& clazz)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  return d->plugin->fwCtx->services->get(d->plugin, clazz);
}

//----------------------------------------------------------------------------
QObject* ctkPluginContext::getService(const ctkServiceReference& reference)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();

  if (!reference)
  {
    throw ctkInvalidArgumentException("Default constructed ctkServiceReference is not a valid input to getService()");
  }
  ctkServiceReference internalRef(reference);
  return internalRef.d_func()->getService(d->plugin->q_func());
}

//----------------------------------------------------------------------------
bool ctkPluginContext::ungetService(const ctkServiceReference& reference)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  ctkServiceReference ref = reference;
  return ref.d_func()->ungetService(d->plugin->q_func(), true);
}

//----------------------------------------------------------------------------
bool ctkPluginContext::connectPluginListener(const QObject* receiver, const char* slot,
                                             Qt::ConnectionType type)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  // TODO check permissions for a direct connection
  if (type == Qt::DirectConnection || type == Qt::BlockingQueuedConnection)
  {
    return receiver->connect(&(d->plugin->fwCtx->listeners), SIGNAL(pluginChangedDirect(ctkPluginEvent)), slot, type);
  }
  else if (type == Qt::QueuedConnection)
  {
    return receiver->connect(&(d->plugin->fwCtx->listeners), SIGNAL(pluginChangedQueued(ctkPluginEvent)), slot, type);
  }
  else
  {
    throw ctkInvalidArgumentException("Only Qt::DirectConnection, Qt::QueuedConnection, or Qt::BlockingQueuedConnection are allowed as type argument.");
  }
}

//----------------------------------------------------------------------------
void ctkPluginContext::disconnectPluginListener(const QObject *receiver, const char* slot)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();

  QObject::disconnect(&(d->plugin->fwCtx->listeners), SIGNAL(pluginChangedDirect(ctkPluginEvent)), receiver, slot);
  QObject::disconnect(&(d->plugin->fwCtx->listeners), SIGNAL(pluginChangedQueued(ctkPluginEvent)), receiver, slot);
}

//----------------------------------------------------------------------------
bool ctkPluginContext::connectFrameworkListener(const QObject* receiver, const char* slot, Qt::ConnectionType type)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  // TODO check permissions for a direct connection
  return receiver->connect(&(d->plugin->fwCtx->listeners), SIGNAL(frameworkEvent(ctkPluginFrameworkEvent)), slot, type);
}

//----------------------------------------------------------------------------
void ctkPluginContext::disconnectFrameworkListener(const QObject *receiver, const char* slot)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();

  QObject::disconnect(&(d->plugin->fwCtx->listeners), SIGNAL(frameworkEvent(ctkPluginFrameworkEvent)), receiver, slot);
}

//----------------------------------------------------------------------------
void ctkPluginContext::connectServiceListener(QObject* receiver, const char* slot,
                                             const QString& filter)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  d->plugin->fwCtx->listeners.addServiceSlot(getPlugin(), receiver, slot, filter);
}

//----------------------------------------------------------------------------
void ctkPluginContext::disconnectServiceListener(QObject* receiver,
                                                const char* slot)
{
  Q_D(ctkPluginContext);
  d->isPluginContextValid();
  d->plugin->fwCtx->listeners.removeServiceSlot(getPlugin(), receiver, slot);
}
