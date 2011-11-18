/*
 *  ctkTopicRegistry.cpp
 *  ctkEventBus
 *
 *  Created by Roberto Mucci on 26/01/11.
 *  Copyright 2011 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkTopicRegistry.h"

using namespace ctkEventBus;

ctkTopicRegistry* ctkTopicRegistry::instance() {
    static ctkTopicRegistry instanceTopicRegistry;
    return &instanceTopicRegistry;
}

ctkTopicRegistry::ctkTopicRegistry() {
}

void ctkTopicRegistry::shutdown() {
    m_TopicHash.clear();
}

bool ctkTopicRegistry::registerTopic(const QString topic, const QObject *owner) {
    if(topic.isEmpty() || owner == NULL ){
        return false;
    }
    if(m_TopicHash.contains(topic)){
        //topic already registered
        const QObject *obj = m_TopicHash.value(topic,NULL);
        QString className(obj->metaObject()->className());
        qWarning() << QObject::tr("Topic %1 already owned by %2").arg(topic, className);
        return false;
    }
    m_TopicHash.insert(topic,owner);
    return true;
}

bool ctkTopicRegistry::unregisterTopic(const QString topic) {
    bool result = false;
    if(m_TopicHash.contains(topic)){
        if (m_TopicHash.remove(topic) > 0) {
            result = true;
        }
    }
    return result;
}

const QObject *ctkTopicRegistry::owner(const QString topic) const {
    return m_TopicHash.value(topic,NULL);
}

bool ctkTopicRegistry::isTopicRegistered(const QString topic) const {
    return m_TopicHash.contains(topic);
}

void ctkTopicRegistry::dump() {
    QHash<QString, const QObject*>::const_iterator i = m_TopicHash.constBegin();
    while (i != m_TopicHash.constEnd()) {
        const QObject *obj = i.value();
        QString key =  i.key();
        QString name = obj->metaObject()->className();
        qDebug() << "Topic: " << key << "   Owner: "  << name;
        ++i;
    }
}
