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


#ifndef CTKPLUGINFRAMEWORKPERFREGISTRYTESTSUITE_P_H
#define CTKPLUGINFRAMEWORKPERFREGISTRYTESTSUITE_P_H

#include "ctkTestSuiteInterface.h"
#include "ctkServiceRegistration.h"

#include <QDebug>

class ctkPluginContext;
class ctkServiceEvent;

class ctkServiceListener;

class ctkPluginFrameworkPerfRegistryTestSuite : public QObject, public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

private:

  ctkPluginContext* pc;

  int nListeners;
  int nServices;

  int nRegistered;
  int nUnregistering;
  int nModified;

  QList<ctkServiceRegistration> regs;
  QList<ctkServiceListener*> listeners;
  QList<QObject*> services;

public:

  ctkPluginFrameworkPerfRegistryTestSuite(ctkPluginContext* context);

  QDebug log()
  {
    return qDebug() << "registry_perf:";
  }

private:

  friend class ctkServiceListener;

  void addListeners(int n);
  void registerServices(int n);
  void modifyServices();
  void unregisterServices();

private Q_SLOTS:

  void initTestCase();
  void cleanupTestCase();

  void testAddListeners();
  void testRegisterServices();

  void testModifyServices();
  void testUnregisterServices();
};

class ctkServiceListener : public QObject
{
  Q_OBJECT

private:

  ctkPluginFrameworkPerfRegistryTestSuite* ts;

public:

  ctkServiceListener(ctkPluginFrameworkPerfRegistryTestSuite* ts);

protected Q_SLOTS:

  void serviceChanged(const ctkServiceEvent& ev);
};

struct IPerfTestService
{
  virtual ~IPerfTestService() {}
};

Q_DECLARE_INTERFACE(IPerfTestService, "org.commontk.test.PerfTestService")

class PerfTestService : public QObject, public IPerfTestService
{
  Q_OBJECT
  Q_INTERFACES(IPerfTestService)
};


#endif // CTKPLUGINFRAMEWORKPERFREGISTRYTESTSUITE_P_H
