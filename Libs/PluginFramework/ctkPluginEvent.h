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

#include <QObject>
#include <QSharedDataPointer>

#include "CTKPluginFrameworkExport.h"

namespace ctk {

  class Plugin;
  class PluginEventData;

  /**
   * An event from the Framework describing a plugin lifecycle change.
   * <p>
   * <code>PluginEvent</code> objects are delivered to slots connected
   * to PluginContext::pluginChanged() or to registerd event handlers
   * for the topic "org.commontk/framework/pluginChanged"
   * when a change occurs in a plugins's lifecycle. A type code is used to identify
   * the event type for future extendability.
   *
   * @see PluginContext#pluginChanged
   * @see EventBus
   */
  class CTK_PLUGINFW_EXPORT PluginEvent : public QObject
  {
    Q_OBJECT
    Q_PROPERTY(Type type READ getType CONSTANT)
    Q_PROPERTY(Plugin* plugin READ getPlugin CONSTANT)
    Q_ENUMS(Type)

    QSharedDataPointer<PluginEventData> d;

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
     * Creates a plugin event of the specified type.
     *
     * @param type The event type.
     * @param plugin The plugin which had a lifecycle change.
     */
    PluginEvent(Type type, Plugin* plugin);

    PluginEvent(const PluginEvent& other);

    /**
     * Returns the plugin which had a lifecycle change.
     *
     * @return The plugin that had a change occur in its lifecycle.
     */
    Plugin* getPlugin() const;

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

  class PluginEventData : public QSharedData
  {
  public:

    PluginEventData(PluginEvent::Type type, Plugin* plugin)
      : type(type), plugin(plugin)
    {

    }

    PluginEventData(const PluginEventData& other)
      : QSharedData(other), type(other.type), plugin(other.plugin)
    {

    }

    PluginEvent::Type type;
    Plugin* plugin;
  };

}

#endif // CTKPLUGINEVENT_H
