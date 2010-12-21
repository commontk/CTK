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


#ifndef CTKMANAGEDSERVICEFACTORYTRACKER_P_H
#define CTKMANAGEDSERVICEFACTORYTRACKER_P_H

#include <ctkServiceTracker.h>
#include <service/cm/ctkManagedServiceFactory.h>

#include "ctkCMSerializedTaskQueue_p.h"

class ctkConfigurationAdminFactory;
class ctkConfigurationStore;
class ctkConfigurationImpl;

/**
 * ctkManagedServiceFactoryTracker tracks... ManagedServiceFactory(s) and notifies them about related configuration changes
 */
class ctkManagedServiceFactoryTracker : public ctkServiceTracker<ctkManagedServiceFactory*>
{

public:

  ctkManagedServiceFactoryTracker(ctkConfigurationAdminFactory* configurationAdminFactory,
                                  ctkConfigurationStore* configurationStore,
                                  ctkPluginContext* context);

  ctkManagedServiceFactory* addingService(const ctkServiceReference& reference);
  void modifiedService(const ctkServiceReference& reference, ctkManagedServiceFactory* service);
  void removedService(const ctkServiceReference& reference, ctkManagedServiceFactory* service);

  void notifyDeleted(ctkConfigurationImpl* config);
  void notifyUpdated(ctkConfigurationImpl* config);

private:

  ctkPluginContext* context;
  ctkConfigurationAdminFactory* configurationAdminFactory;
  QMutex configurationStoreMutex;
  ctkConfigurationStore* configurationStore;

  // managedServiceFactoryMutex guards both managedServiceFactories and managedServiceFactoryReferences
  mutable QMutex managedServiceFactoryMutex;
  QHash<QString, ctkManagedServiceFactory*> managedServiceFactories;
  QHash<QString, ctkServiceReference> managedServiceFactoryReferences;

  ctkCMSerializedTaskQueue queue;

  void addManagedServiceFactory(const ctkServiceReference& reference,
                                const QString& factoryPid,
                                ctkManagedServiceFactory* service);

  void removeManagedServiceFactory(const ctkServiceReference& reference,
                                   const QString& factoryPid);

  bool trackManagedServiceFactory(const QString& factoryPid,
                                  const ctkServiceReference& reference,
                                  ctkManagedServiceFactory* service);

  void untrackManagedServiceFactory(const QString& factoryPid,
                                    const ctkServiceReference& reference);

  ctkManagedServiceFactory* getManagedServiceFactory(const QString& factoryPid) const;

  ctkServiceReference getManagedServiceFactoryReference(const QString& factoryPid) const;

  QString getPidForManagedServiceFactory(ctkManagedServiceFactory* service) const;

  void asynchDeleted(ctkManagedServiceFactory* service, const QString& pid);

  void asynchUpdated(ctkManagedServiceFactory* service, const QString& pid,
                     const ctkDictionary& properties);
};

#endif // CTKMANAGEDSERVICEFACTORYTRACKER_P_H
