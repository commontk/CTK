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


#include "ctkEAScenario2TestSuite_p.h"

#include <ctkPluginContext.h>
#include <service/event/ctkEventConstants.h>
#include <service/event/ctkEventAdmin.h>

#include <QTest>

//----------------------------------------------------------------------------
ctkEAScenario2EventConsumer::ctkEAScenario2EventConsumer(
  ctkPluginContext* pluginContext, const QStringList& topics,
  int numSyncMsg, int numAsyncMsg)
  : context(pluginContext),topicsToConsume(topics),
    asynchMessages(0), synchMessages(0), numSyncMessages(numSyncMsg),
    numAsyncMessages(numAsyncMsg), error(false), exc("")
{

}

//----------------------------------------------------------------------------
void ctkEAScenario2EventConsumer::runTest()
{
  asynchMessages = 0;
  synchMessages = 0;
  /* create the hashtable to put properties in */
  ctkDictionary props;
  /* put service.pid property in hashtable */
  props.insert(ctkEventConstants::EVENT_TOPIC, topicsToConsume);
  /* register the service */
  serviceRegistration = context->registerService<ctkEventHandler>(this, props);

  QVERIFY2(serviceRegistration, "service registration should not be invalid");
}

//----------------------------------------------------------------------------
void ctkEAScenario2EventConsumer::cleanup()
{
  QMutexLocker l(&mutex);
  try
  {
    serviceRegistration.unregister();
  }
  catch (const ctkIllegalStateException&)
  {}

  if (error)
  {
    throw exc;
  }

  QCOMPARE(synchMessages, numSyncMessages); // "Not all synch messages recieved"
  QCOMPARE(asynchMessages, numAsyncMessages); // "Not all asynch messages recieved"
}

//----------------------------------------------------------------------------
void ctkEAScenario2EventConsumer::handleEvent(const ctkEvent& event)
{
  QMutexLocker l(&mutex);
  try
  {
    /* get the topic from the event*/
//    QString eventTopic = event.getTopic();
    //TODO security topic permission
//    /* make a topic permission from the received topic in order to check it*/
//    TopicPermission permissionAccuired = new TopicPermission(eventTopic, "SUBSCRIBE");
//    /* make a topic permission from the topic to consume in order to check it*/
//    TopicPermission actualPermission = new TopicPermission(topicsToConsume[0], "SUBSCRIBE");
//    /* assert if the topic in the event is the same as the topic to listen fore including wildcard */
//    assertTrue("The topics was not equal", actualPermission.implies(permissionAccuired));

    /* try to get the message */
    QString message = event.getProperty("Synchronous message").toString();

    if(!message.isNull())
    {
      /* its a syncronous message */
      qDebug() << "received a synchronous event with message:" << message;
      /* assert that the messages of syncronous type are not too many */
      QVERIFY2(synchMessages < numSyncMessages, "too many synchronous messages");
      synchMessages++;
      qDebug() << "Max number of Sync messages is:" << numSyncMessages << "and number of received Sync messages is: " << synchMessages;
    }
    else
    {
      message = event.getProperty("Asynchronous message").toString();
      if(!message.isNull())
      {
        qDebug() << "received an asynchronous event with message:" << message;
        /* assert that the messsage of the asyncronous type are not too many */
        QVERIFY2(asynchMessages < numAsyncMessages, "too many asynchronous messages");
        asynchMessages++;
        qDebug() << "Max number of Async messages is:" << numAsyncMessages << "and number of received Async messages is:" << asynchMessages;
      }
    }
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
ctkEAScenario2EventPublisher::ctkEAScenario2EventPublisher(ctkPluginContext* context,
                             const QString& name, const QStringList topics,
                             int id, int numOfMessage)
  : eventAdmin(0), context(context),
    messageTosend(numOfMessage), topicsToSend(topics)
{
  thread.setObjectName(QString("%1-%2").arg(name).arg(id));
  moveToThread(&thread);
}

//----------------------------------------------------------------------------
void ctkEAScenario2EventPublisher::runTest()
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

  context->ungetService(serviceReference);

  QTest::qWait(1000); // allow for delivery
}

//----------------------------------------------------------------------------
void ctkEAScenario2EventPublisher::sendEvents()
{
  for (int i = 0; i < messageTosend; i++)
  {
    try
    {
      /* a Hash table to store message in */
      ctkDictionary message;
      /* put some properties into the messages */
      message.insert("Synchronous message",i);
      /* send the message */
      qDebug() << " sending a synchronous event with message:"
               << message << "and the topic:" << topicsToSend[i];
      eventAdmin->sendEvent(ctkEvent(topicsToSend[i], message));
    }
    catch(const std::exception& e)
    {
      qDebug() << "Error when publishing synhronous:" << e.what();
    }
  }
  thread.quit();
}

//----------------------------------------------------------------------------
void ctkEAScenario2EventPublisher::postEvents()
{
  for (int i = 0; i < messageTosend; i++)
  {
    try
    {
      /* create the hasht table */
      ctkDictionary message;
      /* create the message */
      message.insert("Asynchronous message",i);
      /* Sends an asynchronous event to the admin */
      qDebug() << "sending an Asynchronous event with message:"
               << message << "and the topic:" << topicsToSend[i];
      eventAdmin->postEvent(ctkEvent(topicsToSend[i], message));
    }
    catch(const std::exception& e)
    {
      qDebug() << "Error when publishing asynhronous:" << e.what();
    }
  }
  thread.quit();
}

//----------------------------------------------------------------------------
ctkEAScenario2TestSuite::ctkEAScenario2TestSuite(ctkPluginContext* context, long eventPluginId)
  : pluginContext(context), eventPluginId(eventPluginId)
{

}

//----------------------------------------------------------------------------
void ctkEAScenario2TestSuite::initTestCase()
{
  pluginContext->getPlugin(eventPluginId)->start();

  /* create a topic string */
  QStringList scenario2_topics1("com/acme/timer");
  QStringList scenario2_topics2("com/*");

  QStringList scenario2_topicsToPublish;
  scenario2_topicsToPublish << "";
  scenario2_topicsToPublish << "com/AcMe/TiMeR";
  scenario2_topicsToPublish << "com.acme.timer";
  scenario2_topicsToPublish << "com/acme/timer";

  eventConsumers.push_back(new ctkEAScenario2EventConsumer(
                             pluginContext, scenario2_topics1,
                             1,1));
  eventConsumers.push_back(new ctkEAScenario2EventConsumer(
                             pluginContext, scenario2_topics2,
                             2,2));

  eventPublisher = new ctkEAScenario2EventPublisher(
        pluginContext, "Scenario 2 EventPublisher",
        scenario2_topicsToPublish, 2, 4);
}

//----------------------------------------------------------------------------
void ctkEAScenario2TestSuite::cleanupTestCase()
{
  foreach (ctkEAScenario2EventConsumer* consumer, eventConsumers)
  {
    consumer->cleanup();
  }
  delete eventPublisher;
  qDeleteAll(eventConsumers);
  pluginContext->getPlugin(eventPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkEAScenario2TestSuite::testRegisterConsumer()
{
  foreach(ctkEAScenario2EventConsumer* consumer, eventConsumers)
  {
    consumer->runTest();
  }
}

//----------------------------------------------------------------------------
void ctkEAScenario2TestSuite::testPublishEvents()
{
  eventPublisher->runTest();
}
