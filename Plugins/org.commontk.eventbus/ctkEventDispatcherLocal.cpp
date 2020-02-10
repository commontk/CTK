/*
 *  ctkEventDispatcherLocal.cpp
 *  ctkEventBus
 *
 *  Created by Paolo Quadrani on 27/03/09.
 *  Copyright 2009 B3C. All rights reserved.
 *namespace ctkEventBus
 *  See Licence at: http://tiny.cc/QXJ4D
 *
 */

#include "ctkEventDispatcherLocal.h"
#include "ctkBusEvent.h"

using namespace ctkEventBus;

ctkEventDispatcherLocal::ctkEventDispatcherLocal() : ctkEventDispatcher() {
    this->initializeGlobalEvents();
}

void ctkEventDispatcherLocal::initializeGlobalEvents() {
    ctkBusEvent *properties = new ctkBusEvent("ctk/local/eventBus/globalUpdate",ctkEventTypeLocal,ctkSignatureTypeSignal, this, "notifyDefaultEvent()");
    /*QString topic = "ctk/local/eventBus/globalUpdate";
    (*properties)[TOPIC] =  topic;
    (*properties)[TYPE] = ctkEventTypeLocal;
    (*properties)[SIGTYPE] = mafSignatureTypeSignal;
    QVariant var;
    var.setValue((QObject*)this);
    (*properties)[OBJECT] = var;
    (*properties)[SIGNATURE] = "notifyDefaultEvent()";*/
    registerSignal(*properties);

    ctkEventDispatcher::initializeGlobalEvents();
}

void ctkEventDispatcherLocal::notifyEvent(ctkBusEvent &event_dictionary, ctkEventArgumentsList *argList, ctkGenericReturnArgument *returnArg) const {
    QString topic = event_dictionary[TOPIC].toString();
    ctkEventItemListType items = signalItemProperty(topic);
    ctkBusEvent *itemEventProp;
    foreach(itemEventProp, items) {
        if((*itemEventProp)[SIGNATURE].toString().length() != 0) {
            QString signal_to_emit = (*itemEventProp)[SIGNATURE].toString().split("(")[0];
            QObject *obj = (*itemEventProp)[OBJECT].value<QObject *>();
            if(argList != NULL) {
                if (returnArg == NULL || returnArg->data() == NULL) { //don't use return value
                    switch (argList->count()) {
                        case 0:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1());
                            break;
                        case 1:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                            argList->at(0));
                            break;
                        case 2:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1));
                            break;
                        case 3:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1), argList->at(2));
                            break;
                        case 4:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3));
                            break;
                        case 5:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4));
                            break;
                        case 6:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), argList->at(5));
                            break;
                        case 7:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), \
                             argList->at(5), argList->at(6));
                            break;
                        case 8:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), \
                             argList->at(5), argList->at(6), argList->at(7));
                            break;
                        case 9:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), \
                             argList->at(5), argList->at(6), argList->at(7), argList->at(8));
                            break;
                        case 10:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), \
                             argList->at(5), argList->at(6), argList->at(7), argList->at(8), argList->at(9));
                            break;
                        default:
                            qWarning("%s", tr("Number of arguments not supported. Max 10 arguments").toUtf8().data());
                    } //switch
                 } else { //use return value
                    switch (argList->count()) {
                        case 0:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg);
                            break;
                        case 1:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg,\
                            argList->at(0));
                            break;
                        case 2:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1));
                            break;
                        case 3:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1), argList->at(2));
                            break;
                        case 4:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3));
                            break;
                        case 5:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4));
                            break;
                        case 6:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), argList->at(5));
                            break;
                        case 7:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), \
                             argList->at(5), argList->at(6));
                            break;
                        case 8:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), \
                             argList->at(5), argList->at(6), argList->at(7));
                            break;
                        case 9:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), \
                             argList->at(5), argList->at(6), argList->at(7), argList->at(8));
                            break;
                        case 10:
                            this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg, \
                             argList->at(0), argList->at(1), argList->at(2), argList->at(3), argList->at(4), \
                             argList->at(5), argList->at(6), argList->at(7), argList->at(8), argList->at(9));
                            break;
                        default:
                            qWarning("%s", tr("Number of arguments not supported. Max 10 arguments").toUtf8().data());
                    } //switch
                 }
            } else {
                if (returnArg == NULL || returnArg->data() == NULL) { //don't use return value
                    this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1());
                } else {
                    this->metaObject()->invokeMethod(obj, signal_to_emit.toLatin1(), *returnArg);
                }

            }
        }
    }
}

