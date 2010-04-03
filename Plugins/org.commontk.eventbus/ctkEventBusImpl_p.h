#ifndef CTKEVENTBUSIMPL_H
#define CTKEVENTBUSIMPL_H

#include <EventBus/ctkEventBus.h>

#include <QList>
#include <QHash>
#include <QSet>

namespace ctk {

  class EventHandlerWrapper;

  class EventBusImpl : public QObject,
                       public EventBus
  {
    Q_OBJECT
    Q_INTERFACES(ctk::EventBus)

  public:

    static EventBusImpl* instance();

    void postEvent(const Event& event);
    void sendEvent(const Event& event);

    void publishSignal(const QObject* publisher, const char* signal);

    void subscribeSlot(const QObject* subscriber, const char* member, const Properties& properties);

  protected:

    typedef QList<EventHandlerWrapper*> HandlerList;

    HandlerList globalWildcard;

    QHash<QString, HandlerList> topicName;

    void dispatchEvent(const Event& event, bool isAsync);

    void bucket(EventHandlerWrapper* wrapper);

    QSet<EventHandlerWrapper*> handlers(const QString& topic);

  private:

    EventBusImpl();
  };

}

#endif // CTKEVENTBUSIMPL_H
