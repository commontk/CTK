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


#ifndef CTKCONFIGURATIONADMIN_H
#define CTKCONFIGURATIONADMIN_H

#include "ctkConfiguration.h"

/**
 * \ingroup ConfigAdmin
 * Service for administering configuration data.
 *
 * <p>
 * The main purpose of this interface is to store plugin configuration data
 * persistently. This information is represented in <code>ctkConfiguration</code>
 * objects. The actual configuration data is a <code>ctkDictionary</code> of
 * properties inside a <code>ctkConfiguration</code> object.
 *
 * <p>
 * There are two principally different ways to manage configurations. First
 * there is the concept of a Managed Service, where configuration data is
 * uniquely associated with an object registered with the service registry.
 *
 * <p>
 * Next, there is the concept of a factory where the Configuration Admin service
 * will maintain 0 or more <code>ctkConfiguration</code> objects for a Managed
 * Service Factory that is registered with the Framework.
 *
 * <p>
 * The first concept is intended for configuration data about "things/services"
 * whose existence is defined externally, e.g. a specific printer. Factories are
 * intended for "things/services" that can be created any number of times, e.g.
 * a configuration for a DHCP server for different networks.
 *
 * <p>
 * Plugins that require configuration should register a Managed Service or a
 * Managed Service Factory in the service registry. A registration property
 * named <code>service.pid</code> (persistent identifier or PID) must be used to
 * identify this Managed Service or Managed Service Factory to the Configuration
 * Admin service.
 *
 * <p>
 * When the ConfigurationAdmin detects the registration of a Managed Service, it
 * checks its persistent storage for a configuration object whose
 * <code>service.pid</code> property matches the PID service property (
 * <code>service.pid</code>) of the Managed Service. If found, it calls
 * ctkManagedService#updated method with the new properties. The
 * implementation of a Configuration Admin service must run these call-backs
 * asynchronously to allow proper synchronization.
 *
 * <p>
 * When the Configuration Admin service detects a Managed Service Factory
 * registration, it checks its storage for configuration objects whose
 * <code>service.factoryPid</code> property matches the PID service property of
 * the Managed Service Factory. For each such <code>ctkConfiguration</code>
 * objects, it calls the <code>ctkManagedServiceFactory#updated</code> method
 * asynchronously with the new properties. The calls to the <code>%updated</code>
 * method of a <code>ctkManagedServiceFactory</code> must be executed sequentially
 * and not overlap in time.
 *
 * <p>
 * In general, plugins having permission to use the Configuration Admin service
 * can only access and modify their own configuration information. Accessing or
 * modifying the configuration of another bundle requires
 * <code>ctkConfigurationPermission[*,CONFIGURE]</code>.
 *
 * <p>
 * <code>ctkConfiguration</code> objects can be <i>bound </i> to a specified plugin
 * location. In this case, if a matching Managed Service or Managed Service
 * Factory is registered by a plugin with a different location, then the
 * Configuration Admin service must not do the normal callback, and it should
 * log an error. In the case where a <code>ctkConfiguration</code> object is not
 * bound, its location field is <code>null</code>, the Configuration Admin
 * service will bind it to the location of the plugin that registers the first
 * Managed Service or Managed Service Factory that has a corresponding PID
 * property. When a <code>ctkConfiguration</code> object is bound to a plugin
 * location in this manner, the Configuration Admin service must detect if the
 * plugin corresponding to the location is uninstalled. If this occurs, the
 * <code>ctkConfiguration</code> object is unbound, that is its location field is
 * set back to <code>null</code>.
 *
 * <p>
 * The method descriptions of this class refer to a concept of "the calling
 * plugin". This is a loose way of referring to the plugin which obtained the
 * Configuration Admin service from the service registry. Implementations of
 * <code>ctkConfigurationAdmin</code> must use a
 * {@link ctkServiceFactory} to support this concept.
 *
 */
struct CTK_PLUGINFW_EXPORT ctkConfigurationAdmin
{

  /**
   * Configuration property naming the Factory PID in the configuration
   * dictionary. The property's value is of type <code>QString</code>.
   */
  static const QString SERVICE_FACTORYPID; // = "service.factoryPid";

  /**
   * Configuration property naming the location of the plugin that is
   * associated with a a <code>ctkConfiguration</code> object. This property can
   * be searched for but must not appear in the configuration dictionary for
   * security reason. The property's value is of type <code>QString</code>.
   */
  static const QString SERVICE_PLUGINLOCATION; // = "service.pluginLocation";

  /**
   * Create a new factory <code>ctkConfiguration</code> object with a new PID.
   *
   * The properties of the new <code>ctkConfiguration</code> object are
   * <code>null</code> until the first time that its
   * {@link ctkConfiguration#update(const ctkDictionary&)} method is called.
   *
   * <p>
   * It is not required that the <code>factoryPid</code> maps to a
   * registered Managed Service Factory.
   * <p>
   * The <code>ctkConfiguration</code> object is bound to the location of the
   * calling plugin.
   *
   * @param factoryPid PID of factory (not <code>null</code>).
   * @return A new <code>ctkConfiguration</code> object.
   * @throws ctkIOException if access to persistent storage fails.
   * @throws ctkSecurityException if caller does not have
   *         <code>ctkConfigurationPermission[*,CONFIGURE]</code> and
   *         <code>factoryPid</code> is bound to another plugin.
   */
  virtual ctkConfigurationPtr createFactoryConfiguration(const QString& factoryPid) = 0;

  /**
   * Create a new factory <code>ctkConfiguration</code> object with a new PID.
   *
   * The properties of the new <code>ctkConfiguration</code> object are
   * <code>null</code> until the first time that its
   * {@link ctkConfiguration#update(const ctkDictionary&)} method is called.
   *
   * <p>
   * It is not required that the <code>factoryPid</code> maps to a
   * registered Managed Service Factory.
   *
   * <p>
   * The <code>ctkConfiguration</code> is bound to the location specified. If
   * this location is <code>null</code> it will be bound to the location of
   * the first plugin that registers a Managed Service Factory with a
   * corresponding PID.
   *
   * @param factoryPid PID of factory (not <code>null</code>).
   * @param location A plugin location string, or <code>null</code>.
   * @return a new <code>ctkConfiguration</code> object.
   * @throws ctkIOException if access to persistent storage fails.
   * @throws ctkSecurityException if caller does not have <code>ctkConfigurationPermission[*,CONFIGURE]</code>.
   */
 virtual ctkConfigurationPtr createFactoryConfiguration(const QString& factoryPid, const QString& location) = 0;

  /**
   * Get an existing <code>ctkConfiguration</code> object from the persistent
   * store, or create a new <code>ctkConfiguration</code> object.
   *
   * <p>
   * If a <code>ctkConfiguration</code> with this PID already exists in
   * Configuration Admin service return it. The location parameter is ignored
   * in this case.
   *
   * <p>
   * Else, return a new <code>ctkConfiguration</code> object. This new object
   * is bound to the location and the properties are set to <code>null</code>.
   * If the location parameter is <code>null</code>, it will be set when a
   * Managed Service with the corresponding PID is registered for the first
   * time.
   *
   * @param pid Persistent identifier.
   * @param location The plugin location string, or <code>null</code>.
   * @return An existing or new <code>ctkConfiguration</code> object.
   * @throws ctkIOException if access to persistent storage fails.
   * @throws ctkSecurityException if the caller does not have <code>ctkConfigurationPermission[*,CONFIGURE]</code>.
   */
 virtual ctkConfigurationPtr getConfiguration(const QString& pid, const QString& location) = 0;

  /**
   * Get an existing or new <code>ctkConfiguration</code> object from the
   * persistent store.
   *
   * If the <code>ctkConfiguration</code> object for this PID does not exist,
   * create a new <code>ctkConfiguration</code> object for that PID, where
   * properties are <code>null</code>. Bind its location to the calling
   * plugin's location.
   *
   * <p>
   * Otherwise, if the location of the existing <code>ctkConfiguration</code> object
   * is <code>null</code>, set it to the calling plugin's location.
   *
   * @param pid persistent identifier.
   * @return an existing or new <code>ctkConfiguration</code> matching the PID.
   * @throws ctkIOException if access to persistent storage fails.
   * @throws ctkSecurityException if the <code>ctkConfiguration</code> object is
   *         bound to a location different from that of the calling plugin and it
   *         has no <code>ctkConfigurationPermission[*,CONFIGURE]</code>.
   */
 virtual ctkConfigurationPtr getConfiguration(const QString& pid) = 0;

  /**
   * List the current <code>ctkConfiguration</code> objects which match the
   * filter.
   *
   * <p>
   * Only <code>ctkConfiguration</code> objects with non- <code>null</code>
   * properties are considered current. That is,
   * <code>ctkConfiguration#getProperties()</code> is guaranteed not to return
   * an empty list for each of the returned <code>ctkConfiguration</code>
   * objects.
   *
   * <p>
   * Normally only <code>ctkConfiguration</code> objects that are bound to the
   * location of the calling plugin are returned, or all if the caller has
   * <code>ctkConfigurationPermission[*,CONFIGURE]</code>.
   *
   * <p>
   * The syntax of the filter string is as defined in the
   * {@link ctkLDAPSearchFilter} class. The filter can test any
   * configuration properties including the following:
   * <ul>
   * <li><code>service.pid</code>-<code>QString</code>- the PID under which
   * this is registered</li>
   * <li><code>service.factoryPid</code>-<code>QString</code>- the factory if
   * applicable</li>
   * <li><code>service.pluginLocation</code>-<code>QString</code>- the plugin
   * location</li>
   * </ul>
   * The filter can also be empty, meaning that all
   * <code>ctkConfiguration</code> objects should be returned.
   *
   * @param filter A filter string, or an empty string to retrieve all
   *        <code>ctkConfiguration</code> objects.
   * @return All matching <code>ctkConfiguration</code> objects, or
   *         an empty list if there aren't any.
   * @throws ctkIOException if access to persistent storage fails
   * @throws ctkInvalidArgumentException if the filter string is invalid
   */
  virtual QList<ctkConfigurationPtr> listConfigurations(const QString& filter = QString()) = 0;
};

Q_DECLARE_INTERFACE(ctkConfigurationAdmin, "org.commontk.service.cm.ConfigurationAdmin")

#endif // CTKCONFIGURATIONADMIN_H
