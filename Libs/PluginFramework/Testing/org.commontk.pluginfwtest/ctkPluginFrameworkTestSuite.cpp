/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkPluginFrameworkTestSuite_p.h"

#include <ctkPluginFrameworkTestUtil.h>
#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>
#include <ctkPluginException.h>
#include <ctkServiceException.h>

#include <QDir>
#include <QTest>
#include <QDebug>


int ctkPluginFrameworkTestSuite::nRunCount = 0;


//----------------------------------------------------------------------------
ctkPluginFrameworkTestSuite::ctkPluginFrameworkTestSuite(ctkPluginContext* pc)
  : eventDelay(500), pc(pc), p(pc->getPlugin())
{

}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestSuite::initTestCase()
{
  qDebug() << "### plugin framework test suite: SETUP start";
  if (nRunCount > 0)
  {
    QFAIL("The ctkPluginFrameworkTestSuite CANNOT be run reliably more than once. Other test results in this suite are/may not be valid. Restart framework to retest: CLEANUP:FAIL");
  }
  ++nRunCount;

  try
  {
    bool success = pc->connectFrameworkListener(this, SLOT(frameworkListener(ctkPluginFrameworkEvent)));
    if (!success)
    {
      QFAIL("plugin framework test suite: SETUP:FAIL");
    }
  }
  catch (const ctkException& e)
  {
    QString msg  = QString("plugin framework test suite ") + e.what() + ": SETUP:FAIL";
    QFAIL(msg.toLatin1());
  }

  try
  {
    bool success = pc->connectPluginListener(this, SLOT(pluginListener(ctkPluginEvent)));
    if (!success)
    {
      QFAIL("plugin framework test suite: SETUP:FAIL");
    }
  }
  catch (const ctkException& e)
  {
    QString msg  = QString("plugin framework test suite ") + e.what() + ": SETUP:FAIL";
    QFAIL(msg.toLatin1());
  }

  try
  {
    bool success = pc->connectPluginListener(this, SLOT(syncPluginListener(ctkPluginEvent)), Qt::DirectConnection);
    if (!success)
    {
      QFAIL("plugin framework test suite: SETUP:FAIL");
    }
  }
  catch (const ctkException& e)
  {
    QString msg  = QString("plugin framework test suite ") + e.what() + ": SETUP:FAIL";
    QFAIL(msg.toLatin1());
  }

  try
  {
    pc->connectServiceListener(this, "serviceListener");
  }
  catch (const ctkException& e)
  {
    QString msg  = QString("plugin framework test suite ") + e.what() + ": SETUP:FAIL";
    QFAIL(msg.toLatin1());
  }

  qDebug() << "### plugin framework test suite: SETUP:PASS";
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestSuite::cleanupTestCase()
{

}

//----------------------------------------------------------------------------
// Verify information from the getHeaders() method
void ctkPluginFrameworkTestSuite::frame005a()
{
  QHash<QString, QString> headers = p->getHeaders();

  // check expected headers

  QString k = "Plugin-Name";
  QCOMPARE(QString("framework_test"), headers.value(k));

  k = "Plugin-Version";
  QCOMPARE(QString("1.0.1"), headers.value(k));

  k = "Plugin-Description";
  QCOMPARE(QString("Test bundle for the CTK plugin framework"), headers.value(k));

  k = "Plugin-Vendor";
  QCOMPARE(QString("CommonTK"), headers.value(k));

  k = "Plugin-DocURL";
  QCOMPARE(QString("http://www.commontk.org"), headers.value(k));

  k = "Plugin-ContactAddress";
  QCOMPARE(QString("http://www.commontk.org"), headers.value(k));

  k = "Plugin-Category";
  QCOMPARE(QString("test"), headers.value(k));

  k = "Plugin-Copyright";
  QCOMPARE(QString("German Cancer Research Center, Division of Medical and Biological Informatics"), headers.value(k));

  k = "Plugin-License";
  QCOMPARE(QString("http://www.apache.org/licenses/LICENSE-2.0.html"), headers.value(k));
}

//----------------------------------------------------------------------------
// Extract all information from the getProperty in the ctkPluginContext class
void ctkPluginFrameworkTestSuite::frame007a()
{
  QList<QString> NNList;
  NNList << ctkPluginConstants::FRAMEWORK_VERSION
         << ctkPluginConstants::FRAMEWORK_VENDOR;

  foreach(QString k, NNList)
  {
    QVariant v = pc->getProperty(k);
    if(!v.isValid())
    {
      QString msg("'%1' not set");
      QFAIL(qPrintable(msg.arg(k)));
    }
  }
}

//----------------------------------------------------------------------------
// Get context id, location and status of the plugin
void ctkPluginFrameworkTestSuite::frame010a()
{
  qlonglong contextid = p->getPluginId();
  qDebug() << "CONTEXT ID:" << contextid;

  QString location = p->getLocation();
  qDebug() << "LOCATION:" << location;

  ctkPlugin::State pstate = p->getState();
  qDebug() << "PCACTIVE:" << pstate;
}

//----------------------------------------------------------------------------
//Test result of getService(ctkServiceReference()). Should throw ctkInvalidArgumentException
void ctkPluginFrameworkTestSuite::frame018a()
{
  try
  {
    QObject* obj = pc->getService(ctkServiceReference());
    qDebug() << "Got service object =" << obj->metaObject()->className() << ", excpected ctkInvalidArgumentException exception";
    QFAIL("Got service object, excpected ctkInvalidArgumentException exception");
  }
  catch (const ctkInvalidArgumentException& )
  {}
  catch (...)
  {
    QFAIL("Got wrong exception, expected ctkInvalidArgumentException");
  }
}

//----------------------------------------------------------------------------
// Load pluginA_test and check that it exists and that its expected service does not exist,
// also check that the expected events in the framework occur
void ctkPluginFrameworkTestSuite::frame020a()
{
  pA.clear();

  try
  {
    pA = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginA_test");
  }
  catch (ctkPluginException& e)
  {
    QFAIL(e.what());
  }

  const QHash<QString, QString> headers = pA->getHeaders();
  QHash<QString, QString>::const_iterator iter =
      headers.find(ctkPluginConstants::PLUGIN_SYMBOLICNAME);
  QVERIFY(iter != headers.end());
  QCOMPARE(iter.value(), QString("pluginA.test"));

  // Check that no service reference exist yet.
  ctkServiceReference sr1 = pc->getServiceReference("org.commontk.TestPluginAService");
  if (sr1)
  {
    QFAIL("framework test plugin, service from test plugin A unexpectedly found");
  }


  // check the listeners for events, expect only a plugin event,
  // of type installation
  bool lStat = checkListenerEvents(false, ctkPluginFrameworkEvent::PLUGIN_INFO, true , ctkPluginEvent::INSTALLED,
                                   false, ctkServiceEvent::MODIFIED, pA, 0);

  QVERIFY(pA->getState() == ctkPlugin::INSTALLED && lStat == true);
}

//----------------------------------------------------------------------------
// Start pluginA_test and check that it gets state ACTIVE,
// and that the service it registers exist
void ctkPluginFrameworkTestSuite::frame025b()
{
  try
  {
    pA->start();
    QVERIFY2(pA->getState() == ctkPlugin::ACTIVE, "pluginA_test should be ACTIVE");
  }
  catch (const ctkPluginException& pexcA)
  {
    QString failMsg = QString("Unexpected plugin exception: ") + pexcA.what();
    QFAIL(failMsg.toStdString().c_str());
  }
  catch (const std::exception& le)
  {
    QString failMsg = QString("Start plugin exception: ") + le.what();
    QFAIL(failMsg.toStdString().c_str());
  }

  // Check if pluginA_test registered the expected service
  try
  {
    ctkServiceReference sr1 = pc->getServiceReference("org.commontk.pluginAtest.TestPluginAService");
    QObject* o1 = pc->getService(sr1);
    QVERIFY2(o1 != 0, "no service object found");

    try
    {
      QVERIFY2(pc->ungetService(sr1), "Service unget should return true");
    }
    catch (const std::exception& le)
    {
      QString failMsg = QString("Unget service exception: ") + le.what();
      QFAIL(failMsg.toStdString().c_str());
    }

    // check the listeners for events
    QList<ctkPluginEvent> pEvts;
    pEvts.push_back(ctkPluginEvent(ctkPluginEvent::RESOLVED, pA));
    pEvts.push_back(ctkPluginEvent(ctkPluginEvent::STARTED, pA));

    QList<ctkServiceEvent> seEvts;
    seEvts.push_back(ctkServiceEvent(ctkServiceEvent::REGISTERED, sr1));

    QVERIFY2(checkListenerEvents(QList<ctkPluginFrameworkEvent>(), pEvts, seEvts),
             "Unexpected events");

    QList<ctkPluginEvent> syncPEvts;
    syncPEvts.push_back(ctkPluginEvent(ctkPluginEvent::STARTING, pA));

    QVERIFY2(checkSyncListenerEvents(syncPEvts), "Unexpected events");
  }
  catch (const ctkServiceException& /*se*/)
  {
    QFAIL("framework test bundle, expected service not found");
  }
}

//----------------------------------------------------------------------------
// Stop pluginA_test and check that it gets state RESOLVED
void ctkPluginFrameworkTestSuite::frame030b()
{
  ctkServiceReference sr1
      = pc->getServiceReference("org.commontk.pluginAtest.TestPluginAService");

  try
  {
    pA->stop();
    QVERIFY2(pA->getState() == ctkPlugin::RESOLVED, "pluginA should be RESOLVED");
  }
  catch (const ctkIllegalStateException& ise)
  {
    qDebug() << "Unexpected ctkIllegalStateException exception:" << ise.what();
    QFAIL("framework test plugin, stop plugin pluginA");
  }
  catch (const ctkPluginException& pe)
  {
    qDebug() << "Unexpected plugin exception:" << pe;
    QFAIL("framework test plugin, stop plugin pluginA");
  }

  QList<ctkPluginEvent> pEvts;
  pEvts << ctkPluginEvent(ctkPluginEvent::STOPPED, pA);

  QList<ctkServiceEvent> seEvts;
  seEvts << ctkServiceEvent(ctkServiceEvent::UNREGISTERING, sr1);

  QVERIFY2(checkListenerEvents(QList<ctkPluginFrameworkEvent>(), pEvts, seEvts),
           "Unexpected events");

  QList<ctkPluginEvent> syncPEvts;
  syncPEvts << ctkPluginEvent(ctkPluginEvent::STOPPING, pA);

  QVERIFY2(checkSyncListenerEvents(syncPEvts), "Unexpected events");
}

//----------------------------------------------------------------------------
// Uninstall pluginA_test and check that it gets state UNINSTALLED
void ctkPluginFrameworkTestSuite::frame035b()
{
  try
  {
    pA->uninstall();
    QVERIFY2(pA->getState() == ctkPlugin::UNINSTALLED,
             "pluginA_test should be UNINSTALLED");
  }
  catch (const ctkIllegalStateException& ise)
  {
    qDebug() << "Unexpected ctkIllegalStateException exception:" << ise.what();
    QFAIL("framework test plugin, uninstall");
  }
  catch (const ctkPluginException& pe)
  {
    qDebug() << "Unexpected plugin exception:" << pe;
    QFAIL("framework test plugin, uninstall pluginA_test");
  }


  QList<ctkPluginEvent> pEvts;
  pEvts << ctkPluginEvent(ctkPluginEvent::UNRESOLVED, pA);
  pEvts << ctkPluginEvent(ctkPluginEvent::UNINSTALLED, pA);

  QVERIFY2(checkListenerEvents(QList<ctkPluginFrameworkEvent>(),
                               pEvts, QList<ctkServiceEvent>()),
           "Unexpected events");

  QVERIFY2(checkSyncListenerEvents(QList<ctkPluginEvent>()),
           "Unexpected sync events");
}

//----------------------------------------------------------------------------
// Install pluginD_test, check that a ctkPluginException is thrown
// as this plugin is not a Qt plugin
void ctkPluginFrameworkTestSuite::frame040a()
{
  bool teststatus = true;
  bool exception = false;
  try
  {
    pD = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginD_test");
    exception = false;
  }
  catch (const ctkPluginException& pe)
  {
    // This exception is expected
    qDebug() << "Expected exception" << pe;
    exception = true;
  }
  //      catch (SecurityException secA) {
  //        QFAIL("framework test plugin " + secA + " :FRAME040A:FAIL");
  //        teststatus = false;
  //        exception = true;
  //      }

  if (exception == false)
  {
    teststatus = false;
  }

  // check the listeners for events, expect no events
  bool lStat = checkListenerEvents(false, ctkPluginFrameworkEvent::FRAMEWORK_STARTED,
                                   false , ctkPluginEvent::INSTALLED,
                                   false, ctkServiceEvent::MODIFIED,
                                   pD, 0);

  QVERIFY(teststatus == true && pD.isNull() && lStat == true);
}

//----------------------------------------------------------------------------
// Install a non-existent plug-in
void ctkPluginFrameworkTestSuite::frame042a()
{
  bool exception = false;
  try
  {
    pc->installPlugin(QUrl("file://no-plugin"));
    exception = false;
  }
  catch (const ctkPluginException& pe)
  {
    // This exception is expected
    qDebug() << "Expected exception" << pe;
    exception = true;
  }

  // check the listeners for events, expect no events
  bool lStat = checkListenerEvents(false, ctkPluginFrameworkEvent::FRAMEWORK_STARTED,
                                   false , ctkPluginEvent::INSTALLED,
                                   false, ctkServiceEvent::MODIFIED,
                                   QSharedPointer<ctkPlugin>(), 0);

  QVERIFY(exception == true && lStat == true);
}

//----------------------------------------------------------------------------
// Add a service listener with a broken LDAP filter to get an exception
void ctkPluginFrameworkTestSuite::frame045a()
{
  ctkServiceListenerPFW sListen1;
  QString brokenFilter = "A broken LDAP filter";

  try
  {
    pc->connectServiceListener(&sListen1, "serviceChanged", brokenFilter);
  }
  catch (const ctkInvalidArgumentException& /*ia*/)
  {
    //assertEquals("InvalidSyntaxException.getFilter should be same as input string", brokenFilter, ise.getFilter());
  }
  catch (...)
  {
    QFAIL("framework test bundle, wrong exception on broken LDAP filter:");
  }
}

//----------------------------------------------------------------------------
// Reinstalls and the updates testbundle_A.
// The version is checked to see if an update has been made.
void ctkPluginFrameworkTestSuite::frame070a()
{
  QString pluginA = "pluginA_test";
  QString pluginA1 = "libpluginA1_test";
  //InputStream fis;
  QString versionA;
  QString versionA1;

  pA.clear();

  clearEvents();

  try
  {
    pA = ctkPluginFrameworkTestUtil::installPlugin(pc, pluginA);
  }
  catch (const ctkPluginException& pexcA)
  {
    qDebug() << "framework test plugin" << pexcA << ":FRAME070A:FAIL";
  }
//  catch (const ctkSecurityException& secA)
//  {
//    qDebug() << "framework test plugin" << secA << ":FRAME070A:FAIL";
//    teststatus = false;
//  }

  QHash<QString,QString> ai = pA->getHeaders();
  versionA = ai["Plugin-Version"];
  qDebug() << "Before version =" << versionA;

  QDir testPluginDir(pc->getProperty("pluginfw.testDir").toString());
  QString pluginA1Path;

  QStringList libSuffixes;
  libSuffixes << ".so" << ".dll" << ".dylib";
  foreach(QString libSuffix, libSuffixes)
  {
    QFileInfo info(testPluginDir, pluginA1 + libSuffix);
    if (info.exists())
    {
      pluginA1Path = info.absoluteFilePath();
      break;
    }
  }

  if (pluginA1Path.isEmpty())
  {
    qDebug() << "Plug-in" << pluginA1 << "not found in" << testPluginDir;
    QFAIL("Test plug-in not found");
  }

  QUrl urk = QUrl::fromLocalFile(pluginA1Path);
  qDebug() << "update from" << urk;

  try
  {
    pA->update(urk);
  }
  catch (const ctkPluginException& /*pe*/)
  {
    QFAIL("framework test plug-in, update without new plug-in source :FRAME070A:FAIL");
  }

  QHash<QString,QString> a1i = pA->getHeaders();
  versionA1 = a1i["Plugin-Version"];
  qDebug() << "After version =" << versionA1;

  QList<ctkPluginEvent> pEvts;
  pEvts << ctkPluginEvent(ctkPluginEvent::INSTALLED, pA);
  pEvts << ctkPluginEvent(ctkPluginEvent::RESOLVED, pA);
  pEvts << ctkPluginEvent(ctkPluginEvent::UNRESOLVED, pA);
  pEvts << ctkPluginEvent(ctkPluginEvent::UPDATED, pA);


  QVERIFY2(checkListenerEvents(QList<ctkPluginFrameworkEvent>(),
                               pEvts, QList<ctkServiceEvent>()),
           "Unexpected events");

  QVERIFY2(versionA1 != versionA, "framework test plug-in, update of plug-in failed, version info unchanged :FRAME070A:Fail");
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestSuite::frameworkListener(const ctkPluginFrameworkEvent& fwEvent)
{
  frameworkEvents.push_back(fwEvent);
  qDebug() << "FrameworkEvent:" << fwEvent;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestSuite::pluginListener(const ctkPluginEvent& event)
{
  pluginEvents.push_back(event);
  qDebug() << "PluginEvent:" << event;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestSuite::syncPluginListener(const ctkPluginEvent& event)
{
  if (event.getType() == ctkPluginEvent::STARTING ||
      event.getType() == ctkPluginEvent::STOPPING)
  {
    syncPluginEvents.push_back(event);
    qDebug() << "Synchronous PluginEvent:" << event;
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestSuite::serviceListener(const ctkServiceEvent& event)
{
  serviceEvents.push_back(event);
  qDebug() << "ServiceEvent:" << event;
}

//----------------------------------------------------------------------------
ctkPluginEvent ctkPluginFrameworkTestSuite::getPluginEvent() const
{
  if (pluginEvents.empty())
  {
    return ctkPluginEvent();
  }
  return pluginEvents.last();
}

//----------------------------------------------------------------------------
ctkPluginEvent ctkPluginFrameworkTestSuite::getSyncPluginEvent() const
{
  if (syncPluginEvents.empty())
  {
    return ctkPluginEvent();
  }
  return syncPluginEvents.last();
}

//----------------------------------------------------------------------------
ctkPluginFrameworkEvent ctkPluginFrameworkTestSuite::getFrameworkEvent() const
{
  if (frameworkEvents.empty())
  {
    return ctkPluginFrameworkEvent();
  }
  return frameworkEvents.last();
}

//----------------------------------------------------------------------------
ctkServiceEvent ctkPluginFrameworkTestSuite::getServiceEvent() const
{
  if (serviceEvents.empty())
  {
    return ctkServiceEvent();
  }
  return serviceEvents.last();
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkTestSuite::checkListenerEvents(
    bool fwexp, ctkPluginFrameworkEvent::Type fwtype,
    bool pexp, ctkPluginEvent::Type ptype,
    bool sexp, ctkServiceEvent::Type stype,
    QSharedPointer<ctkPlugin> pluginX, ctkServiceReference* servX)
{
  QList<ctkPluginFrameworkEvent> fwEvts;
  QList<ctkPluginEvent> pEvts;
  QList<ctkServiceEvent> seEvts;

  if (fwexp) fwEvts << ctkPluginFrameworkEvent(fwtype, pluginX);
  if (pexp) pEvts << ctkPluginEvent(ptype, pluginX);
  if (sexp) seEvts << ctkServiceEvent(stype, *servX);

  return checkListenerEvents(fwEvts, pEvts, seEvts);
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkTestSuite::checkListenerEvents(
    const QList<ctkPluginFrameworkEvent>& fwEvts,
    const QList<ctkPluginEvent>& pEvts,
    const QList<ctkServiceEvent>& seEvts)
{
  bool listenState = true; // assume everything will work

  // Wait a while to allow events to arrive
  QTest::qWait(eventDelay);
  //QCoreApplication::sendPostedEvents();

  if (fwEvts.size() != frameworkEvents.size())
  {
    listenState = false;
    qDebug() << "*** Plugin Framework event mismatch: expected"
        << fwEvts.size() << "event(s), found"
        << frameworkEvents.size() << "event(s).";

    const int max = fwEvts.size() > frameworkEvents.size()
                    ? fwEvts.size() : frameworkEvents.size();
    for (int i = 0; i < max; ++i)
    {
      const ctkPluginFrameworkEvent& fwE = i < fwEvts.size() ? fwEvts[i] : ctkPluginFrameworkEvent();
      const ctkPluginFrameworkEvent& fwR = i < frameworkEvents.size() ? frameworkEvents[i] : ctkPluginFrameworkEvent();
      qDebug() << "    " << fwE << " - " << fwR;
    }
  }
  else
  {
    for (int i = 0; i < fwEvts.size(); ++i)
    {
      const ctkPluginFrameworkEvent& feE = fwEvts[i];
      const ctkPluginFrameworkEvent& feR = frameworkEvents[i];
      if (feE.getType() != feR.getType()
        || feE.getPlugin() != feR.getPlugin())
      {
        listenState = false;
        qDebug() << "*** Wrong framework event:" << feR
            << "expected" << feE;
      }
    }
  }

  if (pEvts.size() != pluginEvents.size())
  {
    listenState = false;
    qDebug() << "*** Plugin event mismatch: expected"
        << pEvts.size() << "event(s), found "
        << pluginEvents.size() << "event(s).";

    const int max = pEvts.size() > pluginEvents.size() ? pEvts.size() : pluginEvents.size();
    for (int i = 0; i < max; ++i)
    {
      const ctkPluginEvent& pE = i < pEvts.size() ? pEvts[i] : ctkPluginEvent();
      const ctkPluginEvent& pR = i < pluginEvents.size() ? pluginEvents[i] : ctkPluginEvent();
      qDebug() << "    " << pE << " - " << pR;
    }
  }
  else
  {
    for (int i = 0; i < pEvts.size(); ++i)
    {
      const ctkPluginEvent& pE = pEvts[i];
      const ctkPluginEvent& pR = pluginEvents[i];
      if (pE.getType() != pR.getType()
        || pE.getPlugin() != pR.getPlugin())
      {
        listenState = false;
        qDebug() << "*** Wrong plugin event:" << pR << "expected" << pE;
      }
    }
  }

  if (seEvts.size() != serviceEvents.size())
  {
    listenState = false;
    qDebug() << "*** Service event mismatch: expected"
        << seEvts.size() << "event(s), found"
        << serviceEvents.size() << "event(s).";

    const int max = seEvts.size() > serviceEvents.size()
                    ? seEvts.size() : serviceEvents.size();
    for (int i = 0; i < max; ++i)
    {
      const ctkServiceEvent& seE = i < seEvts.size() ? seEvts[i] : ctkServiceEvent();
      const ctkServiceEvent& seR = i < serviceEvents.size() ? serviceEvents[i] : ctkServiceEvent();
      qDebug() << "    " << seE << " - " << seR;
    }
  }
  else
  {
    for (int i = 0; i < seEvts.size(); ++i)
    {
      const ctkServiceEvent& seE = seEvts[i];
      const ctkServiceEvent& seR = serviceEvents[i];
      if (seE.getType() != seR.getType()
        || (!(seE.getServiceReference() == seR.getServiceReference())))
      {
        listenState = false;
        qDebug() << "*** Wrong service event:" << seR << "expected" << seE;
      }
    }
  }

  frameworkEvents.clear();
  pluginEvents.clear();
  serviceEvents.clear();
  return listenState;
}

//----------------------------------------------------------------------------
// Check that the expected events have reached the listeners and
// reset the events
bool ctkPluginFrameworkTestSuite::checkSyncListenerEvents(
    bool pexp, ctkPluginEvent::Type ptype, QSharedPointer<ctkPlugin> pX,
    ctkServiceReference servX)
{
  Q_UNUSED(servX)

  QList<ctkPluginEvent> pEvts;

  if (pexp)
  {
    pEvts << ctkPluginEvent(ptype, pX);
  }

  return checkSyncListenerEvents(pEvts);
}

//----------------------------------------------------------------------------
// Check that the expected events have reached the listeners and
// reset the events
bool ctkPluginFrameworkTestSuite::checkSyncListenerEvents(
    const QList<ctkPluginEvent>& pEvts)
{
  bool listenState = true; // assume everything will work

  // Sleep a while to allow events to arrive
  QTest::qWait(eventDelay);

  if (pEvts.size() != syncPluginEvents.size())
  {
    listenState = false;
    qDebug() << "*** Sync plugin event mismatch: expected"
        << pEvts.size() << "event(s), found"
        << syncPluginEvents.size() << "event(s).";

    const int max = pEvts.size() > syncPluginEvents.size() ? pEvts.size() : syncPluginEvents.size();
    for (int i = 0; i < max; ++i)
    {
      const ctkPluginEvent& pE =  i< pEvts.size() ? pEvts[i] : ctkPluginEvent();
      const ctkPluginEvent& pR = i < syncPluginEvents.size() ? syncPluginEvents[i] : ctkPluginEvent();
      qDebug() << "    " << pE << " - " << pR;
    }
  }
  else
  {
    for (int i = 0; i < pEvts.size(); ++i)
    {
      const ctkPluginEvent& pE = pEvts[i];
      const ctkPluginEvent& pR = syncPluginEvents[i];
      if (pE.getType() != pR.getType() || pE.getPlugin() != pR.getPlugin())
      {
        listenState = false;
        qDebug() << "Wrong sync plugin event:" << pR << "expected" << pE;
      }
    }
  }

  syncPluginEvents.clear();
  return listenState;
}

void ctkPluginFrameworkTestSuite::clearEvents()
{
  QTest::qWait(300);
  pluginEvents.clear();
  syncPluginEvents.clear();
  frameworkEvents.clear();
  serviceEvents.clear();
}

//----------------------------------------------------------------------------
ctkServiceEvent ctkServiceListenerPFW::getEvent() const
{
  return events.size() ? events.last() : ctkServiceEvent();
}

//----------------------------------------------------------------------------
QList<ctkServiceEvent> ctkServiceListenerPFW::getEvents() const
{
  return events;
}

//----------------------------------------------------------------------------
void ctkServiceListenerPFW::clearEvent()
{
  events.clear();
}

//----------------------------------------------------------------------------
void ctkServiceListenerPFW::serviceChanged(const ctkServiceEvent& evt)
{
  events.push_back(evt);
  qDebug() << "ctkServiceEvent:" << evt;
}
