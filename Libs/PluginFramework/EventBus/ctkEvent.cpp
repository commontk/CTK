
#include "ctkEvent.h"


  class EventPrivate {

  public:

    EventPrivate(const QString& topic, const LDAPSearchFilter::Dictionary& properties)
      : ref(1), topic(topic), properties(properties)
    {

    }

    QAtomicInt ref;
    const QString topic;
    const LDAPSearchFilter::Dictionary properties;

  };


  Event::Event(const QString& topic, const LDAPSearchFilter::Dictionary& properties)
    : d(new EventPrivate(topic, properties))
  {

  }

  /*
   * This is fast thanks to implicit sharing
   */
  Event::Event(const Event &event)
    : d(event.d)
  {
    d->ref.ref();
  }

  Event::~Event()
  {
    if (!d->ref.deref())
      delete d;
  }

  bool Event::operator==(const Event& other) const
  {
    if (d == other.d)
      return true;

    if (d->topic == other.d->topic &&
        d->properties == other.d->properties)
      return true;

    return false;
  }

  const QVariant& Event::property(const QString& name) const
  {
    d->properties[name];
  }

  QStringList Event::propertyNames() const
  {
    d->properties.keys();
  }

  const QString& Event::topic() const
  {
    d->topic;
  }

  bool Event::matches(const LDAPSearchFilter& filter) const
  {
    // TODO
    return true;

}
