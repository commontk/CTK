/*
 *  ctkTopicRegistryTest.cpp
 *  ctkEventBusTest
 *
 *  Created by Roberto Mucci on 26/01/11.
 *  Copyright 2011 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTestSuite.h"
#include <ctkTopicRegistry.h>

using namespace ctkEventBus;

/**
 Class name: ctkTopicRegistryTest
 This class implements the test suite for ctkTopicRegistry.
 */

//! <title>
//ctkTopicRegistry
//! </title>
//! <description>
//ctkTopicRegistry provides the registration of topic and topic owner in a hash.
//! </description>

class ctkTopicRegistryTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    /// Initialize test variables
    void initTestCase() {
        m_TopicRegistry = ctkTopicRegistry::instance();
    }

    /// Cleanup test variables memory allocation.
    void cleanupTestCase() {
        m_TopicRegistry->shutdown();
    }

    /// ctkTopicRegistry registration test case.
    void ctkTopicRegistryRegisterTest();

    /// ctkTopicRegistry owner test case.
    void ctkTopicRegistryOwnerTest();


private:
    ctkTopicRegistry *m_TopicRegistry; ///< Test var.

};

void ctkTopicRegistryTest::ctkTopicRegistryRegisterTest() {
    QVERIFY(m_TopicRegistry != NULL);
    QString topic("ctk/local/eventBus/testTopic");

    bool result = m_TopicRegistry->registerTopic(topic, this);
    QVERIFY(result);

    result = m_TopicRegistry->registerTopic("ctk/local/eventBus/testTopic1", this);
    QVERIFY(result);

    result = m_TopicRegistry->registerTopic("ctk/local/eventBus/testTopic2", this);
    QVERIFY(result);

    result = m_TopicRegistry->registerTopic(topic, this);
    QVERIFY(!result);

    result = m_TopicRegistry->registerTopic("", this);
    QVERIFY(!result);

    result = m_TopicRegistry->registerTopic(topic, NULL);
    QVERIFY(!result);

    result = m_TopicRegistry->registerTopic("", NULL);
    QVERIFY(!result);
}

void ctkTopicRegistryTest::ctkTopicRegistryOwnerTest() {
    QString topic("ctk/local/eventBus/testTopic");
    const QObject *obj = m_TopicRegistry->owner(topic);
    QVERIFY(obj == this);

    obj = m_TopicRegistry->owner("");
    QVERIFY(obj == NULL);

    obj = m_TopicRegistry->owner("ctk/local/eventBus/TopicNotRegisterd");
    QVERIFY(obj == NULL);

    //Check isPresent() method.
    bool result = m_TopicRegistry->isTopicRegistered(topic);
    QVERIFY(result);

    result = m_TopicRegistry->isTopicRegistered("");
    QVERIFY(!result);

    result = m_TopicRegistry->isTopicRegistered("ctk/local/eventBus/TopicNotRegisterd");
    QVERIFY(!result);

    // print 3 topic
    m_TopicRegistry->dump();

    result = m_TopicRegistry->unregisterTopic(topic);
    QVERIFY(result);

    result = m_TopicRegistry->unregisterTopic("ctk/local/eventBus/TopicNotRegisterd");
    QVERIFY(!result);

    // print 2 topic
    m_TopicRegistry->dump();
}


CTK_REGISTER_TEST(ctkTopicRegistryTest);
#include "ctkTopicRegistryTest.moc"



