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


#include "ctkServiceListenerTestSuite_p.h"

#include <ctkPlugin.h>
#include <ctkPluginContext.h>
#include <ctkPluginException.h>
#include <ctkPluginConstants.h>
#include <ctkServiceEvent.h>
#include <ctkServiceException.h>

#include <ctkPluginFrameworkTestUtil.h>

#include <QTest>

//----------------------------------------------------------------------------
ctkServiceListenerTestSuite::ctkServiceListenerTestSuite(ctkPluginContext* pc)
  : pc(pc), p(pc->getPlugin())
{
}

//----------------------------------------------------------------------------
void ctkServiceListenerTestSuite::initTestCase()
{
  pA = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginA_test");
  QVERIFY(pA);
  pA2 = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginA2_test");
  QVERIFY(pA2);
  pSL1 = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginSL1_test");
  QVERIFY(pSL1);
  pSL3 = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginSL3_test");
  QVERIFY(pSL3);
  pSL4 = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginSL4_test");
  QVERIFY(pSL4);
}

//----------------------------------------------------------------------------
void ctkServiceListenerTestSuite::cleanupTestCase()
{
  pA->uninstall();
  pA2->uninstall();
  pSL1->uninstall();
  pSL3->uninstall();
  pSL4->uninstall();
}

//----------------------------------------------------------------------------
void ctkServiceListenerTestSuite::frameSL05a()
{
  bool teststatus = true;
  int cnt = 1;

  QList<ctkServiceEvent::Type> events;
  events << ctkServiceEvent::REGISTERED;
  events << ctkServiceEvent::UNREGISTERING;
  teststatus = runStartStopTest("FRAMEsl05A", cnt, pA, events);

  QVERIFY(teststatus);
}

//----------------------------------------------------------------------------
void ctkServiceListenerTestSuite::frameSL10a()
{
  int cnt = 1;

  QList<ctkServiceEvent::Type> events;
  events << ctkServiceEvent::REGISTERED;
  events << ctkServiceEvent::UNREGISTERING;
  QVERIFY(runStartStopTest("FRAMEsl10A", cnt, pA2,events));
}

//----------------------------------------------------------------------------
void ctkServiceListenerTestSuite::frameSL25a()
{
  ctkServiceListener sListen(pc, false);
  try
  {
    pc->connectServiceListener(&sListen, "serviceChanged");
  }
  catch (const ctkIllegalStateException& ise)
  {
    qDebug() << "service listener registration failed " << ise.what();
    QFAIL("service listener registration failed");
  }

  QList<ctkServiceEvent::Type> expectedServiceEventTypes;

  // Startup
  expectedServiceEventTypes << ctkServiceEvent::REGISTERED; // ctkActivator at start of pSL1
  expectedServiceEventTypes << ctkServiceEvent::REGISTERED; // ctkFooService at start of pSL4
  expectedServiceEventTypes << ctkServiceEvent::REGISTERED; // ctkActivator at start of pSL3

  // Stop pSL4
  expectedServiceEventTypes << ctkServiceEvent::UNREGISTERING; // ctkFooService at first stop of pSL4

  // Shutdown
  expectedServiceEventTypes << ctkServiceEvent::UNREGISTERING; // ctkActivator at stop of pSL1
  expectedServiceEventTypes << ctkServiceEvent::UNREGISTERING; // Activator at stop of pSL3


  // Start pSL1 to ensure that the Service interface is available.
  try
  {
    qDebug() << "Starting pSL1: " << pSL1;
    pSL1->start();
  }
  catch (const ctkPluginException& pex)
  {
    qDebug() << "Failed to start plugin, got exception:" << pex;
    QFAIL("Failed to start bundle, got exception: ctkPluginException");
  }
  catch (const std::exception& e)
  {
    qDebug() << "Failed to start plugin, got exception" << e.what();
    QFAIL("Failed to start plugin, got exception: std::exception");
  }

  // Start pSL4 that will require the serivce interface and publish
  // ctkFooService
  try
  {
    qDebug() << "Starting pSL4:" << pSL4;
    pSL4->start();
  }
  catch (const ctkPluginException& pex)
  {
    qDebug() << "Failed to start plugin, got exception:" << pex;
    QFAIL("Failed to start bundle, got exception: ctkPluginException");
  }
  catch (const std::exception& e)
  {
    qDebug() << "Failed to start plugin, got exception" << e.what();
    QFAIL("Failed to start plubin, got exception.");
  }

  // Start buSL3 that will require the serivce interface and get the service
  try
  {
    qDebug() << "Starting pSL3:" << pSL3;
    pSL3->start();
  }
  catch (const ctkPluginException& pex)
  {
    qDebug() << "Failed to start plugin, got exception:" << pex;
    QFAIL("Failed to start plugin, got exception: ctkPluginException");
  }
  catch (const std::exception& e)
  {
    qDebug() << "Failed to start plugin, got exception" << e.what();
    QFAIL("Failed to start plugin, got exception.");
  }

  // sleep to stabelize state.
  QTest::qWait(300);

  // Check that pSL3 has been notified about the ctkFooService.
  qDebug() << "Check that ctkFooService is added to service tracker in pSL3";
  try
  {
    ctkServiceReference pSL3SR
        = pc->getServiceReference("ctkActivatorSL3");
    QObject* pSL3Activator = pc->getService(pSL3SR);
    QVERIFY2(pSL3Activator, "No activator service.");

    QVariant serviceAddedField3 = pSL3Activator->property("serviceAdded");
    QVERIFY2(serviceAddedField3.isValid() && serviceAddedField3.toBool(),
             "pluginSL3 not notified about presence of ctkFooService");
    qDebug() << "pSL3Activator::serviceAdded is true";
    pc->ungetService(pSL3SR);
  }
  catch (const ctkServiceException& se)
  {
    qDebug() << "Failed to get service reference:" << se;
    QFAIL("No activator service reference.");
  }

  // Check that pSL1 has been notified about the ctkFooService.
  qDebug() << "Check that ctkFooService is added to service tracker in pSL1";
  try
  {
    ctkServiceReference pSL1SR
        = pc->getServiceReference("ctkActivatorSL1");
    QObject* pSL1Activator = pc->getService(pSL1SR);
    QVERIFY2(pSL1Activator, "No activator service.");

    QVariant serviceAddedField = pSL1Activator->property("serviceAdded");
    QVERIFY2(serviceAddedField.isValid() && serviceAddedField.toBool(),
             "bundleSL1 not notified about presence of ctkFooService");
    qDebug() << "pSL1Activator::serviceAdded is true";
    pc->ungetService(pSL1SR);
  }
  catch (const ctkServiceException& se)
  {
    qDebug() << "Failed to get service reference:" << se;
    QFAIL("No activator service reference.");
  }

  // Stop the service provider: pSL4
  try
  {
    qDebug() << "Stop pSL4:" << pSL4;
    pSL4->stop();
  }
  catch (const ctkPluginException& pex)
  {
    qDebug() << "Failed to stop plugin, got exception:" << pex;
    QFAIL("Failed to stop bundle, got exception: ctkPluginException");
  }
  catch (const std::exception& e)
  {
    qDebug() << "Failed to stop plugin, got exception:" << e.what();
    QFAIL("Failed to stop plugin, got exception.");
  }

  // sleep to stabelize state.
  QTest::qWait(300);


  // Check that pSL3 has been notified about the removal of ctkFooService.
  qDebug() << "Check that ctkFooService is removed from service tracker in pSL3";
  try
  {
    ctkServiceReference pSL3SR
        = pc->getServiceReference("ctkActivatorSL3");
    QObject* pSL3Activator = pc->getService(pSL3SR);
    QVERIFY2(pSL3Activator, "No activator service.");
    QVariant serviceRemovedField3 = pSL3Activator->property("serviceRemoved");
    QVERIFY2(serviceRemovedField3.isValid() && serviceRemovedField3.toBool(),
             "pluginSL3 not notified about removal of ctkFooService");
    qDebug() << "pSL3Activator::serviceRemoved is true";
    pc->ungetService(pSL3SR);
  }
  catch (const ctkServiceException& se)
  {
    qDebug() << "Failed to get service reference:" << se;
    QFAIL("No activator service reference.");
  }

  // Stop pSL1
  try
  {
    qDebug() << "Stop pSL1:" << pSL1;
    pSL1->stop();
  }
  catch (const ctkPluginException& pex)
  {
    qDebug() << "Failed to stop plugin, got exception:" << pex;
    QFAIL("Failed to stop bundle, got exception: ctkPluginException");
  }
  catch (const std::exception& e)
  {
    qDebug() << "Failed to stop plugin, got exception" << e.what();
    QFAIL("Failed to stop plugin, got exception.");
  }

  // Stop pSL3
  try
  {
    qDebug() << "Stop pSL3:" << pSL3;
    pSL3->stop();
  }
  catch (const ctkPluginException& pex)
  {
    qDebug() << "Failed to stop plugin, got exception:" << pex;
    QFAIL("Failed to stop plugin, got exception: ctkPluginException");
  }
  catch (const std::exception& e)
  {
    qDebug() << "Failed to stop plugin, got exception" << e.what();
    QFAIL("Failed to stop plugin, got exception.");
  }


  // sleep to stabelize state.
  QTest::qWait(300);

  // Check service events seen by this class
  qDebug() << "Checking ServiceEvents(ServiceListener):";
  if (!sListen.checkEvents(expectedServiceEventTypes))
  {
    qDebug() << "Service listener event notification error";
    QFAIL("Service listener event notification error");
  }


  QVERIFY2(sListen.teststatus, "Service listener checks");
  try 
  {
    //pc->disconnectServiceListener(&sListen, "serviceChanged");
    sListen.clearEvents();
  } 
  catch (const ctkIllegalStateException& ise)
  {
    qDebug() << ise.what();
    QFAIL("service listener removal failed ");
  }
}

//----------------------------------------------------------------------------
bool ctkServiceListenerTestSuite::runStartStopTest(
  const QString& tcName, int cnt, QSharedPointer<ctkPlugin> targetPlugin,
  const QList<ctkServiceEvent::Type>& events)
{
  bool teststatus = true;

  for (int i = 0; i < cnt && teststatus; ++i)
  {
    ctkServiceListener sListen(pc);
    try
    {
      pc->connectServiceListener(&sListen, "serviceChanged");
    }
    catch (const ctkIllegalStateException& ise)
    {
      teststatus  = false;
      qDebug() << "service listener registration failed " << ise.what()
               << " :" << tcName << ":FAIL";
    }

    // Start the test target to get a service published.
    try
    {
      qDebug() << "Starting targetPlugin: " << targetPlugin;
      targetPlugin->start();
    }
    catch (const ctkPluginException& pex)
    {
      teststatus  = false;
      qDebug() << "Failed to start plugin, got exception: "
               << pex.what() << " in " << tcName << ":FAIL";
    }
    catch (const std::exception& e)
    {
      teststatus  = false;
      qDebug() << "Failed to start plugin, got exception: "
               << e.what() << " + in " << tcName << ":FAIL";
    }

    // sleep to stabelize state.
    QTest::qWait(300);

    // Stop the test target to get a service unpublished.
    try
    {
      targetPlugin->stop();
    }
    catch (const ctkPluginException& pex)
    {
      teststatus  = false;
      qDebug() << "Failed to stop plugin, got exception: "
               << pex.what() << " in " << tcName << ":FAIL";
    }
    catch (const std::exception& e)
    {
      teststatus  = false;
      qDebug() << "Failed to stop plugin, got exception: "
               << e.what() << " + in " << tcName << ":FAIL";
    }

    if (teststatus && !sListen.checkEvents(events))
    {
      teststatus  = false;
      qDebug() << "Service listener event notification error :"
               << tcName << ":FAIL";
    }

    try
    {
      pc->disconnectServiceListener(&sListen, "serviceChanged");
      teststatus &= sListen.teststatus;
      sListen.clearEvents();
    }
    catch (const ctkIllegalStateException& ise)
    {
      teststatus  = false;
      qDebug() << "service listener removal failed " << ise.what()
               << " :" << tcName << ":FAIL";
    }
  }
  return teststatus;
}

//----------------------------------------------------------------------------
ctkServiceListener::ctkServiceListener(ctkPluginContext* pc, bool checkUsingPlugins)
  : checkUsingPlugins(checkUsingPlugins), teststatus(true), pc(pc)
{

}

//----------------------------------------------------------------------------
void ctkServiceListener::clearEvents()
{
  events.clear();
}

//----------------------------------------------------------------------------
bool ctkServiceListener::checkEvents(const QList<ctkServiceEvent::Type>& eventTypes)
{
  if (events.size() != eventTypes.size())
  {
    dumpEvents(eventTypes);
    return false;
  }

  for (int i=0; i < eventTypes.size(); ++i)
  {
    if (eventTypes[i] != events[i].getType())
    {
      dumpEvents(eventTypes);
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------
void ctkServiceListener::serviceChanged(const ctkServiceEvent& evt)
{
  events.push_back(evt);
  qDebug() << "ServiceEvent:" << evt;
  if (ctkServiceEvent::UNREGISTERING == evt.getType())
  {
    ctkServiceReference sr = evt.getServiceReference();

    // Validate that no bundle is marked as using the service
    QList<QSharedPointer<ctkPlugin> > usingPlugins = sr.getUsingPlugins();
    if (checkUsingPlugins && !usingPlugins.isEmpty())
    {
      teststatus = false;
      printUsingPlugins(sr, "*** Using plugins (unreg) should be empty but is: ");
    }

    // Check if the service can be fetched
    QObject* service = pc->getService(sr);
    usingPlugins = sr.getUsingPlugins();
//    if (UNREGISTERSERVICE_VALID_DURING_UNREGISTERING) {
      // In this mode the service shall be obtainable during
      // unregistration.
      if (service == 0)
      {
        teststatus = false;
        qDebug() << "*** Service should be available to ServiceListener "
                 << "while handling unregistering event.";
      }
      qDebug() << "Service (unreg): " << service->metaObject()->className();
      if (checkUsingPlugins && usingPlugins.size() != 1)
      {
        teststatus = false;
        printUsingPlugins(sr, "*** One using plugin expected "
                          "(unreg, after getService), found: ");
      }
      else
      {
        printUsingPlugins(sr, "Using plugins (unreg, after getService): ");
      }
//    } else {
//      // In this mode the service shall NOT be obtainable during
//      // unregistration.
//      if (null!=service) {
//        teststatus = false;
//        out.print("*** Service should not be available to ServiceListener "
//                  +"while handling unregistering event.");
//      }
//      if (checkUsingBundles && null!=usingBundles) {
//        teststatus = false;
//        printUsingBundles(sr,
//                          "*** Using bundles (unreg, after getService), "
//                          +"should be null but is: ");
//      } else {
//        printUsingBundles(sr,
//                          "Using bundles (unreg, after getService): null");
//      }
//    }
    pc->ungetService(sr);

    // Check that the UNREGISTERING service can not be looked up
    // using the service registry.
    try
    {
      qulonglong sid = sr.getProperty(ctkPluginConstants::SERVICE_ID).toLongLong();
      QString sidFilter = QString("(") + ctkPluginConstants::SERVICE_ID + "=" + sid + ")";
      QList<ctkServiceReference> srs = pc->getServiceReferences("", sidFilter);
      if (srs.isEmpty())
      {
        qDebug() << "ctkServiceReference for UNREGISTERING service is not"
                    " found in the service registry; ok.";
      }
      else
      {
        teststatus = false;
        qDebug() << "*** ctkServiceReference for UNREGISTERING service,"
                 << sr << ", not found in the service registry; fail.";
        qDebug() << "Found the following Service references:";
        foreach(ctkServiceReference sr, srs)
        {
          qDebug() << sr;
        }
      }
    }
    catch (const std::exception& e)
    {
      teststatus = false;
      qDebug() << "*** Unexpected excpetion when trying to lookup a"
                  " service while it is in state UNREGISTERING;"
               << e.what();
    }
  }
}

//----------------------------------------------------------------------------
void ctkServiceListener::printUsingPlugins(const ctkServiceReference& sr,
                                           const QString& caption)
{
  QList<QSharedPointer<ctkPlugin> > usingPlugins = sr.getUsingPlugins();

  qDebug() << (caption.isEmpty() ? "Using plugins: " : caption);
  foreach(QSharedPointer<ctkPlugin> plugin, usingPlugins)
  {
    qDebug() << "  -" << plugin.data();
  }
}

//----------------------------------------------------------------------------
void ctkServiceListener::dumpEvents(const QList<ctkServiceEvent::Type>& eventTypes)
{
  int max = events.size() > eventTypes.size() ? events.size() : eventTypes.size();
  qDebug() << "Expected event type --  Actual event";
  for (int i=0; i < max; ++i)
  {
    ctkServiceEvent evt = i < events.size() ? events[i] : ctkServiceEvent();
    if (i < eventTypes.size())
    {
      qDebug() << " " << eventTypes[i] << "--" << evt;
    }
    else
    {
      qDebug() << " " << "- NONE - " << "--" << evt;
    }
  }
}
