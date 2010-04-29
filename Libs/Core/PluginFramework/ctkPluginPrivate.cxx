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

#include "ctkPluginPrivate_p.h"

#include "ctkPluginConstants.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginFrameworkContextPrivate_p.h"

namespace ctk {

  const Plugin::States PluginPrivate::RESOLVED_FLAGS = Plugin::RESOLVED | Plugin::STARTING | Plugin::ACTIVE | Plugin::STOPPING;


  PluginPrivate::PluginPrivate(
      Plugin& qq,
      PluginFrameworkContextPrivate* fw,
      PluginArchive* pa)
    : q_ptr(&qq), fwCtx(fw), id(pa->getPluginId()),
    location(pa->getPluginLocation()), state(Plugin::INSTALLED),
    archive(pa), pluginContext(0), pluginActivator(0), lastModified(0),
    lazyActivation(true), activating(false), deactivating(false),
    resolveFailException("")
  {
    //TODO
    //checkCertificates(pa);

    checkManifestHeaders();
  }

  PluginPrivate::PluginPrivate(Plugin& qq,
    PluginFrameworkContextPrivate* fw,
    int id, const QString& loc, const QString& sym, const Version& ver)
      : q_ptr(&qq), fwCtx(fw), id(id), location(loc), symbolicName(sym), version(ver),
      state(Plugin::INSTALLED), archive(0), pluginContext(0),
      pluginActivator(0), lastModified(0),
      lazyActivation(true), activating(false), deactivating(false),
      resolveFailException("")
  {

  }

  PluginPrivate::~PluginPrivate()
  {

  }

  QHash<QString, QString> PluginPrivate::getHeaders(const QString& locale)
  {
    return QHash<QString, QString>();
  }

  Plugin::States PluginPrivate::getUpdatedState()
  {
    if (state & Plugin::INSTALLED)
    {
      bool wasResolved = false;
      try
      {
        if (state == Plugin::INSTALLED)
        {
          fwCtx->resolvePlugin(this);
          wasResolved = true;
          state = Plugin::RESOLVED;
        }

      }
      catch (const PluginException& pe)
      {
        // TODO
        //fwCtx.listeners.frameworkError(this, be);
      }

      if (wasResolved)
      {
        // TODO
        // fwCtx.listeners.bundleChanged(new BundleEvent(BundleEvent.RESOLVED, this));
      }
    }

    return state;
  }

  void PluginPrivate::checkManifestHeaders()
  {
    symbolicName = archive->getAttribute(PluginConstants::PLUGIN_SYMBOLICNAME);

    if (symbolicName.isEmpty())
    {
      throw std::invalid_argument(std::string("Plugin has no symbolic name, location=") +
                                           qPrintable(location));
    }

    QString mpv = archive->getAttribute(PluginConstants::PLUGIN_VERSION);
    if (!mpv.isEmpty())
    {
      try
      {
        version = Version(mpv);
      }
      catch (const std::exception& e)
      {
        throw std::invalid_argument(std::string("Plugin does not specify a valid ") +
                                    qPrintable(PluginConstants::PLUGIN_VERSION) + " header. Got exception: " + e.what());
      }
    }

    QString ap = archive->getAttribute(PluginConstants::PLUGIN_ACTIVATIONPOLICY);
    if (PluginConstants::ACTIVATION_EAGER == ap)
    {
      lazyActivation = false;
    }

  }


}
