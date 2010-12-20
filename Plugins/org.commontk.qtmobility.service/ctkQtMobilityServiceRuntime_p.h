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


#ifndef CTKQTMOBILITYSERVICERUNTIME_P_H
#define CTKQTMOBILITYSERVICERUNTIME_P_H

#include <QObject>
#include <QServiceManager>

#include <ctkPluginEvent.h>
#include <ctkPlugin.h>
#include <ctkServiceRegistration.h>

class ctkPluginContext;
class ctkQtMobilityServiceFactory;

using namespace QtMobility;

class ctkQtMobilityServiceRuntime : public QObject
{
  Q_OBJECT

public:

  ctkQtMobilityServiceRuntime(ctkPluginContext* pc);

  void start();
  void stop();

  QServiceManager& getQServiceManager();

protected slots:

  void pluginChanged(const ctkPluginEvent& pe);

private:

  QtMobility::QServiceManager qServiceManager;

  QMultiHash<QSharedPointer<ctkPlugin>, ctkQtMobilityServiceFactory*> mapPluginToServiceFactory;
  QMultiHash<QSharedPointer<ctkPlugin>, ctkServiceRegistration> mapPluginToServiceRegistration;
  QSet<QSharedPointer<ctkPlugin> > lazy;

  ctkPluginContext* pc;

  /**
   * Check if the plugin has a service descriptor and register it.
   *
   * @param plugin Plugin to check
   */
  void processPlugin(QSharedPointer<ctkPlugin> plugin);

  /**
   * Remove service resources for plugin
   *
   * @param p Plugin to check
   */
  void removePlugin(QSharedPointer<ctkPlugin> plugin);

  static QString getQServiceManagerErrorString(QServiceManager::Error error);
};

#endif // CTKQTMOBILITYSERVICERUNTIME_P_H
