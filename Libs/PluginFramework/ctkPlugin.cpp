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

#include "ctkPlugin.h"

#include "ctkPluginContext.h"
#include "ctkPluginFrameworkUtil_p.h"
#include "ctkPluginPrivate_p.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkServices_p.h"

#include <QStringList>


ctkPlugin::ctkPlugin()
: d_ptr(0)
{

}

void ctkPlugin::init(ctkPluginPrivate* dd)
{
  if (d_ptr) throw std::logic_error("ctkPlugin already initialized");
  d_ptr = dd;
}

void ctkPlugin::init(const QWeakPointer<ctkPlugin>& self,
                     ctkPluginFrameworkContext* fw,
                     ctkPluginArchive* pa)
{
  if (d_ptr) throw std::logic_error("ctkPlugin already initialized");
  d_ptr = new ctkPluginPrivate(self, fw, pa);
}

ctkPlugin::~ctkPlugin()
{
  delete d_ptr;
}

ctkPlugin::State ctkPlugin::getState() const
{
  Q_D(const ctkPlugin);
  return d->state;
}

void ctkPlugin::start(const StartOptions& options)
{
  Q_D(ctkPlugin);

  if (d->state == UNINSTALLED)
  {
    throw std::logic_error("ctkPlugin is uninstalled");
  }

  // Initialize the activation; checks initialization of lazy
  // activation.

  //TODO 1: If activating or deactivating, wait a litle
  // we don't use mutliple threads to start plugins for now
  //waitOnActivation(lock, "ctkPlugin::start", false);

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
  if ((options & START_ACTIVATION_POLICY) && !d->eagerActivation )
  {
    if (STARTING == d->state) return;
    d->state = STARTING;
    d->pluginContext.reset(new ctkPluginContext(this->d_func()));
    ctkPluginEvent pluginEvent(ctkPluginEvent::LAZY_ACTIVATION, d->q_ptr);
    d->fwCtx->listeners.emitPluginChanged(pluginEvent);
  }
  else
  {
    d->finalizeActivation();
  }
}

void ctkPlugin::stop(const StopOptions& options)
{
  Q_D(ctkPlugin);

  const std::exception* savedException = 0;

  //1:
  if (d->state == UNINSTALLED)
  {
    throw std::logic_error("Plugin is uninstalled");
  }

  //2: If activating or deactivating, wait a litle
  // We don't support threaded start/stop methods, so we don't need to wait
  //waitOnActivation(fwCtx.packages, "Plugin::stop", false);

  //3:
  if ((options & STOP_TRANSIENT) == 0)
  {
    d->ignoreAutostartSetting();
  }
  bool wasStarted = false;

  switch (d->state)
  {
  case INSTALLED:
  case RESOLVED:
  case STOPPING:
  case UNINSTALLED:
    //4:
    return;

  case ACTIVE:
    wasStarted = true;
  case STARTING: // Lazy start...
    try
    {
      d->stop0(wasStarted);
    }
    catch (const std::exception* exc)
    {
      savedException = exc;
    }
    break;
  };

  if (savedException)
  {
    if (const ctkPluginException* pluginExc = dynamic_cast<const ctkPluginException*>(savedException))
    {
      throw pluginExc;
    }
    else
    {
      throw dynamic_cast<const std::logic_error*>(savedException);
    }
  }
}

void ctkPlugin::uninstall()
{
  bool wasResolved = false;

  Q_D(ctkPlugin);
  if (d->archive)
  {
    try
    {
      d->archive->setStartLevel(-2); // Mark as uninstalled
    }
    catch (...)
    { }
  }

  d->cachedHeaders = getHeaders();

  switch (d->state)
  {
  case UNINSTALLED:
    throw std::logic_error("Plugin is in UNINSTALLED state");

  case STARTING: // Lazy start
  case ACTIVE:
  case STOPPING:
    try
    {
      //TODO: If activating or deactivating, wait a litle
      // we don't use mutliple threads to start plugins for now
      //d->waitOnActivation(fwCtx.packages, "Bundle.uninstall", false);
      if (d->state & (ACTIVE | STARTING))
      {
        try
        {
          d->stop0(d->state == ACTIVE);
        }
        catch (const std::exception& exception)
        {
          // NYI! not call inside lock
          d->fwCtx->listeners.frameworkError(this, exception);
        }
      }
    }
    catch (const std::exception& e)
    {
      d->deactivating = false;
      //fwCtx.packages.notifyAll();
      d->fwCtx->listeners.frameworkError(this, e);
    }
    // Fall through
  case RESOLVED:
    wasResolved = true;
    // Fall through
  case INSTALLED:
    d->fwCtx->plugins->remove(d->location);
    d->pluginActivator = 0;

    if (d->pluginDir.exists())
    {
      if (!ctkPluginFrameworkUtil::removeDir(d->pluginDir.absolutePath()))
      {
        // Plugin dir is not deleted completely, make sure we mark
        // it as uninstalled for next framework restart
        if (d->archive)
        {
          try
          {
            d->archive->setStartLevel(-2); // Mark as uninstalled
          }
          catch (const std::exception& e)
          {
            // NYI! Generate FrameworkError if dir still exists!?
            qDebug() << "Failed to mark plugin" <<  d->id
                     << "as uninstalled," << d->pluginDir.absoluteFilePath()
                     << "must be deleted manually:" << e.what();
          }
        }
      }
      d->pluginDir.setFile("");
    }
    if (d->archive)
    {
      d->archive->purge();
    }

    // id, location and headers survive after uninstall.
    // TODO: UNRESOLVED must be sent out during installed state
    // This needs to be reviewed. See OSGi bug #1374
    d->state = INSTALLED;
    d->modified();

    // Broadcast events
    if (wasResolved)
    {
      d->fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::UNRESOLVED, d->q_ptr));
    }

    d->state = UNINSTALLED;
    d->fwCtx->listeners.emitPluginChanged(ctkPluginEvent(ctkPluginEvent::UNINSTALLED, d->q_ptr));

    break;
  }
}

ctkPluginContext* ctkPlugin::getPluginContext() const
{
  //TODO security checks
  Q_D(const ctkPlugin);
  return d->pluginContext.data();
}

long ctkPlugin::getPluginId() const
{
  Q_D(const ctkPlugin);
  return d->id;
}

QString ctkPlugin::getLocation() const
{
  //TODO security
  Q_D(const ctkPlugin);
  return d->location;
}

QHash<QString, QString> ctkPlugin::getHeaders()
{
  //TODO security
  Q_D(ctkPlugin);
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

QString ctkPlugin::getSymbolicName() const
{
  Q_D(const ctkPlugin);
  return d->symbolicName;
}

QStringList ctkPlugin::getResourceList(const QString& path) const
{
  Q_D(const ctkPlugin);
  return d->archive->findResourcesPath(path);
}

QByteArray ctkPlugin::getResource(const QString& path) const
{
  Q_D(const ctkPlugin);
  return d->archive->getPluginResource(path);
}

ctkVersion ctkPlugin::getVersion() const
{
  Q_D(const ctkPlugin);
  return d->version;
}

QDebug operator<<(QDebug debug, ctkPlugin::State state)
{
  switch (state)
  {
  case ctkPlugin::UNINSTALLED:
    return debug << "UNINSTALLED";
  case ctkPlugin::INSTALLED:
    return debug << "INSTALLED";
  case ctkPlugin::RESOLVED:
    return debug << "RESOLVED";
  case ctkPlugin::STARTING:
    return debug << "STARTING";
  case ctkPlugin::STOPPING:
    return debug << "STOPPING";
  case ctkPlugin::ACTIVE:
    return debug << "ACTIVE";
  default:
    return debug << "unknown";
  }
}

QDebug operator<<(QDebug debug, const ctkPlugin& plugin)
{
  debug.nospace() << "ctkPlugin[" << "id=" << plugin.getPluginId() <<
      ", state=" << plugin.getState() << ", loc=" << plugin.getLocation() <<
      ", symName=" << plugin.getSymbolicName() << "]";
  return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, ctkPlugin const * plugin)
{
  return operator<<(debug, *plugin);
}
