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


#ifndef CTKCONFIGURATIONSTORE_P_H
#define CTKCONFIGURATIONSTORE_P_H

#include <ctkLDAPSearchFilter.h>

#include "ctkConfigurationImpl_p.h"

#include <QSharedPointer>
#include <QHash>
#include <QDir>
#include <QMutex>

class ctkConfigurationImpl;
class ctkConfigurationAdminFactory;
class ctkPluginContext;
class ctkPlugin;

/**
 * ctkConfigurationStore manages all active configurations along with persistence. The current
 * implementation uses a filestore and serialization of the configuration dictionaries to files
 * identified by their pid. Persistence details are in the constructor, saveConfiguration, and
 * deleteConfiguration and can be factored out separately if required.
 */
class ctkConfigurationStore
{

public:

  ctkConfigurationStore(ctkConfigurationAdminFactory* configurationAdminFactory,
                        ctkPluginContext* context);

  void saveConfiguration(const QString& pid, ctkConfigurationImpl* config);
  void removeConfiguration(const QString& pid);

  ctkConfigurationImplPtr getConfiguration(const QString& pid, const QString& location);

  ctkConfigurationImplPtr createFactoryConfiguration(
    const QString& factoryPid, const QString& location);

  ctkConfigurationImplPtr findConfiguration(const QString& pid);

  QList<ctkConfigurationImplPtr> getFactoryConfigurations(const QString& factoryPid);
  QList<ctkConfigurationImplPtr> listConfigurations(const ctkLDAPSearchFilter& filter);

  void unbindConfigurations(QSharedPointer<ctkPlugin> plugin);

private:

  QMutex mutex;
  ctkConfigurationAdminFactory* configurationAdminFactory;
  static const QString STORE_DIR; // = "store"
  static const QString PID_EXT; // = ".pid"
  QHash<QString, ctkConfigurationImplPtr> configurations;
  int createdPidCount;
  QDir store;


  void writeConfigurationFile(QFile& configFile,
                              const ctkDictionary& configProperties);
  void deleteConfigurationFile(QFile& configFile);

};

#endif // CTKCONFIGURATIONSTORE_P_H
