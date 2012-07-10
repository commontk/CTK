/*
 *  ctkNetworkConnectorZeroMQTest.cpp
 *  ctkNetworkConnectorZeroMQTest
 *
 *  Created by Daniele Giunchi on 27/03/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkNetworkConnectorZeroMQ.h>
#include <ctkEventBusManager.h>

#include <QApplication>

using namespace ctkEventBus;

//-------------------------------------------------------------------------
/**
 Class name: ctkObjectCustom
 Custom object needed for testing.
 */
class testObjectCustomForNetworkConnectorZeroMQ : public QObject {
    Q_OBJECT

public:
    /// constructor.
    testObjectCustomForNetworkConnectorZeroMQ();

    /// Return tha var's value.
    int var() {return m_Var;}

public Q_SLOTS:
    /// Test slot that will increment the value of m_Var when an UPDATE_OBJECT event is raised.
    void updateObject();
    void setObjectValue(int v);

Q_SIGNALS:
    void valueModified(int v);
    void objectModified();

private:
    int m_Var; ///< Test var.
};

testObjectCustomForNetworkConnectorZeroMQ::testObjectCustomForNetworkConnectorZeroMQ() : m_Var(0) {
}

void testObjectCustomForNetworkConnectorZeroMQ::updateObject() {
    m_Var++;
}

void testObjectCustomForNetworkConnectorZeroMQ::setObjectValue(int v) {
    m_Var = v;
}


/**
 Class name: ctkNetworkConnectorZeroMQTest
 This class implements the test suite for ctkNetworkConnectorZeroMQ.
 */

//! <title>
//ctkNetworkConnectorZeroMQ
//! </title>
//! <description>
//ctkNetworkConnectorZeroMQ provides the connection with 0MQ library.
//It has been used qxmlrpc library.
//! </description>

class ctkNetworkConnectorZeroMQTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_EventBus = ctkEventBusManager::instance();
        m_NetWorkConnectorZeroMQ = new ctkEventBus::ctkNetworkConnectorZeroMQ();
        m_ObjectTest = new testObjectCustomForNetworkConnectorZeroMQ();
    }

    /// Cleanup tes variables memory allocation.
    void cleanupTestCase() {
        if(m_ObjectTest) {
            delete m_ObjectTest;
            m_ObjectTest = NULL;
        }
        delete m_NetWorkConnectorZeroMQ;
        m_EventBus->shutdown();
    }

    /// Check the existence of the ctkNetworkConnectorZeroMQe singletone creation.
    void ctkNetworkConnectorZeroMQConstructorTest();

    /// Check the existence of the ctkNetworkConnectorZeroMQe singletone creation.
    void ctkNetworkConnectorZeroMQCommunictionTest();

private:
    ctkEventBusManager *m_EventBus; ///< event bus instance
    ctkNetworkConnectorZeroMQ *m_NetWorkConnectorZeroMQ; ///< EventBus test variable instance.
    testObjectCustomForNetworkConnectorZeroMQ *m_ObjectTest;
};

void ctkNetworkConnectorZeroMQTest::ctkNetworkConnectorZeroMQConstructorTest() {
    QVERIFY(m_NetWorkConnectorZeroMQ != NULL);
}


void ctkNetworkConnectorZeroMQTest::ctkNetworkConnectorZeroMQCommunictionTest() {
    QTime dieTime = QTime::currentTime().addSecs(3);
    while(QTime::currentTime() < dieTime) {
       QCoreApplication::processEvents(QEventLoop::AllEvents, 3);
    }
}

CTK_REGISTER_TEST(ctkNetworkConnectorZeroMQTest);
#include "ctkNetworkConnectorZeroMQTest.moc"

