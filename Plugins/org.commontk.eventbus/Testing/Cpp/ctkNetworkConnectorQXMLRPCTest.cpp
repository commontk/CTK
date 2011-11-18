/*
 *  ctkNetworkConnectorQXMLRPCTest.cpp
 *  ctkNetworkConnectorQXMLRPCTest
 *
 *  Created by Daniele Giunchi on 27/03/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkNetworkConnectorQXMLRPC.h>
#include <ctkEventBusManager.h>

#include <QApplication>

using namespace ctkEventBus;

//-------------------------------------------------------------------------
/**
 Class name: ctkObjectCustom
 Custom object needed for testing.
 */
class testObjectCustomForNetworkConnectorXMLRPC : public QObject {
    Q_OBJECT

public:
    /// constructor.
    testObjectCustomForNetworkConnectorXMLRPC();

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

testObjectCustomForNetworkConnectorXMLRPC::testObjectCustomForNetworkConnectorXMLRPC() : m_Var(0) {
}

void testObjectCustomForNetworkConnectorXMLRPC::updateObject() {
    m_Var++;
}

void testObjectCustomForNetworkConnectorXMLRPC::setObjectValue(int v) {
    m_Var = v;
}


/**
 Class name: ctkNetworkConnectorQXMLRPCTest
 This class implements the test suite for ctkNetworkConnectorQXMLRPC.
 */

//! <title>
//ctkNetworkConnectorQXMLRPC
//! </title>
//! <description>
//ctkNetworkConnectorQXMLRPC provides the connection with xml-rpc protocol.
//It has been used qxmlrpc library.
//! </description>

class ctkNetworkConnectorQXMLRPCTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_EventBus = ctkEventBusManager::instance();
        m_NetWorkConnectorQXMLRPC = new ctkEventBus::ctkNetworkConnectorQXMLRPC();
        m_ObjectTest = new testObjectCustomForNetworkConnectorXMLRPC();
    }

    /// Cleanup tes variables memory allocation.
    void cleanupTestCase() {
        if(m_ObjectTest) {
            delete m_ObjectTest;
            m_ObjectTest = NULL;
        }
        delete m_NetWorkConnectorQXMLRPC;
        m_EventBus->shutdown();
    }

    /// Check the existence of the ctkNetworkConnectorQXMLRPCe singletone creation.
    void ctkNetworkConnectorQXMLRPCConstructorTest();

    /// Check the existence of the ctkNetworkConnectorQXMLRPCe singletone creation.
    void ctkNetworkConnectorQXMLRPCCommunictionTest();

private:
    ctkEventBusManager *m_EventBus; ///< event bus instance
    ctkNetworkConnectorQXMLRPC *m_NetWorkConnectorQXMLRPC; ///< EventBus test variable instance.
    testObjectCustomForNetworkConnectorXMLRPC *m_ObjectTest;
};

void ctkNetworkConnectorQXMLRPCTest::ctkNetworkConnectorQXMLRPCConstructorTest() {
    QVERIFY(m_NetWorkConnectorQXMLRPC != NULL);
}


void ctkNetworkConnectorQXMLRPCTest::ctkNetworkConnectorQXMLRPCCommunictionTest() {
    m_NetWorkConnectorQXMLRPC->createServer(8000);
    m_NetWorkConnectorQXMLRPC->startListen();


    // Register callback (done by the remote object).
    ctkRegisterLocalCallback("ctk/local/eventBus/globalUpdate", m_ObjectTest, "updateObject()");

    //m_EventBus->createClient("XMLRPC", "localhost", 8000);
    m_NetWorkConnectorQXMLRPC->createClient("localhost", 8000);

    //create list to send from the client
    //first parameter is a list which contains event prperties


    QVariantList eventParameters;
    eventParameters.append("ctk/local/eventBus/globalUpdate");
    eventParameters.append(ctkEventTypeLocal);
    eventParameters.append(ctkSignatureTypeCallback);
    eventParameters.append("updateObject()");

    QVariantList dataParameters;

    ctkEventArgumentsList listToSend;
    listToSend.append(ctkEventArgument(QVariantList, eventParameters));
    listToSend.append(ctkEventArgument(QVariantList, dataParameters));

    m_NetWorkConnectorQXMLRPC->send("ctk/remote/eventBus/comunication/send/xmlrpc", &listToSend);


    QTime dieTime = QTime::currentTime().addSecs(3);
    while(QTime::currentTime() < dieTime) {
       QCoreApplication::processEvents(QEventLoop::AllEvents, 3);
    }
}

CTK_REGISTER_TEST(ctkNetworkConnectorQXMLRPCTest);
#include "ctkNetworkConnectorQXMLRPCTest.moc"

