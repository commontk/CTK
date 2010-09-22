/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkEvent.h"


class ctkEventPrivate {

public:

  ctkEventPrivate(const QString& topic, const ctkDictionary& properties)
    : ref(1), topic(topic), properties(properties)
  {

  }

  QAtomicInt ref;
  const QString topic;
  const ctkDictionary properties;

};


ctkEvent::ctkEvent(const QString& topic, const ctkDictionary& properties)
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

QVariant ctkEvent::property(const QString& name) const
{
  return d->properties[name];
}

QStringList ctkEvent::propertyNames() const
{
  return d->properties.keys();
}

const QString& ctkEvent::topic() const
{
  return d->topic;
}

bool ctkEvent::matches(const ctkLDAPSearchFilter& filter) const
{
  return filter.matchCase(d->properties);
}
