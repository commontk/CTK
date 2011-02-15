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


#ifndef CTKSERVICETRACKERCUSTOMIZER_H
#define CTKSERVICETRACKERCUSTOMIZER_H

#include <ctkServiceReference.h>


/**
 * \ingroup PluginFramework
 *
 * The <code>ctkServiceTrackerCustomizer</code> interface allows a
 * <code>ctkServiceTracker</code> to customize the service objects that are
 * tracked. A <code>ctkServiceTrackerCustomizer</code> is called when a service is
 * being added to a <code>ctkServiceTracker</code>. The
 * <code>ctkServiceTrackerCustomizer</code> can then return an object for the
 * tracked service. A <code>ctkServiceTrackerCustomizer</code> is also called when
 * a tracked service is modified or has been removed from a
 * <code>ctkServiceTracker</code>.
 *
 * <p>
 * The methods in this interface may be called as the result of a
 * <code>ctkServiceEvent</code> being received by a <code>ctkServiceTracker</code>.
 * Since <code>ctkServiceEvent</code>s are synchronously delivered by the
 * Framework, it is highly recommended that implementations of these methods do
 * not register (<code>ctkPluginContext::registerService</code>), modify (
 * <code>ctkServiceRegistration::setProperties</code>) or unregister (
 * <code>ctkServiceRegistration::unregister</code>) a service while being
 * synchronized on any object.
 *
 * <p>
 * The <code>ctkServiceTracker</code> class is thread-safe. It does not call a
 * <code>ctkServiceTrackerCustomizer</code> while holding any locks.
 * <code>ctkServiceTrackerCustomizer</code> implementations must also be
 * thread-safe.
 *
 * \tparam T The type of the tracked object.
 * \remarks This class is thread safe.
 */
template<class T = QObject*>
struct ctkServiceTrackerCustomizer {

  virtual ~ctkServiceTrackerCustomizer() {}

  /**
   * A service is being added to the <code>ctkServiceTracker</code>.
   *
   * <p>
   * This method is called before a service which matched the search
   * parameters of the <code>ctkServiceTracker</code> is added to the
   * <code>ctkServiceTracker</code>. This method should return the service object
   * to be tracked for the specified <code>ctkServiceReference</code>. The
   * returned service object is stored in the <code>ctkServiceTracker</code> and
   * is available from the <code>getService</code> and
   * <code>getServices</code> methods.
   *
   * @param reference The reference to the service being added to the
   *        <code>ctkServiceTracker</code>.
   * @return The service object to be tracked for the specified referenced
   *         service or <code>0</code> if the specified referenced service
   *         should not be tracked.
   */
  virtual T addingService(const ctkServiceReference& reference) = 0;

  /**
   * A service tracked by the <code>ctkServiceTracker</code> has been modified.
   *
   * <p>
   * This method is called when a service being tracked by the
   * <code>ctkServiceTracker</code> has had it properties modified.
   *
   * @param reference The reference to the service that has been modified.
   * @param service The service object for the specified referenced service.
   */
  virtual void modifiedService(const ctkServiceReference& reference, T service) = 0;

  /**
   * A service tracked by the <code>ctkServiceTracker</code> has been removed.
   *
   * <p>
   * This method is called after a service is no longer being tracked by the
   * <code>ctkServiceTracker</code>.
   *
   * @param reference The reference to the service that has been removed.
   * @param service The service object for the specified referenced service.
   */
  virtual void removedService(const ctkServiceReference& reference, T service) = 0;
};

#endif // CTKSERVICETRACKERCUSTOMIZER_H
