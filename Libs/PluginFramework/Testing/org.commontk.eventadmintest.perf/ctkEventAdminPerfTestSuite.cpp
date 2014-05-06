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


#include "ctkEventAdminPerfTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkServiceEvent.h>

#include <service/event/ctkEventAdmin.h>
#include <service/event/ctkEventConstants.h>

#include <QTest>
#include <QDebug>


//----------------------------------------------------------------------------
TestEventHandler::TestEventHandler(int& counter)
  : counter(counter)
{}

//----------------------------------------------------------------------------
void TestEventHandler::handleEvent(const ctkEvent& )
{
  counter++;
}

//----------------------------------------------------------------------------
ctkEventAdminPerfTestSuite::ctkEventAdminPerfTestSuite(ctkPluginContext *context, int pluginId)
  : pc(context)
  , pluginId(pluginId)
  , nSendEvents(400)
  , nHandlers(40)
  , nEvent1Handled(0)
  , nEvent2Handled(0)
  , eventAdmin(0)
{
}

//----------------------------------------------------------------------------
void ctkEventAdminPerfTestSuite::addHandlers()
{
  qDebug() << "Adding" << nHandlers << "event handlers";
  for (int i = 0; i < nHandlers; ++i)
  {
    TestEventHandler* h1 = new TestEventHandler(nEvent1Handled);
    handlers.push_back(h1);
    ctkDictionary props1;
    props1.insert(ctkEventConstants::EVENT_TOPIC, "org/bla/1");
    handlerRegistrations.push_back(pc->registerService<ctkEventHandler>(h1, props1));


    TestEventHandler* h2 = new TestEventHandler(nEvent2Handled);
    handlers.push_back(h2);
    ctkDictionary props2;
    props2.insert(ctkEventConstants::EVENT_TOPIC, "org/bla/*");
    handlerRegistrations.push_back(pc->registerService<ctkEventHandler>(h2, props2));

    TestEventHandler* h3 = new TestEventHandler(nEvent2Handled);
    handlers.push_back(h3);
    ctkDictionary props3;
    props3.insert(ctkEventConstants::EVENT_TOPIC, "org/bla/*");
    props3.insert(ctkEventConstants::EVENT_FILTER, "(name=bla)");
    handlerRegistrations.push_back(pc->registerService<ctkEventHandler>(h3, props3));
  }
}

//----------------------------------------------------------------------------
void ctkEventAdminPerfTestSuite::removeHandlers()
{
  foreach(ctkServiceRegistration sr, handlerRegistrations)
  {
    sr.unregister();
  }
  handlerRegistrations.clear();
  qDeleteAll(handlers);
  handlers.clear();
}

//----------------------------------------------------------------------------
void ctkEventAdminPerfTestSuite::sendEvents()
{
  ctkEvent event1("org/bla/1");
  for (int i = 0; i < nSendEvents; ++i)
  {
    eventAdmin->sendEvent(event1);
  }

  for (int i = 0; i < nSendEvents; ++i)
  {
    ctkDictionary props;
    props.insert("name", "bla");
    props.insert("level", i);
    ctkEvent event2("org/bla/2", props);
    eventAdmin->sendEvent(event2);
  }
}

//----------------------------------------------------------------------------
void ctkEventAdminPerfTestSuite::postEvents()
{
  ctkEvent event1("org/bla/1");
  for (int i = 0; i < nSendEvents; ++i)
  {
    eventAdmin->postEvent(event1);
  }

  for (int i = 0; i < nSendEvents; ++i)
  {
    ctkDictionary props;
    props.insert("name", "bla");
    props.insert("level", i);
    ctkEvent event2("org/bla/2", props);
    eventAdmin->postEvent(event2);
  }
}

//----------------------------------------------------------------------------
void ctkEventAdminPerfTestSuite::initTestCase()
{
  nEvent1Handled = 0;
  nEvent2Handled = 0;

  pc->getPlugin(pluginId)->start();
  ctkServiceReference reference = pc->getServiceReference<ctkEventAdmin>();
  QVERIFY(reference);
  eventAdmin = pc->getService<ctkEventAdmin>(reference);
  QVERIFY(eventAdmin);

  addHandlers();
}

//----------------------------------------------------------------------------
void ctkEventAdminPerfTestSuite::testSendEvents()
{
  QTime t;
  t.start();
  sendEvents();
  int ms = t.elapsed();
  QCOMPARE(nEvent1Handled, nSendEvents * nHandlers);
  QCOMPARE(nEvent2Handled, nSendEvents * nHandlers * 3);
  qDebug() << "Sending" << 2*nSendEvents << "synchronous events took" << ms << "ms";
}

//----------------------------------------------------------------------------
void ctkEventAdminPerfTestSuite::testPostEvents()
{
  QTime t;
  t.start();
  postEvents();
  int ms = t.elapsed();
  qDebug() << "Sending" << 2*nSendEvents << "asynchronous events took" << ms << "ms";
  // wait a little for the asynchronous handling of events
  QTest::qWait(10000);
}

//----------------------------------------------------------------------------
void ctkEventAdminPerfTestSuite::cleanupTestCase()
{
  try
  {
    removeHandlers();
    pc->getPlugin(pluginId)->stop();
  }
  catch (const ctkInvalidArgumentException&)
  {
    qDebug() << "NOooooo!!!";
  }
}
