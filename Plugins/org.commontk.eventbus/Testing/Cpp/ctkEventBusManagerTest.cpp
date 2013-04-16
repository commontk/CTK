/*
 *  ctkEventBusManagerTest.cpp
 *  ctkEventBusManagerTest
 *
 *  Created by Paolo Quadrani on 27/03/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkEventBusManager.h>
#include <ctkEventDefinitions.h>
#include <ctkNetworkConnector.h>

using namespace ctkEventBus;

//-------------------------------------------------------------------------
/**
 Class name: testObjectCustom
 Custom object needed for testing.
 */
class testObjectCustom : public QObject {
    Q_OBJECT

public:
    /// constructor.
    testObjectCustom();

    /// Return tha var's value.
    int var() {return m_Var;}

    /// register a custom callback
    void registerCustomCallback();

public Q_SLOTS:
    /// Test slot that will increment the value of m_Var when an UPDATE_OBJECT event is raised.
    void updateObject();
    void setObjectValue(int v);
    int returnObjectValue();

Q_SIGNALS:
    void valueModified(int v);
    void objectModified();
    int returnObjectValueSignal();

private:
    int m_Var; ///< Test var.
};

testObjectCustom::testObjectCustom() : m_Var(0) {
}

void testObjectCustom::updateObject() {
    m_Var++;
}

void testObjectCustom::setObjectValue(int v) {
    m_Var = v;
}

int testObjectCustom::returnObjectValue() {
    int value = 5;
    return value;
}

void testObjectCustom::registerCustomCallback() {
    ctkRegisterLocalCallback("ctk/local/custom/topic", this, "updateObject()");
}

//------------------------------------------------------------------------------------------
/**
 Class name: testNetworkConnectorForEventBus
 This class implements the network connector to be tested.
 */
class  testNetworkConnectorForEventBus : public  ctkNetworkConnector {
    Q_OBJECT

public:
    /// Object constructor.
    testNetworkConnectorForEventBus();

    /// Create and initialize client
    /*virtual*/ void createClient(QString hostName, unsigned int port);

    /// Return the string variable initializated and updated from the data pipe.
    /*virtual*/ void createServer(unsigned int port);

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

ctkNetworkConnector *testNetworkConnectorForEventBus::clone() {
    return new testNetworkConnectorForEventBus();
}

void testNetworkConnectorForEventBus::initializeForEventBus() {
}

testNetworkConnectorForEventBus::testNetworkConnectorForEventBus() : ctkNetworkConnector(), m_ConnectorStatus("") {
     m_Protocol = "FakeProtocol";
}

void testNetworkConnectorForEventBus::createServer(unsigned int port) {
    m_ConnectorStatus = "Server Created - Port: ";
    m_ConnectorStatus.append(QString::number(port));
}

void testNetworkConnectorForEventBus::startListen() {
    m_ConnectorStatus = "Server Listening";
}

void testNetworkConnectorForEventBus::createClient(QString hostName, unsigned int port) {
    m_ConnectorStatus = "Client Created - Host: ";
    m_ConnectorStatus.append(hostName);
    m_ConnectorStatus.append(" Port: ");
    m_ConnectorStatus.append(QString::number(port));
}

void testNetworkConnectorForEventBus::send(const QString event_id, ctkEventArgumentsList *params) {
    Q_UNUSED(params);
    m_ConnectorStatus = "Event sent with ID: ";
    m_ConnectorStatus.append(event_id);
}

QString testNetworkConnectorForEventBus::connectorStatus() {
    return m_ConnectorStatus;
}
//-------------------------------------------------------------------------

/**
 Class name: ctkEventBusManagerTest
 This class implements the test suite for ctkEventBusManager.
 */

//! <title>
//ctkEventBusManager
//! </title>
//! <description>
//ctkEventBusManager provides the access point of the Communication Bus for ctk framework and
//allows dispatching events coming from local application to attached observers.
//It provides APIs to add a new event property (observer or event) to the event bus hash or
//to remove the event property from the event bus hash.
//It also provides APIs to add or remove observer to the events and to register custom signals
//use by objects to raise them events.
//
//Sender has to:
//- Register a signal (with argument if necessary)
//
//Observer has to:
//- Register the callback that will be called when event will be notified (with argument if necessary)
// with the same Event ID used by the sender
//
//The method "notifyEventLocal(QString topic, ctkEventArgumentsList *argList)" accept a string and a list
//of QGenericArgument: to set QGenericArgument use the ctkEventArgument() macros.
//ctkEventArgument() takes a type name and a const reference of that type.
//! </description>

class ctkEventBusManagerTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_EventBus = ctkEventBusManager::instance();
        m_ObjTestObserver = new testObjectCustom();
        m_ObjTestObserver2 = new testObjectCustom();
    }

    /// Cleanup tes variables memory allocation.
    void cleanupTestCase() {
        delete m_ObjTestObserver;
        delete m_ObjTestObserver2;
        m_EventBus->shutdown();
    }

    /// Check the existence of the ctkEventBusManager singletone creation.
    void ctkEventBusManagerConstructorTest();

    /// Check the event observing registration and notification.
    void eventBusRegistrationNotificationTest();

    /// Check the event observing registration and notification passing one argument.
    void eventBusWithArgumentTest();

    /// Check the event observing registration and notification returning one argument.
    void eventBusWithReturnArgumentTest();

    /// Event notification benchmarks.
    void eventNotificationBenchmarkTest();

    /// Test eventbus with remote connection (xmlrpc test)
    void remoteConnectionTest();

    /// test plugNetworkConnector
    void plugNetworkConnectorTest();

    /// test method for check if the signal is present.
    void isLocalSignalPresentTest();

    /// test registration reversing the order of signal and callback
    void reverseOrderRegistrationTest();

private:
    testObjectCustom *m_ObjTestObserver; ///< Test variable.
    testObjectCustom *m_ObjTestObserver2; ///< Test variable.
    ctkEventBusManager *m_EventBus; ///< EventBus test variable instance.
};

void ctkEventBusManagerTest::ctkEventBusManagerConstructorTest() {
    QVERIFY(m_EventBus != NULL);
}

void ctkEventBusManagerTest::eventBusRegistrationNotificationTest() {
    int status = m_ObjTestObserver->var();
    QVERIFY(status == 0);

    // Create new Event ID used for callback and event notification.
    QString updateID = "ctk/local/eventBus/globalUpdate";
    ctkRegisterLocalCallback(updateID, m_ObjTestObserver, "updateObject()");

    // Register also the second test observer to the global update event
    ctkRegisterLocalCallback(updateID, m_ObjTestObserver2, "updateObject()");

    // Notify the update event... (event logging is disabled)
    m_EventBus->notifyEvent("ctk/local/eventBus/globalUpdate");

    status = m_ObjTestObserver->var();
    QVERIFY(status == 1);

    // Notify again but with event logging enabled...
    m_EventBus->enableEventLogging();
    m_EventBus->notifyEvent(updateID);

    // ... and now filter events only with ID == "CUSTOM_SIGNAL"
    //ctkId customID = idProvider->createNewId("CUSTOM_SIGNAL");
    QString customID = "CUSTOM_SIGNAL";

    // ...and enable event logging for that id.
    m_EventBus->logEventTopic(customID);

    // Previous ID notification should be skipped by logger (so only one log in the console)
    m_EventBus->notifyEvent(updateID);
}

 void ctkEventBusManagerTest::eventBusWithArgumentTest() {
    testObjectCustom *ObjTestSender = new testObjectCustom();
    ObjTestSender->setObjectValue(52);

    // Create new Event ID used for callback and event notification.
    QString setValueID = "SETVALUE_SIGNAL";
    ctkRegisterLocalSignal(setValueID, ObjTestSender, "valueModified(int)");

    // Register the callback to update the object custom:
    ctkRegisterLocalCallback(setValueID, m_ObjTestObserver, "setObjectValue(int)");
    // Register also the second observer...
    ctkRegisterLocalCallback(setValueID, m_ObjTestObserver2, "setObjectValue(int)");

    //! <snippet>
    ctkEventArgumentsList list;
    list.append(ctkEventArgument(int, ObjTestSender->var()));

    m_EventBus->notifyEvent(setValueID, ctkEventTypeLocal, &list);
    //! </snippet>
    int status = m_ObjTestObserver->var();
    QVERIFY(status == ObjTestSender->var());
    delete ObjTestSender;
    ctkEventBusManager::instance()->removeSignal(ObjTestSender, setValueID);
}

void ctkEventBusManagerTest::eventBusWithReturnArgumentTest() {
    testObjectCustom *ObjTestSender = new testObjectCustom();
    ObjTestSender->setObjectValue(52);

    // Create new Event ID used for callback and event notification.
    QString returnValueID = "RETURNVALUE_SIGNAL";
    ctkRegisterLocalSignal(returnValueID, ObjTestSender, "returnObjectValueSignal()");
    ctkRegisterLocalCallback(returnValueID, ObjTestSender, "returnObjectValue()");


    //Notify event with return argument
    int returnValue = 0;
    ctkGenericReturnArgument ret_val = ctkEventReturnArgument(int,returnValue);
    m_EventBus->notifyEvent(returnValueID, ctkEventTypeLocal, NULL, &ret_val);

    QVERIFY(returnValue == 5);
    delete ObjTestSender;
}


void ctkEventBusManagerTest::eventNotificationBenchmarkTest() {
    QString updateID = "ctk/local/eventBus/globalUpdate";
    m_EventBus->logAllEvents();

    QBENCHMARK {
        m_EventBus->notifyEvent(updateID);
    }
}

void ctkEventBusManagerTest::remoteConnectionTest() {
    m_EventBus->createServer("XMLRPC", 8000);
    m_EventBus->startListen();

    m_EventBus->createClient("XMLRPC", "localhost", 8000);

    //create list to send from the client
    //first parameter is a list which contains event properties
    QVariantList eventParameters;
    eventParameters.append("ctk/local/eventBus/globalUpdate");
    eventParameters.append(ctkEventTypeLocal);
    eventParameters.append(ctkSignatureTypeCallback);
    eventParameters.append("updateObject()");

    QVariantList dataParameters;

    ctkEventArgumentsList listToSend;
    listToSend.append(ctkEventArgument(QVariantList, eventParameters));
    listToSend.append(ctkEventArgument(QVariantList, dataParameters));

    //with eventbus
    QString topic = "ctk/remote/eventBus/comunication/send/xmlrpc";
    m_EventBus->notifyEvent(topic, ctkEventTypeRemote , &listToSend);

    QTime dieTime = QTime::currentTime().addSecs(3);
    while(QTime::currentTime() < dieTime) {
       QCoreApplication::processEvents(QEventLoop::AllEvents, 3);
    }
}

void ctkEventBusManagerTest::plugNetworkConnectorTest() {
    m_EventBus->plugNetworkConnector("CUSTOM_CONNECTOR", NULL);
}

void ctkEventBusManagerTest::isLocalSignalPresentTest() {
    QVERIFY(m_EventBus->isLocalSignalPresent("ctk/invalid/signal") == false);
}

void ctkEventBusManagerTest::reverseOrderRegistrationTest() {
    testObjectCustom *ObjTestSender = new testObjectCustom();
    int startValue = 42;
    m_ObjTestObserver->setObjectValue(startValue);

    //register a custom callback
    m_ObjTestObserver->registerCustomCallback();

    //register custom signal
    ctkRegisterLocalSignal("ctk/local/custom/topic", ObjTestSender, "objectModified()");

    //notify event
    m_EventBus->notifyEvent("ctk/local/custom/topic");

    startValue++; //notify will update the value adding 1
    int check = m_ObjTestObserver->var();
    QVERIFY(startValue == check);
    delete ObjTestSender;
}


CTK_REGISTER_TEST(ctkEventBusManagerTest);
#include "ctkEventBusManagerTest.moc"
