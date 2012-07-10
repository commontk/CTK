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


#ifndef CTKEASCENARIO1TESTSUITE_P_H
#define CTKEASCENARIO1TESTSUITE_P_H

#include <QObject>
#include <QThread>

#include <service/event/ctkEventHandler.h>
#include <ctkTestSuiteInterface.h>
#include <ctkServiceRegistration.h>
#include <ctkException.h>

class ctkPluginContext;
struct ctkEventAdmin;

/**
 * This class consumes events
 */
class ctkEAScenario1EventConsumer : public QObject, public ctkEventHandler
{
  Q_OBJECT
  Q_INTERFACES(ctkEventHandler)

private:

  ctkPluginContext* context;

  ctkServiceReference eventAdminRef;
  ctkEventAdmin* eventAdmin;
  qlonglong slotId;

  /** class variable for service registration */
  ctkServiceRegistration serviceRegistration;

  /** class variable indicating the topics */
  QStringList topicsToConsume;

  /** class variable keeping number of asynchronus messages */
  int numOfasynchMessages;

  /** class variable keeping number of synchronus messages */
  int numOfsynchMessages;

  /** class variable holding the old syncronus message nummber */
  int synchMessageExpectedNumber;

  /** class variable holding the old asyncronus message nummber */
  int asynchMessageExpectedNumber;

  const int messagesSent;

  bool error;
  ctkRuntimeException exc;

  bool useSignalSlot;

public:

  /**
   * Constructor creates a consumer service
   */
  ctkEAScenario1EventConsumer(ctkPluginContext* pluginContext, const QStringList& topics,
                              int messagesSent, bool useSignalSlot);

  /**
   * run the test
   */
  void runTest();
  void cleanup();
  void reset();

public Q_SLOTS:

  /**
   * This method takes events from the event admin service.
   */
  void handleEvent(const ctkEvent& event);

};

/**
 * Class publish events
 */
class ctkEAScenario1EventPublisher : public QObject
{
  Q_OBJECT

private:

  /** A reference to a service */
  ctkServiceReference serviceReference;

  /** The admin which delivers the events */
  ctkEventAdmin* eventAdmin;

  /** class variable holding bundle context */
  ctkPluginContext* context;

  /** variable holding messages to send */
  int messageTosend;

  QThread thread;

  bool useSignalSlot;

public:

  ctkEAScenario1EventPublisher(ctkPluginContext* context, const QString& name,
                               int id, int numOfMessage, bool useSignalSlot);

  void runTest();

protected Q_SLOTS:

  void sendEvents();
  void postEvents();

Q_SIGNALS:

  void syncSignalEvent(const ctkDictionary&);
  void asyncSignalEvent(const ctkDictionary&);
};

/**
 * Test sute for testing the requirements specified in the test
 * specification for the EventAdmin service. It will ensure that the
 * events arrive in time and order it will even check that the
 * wildcards according to the topics works properly.
 *
 * @author Magnus Klack
 */
class ctkEAScenario1TestSuite : public QObject, public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

private:

  /** plugin context variable */
  ctkPluginContext* pluginContext;
  /** the messages to be deliverd */
  const int MESSAGES_SENT;
  long eventPluginId;
  ctkServiceReference reference;

  ctkEAScenario1EventConsumer* eventConsumer;
  ctkEAScenario1EventPublisher* eventPublisher;

  bool useSignalSlot;

public:

  /**
   * Constructor for the TestSuite class.
   *
   * @param context The handle to the framework
   * @param eventPluginId The id of the plugin implementing the EventAdmin spec
   */
  ctkEAScenario1TestSuite(ctkPluginContext* context, long eventPluginId, bool useSignalSlot);

private Q_SLOTS:

  void initTestCase();
  void cleanupTestCase();

  void testRegisterConsumer();
  void testPublishEvents();

};

#endif // CTKEASCENARIO1TESTSUITE_P_H
