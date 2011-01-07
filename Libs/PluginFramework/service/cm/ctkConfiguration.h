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


#ifndef CTKCONFIGURATION_H
#define CTKCONFIGURATION_H

#include <QSharedPointer>

#include "ctkPluginFramework_global.h"


/**
 * The configuration information for a {@code ctkManagedService} or
 * {@code ctkManagedServiceFactory} object.
 *
 * The Configuration Admin service uses this interface to represent the
 * configuration information for a {@code ctkManagedService} or for a
 * service instance of a {@code ctkManagedServiceFactory}.
 *
 * <p>
 * A {@code ctkConfiguration} object contains a configuration dictionary and
 * allows the properties to be updated via this object. Plugins wishing to
 * receive configuration dictionaries do not need to use this class - they
 * register a {@code ctkManagedService} or
 * {@code ctkManagedServiceFactory}. Only administrative plugins, and
 * plugins wishing to update their own configurations need to use this class.
 *
 * <p>
 * The properties handled in this configuration have case insensitive
 * {@code QString} objects as keys. However, case is preserved from the
 * last set key/value.
 * <p>
 * A configuration can be <i>bound </i> to a plugin location (
 * {@code ctkPlugin#getLocation()}). The purpose of binding a
 * {@code ctkConfiguration} object to a location is to make it impossible
 * for another plugin to forge a PID that would match this configuration. When a
 * configuration is bound to a specific location, and a plugin with a different
 * location registers a corresponding {@code ctkManagedService} object or
 * {@code ctkManagedServiceFactory} object, then the configuration is not
 * passed to the updated method of that object.
 *
 * <p>
 * If a configuration's location is empty, it is not yet bound to
 * a location. It will become bound to the location of the first plugin that
 * registers a {@code ctkManagedService} or
 * {@code ctkManagedServiceFactory} object with the corresponding PID.
 * <p>
 * The same {@code ctkConfiguration} object is used for configuring both a
 * Managed Service Factory and a Managed Service. When it is important to
 * differentiate between these two the term "factory configuration" is used.
 *
 * @noimplement
 * @version
 */
struct CTK_PLUGINFW_EXPORT ctkConfiguration
{

  virtual ~ctkConfiguration();

  /**
   * Get the PID for this {@code ctkConfiguration} object.
   *
   * @return the PID for this {@code ctkConfiguration} object.
   * @throws std::logic_error if this configuration has been deleted
   */
  virtual QString getPid() const = 0;

  /**
   * Return the properties of this {@code ctkConfiguration} object.
   *
   * The {@code Dictionary} object returned is a private copy for the
   * caller and may be changed without influencing the stored configuration.
   * The keys in the returned dictionary are case insensitive and are always
   * of type {@code String}.
   *
   * <p>
   * If called just after the configuration is created and before update has
   * been called, this method returns {@code null}.
   *
   * @return A private copy of the properties for the caller or
   *         {@code null}. These properties must not contain the
   *         "service.bundleLocation" property. The value of this property may
   *         be obtained from the {@code getBundleLocation} method.
   * @throws IllegalStateException if this configuration has been deleted
   */
  virtual ctkDictionary getProperties() const = 0;

  /**
   * Update the properties of this {@code ctkConfiguration} object.
   *
   * Stores the properties in persistent storage after adding or overwriting
   * the following properties:
   * <ul>
   * <li>"service.pid" : is set to be the PID of this configuration.</li>
   * <li>"service.factoryPid" : if this is a factory configuration it is set
   * to the factory PID else it is not set.</li>
   * </ul>
   * These system properties are all of type {@code QString}.
   *
   * <p>
   * If the corresponding Managed Service/Managed Service Factory is
   * registered, its updated method must be called asynchronously. Else, this
   * callback is delayed until aforementioned registration occurs.
   *
   * <p>
   * Also initiates an asynchronous call to all
   * {@code ctkConfigurationListener}s with a
   * {@code ctkConfigurationEvent::CM_UPDATED} event.
   *
   * @param properties the new set of properties for this configuration
   * @throws ctkIOException if update cannot be made persistent
   * @throws std::invalid_argument if the {@code ctkDictionary} object
   *         contains invalid configuration types or contains case variants of
   *         the same key name.
   * @throws std::logic_error if this configuration has been deleted
   */
  virtual void update(const ctkDictionary& properties) = 0;

  /**
   * Delete this {@code ctkConfiguration} object.
   *
   * Removes this configuration object from the persistent store. Notify
   * asynchronously the corresponding Managed Service or Managed Service
   * Factory. A {@code ctkManagedService} object is notified by a call to
   * its {@code updated} method with a {@code null} properties
   * argument. A {@code ctkManagedServiceFactory} object is notified by a
   * call to its {@code deleted} method.
   *
   * <p>
   * Also initiates an asynchronous call to all
   * {@code ctkConfigurationListener}s with a
   * {@code ctkConfigurationEvent::CM_DELETED} event.
   *
   * @throws ctkIOException If delete fails
   * @throws std::logic_error if this configuration has been deleted
   */
  virtual void remove() = 0;

  /**
   * For a factory configuration return the PID of the corresponding Managed
   * Service Factory, else return a null QString.
   *
   * @return factory PID or {@code null}
   * @throws std::logic_error if this configuration has been deleted
   */
  virtual QString getFactoryPid() const = 0;

  /**
   * Update the {@code ctkConfiguration} object with the current
   * properties.
   *
   * Initiate the {@code updated} callback to the Managed Service or
   * Managed Service Factory with the current properties asynchronously.
   *
   * <p>
   * This is the only way for a plugin that uses a Configuration Plugin
   * service to initiate a callback. For example, when that plugin detects a
   * change that requires an update of the Managed Service or Managed Service
   * Factory via its {@code ctkConfigurationPlugin} object.
   *
   * @see ctkConfigurationPlugin
   * @throws ctkIOException if update cannot access the properties in persistent
   *         storage
   * @throws std::logic_error if this configuration has been deleted
   */
  virtual void update() = 0;

  /**
   * Bind this {@code ctkConfiguration} object to the specified plugin
   * location.
   *
   * If the pluginLocation parameter is {@code null} then the
   * {@code ctkConfiguration} object will not be bound to a location. It
   * will be set to the plugin's location before the first time a Managed
   * Service/Managed Service Factory receives this {@code ctkConfiguration}
   * object via the updated method and before any plugins are called. The
   * plugin location will be set persistently.
   *
   * @param pluginLocation a plugin location or {@code null}.
   * @throws std::logic_error If this configuration has been deleted.
   * @throws ctkSecurityException If the caller does not have
   *         {@code ctkConfigurationPermission[*,CONFIGURE]}.
   */
  virtual void setPluginLocation(const QString& pluginLocation) = 0;

  /**
   * Get the plugin location.
   *
   * Returns the plugin location to which this configuration is bound, or
   * {@code null} if it is not yet bound to a plugin location.
   *
   * @return location to which this configuration is bound, or
   *         {@code null}.
   * @throws std::logic_error If this {@code Configuration} object
   *         has been deleted.
   * @throws ctkSecurityException If the caller does not have
   *         {@code ctkConfigurationPermission[*,CONFIGURE]}.
   */
  virtual QString getPluginLocation() const = 0;

  /**
   * Equality is defined to have equal PIDs
   *
   * Two Configuration objects are equal when their PIDs are equal.
   *
   * @param other {@code ctkConfiguration} object to compare against
   * @return {@code true} if equal, {@code false} if the PID's differ.
   */
  bool operator==(const ctkConfiguration& other) const;

};

typedef QSharedPointer<ctkConfiguration> ctkConfigurationPtr;

/**
 * Hash code is based on PID.
 *
 * The hashcode for two ctkConfiguration objects must be the same when the
 * Configuration PID's are the same.
 *
 * @param configuration The configuration object for which to compute the hash value.
 * @return hash code for this ctkConfiguration object
 */
uint CTK_PLUGINFW_EXPORT qHash(ctkConfigurationPtr configuration);

bool CTK_PLUGINFW_EXPORT operator==(const ctkConfigurationPtr& c1, const ctkConfigurationPtr c2);

#endif // CTKCONFIGURATION_H
