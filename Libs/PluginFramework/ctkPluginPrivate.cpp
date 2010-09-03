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


  const ctkPlugin::States ctkPluginPrivate::RESOLVED_FLAGS = ctkPlugin::RESOLVED | ctkPlugin::STARTING | ctkPlugin::ACTIVE | ctkPlugin::STOPPING;


  ctkPluginPrivate::ctkPluginPrivate(
      ctkPlugin& qq,
      ctkPluginFrameworkContext* fw,
      ctkPluginArchive* pa)
    : q_ptr(&qq), fwCtx(fw), id(pa->getPluginId()),
    location(pa->getPluginLocation().toString()), state(ctkPlugin::INSTALLED),
    archive(pa), pluginContext(0), pluginActivator(0), pluginLoader(pa->getLibLocation()),
    lastModified(0), eagerActivation(false), activating(false), deactivating(false)
  {
    //TODO
    //checkCertificates(pa);

    checkManifestHeaders();

    // fill require list
    QString requireString = archive->getAttribute(ctkPluginConstants::REQUIRE_PLUGIN);
    QList<QMap<QString, QStringList> > requireList = ctkPluginFrameworkUtil::parseEntries(ctkPluginConstants::REQUIRE_PLUGIN,
                                                                         requireString, true, true, false);
    QListIterator<QMap<QString, QStringList> > i(requireList);
    while (i.hasNext())
    {
      const QMap<QString, QStringList>& e = i.next();
      const QStringList& res = e.value(ctkPluginConstants::RESOLUTION_DIRECTIVE);
      const QStringList& version = e.value(ctkPluginConstants::PLUGIN_VERSION_ATTRIBUTE);
      ctkRequirePlugin* rp = new ctkRequirePlugin(this, e.value("$key").front(),
                                            res.empty() ? QString() : res.front(),
                                            version.empty() ? QString() : version.front());
      require.push_back(rp);
    }
  }

  ctkPluginPrivate::ctkPluginPrivate(ctkPlugin& qq,
    ctkPluginFrameworkContext* fw,
    long id, const QString& loc, const QString& sym, const ctkVersion& ver)
      : q_ptr(&qq), fwCtx(fw), id(id), location(loc), symbolicName(sym), version(ver),
      state(ctkPlugin::INSTALLED), archive(0), pluginContext(0),
      pluginActivator(0), lastModified(0),
      eagerActivation(false), activating(false), deactivating(false)
  {

  }

  ctkPluginPrivate::~ctkPluginPrivate()
  {
    qDeleteAll(require);
  }

  ctkPlugin::State ctkPluginPrivate::getUpdatedState()
  {
    if (state & ctkPlugin::INSTALLED)
    {
      try
      {
        if (state == ctkPlugin::INSTALLED)
        {
          fwCtx->resolvePlugin(this);
          state = ctkPlugin::RESOLVED;
          fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::RESOLVED, this->q_func()));
        }

      }
      catch (const ctkPluginException& pe)
      {
        Q_Q(ctkPlugin);
        this->fwCtx->listeners.frameworkError(q, pe);
        throw;
      }
    }

    return state;
  }

  void ctkPluginPrivate::setAutostartSetting(const ctkPlugin::StartOptions& setting) {
    try
    {
      if (archive)
      {
        archive->setAutostartSetting(setting);
      }
    }
    catch (const ctkPluginDatabaseException& e)
    {
      Q_Q(ctkPlugin);
      this->fwCtx->listeners.frameworkError(q, e);
    }
  }

  void ctkPluginPrivate::checkManifestHeaders()
  {
    symbolicName = archive->getAttribute(ctkPluginConstants::PLUGIN_SYMBOLICNAME);

    if (symbolicName.isEmpty())
    {
      throw std::invalid_argument(std::string("ctkPlugin has no symbolic name, location=") +
                                           qPrintable(location));
    }

    QString mpv = archive->getAttribute(ctkPluginConstants::PLUGIN_VERSION);
    if (!mpv.isEmpty())
    {
      try
      {
        version = ctkVersion(mpv);
      }
      catch (const std::exception& e)
      {
        throw std::invalid_argument(std::string("ctkPlugin does not specify a valid ") +
                                    qPrintable(ctkPluginConstants::PLUGIN_VERSION) + " header. Got exception: " + e.what());
      }
    }

    QString ap = archive->getAttribute(ctkPluginConstants::PLUGIN_ACTIVATIONPOLICY);
    if (ctkPluginConstants::ACTIVATION_EAGER == ap)
    {
      eagerActivation = true;
    }

  }

  void ctkPluginPrivate::finalizeActivation()
  {
    switch (getUpdatedState())
    {
      case ctkPlugin::INSTALLED:
        // we shouldn't be here, getUpdatedState should have thrown
        // an exception during resolving the plugin
        throw ctkPluginException("Internal error: expected exception on plugin resolve not thrown!");
      case ctkPlugin::STARTING:
        if (activating) return; // finalization already in progress.
        // Lazy activation; fall through to RESOLVED.
      case ctkPlugin::RESOLVED:
        //6:
        state = ctkPlugin::STARTING;
        activating = true;
        qDebug() << "activating #" << this->id;
        //7:
        if (!pluginContext)
        {
          pluginContext = new ctkPluginContext(this);
        }
        try
        {
          //TODO maybe call this in its own thread
          start0();
        }
        catch (...)
        {
          //8:
          state = ctkPlugin::STOPPING;
          // NYI, call outside lock
          fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STOPPING, this->q_func()));
          removePluginResources();
          delete pluginContext;

          state = ctkPlugin::RESOLVED;
          // NYI, call outside lock
          fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STOPPED, this->q_func()));
          activating = false;
          throw;
        }
        activating = false;
        break;
      case ctkPlugin::ACTIVE:
        break;
      case ctkPlugin::STOPPING:
        // This happens if start is called from inside the ctkPluginActivator::stop method.
        // Don't allow it.
        throw ctkPluginException("start called from ctkPluginActivator::stop",
                              ctkPluginException::ACTIVATOR_ERROR);
      case ctkPlugin::UNINSTALLED:
        throw std::logic_error("ctkPlugin is in UNINSTALLED state");
      }
  }

  void ctkPluginPrivate::start0()
  {
    fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STARTING, this->q_func()));

    try {
      pluginLoader.load();
      if (!pluginLoader.isLoaded())
      {
        throw ctkPluginException(QString("Loading plugin %1 failed: %2").arg(pluginLoader.fileName()).arg(pluginLoader.errorString()),
                              ctkPluginException::ACTIVATOR_ERROR);
      }

      pluginActivator = qobject_cast<ctkPluginActivator*>(pluginLoader.instance());
      if (!pluginActivator)
      {
        throw ctkPluginException(QString("Creating ctkPluginActivator instance from %1 failed: %2").arg(pluginLoader.fileName()).arg(pluginLoader.errorString()),
                              ctkPluginException::ACTIVATOR_ERROR);
      }

      pluginActivator->start(pluginContext);

      if (ctkPlugin::UNINSTALLED == state)
      {
        throw ctkPluginException("ctkPlugin uninstalled during start()", ctkPluginException::STATECHANGE_ERROR);
      }
      state = ctkPlugin::ACTIVE;
    }
    catch (const std::exception& e)
    {
      throw ctkPluginException("ctkPlugin start failed", ctkPluginException::ACTIVATOR_ERROR, e);
    }

    qDebug() << "activating #" << id << "completed.";

    //10:
    fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::STARTED, this->q_func()));
  }

  void ctkPluginPrivate::removePluginResources()
  {
    // automatic disconnect due to Qt signal slot
    //fwCtx->listeners.removeAllListeners(this);

    // TODO
//    Set srs = fwCtx.services.getRegisteredByBundle(this);
//    for (Iterator i = srs.iterator(); i.hasNext();) {
//      try {
//        ((ctkServiceRegistration)i.next()).unregister();
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
