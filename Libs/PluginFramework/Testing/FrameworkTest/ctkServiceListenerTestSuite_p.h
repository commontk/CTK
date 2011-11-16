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


#ifndef CTKSERVICELISTENERTESTSUITE_P_H
#define CTKSERVICELISTENERTESTSUITE_P_H

#include <QObject>

#include <ctkTestSuiteInterface.h>
#include <ctkServiceEvent.h>

class ctkPlugin;
class ctkPluginContext;

class ctkServiceListenerTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:
    ctkServiceListenerTestSuite(ctkPluginContext* pc);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    // test functions

    // Checks that the correct service events
    // are sent to a registered service listener.
    // Case where the plugin does not unregister its
    // service in the stop()-method.
    void frameSL05a();

    // Checks that the correct service events
    // are sent to a registered service listener.
    // Case where the plugin unregisters its service
    // in the stop()-method.
    void frameSL10a();

//    void frameSL15a();
//    void frameSL20a();
    void frameSL25a();

private:

    ctkPluginContext* pc;
    QSharedPointer<ctkPlugin> p;

    QSharedPointer<ctkPlugin> pA;
    QSharedPointer<ctkPlugin> pA2;
    QSharedPointer<ctkPlugin> pSL1;
    QSharedPointer<ctkPlugin> pSL3;
    QSharedPointer<ctkPlugin> pSL4;

    bool runStartStopTest(
      const QString& tcName, int cnt, QSharedPointer<ctkPlugin> targetPlugin,
      const QList<ctkServiceEvent::Type>& events);
};

class ctkServiceListener : public QObject
{
  Q_OBJECT

private:

  friend class ctkServiceListenerTestSuite;

  const bool checkUsingPlugins;
  QList<ctkServiceEvent> events;

  bool teststatus;

  ctkPluginContext* pc;

public:

  ctkServiceListener(ctkPluginContext* pc, bool checkUsingPlugins = true);

  void clearEvents();

  bool checkEvents(const QList<ctkServiceEvent::Type>& eventTypes);

protected Q_SLOTS:

  void serviceChanged(const ctkServiceEvent& evt);

private:

  void printUsingPlugins(const ctkServiceReference& sr, const QString& caption);

  // Print expected and actual service events.
  void dumpEvents(const QList<ctkServiceEvent::Type>& eventTypes);

}; // end of class ctkServiceListener



#endif // CTKSERVICELISTENERTESTSUITE_P_H
