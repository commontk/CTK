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


#ifndef CTKSERVICETRACKERTESTSUITE_P_H
#define CTKSERVICETRACKERTESTSUITE_P_H

#include <QObject>
#include <QThread>

#include <ctkTestSuiteInterface.h>
#include <ctkServiceEvent.h>

class ctkPlugin;
class ctkPluginContext;

class ctkServiceTrackerTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:
    ctkServiceTrackerTestSuite(ctkPluginContext* pc);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    // test functions

    // Checks that the correct service events
    // are sent to a registered service listener.
    // Case where the plugin does not unregister its
    // service in the stop()-method.
    void runTest();

Q_SIGNALS:

    void serviceControl(int service, const QString operation, long rank);

private:

    ctkPluginContext* pc;
    QSharedPointer<ctkPlugin> p;

    QSharedPointer<ctkPlugin> pS;

};

class ctkServiceTrackerTestWorker : public QThread
{
  Q_OBJECT

public:

  ctkServiceTrackerTestWorker(ctkPluginContext* pc);

  bool waitSuccess;

protected:

  void run();

private:

  ctkPluginContext* pc;

};


#endif // CTKSERVICETRACKERTESTSUITE_P_H
