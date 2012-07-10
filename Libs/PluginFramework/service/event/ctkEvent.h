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

#ifndef CTKEVENT_H
#define CTKEVENT_H

#include "ctkPluginFrameworkExport.h"

#include <QMap>
#include <QVariant>
#include <QStringList>

#include <ctkLDAPSearchFilter.h>


class ctkEventData;

/**
 * \ingroup EventAdmin
 *
 * A CTK event.
 *
 * <code>ctkEvent</code> objects are delivered to <code>ctkEventHandler</code>
 * or Qt slots which subscribe to the topic of the event.
 */
class CTK_PLUGINFW_EXPORT ctkEvent
{

  QSharedDataPointer<ctkEventData> d;

public:

  /**
   * Default constructor for use with the Qt meta object system.
   */
  ctkEvent();

  ~ctkEvent();

  /**
   * Can be used to check if this ctkEvent instance is valid,
   * or if it has been constructed using the default constructor.
   *
   * @return <code>true</code> if this event object is valid,
   *         <code>false</code> otherwise.
   */
  bool isNull() const;

  /**
   * Constructs an event.
   *
   * @param topic The topic of the event.
   * @param properties The event's properties (may be empty).
   * @throws ctkInvalidArgumentException If topic is not a valid topic name.
   */
  ctkEvent(const QString& topic, const ctkDictionary& properties = ctkDictionary());
  ctkEvent(const ctkEvent& event);

  ctkEvent& operator=(const ctkEvent& other);

  /**
   * Compares this <code>ctkEvent</code> object to another object.
   *
   * <p>
   * An event is considered to be <b>equal to</b> another event if the topic
   * is equal and the properties are equal.
   *
   * @param other The <code>ctkEvent</code> object to be compared.
   * @return <code>true</code> if <code>other</code> is equal to
   *         this object; <code>false</code> otherwise.
   */
  bool operator==(const ctkEvent& other) const;

  /**
   * Retrieve the value of an event property. The event topic may be retrieved
   * with the property name &quot;event.topics&quot;.
   *
   * @param name the name of the property to retrieve
   * @return The value of the property, or an invalid QVariant if not found.
   */
  QVariant getProperty(const QString& name) const;

  /**
   * Indicate the presence of an event property. The event topic is present
   * using the property name &quot;event.topics&quot;.
   *
   * @param name The name of the property.
   * @return <code>true</code> if a property with the specified name is in the
   *         event. This property may have an invalid QVariant value.
   *         <code>false</code> otherwise.
   */
  bool containsProperty(const QString& name) const;

  /**
   * Returns a list of this event's property names. The list will include the
   * event topic property name &quot;event.topics&quot;.
   *
   * @return A non-empty list with one element per property.
   */
  QStringList getPropertyNames() const;

  /**
   * Returns the topic of this event.
   *
   * @return The topic of this event.
   */
  const QString& getTopic() const;

  /**
   * Tests this event's properties against the given filter using a case
   * sensitive match.
   *
   * @param filter The filter to test.
   * @return true If this event's properties match the filter, false
   *         otherwise.
   */
  bool matches(const ctkLDAPSearchFilter& filter) const;

};

Q_DECLARE_METATYPE(ctkEvent)

#endif // CTKEVENT_H
