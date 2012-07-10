/*
 *  ctkNetworkConnectorZeroMQ.h
 *  ctkEventBus
 *
 *  Created by Daniele Giunchi on 11/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef ctkNetworkConnectorZeroMQ_H
#define ctkNetworkConnectorZeroMQ_H

// include list
#include "ctkNetworkConnector.h"

namespace ctkEventBus {

/**
 Class name: ctkNetworkConnectorZeroMQ
 This class is the implementation class for client/server objects that works over network
 with xml-rpc protocol. The server side part also create a new ID named REGISTER_SERVER_METHODS_XXX
 (where the XXX is the port on which run the server) that allows you to register your own remote
 callbacks. The library used is qxmlrpc.
 */
class org_commontk_eventbus_EXPORT ctkNetworkConnectorZeroMQ : public ctkNetworkConnector {
    Q_OBJECT


public:
    /// object constructor.
    ctkNetworkConnectorZeroMQ();

    /// object destructor.
    /*virtual*/ ~ctkNetworkConnectorZeroMQ();

    /// create the unique instance of the client.
    /*virtual*/ void createClient(const QString hostName, const unsigned int port);

    /// create the unique instance of the server.
    /*virtual*/ void createServer(const unsigned int port);

    /// Start the server.
    /*virtual*/ void startListen();

    //retrieve an instance of the object
    /*virtual*/ ctkNetworkConnector *clone();

    /// register all the signals and slots
    /*virtual*/ void initializeForEventBus();

    /// Allow to send a network request.
    /** Contains the conversion between maf datatypes and qxmlrpc datatype based both on QVariant. */
    /*virtual*/ void send(const QString event_id, ctkEventArgumentsList *argList);

private Q_SLOTS:
    /// callback for the client which retrieve the variable from the server
    virtual void processReturnValue( int requestId, QVariant value );

    //// here goes slots which handle the connection

    
protected:
    //here goes zeromq vars
    
private:
    //here ges function for zeromq connection
    
    /// stop and destroy the server instance.
    void stopServer();

    
};

} //namespace ctkEventBus


#endif // ctkNetworkConnectorZeroMQ_H
