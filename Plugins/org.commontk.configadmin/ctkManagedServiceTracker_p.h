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


#ifndef CTKMANAGEDSERVICETRACKER_P_H
#define CTKMANAGEDSERVICETRACKER_P_H

#include <ctkServiceTracker.h>
#include <service/cm/ctkManagedService.h>

#include "ctkCMSerializedTaskQueue_p.h"

class ctkConfigurationAdminFactory;
class ctkConfigurationStore;
class ctkConfigurationImpl;

/**
 * ctkManagedServiceTracker tracks... ManagedServices and notifies them about related configuration changes
 */
class ctkManagedServiceTracker : public ctkServiceTracker<ctkManagedService*>
{

public:

  ctkManagedServiceTracker(ctkConfigurationAdminFactory* configurationAdminFactory,
                           ctkConfigurationStore* configurationStore,
                           ctkPluginContext* context);

  ctkManagedService* addingService(const ctkServiceReference& reference);
  void modifiedService(const ctkServiceReference& reference, ctkManagedService* service);
  void removedService(const ctkServiceReference& reference, ctkManagedService* service);

  void notifyDeleted(ctkConfigurationImpl* config);
  void notifyUpdated(ctkConfigurationImpl* config);

private:

  ctkPluginContext* context;
  ctkConfigurationAdminFactory* configurationAdminFactory;
  QMutex configurationStoreMutex;
  ctkConfigurationStore* configurationStore;

  // managedServiceMutex guards both managedServices and managedServiceReferences
  mutable QMutex managedServiceMutex;
  QHash<QString, ctkManagedService*> managedServices;
  QHash<QString, ctkServiceReference> managedServiceReferences;

  ctkCMSerializedTaskQueue queue;

  void addManagedService(const ctkServiceReference& reference, const QString& pid,
                         ctkManagedService* service);

  void removeManagedService(const ctkServiceReference& reference, const QString& pid);

  bool trackManagedService(const QString& pid, const ctkServiceReference& reference,
                           ctkManagedService* service);

  void untrackManagedService(const QString& pid, const ctkServiceReference& reference);

  ctkManagedService* getManagedService(const QString& pid) const;

  ctkServiceReference getManagedServiceReference(const QString& pid) const;

  QString getPidForManagedService(ctkManagedService* service) const;

  void asynchUpdated(ctkManagedService* service, const ctkDictionary& properties);
};

#endif // CTKMANAGEDSERVICETRACKER_P_H
