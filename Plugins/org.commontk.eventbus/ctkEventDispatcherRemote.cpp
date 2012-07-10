/*
 *  ctkEventDispatcherRemote.cpp
 *  ctkEventBus
 *
 *  Created by Paolo Quadrani on 27/03/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkEventDispatcherRemote.h"
#include "ctkBusEvent.h"
#include "ctkNetworkConnector.h"

using namespace ctkEventBus;

ctkEventDispatcherRemote::ctkEventDispatcherRemote() : ctkEventDispatcher(), m_NetworkConnectorServer(NULL), m_NetworkConnectorClient(NULL) {
    this->initializeGlobalEvents();
}

ctkEventDispatcherRemote::~ctkEventDispatcherRemote() {
    if(m_NetworkConnectorServer) delete m_NetworkConnectorServer;
    m_NetworkConnectorServer = NULL;
    if(m_NetworkConnectorClient) delete m_NetworkConnectorClient;
    m_NetworkConnectorClient = NULL;
}

ctkNetworkConnector *ctkEventDispatcherRemote::networkConnectorServer() {
    return m_NetworkConnectorServer;
}

ctkNetworkConnector *ctkEventDispatcherRemote::networkConnectorClient() {
    return m_NetworkConnectorClient;
}

void ctkEventDispatcherRemote::initializeGlobalEvents() {
    ctkBusEvent *properties = new ctkBusEvent("ctk/remote/eventBus/globalUpdate",ctkEventTypeRemote,ctkSignatureTypeSignal,this,"notifyDefaultEvent()");
    this->registerSignal(*properties);

    // events like remoteCommunicationDone or failed represents th bridge events between a remote communication
    // and the possibility to call local slots. The notifyEvent local sends events inside the local objects registered as observers
    // through the event bus manager while the remote notification (ctkEventTypeRemote) uses the network connector.
}

void ctkEventDispatcherRemote::setNetworkConnectorServer(ctkNetworkConnector *connector) {
    if(m_NetworkConnectorServer == NULL) {
        m_NetworkConnectorServer = connector->clone();
        m_NetworkConnectorServer->initializeForEventBus();
    } else {
        if(m_NetworkConnectorServer->protocol() != connector->protocol()) {
            delete m_NetworkConnectorServer; //if there will be multiprotocol , here there will be a problem for thread app
            m_NetworkConnectorServer = connector->clone();
            m_NetworkConnectorServer->initializeForEventBus();
       }
   }
}

void ctkEventDispatcherRemote::setNetworkConnectorClient(ctkNetworkConnector *connector) {
     if(m_NetworkConnectorClient == NULL) {
         m_NetworkConnectorClient = connector->clone();
         m_NetworkConnectorClient->initializeForEventBus();
     } else {
         if(m_NetworkConnectorClient->protocol() != connector->protocol()) {
             delete m_NetworkConnectorClient; //if there will be multiprotocol , here there will be a problem for thread app
             m_NetworkConnectorClient = connector->clone();
             m_NetworkConnectorClient->initializeForEventBus();
        }
    }
}

void ctkEventDispatcherRemote::notifyEvent(ctkBusEvent &event_dictionary, ctkEventArgumentsList *argList, ctkGenericReturnArgument *returnArg) const {
    //Q_UNUSED(event_dictionary);
    //Q_UNUSED(argList);
    Q_UNUSED(returnArg);

    // Call the notifyEventRemote converting the arguments...
    m_NetworkConnectorClient->send(event_dictionary[TOPIC].toString(), argList);
}
