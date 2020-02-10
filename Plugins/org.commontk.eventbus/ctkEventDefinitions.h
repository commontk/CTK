/*
 *  ctkEventDefinitions.h
 *  ctkEventBus
 *
 *  Created by Paolo Quadrani on 27/03/09.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#ifndef CTKEVENTDEFINITIONS_H
#define CTKEVENTDEFINITIONS_H

// Qt includes
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QThread>
#include <QThreadPool>
#include <QObject>
#include <QDebug>

#include "org_commontk_eventbus_Export.h"

//defines

#define TOPIC "event.topics"
#define TYPE "EventType"
#define SIGTYPE "SignatureType"
#define OBJECT "ObjectPointer"
#define SIGNATURE "Signature"

class ctkBusEvent;

#define ctkRegisterLocalSignal(topic, sender, signature) \
    {\
        ctkBusEvent *properties = new ctkBusEvent(topic, ctkEventBus::ctkEventTypeLocal, ctkEventBus::ctkSignatureTypeSignal, static_cast<QObject*>(sender), signature); \
        bool ok = ctkEventBus::ctkEventBusManager::instance()->addEventProperty(*properties);\
        if(!ok) {\
            qWarning("%s", tr("Some problem occourred during the signal registration with ID '%1'.").arg(topic).toUtf8().data());\
            if(properties) {delete properties; properties = NULL;} \
            }\
    }

#define ctkRegisterRemoteSignal(topic, sender, signature) \
    {\
        ctkBusEvent *properties = new ctkBusEvent(topic, ctkEventBus::ctkEventTypeRemote, ctkEventBus::ctkSignatureTypeSignal, static_cast<QObject*>(sender), signature); \
        bool ok =  ctkEventBus::ctkEventBusManager::instance()->addEventProperty(*properties);\
        if(!ok) {\
            qWarning("%s", tr("Some problem occourred during the signal registration with ID '%1'.").arg(topic).toUtf8().data());\
            if(properties) {delete properties; properties = NULL;} \
        }\
    }

#define ctkRegisterLocalCallback(topic, observer, signature) \
    {\
        ctkBusEvent *properties = new ctkBusEvent(topic, ctkEventBus::ctkEventTypeLocal, ctkEventBus::ctkSignatureTypeCallback, static_cast<QObject*>(observer), signature); \
        bool ok =  ctkEventBus::ctkEventBusManager::instance()->addEventProperty(*properties);\
        if(!ok) {\
            qWarning("%s", tr("Some problem occourred during the callback registration with ID '%1'.").arg(topic).toUtf8().data());\
            if(properties) {delete properties; properties = NULL;} \
        }\
    }

#define ctkRegisterRemoteCallback(topic, sender, signature) \
    {\
        ctkBusEvent *properties = new ctkBusEvent(topic, ctkEventBus::ctkEventTypeRemote, ctkEventBus::ctkSignatureTypeCallback, static_cast<QObject*>(sender), signature); \
        bool ok =  ctkEventBus::ctkEventBusManager::instance()->addEventProperty(*properties);\
        if(!ok) {\
            qWarning("%s", tr("Some problem occourred during the callback registration with ID '%1'.").arg(topic).toUtf8().data());\
            if(properties) {delete properties; properties = NULL;} \
        }\
    }


namespace ctkEventBus {
//forward classes
class ctkNetworkConnector;

/// Hash table that associate the communication protocol with the corresponding network connector class (Eg. XMLRPC, ctkEventBus::ctkNetworkConnectorQXMLRPC)
typedef QHash<QString, ctkNetworkConnector *> ctkNetworkConnectorHash;

/// typedef that represents dictionary entries ( key  , value )
typedef QHash<QString, QVariant> ctkEventHash;

///< Enum that identify the ctkEventType's type: Local or Remote.
typedef enum {
    ctkEventTypeLocal,
    ctkEventTypeRemote
} ctkEventType;

///< Enum that identify the mafSignatureType's type: Signal or Callback.
typedef enum {
    ctkSignatureTypeSignal = 0,
    ctkSignatureTypeCallback = 1
} ctkSignatureType;

/// List of the arguments to be sent through the event bus.
typedef QList<QGenericArgument> ctkEventArgumentsList;
typedef ctkEventArgumentsList * ctkEventArgumentsListPointer;
#define ctkEventArgument(type, data) QArgument<type >(#type, data)
#define ctkGenericReturnArgument QGenericReturnArgument
#define ctkEventReturnArgument(type, data) QReturnArgument<type >(#type, data)


/// Types definitions for events' hash (to be more readable).
typedef QHash<QString, ctkBusEvent *> ctkEventsHashType;

/// type definition for observers' properties list to be stored into the event's hash.
typedef QList<ctkBusEvent *> ctkEventItemListType;

/// map which represent list of function to be registered in the server, with parameters
typedef QMap<QString, QList<QVariant::Type> >  mafRegisterMethodsMap;

} // namespace ctkEventBus


Q_DECLARE_METATYPE(ctkEventBus::ctkEventArgumentsListPointer);
Q_DECLARE_METATYPE(ctkEventBus::mafRegisterMethodsMap);

#endif // CTKEVENTDEFINITIONS_H
