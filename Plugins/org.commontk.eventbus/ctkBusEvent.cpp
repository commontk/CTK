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

#include "ctkBusEvent.h"
#include "ctkEventDefinitions.h"
#include <ctkException.h>

class ctkBusEventData : public QSharedData
{

public:

  ctkBusEventData(const QString& topic, const ctkDictionary& properties)
    : topic(topic), properties(properties)
  {
    this->properties.insert(TOPIC, topic);
  }

  ctkBusEventData(QString topic, int event_type, int signature_type, QObject *objectPointer, QString signature)
    : topic(topic)
  {
      properties.insert(TOPIC, topic);
      properties.insert(TYPE, static_cast<int>(event_type));
      properties.insert(SIGTYPE, static_cast<int>(signature_type));
      QVariant var;
      var.setValue(objectPointer);
      properties.insert(OBJECT, var);
      properties.insert(SIGNATURE, signature);
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


ctkBusEvent::ctkBusEvent()
  : d(0)
{

}

ctkBusEvent::ctkBusEvent(const QString& topic, const ctkDictionary& properties)
  : d(new ctkBusEventData(topic, properties))
{

}

ctkBusEvent::ctkBusEvent(QString topic, int event_type, int signature_type, QObject *objectPointer, QString signature)
: d(new ctkBusEventData(topic, event_type, signature_type, objectPointer, signature)) {

}

/*
 * This is fast thanks to implicit sharing
 */
ctkBusEvent::ctkBusEvent(const ctkBusEvent &event)
  : ctkEvent(event), d(event.d)
{

}

ctkBusEvent::~ctkBusEvent()
{
}

ctkBusEvent& ctkBusEvent::operator=(const ctkBusEvent& event)
{
  d = event.d;
  return *this;
}

QVariant &ctkBusEvent::operator[](QString key) {
    return (d->properties)[key];
}

int ctkBusEvent::eventType() const {
    return static_cast<int>((d->properties).value("EventType").toInt());
}

QString ctkBusEvent::eventTopic() const {
    return (d->properties).value(TOPIC).toString();
}

bool ctkBusEvent::isEventLocal() const {
    int et = (d->properties).value(TYPE).toInt();
    return et == 0; //is local
}

void ctkBusEvent::setEventType(int et) {
    (d->properties).insert(TYPE, static_cast<int>(et));
}

void ctkBusEvent::setEventTopic(QString topic) {
    (d->properties).insert(TOPIC, topic);
}
