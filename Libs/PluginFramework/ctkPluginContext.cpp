/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkPluginPrivate_p.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkServiceRegistration.h"
#include "ctkServiceReference.h"
#include "ctkServiceReferencePrivate.h"

#include <stdexcept>


  class ctkPluginContextPrivate
  {

  public:

    ctkPluginPrivate* plugin;

    ctkPluginContextPrivate(ctkPluginPrivate* plugin)
      : plugin(plugin)
    {}

    /**
     * Check that the plugin is still valid.
     */
    void isPluginContextValid() const
    {
      if (!plugin) {
        throw std::logic_error("This plugin context is no longer valid");
      }
    }

    void invalidate()
    {
      plugin = 0;
    }

  };


  ctkPluginContext::ctkPluginContext(ctkPluginPrivate* plugin)
  : d_ptr(new ctkPluginContextPrivate(plugin))
  {}

  ctkPluginContext::~ctkPluginContext()
  {
    Q_D(ctkPluginContext);
    delete d;
  }

  ctkPlugin* ctkPluginContext::getPlugin() const
  {
    Q_D(const ctkPluginContext);
    d->isPluginContextValid();
    return d->plugin->q_func();
  }

  ctkPlugin* ctkPluginContext::getPlugin(long id) const
  {
    Q_D(const ctkPluginContext);
    return d->plugin->fwCtx->plugins->getPlugin(id);
  }

  QList<ctkPlugin*> ctkPluginContext::getPlugins() const
  {
    Q_D(const ctkPluginContext);
    d->isPluginContextValid();
    return d->plugin->fwCtx->plugins->getPlugins();
  }

  ctkPlugin* ctkPluginContext::installPlugin(const QUrl& location, QIODevice* in)
  {
    Q_D(ctkPluginContext);
    d->isPluginContextValid();
    return d->plugin->fwCtx->plugins->install(location, in);
  }

  ctkServiceRegistration* ctkPluginContext::registerService(const QStringList& clazzes, QObject* service, const ServiceProperties& properties)
  {
    Q_D(ctkPluginContext);
    d->isPluginContextValid();
    return d->plugin->fwCtx->services.registerService(d->plugin, clazzes, service, properties);
  }

  QList<ctkServiceReference> ctkPluginContext::getServiceReferences(const QString& clazz, const QString& filter)
  {
    Q_D(ctkPluginContext);
    d->isPluginContextValid();
    return d->plugin->fwCtx->services.get(clazz, filter);
  }

  ctkServiceReference ctkPluginContext::getServiceReference(const QString& clazz)
  {
    Q_D(ctkPluginContext);
    d->isPluginContextValid();
    return d->plugin->fwCtx->services.get(d->plugin, clazz);
  }

  QObject* ctkPluginContext::getService(ctkServiceReference reference)
  {
    Q_D(ctkPluginContext);
    d->isPluginContextValid();
    return reference.d_func()->getService(d->plugin->q_func());
  }

  bool ctkPluginContext::connectPluginListener(const QObject* receiver, const char* method,
                                            Qt::ConnectionType type)
  {
    Q_D(ctkPluginContext);
    // TODO check permissions for a direct connection
    return receiver->connect(&(d->plugin->fwCtx->listeners), SIGNAL(pluginChanged(ctkPluginEvent)), method, type);
  }

  bool ctkPluginContext::connectFrameworkListener(const QObject* receiver, const char* method, Qt::ConnectionType type)
  {
    Q_D(ctkPluginContext);
    return receiver->connect(&(d->plugin->fwCtx->listeners), SIGNAL(frameworkEvent(ctkPluginFrameworkEvent)), method, type);
  }
