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

#ifndef CTKPLUGINEVENT_H
#define CTKPLUGINEVENT_H

#include <QSharedDataPointer>

#include "CTKPluginFrameworkExport.h"


class ctkPlugin;
class ctkPluginEventData;

/**
 * An event from the Framework describing a plugin lifecycle change.
 * <p>
 * <code>ctkPluginEvent</code> objects are delivered to slots connected
 * via ctkPluginContext::connectPluginListener() when a change
 * occurs in a plugins's lifecycle. A type code is used to identify
 * the event type for future extendability.
 *
 * @see ctkPluginContext#connectPluginListener
 * @see ctkEventBus
 */
class CTK_PLUGINFW_EXPORT ctkPluginEvent
{

  QSharedDataPointer<ctkPluginEventData> d;

public:

  enum Type {
    INSTALLED,
    STARTED,
    STOPPED,
    UPDATED,
    UNINSTALLED,
    RESOLVED,
    UNRESOLVED,
    STARTING,
    STOPPING,
    LAZY_ACTIVATION
  };

  /**
   * Default constructor for use with the Qt meta object system.
   */
  ctkPluginEvent();

  ~ctkPluginEvent();

  /**
   * Creates a plugin event of the specified type.
   *
   * @param type The event type.
   * @param plugin The plugin which had a lifecycle change.
   */
  ctkPluginEvent(Type type, ctkPlugin* plugin);

  ctkPluginEvent(const ctkPluginEvent& other);

  /**
   * Returns the plugin which had a lifecycle change.
   *
   * @return The plugin that had a change occur in its lifecycle.
   */
  ctkPlugin* getPlugin() const;

  /**
   * Returns the type of lifecyle event. The type values are:
   * <ul>
   * <li>{@link #INSTALLED}
   * <li>{@link #RESOLVED}
   * <li>{@link #LAZY_ACTIVATION}
   * <li>{@link #STARTING}
   * <li>{@link #STARTED}
   * <li>{@link #STOPPING}
   * <li>{@link #STOPPED}
   * <li>{@link #UPDATED}
   * <li>{@link #UNRESOLVED}
   * <li>{@link #UNINSTALLED}
   * </ul>
   *
   * @return The type of lifecycle event.
   */
  Type getType() const;

};

#endif // CTKPLUGINEVENT_H
