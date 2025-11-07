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


#include "ctkManagedServiceTracker_p.h"

#include <service/cm/ctkConfigurationException.h>
#include <service/log/ctkLogService.h>

#include "ctkConfigurationImpl_p.h"
#include "ctkConfigurationAdminFactory_p.h"

#include <QRunnable>

ctkManagedServiceTracker::ctkManagedServiceTracker(ctkConfigurationAdminFactory* configurationAdminFactory,
                         ctkConfigurationStore* configurationStore,
                         ctkPluginContext* context)
  : ctkServiceTracker<ctkManagedService*>(context),
    context(context),
    configurationAdminFactory(configurationAdminFactory),
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    configurationStoreMutex(),
#else
    configurationStoreMutex(QMutex::Recursive),
#endif
    configurationStore(configurationStore),
    queue("ctkManagedService Update Queue")
{

}

ctkManagedService* ctkManagedServiceTracker::addingService(const ctkServiceReference& reference)
{
  QString pid = reference.getProperty(ctkPluginConstants::SERVICE_PID).toString();
  if (pid.isEmpty())
    return 0;

  ctkManagedService* service = context->getService<ctkManagedService>(reference);
  if (service == 0)
    return 0;

  {
    QMutexLocker lock(&configurationStoreMutex);
    addManagedService(reference, pid, service);
  }
  return service;
}

void ctkManagedServiceTracker::modifiedService(const ctkServiceReference& reference, ctkManagedService* service)
{
  QString pid = reference.getProperty(ctkPluginConstants::SERVICE_PID).toString();
  {
    QMutexLocker lock(&configurationStoreMutex);
    if (getManagedService(pid) == service)
      return;
    QString previousPid = getPidForManagedService(service);
    removeManagedService(reference, previousPid);
    addingService(reference);
  }
}

void ctkManagedServiceTracker::removedService(const ctkServiceReference& reference, ctkManagedService* service)
{
  Q_UNUSED(service)

  QString pid = reference.getProperty(ctkPluginConstants::SERVICE_PID).toString();
  {
    QMutexLocker lock(&configurationStoreMutex);
    removeManagedService(reference, pid);
  }
  context->ungetService(reference);
}

void ctkManagedServiceTracker::notifyDeleted(ctkConfigurationImpl* config)
{
  config->checkLocked();
  QString pid = config->getPid(false);
  ctkServiceReference reference = getManagedServiceReference(pid);
  if (reference && config->bind(reference.getPlugin()))
    asynchUpdated(getManagedService(pid), ctkDictionary());
}

void ctkManagedServiceTracker::notifyUpdated(ctkConfigurationImpl* config) {
  config->checkLocked();
  QString pid = config->getPid();
  ctkServiceReference reference = getManagedServiceReference(pid);
  if (reference && config->bind(reference.getPlugin()))
  {
    ctkDictionary properties = config->getProperties();
    configurationAdminFactory->modifyConfiguration(reference, properties);
    asynchUpdated(getManagedService(pid), properties);
  }
}

void ctkManagedServiceTracker::addManagedService(const ctkServiceReference& reference,
                                                 const QString& pid,
                                                 ctkManagedService* service)
{
  ctkConfigurationImplPtr config = configurationStore->findConfiguration(pid);
  if (config.isNull() && trackManagedService(pid, reference, service))
  {
    asynchUpdated(service, ctkDictionary());
  }
  else
  {
    ctkConfigurationImplLocker lock(config.data());
    if (trackManagedService(pid, reference, service))
    {
      if (!config->getFactoryPid().isEmpty())
      {
        CTK_WARN(configurationAdminFactory->getLogService())
            << "Configuration for " << ctkPluginConstants::SERVICE_PID << "="
            << pid << " should only be used by a ctkManagedServiceFactory";
      }
      else if (config->isDeleted())
      {
        asynchUpdated(service, ctkDictionary());
      }
      else if (config->bind(reference.getPlugin()))
      {
        ctkDictionary properties = config->getProperties();
        configurationAdminFactory->modifyConfiguration(reference, properties);
        asynchUpdated(service, properties);
      }
      else
      {
        CTK_WARN(configurationAdminFactory->getLogService())
            << "Configuration for " << ctkPluginConstants::SERVICE_PID << "="
            << pid << " could not be bound to " << reference.getPlugin()->getLocation();
      }
    }
  }
}

void ctkManagedServiceTracker::removeManagedService(const ctkServiceReference& reference,
                                                    const QString& pid)
{
  ctkConfigurationImplPtr config = configurationStore->findConfiguration(pid);
  if (config.isNull())
  {
    untrackManagedService(pid, reference);
  }
  else
  {
    ctkConfigurationImplLocker lock(config.data());
    untrackManagedService(pid, reference);
  }
}

bool ctkManagedServiceTracker::trackManagedService(const QString& pid, const ctkServiceReference& reference,
                         ctkManagedService* service)
{
  QMutexLocker lock(&managedServiceMutex);
  if (managedServiceReferences.contains(pid))
  {
    CTK_WARN(configurationAdminFactory->getLogService())
        << "ctkManagedService already registered for " << ctkPluginConstants::SERVICE_PID
        << "=" << pid;
    return false;
  }
  managedServiceReferences.insert(pid, reference);
  managedServices.insert(pid, service);
  return true;
}

void ctkManagedServiceTracker::untrackManagedService(const QString& pid, const ctkServiceReference& reference)
{
  Q_UNUSED(reference)

  QMutexLocker lock(&managedServiceMutex);
  managedServiceReferences.remove(pid);
  managedServices.remove(pid);
}

ctkManagedService* ctkManagedServiceTracker::getManagedService(const QString& pid) const
{
  QMutexLocker lock(&managedServiceMutex);
  return managedServices.value(pid);
}

ctkServiceReference ctkManagedServiceTracker::getManagedServiceReference(const QString& pid) const
{
  QMutexLocker lock(&managedServiceMutex);
  return managedServiceReferences.value(pid);
}

QString ctkManagedServiceTracker::getPidForManagedService(ctkManagedService* service) const
{
  QMutexLocker lock(&managedServiceMutex);
  QHash<QString, ctkManagedService*>::ConstIterator end = managedServices.end();
  QHash<QString, ctkManagedService*>::ConstIterator it;
  for (it = managedServices.begin(); it != end; ++it)
  {
    if (it.value() == service)
      return it.key();
  }
  return QString();
}

class _AsynchUpdateRunnable : public QRunnable
{

public:

  _AsynchUpdateRunnable(ctkManagedService* service,
                        const ctkDictionary& properties,
                        ctkLogService* log)
    : service(service), properties(properties), log(log)
  {
  }

  void run()
  {
    try
    {
      service->updated(properties);
    }
    catch (const ctkConfigurationException* e)
    {
      // we might consider doing more for ctkConfigurationExceptions
      CTK_ERROR_EXC(log, e);
    }
    catch (const std::exception* e)
    {
      CTK_ERROR_EXC(log, e);
    }
  }

private:

  ctkManagedService* service;
  const ctkDictionary properties;
  ctkLogService * const log;
};

void ctkManagedServiceTracker::asynchUpdated(ctkManagedService* service, const ctkDictionary& properties)
{
  queue.put(new _AsynchUpdateRunnable(service, properties, configurationAdminFactory->getLogService()));
}
