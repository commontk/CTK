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

#include "ctkServiceRegistration.h"

#include <QMutex>

namespace ctk {

  class ServiceRegistrationPrivate
  {

  private:

    /**
     * Lock object for synchronous event delivery.
     */
    QMutex eventLock;


  public:

    /**
     * Plugin registering this service.
     */
    Plugin* plugin;

    /**
     * Service or ServiceFactory object.
     */
    QObject* service;

    /**
     * Reference object to this service registration.
     */
    ServiceReference reference;

    /**
     * Service properties.
     */
    PluginContext::ServiceProperties properties;

    /**
     * Plugins dependent on this service. Integer is used as
     * reference counter, counting number of unbalanced getService().
     */
    QHash<Plugin*,int> dependents;

    /**
     * Object instances that factory has produced.
     */
    QHash<Plugin*, QObject*> serviceInstances;

    /**
     * Is service available. I.e., if <code>true</code> then holders
     * of a ServiceReference for the service are allowed to get it.
     */
    volatile bool available;

    /**
     * Avoid recursive unregistrations. I.e., if <code>true</code> then
     * unregistration of this service has started but is not yet
     * finished.
     */
    volatile bool unregistering;

    QMutex propsLock;

    ServiceRegistrationPrivate(Plugin* plugin, QObject* service,
                               const PluginContext::ServiceProperties& props)
                                 : plugin(plugint), service(service), reference(this),
                                 properties(props), available(true), unregistering(false)
    {

    }

    /**
     * Check if a plugin uses this service
     *
     * @param p Plugin to check
     * @return true if plugin uses this service
     */
    bool isUsedByPlugin(Plugin* p)
    {
      QHash<Plugin*, int> deps = dependents;
      return deps.contains(p);
    }

  };

  ServiceReference ServiceRegistration::getReference() const
  {
    if (!available) throw std::logic_error("Service is unregistered");

    return reference;
  }

  void ServiceRegistration::setProperties(const PluginContext::ServiceProperties& properties)
  {
    QMutexLocker lock(eventLock);
          Set before;
          // TBD, optimize the locking of services
          synchronized (bundle.fwCtx.services) {
            synchronized (properties) {
              if (available) {
                // NYI! Optimize the MODIFIED_ENDMATCH code
                Object old_rank = properties.get(Constants.SERVICE_RANKING);
                before = bundle.fwCtx.listeners.getMatchingServiceListeners(reference);
                String[] classes = (String[])properties.get(Constants.OBJECTCLASS);
                Long sid = (Long)properties.get(Constants.SERVICE_ID);
                properties = new PropertiesDictionary(props, classes, sid);
                Object new_rank = properties.get(Constants.SERVICE_RANKING);
                if (old_rank != new_rank && new_rank instanceof Integer &&
                    !((Integer)new_rank).equals(old_rank)) {
                  bundle.fwCtx.services.updateServiceRegistrationOrder(this, classes);
                }
              } else {
                throw new IllegalStateException("Service is unregistered");
              }
            }
          }
          bundle.fwCtx.listeners
            .serviceChanged(bundle.fwCtx.listeners.getMatchingServiceListeners(reference),
                            new ServiceEvent(ServiceEvent.MODIFIED, reference),
                            before);
          bundle.fwCtx.listeners
            .serviceChanged(before,
                            new ServiceEvent(ServiceEvent.MODIFIED_ENDMATCH, reference),
                            null);

  }

  void ServiceRegistration::unregister() const
  {

  }

}
