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

#ifndef CTKEVENTCONSTANTS_H
#define CTKEVENTCONSTANTS_H

#include <QString>

#include "ctkPluginFrameworkExport.h"

/**
 * Defines standard names for event properties
 *
 * \see ctkEventBus::subscribeSlot()
 */
struct CTK_PLUGINFW_EXPORT EventConstants {

  /**
   * Qt slot property (named <code>event.topics</code>)
   * specifying the <code>ctkEvent</code> topics of interest to a
   * subscribed slot.
   * <p>
   * Subscribed slots SHOULD be registered with this property. The value of the
   * property is a QString or a QStringList that describes the topics in
   * which the handler is interested. An asterisk ('*') may be used as a
   * trailing wildcard. Subscribed slots which do not have a value for this
   * property must not receive events. More precisely, the value of each
   * string must conform to the following grammar:
   *
   * <pre>
   *  topic-description := '*' | topic ( '&#47*' )?
   *  topic := token ( '/' token )*
   * </pre>
   *
   * @see ctkEvent
   */
  static const QString EVENT_TOPIC; // = "event.topics"

  /**
   * Qt slot property (named <code>event.filter</code>)
   * specifying a filter to further select <code>ctkEvent</code> s of interest to
   * a subscribed slot.
   * <p>
   * Subscribed slots MAY be registered with this property. The value of this
   * property is a QString containing an LDAP-style filter specification. Any
   * of the event's properties may be used in the filter expression. Each
   * subscribed slot is notified for any event which belongs to the topics in
   * which the slot has expressed an interest. If the slot is also
   * registered with this property, then the properties of the event
   * must also match the filter for the event to be delivered to the slot.
   * <p>
   * If the filter syntax is invalid, then the slot must be ignored
   * and a warning should be logged.
   *
   * @see ctkEvent
   * @see ctkLDAPSearchFilter
   */
  static const QString EVENT_FILTER; // = "event.filter"

  /**
   * The Plugin Symbolic Name of the plugin relevant to the event. The type of
   * the value for this event property is <code>QString</code>.
   */
  static const QString PLUGIN_SYMBOLICNAME; // = "plugin.symbolicName";

  /**
   * The Plugin id of the plugin relevant to the event. The type of the value
   * for this event property is <code>long</code>.
   */
  static const QString PLUGIN_ID; // = "plugin.id";

  /**
   * The ctkPlugin object of the plugin relevant to the event. The type of the
   * value for this event property is {@link ctkPlugin}.
   */
  static const QString PLUGIN; // = "plugin";

  /**
   * The version of the plugin relevant to the event. The type of the value
   * for this event property is {@link ctkVersion}.
   */
  static const QString PLUGIN_VERSION; // = "plugin.version";

};


#endif // CTKEVENTCONSTANTS_H
