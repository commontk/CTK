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
  Q_INTERFACES(ctkTestSuiteInterface)

public:

  ctkPluginFrameworkTestSuite(ctkPluginContext* pc);

protected Q_SLOTS:

  void frameworkListener(const ctkPluginFrameworkEvent& fwEvent);
  void pluginListener(const ctkPluginEvent& event);
  void syncPluginListener(const ctkPluginEvent& event);
  void serviceListener(const ctkServiceEvent& event);

private Q_SLOTS:

  void initTestCase();
  void cleanupTestCase();

  // test functions
  void frame005a();
  void frame007a();
  void frame010a();
  void frame018a();
  void frame020a();
  void frame025b();
  void frame030b();
  void frame035b();
  void frame040a();
  void frame042a();
  void frame045a();
  void frame070a();

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
      QSharedPointer<ctkPlugin> pluginX, ctkServiceReference* servX);

  // Check that the expected events have reached the listeners and
  // reset the events
  bool checkListenerEvents(
      const QList<ctkPluginFrameworkEvent>& fwEvts,
      const QList<ctkPluginEvent>& pEvts,
      const QList<ctkServiceEvent>& seEvts);

  // Check that the expected events have reached the listeners and
  // reset the events
  bool checkSyncListenerEvents(
      bool pexp, ctkPluginEvent::Type ptype, QSharedPointer<ctkPlugin> pX,
      ctkServiceReference servX);

  // Check that the expected events have reached the listeners and
  // reset the events
  bool checkSyncListenerEvents(const QList<ctkPluginEvent>& pEvts);

  void clearEvents();

  static int nRunCount;

  QList<ctkPluginEvent> pluginEvents;
  QList<ctkPluginEvent> syncPluginEvents;
  QList<ctkPluginFrameworkEvent> frameworkEvents;
  QList<ctkServiceEvent> serviceEvents;

  int eventDelay;

  ctkPluginContext* pc;

  QSharedPointer<ctkPlugin> p;
  QSharedPointer<ctkPlugin> pA;
  QSharedPointer<ctkPlugin> pD;

};

class ctkServiceListenerPFW : public QObject
{
  Q_OBJECT

public:

  ctkServiceEvent getEvent() const;
  QList<ctkServiceEvent> getEvents() const;
  void clearEvent();

public Q_SLOTS:

  void serviceChanged(const ctkServiceEvent& evt);

private:

  QList<ctkServiceEvent> events;
};

#endif // CTKPLUGINFRAMEWORKTESTSUITE_P_H
