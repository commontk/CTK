/*
 *  ctkEventDispatcherTest.cpp
 *  ctkEventBusTest
 *
 *  Created by Daniele Giunchi on 22/09/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkEventBusManager.h>

using namespace ctkEventBus;

//-------------------------------------------------------------------------
/**
 Class name: ctkObjectCustom
 Custom object needed for testing.
 */
class testObjectCustomForDispatcher : public QObject {
    Q_OBJECT

public:
    /// constructor.
    testObjectCustomForDispatcher();

    /// Return the var's value.
    int var() {return m_Var;}

    /// register a custom callback
    void registerCustomCallback();

public Q_SLOTS:
    /// Test slot that will increment the value of m_Var when an UPDATE_OBJECT event is raised.
    void updateObject();
    void updateObject2();
    void setObjectValue(int v);

Q_SIGNALS:
    void valueModify(int v);
    void objectModify();

private:
    int m_Var; ///< Test var.
};

testObjectCustomForDispatcher::testObjectCustomForDispatcher() : QObject(), m_Var(0) {
}

void testObjectCustomForDispatcher::updateObject() {
    m_Var++;
}

void testObjectCustomForDispatcher::updateObject2() {

}

void testObjectCustomForDispatcher::setObjectValue(int v) {
    m_Var = v;
}

void testObjectCustomForDispatcher::registerCustomCallback() {
    ctkRegisterLocalCallback("ctk/local/custom/topic", this, "updateObject()");
}


//-------------------------------------------------------------------------


/**
 Class name: ctkEventDispatcherTest
 This class implements the test suite for ctkEventDispatcher.
 */

//! <title>
//ctkEventDispatcher
//! </title>
//! <description>
//ctkEventDispatcher allows dispatching events coming from local application
//to attached observers.
//! </description>

class ctkEventDispatcherTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_ObjTestObserver = new testObjectCustomForDispatcher;
        m_ObjTestObserver->setObjectName("TestObserver");
        m_EventDispatcher = new ctkEventDispatcher;
        m_EventDispatcher->setObjectName("EventDispatcher");
    }

    /// Cleanup test variables memory allocation.
    void cleanupTestCase() {
        delete m_ObjTestObserver;
        delete m_EventDispatcher;
    }

    /// ctkEventDispatcher allocation test case.
    void ctkEventDispatcherAllocationTest();
    /// Test the add and remove observer.
    void ctkEventDispatcherAddAndRemoveObserverAndNotifyEventTest();
    /// Test the add and remove signal.
    void ctkEventDispatcherRegisterAndRemoveSignalAndNotifyEventTest();
    /// Test if the signal is present after registration
    void isSignalPresentTest();
    /// Remove observer given a pointer.
    void removeObserverTest();
    /// Remove item from the dispatcher.
    void removeItemTest();
    /// Remove signal given a pointer.
    void removeSignalTest();
    /// test if the local signal is present
    void isLocalSignalPresentTest();
    /// reverse order registration test
    void reverseOrderRegistrationTest();

private:
    ctkEventDispatcher *m_EventDispatcher; ///< Test var.
    testObjectCustomForDispatcher *m_ObjTestObserver; ///< Test Object var
};

void ctkEventDispatcherTest::ctkEventDispatcherAllocationTest() {
    QVERIFY(m_EventDispatcher != NULL);
}

void ctkEventDispatcherTest::ctkEventDispatcherAddAndRemoveObserverAndNotifyEventTest() {
    // Create new Event ID used for callback and event notification.
    QString updateID = "ctk/local/dispatcherTest/update";

    ctkBusEvent *properties = new ctkBusEvent(updateID, ctkDictionary());
    (*properties)[TYPE] = ctkEventTypeLocal;
    (*properties)[SIGTYPE] = ctkSignatureTypeSignal;
    QVariant var;
    var.setValue((QObject*)m_ObjTestObserver);
    (*properties)[OBJECT] = var;
    (*properties)[SIGNATURE] = "objectModify()";
    QVERIFY(m_EventDispatcher->registerSignal(*properties));

    ctkBusEvent *propCallback = new ctkBusEvent(updateID, ctkDictionary());
    (*propCallback)[TYPE] = ctkEventTypeLocal;
    (*propCallback)[SIGTYPE] = ctkSignatureTypeCallback;
    QVariant varobserver;
    varobserver.setValue((QObject*)m_ObjTestObserver);
    (*propCallback)[OBJECT] = varobserver;
    (*propCallback)[SIGNATURE] = "updateObject()";
    QVERIFY(m_EventDispatcher->addObserver(*propCallback));
}

void ctkEventDispatcherTest::ctkEventDispatcherRegisterAndRemoveSignalAndNotifyEventTest() {
    QString updateID = "ctk/local/dispatcherTest/update";

    ctkBusEvent *properties = new ctkBusEvent(updateID, ctkDictionary());
    (*properties)[TYPE] = ctkEventTypeLocal;
    (*properties)[SIGTYPE] = ctkSignatureTypeSignal;
    QVariant var;
    var.setValue((QObject*)m_ObjTestObserver);
    (*properties)[OBJECT] = var;
    (*properties)[SIGNATURE] = "objectModify()";
    QVERIFY(m_EventDispatcher->removeSignal(*properties));

    QVERIFY(m_EventDispatcher->registerSignal(*properties));

    // Register the callback to update the object custom:
    ctkBusEvent *propCallback = new ctkBusEvent(updateID, ctkDictionary());
    (*propCallback)[TYPE] = ctkEventTypeLocal;
    (*propCallback)[SIGTYPE] = ctkSignatureTypeCallback;
    QVariant varobserver;
    varobserver.setValue((QObject*)m_ObjTestObserver);
    (*propCallback)[OBJECT] = varobserver;
    (*propCallback)[SIGNATURE] = "updateObject()";
    QVERIFY(m_EventDispatcher->addObserver(*propCallback));
}

void ctkEventDispatcherTest::isSignalPresentTest() {
    QString updateID = "ctk/local/dispatcherTest/update";
    // Register the callback to update the object custom:
    QVERIFY(m_EventDispatcher->isLocalSignalPresent(updateID));
}

void ctkEventDispatcherTest::removeObserverTest() {
    QString updateID = "ctk/local/dispatcherTest/update";

    // remove the observer from the updateID topics...
    QVERIFY(m_EventDispatcher->removeObserver(m_ObjTestObserver, updateID));

    // Add again the test object as observer...
    ctkBusEvent *propCallback = new ctkBusEvent(updateID, ctkDictionary());
    (*propCallback)[TYPE] = ctkEventTypeLocal;
    (*propCallback)[SIGTYPE] = ctkSignatureTypeCallback;
    QVariant varobserver;
    varobserver.setValue((QObject*)m_ObjTestObserver);
    (*propCallback)[OBJECT] = varobserver;
    (*propCallback)[SIGNATURE] = "updateObject()";
    QVERIFY(m_EventDispatcher->addObserver(*propCallback));

    // remove the observer from all the topics...
    QVERIFY(m_EventDispatcher->removeObserver(m_ObjTestObserver, ""));
}

void ctkEventDispatcherTest::removeItemTest() {
    QString updateID = "ctk/local/dispatcherTest/update";


    // Add again the test object as observer...
    ctkBusEvent *propCallback = new ctkBusEvent(updateID, ctkDictionary());
    (*propCallback)[TYPE] = ctkEventTypeLocal;
    (*propCallback)[SIGTYPE] = ctkSignatureTypeCallback;
    QVariant varobserver;
    varobserver.setValue((QObject*)m_ObjTestObserver);
    (*propCallback)[OBJECT] = varobserver;
    (*propCallback)[SIGNATURE] = "updateObject()";
    QVERIFY(m_EventDispatcher->addObserver(*propCallback));

    // remove the observer from all the topics...

    ctkBusEvent *propCallback2 = new ctkBusEvent(updateID, ctkDictionary());
    (*propCallback2)[TYPE] = ctkEventTypeLocal;
    (*propCallback2)[SIGTYPE] = ctkSignatureTypeCallback;
    (*propCallback2)[OBJECT] = varobserver;
    (*propCallback2)[SIGNATURE] = "updateObject2()";
    QVERIFY(m_EventDispatcher->addObserver(*propCallback2));

    //this will be removed
    QVERIFY(m_EventDispatcher->removeObserver(*propCallback));

    // this will be removed and will cover the code of iterator which simple go to the next element
    QVERIFY(m_EventDispatcher->removeObserver(*propCallback2));

}

void ctkEventDispatcherTest::removeSignalTest() {
    QString updateID = "ctk/local/dispatcherTest/update";

    // remove the signal from the updateID topics...
    // ...but don't need to make a qt disconnect because all observer has been disconnected already on previous test case.
    QVERIFY(m_EventDispatcher->removeSignal(m_ObjTestObserver, updateID, false));

    ctkBusEvent *properties = new ctkBusEvent(updateID, ctkDictionary());
    (*properties)[TYPE] = ctkEventTypeLocal;
    (*properties)[SIGTYPE] = ctkSignatureTypeSignal;
    QVariant var;
    var.setValue((QObject*)m_ObjTestObserver);
    (*properties)[OBJECT] = var;
    (*properties)[SIGNATURE] = "objectModify()";
    QVERIFY(m_EventDispatcher->registerSignal(*properties));

    QVERIFY(m_EventDispatcher->removeSignal(m_ObjTestObserver, "", false));
}


void ctkEventDispatcherTest::reverseOrderRegistrationTest() {
    QString updateID = "ctk/local/dispatcherTest/custom";

    // Register the callback to update the object custom:
    ctkBusEvent *propCallback = new ctkBusEvent(updateID, ctkDictionary());
    (*propCallback)[TYPE] = ctkEventTypeLocal;
    (*propCallback)[SIGTYPE] = ctkSignatureTypeCallback;
    QVariant varobserver;
    varobserver.setValue((QObject*)m_ObjTestObserver);
    (*propCallback)[OBJECT] = varobserver;
    (*propCallback)[SIGNATURE] = "updateObject()";
    QVERIFY(m_EventDispatcher->addObserver(*propCallback));

    ctkBusEvent *properties = new ctkBusEvent(updateID, ctkDictionary());
    (*properties)[TYPE] = ctkEventTypeLocal;
    (*properties)[SIGTYPE] = ctkSignatureTypeSignal;
    QVariant var;
    var.setValue((QObject*)m_ObjTestObserver);
    (*properties)[OBJECT] = var;
    (*properties)[SIGNATURE] = "objectModify()";
    QVERIFY(m_EventDispatcher->registerSignal(*properties));
}

void ctkEventDispatcherTest::isLocalSignalPresentTest() {
    QVERIFY(m_EventDispatcher->isLocalSignalPresent("ctk/wrong/topic") == false);
}

//CTK_REGISTER_TEST(ctkEventDispatcherTest);
#include "ctkEventDispatcherTest.moc"
