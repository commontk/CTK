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


#include "ctkConfigurationAdminImpl_p.h"

#include "ctkConfigurationAdminFactory_p.h"
#include "ctkConfigurationStore_p.h"

ctkConfigurationAdminImpl::ctkConfigurationAdminImpl(ctkConfigurationAdminFactory* configurationAdminFactory,
                          ctkConfigurationStore* configurationStore,
                          QSharedPointer<ctkPlugin> plugin)
  : configurationAdminFactory(configurationAdminFactory),
    plugin(plugin), configurationStore(configurationStore)
{

}

ctkConfigurationPtr ctkConfigurationAdminImpl::createFactoryConfiguration(const QString& factoryPid)
{
  checkPID(factoryPid);
  return configurationStore->createFactoryConfiguration(factoryPid, plugin->getLocation());
}

ctkConfigurationPtr ctkConfigurationAdminImpl::createFactoryConfiguration(const QString& factoryPid,
                                               const QString& location)
{
  checkPID(factoryPid);
  configurationAdminFactory->checkConfigurationPermission();
  return configurationStore->createFactoryConfiguration(factoryPid, location);
}

ctkConfigurationPtr ctkConfigurationAdminImpl::getConfiguration(const QString& pid)
{
  checkPID(pid);
  ctkConfigurationImplPtr config = configurationStore->getConfiguration(pid, plugin->getLocation());
  if (!config->getPluginLocation(false).isEmpty() && config->getPluginLocation(false) != plugin->getLocation())
  {
    configurationAdminFactory->checkConfigurationPermission();
  }
  config->bind(plugin);
  return config;
}

ctkConfigurationPtr ctkConfigurationAdminImpl::getConfiguration(const QString& pid, const QString& location)
{
  checkPID(pid);
  configurationAdminFactory->checkConfigurationPermission();
  return configurationStore->getConfiguration(pid, location);
}

QList<ctkConfigurationPtr> ctkConfigurationAdminImpl::listConfigurations(const QString& filter)
{
  QString filterString = filter;
  if (filterString.isEmpty())
  {
    filterString = QString("(%1=*)").arg(ctkPluginConstants::SERVICE_PID);
  }

  //TODO security
  //try
  //{
    configurationAdminFactory->checkConfigurationPermission();
  //}
  //catch (SecurityException e) {
  //  filterString = "(&(" + ConfigurationAdmin.SERVICE_BUNDLELOCATION + "=" + bundle.getLocation() + ")" + filterString + ")";
  //}
  QList<ctkConfigurationImplPtr> configs = configurationStore->listConfigurations(ctkLDAPSearchFilter(filterString));
  QList<ctkConfigurationPtr> result;
  foreach(ctkConfigurationImplPtr config, configs)
  {
    result.push_back(config);
  }
  return result;
}

void ctkConfigurationAdminImpl::checkPID(const QString& pid)
{
  if (pid.isEmpty())
  {
    throw ctkInvalidArgumentException("PID cannot be empty");
  }
}
