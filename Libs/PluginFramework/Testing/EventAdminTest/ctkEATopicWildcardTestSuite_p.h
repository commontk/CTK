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


#ifndef CTKEATOPICWILDCARDTESTSUITE_P_H
#define CTKEATOPICWILDCARDTESTSUITE_P_H

#include <QObject>
#include <QReadWriteLock>

#include <ctkServiceReference.h>
#include <ctkTestSuiteInterface.h>

#include <service/event/ctkEventHandler.h>

class ctkPluginContext;
struct ctkEventAdmin;

class ctkEATopicWildcardTestHelper : public QObject, public ctkEventHandler
{
  Q_OBJECT
  Q_INTERFACES(ctkEventHandler)

private:

  mutable QReadWriteLock rwlock;
  ctkEvent last;

public Q_SLOTS:

  void handleEvent(const ctkEvent& event);

public:

  ctkEvent clearLastEvent();

  ctkEvent lastEvent() const;

};


class ctkEATopicWildcardTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:

  ctkEATopicWildcardTestSuite(ctkPluginContext* pc, long eventPluginId, bool useSignalSlot);

Q_SIGNALS:

  void syncSignal(const ctkDictionary& event);

private Q_SLOTS:

  void initTestCase();

  void init();
  void cleanup();

  /*
   * Ensures ctkEventAdmin does not deliver an event published on topic "a/b/c"
   * to a ctkEventHandler listening to topic a/b/c/&#42;.
   */
  void testEventDeliveryForWildcardTopic1();

  /*
   * Ensures ctkEventAdmin does not deliver an event published on topic "a/b" to
   * an ctkEventHandler listening to topic a/b/c/&#42;.
   */
  void testEventDeliveryForWildcardTopic2();

  /*
   * Ensures ctkEventAdmin does not deliver an event published on topic "a" to
   * an ctkEventHandler listening to topic a/b/c/&#42;.
   */
  void testEventDeliveryForWildcardTopic3();

  /*
   * Ensures ctkEventAdmin delivers an event published on topic "a/b/c/d" to an
   * ctkEventHandler listening to topic "a/b/c/&#42;".
   */
  void testEventDeliveryForWildcardTopic4();

  /*
   * Ensures ctkEventAdmin delivers an event published on topic "a/b/c/d/e" to
   * an ctkEventHandler listening to topic "a/b/c/&#42;".
   */
  void testEventDeliveryForWildcardTopic5();

  /*
   * Ensures ctkEventAdmin delivers an event published on topic "a/b/c/d/e/f" to
   * an ctkEventHandler listening to topic "a/b/c/&#42;".
   */
  void testEventDeliveryForWildcardTopic6();

  /*
   * Ensures ctkEventAdmin delivers an event published to topics "a/b/c" and
   * "a/b/c/d" to an ctkEventHandler listening to topics "a/b/c" and "a/b/c/&#42;".
   */
  void testEventDeliveryForWildcardTopic7();


private:

  ctkPluginContext* context;
  long eventPluginId;
  bool useSignalSlot;
  ctkEventAdmin* eventAdmin;
  ctkServiceReference reference;
};

#endif // CTKEATOPICWILDCARDTESTSUITE_P_H
