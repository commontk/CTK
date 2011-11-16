/*
 *  ctkEventDispatcherLocalTest.cpp
 *  ctkEventBusTest
 *
 *  Created by Daniele Giunchi on 20/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkEventDispatcherLocal.h>
#include <ctkBusEvent.h>

using namespace ctkEventBus;

//-------------------------------------------------------------------------
/**
 Class name: ctkObjectCustom
 Custom object needed for testing.
 */
class testObjectCustomForDispatcherLocal : public QObject {
    Q_OBJECT

public:
    /// constructor.
    testObjectCustomForDispatcherLocal();

    /// Return the var's value.
    int var() {return m_Var;}

public Q_SLOTS:
    /// Test slot that will increment the value of m_Var when an UPDATE_OBJECT event is raised.
    // no return value
    void setObjectValue0(){};
    void setObjectValue1(int v1){QVERIFY(v1 != 0);};
    void setObjectValue2(int v1, int v2){QVERIFY(v1 != 0);QVERIFY(v2 != 0);};
    void setObjectValue3(int v1, int v2, int v3){QVERIFY(v1 != 0);QVERIFY(v2 != 0);QVERIFY(v3 != 0);};
    void setObjectValue4(int v1, int v2, int v3, int v4){QVERIFY(v1 != 0);QVERIFY(v2 != 0);QVERIFY(v3 != 0);QVERIFY(v4 != 0);};
    void setObjectValue5(int v1, int v2, int v3, int v4, int v5){QVERIFY(v1 != 0);QVERIFY(v2 != 0);QVERIFY(v3 != 0);QVERIFY(v4 != 0);QVERIFY(v5 != 0);};
    void setObjectValue6(int v1, int v2, int v3, int v4, int v5, int v6){QVERIFY(v1 != 0);QVERIFY(v2 != 0);QVERIFY(v3 != 0);QVERIFY(v4 != 0);QVERIFY(v5 != 0);QVERIFY(v6 != 0);};
    void setObjectValue7(int v1, int v2, int v3, int v4, int v5, int v6, int v7){QVERIFY(v1 != 0);QVERIFY(v2 != 0);QVERIFY(v3 != 0);QVERIFY(v4 != 0);QVERIFY(v5 != 0);QVERIFY(v6 != 0);QVERIFY(v7 != 0);};
    void setObjectValue8(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8){QVERIFY(v1 != 0);QVERIFY(v2 != 0);QVERIFY(v3 != 0);QVERIFY(v4 != 0);QVERIFY(v5 != 0);QVERIFY(v6 != 0);QVERIFY(v7 != 0);QVERIFY(v8 != 0);};
    void setObjectValue9(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9){QVERIFY(v1 != 0);QVERIFY(v2 != 0);QVERIFY(v3 != 0);QVERIFY(v4 != 0);QVERIFY(v5 != 0);QVERIFY(v6 != 0);QVERIFY(v7 != 0);QVERIFY(v8 != 0);QVERIFY(v9 != 0);};
    void setObjectValue10(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10){QVERIFY(v1 != 0);QVERIFY(v2 != 0);QVERIFY(v3 != 0);QVERIFY(v4 != 0);QVERIFY(v5 != 0);QVERIFY(v6 != 0);QVERIFY(v7 != 0);QVERIFY(v8 != 0);QVERIFY(v9 != 0);QVERIFY(v10 != 0);};

    // with return value
    int setObjectValue0WithReturnValue(){ return 0;};
    int setObjectValue1WithReturnValue(int v1){return v1;};
    int setObjectValue2WithReturnValue(int v1, int v2){return v1 + v2;};
    int setObjectValue3WithReturnValue(int v1, int v2, int v3){return v1 + v2 + v3;};
    int setObjectValue4WithReturnValue(int v1, int v2, int v3, int v4){return v1 + v2 + v3 + v4;};
    int setObjectValue5WithReturnValue(int v1, int v2, int v3, int v4, int v5){return v1 + v2 + v3 + v4 +v5;};
    int setObjectValue6WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6){return v1 + v2 + v3 + v4 +v5 + v6;};
    int setObjectValue7WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7){return v1 + v2 + v3 + v4 +v5 + v6 + v7;};
    int setObjectValue8WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8){return v1 + v2 + v3 + v4 +v5 + v6 + v7 + v8;};
    int setObjectValue9WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9){return v1 + v2 + v3 + v4 +v5 + v6 + v7 + v8 + v9;};
    int setObjectValue10WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10){return v1 + v2 + v3 + v4 +v5 + v6 + v7 + v8 + v9 + v10;};

Q_SIGNALS:
    void signalSetObjectValue0();
    void signalSetObjectValue1(int v1);
    void signalSetObjectValue2(int v1, int v2);
    void signalSetObjectValue3(int v1, int v2, int v3);
    void signalSetObjectValue4(int v1, int v2, int v3, int v4);
    void signalSetObjectValue5(int v1, int v2, int v3, int v4, int v5);
    void signalSetObjectValue6(int v1, int v2, int v3, int v4, int v5, int v6);
    void signalSetObjectValue7(int v1, int v2, int v3, int v4, int v5, int v6, int v7);
    void signalSetObjectValue8(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8);
    void signalSetObjectValue9(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);
    void signalSetObjectValue10(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10);

    int signalSetObjectValue0WithReturnValue();
    int signalSetObjectValue1WithReturnValue(int v1);
    int signalSetObjectValue2WithReturnValue(int v1, int v2);
    int signalSetObjectValue3WithReturnValue(int v1, int v2, int v3);
    int signalSetObjectValue4WithReturnValue(int v1, int v2, int v3, int v4);
    int signalSetObjectValue5WithReturnValue(int v1, int v2, int v3, int v4, int v5);
    int signalSetObjectValue6WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6);
    int signalSetObjectValue7WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7);
    int signalSetObjectValue8WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8);
    int signalSetObjectValue9WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);
    int signalSetObjectValue10WithReturnValue(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10);

private:
    int m_Var; ///< Test var.
};

testObjectCustomForDispatcherLocal::testObjectCustomForDispatcherLocal() : m_Var(0) {
}

//-------------------------------------------------------------------------


/**
 Class name: ctkEventDispatcherLocalTest
 This class implements the test suite for ctkEventDispatcherlocal/
 */

//! <title>
//ctkEventDispatcherLocal
//! </title>
//! <description>
//ctkEventDispatcherLocal allows dispatching events coming from local
//application to attached observers.
//! </description>

class ctkEventDispatcherLocalTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_ObjTest = new testObjectCustomForDispatcherLocal;
        m_EventDispatcherLocal =new ctkEventBus::ctkEventDispatcherLocal;

    }

    /// Cleanup test variables memory allocation.
    void cleanupTestCase() {
        delete m_ObjTest;
        delete m_EventDispatcherLocal;
    }

    /// ctkEventDispatcherLocal allocation test case.
    void ctkEventDispatcherLocalAllocationTest();

    /// notify event test which cover all the possibilities in terms of arguments
    void notifyEventWithoutReturnValueTest();

    /// notify event test which cover all the possibilities in terms of arguments with returned value
    void notifyEventWitReturnValueTest();

private:
    testObjectCustomForDispatcherLocal *m_ObjTest; ///< Test Object var
    ctkEventDispatcherLocal *m_EventDispatcherLocal; ///< Test var.
};

void ctkEventDispatcherLocalTest::ctkEventDispatcherLocalAllocationTest() {
    QVERIFY(m_EventDispatcherLocal != NULL);
}

void ctkEventDispatcherLocalTest::notifyEventWithoutReturnValueTest() {

    //test with events which returns no value
    QString topic0 = "ctk/local/setObjectValue0";
    QString topic1 = "ctk/local/setObjectValue1";
    QString topic2 = "ctk/local/setObjectValue2";
    QString topic3 = "ctk/local/setObjectValue3";
    QString topic4 = "ctk/local/setObjectValue4";
    QString topic5 = "ctk/local/setObjectValue5";
    QString topic6 = "ctk/local/setObjectValue6";
    QString topic7 = "ctk/local/setObjectValue7";
    QString topic8 = "ctk/local/setObjectValue8";
    QString topic9 = "ctk/local/setObjectValue9";
    QString topic10 = "ctk/local/setObjectValue10";

    //signal
    ctkBusEvent *propSignal0 = new ctkBusEvent(topic0, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue0()");
    m_EventDispatcherLocal->registerSignal(*propSignal0);

    ctkBusEvent *propSignal1 = new ctkBusEvent(topic1, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue1(int)");
    m_EventDispatcherLocal->registerSignal(*propSignal1);

    ctkBusEvent *propSignal2 = new ctkBusEvent(topic2, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue2(int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal2);

    ctkBusEvent *propSignal3 = new ctkBusEvent(topic3, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue3(int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal3);

    ctkBusEvent *propSignal4 = new ctkBusEvent(topic4, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue4(int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal4);

    ctkBusEvent *propSignal5 = new ctkBusEvent(topic5, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue5(int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal5);

    ctkBusEvent *propSignal6 = new ctkBusEvent(topic6, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue6(int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal6);

    ctkBusEvent *propSignal7 = new ctkBusEvent(topic7, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue7(int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal7);

    ctkBusEvent *propSignal8 = new ctkBusEvent(topic8, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue8(int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal8);

    ctkBusEvent *propSignal9 = new ctkBusEvent(topic9, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue9(int,int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal9);

    ctkBusEvent *propSignal10 = new ctkBusEvent(topic10, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue10(int,int,int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal10);


    //callback
    ctkBusEvent *propCallback0 = new ctkBusEvent(topic0, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue0()");
    m_EventDispatcherLocal->addObserver(*propCallback0);

    ctkBusEvent *propCallback1 = new ctkBusEvent(topic1, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue1(int)");
    m_EventDispatcherLocal->addObserver(*propCallback1);

    ctkBusEvent *propCallback2 = new ctkBusEvent(topic2, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue2(int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback2);

    ctkBusEvent *propCallback3 = new ctkBusEvent(topic3, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue3(int,int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback3);

    ctkBusEvent *propCallback4 = new ctkBusEvent(topic4, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue4(int,int,int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback4);

    ctkBusEvent *propCallback5 = new ctkBusEvent(topic5, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue5(int,int,int,int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback5);

    ctkBusEvent *propCallback6 = new ctkBusEvent(topic6, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue6(int,int,int,int,int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback6);

    ctkBusEvent *propCallback7 = new ctkBusEvent(topic7, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue7(int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback7);

    ctkBusEvent *propCallback8 = new ctkBusEvent(topic8, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue8(int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback8);

    ctkBusEvent *propCallback9 = new ctkBusEvent(topic9, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue9(int,int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback9);

    ctkBusEvent *propCallback10 = new ctkBusEvent(topic10, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue10(int,int,int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->addObserver(*propCallback10);


    ctkEventArgumentsList argList;

    m_EventDispatcherLocal->notifyEvent(*propSignal0);
    m_EventDispatcherLocal->notifyEvent(*propSignal0, &argList);


    int argCounter=1;
    for( ; argCounter <= 10; argCounter++) {
        if(argCounter != 0) {
            argList.append(ctkEventArgument(int, argCounter));
        }
        QString topicToNotify = "ctk/local/setObjectValue";
        topicToNotify.append(QString::number(argCounter));
        ctkBusEvent *notEvent = new ctkBusEvent(topicToNotify, ctkDictionary());


        m_EventDispatcherLocal->notifyEvent(*notEvent, &argList);

        delete notEvent;
    }

    //delete all events
    delete propSignal0;
    delete propSignal1;
    delete propSignal2;
    delete propSignal3;
    delete propSignal4;
    delete propSignal5;
    delete propSignal6;
    delete propSignal7;
    delete propSignal8;
    delete propSignal9;
    delete propSignal10;

    //delete all events
    delete propCallback0;
    delete propCallback1;
    delete propCallback2;
    delete propCallback3;
    delete propCallback4;
    delete propCallback5;
    delete propCallback6;
    delete propCallback7;
    delete propCallback8;
    delete propCallback9;
    delete propCallback10;
}

void ctkEventDispatcherLocalTest::notifyEventWitReturnValueTest() {

    //test with events which returns no value
    QString topic0 = "ctk/local/setObjectValue0WithReturnValue";
    QString topic1 = "ctk/local/setObjectValue1WithReturnValue";
    QString topic2 = "ctk/local/setObjectValue2WithReturnValue";
    QString topic3 = "ctk/local/setObjectValue3WithReturnValue";
    QString topic4 = "ctk/local/setObjectValue4WithReturnValue";
    QString topic5 = "ctk/local/setObjectValue5WithReturnValue";
    QString topic6 = "ctk/local/setObjectValue6WithReturnValue";
    QString topic7 = "ctk/local/setObjectValue7WithReturnValue";
    QString topic8 = "ctk/local/setObjectValue8WithReturnValue";
    QString topic9 = "ctk/local/setObjectValue9WithReturnValue";
    QString topic10 = "ctk/local/setObjectValue10WithReturnValue";

    //signal
    ctkBusEvent *propSignal0 = new ctkBusEvent(topic0, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue0WithReturnValue()");
    m_EventDispatcherLocal->registerSignal(*propSignal0);

    ctkBusEvent *propSignal1 = new ctkBusEvent(topic1, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue1WithReturnValue(int)");
    m_EventDispatcherLocal->registerSignal(*propSignal1);

    ctkBusEvent *propSignal2 = new ctkBusEvent(topic2, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue2WithReturnValue(int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal2);

    ctkBusEvent *propSignal3 = new ctkBusEvent(topic3, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue3WithReturnValue(int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal3);

    ctkBusEvent *propSignal4 = new ctkBusEvent(topic4, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue4WithReturnValue(int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal4);

    ctkBusEvent *propSignal5 = new ctkBusEvent(topic5, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue5WithReturnValue(int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal5);

    ctkBusEvent *propSignal6 = new ctkBusEvent(topic6, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue6WithReturnValue(int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal6);

    ctkBusEvent *propSignal7 = new ctkBusEvent(topic7, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue7WithReturnValue(int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal7);

    ctkBusEvent *propSignal8 = new ctkBusEvent(topic8, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue8WithReturnValue(int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal8);

    ctkBusEvent *propSignal9 = new ctkBusEvent(topic9, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue9WithReturnValue(int,int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal9);

    ctkBusEvent *propSignal10 = new ctkBusEvent(topic10, ctkEventTypeLocal, ctkSignatureTypeSignal, m_ObjTest, "signalSetObjectValue10WithReturnValue(int,int,int,int,int,int,int,int,int,int)");
    m_EventDispatcherLocal->registerSignal(*propSignal10);

    //callback
    ctkBusEvent *propCallback0 = new ctkBusEvent(topic0, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue0WithReturnValue()");
    bool result = m_EventDispatcherLocal->addObserver(*propCallback0);

    ctkBusEvent *propCallback1 = new ctkBusEvent(topic1, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue1WithReturnValue(int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback1);

    ctkBusEvent *propCallback2 = new ctkBusEvent(topic2, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue2WithReturnValue(int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback2);

    ctkBusEvent *propCallback3 = new ctkBusEvent(topic3, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue3WithReturnValue(int,int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback3);

    ctkBusEvent *propCallback4 = new ctkBusEvent(topic4, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue4WithReturnValue(int,int,int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback4);

    ctkBusEvent *propCallback5 = new ctkBusEvent(topic5, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue5WithReturnValue(int,int,int,int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback5);

    ctkBusEvent *propCallback6 = new ctkBusEvent(topic6, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue6WithReturnValue(int,int,int,int,int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback6);

    ctkBusEvent *propCallback7 = new ctkBusEvent(topic7, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue7WithReturnValue(int,int,int,int,int,int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback7);

    ctkBusEvent *propCallback8 = new ctkBusEvent(topic8, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue8WithReturnValue(int,int,int,int,int,int,int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback8);

    ctkBusEvent *propCallback9 = new ctkBusEvent(topic9, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue9WithReturnValue(int,int,int,int,int,int,int,int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback9);

    ctkBusEvent *propCallback10 = new ctkBusEvent(topic10, ctkEventTypeLocal, ctkSignatureTypeCallback, m_ObjTest, "setObjectValue10WithReturnValue(int,int,int,int,int,int,int,int,int,int)");
    result = m_EventDispatcherLocal->addObserver(*propCallback10);


    ctkEventArgumentsList argList;
    int returnValue = -1;
    ctkGenericReturnArgument ret_val = ctkEventReturnArgument(int,returnValue);

    int argCounter=0;
    bool ret(false);
    for( ; argCounter <= 10; argCounter++) {
        returnValue = -1;
        int *toAppend = new int(argCounter);
        if(argCounter != 0) { 
            argList.append(ctkEventArgument(int, *toAppend));
        }
        QString topicToNotify = "ctk/local/setObjectValue";
        topicToNotify.append(QString::number(argCounter));
        topicToNotify.append("WithReturnValue");
        ctkBusEvent *notEvent = new ctkBusEvent(topicToNotify, ctkDictionary());

        m_EventDispatcherLocal->notifyEvent(*notEvent, &argList, &ret_val);
        int c = 0, size = argList.count(), tempSum = 0;
        for(; c<size;c++) {
            tempSum += *((int*)(argList.at(c).data()));
        }

        ret = returnValue == tempSum;

        QVERIFY(ret);
        delete notEvent;
    }

    for( ;argCounter < 10; argCounter++) {
        int *i = (int *)(argList.at(argCounter).data());
        delete i;
    }

    //delete all events
    delete propSignal0;
    delete propSignal1;
    delete propSignal2;
    delete propSignal3;
    delete propSignal4;
    delete propSignal5;
    delete propSignal6;
    delete propSignal7;
    delete propSignal8;
    delete propSignal9;
    delete propSignal10;

    //delete all events
    delete propCallback0;
    delete propCallback1;
    delete propCallback2;
    delete propCallback3;
    delete propCallback4;
    delete propCallback5;
    delete propCallback6;
    delete propCallback7;
    delete propCallback8;
    delete propCallback9;
    delete propCallback10;
}

CTK_REGISTER_TEST(ctkEventDispatcherLocalTest);
#include "ctkEventDispatcherLocalTest.moc"
