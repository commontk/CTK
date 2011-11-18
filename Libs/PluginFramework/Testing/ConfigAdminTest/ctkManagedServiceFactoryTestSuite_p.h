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


#ifndef CTKMANAGEDSERVICEFACTORYTESTSUITE_P_H
#define CTKMANAGEDSERVICEFACTORYTESTSUITE_P_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>

#include <service/cm/ctkManagedServiceFactory.h>
#include <ctkServiceReference.h>
#include <ctkTestSuiteInterface.h>

struct ctkConfigurationAdmin;
class ctkManagedServiceFactoryTestSuite;

class _ManagedServiceFactoryUpdateTest : public QObject,
    public ctkManagedServiceFactory
{
  Q_OBJECT
  Q_INTERFACES(ctkManagedServiceFactory)

public:

  _ManagedServiceFactoryUpdateTest(ctkManagedServiceFactoryTestSuite* ts);

  void deleted(const QString& pid);
  QString getName();
  void updated(const QString& pid, const ctkDictionary& properties);

private:

  ctkManagedServiceFactoryTestSuite* const ts;
};

class ctkManagedServiceFactoryTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:

  ctkManagedServiceFactoryTestSuite(ctkPluginContext* pc, long cmPluginId);

private Q_SLOTS:

  void init();
  void cleanup();

  void testSamePidManagedServiceFactory();
  void testGeneralManagedServiceFactory();

private:

  ctkPluginContext* context;
  long cmPluginId;
  ctkConfigurationAdmin* cm;
  ctkServiceReference reference;
  int updateCount;
  bool locked;
  QMutex mutex;
  QWaitCondition lock;

  friend class _ManagedServiceFactoryUpdateTest;
};

#endif // CTKMANAGEDSERVICEFACTORYTESTSUITE_P_H
