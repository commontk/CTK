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

#ifndef CTKSERVICEREGISTRATION_H
#define CTKSERVICEREGISTRATION_H

#include "ctkPluginContext.h"

#include "ctkServiceReference.h"

namespace ctk {

  class ServiceRegistrationPrivate;

  /**
   * A registered service.
   *
   * <p>
   * The Framework returns a <code>ServiceRegistration</code> object when a
   * <code>PluginContext#registerService()</code> method invocation is successful.
   * The <code>ServiceRegistration</code> object is for the private use of the
   * registering plugin and should not be shared with other plugins.
   * <p>
   * The <code>ServiceRegistration</code> object may be used to update the
   * properties of the service or to unregister the service.
   *
   * @see PluginContext#registerService()
   * @threadsafe
   */
  class ServiceRegistration {

    Q_DECLARE_PRIVATE(ServiceRegistration)

  public:

    /**
     * Returns a <code>ServiceReference</code> object for a service being
     * registered.
     * <p>
     * The <code>ServiceReference</code> object may be shared with other
     * plugins.
     *
     * @throws std::logic_error If this
     *         <code>ServiceRegistration</code> object has already been
     *         unregistered.
     * @return <code>ServiceReference</code> object.
     */
    ServiceReference getReference() const;

    /**
     * Updates the properties associated with a service.
     *
     * <p>
     * The {@link PluginConstants#OBJECTCLASS} and {@link PluginConstants#SERVICE_ID} keys
     * cannot be modified by this method. These values are set by the Framework
     * when the service is registered in the environment.
     *
     * <p>
     * The following steps are required to modify service properties:
     * <ol>
     * <li>The service's properties are replaced with the provided properties.
     * <li>A service event of type {@link ServiceEvent#MODIFIED} is fired.
     * </ol>
     *
     * @param properties The properties for this service. See {@link PluginConstants}
     *        for a list of standard service property keys. Changes should not
     *        be made to this object after calling this method. To update the
     *        service's properties this method should be called again.
     *
     * @throws std::logic_error If this <code>ServiceRegistration</code>
     *         object has already been unregistered.
     * @throws std::invalid_argument If <code>properties</code> contains
     *         case variants of the same key name.
     */
    void setProperties(const PluginContext::ServiceProperties& properties);

    /**
     * Unregisters a service. Remove a <code>ServiceRegistration</code> object
     * from the Framework service registry. All <code>ServiceReference</code>
     * objects associated with this <code>ServiceRegistration</code> object
     * can no longer be used to interact with the service once unregistration is
     * complete.
     *
     * <p>
     * The following steps are required to unregister a service:
     * <ol>
     * <li>The service is removed from the Framework service registry so that
     * it can no longer be obtained.
     * <li>A service event of type {@link ServiceEvent#UNREGISTERING} is fired
     * so that plugins using this service can release their use of the service.
     * Once delivery of the service event is complete, the
     * <code>ServiceReference</code> objects for the service may no longer be
     * used to get a service object for the service.
     * <li>For each plugin whose use count for this service is greater than
     * zero: <br>
     * The plugin's use count for this service is set to zero. <br>
     * If the service was registered with a {@link ServiceFactory} object, the
     * <code>ServiceFactory#ungetService</code> method is called to release
     * the service object for the plugin.
     * </ol>
     *
     * @throws std::logic_error If this
     *         <code>ServiceRegistration</code> object has already been
     *         unregistered.
     * @see BundleContext#ungetService
     * @see ServiceFactory#ungetService
     */
    void unregister() const;

  private:

    ServiceRegistration(Plugin* plugin, QObject* service,
                        const PluginContext::ServiceProperties& props);

    ServiceRegistrationPrivate * const d_ptr;

  };

}

#endif // CTKSERVICEREGISTRATION_H
