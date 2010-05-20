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

#ifndef CTKPLUGINFRAMEWORKEVENT_H
#define CTKPLUGINFRAMEWORKEVENT_H

#include <QObject>
#include <QSharedDataPointer>

#include "CTKPluginFrameworkExport.h"


  class Plugin;
  class PluginFrameworkEventData;

  /**
   * A general event from the Framework.
   *
   * <p>
   * <code>PluginFrameworkEvent</code> objects are delivered to slots connected
   * <code>FrameworkListener</code>s when a general event occurs within the plugin
   * environment. A type code is used to identify the event type for future
   * extendability.
   *
   * @see PluginContext#connectFrameworkListener
   * @see EventBus
   */
  class CTK_PLUGINFW_EXPORT PluginFrameworkEvent : public QObject
  {
    Q_OBJECT
    Q_PROPERTY(Type type READ getType CONSTANT)
    Q_PROPERTY(Plugin* plugin READ getPlugin CONSTANT)
    Q_PROPERTY(QString errorString READ getErrorString CONSTANT)
    Q_ENUMS(Type)

    QSharedDataPointer<PluginFrameworkEventData> d;

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
    PluginFrameworkEvent();

    /**
     * Creates a Framework event regarding the specified plugin and exception.
     *
     * @param type The event type.
     * @param plugin The event source.
     * @param fwException The related exception.
     */
    PluginFrameworkEvent(Type type, Plugin* plugin, const std::exception& fwException);

    /**
     * Creates a Framework event regarding the specified plugin.
     *
     * @param type The event type.
     * @param plugin The event source.
     */
    PluginFrameworkEvent(Type type, Plugin* plugin);

    PluginFrameworkEvent(const PluginFrameworkEvent& other);

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
    Plugin* getPlugin() const;

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


  class PluginFrameworkEventData : public QSharedData
  {
  public:

    PluginFrameworkEventData(PluginFrameworkEvent::Type type, Plugin* plugin, const QString& exc)
      : plugin(plugin), errorString(exc), type(type)
    {

    }

    PluginFrameworkEventData(const PluginFrameworkEventData& other)
      : QSharedData(other), plugin(other.plugin), errorString(other.errorString),
        type(other.type)
    {

    }

    /**
     * Plugin related to the event.
     */
    Plugin* const	plugin;

    /**
     * Exception related to the event.
     */
    const QString errorString;

    /**
     * Type of event.
     */
    const PluginFrameworkEvent::Type type;
  };


#endif // CTKPLUGINFRAMEWORKEVENT_H
