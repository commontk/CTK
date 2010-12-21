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


#ifndef CTKCONFIGURATIONEVENT_H
#define CTKCONFIGURATIONEVENT_H

#include <QSharedDataPointer>
#include <QDebug>

#include "ctkServiceReference.h"

class ctkConfigurationEventData;

/**
 * A Configuration Event.
 *
 * <p>
 * {@code ctkConfigurationEvent} objects are delivered to all registered
 * {@code ctkConfigurationListener} service objects. ctkConfigurationEvents
 * must be asynchronously delivered in chronological order with respect to each
 * listener.
 *
 * <p>
 * An enum type is used to identify the type of event. The following event types
 * are defined:
 * <ul>
 * <li>{@link #CM_UPDATED}
 * <li>{@link #CM_DELETED}
 * </ul>
 * Additional event types may be defined in the future.
 *
 * <p>
 * Security Considerations. {@code ctkConfigurationEvent} objects do not
 * provide {@code ctkConfiguration} objects, so no sensitive configuration
 * information is available from the event. If the listener wants to locate the
 * {@code ctkConfiguration} object for the specified pid, it must use
 * {@code ctkConfigurationAdmin}.
 *
 * @see ctkConfigurationListener
 */
class CTK_PLUGINFW_EXPORT ctkConfigurationEvent
{

  QSharedDataPointer<ctkConfigurationEventData> d;

public:

  enum Type {

    /**
     * A {@code ctkConfiguration} has been updated.
     *
     * <p>
     * This {@code ctkConfigurationEvent} type indicates that a
      {@code ctkConfiguration} object has been updated with new properties.
     *
     * An event is fired when a call to {@link ctkConfiguration#update(const ctkDictionary&)}
     * successfully changes a configuration.
     *
     * <p>
     * The value of {@code CM_UPDATED} is 1.
     */
    CM_UPDATED  = 0x00000001,

    /**
     * A {@code ctkConfiguration} has been deleted.
     *
     * <p>
     * This {@code ctkConfigurationEvent} type indicates that a
     * {@code ctkConfiguration} object has been deleted.
     *
     * An event is fired when a call to {@link ctkConfiguration#remove()}
     * successfully deletes a configuration.
     *
     * <p>
     * The value of {@code CM_DELETED} is 2.
     */
    CM_DELETED  = 0x00000002

  };

  /**
   * Default constructor for use with the Qt meta object system.
   */
  ctkConfigurationEvent();

  ~ctkConfigurationEvent();

  /**
   * Can be used to check if this ctkConfigurationEvent instance is valid,
   * or if it has been constructed using the default constructor.
   *
   * @return <code>true</code> if this event object is valid,
   *         <code>false</code> otherwise.
   */
  bool isNull() const;

  /**
   * Constructs a {@code ConfigurationEvent} object from the given
   * {@code ServiceReference} object, event type, and pids.
   *
   * @param reference The {@code ServiceReference} object of the
   *        Configuration Admin service that created this event.
   * @param type The event type. See {@link #getType}.
   * @param factoryPid The factory pid of the associated configuration if the
   *        target of the configuration is a ManagedServiceFactory. Otherwise
   *        {@code null} if the target of the configuration is a
   *        ManagedService.
   * @param pid The pid of the associated configuration.
   */
  ctkConfigurationEvent(const ctkServiceReference& reference,
                        Type type, const QString& factoryPid,
                        const QString& pid);

  ctkConfigurationEvent(const ctkConfigurationEvent& other);

  ctkConfigurationEvent& operator=(const ctkConfigurationEvent& other);

  /**
   * Returns the factory pid of the associated configuration.
   *
   * @return Returns the factory pid of the associated configuration if the
   *         target of the configuration is a ctkManagedServiceFactory. Otherwise
   *         an invalid string if the target of the configuration is a
   *         ctkManagedService.
   */
  QString getFactoryPid() const;

  /**
   * Returns the pid of the associated configuration.
   *
   * @return Returns the pid of the associated configuration.
   */
  QString getPid() const;

  /**
   * Return the type of this event.
   * <p>
   * The type values are:
   * <ul>
   * <li>{@link #CM_UPDATED}
   * <li>{@link #CM_DELETED}
   * </ul>
   *
   * @return The type of this event.
   */
  int getType() const;

  /**
   * Return the {@code ctkServiceReference} object of the Configuration
   * Admin service that created this event.
   *
   * @return The {@code ctkServiceReference} object for the Configuration
   *         Admin service that created this event.
   */
  ctkServiceReference getReference() const;
};

CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, ctkConfigurationEvent::Type type);
CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, const ctkConfigurationEvent& event);


#endif // CTKCONFIGURATIONEVENT_H
