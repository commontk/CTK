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


#ifndef CTKCONFIGURATIONPLUGINTESTSUITE_P_H
#define CTKCONFIGURATIONPLUGINTESTSUITE_P_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>

#include <service/cm/ctkConfigurationPlugin.h>
#include <service/cm/ctkManagedService.h>
#include <ctkTestSuiteInterface.h>

class ctkConfigurationPluginTestSuite;
struct ctkConfigurationAdmin;

class _ConfigurationPluginTest : public QObject, public ctkConfigurationPlugin
{
  Q_OBJECT
  Q_INTERFACES(ctkConfigurationPlugin)

public:

  _ConfigurationPluginTest(const QString& prop = "plugin1");

  void modifyConfiguration(const ctkServiceReference& reference,
                           ctkDictionary& properties);

private:

  const QString prop;
};

class _ManagedServiceCMPluginTest : public QObject, public ctkManagedService
{
  Q_OBJECT
  Q_INTERFACES(ctkManagedService)

public:

  _ManagedServiceCMPluginTest(ctkConfigurationPluginTestSuite* ts);

  void updated(const ctkDictionary& properties);

private:

  ctkConfigurationPluginTestSuite* const ts;
};

class ctkConfigurationPluginTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:

  ctkConfigurationPluginTestSuite(ctkPluginContext* pc, long cmPluginId);
  ~ctkConfigurationPluginTestSuite();

private Q_SLOTS:

  void init();
  void cleanup();

  void testPlugin();
  void testPidSpecificPlugin();
  void testPidSpecificMissPlugin();
  void testRankedPlugin();

private:

  ctkPluginContext* context;
  long cmPluginId;
  ctkConfigurationAdmin* cm;
  ctkServiceReference reference;
  bool locked;
  QMutex mutex;
  QWaitCondition lock;
  bool success;

  friend class _ManagedServiceCMPluginTest;
};

#endif // CTKCONFIGURATIONPLUGINTESTSUITE_P_H
