/*
 *  ctkEventBusManager.cpp
 *  ctkEventBus
 *
 *  Created by Paolo Quadrani on 27/03/09.ctkEventBusManager *  Copyright 2010 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkEventBusManager.h"
#include "ctkTopicRegistry.h"
#include "ctkNetworkConnectorQtSoap.h"
#include "ctkNetworkConnectorQXMLRPC.h"

using namespace ctkEventBus;


ctkEventBusManager::ctkEventBusManager() : m_EnableEventLogging(false), m_LogEventTopic("*"), m_SkipDetach(false) {
    // Create local event dispatcher.
    m_LocalDispatcher = new ctkEventDispatcherLocal();
    m_LocalDispatcher->setObjectName("Local Event Dispatcher");

    // Create the remote event dispatcher.
    m_RemoteDispatcher = new ctkEventDispatcherRemote();
    m_RemoteDispatcher->setObjectName("Remote Event Dispatcher");
}

ctkEventBusManager::~ctkEventBusManager() {
    ctkNetworkConnectorHash::iterator i = m_NetworkConnectorHash.begin();
    while(i != m_NetworkConnectorHash.end()) {
        delete i.value();
        ++i;
    }
    m_NetworkConnectorHash.clear();

    //disconnet detachFromEventBus
    m_SkipDetach = true;

    if(m_LocalDispatcher) {
        m_LocalDispatcher->resetHashes();
        delete m_LocalDispatcher;
    }
    if(m_RemoteDispatcher) {
        m_RemoteDispatcher->resetHashes();
        delete m_RemoteDispatcher;
    }
}

void ctkEventBusManager::plugNetworkConnector(const QString &protocol, ctkNetworkConnector *connector) {
    m_NetworkConnectorHash.insert(protocol, connector);
}

bool ctkEventBusManager::isLocalSignalPresent(const QString topic) const {
    return m_LocalDispatcher->isLocalSignalPresent(topic);
}

ctkEventBusManager* ctkEventBusManager::instance() {
    static ctkEventBusManager instanceEventBus;
    return &instanceEventBus;
}

void ctkEventBusManager::shutdown() {
    ctkEventBus::ctkTopicRegistry::instance()->shutdown();
}

void ctkEventBusManager::initializeNetworkConnectors() {
    plugNetworkConnector("SOAP", new ctkNetworkConnectorQtSoap());
    plugNetworkConnector("XMLRPC", new ctkNetworkConnectorQXMLRPC());
}

bool ctkEventBusManager::addEventProperty(ctkBusEvent &props) const {
    bool result(false);
    QString topic = props[TOPIC].toString();
    QObject *obj = props[OBJECT].value<QObject*>();

    if(props[TYPE].toInt() == ctkEventTypeLocal) {
        // Local event dispatching.
        if(props[SIGTYPE].toInt() == ctkSignatureTypeCallback) {
            result = m_LocalDispatcher->addObserver(props);
        } else {
            //Add topic to the ctkTopicRegistry
            result = ctkEventBus::ctkTopicRegistry::instance()->registerTopic(topic, obj);
            if(!result) {
                return result;
            }
            result = m_LocalDispatcher->registerSignal(props);
        }
    } else {
        // Remote event dispatching.
        if(props[SIGTYPE].toInt() == ctkSignatureTypeCallback) {
            result = m_RemoteDispatcher->addObserver(props);
        } else {
            //Add topic to the ctkTopicRegistry
            result = ctkEventBus::ctkTopicRegistry::instance()->registerTopic(topic, obj);
            if(!result) {
                return result;
            }
            result = m_RemoteDispatcher->registerSignal(props);
        }
    }

    result = connect(obj, SIGNAL(destroyed()), this, SLOT(detachObjectFromBus()));
    return result;
}

void ctkEventBusManager::detachObjectFromBus() {
    if(m_SkipDetach) {
        return;
    }

    QObject *obj = QObject::sender();
    removeObserver(obj, "", false);
    removeSignal(obj, "", false);
}

void ctkEventBusManager::removeObserver(const QObject *obj, const QString topic, bool qt_disconnect) {
    if(obj == NULL) {
        return;
    }
    m_LocalDispatcher->removeObserver(obj, topic, qt_disconnect);
    m_RemoteDispatcher->removeObserver(obj, topic, qt_disconnect);
}

void ctkEventBusManager::removeSignal(const QObject *obj, QString topic, bool qt_disconnect) {
    if(obj == NULL) {
        return;
    }
    //remove topic from the ctkTopicRegistry
    bool result = ctkEventBus::ctkTopicRegistry::instance()->unregisterTopic(topic);
    if(result) {
        return;
    }

    m_LocalDispatcher->removeSignal(obj, topic, qt_disconnect);
    m_RemoteDispatcher->removeSignal(obj, topic, qt_disconnect);
}

bool ctkEventBusManager::removeEventProperty(ctkBusEvent &props) const {
    if(props.eventType() == ctkEventTypeLocal) {
        // Local event dispatching.
        if(props[SIGTYPE].toInt() == ctkSignatureTypeCallback) {
            return m_LocalDispatcher->removeObserver(props);
        } else {
            return m_LocalDispatcher->removeSignal(props);
        }
    } else {
        // Remote event dispatching.
        if(props[SIGTYPE].toInt() == ctkSignatureTypeCallback) {
            return m_RemoteDispatcher->removeObserver(props);
        } else {
            return m_RemoteDispatcher->removeSignal(props);
        }
    }
    return false;
}

void ctkEventBusManager::notifyEvent(const QString topic, ctkEventType ev_type, ctkEventArgumentsList *argList, ctkGenericReturnArgument *returnArg) const {
    if(m_EnableEventLogging) {
        if(m_LogEventTopic == "*" || m_LogEventTopic == topic) {
            qDebug() << tr("Event notification for TOPIC: %1").arg(topic);
        }
    }

    //event dispatched in local channel
    ctkBusEvent *event_dic = new ctkBusEvent(topic, ev_type, 0, NULL, "");
    /*(*event_dic)[TOPIC] = topic;
    (*event_dic)[TYPE] = static_cast<int>(ev_type);*/
    notifyEvent(*event_dic, argList, returnArg);
    delete event_dic;
}

void ctkEventBusManager::notifyEvent(ctkBusEvent &event_dictionary, ctkEventArgumentsList *argList, ctkGenericReturnArgument *returnArg) const {
    //event dispatched in remote channel
    if(event_dictionary[TYPE].toInt() == ctkEventTypeLocal) {
        m_LocalDispatcher->notifyEvent(event_dictionary, argList, returnArg);
    } else {
        m_RemoteDispatcher->notifyEvent(event_dictionary, argList);
    }
}

void ctkEventBusManager::enableEventLogging(bool enable) {
    m_EnableEventLogging = enable;
}

void ctkEventBusManager::logEventTopic(const QString topic) {
    m_LogEventTopic = topic;
}

void ctkEventBusManager::logAllEvents() {
    m_LogEventTopic = "*";
}

bool ctkEventBusManager::createServer(const QString &communication_protocol, unsigned int listen_port) {
    if(m_NetworkConnectorHash.count() == 0) {
        initializeNetworkConnectors();
    }

    bool res(m_NetworkConnectorHash.contains(communication_protocol));
    if(res) {
        ctkNetworkConnector *connector = m_NetworkConnectorHash.value(communication_protocol);
        m_RemoteDispatcher->setNetworkConnectorServer(connector);
        //ctkNetworkConnector *connector = m_RemoteDispatcher->networkConnectorServer();
        res = connector != NULL;
        if(res) {
            m_RemoteDispatcher->networkConnectorServer()->createServer(listen_port);
        }
    }
    return res;
}

void ctkEventBusManager::startListen() {
    ctkNetworkConnector *connector = m_RemoteDispatcher->networkConnectorServer();
    if(connector) {
        connector->startListen();
    } else {
        qWarning("%s", tr("Server can not start. Create it first, then call startListen again!!").toUtf8().data());
    }
}

bool ctkEventBusManager::createClient(const QString &communication_protocol, const QString &server_host, unsigned int port) {
    if(m_NetworkConnectorHash.count() == 0) {
        initializeNetworkConnectors();
    }

    bool res(m_NetworkConnectorHash.contains(communication_protocol));
    if(res) {
        m_RemoteDispatcher->setNetworkConnectorClient(m_NetworkConnectorHash.value(communication_protocol));
        ctkNetworkConnector *connector = m_RemoteDispatcher->networkConnectorClient();
        res = connector != NULL;
        if(res) {
            m_RemoteDispatcher->networkConnectorClient()->createClient(server_host, port);
        }
    }
    return res;
}
