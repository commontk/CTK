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


#include "ctkManagedServiceFactoryTracker_p.h"

#include <service/cm/ctkConfigurationException.h>
#include <service/log/ctkLogService.h>

#include "ctkConfigurationAdminActivator_p.h"
#include "ctkConfigurationAdminFactory_p.h"
#include "ctkConfigurationStore_p.h"
#include "ctkConfigurationImpl_p.h"

#include <QRunnable>

ctkManagedServiceFactoryTracker::ctkManagedServiceFactoryTracker(
  ctkConfigurationAdminFactory* configurationAdminFactory,
  ctkConfigurationStore* configurationStore,
  ctkPluginContext* context)
  : ctkServiceTracker<ctkManagedServiceFactory*>(context),
    context(context),
    configurationAdminFactory(configurationAdminFactory),
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    configurationStoreMutex(),
#else
    configurationStoreMutex(QMutex::Recursive),
#endif
    configurationStore(configurationStore),
    queue("ctkManagedServiceFactory Update Queue")
{

}

ctkManagedServiceFactory* ctkManagedServiceFactoryTracker::addingService(const ctkServiceReference& reference)
{
  QString factoryPid = reference.getProperty(ctkPluginConstants::SERVICE_PID).toString();
  if (factoryPid.isEmpty())
    return 0;

  ctkManagedServiceFactory* service = context->getService<ctkManagedServiceFactory>(reference);
  if (service == 0)
    return 0;

  {
    QMutexLocker lock(&configurationStoreMutex);
    addManagedServiceFactory(reference, factoryPid, service);
  }
  return service;
}

void ctkManagedServiceFactoryTracker::modifiedService(const ctkServiceReference& reference,
                                                      ctkManagedServiceFactory* service)
{
  QString factoryPid = reference.getProperty(ctkPluginConstants::SERVICE_PID).toString();
  {
    QMutexLocker lock(&configurationStoreMutex);
    if (getManagedServiceFactory(factoryPid) == service)
      return;
    QString previousPid = getPidForManagedServiceFactory(service);
    removeManagedServiceFactory(reference, previousPid);
    addingService(reference);
  }
}

void ctkManagedServiceFactoryTracker::removedService(const ctkServiceReference& reference,
                                                     ctkManagedServiceFactory* service)
{
  Q_UNUSED(service)

  QString factoryPid = reference.getProperty(ctkPluginConstants::SERVICE_PID).toString();
  {
    QMutexLocker lock(&configurationStoreMutex);
    removeManagedServiceFactory(reference, factoryPid);
  }
  context->ungetService(reference);
}

void ctkManagedServiceFactoryTracker::notifyDeleted(ctkConfigurationImpl* config)
{
  config->checkLocked();
  QString factoryPid = config->getFactoryPid(false);
  ctkServiceReference reference = getManagedServiceFactoryReference(factoryPid);
  if (reference && config->bind(reference.getPlugin()))
  {
    asynchDeleted(getManagedServiceFactory(factoryPid), config->getPid(false));
  }
}

void ctkManagedServiceFactoryTracker::notifyUpdated(ctkConfigurationImpl* config)
{
  config->checkLocked();
  QString factoryPid = config->getFactoryPid();
  ctkServiceReference reference = getManagedServiceFactoryReference(factoryPid);
  if (reference && config->bind(reference.getPlugin()))
  {
    ctkDictionary properties = config->getProperties();
    configurationAdminFactory->modifyConfiguration(reference, properties);
    asynchUpdated(getManagedServiceFactory(factoryPid), config->getPid(), properties);
  }
}

void ctkManagedServiceFactoryTracker::addManagedServiceFactory(
  const ctkServiceReference& reference, const QString& factoryPid,
  ctkManagedServiceFactory* service)
{
  QList<ctkConfigurationImplPtr> configs = configurationStore->getFactoryConfigurations(factoryPid);
  ctkConfigurationImplLocker lock(configs);

  if (trackManagedServiceFactory(factoryPid, reference, service))
  {
    foreach (ctkConfigurationImplPtr config, configs)
    {
      if (config->isDeleted())
      {
        // ignore this config
      }
      else if (config->bind(reference.getPlugin()))
      {
        ctkDictionary properties = config->getProperties();
        configurationAdminFactory->modifyConfiguration(reference, properties);
        asynchUpdated(service, config->getPid(), properties);
      }
      else
      {
        CTK_WARN(configurationAdminFactory->getLogService())
            << "Configuration for " << ctkPluginConstants::SERVICE_PID << "="
            << config->getPid() << " could not be bound to "
            << reference.getPlugin()->getLocation();
      }
    }
  }
}

void ctkManagedServiceFactoryTracker::removeManagedServiceFactory(
  const ctkServiceReference& reference, const QString& factoryPid)
{
  QList<ctkConfigurationImplPtr> configs = configurationStore->getFactoryConfigurations(factoryPid);
  ctkConfigurationImplLocker lock(configs);
  untrackManagedServiceFactory(factoryPid, reference);
}

bool ctkManagedServiceFactoryTracker::trackManagedServiceFactory(const QString& factoryPid,
                                                                 const ctkServiceReference& reference,
                                                                 ctkManagedServiceFactory* service)
{
  QMutexLocker lock(&managedServiceFactoryMutex);
  if (managedServiceFactoryReferences.contains(factoryPid))
  {
    CTK_WARN(configurationAdminFactory->getLogService())
        << "ctkManagedServiceFactory already registered for "
        << ctkPluginConstants::SERVICE_PID << "=" << factoryPid;
    return false;
  }
  managedServiceFactoryReferences.insert(factoryPid, reference);
  managedServiceFactories.insert(factoryPid, service);
  return true;
}

void ctkManagedServiceFactoryTracker::untrackManagedServiceFactory(const QString& factoryPid,
                                                                   const ctkServiceReference& reference)
{
  Q_UNUSED(reference)

  QMutexLocker lock(&managedServiceFactoryMutex);
  managedServiceFactoryReferences.remove(factoryPid);
  managedServiceFactories.remove(factoryPid);
}

ctkManagedServiceFactory* ctkManagedServiceFactoryTracker::getManagedServiceFactory(const QString& factoryPid) const
{
  QMutexLocker lock(&managedServiceFactoryMutex);
  return managedServiceFactories.value(factoryPid);
}

ctkServiceReference ctkManagedServiceFactoryTracker::getManagedServiceFactoryReference(const QString& factoryPid) const
{
  QMutexLocker lock(&managedServiceFactoryMutex);
  return managedServiceFactoryReferences.value(factoryPid);
}

QString ctkManagedServiceFactoryTracker::getPidForManagedServiceFactory(ctkManagedServiceFactory* service) const
{
  QMutexLocker lock(&managedServiceFactoryMutex);
  QHash<QString, ctkManagedServiceFactory*>::ConstIterator end = managedServiceFactories.end();
  QHash<QString, ctkManagedServiceFactory*>::ConstIterator it;
  for (it = managedServiceFactories.begin(); it != end; ++it)
  {
    if (it.value() == service)
      return it.key();
  }
  return QString();
}

class _AsynchDeleteRunnable : public QRunnable
{
public:

  _AsynchDeleteRunnable(ctkManagedServiceFactory* service, const QString& pid,
                        ctkLogService* log)
    : service(service), pid(pid), log(log)
  {

  }

  void run()
  {
    try
    {
      service->deleted(pid);
    }
    catch (const std::exception* e)
    {
      CTK_ERROR_EXC(log, e);
    }
  }

private:

  ctkManagedServiceFactory* const service;
  const QString pid;
  ctkLogService* const log;
};

void ctkManagedServiceFactoryTracker::asynchDeleted(ctkManagedServiceFactory* service, const QString& pid)
{
  queue.put(new _AsynchDeleteRunnable(service, pid, configurationAdminFactory->getLogService()));
}

class _AsynchFactoryUpdateRunnable : public QRunnable
{
public:

  _AsynchFactoryUpdateRunnable(ctkManagedServiceFactory* service, const QString& pid,
                               const ctkDictionary& properties, ctkLogService* log)
    : service(service), pid(pid), properties(properties), log(log)
  {

  }

  void run()
  {
    try
    {
      service->updated(pid, properties);
    }
    catch (const ctkConfigurationException* e)
    {
      // we might consider doing more for ConfigurationExceptions
      CTK_ERROR_EXC(log, e);
    }
    catch (const std::exception* e)
    {
      CTK_ERROR_EXC(log, e);
    }
  }

private:

  ctkManagedServiceFactory* const service;
  const QString pid;
  const ctkDictionary properties;
  ctkLogService* const log;
};

void ctkManagedServiceFactoryTracker::asynchUpdated(ctkManagedServiceFactory* service, const QString& pid,
                                                    const ctkDictionary& properties)
{
  queue.put(new _AsynchFactoryUpdateRunnable(service, pid, properties, configurationAdminFactory->getLogService()));
}
