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


#include "ctkConfigurationAdminFactory_p.h"

#include <service/log/ctkLogService.h>

#include "ctkConfigurationAdminImpl_p.h"
#include "ctkConfigurationImpl_p.h"

ctkConfigurationAdminFactory::ctkConfigurationAdminFactory(ctkPluginContext* context, ctkLogService* log)
  : eventDispatcher(context, log), pluginManager(context), logService(log),
    configurationStore(this, context),
    managedServiceTracker(this, &configurationStore, context),
    managedServiceFactoryTracker(this, &configurationStore, context)
{

}

ctkConfigurationAdminFactory::~ctkConfigurationAdminFactory()
{
  qDeleteAll(configAdmins);
}

void ctkConfigurationAdminFactory::start()
{
  eventDispatcher.start();
  pluginManager.start();
  managedServiceTracker.open();
  managedServiceFactoryTracker.open();
}

void ctkConfigurationAdminFactory::stop()
{
  managedServiceTracker.close();
  managedServiceFactoryTracker.close();
  eventDispatcher.stop();
  pluginManager.stop();
}

QObject* ctkConfigurationAdminFactory::getService(QSharedPointer<ctkPlugin> plugin,
                                                  ctkServiceRegistration registration)
{
  ctkServiceReference reference = registration.getReference();
  eventDispatcher.setServiceReference(reference);
  ctkConfigurationAdminImpl* configAdmin = new ctkConfigurationAdminImpl(this, &configurationStore, plugin);
  configAdmins.push_back(configAdmin);
  return configAdmin;
}

void ctkConfigurationAdminFactory::ungetService(QSharedPointer<ctkPlugin> plugin,
                                                ctkServiceRegistration registration, QObject* service)
{
  Q_UNUSED(plugin)
  Q_UNUSED(registration)
  Q_UNUSED(service)
  // do nothing
}

void ctkConfigurationAdminFactory::pluginChanged(const ctkPluginEvent& event)
{
  if (event.getType() == ctkPluginEvent::UNINSTALLED)
  {
    configurationStore.unbindConfigurations(event.getPlugin());
  }
}

void ctkConfigurationAdminFactory::checkConfigurationPermission()
{
  //TODO security
//  SecurityManager sm = System.getSecurityManager();
//  if (sm != null)
//    sm.checkPermission(configurationPermission);
}

ctkLogService* ctkConfigurationAdminFactory::getLogService() const
{
  return logService;
}

void ctkConfigurationAdminFactory::dispatchEvent(ctkConfigurationEvent::Type type, const QString& factoryPid, const QString& pid)
{
  eventDispatcher.dispatchEvent(type, factoryPid, pid);
}

void ctkConfigurationAdminFactory::notifyConfigurationUpdated(ctkConfigurationImpl* config, bool isFactory)
{
  if (isFactory)
  {
    managedServiceFactoryTracker.notifyUpdated(config);
  }
  else
  {
    managedServiceTracker.notifyUpdated(config);
  }
}

void ctkConfigurationAdminFactory::notifyConfigurationDeleted(ctkConfigurationImpl* config, bool isFactory)
{
  if (isFactory)
  {
    managedServiceFactoryTracker.notifyDeleted(config);
  }
  else
  {
    managedServiceTracker.notifyDeleted(config);
  }
}

void ctkConfigurationAdminFactory::modifyConfiguration(const ctkServiceReference& reference,
                                                       ctkDictionary& properties)
{
  pluginManager.modifyConfiguration(reference, properties);
}
