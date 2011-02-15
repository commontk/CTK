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

#ifndef CTKPLUGINEVENT_H
#define CTKPLUGINEVENT_H

#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QMetaType>

#include "ctkPluginFrameworkExport.h"


class ctkPlugin;
class ctkPluginEventData;

/**
 * \ingroup PluginFramework
 *
 * An event from the Framework describing a plugin lifecycle change.
 * <p>
 * <code>ctkPluginEvent</code> objects are delivered to slots connected
 * via ctkPluginContext::connectPluginListener() when a change
 * occurs in a plugins's lifecycle. A type code is used to identify
 * the event type for future extendability.
 *
 * @see ctkPluginContext#connectPluginListener
 */
class CTK_PLUGINFW_EXPORT ctkPluginEvent
{

  QSharedDataPointer<ctkPluginEventData> d;

public:

  enum Type {
    /**
     * The plugin has been installed.
     *
     * @see ctkPluginContext::installBundle(const QString&)
     */
    INSTALLED,

    /**
     * The plugin has been started.
     * <p>
     * The plugin's
     * \link ctkPluginActivator::start(ctkPluginContext*) ctkPluginActivator start\endlink method
     * has been executed.
     *
     * @see ctkPlugin::start()
     */
    STARTED,

    /**
     * The plugin has been stopped.
     * <p>
     * The plugin's
     * \link ctkPluginActivator::stop(ctkPluginContext*) ctkPluginActivator stop\endlink method
     * has been executed.
     *
     * @see ctkPlugin::stop()
     */
    STOPPED,

    /**
     * The plugin has been updated.
     *
     * @see ctkPlugin::update()
     */
    UPDATED,

    /**
     * The plugin has been uninstalled.
     *
     * @see ctkPlugin::uninstall()
     */
    UNINSTALLED,

    /**
     * The plugin has been resolved.
     *
     * @see ctkPlugin::RESOLVED
     */
    RESOLVED,

    /**
     * The plugin has been unresolved.
     *
     * @see ctkPlugin::INSTALLED
     */
    UNRESOLVED,

    /**
     * The plugin is about to be activated.
     * <p>
     * The plugin's
     * \link ctkPluginActivator::start(ctkPluginContext*) ctkPluginActivator start\endlink method
     * is about to be called. This
     * event is only delivered to synchronuous slots, which have been registered with
     * Qt::DirectConnection or Qt::BlockingQueuedConnection in ctkPluginContext::connectPluginListener().
     *
     * @see ctkPlugin::start()
     */
    STARTING,

    /**
     * The plugin is about to deactivated.
     * <p>
     * The plugin's
     * \link ctkPluginActivator::stop(ctkPluginContext*) ctkPluginActivator stop\endlink method
     * is about to be called. This
     * event is only delivered to synchronuous slots, which have been registered with
     * Qt::DirectConnection or Qt::BlockingQueuedConnection in ctkPluginContext::connectPluginListener().
     *
     * @see ctkPlugin::stop()
     */
    STOPPING,

    /**
     * The plugin will be lazily activated.
     * <p>
     * The plugin has a \link ctkPluginConstants::ACTIVATION_LAZY lazy activation policy\endlink
     * and is waiting to be activated. It is now in the
     * \link ctkPlugin::STARTING STARTING\endlink state and has a valid
     * <code>ctkPluginContext</code>. This
     * event is only delivered to synchronuous slots, which have been registered with
     * Qt::DirectConnection or Qt::BlockingQueuedConnection in ctkPluginContext::connectPluginListener().
     */
    LAZY_ACTIVATION
  };

  /**
   * Default constructor for use with the Qt meta object system.
   */
  ctkPluginEvent();

  ~ctkPluginEvent();

  /**
   * Can be used to check if this ctkPluginEvent instance is valid,
   * or if it has been constructed using the default constructor.
   *
   * @return <code>true</code> if this event object is valid,
   *         <code>false</code> otherwise.
   */
  bool isNull() const;

  /**
   * Creates a plugin event of the specified type.
   *
   * @param type The event type.
   * @param plugin The plugin which had a lifecycle change.
   */
  ctkPluginEvent(Type type, QSharedPointer<ctkPlugin> plugin);

  ctkPluginEvent(const ctkPluginEvent& other);

  ctkPluginEvent& operator=(const ctkPluginEvent& other);

  /**
   * Returns the plugin which had a lifecycle change.
   *
   * @return The plugin that had a change occur in its lifecycle.
   */
  QSharedPointer<ctkPlugin> getPlugin() const;

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

Q_DECLARE_METATYPE(ctkPluginEvent)

/**
 * \ingroup PluginFramework
 * @{
 */
CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug debug, ctkPluginEvent::Type eventType);
CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug debug, const ctkPluginEvent& event);
/** @}*/

#endif // CTKPLUGINEVENT_H
