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

#ifndef CTKEVENTBUS_H
#define CTKEVENTBUS_H

#include "ctkEvent.h"


/**
 * The Event Bus service. Plugins wishing to publish events can either
 * obtain the Event Bus service and call one of the event delivery methods
 * or publish a Qt signal for a specific event topic.
 *
 */
class ctkEventBus {

public:

  virtual ~ctkEventBus() {}

  /**
   * Initiate asynchronous delivery of an event. This method returns to the
   * caller before delivery of the event is completed.
   *
   * @param event The event to send to all listeners which subscribe to the
   *        topic of the event.
   *
   */
  virtual void postEvent(const ctkEvent& event) = 0;

  /**
   * Initiate synchronous delivery of an event. This method does not return to
   * the caller until delivery of the event is completed.
   *
   * @param event The event to send to all listeners which subscribe to the
   *        topic of the event.
   *
   */
  virtual void sendEvent(const ctkEvent& event) = 0;

  /**
   * Publish (register) a Qt signal for event delivery. Emitting the signal
   * has the same effect as calling postEvent() if <code>type</code> is
   * Qt::QueuedConnection and as sendEvent() if <code>type</code> is
   * Qt::DirectConnection.
   *
   * @param publisher The owner of the signal.
   * @param signal The signal in normalized form.
   * @param signal_topic The topic string for the events this signal is emitting.
   * @param type Qt::QueuedConnection for asynchronous delivery and
   *        Qt::DirectConnection for synchronous delivery.
   */
  virtual void publishSignal(const QObject* publisher, const char* signal,
                             const QString& signal_topic, Qt::ConnectionType type = Qt::QueuedConnection) = 0;

  /**
   * Subsribe for (observe) events. The slot is called whenever an event is sent
   * which matches the topic string and LDAP search expression contained
   * in the properties.
   *
   * Slots should be registered with a property EventConstants::EVENT_TOPIC.
   * The value being a QString or QStringList object that describes which
   * topics the slot is interested in. A wildcard (’*’ \u002A) may be used as
   * the last token of a topic name, for example com/action&#47*. This matches any
   * topic that shares the same first tokens. For example, com/action&#47* matches
   * com/action/listen. Slot which have not been specified with the EVENT_TOPIC
   * property must not receive events.
   * The value of each entry in the EVENT_TOPIC property must conform to the
   * following grammar:
   * \verbatim
   * topic-scope ::= ’*’ | ( topic ’&#47*’ ? )
   * \endverbatim
   *
   * Slots can also be registered with a property named EventConstants::EVENT_FILTER.
   * The value of this property must be a string containing a filter specification.
   * Any of the event's properties can be used in the filter expression.
   * Each slot is notified for any event which belongs to the topics the
   * slot has expressed an interest in. If the handler has defined a
   * EVENT_FILTER property then the event properties must also match the filter
   * expression. If the filter is an error, then the Event Bus service
   * should log a warning and further ignore the registered slot.
   *
   * @param subscriber The owner of the slot.
   * @param member The slot in normalized form.
   * @param properties A map containing topics and a filter expression.
   * @return Returns an id which can be used to update the properties.
   */
  virtual QString subscribeSlot(const QObject* subscriber, const char* member, const ctkProperties& properties) = 0;

  /**
   * Updates the properties of a previously registered slot. This can be used
   * to change the topics the slot is interested in or to change the filter expression.
   * A previously registered property can be removed by providing an invalid QVariant.
   *
   * @param subscriptionId The slot id obtained by a call to subscribeSlot().
   * @param properties The properties which should be updated.
   */
  virtual void updateProperties(const QString& subsriptionId, const ctkProperties& properties) = 0;

};


Q_DECLARE_INTERFACE(ctkEventBus, "org.commontk.core.ctkEventBus")

#endif // CTKEVENTBUS_H
