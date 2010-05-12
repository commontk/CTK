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

#include "ctkPlugin.h"

#include "ctkPluginPrivate_p.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginFrameworkContext_p.h"

#include <QStringList>

namespace ctk {

  Plugin::Plugin(PluginFrameworkContext* fw,
                 PluginArchive* pa)
    : d_ptr(new PluginPrivate(*this, fw, pa))
  {

  }

  Plugin::Plugin(PluginPrivate& dd)
    : d_ptr(&dd)
  {

  }

  Plugin::~Plugin()
  {
    delete d_ptr;
  }

  Plugin::State Plugin::getState() const
  {
    Q_D(const Plugin);
    return d->state;
  }

  void Plugin::start(const StartOptions& options)
  {
    Q_D(Plugin);

    if (d->state == UNINSTALLED)
    {
      throw std::logic_error("Plugin is uninstalled");
    }

    // Initialize the activation; checks initialization of lazy
    // activation.

    //TODO 1: If activating or deactivating, wait a litle
    // we don't use mutliple threads to start plugins for now
    //waitOnActivation(lock, "Plugin::start", false);

    //2: start() is idempotent, i.e., nothing to do when already started
    if (d->state == ACTIVE)
    {
      return;
    }

    //3: Record non-transient start requests.
    if ((options & START_TRANSIENT) == 0)
    {
      d->setAutostartSetting(options);
    }

    //4: Resolve plugin (if needed)
    d->getUpdatedState();

    //5: Eager?
    if ((options & START_ACTIVATION_POLICY) && d->eagerActivation )
    {
      d->finalizeActivation();
    }
    else
    {
      if (STARTING == d->state) return;
      d->state = STARTING;
      d->pluginContext = new PluginContext(this->d_func());
      PluginEvent pluginEvent(PluginEvent::LAZY_ACTIVATION, this);
      d->fwCtx->listeners.emitPluginChanged(pluginEvent);
    }
  }

  void Plugin::stop(const StopOptions& options)
  {

  }

  PluginContext* Plugin::getPluginContext() const
  {
    //TODO security checks
    Q_D(const Plugin);
    return d->pluginContext;
  }

  long Plugin::getPluginId() const
  {
    Q_D(const Plugin);
    return d->id;
  }

  QString Plugin::getLocation() const
  {
    //TODO security
    Q_D(const Plugin);
    return d->location;
  }

  QHash<QString, QString> Plugin::getHeaders()
  {
    //TODO security
    Q_D(Plugin);
    if (d->cachedRawHeaders.empty())
    {
      d->cachedRawHeaders = d->archive->getUnlocalizedAttributes();
    }

    if (d->state == UNINSTALLED)
    {
      return d->cachedHeaders;
    }

    //TODO use the embedded .qm files to localize header values
    return d->cachedRawHeaders;
  }

  QString Plugin::getSymbolicName() const
  {
    Q_D(const Plugin);
    return d->symbolicName;
  }

  QStringList Plugin::getResourceList(const QString& path) const
  {
    Q_D(const Plugin);
    return d->archive->findResourcesPath(path);
  }

  QByteArray Plugin::getResource(const QString& path) const
  {
    Q_D(const Plugin);
    return d->archive->getPluginResource(path);
  }

  Version Plugin::getVersion() const
  {
    Q_D(const Plugin);
    return d->version;
  }

}
