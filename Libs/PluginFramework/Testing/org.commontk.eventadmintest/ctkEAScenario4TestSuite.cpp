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


#include "ctkEAScenario4TestSuite_p.h"

#include <ctkPluginContext.h>
#include <service/event/ctkEventConstants.h>
#include <service/event/ctkEventAdmin.h>

#include <QTest>

//----------------------------------------------------------------------------
ctkEAScenario4EventConsumer::ctkEAScenario4EventConsumer(
  ctkPluginContext* pluginContext, const QStringList topics,
  int numSyncMsg, int numAsyncMsg, const QString& filter)
  : context(pluginContext), topicsToConsume(topics), filterToConsume(filter),
    asynchMessages(0), synchMessages(0), numSyncMessages(numSyncMsg),
    numAsyncMessages(numAsyncMsg), error(false), exc("")
{

}

//----------------------------------------------------------------------------
void ctkEAScenario4EventConsumer::runTest()
{
  asynchMessages = 0;
  synchMessages = 0;
  /* create the hashtable to put properties in */
  ctkDictionary props;

  /* put service.pid property in hashtable */
  props.insert(ctkEventConstants::EVENT_TOPIC, topicsToConsume);

  if (!filterToConsume.isEmpty())
  {
    props.insert(ctkEventConstants::EVENT_FILTER, filterToConsume);
  }

  /* register the service */
  serviceRegistration = context->registerService<ctkEventHandler>(this, props);

  QVERIFY2(serviceRegistration, "service registration should not be null");
}

//----------------------------------------------------------------------------
void ctkEAScenario4EventConsumer::cleanup()
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
void ctkEAScenario4EventConsumer::handleEvent(const ctkEvent& event)
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
void ctkEAScenario4EventPublisher::sendEvents()
{
  /* a Hash table to store message in */
  ctkDictionary message(propertiesToSend);

  for (int i = 0; i < messageTosend; i++)
  {
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
void ctkEAScenario4EventPublisher::postEvents()
{
  /* create the hash table */
  ctkDictionary message(propertiesToSend);

  for (int i = 0; i < messageTosend; i++)
  {
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
ctkEAScenario4EventPublisher::ctkEAScenario4EventPublisher(ctkPluginContext* context, const QString& name,
                                                           const QString& topic, const ctkDictionary& props,
                                                           int id, int numOfMessage)
  : eventAdmin(0), context(context), messageTosend(numOfMessage), topicToSend(topic), propertiesToSend(props)
{
  thread.setObjectName(QString("%1-%2").arg(name).arg(id));
  moveToThread(&thread);
}

//----------------------------------------------------------------------------
void ctkEAScenario4EventPublisher::runTest()
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
ctkEAScenario4TestSuite::ctkEAScenario4TestSuite(ctkPluginContext* context, long eventPluginId)
  : pluginContext(context), eventPluginId(eventPluginId)
{

}

//----------------------------------------------------------------------------
void ctkEAScenario4TestSuite::initTestCase()
{
  pluginContext->getPlugin(eventPluginId)->start();

  /* keys and properties to be used in the EventProducers*/
  ctkDictionary keysAndProps1;
  keysAndProps1.insert("year", "2004");
  keysAndProps1.insert("month", "12");
  ctkDictionary keysAndProps2;
  keysAndProps2.insert("year", "2005");
  keysAndProps2.insert("month", "12");
  ctkDictionary keysAndProps3;
  keysAndProps3.insert("YEAR", "2005");
  keysAndProps3.insert("month", "11"); // Won't year filters match because year is not present?

  /*Topics to be used in the EventConsumers*/
  QStringList scenario4_topics1("com/acme/timer");
  /*Filters to be used in the EventConsumers*/
  QString scenario4_filter1 = "(year=2004)";
  QString scenario4_filter2 = "(year=2005)";
  QString scenario4_filter3 = "(year:2004)";
  QString scenario4_filter4;
  QString scenario4_filter5 = "(month=12)";

  /* add the event consumer with the correct topics to the test suite */
  eventConsumers.push_back(new ctkEAScenario4EventConsumer(pluginContext, scenario4_topics1,
                                                           1, 1, scenario4_filter1));
  eventConsumers.push_back(new ctkEAScenario4EventConsumer(pluginContext, scenario4_topics1,
                                                           1, 1, scenario4_filter2));
  eventConsumers.push_back(new ctkEAScenario4EventConsumer(pluginContext, scenario4_topics1,
                                                           0, 0, scenario4_filter3));
  eventConsumers.push_back(new ctkEAScenario4EventConsumer(pluginContext, scenario4_topics1,
                                                           3, 3, scenario4_filter4));
  eventConsumers.push_back(new ctkEAScenario4EventConsumer(pluginContext, scenario4_topics1,
                                                           2, 2, scenario4_filter5));

  /* add the event publisher to the test suite */
  eventPublishers.push_back(new ctkEAScenario4EventPublisher(pluginContext, "Scenario 4 EventPublisher1",
                                                             "com/acme/timer", keysAndProps1, 4, 1));
  eventPublishers.push_back(new ctkEAScenario4EventPublisher(pluginContext, "Scenario 4 EventPublisher2",
                                                             "com/acme/timer", keysAndProps2, 4, 1));
  eventPublishers.push_back(new ctkEAScenario4EventPublisher(pluginContext, "Scenario 4 EventPublisher3",
                                                             "com/acme/timer", keysAndProps3, 4, 1));
}

//----------------------------------------------------------------------------
void ctkEAScenario4TestSuite::cleanupTestCase()
{
  foreach(ctkEAScenario4EventConsumer* eventConsumer, eventConsumers)
  {
    eventConsumer->cleanup();
  }
  qDeleteAll(eventPublishers);
  qDeleteAll(eventConsumers);
  pluginContext->getPlugin(eventPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkEAScenario4TestSuite::testRegisterConsumer()
{
  foreach(ctkEAScenario4EventConsumer* consumer, eventConsumers)
  {
    consumer->runTest();
  }
}

//----------------------------------------------------------------------------
void ctkEAScenario4TestSuite::testPublishEvents()
{
  foreach(ctkEAScenario4EventPublisher* publisher, eventPublishers)
  {
    publisher->runTest();
  }
}
