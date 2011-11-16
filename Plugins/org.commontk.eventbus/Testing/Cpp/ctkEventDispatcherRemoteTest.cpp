/*
 *  ctkEventDispatcherRemoteTest.cpp
 *  ctkEventBusTest
 *
 *  Created by Daniele Giunchi on 20/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkEventDispatcherRemote.h>

using namespace ctkEventBus;

/**
 Class name: ctkEventDispatcherRemoteTest
 This class implements the test suite for ctkEventDispatcherRemote.
 */

//! <title>
//ctkEventDispatcherRemote
//! </title>
//! <description>
//ctkEventDispatcherRemote allows dispatching events coming from local
// application to attached observers.
//! </description>

class ctkEventDispatcherRemoteTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_EventDispatcherRemote = new ctkEventDispatcherRemote();
    }

    /// Cleanup test variables memory allocation.
    void cleanupTestCase() {
        delete m_EventDispatcherRemote;
    }

    /// ctkEventDispatcherRemote allocation test case.
    void ctkEventDispatcherRemoteAllocationTest();

    /// ctkEventDispatcherRemote accessors test case: test client and server pointers.
    void ctkEventDispatcherRemoteAccessorsTest();

private:
    ctkEventDispatcherRemote *m_EventDispatcherRemote; ///< Test var.
};

void ctkEventDispatcherRemoteTest::ctkEventDispatcherRemoteAllocationTest() {
    QVERIFY(m_EventDispatcherRemote != NULL);
}

void ctkEventDispatcherRemoteTest::ctkEventDispatcherRemoteAccessorsTest() {
    QVERIFY(m_EventDispatcherRemote->networkConnectorClient() == NULL);
    QVERIFY(m_EventDispatcherRemote->networkConnectorServer() == NULL);
}

CTK_REGISTER_TEST(ctkEventDispatcherRemoteTest);
#include "ctkEventDispatcherRemoteTest.moc"



