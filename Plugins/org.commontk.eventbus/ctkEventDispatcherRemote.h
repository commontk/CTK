/*
 *  ctkEventDispatcherRemote.h
 *  ctkEventBus
 *
 *  Created by Daniele Giunchi on 11/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef CTKEVENTDISPATCHERREMOTE_H
#define CTKEVENTDISPATCHERREMOTE_H

//include list
#include "ctkEventDefinitions.h"
#include "ctkEventDispatcher.h"

namespace ctkEventBus {

class ctkNetworkConnector;

/**
 Class name: ctkEventDispatcherRemote
 This allows dispatching events coming from local application to attached observers.
 */
class org_commontk_eventbus_EXPORT ctkEventDispatcherRemote : public ctkEventDispatcher {
    Q_OBJECT

public:
    /// object constructor.
    ctkEventDispatcherRemote();

    /// object destructor.
    /*virtual*/ ~ctkEventDispatcherRemote();

    /// Emit event corresponding to the given id (present into the event_dictionary) locally to the application.
    /*virtual*/ void notifyEvent(ctkBusEvent &event_dictionary, ctkEventArgumentsList *argList = NULL, ctkGenericReturnArgument *returnArg = NULL) const;

    /// Set the active server side network connector type.
    void setNetworkConnectorServer(ctkNetworkConnector *connector);

    /// return the current instantiated server side network connector.
    ctkNetworkConnector *networkConnectorServer();

    /// Set the active client side network connector type.
    void setNetworkConnectorClient(ctkNetworkConnector *connector);

    /// return the current instantiated client side network connector.
    ctkNetworkConnector *networkConnectorClient();

protected:
    /// Register MAF global events
    /*virtual*/ void initializeGlobalEvents();

private:
    ctkNetworkConnector *m_NetworkConnectorServer; ///< Class used to manage the communication with the remote event bus.
    ctkNetworkConnector *m_NetworkConnectorClient; ///< Class used to manage the communication with the remote event bus.
};


} //namespace ctkEventBus

Q_DECLARE_METATYPE(QVariantList);

#endif // CTKEVENTDISPATCHERREMOTE_H
