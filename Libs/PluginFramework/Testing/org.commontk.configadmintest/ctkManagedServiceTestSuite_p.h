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


#ifndef CTKMANAGEDSERVICETESTSUITE_P_H
#define CTKMANAGEDSERVICETESTSUITE_P_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>

#include <service/cm/ctkManagedService.h>
#include <ctkServiceReference.h>
#include <ctkTestSuiteInterface.h>

class ctkManagedServiceTestSuite;
struct ctkConfigurationAdmin;

class _ManagedServiceUpdateTest : public QObject, public ctkManagedService
{
  Q_OBJECT
  Q_INTERFACES(ctkManagedService)

public:

  _ManagedServiceUpdateTest(ctkManagedServiceTestSuite* ts);

  void updated(const ctkDictionary& properties);

private:

  ctkManagedServiceTestSuite* const ts;
};

class ctkManagedServiceTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:

  ctkManagedServiceTestSuite(ctkPluginContext* pc, long cmPluginId);

private Q_SLOTS:

  void init();
  void cleanup();

  void testSamePidManagedService();
  void testGeneralManagedService();

private:

  ctkPluginContext* context;
  long cmPluginId;
  ctkConfigurationAdmin* cm;
  ctkServiceReference reference;
  int updateCount;
  bool locked;
  QMutex mutex;
  QWaitCondition lock;

  friend class _ManagedServiceUpdateTest;
};

#endif // CTKMANAGEDSERVICETESTSUITE_P_H
