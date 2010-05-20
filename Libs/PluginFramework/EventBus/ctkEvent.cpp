
#include "ctkEvent.h"


class ctkEventPrivate {

public:

  ctkEventPrivate(const QString& topic, const ctkLDAPSearchFilter::Dictionary& properties)
    : ref(1), topic(topic), properties(properties)
  {

  }

  QAtomicInt ref;
  const QString topic;
  const ctkLDAPSearchFilter::Dictionary properties;

};


ctkEvent::ctkEvent(const QString& topic, const ctkLDAPSearchFilter::Dictionary& properties)
  : d(new ctkEventPrivate(topic, properties))
{

}

/*
 * This is fast thanks to implicit sharing
 */
ctkEvent::ctkEvent(const ctkEvent &event)
  : d(event.d)
{
  d->ref.ref();
}

ctkEvent::~ctkEvent()
{
  if (!d->ref.deref())
    delete d;
}

bool ctkEvent::operator==(const ctkEvent& other) const
{
  if (d == other.d)
    return true;

  if (d->topic == other.d->topic &&
      d->properties == other.d->properties)
    return true;

  return false;
}

const QVariant& ctkEvent::property(const QString& name) const
{
  d->properties[name];
}

QStringList ctkEvent::propertyNames() const
{
  d->properties.keys();
}

const QString& ctkEvent::topic() const
{
  d->topic;
}

bool ctkEvent::matches(const ctkLDAPSearchFilter& filter) const
{
  // TODO
  return true;
}
