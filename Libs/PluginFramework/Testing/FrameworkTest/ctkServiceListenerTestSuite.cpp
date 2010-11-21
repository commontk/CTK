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

#include <ctkPluginFrameworkTestUtil.h>

#include <QTest>

ctkServiceListenerTestSuite::ctkServiceListenerTestSuite(ctkPluginContext* pc)
  : pc(pc), p(pc->getPlugin())
{
}

void ctkServiceListenerTestSuite::initTestCase()
{
  pA = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginA_test");
  QVERIFY(pA);
}

void ctkServiceListenerTestSuite::cleanupTestCase()
{
  pA->uninstall();
}

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
    catch (const std::logic_error& ise)
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
    catch (const std::logic_error& ise)
    {
      teststatus  = false;
      qDebug() << "service listener removal failed " << ise.what()
               << " :" << tcName << ":FAIL";
    }
  }
  return teststatus;
}

ctkServiceListener::ctkServiceListener(ctkPluginContext* pc, bool checkUsingBundles)
  : checkUsingBundles(checkUsingBundles), teststatus(true), pc(pc)
{

}

void ctkServiceListener::clearEvents()
{
  events.clear();
}

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

void ctkServiceListener::serviceChanged(const ctkServiceEvent& evt)
{
  events.push_back(evt);
  qDebug() << "ServiceEvent:" << evt;
  if (ctkServiceEvent::UNREGISTERING == evt.getType())
  {
    ctkServiceReference sr = evt.getServiceReference();

    // Validate that no bundle is marked as using the service
    QList<QSharedPointer<ctkPlugin> > usingPlugins = sr.getUsingPlugins();
    if (checkUsingBundles && !usingPlugins.isEmpty())
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
      if (checkUsingBundles && usingPlugins.size() != 1)
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

void ctkServiceListener::printUsingPlugins(const ctkServiceReference& sr,
                                           const QString& caption)
{
  QList<QSharedPointer<ctkPlugin> > usingPlugins = sr.getUsingPlugins();

  qDebug() << (caption.isEmpty() ? "Using plugins: " : caption);
  foreach(QSharedPointer<ctkPlugin> plugin, usingPlugins)
  {
    qDebug() << "  -" << plugin;
  }
}

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
