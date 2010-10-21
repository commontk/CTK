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

#ifndef CTKPLUGINFRAMEWORKEVENT_H
#define CTKPLUGINFRAMEWORKEVENT_H

#include <QSharedDataPointer>

#include "ctkPluginFrameworkExport.h"

#include <stdexcept>

class ctkPlugin;
class ctkPluginFrameworkEventData;

/**
 * A general event from the Framework.
 *
 * <p>
 * <code>ctkPluginFrameworkEvent</code> objects are delivered to slots connected
 * via ctkPluginContext::connectFrameworkListener when a general event occurs
 * within the plugin environment.
 * A type code is used to identify the event type for future extendability.
 *
 * @see ctkPluginContext#connectFrameworkListener
 * @see ctkEventBus
 */
class CTK_PLUGINFW_EXPORT ctkPluginFrameworkEvent
{

  QSharedDataPointer<ctkPluginFrameworkEventData> d;

public:

  enum Type {
    /**
     * The Framework has started.
     *
     * <p>
     * This event is fired when the Framework has started after all installed
     * plugins that are marked to be started have been started and the Framework
     * has reached the initial start level. The source of this event is the
     * System Plugin.
     */
    STARTED,

    /**
     * An error has occurred.
     *
     * <p>
     * There was an error associated with a plugin.
     */
    ERROR,

    /**
     * A warning has occurred.
     *
     * <p>
     * There was a warning associated with a plugin.
     */
    WARNING,

    /**
     * An informational event has occurred.
     *
     * <p>
     * There was an informational event associated with a plugin.
     */
    INFO,

    /**
     * The Framework has stopped.
     *
     * <p>
     * This event is fired when the Framework has been stopped because of a stop
     * operation on the system plugin. The source of this event is the System
     * Plugin.
     */
    STOPPED,

    /**
     * The Framework has stopped during update.
     *
     * <p>
     * This event is fired when the Framework has been stopped because of an
     * update operation on the system plugin. The Framework will be restarted
     * after this event is fired. The source of this event is the System Plugin.
     */
    STOPPED_UPDATE,

    /**
     * The Framework did not stop before the wait timeout expired.
     *
     * <p>
     * This event is fired when the Framework did not stop before the wait
     * timeout expired. The source of this event is the System Plugin.
     */
    WAIT_TIMEDOUT

  };

  /**
   * Default constructor for use with the Qt meta object system.
   */
  ctkPluginFrameworkEvent();

  ~ctkPluginFrameworkEvent();

  /**
   * Can be used to check if this ctkPluginFrameworkEvent instance is valid,
   * or if it has been constructed using the default constructor.
   *
   * @return <code>true</code> if this event object is valid,
   *         <code>false</code> otherwise.
   */
  bool isNull() const;

  /**
   * Creates a Framework event regarding the specified plugin and exception.
   *
   * @param type The event type.
   * @param plugin The event source.
   * @param fwException The related exception.
   */
  ctkPluginFrameworkEvent(Type type, ctkPlugin* plugin, const std::exception& fwException);

  /**
   * Creates a Framework event regarding the specified plugin.
   *
   * @param type The event type.
   * @param plugin The event source.
   */
  ctkPluginFrameworkEvent(Type type, ctkPlugin* plugin);

  ctkPluginFrameworkEvent(const ctkPluginFrameworkEvent& other);

  ctkPluginFrameworkEvent& operator=(const ctkPluginFrameworkEvent& other);

  /**
   * Returns the exception error string related to this event.
   *
   * @return The related error string.
   */
  QString getErrorString() const;

  /**
   * Returns the plugin associated with the event. This plugin is also the
   * source of the event.
   *
   * @return The plugin associated with the event.
   */
  ctkPlugin* getPlugin() const;

  /**
   * Returns the type of framework event.
   * <p>
   * The type values are:
   * <ul>
   * <li>{@link #STARTED}
   * <li>{@link #ERROR}
   * <li>{@link #WARNING}
   * <li>{@link #INFO}
   * <li>{@link #STARTLEVEL_CHANGED}
   * <li>{@link #STOPPED}
   * <li>{@link #STOPPED_UPDATE}
   * <li>{@link #WAIT_TIMEDOUT}
   * </ul>
   *
   * @return The type of state change.
   */
  Type getType() const;
};

CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, ctkPluginFrameworkEvent::Type type);
CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, const ctkPluginFrameworkEvent& event);

#endif // CTKPLUGINFRAMEWORKEVENT_H
