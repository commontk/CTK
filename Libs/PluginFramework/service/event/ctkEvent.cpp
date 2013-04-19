/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
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

#include "ctkEventConstants.h"

#include <ctkException.h>

class ctkEventData : public QSharedData
{

public:

  ctkEventData(const QString& topic, const ctkDictionary& properties)
    : topic(topic), properties(properties)
  {
    validateTopicName(topic);
    this->properties.insert(ctkEventConstants::EVENT_TOPIC, topic);
  }

  static void validateTopicName(const QString& topic)
  {
    if (topic.isEmpty())
    {
      throw ctkInvalidArgumentException("empty topic");
    }

    // Can't start or end with a '/' but anywhere else is okay
    // Can't have "//" as that implies empty token
    if (topic.startsWith("/") || topic.endsWith("/") ||
        topic.contains("//"))
    {
      throw ctkInvalidArgumentException(QString("invalid topic: %1").arg(topic));
    }

    QString::const_iterator topicEnd = topic.end();
    QChar A('A'), Z('Z'), a('a'), z('z'), zero('0'), nine('9');
    QChar dash('-'), slash('/'), underscore('_');
    for (QString::const_iterator i = topic.begin(); i < topicEnd; ++i)
    {
      QChar c(*i);
      if ((A <= c) && (c <= Z)) continue;
      if ((a <= c) && (c <= z)) continue;
      if ((zero <= c) && (c <= nine)) continue;
      if ((c == underscore) || (c == dash) || (c == slash)) continue;
      throw ctkInvalidArgumentException(QString("invalid topic: %1").arg(topic));
    }
  }

  const QString topic;
  ctkDictionary properties;

};

//----------------------------------------------------------------------------
ctkEvent::ctkEvent()
  : d(0)
{

}

//----------------------------------------------------------------------------
ctkEvent::ctkEvent(const QString& topic, const ctkDictionary& properties)
  : d(new ctkEventData(topic, properties))
{

}

//----------------------------------------------------------------------------
// This is fast thanks to implicit sharing
ctkEvent::ctkEvent(const ctkEvent &event)
  : d(event.d)
{

}

//----------------------------------------------------------------------------
ctkEvent::~ctkEvent()
{
}

//----------------------------------------------------------------------------
bool ctkEvent::isNull() const
{
  return !d;
}

//----------------------------------------------------------------------------
ctkEvent& ctkEvent::operator=(const ctkEvent& other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
bool ctkEvent::operator==(const ctkEvent& other) const
{
  if (d == other.d)
    return true;

  if (d->topic == other.d->topic &&
      d->properties == other.d->properties)
    return true;

  return false;
}

//----------------------------------------------------------------------------
QVariant ctkEvent::getProperty(const QString& name) const
{
  return d->properties[name];
}

//----------------------------------------------------------------------------
bool ctkEvent::containsProperty(const QString& name) const
{
  if (ctkEventConstants::EVENT_TOPIC == name)
  {
   return true;
  }
  return d->properties.contains(name);
}

//----------------------------------------------------------------------------
QStringList ctkEvent::getPropertyNames() const
{
  return d->properties.keys();
}

//----------------------------------------------------------------------------
const QString& ctkEvent::getTopic() const
{
  return d->topic;
}

//----------------------------------------------------------------------------
bool ctkEvent::matches(const ctkLDAPSearchFilter& filter) const
{
  return filter.matchCase(d->properties);
}
