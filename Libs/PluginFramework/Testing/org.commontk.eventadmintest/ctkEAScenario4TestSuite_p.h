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


#ifndef CTKEASCENARIO4TESTSUITE_P_H
#define CTKEASCENARIO4TESTSUITE_P_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include <ctkException.h>
#include <service/event/ctkEventHandler.h>
#include <ctkTestSuiteInterface.h>
#include <ctkServiceRegistration.h>

struct ctkEventAdmin;

class ctkEAScenario4EventConsumer : public QObject, public ctkEventHandler
{
  Q_OBJECT
  Q_INTERFACES(ctkEventHandler)

private:

  ctkPluginContext* context;

  /** class variable for service registration */
  ctkServiceRegistration serviceRegistration;

  /** class variable indicating the topics correct version */
  QStringList topicsToConsume;

  /** class variable indicating the topics */
  QString filterToConsume;

  /** class variable keeping number of asynchronous message */
  int asynchMessages;

  /** class variable keeping number of synchronous message */
  int synchMessages;

  /**
   * class variable indication the number of synchronous messages to be
   * received
   */
  int numSyncMessages;

  /**
   * class variable indication the number of asynchronous messages to be
   * received
   */
  int numAsyncMessages;

  bool error;
  ctkRuntimeException exc;

  QMutex mutex;

public:

  /**
   * Constructor creates a consumer service
   *
   * @param bundleContext
   * @param topics
   * @param numSyncMsg
   *            number of synchronous messages to be received
   * @param numAsyncMsg
   *            number of asynchronous messages to be received
   * @param name
   *            name of the Consumer
   * @param id
   */
  ctkEAScenario4EventConsumer(ctkPluginContext* pluginContext, const QStringList topics,
                       int numSyncMsg, int numAsyncMsg, const QString& filter);

  void runTest();

  void cleanup();

  /**
   * This method takes events from the event admin service.
   */
  void handleEvent(const ctkEvent& event);
};

class ctkEAScenario4EventPublisher : public QObject
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

  /** variable holding the topic to use */
  QString topicToSend;

  /** variable holding the parameters to use */
  ctkDictionary propertiesToSend;

  QThread thread;

protected Q_SLOTS:

  void sendEvents();
  void postEvents();

public:

  ctkEAScenario4EventPublisher(ctkPluginContext* context, const QString& name,
                               const QString& topic, const ctkDictionary& props,
                               int id, int numOfMessage);

  void runTest();
};

/**
 * Test suite for testing the requirements specified in the test specification
 * for the EventAdmin service.
 *
 * Check the filter both faulty and correctly, also check if the events are
 * received in the order that they were sent both synchronously and
 * asynchronously.
 */
class ctkEAScenario4TestSuite : public QObject, public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

private:

  /** plugin context variable */
  ctkPluginContext* pluginContext;

  long eventPluginId;

  QList<ctkEAScenario4EventConsumer*> eventConsumers;
  QList<ctkEAScenario4EventPublisher*> eventPublishers;

public:

  /**
   * Constructor for the TestSuite class.
   *
   * @param context
   *            the handle to the frame work
   */
  ctkEAScenario4TestSuite(ctkPluginContext* context, long eventPluginId);

private Q_SLOTS:

  void initTestCase();
  void cleanupTestCase();

  void testRegisterConsumer();
  void testPublishEvents();

};

#endif // CTKEASCENARIO4TESTSUITE_P_H
