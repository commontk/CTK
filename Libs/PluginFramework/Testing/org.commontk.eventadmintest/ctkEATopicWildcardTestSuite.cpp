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


#include "ctkEATopicWildcardTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>

#include <service/event/ctkEventAdmin.h>
#include <service/event/ctkEventConstants.h>

#include <QTest>
#include <QDebug>

//----------------------------------------------------------------------------
ctkEvent ctkEATopicWildcardTestHelper::clearLastEvent()
{
  QWriteLocker l(&rwlock);
  ctkEvent result = last;
  last = ctkEvent();
  return result;
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestHelper::handleEvent(const ctkEvent& event)
{
  QWriteLocker l(&rwlock);
  last = event;
}

//----------------------------------------------------------------------------
ctkEvent ctkEATopicWildcardTestHelper::lastEvent() const
{
  QReadLocker l(&rwlock);
  return last;
}

//----------------------------------------------------------------------------
ctkEATopicWildcardTestSuite::ctkEATopicWildcardTestSuite(
  ctkPluginContext* pc, long eventPluginId, bool useSignalSlot)
  : context(pc), eventPluginId(eventPluginId),
    useSignalSlot(useSignalSlot), eventAdmin(0)
{

}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::initTestCase()
{
  qDebug() << "Using" << (useSignalSlot ? "" : "no") << "Qt signal/slot mechanism";
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::init()
{
  context->getPlugin(eventPluginId)->start();
  reference = context->getServiceReference<ctkEventAdmin>();
  eventAdmin = context->getService<ctkEventAdmin>(reference);

  if (useSignalSlot)
  {
    eventAdmin->publishSignal(this, SIGNAL(syncSignal(ctkDictionary)), "a/b/c", Qt::DirectConnection);
  }
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::cleanup()
{
  context->ungetService(reference);
  context->getPlugin(eventPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::testEventDeliveryForWildcardTopic1()
{
  ctkDictionary properties;
  properties.insert(ctkEventConstants::EVENT_TOPIC, "a/b/c/*");
  ctkEATopicWildcardTestHelper handler;
  ctkEvent event("a/b/c");

  if (useSignalSlot)
  {
    qlonglong id = eventAdmin->subscribeSlot(&handler, SLOT(handleEvent(ctkEvent)), properties);
    emit syncSignal(ctkDictionary());
    eventAdmin->unsubscribeSlot(id);
  }
  else
  {
    ctkServiceRegistration handlerRegistration = context->registerService<ctkEventHandler>(&handler, properties);
    eventAdmin->sendEvent(event);
    handlerRegistration.unregister();
  }

  QVERIFY2(handler.lastEvent().isNull(), "Received event published to topic 'a/b/c' while listening to 'a/b/c/*'");
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::testEventDeliveryForWildcardTopic2()
{
  ctkDictionary properties;
  properties.insert(ctkEventConstants::EVENT_TOPIC, "a/b/c/*");
  ctkEATopicWildcardTestHelper handler;
  ctkServiceRegistration handlerRegistration = context->registerService<ctkEventHandler>(&handler, properties);
  ctkEvent event("a/b");
  eventAdmin->sendEvent(event);
  QVERIFY2(handler.lastEvent().isNull(), "Received event published to topic 'a/b' while listening to 'a/b/c/*'");
  handlerRegistration.unregister();
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::testEventDeliveryForWildcardTopic3()
{
  ctkDictionary properties;
  properties.insert(ctkEventConstants::EVENT_TOPIC, "a/b/c/*");
  ctkEATopicWildcardTestHelper handler;
  ctkServiceRegistration handlerRegistration = context->registerService<ctkEventHandler>(&handler, properties);
  ctkEvent event("a");
  eventAdmin->sendEvent(event);
  QVERIFY2(handler.lastEvent().isNull(), "Received event published to topic 'a' while listening to 'a/b/c/*'");
  handlerRegistration.unregister();
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::testEventDeliveryForWildcardTopic4()
{
  ctkDictionary properties;
  properties.insert(ctkEventConstants::EVENT_TOPIC, "a/b/c/*");
  ctkEATopicWildcardTestHelper handler;
  ctkServiceRegistration handlerRegistration = context->registerService<ctkEventHandler>(&handler, properties);
  ctkEvent event("a/b/c/d");
  eventAdmin->sendEvent(event);
  QVERIFY2(!handler.lastEvent().isNull(), "Did not receive event published to topic 'a/b/c/d' while listening to 'a/b/c/*'");
  handlerRegistration.unregister();
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::testEventDeliveryForWildcardTopic5()
{
  ctkDictionary properties;
  properties.insert(ctkEventConstants::EVENT_TOPIC, "a/b/c/*");
  ctkEATopicWildcardTestHelper handler;
  ctkServiceRegistration handlerRegistration = context->registerService<ctkEventHandler>(&handler, properties);
  ctkEvent event("a/b/c/d/e");
  eventAdmin->sendEvent(event);
  QVERIFY2(!handler.lastEvent().isNull(), "Did not receive event published to topic 'a/b/c/d/e' while listening to 'a/b/c/*'");
  handlerRegistration.unregister();
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::testEventDeliveryForWildcardTopic6()
{
  ctkDictionary properties;
  properties.insert(ctkEventConstants::EVENT_TOPIC, "a/b/c/*");
  ctkEATopicWildcardTestHelper handler;
  ctkServiceRegistration handlerRegistration = context->registerService<ctkEventHandler>(&handler, properties);
  ctkEvent event("a/b/c/d/e/f");
  eventAdmin->sendEvent(event);
  QVERIFY2(!handler.lastEvent().isNull(), "Did not receive event published to topic 'a/b/c/d/e/f' while listening to 'a/b/c/*'");
  handlerRegistration.unregister();
}

//----------------------------------------------------------------------------
void ctkEATopicWildcardTestSuite::testEventDeliveryForWildcardTopic7()
{
  ctkDictionary properties;
  QStringList topics("a/b/c");
  topics << "a/b/c/*";
  properties.insert(ctkEventConstants::EVENT_TOPIC, topics);
  ctkEATopicWildcardTestHelper handler;
  ctkServiceRegistration handlerRegistration = context->registerService<ctkEventHandler>(&handler, properties);
  ctkEvent event("a/b/c");
  eventAdmin->sendEvent(event);
  QVERIFY2(!handler.clearLastEvent().isNull(), "Did not receive event published to topic 'a/b/c' while listening to 'a/b/c'");
  event = ctkEvent("a/b/c/d");
  eventAdmin->sendEvent(event);
  QVERIFY2(!handler.lastEvent().isNull(), "Did not receive event published to topic 'a/b/c/d' while listening to 'a/b/c/*'");
  handlerRegistration.unregister();
}

