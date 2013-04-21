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


#ifndef CTKEAPERFTESTSUITE_P_H
#define CTKEAPERFTESTSUITE_P_H

#include "ctkTestSuiteInterface.h"

#include <service/event/ctkEventHandler.h>
#include <ctkServiceRegistration.h>

#include <QDebug>

struct ctkEventAdmin;

class ctkEventAdminPerfTestSuite : public QObject, public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

private:

  ctkPluginContext* pc;
  int pluginId;

  int nSendEvents;
  int nHandlers;

  int nEvent1Handled;
  int nEvent2Handled;

  ctkEventAdmin* eventAdmin;

  QList<ctkEventHandler*> handlers;
  QList<ctkServiceRegistration> handlerRegistrations;

public:

  ctkEventAdminPerfTestSuite(ctkPluginContext* context, int pluginId);

private:

  void addHandlers();
  void removeHandlers();

  void sendEvents();
  void postEvents();

private Q_SLOTS:

  void initTestCase();
  void testSendEvents();
  void testPostEvents();
  void cleanupTestCase();
};

class TestEventHandler : public QObject, public ctkEventHandler
{
  Q_OBJECT
  Q_INTERFACES(ctkEventHandler)
private:
  int& counter;
public:
  TestEventHandler(int& counter);
  void handleEvent(const ctkEvent& );
};

#endif // CTKEAPERFTESTSUITE_P_H
