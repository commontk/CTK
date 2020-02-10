/*
 *  ctkNetworkConnectorQXMLRPC.cpp
 *  ctkEventBus
 *
 *  Created by Daniele Giunchi on 11/04/10.
 *  Copyright 2009 B3C. All rights reserved.
 *
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkNetworkConnectorQXMLRPC.h"
#include "ctkEventBusManager.h"

#include <service/event/ctkEvent.h>

using namespace ctkEventBus;

ctkNetworkConnectorQXMLRPC::ctkNetworkConnectorQXMLRPC() : ctkNetworkConnector(), m_Client(NULL), m_Server(NULL), m_RequestId(0) {
    //generate remote signal, this signal must map in the
    //possible connection with the remote server.
    //Server, in this case XMLRPC, will register a method with id REMOTE_COMMUNICATION
    //and parameters QVariantList

    m_Protocol = "XMLRPC";
}

void ctkNetworkConnectorQXMLRPC::initializeForEventBus() {
    ctkRegisterRemoteSignal("ctk/remote/eventBus/comunication/send/xmlrpc", this, "remoteCommunication(const QString, ctkEventArgumentsList *)");
    ctkRegisterRemoteCallback("ctk/remote/eventBus/comunication/send/xmlrpc", this, "send(const QString, ctkEventArgumentsList *)");
}

ctkNetworkConnectorQXMLRPC::~ctkNetworkConnectorQXMLRPC() {
    if(m_Client) {
        delete m_Client;
        m_Client = NULL;
    }
    if(m_Server) {
        stopServer();
    }
}

//retrieve an instance of the object
ctkNetworkConnector *ctkNetworkConnectorQXMLRPC::clone() {
    ctkNetworkConnectorQXMLRPC *copy = new ctkNetworkConnectorQXMLRPC();
    return copy;
}

void ctkNetworkConnectorQXMLRPC::createClient(const QString hostName, const unsigned int port) {
    bool result(false);
    if(m_Client == NULL) {
        m_Client = new xmlrpc::Client(NULL);
        result = connect( m_Client, SIGNAL(done( int, QVariant )),
                 this, SLOT(processReturnValue( int, QVariant )) );
        result = connect( m_Client, SIGNAL(failed( int, int, QString )),
                 this, SLOT(processFault( int, int, QString )) );
    }
    m_Client->setHost( hostName, port );
}

void ctkNetworkConnectorQXMLRPC::createServer(const unsigned int port) {
    if(m_Server != NULL) {
        unsigned int p = m_Server->property("port").toUInt();
        if(p != port) {
            stopServer();
        }
    }
    m_Server = new xmlrpc::Server(NULL);
    m_Server->setProperty("port", port);

    // Create a new ID to allow methods registration on new server instance.
    QString id_name(tr("ctk/remote/eventbus/communication/send/xmlrpc/serverMethods%1").arg(port));

    // Register the signal to the event bus.
    /*mafRegisterRemoteSignal(id_name, this, "registerMethodsServer(mafRegisterMethodsMap)");
    connect(this, SIGNAL(registerMethodsServer(mafRegisterMethodsMap)),
            this, SLOT(registerServerMethod(mafRegisterMethodsMap)));*/

    QList<QVariant::Type> parametersForRegisterteredFunction;
    parametersForRegisterteredFunction.append(QVariant::String); //return argument
    parametersForRegisterteredFunction.append(QVariant::List); //parameters to send, event control parameters
    parametersForRegisterteredFunction.append(QVariant::List); //parameters to send, data parameters

    //registration of the method maf.remote.eventBus.comunication.xmlrpc at XMLRPC level
    // the connect uses function name ad signature defined by parametersForRegisterteredFunction
    mafRegisterMethodsMap methodsMapping;
    methodsMapping.insert("ctk/remote/eventBus/comunication/send/xmlrpc", parametersForRegisterteredFunction);
    registerServerMethod(methodsMapping);

    //if a user want to register another method, it is important to know that ctkEventDispatcherRemote allows
    // the registration of function with QVariantList parameter.
}

void ctkNetworkConnectorQXMLRPC::stopServer() {
    unsigned int p = m_Server->property("port").toUInt();
    if(p != 0) {
        // get the ID for the previous server;
        /*QString old_id_name(tr("ctk/remote/eventbus/communication/send/xmlrpc/serverMethods%1").arg(p));
        // Remove the old signal.
        ctkBusEvent props;
        props[TOPIC] = old_id_name;
        props[TYPE] = ctkEventTypeRemote;
        props[SIGNATURE] = "registerMethodsServer(mafRegisterMethodsMap)";
        ctkEventBusManager::instance()->removeEventProperty(props);*/
        // Delete (and stop) the previous instance of the server.
        if(m_Server) {
            delete m_Server;
            m_Server = NULL;
        }
    }
}

void ctkNetworkConnectorQXMLRPC::registerServerMethod(mafRegisterMethodsMap registerMethodsList) {
    if(m_Server->isListening()) {
        qDebug("%s", tr("Server is already listening on port %1").arg(m_Server->property("port").toUInt()).toUtf8().data());
        return;
    }
    // cycle over map:  method name and parameter list
    foreach (QString key, registerMethodsList.keys()) {
       const unsigned int parametersNumber = registerMethodsList.value(key).count();
       switch(parametersNumber) {
       case 1:
           m_Server->registerMethod(key, registerMethodsList.value(key).at(0) );
           break;
       case 2:
           m_Server->registerMethod(key, registerMethodsList.value(key).at(0), registerMethodsList.value(key).at(1));
           break;
       case 3:
           m_Server->registerMethod(key, registerMethodsList.value(key).at(0), registerMethodsList.value(key).at(1), registerMethodsList.value(key).at(2));
           break;
       case 4:
           m_Server->registerMethod(key, registerMethodsList.value(key).at(0), registerMethodsList.value(key).at(1), registerMethodsList.value(key).at(2), registerMethodsList.value(key).at(3));
           break;
       case 5:
           m_Server->registerMethod(key, registerMethodsList.value(key).at(0), registerMethodsList.value(key).at(1), registerMethodsList.value(key).at(2), registerMethodsList.value(key).at(3) , registerMethodsList.value(key).at(4));
           break;
       default:
           break;
       }
    }
}

void ctkNetworkConnectorQXMLRPC::startListen() {
    connect( m_Server, SIGNAL(incomingRequest( int, QString, QList<xmlrpc::Variant>)),
                 this, SLOT(processRequest( int, QString, QList<xmlrpc::Variant>)));

    unsigned int port = m_Server->property("port").toInt();
    if( m_Server->listen( port ) ) {
        qDebug() << "Listening for XML-RPC requests on port" << port;
    } else {
        qDebug() << "Error listening port" << port;
    }
}

void ctkNetworkConnectorQXMLRPC::send(const QString event_id, ctkEventArgumentsList *argList) {
    QList<xmlrpc::Variant> *vl = NULL;
    if(argList != NULL) {
        vl = new QList<xmlrpc::Variant>();

        int i=0, size = argList->count();
        for(;i<size;i++) {
            QString typeArgument;
            typeArgument = argList->at(i).name();
            if(typeArgument != "QVariantList") {
                qDebug() << typeArgument;
                qWarning("%s", tr("Remote Dispatcher need to have arguments that are QVariantList").toUtf8().data());
                delete vl;
                return;
            }

            void *vp = argList->at(i).data();
            QVariantList *l;
            l = (QVariantList *)vp;
            xmlrpc::Variant var;
            var.setValue(*l);

            vl->push_back(var); //only the first parameter represent the whole list of arguments
        }
        if(size == 0) {
            qWarning("%s", tr("Remote Dispatcher need to have at least one argument that is a QVariantList").toUtf8().data());
            return;
        }
    }

   xmlrpcSend(event_id, *vl);
   delete vl;
}

void ctkNetworkConnectorQXMLRPC::xmlrpcSend(const QString &methodName, QList<xmlrpc::Variant> parameters) {
    const unsigned int parametersNumber = parameters.count();
    switch(parametersNumber) {
    case 1:
        m_RequestId = m_Client->request(methodName, parameters.at(0) );
        break;
    case 2:
        m_RequestId = m_Client->request(methodName, parameters.at(0), parameters.at(1));
        break;
    case 3:
        m_RequestId = m_Client->request(methodName, parameters.at(0), parameters.at(1), parameters.at(2));
        break;
    case 4:
        m_RequestId = m_Client->request(methodName, parameters.at(0), parameters.at(1), parameters.at(2), parameters.at(3));
        break;
    default:
        break;
    }
}

void ctkNetworkConnectorQXMLRPC::processReturnValue( int requestId, QVariant value ) {
    Q_UNUSED( requestId );
    Q_ASSERT( value.canConvert( QVariant::String ) );
    qDebug("%s", value.toString().toUtf8().data());
    ctkEventBusManager::instance()->notifyEvent("ctk/local/eventBus/remoteCommunicationDone", ctkEventTypeLocal);
}

void ctkNetworkConnectorQXMLRPC::processFault( int requestId, int errorCode, QString errorString ) {
    // Log the error.
    qDebug("%s", tr("Process Fault for requestID %1 with error %2 - %3").arg(QString::number(requestId), QString::number(errorCode), errorString).toUtf8().data());
    ctkEventBusManager::instance()->notifyEvent("ctk/local/eventBus/remoteCommunicationFailed", ctkEventTypeLocal);
}

void ctkNetworkConnectorQXMLRPC::processRequest( int requestId, QString methodName, QList<xmlrpc::Variant> parameters ) {
    Q_UNUSED( methodName );

    //first parameter is ctkEventBus message
    enum {
      EVENT_PARAMETERS,
      DATA_PARAMETERS,
    };

    enum {
      EVENT_ID,
      EVENT_ITEM_TYPE,
      EVENT_SIGNATURE_TYPE,
      EVENT_METHOD_SIGNATURE,
    };

    if(parameters.at(EVENT_PARAMETERS).toList().count() == 0) {
        m_Server->sendReturnValue( requestId, QString("No Command to Execute, command list is empty") );
    }

    //here eventually can be used a filter for events

    //first argument regards local signal to be called.
    QString id_name = parameters.at(EVENT_PARAMETERS).toList().at(EVENT_ID).toString();

    int size = parameters.count();

    ctkEventArgumentsList *argList = NULL;
    QVariantList p;
    p.append((parameters.at(1).value< QVariantList >()));
    if(size > 1 && p.count() != 0) {
        argList = new ctkEventArgumentsList();
        argList->push_back(Q_ARG(QVariantList, p));
    }

    if ( ctkEventBusManager::instance()->isLocalSignalPresent(id_name) ) {
        ctkBusEvent dictionary(id_name,ctkEventTypeLocal,0,NULL,"");
        /*dictionary.setEventTopic(id_name);
        dictionary.setEventType(ctkEventTypeLocal);*/
        ctkEventBusManager::instance()->notifyEvent(dictionary, argList);
        m_Server->sendReturnValue( requestId, QString("OK") );
    } else {
        m_Server->sendReturnValue( requestId, QString("FAIL") );
    }
    if(argList){
        delete argList;
        argList = NULL;
    }
}
