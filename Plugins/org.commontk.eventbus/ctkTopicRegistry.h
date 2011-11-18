/*
 *  ctkTopicRegistry.h
 *  ctkEventBus
 *
 *  Created by Roberto Mucci on 26/01/11.
 *  Copyright 2011 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef CTKTOPICREGISTRY_H
#define CTKTOPICREGISTRY_H

// Includes list
#include "ctkEventDefinitions.h"

namespace ctkEventBus {

/**
  Class name: ctkTopicRegistry
  This singletone provides the registration of topic and topic owner in a hash.
*/
class org_commontk_eventbus_EXPORT ctkTopicRegistry {
public:
    /// Return an instance of the event bus.
    static ctkTopicRegistry *instance();

    /// Destroy the singleton instance. To be called at the end of the application.
    void shutdown();

    /// Register a new topic.
    bool registerTopic(const QString topic, const QObject *owner);

    /// Unregister a topic.
    bool unregisterTopic(const QString topic);

    /// Return the owner of a topic.
    const QObject *owner(const QString topic) const;

    /// Check if a topic is present in the topic hash.
    bool isTopicRegistered(const QString topic) const;

    /// Dump of the topic hash.
    void dump();

private:
    /// Object constructor.
    ctkTopicRegistry();

    QHash<QString, const QObject*> m_TopicHash; ///< Hash containing pairs (topic,owner).
};

} //nameSpace ctkEventBus

#endif // MAFTOPICREGISTRY_H
