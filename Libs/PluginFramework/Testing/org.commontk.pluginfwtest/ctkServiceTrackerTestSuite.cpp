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


#include "ctkServiceTrackerTestSuite_p.h"

#include <ctkPlugin.h>
#include <ctkPluginContext.h>
#include <ctkPluginException.h>
#include <ctkServiceTracker.h>

#include <ctkPluginFrameworkTestUtil.h>

#include <QTest>

//----------------------------------------------------------------------------
ctkServiceTrackerTestSuite::ctkServiceTrackerTestSuite(ctkPluginContext* pc)
  : pc(pc), p(pc->getPlugin())
{
}

//----------------------------------------------------------------------------
void ctkServiceTrackerTestSuite::initTestCase()
{
  // load and start pluginS_test, that may be prodded to
  // register/unregister some services. At start it registers one
  // service, org.commontk.pluginStest.TestPluginSService0
  pS = ctkPluginFrameworkTestUtil::installPlugin(pc, "pluginS_test");
  QVERIFY(pS);
}

//----------------------------------------------------------------------------
void ctkServiceTrackerTestSuite::cleanupTestCase()
{
  pS->uninstall();
}

//----------------------------------------------------------------------------
void ctkServiceTrackerTestSuite::runTest()
{
  try
  {
    pS->start();
    qDebug() << "started service plugin";
  }
  catch (const ctkPluginException& pexcS)
  {
    QString msg = QString("Test plugin: ") + pexcS.what();
    QFAIL(msg.toLatin1());
  }

  // 1. Create a ctkServiceTracker with ctkServiceTrackerCustomizer == null

  QString s1("org.commontk.pluginStest.TestPluginSService");
  ctkServiceReference servref = pc->getServiceReference(s1 + "0");

  QVERIFY2(servref != 0, "Test if registered service of id org.commontk.pluginStest.TestPluginSService0");

  connect(this, SIGNAL(serviceControl(int,QString,long)),
          pc->getService(servref), SLOT(controlService(int,QString,long)));

  ctkServiceTracker<>* st1 = new ctkServiceTracker<>(pc, servref);

  // 2. Check the size method with an unopened service tracker

  QVERIFY2(st1->size() == 0, "Test if size == 0");

  // 3. Open the service tracker and see what it finds,
  // expect to find one instance of the implementation,
  // "org.commontk.pluginStest.TestPluginSService0"

  st1->open();
  QString expName  = "ctkTestPluginS";
  QList<ctkServiceReference> sa2 = st1->getServiceReferences();

  QVERIFY(sa2.size() == 1);
  QString name = pc->getService(sa2[0])->metaObject()->className();
  QVERIFY(name == expName);

  // 5. Close this service tracker
  st1->close();

  // 6. Check the size method, now when the servicetracker is closed
  QVERIFY(st1->size() == 0);

  // 7. Check if we still track anything , we should get null
  sa2 = st1->getServiceReferences();
  QVERIFY(sa2.empty());

  // 8. A new Servicetracker, this time with a filter for the object
  QString fs = QString("(") + ctkPluginConstants::OBJECTCLASS + "=" + s1 + "*" + ")";
  ctkLDAPSearchFilter f1(fs);
  delete st1;
  st1 = new ctkServiceTracker<>(pc, f1);
  // add a service
  emit serviceControl(1, "register", 7);

  // 9. Open the service tracker and see what it finds,
  // expect to find two instances of references to
  // "org.commontk.pluginStest.TestPluginSService*"
  // i.e. they refer to the same piece of code

  st1->open();
  sa2 = st1->getServiceReferences();
  QVERIFY(sa2.size() == 2);
  for (int i = 0;  i < sa2.size(); ++i)
  {
    QString name = pc->getService(sa2[i])->metaObject()->className();
    QVERIFY(name == expName);
  }

  // 10. Get pluginS_test to register one more service and see if it appears
  emit serviceControl(2, "register", 1);
  sa2 = st1->getServiceReferences();
  QVERIFY(sa2.size() == 3);
  for (int i = 0; i < sa2.size(); ++i)
  {
    QString name = pc->getService(sa2[i])->metaObject()->className();
    QVERIFY(name == expName);
  }

  // 11. Get pluginS_test to register one more service and see if it appears
  emit serviceControl(3, "register", 2);
  sa2 = st1->getServiceReferences();
  QVERIFY(sa2.size() == 4);
  for (int i = 0; i < sa2.size(); ++i)
  {
    QString name = pc->getService(sa2[i])->metaObject()->className();
    QVERIFY(name == expName);
  }

  // 12. Get pluginS_test to unregister one service and see if it disappears
  emit serviceControl(3, "unregister", 0);
  sa2 = st1->getServiceReferences();
  QVERIFY(sa2.size() == 3);
  for (int i = 0; i < sa2.size(); ++i)
  {
    QString name = pc->getService(sa2[i])->metaObject()->className();
    QVERIFY(name == expName);
  }

  // 13. Get the highest ranking service reference, it should have ranking 7
  ctkServiceReference h1 = st1->getServiceReference();
  long rank = h1.getProperty(ctkPluginConstants::SERVICE_RANKING).toLongLong();
  QVERIFY(rank == 7);

  // 14. Get the service of the highest ranked service reference

  QObject* o1 = st1->getService(h1);
  QVERIFY(o1 != 0);

  // 14a Get the highest ranked service, directly this time
  QObject* o3 = st1->getService();
  QVERIFY(o3 != 0);
  QVERIFY(o1 == o3);

  // 15. Now release the tracking of that service and then try to get it
  //     from the servicetracker, which should yield a null object
  emit serviceControl (1, "unregister", 7);
  QObject* o2 = st1->getService(h1);
  QVERIFY(o2 == 0);

  // 16. Get all service objects this tracker tracks, it should be 2
  QList<QObject*> ts1 = st1->getServices();
  QVERIFY(ts1.size() == 2);

  // 17. Test the remove method.
  //     First register another service, then remove it being tracked
  emit serviceControl(1, "register", 7);
  h1 = st1->getServiceReference();
  QList<ctkServiceReference> sa3 = st1->getServiceReferences();
  QVERIFY(sa3.size() == 3);
  for (int i = 0; i < sa3.size(); ++i)
  {
    QString name = pc->getService(sa3[i])->metaObject()->className();
    QVERIFY(name == expName);
  }

  st1->remove(h1);           // remove tracking on one servref
  sa2 = st1->getServiceReferences();
  QVERIFY(sa2.size() == 2);

  // 18. Test the addingService method,add a service reference

  // 19. Test the removedService method, remove a service reference


  // 20. Test the waitForService method
  QObject* o9 = st1->waitForService(50);
  QVERIFY(o9 != 0);

  // 21. Test the waitForService method across threads
  ctkServiceTrackerTestWorker worker(pc);
  worker.start();
  QTest::qWait(100);
  // register "org.commontk.pluginStest.TestPluginSService3"
  emit serviceControl(3, "register", 2);
  // wait until the thread is finished
  QVERIFY(worker.wait());
  QVERIFY(worker.waitSuccess);

  delete st1;
}

ctkServiceTrackerTestWorker::ctkServiceTrackerTestWorker(ctkPluginContext* pc)
  : waitSuccess(false), pc(pc)
{

}

void ctkServiceTrackerTestWorker::run()
{
  ctkServiceTracker<> tracker(pc, "org.commontk.pluginStest.TestPluginSService3");
  tracker.open();
  // the tracker should initially be empty
  QVERIFY(tracker.isEmpty());
  QObject* obj = tracker.waitForService(1000);
  if (obj != 0) waitSuccess = true;
  quit();
}
