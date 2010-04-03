#include "ctkEventBusImpl_p.h"

#include <QSetIterator>

#include "ctkEventHandlerWrapper_p.h"

namespace ctk {

  EventBusImpl* EventBusImpl::instance()
  {
    static EventBusImpl inst;
    return &inst;
  }

  EventBusImpl::EventBusImpl()
  {

  }

  void EventBusImpl::postEvent(const Event& event)
  {
    dispatchEvent(event, true);
  }

  void EventBusImpl::sendEvent(const Event& event)
  {
    dispatchEvent(event, false);
  }

  void EventBusImpl::publishSignal(const QObject* publisher, const char* signal)
  {

  }

  void EventBusImpl::subscribeSlot(const QObject* subscriber, const char* member, const Properties& properties)
  {

    // TODO check for duplicates

    EventHandlerWrapper* wrapper = new EventHandlerWrapper(subscriber, member, properties);
    if (wrapper->init())
    {
      bucket(wrapper);
    }
  }

  void EventBusImpl::dispatchEvent(const Event& event, bool isAsync)
  {
    QString topic = event.topic();

    QSet<EventHandlerWrapper*> eventHandlers = this->handlers(topic);
    if (eventHandlers.empty()) return;

    QSetIterator<EventHandlerWrapper*> iter(eventHandlers);
    while (iter.hasNext())
    {
      iter.next()->handleEvent(event);
    }
  }

  void EventBusImpl::bucket(EventHandlerWrapper* wrapper)
  {
    // TODO bucket logic
    globalWildcard.push_back(wrapper);
  }

  QSet<EventHandlerWrapper*> EventBusImpl::handlers(const QString& topic)
  {
    // TODO
    return globalWildcard.toSet();
  }

}
