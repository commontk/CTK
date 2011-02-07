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

#ifndef CTKCONFIGURATIONEVENTADAPTER_P_H
#define CTKCONFIGURATIONEVENTADAPTER_P_H

#include <QObject>

#include <ctkServiceTracker.h>
#include <service/cm/ctkConfigurationListener.h>
#include <service/event/ctkEventAdmin.h>


class ctkConfigurationEventAdapter : public QObject, public ctkConfigurationListener
{
  Q_OBJECT
  Q_INTERFACES(ctkConfigurationListener)

public:

  // constants for Event topic substring
  static const QString TOPIC; // = "org/commontk/service/cm/ConfigurationEvent"
  static const QChar TOPIC_SEPARATOR; // = '/'
  // constants for Event types
  static const QString CM_UPDATED; // = "CM_UPDATED"
  static const QString CM_DELETED; // = "CM_DELETED"
  // constants for Event properties
  static const QString CM_FACTORY_PID; // = "cm.factoryPid"
  static const QString CM_PID; // = "cm.pid"
  static const QString SERVICE; // = "service"
  static const QString SERVICE_ID; // = "service.id"
  static const QString SERVICE_OBJECTCLASS; // = "service.objectClass"
  static const QString SERVICE_PID; // = "service.pid"

private:

  ctkPluginContext* const context;
  ctkServiceRegistration configListenerRegistration;
  ctkServiceTracker<ctkEventAdmin*> eventAdminTracker;

public:

  ctkConfigurationEventAdapter(ctkPluginContext* context);

  void start();
  void stop();

   void configurationEvent(const ctkConfigurationEvent& event);

  void putServiceReferenceProperties(ctkDictionary& properties, const ctkServiceReference& ref);
};

#endif // CTKCONFIGURATIONEVENTADAPTER_P_H
