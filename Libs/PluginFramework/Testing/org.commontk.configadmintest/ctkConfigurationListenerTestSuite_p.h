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


#ifndef CTKCONFIGURATIONLISTENERTESTSUITE_P_H
#define CTKCONFIGURATIONLISTENERTESTSUITE_P_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>

#include <service/cm/ctkConfigurationListener.h>
#include <ctkTestSuiteInterface.h>

class ctkPluginContext;
struct ctkConfigurationAdmin;
class ctkConfigurationListenerTestSuite;

class _ConfigurationListenerTest : public QObject,
    public ctkConfigurationListener
{
  Q_OBJECT
  Q_INTERFACES(ctkConfigurationListener)

public:

  _ConfigurationListenerTest(ctkConfigurationListenerTestSuite* ts);

  void configurationEvent(const ctkConfigurationEvent& event);

private:

  ctkConfigurationListenerTestSuite* ts;
};

class ctkConfigurationListenerTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:

  ctkConfigurationListenerTestSuite(ctkPluginContext* pc, long cmPluginId);

private Q_SLOTS:

  void init();
  void cleanup();

  void testListener();

private:

  ctkPluginContext* context;
  ctkConfigurationAdmin* cm;
  ctkServiceReference reference;
  bool locked;
  QMutex mutex;
  QWaitCondition lock;
  long cmPluginId;

  _ConfigurationListenerTest* listener;

  friend class _ConfigurationListenerTest;
};

#endif // CTKCONFIGURATIONLISTENERTESTSUITE_P_H
