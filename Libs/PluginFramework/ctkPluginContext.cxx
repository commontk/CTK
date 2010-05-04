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
#include "ctkPluginFrameworkContextPrivate_p.h"
#include "ctkServiceRegistration.h"
#include "ctkServiceReference.h"

#include <stdexcept>

namespace ctk {

  class PluginContextPrivate
  {

  public:

    PluginPrivate* plugin;

    PluginContextPrivate(PluginPrivate* plugin)
      : plugin(plugin)
    {}

    /**
     * Check that the plugin is still valid.
     */
    void isPluginContextValid()
    {
      if (!plugin) {
        throw std::logic_error("This plugin context is no longer valid");
      }
    }

  };


  PluginContext::PluginContext(PluginPrivate* plugin)
  : d_ptr(new PluginContextPrivate(plugin))
  {}

  PluginContext::~PluginContext()
  {
	  Q_D(PluginContext);
	  delete d;
  }

  Plugin* PluginContext::getPlugin(int id) const
  {
    Q_D(const PluginContext);
    return d->plugin->fwCtx->plugins->getPlugin(id);
  }

  QList<Plugin*> PluginContext::getPlugins() const
  {
    Q_D(const PluginContext);
    return d->plugin->fwCtx->plugins->getPlugins();
  }

  Plugin* PluginContext::installPlugin(const QUrl& location, QIODevice* in)
  {
    Q_D(PluginContext);
    d->isPluginContextValid();
    return d->plugin->fwCtx->plugins->install(location, in);
  }

  ServiceRegistration PluginContext::registerService(const QStringList& clazzes, QObject* service, const ServiceProperties& properties)
  {

  }

  QList<ServiceReference> PluginContext::getServiceReferences(const QString& clazz, const QString& filter)
  {

  }

  ServiceReference PluginContext::getServiceReference(const QString& clazz)
  {

  }

  QObject* PluginContext::getService(const ServiceReference& reference)
  {

  }


}
