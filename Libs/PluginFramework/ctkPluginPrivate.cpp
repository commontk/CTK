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
#include "ctkPluginDatabaseException.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginFrameworkUtil_p.h"
#include "ctkPluginActivator.h"

namespace ctk {

  const Plugin::States PluginPrivate::RESOLVED_FLAGS = Plugin::RESOLVED | Plugin::STARTING | Plugin::ACTIVE | Plugin::STOPPING;


  PluginPrivate::PluginPrivate(
      Plugin& qq,
      PluginFrameworkContext* fw,
      PluginArchive* pa)
    : q_ptr(&qq), fwCtx(fw), id(pa->getPluginId()),
    location(pa->getPluginLocation().toString()), state(Plugin::INSTALLED),
    archive(pa), pluginContext(0), pluginActivator(0), pluginLoader(pa->getLibLocation()),
    lastModified(0), eagerActivation(false), activating(false), deactivating(false)
  {
    //TODO
    //checkCertificates(pa);

    checkManifestHeaders();

    // fill require list
    QString requireString = archive->getAttribute(PluginConstants::REQUIRE_PLUGIN);
    QList<QMap<QString, QStringList> > requireList = PluginFrameworkUtil::parseEntries(PluginConstants::REQUIRE_PLUGIN,
                                                                         requireString, true, true, false);
    QListIterator<QMap<QString, QStringList> > i(requireList);
    while (i.hasNext())
    {
      const QMap<QString, QStringList>& e = i.next();
      const QStringList& res = e.value(PluginConstants::RESOLUTION_DIRECTIVE);
      const QStringList& version = e.value(PluginConstants::PLUGIN_VERSION_ATTRIBUTE);
      RequirePlugin* rp = new RequirePlugin(this, e.value("$key").front(),
                                            res.empty() ? QString() : res.front(),
                                            version.empty() ? QString() : version.front());
      require.push_back(rp);
    }
  }

  PluginPrivate::PluginPrivate(Plugin& qq,
    PluginFrameworkContext* fw,
    long id, const QString& loc, const QString& sym, const Version& ver)
      : q_ptr(&qq), fwCtx(fw), id(id), location(loc), symbolicName(sym), version(ver),
      state(Plugin::INSTALLED), archive(0), pluginContext(0),
      pluginActivator(0), lastModified(0),
      eagerActivation(false), activating(false), deactivating(false)
  {

  }

  PluginPrivate::~PluginPrivate()
  {
    qDeleteAll(require);
  }

  Plugin::State PluginPrivate::getUpdatedState()
  {
    if (state & Plugin::INSTALLED)
    {
      try
      {
        if (state == Plugin::INSTALLED)
        {
          fwCtx->resolvePlugin(this);
          state = Plugin::RESOLVED;
          fwCtx->listeners.emitPluginChanged(PluginEvent(PluginEvent::RESOLVED, this->q_func()));
        }

      }
      catch (const PluginException& pe)
      {
        Q_Q(Plugin);
        this->fwCtx->listeners.frameworkError(q, pe);
        throw;
      }
    }

    return state;
  }

  void PluginPrivate::setAutostartSetting(const Plugin::StartOptions& setting) {
    try
    {
      if (archive)
      {
        archive->setAutostartSetting(setting);
      }
    }
    catch (const PluginDatabaseException& e)
    {
      Q_Q(Plugin);
      this->fwCtx->listeners.frameworkError(q, e);
    }
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
      eagerActivation = true;
    }

  }

  void PluginPrivate::finalizeActivation()
  {
    switch (getUpdatedState())
    {
      case Plugin::INSTALLED:
        // we shouldn't be here, getUpdatedState should have thrown
        // an exception during resolving the plugin
        throw PluginException("Internal error: expected exception on plugin resolve not thrown!");
      case Plugin::STARTING:
        if (activating) return; // finalization already in progress.
        // Lazy activation; fall through to RESOLVED.
      case Plugin::RESOLVED:
        //6:
        state = Plugin::STARTING;
        activating = true;
        qDebug() << "activating #" << this->id;
        //7:
        if (!pluginContext)
        {
          pluginContext = new PluginContext(this);
        }
        try
        {
          //TODO maybe call this in its own thread
          start0();
        }
        catch (...)
        {
          //8:
          state = Plugin::STOPPING;
          // NYI, call outside lock
          fwCtx->listeners.emitPluginChanged(PluginEvent(PluginEvent::STOPPING, this->q_func()));
          removePluginResources();
          delete pluginContext;

          state = Plugin::RESOLVED;
          // NYI, call outside lock
          fwCtx->listeners.emitPluginChanged(PluginEvent(PluginEvent::STOPPED, this->q_func()));
          activating = false;
          throw;
        }
        activating = false;
        break;
      case Plugin::ACTIVE:
        break;
      case Plugin::STOPPING:
        // This happens if start is called from inside the PluginActivator::stop method.
        // Don't allow it.
        throw PluginException("start called from PluginActivator::stop",
                              PluginException::ACTIVATOR_ERROR);
      case Plugin::UNINSTALLED:
        throw std::logic_error("Plugin is in UNINSTALLED state");
      }
  }

  void PluginPrivate::start0()
  {
    fwCtx->listeners.emitPluginChanged(PluginEvent(PluginEvent::STARTING, this->q_func()));

    try {
      pluginLoader.load();
      if (!pluginLoader.isLoaded())
      {
        throw PluginException(QString("Loading plugin %1 failed: %2").arg(pluginLoader.fileName()).arg(pluginLoader.errorString()),
                              PluginException::ACTIVATOR_ERROR);
      }

      pluginActivator = qobject_cast<PluginActivator*>(pluginLoader.instance());
      if (!pluginActivator)
      {
        throw PluginException(QString("Creating PluginActivator instance from %1 failed: %2").arg(pluginLoader.fileName()).arg(pluginLoader.errorString()),
                              PluginException::ACTIVATOR_ERROR);
      }

      pluginActivator->start(pluginContext);

      if (Plugin::UNINSTALLED == state)
      {
        throw PluginException("Plugin uninstalled during start()", PluginException::STATECHANGE_ERROR);
      }
      state = Plugin::ACTIVE;
    }
    catch (const std::exception& e)
    {
      throw PluginException("Plugin start failed", PluginException::ACTIVATOR_ERROR, e);
    }

    qDebug() << "activating #" << id << "completed.";

    //10:
    fwCtx->listeners.emitPluginChanged(PluginEvent(PluginEvent::STARTED, this->q_func()));
  }

  void PluginPrivate::removePluginResources()
  {
    // automatic disconnect due to Qt signal slot
    //fwCtx->listeners.removeAllListeners(this);

    // TODO
//    Set srs = fwCtx.services.getRegisteredByBundle(this);
//    for (Iterator i = srs.iterator(); i.hasNext();) {
//      try {
//        ((ServiceRegistration)i.next()).unregister();
//      } catch (IllegalStateException ignore) {
//        // Someone has unregistered the service after stop completed.
//        // This should not occur, but we don't want get stuck in
//        // an illegal state so we catch it.
//      }
//    }
//    Set s = fwCtx.services.getUsedByBundle(this);
//    for (Iterator i = s.iterator(); i.hasNext(); ) {
//      ((ServiceRegistrationImpl) i.next()).reference.ungetService(this, false);
//    }
  }

}
