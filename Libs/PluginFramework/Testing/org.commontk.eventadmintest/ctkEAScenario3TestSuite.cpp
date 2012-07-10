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


#include "ctkEAScenario3TestSuite_p.h"

#include <ctkPluginContext.h>
#include <service/event/ctkEventConstants.h>
#include <service/event/ctkEventAdmin.h>

#include <QTest>

//----------------------------------------------------------------------------
ctkEAScenario3EventConsumer::ctkEAScenario3EventConsumer(
  ctkPluginContext* pluginContext, const QStringList topics,
  int numSyncMsg, int numAsyncMsg)
  : context(pluginContext), topicsToConsume(topics), asynchMessages(0), synchMessages(0),
    numSyncMessages(numSyncMsg), numAsyncMessages(numAsyncMsg), error(false), exc("")
{

}

//----------------------------------------------------------------------------
void ctkEAScenario3EventConsumer::runTest()
{
  asynchMessages = 0;
  synchMessages = 0;
  /* create the hashtable to put properties in */
  ctkDictionary props;

  /* put service.pid property in hashtable */
  props.insert(ctkEventConstants::EVENT_TOPIC, topicsToConsume);

  /* register the service */
  serviceRegistration = context->registerService<ctkEventHandler>(this, props);

  QVERIFY2(serviceRegistration, "service registration should not be null");
}

//----------------------------------------------------------------------------
void ctkEAScenario3EventConsumer::cleanup()
{
  QMutexLocker l(&mutex);
  try
  {
    serviceRegistration.unregister();
  }
  catch (const ctkIllegalStateException&) {}

  if (error)
  {
    throw exc;
  }

  QCOMPARE(synchMessages, numSyncMessages); // "Not all synch messages recieved"
  QCOMPARE(asynchMessages, numAsyncMessages); // "Not all asynch messages recieved"
}

//----------------------------------------------------------------------------
void ctkEAScenario3EventConsumer::handleEvent(const ctkEvent& event)
{
  QMutexLocker l(&mutex);
  try
  {
    /* try to get the message */
    QString message = event.getProperty("Synchronous message").toString();

    if (!message.isNull())
    {
      /* its an asyncronous message */
      synchMessages++;
      qDebug() << "received a Synchronous event with message:" << message;
    }
    else
    {
      message = event.getProperty("Asynchronous message").toString();
      if (!message.isNull())
      {
        asynchMessages++;
        qDebug() << "received an Asynchronus event with message:" << message;
      }
    }

    /* assert that the messages property is not null */
    QVERIFY2(!message.isNull(), "Message should not be null in handleEvent()");

    /* assert that the messages of syncronous type are not too many */
    QVERIFY2(synchMessages < numSyncMessages + 1, "too many synchronous messages");
    /* assert that the messsage of the asyncronous type are not too many */
    QVERIFY2(asynchMessages < numAsyncMessages + 1, "too many asynchronous messages");
  }
  catch (const ctkRuntimeException& e)
  {
    error = true;
    exc = e;
    throw e;
  }
  catch (...)
  {
    error = true;
  }
}

//----------------------------------------------------------------------------
void ctkEAScenario3EventPublisher::sendEvents()
{
  for (int i = 0; i < messageTosend; i++)
  {
    /* a Hash table to store message in */
    ctkDictionary message;
    /* put some properties into the messages */
    message.insert("Synchronous message", i);
    /* send the message */
    qDebug() << "sending a synchronous event with message:"
             << message << "and the topic:" << topicToSend;
    eventAdmin->sendEvent(ctkEvent(topicToSend, message));
  }
  thread.quit();
}

//----------------------------------------------------------------------------
void ctkEAScenario3EventPublisher::postEvents()
{
  for (int i = 0; i < messageTosend; i++)
  {
    /* create the hasht table */
    ctkDictionary message;
    /* create the message */
    message.insert("Asynchronous message", i);
    /* Sends a asynchronous event to the admin */
    qDebug() << "sending an Asynchronous event with message:"
             << message << "and the topic:" << topicToSend;
    eventAdmin->postEvent(ctkEvent(topicToSend, message));
  }
  thread.quit();
}

//----------------------------------------------------------------------------
ctkEAScenario3EventPublisher::ctkEAScenario3EventPublisher(ctkPluginContext* context, const QString& name,
                             int id, int numOfMessage, const QString& topic)
  : eventAdmin(0), context(context), messageTosend(numOfMessage), topicToSend(topic)
{
  thread.setObjectName(QString("%1-%2").arg(name).arg(id));
  moveToThread(&thread);
}

//----------------------------------------------------------------------------
void ctkEAScenario3EventPublisher::runTest()
{
  /* Claims the reference of the EventAdmin Service */
  serviceReference = context->getServiceReference<ctkEventAdmin>();

  /* assert that a reference is aquired */
  QVERIFY2(serviceReference, "Should be able to get reference to ctkEventAdmin service");

  eventAdmin = context->getService<ctkEventAdmin>(serviceReference);

  QVERIFY2(eventAdmin, "Should be able to get instance to ctkEventAdmin object");

  connect(&thread, SIGNAL(started()), SLOT(sendEvents()));
  thread.start();
  /* wait until thread is dead */
  thread.wait();

  disconnect(&thread, SIGNAL(started()), this, SLOT(sendEvents()));
  connect(&thread, SIGNAL(started()), SLOT(postEvents()));
  thread.start();
  /* wait until thread is dead */
  thread.wait();

  QTest::qWait(1000); // allow for delivery
}

//----------------------------------------------------------------------------
ctkEAScenario3TestSuite::ctkEAScenario3TestSuite(ctkPluginContext* context, long eventPluginId)
  : pluginContext(context), eventPluginId(eventPluginId)
{

}

//----------------------------------------------------------------------------
void ctkEAScenario3TestSuite::initTestCase()
{
  pluginContext->getPlugin(eventPluginId)->start();

  /* create a topic string */
  QStringList scenario3_topics1;
  scenario3_topics1 << "com/acme/timer";
  scenario3_topics1 << "com/acme/log";

  /* add the event consumer with the correct topics to the test suite */
  eventConsumers.push_back(new ctkEAScenario3EventConsumer(pluginContext, scenario3_topics1, 8, 8));

  eventPublishers.push_back(new ctkEAScenario3EventPublisher(
                              pluginContext, "Scenario 3 EventPublisher1",
                              3, 4, "com/acme/timer"));
  eventPublishers.push_back(new ctkEAScenario3EventPublisher(
                              pluginContext, "Scenario 3 EventPublisher2",
                              3, 4, "com/acme/log"));
}

//----------------------------------------------------------------------------
void ctkEAScenario3TestSuite::cleanupTestCase()
{
  foreach(ctkEAScenario3EventConsumer* eventConsumer, eventConsumers)
  {
    eventConsumer->cleanup();
  }
  qDeleteAll(eventPublishers);
  qDeleteAll(eventConsumers);
  pluginContext->getPlugin(eventPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkEAScenario3TestSuite::testRegisterConsumer()
{
  foreach(ctkEAScenario3EventConsumer* consumer, eventConsumers)
  {
    consumer->runTest();
  }
}

//----------------------------------------------------------------------------
void ctkEAScenario3TestSuite::testPublishEvents()
{
  foreach(ctkEAScenario3EventPublisher* publisher, eventPublishers)
  {
    publisher->runTest();
  }
}
