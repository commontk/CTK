#ifndef CTKEVENTBUS_H
#define CTKEVENTBUS_H

#include "ctkEvent.h"


class ctkEventBus {

public:

  virtual ~ctkEventBus() {}

  virtual void postEvent(const ctkEvent& event) = 0;
  virtual void sendEvent(const ctkEvent& event) = 0;

  virtual void publishSignal(const QObject* publisher, const char* signal) = 0;

  virtual void subscribeSlot(const QObject* subscriber, const char* member, const ctkProperties& properties) = 0;

};


Q_DECLARE_INTERFACE(ctkEventBus, "org.commontk.core.ctkEventBus")

#endif // CTKEVENTBUS_H
