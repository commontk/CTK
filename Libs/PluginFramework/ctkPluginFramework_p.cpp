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

#include "ctkPluginConstants.h"
#include "ctkPluginContext.h"
#include "ctkPluginContext_p.h"
#include "ctkPluginFramework.h"
#include "ctkPluginFrameworkLauncher.h"

#include "ctkLocationManager_p.h"
#include "ctkPluginFramework_p.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginFrameworkUtil_p.h"
#include "ctkPluginFrameworkDebugOptions_p.h"

#include "ctkBasicLocation_p.h"

#include <QtConcurrentRun>

//----------------------------------------------------------------------------
ctkPluginFrameworkPrivate::ctkPluginFrameworkPrivate(QWeakPointer<ctkPlugin> qq, ctkPluginFrameworkContext* fw)
  : ctkPluginPrivate(qq, fw, 0, ctkPluginConstants::SYSTEM_PLUGIN_LOCATION,
                     ctkPluginConstants::SYSTEM_PLUGIN_SYMBOLICNAME,
                     // TODO: read version from the manifest resource
                     ctkVersion(0, 9, 0)),
    shuttingDown(0)
{
  systemHeaders.insert(ctkPluginConstants::PLUGIN_SYMBOLICNAME, symbolicName);
  systemHeaders.insert(ctkPluginConstants::PLUGIN_NAME, location);
  systemHeaders.insert(ctkPluginConstants::PLUGIN_VERSION, version.toString());
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::init()
{
  this->state = ctkPlugin::STARTING;
  this->fwCtx->init();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::initSystemPlugin()
{
  this->pluginContext.reset(new ctkPluginContext(this));
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::activate(ctkPluginContext* context)
{
  ctkProperties locationProperties;
  ctkBasicLocation* location = ctkLocationManager::getUserLocation();
  if (location != NULL)
  {
    locationProperties["type"] =  ctkPluginFrameworkLauncher::PROP_USER_AREA;
    registrations.push_back(context->registerService<ctkLocation>(location, locationProperties));
  }
  location = ctkLocationManager::getInstanceLocation();
  if (location != NULL)
  {
    locationProperties["type"] = ctkPluginFrameworkLauncher::PROP_INSTANCE_AREA;
    registrations.push_back(context->registerService<ctkLocation>(location, locationProperties));
  }
  location = ctkLocationManager::getConfigurationLocation();
  if (location != NULL)
  {
    locationProperties["type"] = ctkPluginFrameworkLauncher::PROP_CONFIG_AREA;
    registrations.push_back(context->registerService<ctkLocation>(location, locationProperties));
  }
  location = ctkLocationManager::getInstallLocation();
  if (location != NULL)
  {
    locationProperties["type"] = ctkPluginFrameworkLauncher::PROP_INSTALL_AREA;
    registrations.push_back(context->registerService<ctkLocation>(location, locationProperties));
  }

  location = ctkLocationManager::getCTKHomeLocation();
  if (location != NULL)
  {
    locationProperties["type"] = ctkPluginFrameworkLauncher::PROP_HOME_LOCATION_AREA;
    registrations.push_back(context->registerService<ctkLocation>(location, locationProperties));
  }

  ctkPluginFrameworkDebugOptions* dbgOptions = ctkPluginFrameworkDebugOptions::getDefault();
  dbgOptions->start(context);
  context->registerService<ctkDebugOptions>(dbgOptions);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::deactivate(ctkPluginContext* /*context*/)
{
  foreach(ctkServiceRegistration registration, registrations)
  {
    if (registration)
    {
      try
      {
        registration.unregister();
      }
      catch (const ctkIllegalStateException&) {}
    }
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::uninitSystemPlugin()
{
  this->pluginContext->d_func()->invalidate();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::shutdown(bool restart)
{
  Locker sync(&lock);

  bool wasActive = false;
  switch (state)
  {
  case ctkPlugin::INSTALLED:
  case ctkPlugin::RESOLVED:
    shutdownDone_unlocked(false);
    break;
  case ctkPlugin::ACTIVE:
    wasActive = true;
    // Fall through
  case ctkPlugin::STARTING:
    if (shuttingDown.fetchAndAddOrdered(0) == 0)
    {
      try
      {
        const bool wa = wasActive;
        shuttingDown.fetchAndStoreOrdered(1);
        QFuture<void> future = QtConcurrent::run([=]() { shutdown0(restart, wa); });
      }
      catch (const ctkException& e)
      {
        systemShuttingdownDone(ctkPluginFrameworkEvent(ctkPluginFrameworkEvent::PLUGIN_ERROR,
                                                       this->q_func(), e));
      }
    }
    break;
  case ctkPlugin::STOPPING:
    // Shutdown already inprogress, fall through
  case ctkPlugin::UNINSTALLED:
    break;
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::shutdown0(bool restart, bool wasActive)
{
  try
  {
    {
      Locker sync(&lock);
      waitOnOperation(&lock, QString("Framework::") + (restart ? "update" : "stop"), true);
      operation = DEACTIVATING;
      state = ctkPlugin::STOPPING;
    }

    fwCtx->listeners.emitPluginChanged(
        ctkPluginEvent(ctkPluginEvent::STOPPING, this->q_func()));

    if (wasActive)
    {
      stopAllPlugins();
      deactivate(this->pluginContext.data());
    }

    {
      Locker sync(&lock);
      fwCtx->uninit();
      shuttingDown.fetchAndStoreOrdered(0);
      shutdownDone_unlocked(restart);
    }

    if (restart)
    {
      if (wasActive)
      {
        q_func().toStrongRef()->start();
      }
      else
      {
        init();
      }
    }
  }
  catch (const ctkException& e)
  {
    shuttingDown.fetchAndStoreOrdered(0);
    systemShuttingdownDone(ctkPluginFrameworkEvent(ctkPluginFrameworkEvent::PLUGIN_ERROR,
                                                   this->q_func(), e));
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::shutdownDone_unlocked(bool restart)
{
  ctkPluginFrameworkEvent::Type t = restart ? ctkPluginFrameworkEvent::FRAMEWORK_STOPPED_UPDATE : ctkPluginFrameworkEvent::FRAMEWORK_STOPPED;
  systemShuttingdownDone_unlocked(ctkPluginFrameworkEvent(t, this->q_func()));
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::stopAllPlugins()
{
  // TODO start level
//  if (fwCtx.startLevelController != null)
//  {
//    fwCtx.startLevelController.shutdown();
//  }

  // Stop all active plug-ins, in reverse plug-in ID order
  // The list will be empty when the start level service is in use.
  QList<QSharedPointer<ctkPlugin> > activePlugins = fwCtx->plugins->getActivePlugins();
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
  std::sort(activePlugins.begin(), activePlugins.end(), pluginIdLessThan);
#else
  qSort(activePlugins.begin(), activePlugins.end(), pluginIdLessThan);
#endif
  QListIterator<QSharedPointer<ctkPlugin> > i(activePlugins);
  i.toBack();
  while(i.hasPrevious())
  {
    QSharedPointer<ctkPlugin> p = i.previous();
    try
    {
      if (p->getState() & (ctkPlugin::ACTIVE | ctkPlugin::STARTING))
      {
        // Stop plugin without changing its autostart setting.
        p->stop(ctkPlugin::STOP_TRANSIENT);
      }
    }
    catch (const ctkPluginException& pe)
    {
      fwCtx->listeners.frameworkError(p, pe);
    }
  }

  QList<QSharedPointer<ctkPlugin> > allPlugins = fwCtx->plugins->getPlugins();

  // Set state to INSTALLED and purge any unrefreshed bundles
  foreach (QSharedPointer<ctkPlugin> p, allPlugins)
  {
    if (p->getPluginId() != 0)
    {
      p->d_func()->setStateInstalled(false);
      p->d_func()->purge();
    }
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::systemShuttingdownDone(const ctkPluginFrameworkEvent& fe)
{
  Locker sync(&lock);
  systemShuttingdownDone_unlocked(fe);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPrivate::systemShuttingdownDone_unlocked(const ctkPluginFrameworkEvent& fe)
{

  if (state != ctkPlugin::INSTALLED)
  {
    state = ctkPlugin::RESOLVED;
    operation.fetchAndStoreOrdered(IDLE);
    lock.wakeAll();
  }
  stopEvent.isNull = fe.isNull();
  stopEvent.type = fe.getType();
}
