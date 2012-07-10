/*
 *  ctkNetworkConnectorZeroMQ.cpp
 *  ctkEventBus
 *
 *  Created by Daniele Giunchi on 11/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkNetworkConnectorZeroMQ.h"
#include "ctkEventBusManager.h"

#include <service/event/ctkEvent.h>

using namespace ctkEventBus;

ctkNetworkConnectorZeroMQ::ctkNetworkConnectorZeroMQ() : ctkNetworkConnector() {

    m_Protocol = "SOCKET";
}

void ctkNetworkConnectorZeroMQ::initializeForEventBus() {
    //ctkRegisterRemoteSignal("ctk/remote/eventBus/comunication/??????????", this, "remoteCommunication(const QString, ctkEventArgumentsList *)");
    //ctkRegisterRemoteCallback("ctk/remote/eventBus/comunication/????????????", this, "send(const QString, ctkEventArgumentsList *)");
}

ctkNetworkConnectorZeroMQ::~ctkNetworkConnectorZeroMQ() {
    }

//retrieve an instance of the object
ctkNetworkConnector *ctkNetworkConnectorZeroMQ::clone() {
    ctkNetworkConnectorZeroMQ *copy = new ctkNetworkConnectorZeroMQ();
    return copy;
}

void ctkNetworkConnectorZeroMQ::createClient(const QString hostName, const unsigned int port) {
    Q_UNUSED(hostName);
    Q_UNUSED(port);
    //@@ TODO
}

void ctkNetworkConnectorZeroMQ::createServer(const unsigned int port) {
   Q_UNUSED(port);
   //@@ TODO
}

void ctkNetworkConnectorZeroMQ::stopServer() {
    //@@ TODO
}


void ctkNetworkConnectorZeroMQ::startListen() {
    //@@ TODO
}

void ctkNetworkConnectorZeroMQ::send(const QString event_id, ctkEventArgumentsList *argList) {
	Q_UNUSED(event_id);
	Q_UNUSED(argList);
    //@@ TODO
}


void ctkNetworkConnectorZeroMQ::processReturnValue( int requestId, QVariant value ) {
	Q_UNUSED(requestId);
	Q_UNUSED(value);
    //@@ TODO
}
