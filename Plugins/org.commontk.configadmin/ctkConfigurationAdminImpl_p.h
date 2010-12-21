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


#ifndef CTKCONFIGURATIONADMINIMPL_P_H
#define CTKCONFIGURATIONADMINIMPL_P_H

#include <service/cm/ctkConfigurationAdmin.h>

#include <ctkPlugin.h>

class ctkConfigurationAdminFactory;
class ctkConfigurationStore;

/**
 * ctkConfigurationAdminImpl provides the ConfigurationAdmin service implementation
 */
class ctkConfigurationAdminImpl : public QObject, public ctkConfigurationAdmin
{
  Q_OBJECT
  Q_INTERFACES(ctkConfigurationAdmin)

public:

  ctkConfigurationAdminImpl(ctkConfigurationAdminFactory* configurationAdminFactory,
                            ctkConfigurationStore* configurationStore,
                            QSharedPointer<ctkPlugin> plugin);

  ctkConfigurationPtr createFactoryConfiguration(const QString& factoryPid);

  ctkConfigurationPtr createFactoryConfiguration(const QString& factoryPid,
                                                 const QString& location);

  ctkConfigurationPtr getConfiguration(const QString& pid);

  ctkConfigurationPtr getConfiguration(const QString& pid, const QString& location);

  QList<ctkConfigurationPtr> listConfigurations(const QString& filterString = QString());

private:

  ctkConfigurationAdminFactory* const configurationAdminFactory;
  const QSharedPointer<ctkPlugin> plugin;
  ctkConfigurationStore* const configurationStore;

  void checkPID(const QString& pid);
};

#endif // CTKCONFIGURATIONADMINIMPL_P_H
