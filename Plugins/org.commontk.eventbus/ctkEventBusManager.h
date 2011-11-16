/*
 *  ctkEventBusManager.h
 *  ctkEventBus
 *
 *  Created by Paolo Quadrani on 27/03/09.
 *  Copyright 2010 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef CTKEVENTBUSMANAGER_H
#define CTKEVENTBUSMANAGER_H

// Includes list
#include "ctkEventDefinitions.h"
#include "ctkEventDispatcherLocal.h"
#include "ctkEventDispatcherRemote.h"
#include "ctkBusEvent.h"

namespace ctkEventBus {

// Class forwarding list

/**
 Class name: ctkEventBusManager
 This singletone provides the access point of the Communication Bus for MAF3 framework.
 ctkEventBusManager defines also the mafId maf.local.eventBus.globalUpdate to be used as generic update notification mechanism that the event bus can send to all the observers.
 */
class org_commontk_eventbus_EXPORT ctkEventBusManager : public QObject {
    Q_OBJECT

public:
    /// Return an instance of the event bus.
    static ctkEventBusManager *instance();

    /// Add a new event property (observer or event) to the event bus hash.
    /** Return true if observer has beed added correctly, false otherwise.
    This method check before adding a new observer that it has not already been inserted into the events' Hash with the same id and callback signature.*/
    bool addEventProperty(ctkBusEvent &props) const;

    /// Remove the event property from the event bus hash.
    bool removeEventProperty(ctkBusEvent &props) const;

    /// Remove the object passed as argument from the observer's hash.
    /** This method allows to remove from the observer's hash the object
    passed as argument. If no topic is specified, the observer will be removed from all
    the topic it was listening, otherwise it will be disconnected only from the given topic.*/
    void removeObserver(const QObject *obj, const QString topic = "", bool qt_disconnect = true);

    /// Remove the object passed as argument from the signal emitter's hash.
    /** This method allows to remove from the signal emitter's hash the object
    passed as argument. If no topic is specified, the emitter will be removed from all
    the topic it was emitting signals, otherwise it will be removed only from the given topic.*/
    void removeSignal(const QObject *obj, QString topic = "", bool qt_disconnect = true);

    /// Notify events associated to the given id locally to the application.
    void notifyEvent(ctkBusEvent &event_dictionary, ctkEventArgumentsList *argList = NULL, ctkGenericReturnArgument *returnArg = NULL) const;

    /// Notify event associated to the given id locally to the application.
    void notifyEvent(const QString topic, ctkEventType ev_type = ctkEventTypeLocal, ctkEventArgumentsList *argList = NULL, ctkGenericReturnArgument *returnArg = NULL) const;

    /// Enable/Disable event logging to allow dumping events notification into the selected logging output stream.
    void enableEventLogging(bool enable = true);

    /// When logging is enabled, allows logging events releted to specific id (require a valid topic).
    void logEventTopic(const QString topic);

    /// When enabled, allows logging all events. It reset the value for m_LogEventId to -1 (the default)
    void logAllEvents();

    /// Destroy the singleton instance. To be called at the end of the application.
    void shutdown();

    /// initialize NetworkConnectors
    void initializeNetworkConnectors();

    /// Retrieve if the signal has been registered previously.
    bool isLocalSignalPresent(const QString topic) const;

    /// Plug a new network connector into the connector hash for the given network protocol (protocol eg. "XMLRPC") (connector_type eg. "ctkEventBus::ctkNetworkConnectorQXMLRPC").
    void plugNetworkConnector(const QString &protocol, ctkNetworkConnector *connector);

    /// Create the server for remote communication according to the given protocol and listen port.
    bool createServer(const QString &communication_protocol, unsigned int listen_port);

    /// Allow to start server listening.
    void startListen();

    /// Create the client for remote communication according to the given protocol, server host and port.
    bool createClient(const QString &communication_protocol, const QString &server_host, unsigned int port);

public Q_SLOTS:
    /// Intercepts objects deletation and detach them from the event bus.
    void detachObjectFromBus();

private:
    /// Object constructor.
    ctkEventBusManager();

    /// Object destructor.
    ~ctkEventBusManager();

    ctkEventDispatcherLocal *m_LocalDispatcher; ///< Dispatcher class which dispatches events locally to the application.
    ctkEventDispatcherRemote *m_RemoteDispatcher; ///< Dispatcher class dispatches events remotely to another applications or via network.

    bool m_EnableEventLogging; ///< Flag to enable/disable logging for event sent.
    QString m_LogEventTopic; ///< Store the current Event_Id to track through the logger.
    ctkNetworkConnectorHash m_NetworkConnectorHash; ///< Hash table used to store the association of network protocols and network connector types.

    bool m_SkipDetach; ///< lifesafe variable to avoid the detach from eventbus.

};

} // namespace ctkEventBus

#endif // CTKEVENTBUSMANAGER
