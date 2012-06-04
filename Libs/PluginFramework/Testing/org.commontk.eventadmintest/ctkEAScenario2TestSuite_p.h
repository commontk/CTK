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


#ifndef CTKEASCENARIO2TESTSUITE_P_H
#define CTKEASCENARIO2TESTSUITE_P_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include <service/event/ctkEventHandler.h>
#include <ctkTestSuiteInterface.h>
#include <ctkServiceRegistration.h>
#include <ctkException.h>

struct ctkEventAdmin;

class ctkEAScenario2EventConsumer : public QObject, public ctkEventHandler
{
  Q_OBJECT
  Q_INTERFACES(ctkEventHandler)

private:

  ctkPluginContext* context;

  /** class variable for service registration */
  ctkServiceRegistration serviceRegistration;

  /** class variable indicating the topics */
  QStringList topicsToConsume;

  /** class variable keeping number of asynchronus message */
  int asynchMessages;

  /** class variable keeping number of synchronus message */
  int synchMessages;

  /** class variable indication the number of synchronous messages to be received */
  int numSyncMessages;

  /** class variable indication the number of asynchronous messages to be received */
  int numAsyncMessages;

  bool error;
  ctkRuntimeException exc;

  QMutex mutex;

public:

  /**
   * Constructor creates a consumer service
   */
  ctkEAScenario2EventConsumer(ctkPluginContext* pluginContext, const QStringList& topics,
                              int numSyncMsg, int numAsyncMsg);

  void runTest();

  void cleanup();

  /**
   * This method takes events from the event admin service.
   */
  void handleEvent(const ctkEvent& event);
};

class ctkEAScenario2EventPublisher : public QObject
{
  Q_OBJECT

private:

  /** A reference to a service */
  ctkServiceReference serviceReference;

  /** The admin which delivers the events */
  ctkEventAdmin* eventAdmin;

  /** class variable holding plugin context */
  ctkPluginContext* context;

  /** variable holding messages to send */
  int messageTosend;

  /** variable holding the topic to send */
  QStringList topicsToSend;

  QThread thread;

public:

  ctkEAScenario2EventPublisher(ctkPluginContext* context,
                               const QString& name, const QStringList topics,
                               int id, int numOfMessage);

  void runTest();

protected Q_SLOTS:

  void sendEvents();
  void postEvents();
};


/**
 * Test suite for testing the requirements specified in the test specification
 * for the EventAdmin service.
 *
 * Check the Topic sorting of events, both synchronous and asynchronous.
 *
 */
class ctkEAScenario2TestSuite : public QObject, public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

private:

  /** plugin context variable */
  ctkPluginContext* pluginContext;

  long eventPluginId;

  QList<ctkEAScenario2EventConsumer*> eventConsumers;
  ctkEAScenario2EventPublisher* eventPublisher;

public:

  /**
   * Constructor for the TestSuite class.
   *
   * @param context the handle to the frame work
   * @param eventPluginId The id of the plugin implementing the EventAdmin spec
   */
  ctkEAScenario2TestSuite(ctkPluginContext* context, long eventPluginId);

private Q_SLOTS:

  void initTestCase();
  void cleanupTestCase();

  void testRegisterConsumer();
  void testPublishEvents();

};

#endif // CTKEASCENARIO2TESTSUITE_P_H
