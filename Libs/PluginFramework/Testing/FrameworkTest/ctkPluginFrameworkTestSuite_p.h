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

#ifndef CTKPLUGINFRAMEWORKTESTSUITE_P_H
#define CTKPLUGINFRAMEWORKTESTSUITE_P_H

#include <QObject>

#include <ctkPluginFrameworkEvent.h>
#include <ctkPluginEvent.h>
#include <ctkServiceEvent.h>

#include <ctkTestSuiteInterface.h>

class ctkPluginContext;

class ctkPluginFrameworkTestSuite : public QObject,
                                    public ctkTestSuiteInterface
{
  Q_OBJECT

public:

  ctkPluginFrameworkTestSuite(ctkPluginContext* pc);

protected slots:

  void frameworkListener(const ctkPluginFrameworkEvent& fwEvent);
  void pluginListener(const ctkPluginEvent& event);
  void syncPluginListener(const ctkPluginEvent& event);
  void serviceListener(const ctkServiceEvent& event);

private slots:

  void initTestCase();
  void cleanupTestCase();

  // test functions
  void frame005a();
  void frame020a();
  void frame025b();

private:

  ctkPluginEvent getPluginEvent() const;
  ctkPluginEvent getSyncPluginEvent() const;
  ctkPluginFrameworkEvent getFrameworkEvent() const;
  ctkServiceEvent getServiceEvent() const;

  // Check that the expected events have reached the listeners and
  // reset the events
  bool checkListenerEvents(
      bool fwexp, ctkPluginFrameworkEvent::Type fwtype,
      bool pexp, ctkPluginEvent::Type ptype,
      bool sexp, ctkServiceEvent::Type stype,
      ctkPlugin* pluginX, ctkServiceReference* servX);

  // Check that the expected events have reached the listeners and
  // reset the events
  bool checkListenerEvents(
      const QList<ctkPluginFrameworkEvent>& fwEvts,
      const QList<ctkPluginEvent>& pEvts,
      const QList<ctkServiceEvent>& seEvts);

  // Check that the expected events have reached the listeners and
  // reset the events
  bool checkSyncListenerEvents(
      bool pexp, ctkPluginEvent::Type ptype, ctkPlugin* pX,
      ctkServiceReference servX);

  // Check that the expected events have reached the listeners and
  // reset the events
  bool checkSyncListenerEvents(const QList<ctkPluginEvent>& pEvts);

  static int nRunCount;

  QList<ctkPluginEvent> pluginEvents;
  QList<ctkPluginEvent> syncPluginEvents;
  QList<ctkPluginFrameworkEvent> frameworkEvents;
  QList<ctkServiceEvent> serviceEvents;

  int eventDelay;

  ctkPluginContext* pc;
  ctkPlugin* p;

  ctkPlugin* pA;

};

#endif // CTKPLUGINFRAMEWORKTESTSUITE_P_H
