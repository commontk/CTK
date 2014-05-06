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


#ifndef CTKSERVICEREGISTRATIONPRIVATE_H
#define CTKSERVICEREGISTRATIONPRIVATE_H

#include <QHash>
#include <QMutex>

#include "ctkServiceProperties_p.h"
#include "ctkServiceReference.h"


class ctkPluginPrivate;
class ctkServiceRegistration;

/**
 * \ingroup PluginFramework
 */
class ctkServiceRegistrationPrivate
{

protected:

  friend class ctkServiceRegistration;

  // The ctkServiceReferencePrivate class holds a pointer to a
  // ctkServiceRegistrationPrivate instance and needs to manipulate
  // its reference count. This way it can keep the ctkServiceRegistrationPrivate
  // instance alive and keep returning service properties for
  // unregistered service instances.
  friend class ctkServiceReferencePrivate;

  /**
   * Reference count for implicitly shared private implementation.
   */
  QAtomicInt ref;

  /**
   * Service or ctkServiceFactory object.
   */
  QObject* service;

public:

  /**
   * Plugin registering this service.
   */
  ctkPluginPrivate* plugin;

  /**
   * Reference object to this service registration.
   */
  ctkServiceReference reference;

  /**
   * Service properties.
   */
  ctkServiceProperties properties;

  /**
   * Plugins dependent on this service. Integer is used as
   * reference counter, counting number of unbalanced getService().
   */
  QHash<QSharedPointer<ctkPlugin>,int> dependents;

  /**
   * Object instances that factory has produced.
   */
  QHash<QSharedPointer<ctkPlugin>, QObject*> serviceInstances;

  /**
   * Is service available. I.e., if <code>true</code> then holders
   * of a ctkServiceReference for the service are allowed to get it.
   */
  volatile bool available;

  /**
   * Avoid recursive unregistrations. I.e., if <code>true</code> then
   * unregistration of this service has started but is not yet
   * finished.
   */
  volatile bool unregistering;

  /**
   * Lock object for synchronous event delivery.
   */
  QMutex eventLock;

  QMutex propsLock;

  ctkServiceRegistrationPrivate(ctkPluginPrivate* plugin, QObject* service,
                                const ctkDictionary& props);

  virtual ~ctkServiceRegistrationPrivate();

  /**
   * Check if a plugin uses this service
   *
   * @param p Plugin to check
   * @return true if plugin uses this service
   */
  bool isUsedByPlugin(QSharedPointer<ctkPlugin> p);

  virtual QObject* getService();

private:

  Q_DISABLE_COPY(ctkServiceRegistrationPrivate)

};


#endif // CTKSERVICEREGISTRATIONPRIVATE_H
