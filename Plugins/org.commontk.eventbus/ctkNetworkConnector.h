/*
 *  ctkNetworkConnector.h
 *  ctkEventBus
 *
 *  Created by Daniele Giunchi on 11/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef CTKNETWORKCONNECTOR_H
#define CTKNETWORKCONNECTOR_H

//include list
#include "ctkEventDefinitions.h"

namespace ctkEventBus {

/**
 Class name: ctkNetworkConnector
 This class is the interface class for client/server objects that works over network.
 */
class org_commontk_eventbus_EXPORT ctkNetworkConnector : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString protocol READ protocol);

public:
    /// object constructor.
    ctkNetworkConnector();

    /// create the unique instance of the client.
    virtual void createClient(const QString hostName, const unsigned int port) = 0;

    /// create the unique instance of the server.
    virtual void createServer(const unsigned int port) = 0;

    /// Start the server.
    virtual void startListen() = 0;

    /// Allow to send a network request.
    virtual void send(const QString event_id, ctkEventArgumentsList *argList) = 0;

    /// retrieve an instance of the object
    virtual ctkNetworkConnector *clone() = 0;

    /// retrieve the protocol type of the connector
    QString protocol();

    /// register all the signals and slots
    virtual void initializeForEventBus() = 0;

Q_SIGNALS:
    /// signal for send a message to through network
    void remoteCommunication(const QString event_id, ctkEventArgumentsList *argList);

protected:
    QString m_Protocol; ///< define the protocol of the connector (xmlrpc, soap, etc...)
};

} //namespace ctkEventBus

#endif // CTKNETWORKCONNECTOR_H
