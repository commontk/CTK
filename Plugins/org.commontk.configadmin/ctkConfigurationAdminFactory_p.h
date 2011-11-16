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


#ifndef CTKCONFIGURATIONADMINFACTORY_P_H
#define CTKCONFIGURATIONADMINFACTORY_P_H

#include <QObject>

#include <ctkPluginEvent.h>
#include <ctkServiceFactory.h>
#include <service/cm/ctkConfigurationListener.h>

#include "ctkCMEventDispatcher_p.h"
#include "ctkCMPluginManager_p.h"
#include "ctkConfigurationStore_p.h"
#include "ctkManagedServiceTracker_p.h"
#include "ctkManagedServiceFactoryTracker_p.h"

struct ctkLogService;
class ctkConfigurationAdminImpl;
class ctkConfigurationImpl;

/**
 * ctkConfigurationAdminFactory provides a Configuration Admin ServiceFactory but more significantly
 * launches the whole implementation.
 */
class ctkConfigurationAdminFactory : public QObject, public ctkServiceFactory
{
  Q_OBJECT
  Q_INTERFACES(ctkServiceFactory)

public:

  ctkConfigurationAdminFactory(ctkPluginContext* context, ctkLogService* log);
  ~ctkConfigurationAdminFactory();

  void start();
  void stop();

  QObject* getService(QSharedPointer<ctkPlugin> plugin, ctkServiceRegistration registration);
  void ungetService(QSharedPointer<ctkPlugin> plugin, ctkServiceRegistration registration, QObject* service);

  void checkConfigurationPermission();

  ctkLogService* getLogService() const;

  void dispatchEvent(ctkConfigurationEvent::Type type, const QString& factoryPid, const QString& pid);

  void notifyConfigurationUpdated(ctkConfigurationImpl* config, bool isFactory);
  void notifyConfigurationDeleted(ctkConfigurationImpl* config, bool isFactory);

  void modifyConfiguration(const ctkServiceReference& reference, ctkDictionary& properties);

public Q_SLOTS:

  void pluginChanged(const ctkPluginEvent& event);

private:

  //Permission configurationPermission = new ConfigurationPermission("*", ConfigurationPermission.CONFIGURE); //$NON-NLS-1$
  ctkCMEventDispatcher eventDispatcher;
  ctkCMPluginManager pluginManager;
  ctkLogService* logService;
  ctkConfigurationStore configurationStore;
  ctkManagedServiceTracker managedServiceTracker;
  ctkManagedServiceFactoryTracker managedServiceFactoryTracker;

  QList<ctkConfigurationAdminImpl*> configAdmins;
};

#endif // CTKCONFIGURATIONADMINFACTORY_P_H
