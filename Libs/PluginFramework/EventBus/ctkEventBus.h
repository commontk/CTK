#ifndef CTKEVENTBUS_H
#define CTKEVENTBUS_H

#include "ctkEvent.h"


  class EventBus {

  public:

    typedef QMap<QString, QVariant> Properties;

    virtual ~EventBus() {}

    virtual void postEvent(const Event& event) = 0;
    virtual void sendEvent(const Event& event) = 0;

    virtual void publishSignal(const QObject* publisher, const char* signal) = 0;

    virtual void subscribeSlot(const QObject* subscriber, const char* member, const Properties& properties) = 0;

  };


Q_DECLARE_INTERFACE(ctk::EventBus, "org.commontk.core.EventBus")

#endif // CTKEVENTBUS_H
