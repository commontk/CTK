
/*
 *  ctkBusEventTest.cpp
 *  ctkEventBusTest
 *
 *  Created by Daniele Giunchi on 20/04/10.
 *  Copyright 2010 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkEventDefinitions.h>
#include <ctkBusEvent.h>

using namespace ctkEventBus;

/**
 Class name: ctkBusEventTest
 This class implements the test suite for ctkBusEvent.
 */

//! <title>
//ctkBusEvent
//! </title>
//! <description>
//ctkBusEvent is the object which contain information in a dictionary structure,
//regarding message between classes through ctkEventBus
//! </description>

class ctkBusEventTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_Event = new ctkBusEvent("", ctkDictionary());
    }

    /// Cleanup test variables memory allocation.
    void cleanupTestCase() {
        delete m_Event;
    }

    /// ctkEventDispatcherRemote allocation test case.
    void ctkEventAllocationTest();

    /// test all the accessors of a ctkBusEvent
    void ctkEventAccessorsTest();

private:
    ctkBusEvent *m_Event; ///< Test var.
};

void ctkBusEventTest::ctkEventAllocationTest() {
    QVERIFY(m_Event != NULL);
}

void ctkBusEventTest::ctkEventAccessorsTest() {
    m_Event->setEventType(ctkEventTypeRemote);
    QVERIFY(m_Event->eventType() == ctkEventTypeRemote);
    QVERIFY(m_Event->isEventLocal() != true);

    QString topic = "ctk/level1/level2/level3";
    m_Event->setEventTopic(topic);
    QCOMPARE(m_Event->eventTopic(), topic);

    QVariant var = (*m_Event)[TOPIC];
    QString check = var.toString();
    QCOMPARE(check, topic);

    delete m_Event;
    topic = "ctk/lev1/lev2/lev3";
    m_Event = new ctkBusEvent(topic, ctkEventTypeLocal, ctkSignatureTypeSignal, this, "testmethod");
    QCOMPARE(m_Event->eventTopic(), topic);
}


CTK_REGISTER_TEST(ctkBusEventTest);
#include "ctkBusEventTest.moc"



