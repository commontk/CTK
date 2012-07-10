/*
 *  ctkNetworkConnectorTest.cpp
 *  ctkResourcesTest
 *
 *  Created by Daniele Giunchi on 20/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkNetworkConnector.h>

using namespace ctkEventBus;

//------------------------------------------------------------------------------------------
/**
 Class name: testNetworkConnectorCustom
 This class implements the network connector to be tested.
 */
class  testNetworkConnectorCustom : public  ctkNetworkConnector {
    Q_OBJECT

public:
    /// Object constructor.
    testNetworkConnectorCustom();

    /// Create and initialize client
    /*virtual*/ void createClient(const QString hostName, const unsigned int port);

    /// Return the string variable initializated and updated from the data pipe.
    /*virtual*/ void createServer(const unsigned int port);

    /// Allow to send a network request.
    /*virtual*/ void send(const QString event_id, ctkEventArgumentsList *params);

    /// Start the server.
    /*virtual*/ void startListen();

    /// Return connector status.
    QString connectorStatus();

    /// retrieve instance of object
    /*virtual*/ ctkNetworkConnector *clone();

    /// register all the signals and slots
    /*virtual*/ void initializeForEventBus();

private:
    QString m_ConnectorStatus; ///< Test Var.
};

ctkNetworkConnector *testNetworkConnectorCustom::clone() {
    return new testNetworkConnectorCustom();
}

void testNetworkConnectorCustom::initializeForEventBus() {
}

testNetworkConnectorCustom::testNetworkConnectorCustom() : ctkNetworkConnector(), m_ConnectorStatus("") {
     m_Protocol = "FakeProtocol";
}

void testNetworkConnectorCustom::createServer(const unsigned int port) {
    m_ConnectorStatus = "Server Created - Port: ";
    m_ConnectorStatus.append(QString::number(port));
}

void testNetworkConnectorCustom::startListen() {
    m_ConnectorStatus = "Server Listening";
}

void testNetworkConnectorCustom::createClient(const QString hostName, const unsigned int port) {
    m_ConnectorStatus = "Client Created - Host: ";
    m_ConnectorStatus.append(hostName);
    m_ConnectorStatus.append(" Port: ");
    m_ConnectorStatus.append(QString::number(port));
}

void testNetworkConnectorCustom::send(const QString event_id, ctkEventArgumentsList *params) {
    Q_UNUSED(params);
    m_ConnectorStatus = "Event sent with ID: ";
    m_ConnectorStatus.append(event_id);
}

QString testNetworkConnectorCustom::connectorStatus() {
    return m_ConnectorStatus;
}

//------------------------------------------------------------------------------------------

/**
 Class name: ctkNetworkConnectorTest
 This class implements the test suite for ctkNetworkConnector.
 */

 //! <title>
//ctkNetworkConnector
//! </title>
//! <description>
//ctkNetworkConnector is the interface class for client/server objects that
//works over network.
//! </description>


class ctkNetworkConnectorTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_NetworkConnector = new testNetworkConnectorCustom();
    }

    /// Cleanup test variables memory allocation.
    void cleanupTestCase() {
        if(m_NetworkConnector) delete m_NetworkConnector;
    }

    /// ctkNetworkConnector allocation test case.
    void ctkNetworkConnectorAllocationTest();
    /// Test the creation of client and server.
    void ctkNetworkConnectorCreateClientAndServerTest();
    /// test the function that retrive protocol type
    void retrieveProtocolTest();

private:
    ctkNetworkConnector *m_NetworkConnector; ///< Test var.
};

void ctkNetworkConnectorTest::ctkNetworkConnectorAllocationTest() {
    QVERIFY(m_NetworkConnector != NULL);
}

void ctkNetworkConnectorTest::ctkNetworkConnectorCreateClientAndServerTest() {
    QString res;
    res = "Server Created - Port: 8000";
    m_NetworkConnector->createServer(8000);

    testNetworkConnectorCustom *conn = (testNetworkConnectorCustom *)m_NetworkConnector;
    QCOMPARE(conn->connectorStatus(), res);

    res = "Client Created - Host: localhost Port: 8000";
    m_NetworkConnector->createClient("localhost", 8000);
    QCOMPARE(conn->connectorStatus(), res);
}

void ctkNetworkConnectorTest::retrieveProtocolTest() {
    QString res = "FakeProtocol";
    QString check = m_NetworkConnector->protocol();
    QCOMPARE(check, res);
}


CTK_REGISTER_TEST(ctkNetworkConnectorTest);
#include "ctkNetworkConnectorTest.moc"

