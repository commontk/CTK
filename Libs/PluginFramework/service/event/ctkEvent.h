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


class ctkEventPrivate;

/**
 * A CTK event.
 *
 * <code>ctkEvent</code> objects are delivered to Qt slots
 * which subscribe to the topic of the event.
 */
class CTK_PLUGINFW_EXPORT ctkEvent {

public:

  /**
   * Constructs an event.
   *
   * @param topic The topic of the event.
   * @param properties The event's properties (may be empty).
   * @throws std::invalid_argument If topic is not a valid topic name.
   */
  ctkEvent(const QString& topic, const ctkDictionary& properties = ctkDictionary());
  ctkEvent(const ctkEvent& event);
  ~ctkEvent();

  /**
   * Compares this <code>ctkEvent</code> object to another object.
   *
   * <p>
   * An event is considered to be <b>equal to</b> another event if the topic
   * is equal and the properties are equal.
   *
   * @param object The <code>ctkEvent</code> object to be compared.
   * @return <code>true</code> if <code>other</code> is equal to
   *         this object; <code>false</code> otherwise.
   */
  bool operator==(const ctkEvent& other) const;

  /**
   * Retrieves a property.
   *
   * @param name the name of the property to retrieve
   * @return The value of the property, or an invalid QVariant if not found.
   */
  QVariant property(const QString& name) const;

  /**
   * Returns a list of this event's property names.
   *
   * @return A non-empty list with one element per property.
   */
  QStringList propertyNames() const;

  /**
   * Returns the topic of this event.
   *
   * @return The topic of this event.
   */
  const QString& topic() const;

  /**
   * Tests this event's properties against the given filter using a case
   * sensitive match.
   *
   * @param filter The filter to test.
   * @return true If this event's properties match the filter, false
   *         otherwise.
   */
  bool matches(const ctkLDAPSearchFilter& filter) const;

protected:

  ctkEventPrivate * const d;
};


#endif // CTKEVENT_H
