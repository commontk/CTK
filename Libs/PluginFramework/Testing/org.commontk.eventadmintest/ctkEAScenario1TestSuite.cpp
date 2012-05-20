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


#include "ctkEAScenario1TestSuite_p.h"

#include <ctkPluginContext.h>
#include <service/event/ctkEventConstants.h>
#include <service/event/ctkEventAdmin.h>

#include <QTest>

//----------------------------------------------------------------------------
ctkEAScenario1EventConsumer::ctkEAScenario1EventConsumer(
  ctkPluginContext* pluginContext, const QStringList& topics,
  int messagesSent, bool useSignalSlot)
  : context(pluginContext), topicsToConsume(topics),
    numOfasynchMessages(0), numOfsynchMessages(0),
    synchMessageExpectedNumber(0), asynchMessageExpectedNumber(0),
    messagesSent(messagesSent), error(false), exc(""),
    useSignalSlot(useSignalSlot)
{

}

//----------------------------------------------------------------------------
void ctkEAScenario1EventConsumer::runTest()
{
  numOfasynchMessages = 0;
  numOfsynchMessages = 0;
  synchMessageExpectedNumber = 0;
  asynchMessageExpectedNumber = 0;

  /* create the hashtable to put properties in */
  ctkDictionary props;
  /* put service.pid property in hashtable */
  props.insert(ctkEventConstants::EVENT_TOPIC, topicsToConsume);

  if (useSignalSlot)
  {
    /* Claims the reference of the EventAdmin Service */
    eventAdminRef = context->getServiceReference<ctkEventAdmin>();

    /* assert that a reference is aquired */
    QVERIFY2(eventAdminRef, "Should be able to get reference to ctkEventAdmin service");

    /* get the service  */
    eventAdmin = context->getService<ctkEventAdmin>(eventAdminRef);

    /* assert that service is available */
    QVERIFY2(eventAdmin, "Should be able to get instance to ctkEventAdmin object");

    slotId = eventAdmin->subscribeSlot(this, SLOT(handleEvent(ctkEvent)), props);
  }
  else
  {
    /* register the service */
    serviceRegistration = context->registerService<ctkEventHandler>(this, props);
    QVERIFY2(serviceRegistration, "service registration should not be invalid");
  }

}

//----------------------------------------------------------------------------
void ctkEAScenario1EventConsumer::cleanup()
{
  try
  {
    if (useSignalSlot)
    {
      eventAdmin->unsubscribeSlot(slotId);
      context->ungetService(eventAdminRef);
    }
    else
    {
      serviceRegistration.unregister();
    }
  }
  catch (const ctkException&)
  {}

  if (error)
  {
    throw exc;
  }
  QCOMPARE(messagesSent, synchMessageExpectedNumber); //"Not all synch messages recieved"
  QCOMPARE(messagesSent, asynchMessageExpectedNumber); //"Not all asynch messages recieved"
}

//----------------------------------------------------------------------------
void ctkEAScenario1EventConsumer::reset()
{
  numOfasynchMessages = 0;
  numOfsynchMessages = 0;
  synchMessageExpectedNumber = 0;
  asynchMessageExpectedNumber = 0;
}

//----------------------------------------------------------------------------
void ctkEAScenario1EventConsumer::handleEvent(const ctkEvent& event)
{
  try
  {

//TODO security topic permission
//    TopicPermission permissionAquired
//      = new TopicPermission((String)event.getProperty
//                            (EventConstants.EVENT_TOPIC),"subscribe");
//    TopicPermission actualPermission
//      = new TopicPermission("com/acme/*","subscribe");

//    assertTrue(getName() +"Should not recevice this topic:"
//               +(String)event.getProperty(EventConstants.EVENT_TOPIC),
//               actualPermission.implies(permissionAquired));

    /* try to get the message */
    QString message = event.getProperty("Synchronous message").toString();
    /* check if message is null */
    if(!message.isNull())
    {
      /* its a syncronous message */
      numOfsynchMessages++;
      /* print that a message is received */
      qDebug() << "received a synchronous event with message:" << message;
      /* get the message number */
      int aquiredNumber= message.toInt();
      /* assert that the message is the expected one */
      QVERIFY2(synchMessageExpectedNumber == aquiredNumber,
               qPrintable(QString("Expected syncronous message number: %1 got: %2 - order NOT conserved")
               .arg(synchMessageExpectedNumber).arg(aquiredNumber)));

      /* the next messages of this type should be +1 */
      synchMessageExpectedNumber++;
    }
    else
    {
      message = event.getProperty("Asynchronous message").toString();
      if(!message.isNull())
      {
        numOfasynchMessages++;
        qDebug() << "received an asynchronous event with message:" << message;
        /* get the message number */
        int aquiredNumber= message.toInt();
        /* assert that the message is the expected one */
        QVERIFY2(asynchMessageExpectedNumber==aquiredNumber,
                 qPrintable(QString("Expected asyncronous message number: %1 got: %2 - order NOT conserved")
                 .arg(asynchMessageExpectedNumber).arg(aquiredNumber)));

        /* the next messages of this type should be +1 */
        asynchMessageExpectedNumber++;
      }
    }

    /* assert that the messages property is not null */
    QVERIFY2(!message.isNull(), "Message should not be null in ");

    /* assert that the messages of syncronous type are not too many */
    QVERIFY2(numOfsynchMessages < messagesSent+1,
             "to many synchronous messages in handleEvent()");

    /* assert that the messsage of the asyncronous type are not too many */
    QVERIFY2(numOfasynchMessages < messagesSent+1,
             "to many asynchronous messages in handleEvent()");

  }
  catch (const ctkRuntimeException& e)
  {
    error = true;
    exc = e;
    throw;
  }
  catch (...)
  {
    error = true;
  }
}

//----------------------------------------------------------------------------
ctkEAScenario1EventPublisher::ctkEAScenario1EventPublisher(
  ctkPluginContext* context, const QString& name,
  int id, int numOfMessage, bool useSignalSlot)
  : eventAdmin(0), context(context), messageTosend(numOfMessage), useSignalSlot(useSignalSlot)
{
  thread.setObjectName(QString("%1-%2").arg(name).arg(id));
  moveToThread(&thread);
}

//----------------------------------------------------------------------------
void ctkEAScenario1EventPublisher::runTest()
{
  /* Claims the reference of the EventAdmin Service */
  serviceReference = context->getServiceReference<ctkEventAdmin>();

  /* assert that a reference is aquired */
  QVERIFY2(serviceReference, "Should be able to get reference to ctkEventAdmin service");

  /* get the service  */
  eventAdmin = context->getService<ctkEventAdmin>(serviceReference);

  /* assert that service is available */
  QVERIFY2(eventAdmin, "Should be able to get instance to ctkEventAdmin object");

  if (useSignalSlot)
  {
    eventAdmin->publishSignal(this, SIGNAL(syncSignalEvent(ctkDictionary)), "com/acme/timer", Qt::DirectConnection);
    eventAdmin->publishSignal(this, SIGNAL(asyncSignalEvent(ctkDictionary)), "com/acme/timer", Qt::QueuedConnection);
  }

  /* start the delivery thread */
  connect(&thread, SIGNAL(started()), SLOT(sendEvents()));

  /* print that the test has started */
  qDebug() << "Testing synchronus delivery";
  thread.start();

  /* wait until thread is dead */
  thread.wait();

  disconnect(&thread, SIGNAL(started()), this, SLOT(sendEvents()));
  connect(&thread, SIGNAL(started()), SLOT(postEvents()));
  /* print that the test has started */
  qDebug() << "Testing asynchronous delivery";
  /* start the test */
  thread.start();

  /* wait until thread is dead */
  thread.wait();

  context->ungetService(serviceReference);

  QTest::qWait(500); // allow for delivery
}

//----------------------------------------------------------------------------
void ctkEAScenario1EventPublisher::sendEvents()
{
  qDebug() << "Starting to send events";
  /* deliver the messages */
  for (int i = 0; i < messageTosend; i++)
  {
    /* a Hash table to store message in */
    ctkDictionary message;
    /* put some properties into the messages */
    message.insert("Synchronous message", i);
    /* send the message */
    ctkEvent event("com/acme/timer", message);
    if (useSignalSlot)
    {
      emit syncSignalEvent(message);
    }
    else
    {
      eventAdmin->sendEvent(event);
    }
  }
  thread.quit();
}

//----------------------------------------------------------------------------
void ctkEAScenario1EventPublisher::postEvents()
{
  qDebug() << "Starting to post events";
  for (int i = 0; i < messageTosend; i++)
  {
    /* create the hasht table */
    ctkDictionary message;
    /* create the message */
    message.insert("Asynchronous message", i);
    /* Sends an asynchronous event to the admin */
    ctkEvent event("com/acme/timer", message);
    if (useSignalSlot)
    {
      emit asyncSignalEvent(message);
    }
    else
    {
      eventAdmin->postEvent(event);
    }
  }
  thread.quit();
}

//----------------------------------------------------------------------------
ctkEAScenario1TestSuite::ctkEAScenario1TestSuite(ctkPluginContext* context, long eventPluginId, bool useSignalSlot)
  : pluginContext(context), MESSAGES_SENT(10), eventPluginId(eventPluginId),
    eventConsumer(0), eventPublisher(0), useSignalSlot(useSignalSlot)
{

}

//----------------------------------------------------------------------------
void ctkEAScenario1TestSuite::initTestCase()
{
  qDebug() << "Using" << (useSignalSlot ? "" : "no") << "Qt signal/slot mechanism";

  pluginContext->getPlugin(eventPluginId)->start();

  QStringList scenario1_topics("com/acme/*");

  /* add the event consumer to the test suite */
  eventConsumer = new ctkEAScenario1EventConsumer(pluginContext,
                                                  scenario1_topics, MESSAGES_SENT,
                                                  useSignalSlot);

  eventPublisher = new ctkEAScenario1EventPublisher(pluginContext,
                                                    "Scenario 1 EventPublisher",
                                                    1, MESSAGES_SENT,
                                                    useSignalSlot);
}

//----------------------------------------------------------------------------
void ctkEAScenario1TestSuite::cleanupTestCase()
{
  eventConsumer->cleanup();
  delete eventPublisher;
  delete eventConsumer;
  pluginContext->getPlugin(eventPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkEAScenario1TestSuite::testRegisterConsumer()
{
  eventConsumer->reset();
  eventConsumer->runTest();
}

//----------------------------------------------------------------------------
void ctkEAScenario1TestSuite::testPublishEvents()
{
  eventPublisher->runTest();
}
