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


#include "ctkPluginFrameworkPerfRegistryTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkHighPrecisionTimer.h>

#undef REGISTERED
#include <ctkServiceEvent.h>

#include <QTest>
#include <QDebug>

//----------------------------------------------------------------------------
ctkPluginFrameworkPerfRegistryTestSuite::ctkPluginFrameworkPerfRegistryTestSuite(ctkPluginContext* context)
  : QObject(0)
  , pc(context)
  , nListeners(100)
  , nServices(1000)
  , nRegistered(0)
  , nUnregistering(0)
  , nModified(0)
{
  this->setObjectName("ctkPluginFrameworkPerfRegistryTestSuite");
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::initTestCase()
{
  qDebug() << "Initialize event counters";

  nRegistered    = 0;
  nUnregistering = 0;
  nModified      = 0;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::cleanupTestCase()
{
  qDebug() << "Remove all service listeners";

  for(int i = 0; i < listeners.size(); i++)
  {
    try
    {
      ctkServiceListener* l = listeners[i];
      pc->disconnectServiceListener(l, "serviceChanged");
    }
    catch (const ctkException& e)
    {
      qDebug() << e.printStackTrace();
    }
  }
  listeners.clear();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::testAddListeners()
{
  addListeners(nListeners);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::addListeners(int n)
{
  log() << "adding" << n << "service listeners";
  for(int i = 0; i < n; i++)
  {
    ctkServiceListener* l = new ctkServiceListener(this);
    try
    {
      listeners.push_back(l);
      pc->connectServiceListener(l, "serviceChanged", "(perf.service.value>=0)");
    }
    catch (const ctkException& e)
    {
      qDebug() << e.printStackTrace();
    }
  }
  log() << "listener count=" << listeners.size();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::testRegisterServices()
{
  qDebug() << "Register services, and check that we get #of services ("
           << nServices << ") * #of listeners (" << nListeners << ")  REGISTERED events";

  log() << "registering" << nServices << "services, listener count=" << listeners.size();

  ctkHighPrecisionTimer t;
  t.start();
  registerServices(nServices);
  int ms = t.elapsedMilli();
  log() << "register took" << ms << "ms";
  QVERIFY2(nServices * listeners.size() == nRegistered,
           "# REGISTERED events must be same as # of registered services  * # of listeners");
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::registerServices(int n)
{
  QString pid("my.service.%1");

  for(int i = 0; i < n; i++)
  {
    ctkDictionary props;
    props.insert("service.pid", pid.arg(i));
    props.insert("perf.service.value", i+1);

    QObject* service = new PerfTestService();
    services.push_back(service);
    ctkServiceRegistration reg =
        pc->registerService<IPerfTestService>(service, props);
    regs.push_back(reg);
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::testModifyServices()
{
  qDebug() << "Modify all services, and check that we get #of services ("
           << nServices << ") * #of listeners (" << nListeners << ")  MODIFIED "
              << " events";

  ctkHighPrecisionTimer t;
  t.start();
  modifyServices();
  int ms = t.elapsedMilli();
  log() << "modify took" << ms << "ms";
  QVERIFY2(nServices * listeners.size() == nModified,
           "# MODIFIED events must be same as # of modified services  * # of listeners");
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::modifyServices()
{
  log() << "modifying " << regs.size() << "services, listener count=" << listeners.size();

  for(int i = 0; i < regs.size(); i++)
  {
    ctkServiceRegistration reg = regs[i];
    ctkDictionary props;
    props.insert("perf.service.value", i * 2);
    reg.setProperties(props);
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::testUnregisterServices()
{
  qDebug() << "Unregister all services, and check that we get #of services ("
           << nServices << ") * #of listeners (" << nListeners
           << ")  UNREGISTERING events";

  ctkHighPrecisionTimer t;
  t.start();
  unregisterServices();
  int ms = t.elapsedMilli();
  log() <<  "unregister took " << ms << "ms";
  QVERIFY2(nServices * listeners.size() == nUnregistering, "# UNREGISTERING events must be same as # of (un)registered services * # of listeners");
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkPerfRegistryTestSuite::unregisterServices()
{
  log() << "unregistering " << regs.size() << " services, listener count="
        << listeners.size();
  for(int i = 0; i < regs.size(); i++)
  {
    ctkServiceRegistration reg = regs[i];
    reg.unregister();
  }
  regs.clear();
}


//----------------------------------------------------------------------------
ctkServiceListener::ctkServiceListener(ctkPluginFrameworkPerfRegistryTestSuite* ts)
  : ts(ts)
{
}

//----------------------------------------------------------------------------
void ctkServiceListener::serviceChanged(const ctkServiceEvent& ev)
{
  switch(ev.getType())
  {
  case ctkServiceEvent::REGISTERED:
    ts->nRegistered++;
    break;
  case ctkServiceEvent::UNREGISTERING:
    ts->nUnregistering++;
    break;
  case ctkServiceEvent::MODIFIED:
    ts->nModified++;
    break;
  default:
    break;
  }
}
