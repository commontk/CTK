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


#ifndef CTKSERVICETRACKER_H
#define CTKSERVICETRACKER_H

#include <QScopedPointer>

#include "ctkPluginFrameworkExport.h"

#include "ctkServiceReference.h"
#include "ctkServiceTrackerCustomizer.h"
#include "ctkLDAPSearchFilter.h"

template<class S, class T> class ctkTrackedService;
template<class S, class T> class ctkServiceTrackerPrivate;
class ctkPluginContext;

/**
 * \ingroup PluginFramework
 *
 * The <code>ctkServiceTracker</code> class simplifies using services from the
 * Framework's service registry.
 * <p>
 * A <code>ctkServiceTracker</code> object is constructed with search criteria and
 * a <code>ctkServiceTrackerCustomizer</code> object. A <code>ctkServiceTracker</code>
 * can use a <code>ctkServiceTrackerCustomizer</code> to customize the service
 * objects to be tracked. The <code>ctkServiceTracker</code> can then be opened to
 * begin tracking all services in the Framework's service registry that match
 * the specified search criteria. The <code>ctkServiceTracker</code> correctly
 * handles all of the details of listening to <code>ctkServiceEvent</code>s and
 * getting and ungetting services.
 * <p>
 * The <code>getServiceReferences</code> method can be called to get references
 * to the services being tracked. The <code>getService</code> and
 * <code>getServices</code> methods can be called to get the service objects for
 * the tracked service.
 * <p>
 * The <code>ctkServiceTracker</code> class is thread-safe. It does not call a
 * <code>ctkServiceTrackerCustomizer</code> while holding any locks.
 * <code>ctkServiceTrackerCustomizer</code> implementations must also be
 * thread-safe.
 *
 * \tparam S The type of the service being tracked. The type must be an
 *         assignable datatype. Further, if the
 *         <code>ctkServiceTracker(ctkPluginContext*, ctkServiceTrackerCustomizer<T>*)</code>
 *         constructor is used, the type must have an associated interface id via
 *         Q_DECLARE_INTERFACE.
 * \tparam T The type of the tracked object. The type must be an assignable
 *         datatype, provide a boolean conversion function, and provide
 *         a constructor and an assignment operator which can handle 0 as an argument.
 * \remarks This class is thread safe.
 */
template<class S = QObject*, class T = S>
class ctkServiceTracker : protected ctkServiceTrackerCustomizer<T>
{
public:

  ~ctkServiceTracker();

  /**
   * Create a <code>ctkServiceTracker</code> on the specified
   * <code>ctkServiceReference</code>.
   *
   * <p>
   * The service referenced by the specified <code>ctkServiceReference</code>
   * will be tracked by this <code>ctkServiceTracker</code>.
   *
   * @param context The <code>ctkPluginContext</code> against which the tracking
   *        is done.
   * @param reference The <code>ctkServiceReference</code> for the service to be
   *        tracked.
   * @param customizer The customizer object to call when services are added,
   *        modified, or removed in this <code>ctkServiceTracker</code>. If
   *        customizer is <code>null</code>, then this
   *        <code>ctkServiceTracker</code> will be used as the
   *        <code>ctkServiceTrackerCustomizer</code> and this
   *        <code>ctkServiceTracker</code> will call the
   *        <code>ctkServiceTrackerCustomizer</code> methods on itself.
   */
  ctkServiceTracker(ctkPluginContext* context,
                    const ctkServiceReference& reference,
                    ctkServiceTrackerCustomizer<T>* customizer = 0);

  /**
   * Create a <code>ctkServiceTracker</code> on the specified class name.
   *
   * <p>
   * Services registered under the specified class name will be tracked by
   * this <code>ctkServiceTracker</code>.
   *
   * @param context The <code>ctkPluginContext</code> against which the tracking
   *        is done.
   * @param clazz The class name of the services to be tracked.
   * @param customizer The customizer object to call when services are added,
   *        modified, or removed in this <code>ctkServiceTracker</code>. If
   *        customizer is <code>null</code>, then this
   *        <code>ctkServiceTracker</code> will be used as the
   *        <code>ctkServiceTrackerCustomizer</code> and this
   *        <code>ctkServiceTracker</code> will call the
   *        <code>ctkServiceTrackerCustomizer</code> methods on itself.
   */
  ctkServiceTracker(ctkPluginContext* context, const QString& clazz,
                    ctkServiceTrackerCustomizer<T>* customizer = 0);

  /**
   * Create a <code>ctkServiceTracker</code> on the specified
   * <code>ctkLDAPSearchFilter</code> object.
   *
   * <p>
   * Services which match the specified <code>ctkLDAPSearchFilter</code> object will be
   * tracked by this <code>ctkServiceTracker</code>.
   *
   * @param context The <code>ctkPluginContext</code> against which the tracking
   *        is done.
   * @param filter The <code>ctkLDAPSearchFilter</code> to select the services to be
   *        tracked.
   * @param customizer The customizer object to call when services are added,
   *        modified, or removed in this <code>ctkServiceTracker</code>. If
   *        customizer is null, then this <code>ctkServiceTracker</code> will be
   *        used as the <code>ctkServiceTrackerCustomizer</code> and this
   *        <code>ctkServiceTracker</code> will call the
   *        <code>ctkServiceTrackerCustomizer</code> methods on itself.
   */
  ctkServiceTracker(ctkPluginContext* context, const ctkLDAPSearchFilter& filter,
                    ctkServiceTrackerCustomizer<T>* customizer = 0);

  /**
   * Create a <code>ctkServiceTracker</code> on the class template
   * argument S.
   *
   * <p>
   * Services registered under the interface name of the class template
   * argument S will be tracked by this <code>ctkServiceTracker</code>.
   *
   * @param context The <code>ctkPluginContext</code> against which the tracking
   *        is done.
   * @param customizer The customizer object to call when services are added,
   *        modified, or removed in this <code>ctkServiceTracker</code>. If
   *        customizer is null, then this <code>ctkServiceTracker</code> will be
   *        used as the <code>ctkServiceTrackerCustomizer</code> and this
   *        <code>ctkServiceTracker</code> will call the
   *        <code>ctkServiceTrackerCustomizer</code> methods on itself.
   */
  ctkServiceTracker(ctkPluginContext* context, ctkServiceTrackerCustomizer<T>* customizer = 0);

  /**
   * Open this <code>ctkServiceTracker</code> and begin tracking services.
   *
   * <p>
   * Services which match the search criteria specified when this
   * <code>ctkServiceTracker</code> was created are now tracked by this
   * <code>ctkServiceTracker</code>.
   *
   * @throws ctkIllegalStateException If the <code>ctkPluginContext</code>
   *         with which this <code>ctkServiceTracker</code> was created is no
   *         longer valid.
   */
  virtual void open();

  /**
   * Close this <code>ctkServiceTracker</code>.
   *
   * <p>
   * This method should be called when this <code>ctkServiceTracker</code> should
   * end the tracking of services.
   *
   * <p>
   * This implementation calls getServiceReferences() to get the list
   * of tracked services to remove.
   */
  virtual void close();

  /**
   * Wait for at least one service to be tracked by this
   * <code>ctkServiceTracker</code>. This method will also return when this
   * <code>ctkServiceTracker</code> is closed.
   *
   * <p>
   * It is strongly recommended that <code>waitForService</code> is not used
   * during the calling of the <code>ctkPluginActivator</code> methods.
   * <code>ctkPluginActivator</code> methods are expected to complete in a short
   * period of time.
   *
   * <p>
   * This implementation calls getService() to determine if a service
   * is being tracked.
   *
   * @param timeout The time interval in milliseconds to wait. If zero, the
   *        method will wait indefinitely.
   * @return Returns the result of getService().
   */
  virtual T waitForService(unsigned long timeout);

  /**
   * Return a list of <code>ctkServiceReference</code>s for all services being
   * tracked by this <code>ctkServiceTracker</code>.
   *
   * @return List of <code>ctkServiceReference</code>s.
   */
  virtual QList<ctkServiceReference> getServiceReferences() const;

  /**
   * Returns a <code>ctkServiceReference</code> for one of the services being
   * tracked by this <code>ctkServiceTracker</code>.
   *
   * <p>
   * If multiple services are being tracked, the service with the highest
   * ranking (as specified in its <code>service.ranking</code> property) is
   * returned. If there is a tie in ranking, the service with the lowest
   * service ID (as specified in its <code>service.id</code> property); that
   * is, the service that was registered first is returned. This is the same
   * algorithm used by <code>ctkPluginContext::getServiceReference()</code>.
   *
   * <p>
   * This implementation calls getServiceReferences() to get the list
   * of references for the tracked services.
   *
   * @return A <code>ctkServiceReference</code> for a tracked service.
   * @throws ctkServiceException if no services are being tracked.
   */
  virtual ctkServiceReference getServiceReference() const;

  /**
   * Returns the service object for the specified
   * <code>ctkServiceReference</code> if the specified referenced service is
   * being tracked by this <code>ctkServiceTracker</code>.
   *
   * @param reference The reference to the desired service.
   * @return A service object or <code>null</code> if the service referenced
   *         by the specified <code>ctkServiceReference</code> is not being
   *         tracked.
   */
  virtual T getService(const ctkServiceReference& reference) const;

  /**
   * Return a list of service objects for all services being tracked by this
   * <code>ctkServiceTracker</code>.
   *
   * <p>
   * This implementation calls getServiceReferences() to get the list
   * of references for the tracked services and then calls
   * getService(const ctkServiceReference&) for each reference to get the
   * tracked service object.
   *
   * @return A list of service objects or an empty list if no services
   *         are being tracked.
   */
  virtual QList<T> getServices() const;

  /**
   * Returns a service object for one of the services being tracked by this
   * <code>ctkServiceTracker</code>.
   *
   * <p>
   * If any services are being tracked, this implementation returns the result
   * of calling <code>getService(getServiceReference())</code>.
   *
   * @return A service object or <code>null</code> if no services are being
   *         tracked.
   */
  virtual T getService() const;

  /**
   * Remove a service from this <code>ctkServiceTracker</code>.
   *
   * The specified service will be removed from this
   * <code>ctkServiceTracker</code>. If the specified service was being tracked
   * then the <code>ctkServiceTrackerCustomizer::removedService</code> method will
   * be called for that service.
   *
   * @param reference The reference to the service to be removed.
   */
  virtual void remove(const ctkServiceReference& reference);

  /**
   * Return the number of services being tracked by this
   * <code>ctkServiceTracker</code>.
   *
   * @return The number of services being tracked.
   */
  virtual int size() const;

  /**
   * Returns the tracking count for this <code>ctkServiceTracker</code>.
   *
   * The tracking count is initialized to 0 when this
   * <code>ctkServiceTracker</code> is opened. Every time a service is added,
   * modified or removed from this <code>ctkServiceTracker</code>, the tracking
   * count is incremented.
   *
   * <p>
   * The tracking count can be used to determine if this
   * <code>ctkServiceTracker</code> has added, modified or removed a service by
   * comparing a tracking count value previously collected with the current
   * tracking count value. If the value has not changed, then no service has
   * been added, modified or removed from this <code>ctkServiceTracker</code>
   * since the previous tracking count was collected.
   *
   * @return The tracking count for this <code>ctkServiceTracker</code> or -1 if
   *         this <code>ctkServiceTracker</code> is not open.
   */
  virtual int getTrackingCount() const;

  /**
   * Return a sorted <code>QMap</code> of the <code>ctkServiceReference</code>s and
   * service objects for all services being tracked by this
   * <code>ctkServiceTracker</code>. The map is sorted in natural order
   * of <code>ctkServiceReference</code>. That is, the last entry is the service
   * with the highest ranking and the lowest service id.
   *
   * @return A <code>QMap</code> with the <code>ctkServiceReference</code>s
   *         and service objects for all services being tracked by this
   *         <code>ctkServiceTracker</code>. If no services are being tracked,
   *         then the returned map is empty.
   */
  virtual QMap<ctkServiceReference, T> getTracked() const;

  /**
   * Return if this <code>ctkServiceTracker</code> is empty.
   *
   * @return <code>true</code> if this <code>ctkServiceTracker</code> is not tracking any
   *         services.
   */
  virtual bool isEmpty() const;

protected:

  /**
   * Default implementation of the
   * <code>ctkServiceTrackerCustomizer::addingService</code> method.
   *
   * <p>
   * This method is only called when this <code>ctkServiceTracker</code> has been
   * constructed with a <code>null</code> ctkServiceTrackerCustomizer argument.
   *
   * <p>
   * This implementation returns the result of calling <code>getService</code>
   * on the <code>ctkPluginContext</code> with which this
   * <code>ctkServiceTracker</code> was created passing the specified
   * <code>ctkServiceReference</code>.
   * <p>
   * This method can be overridden in a subclass to customize the service
   * object to be tracked for the service being added. In that case, take care
   * not to rely on the default implementation of
   * \link removedService(const ctkServiceReference&, T service) removedService\endlink
   * to unget the service.
   *
   * @param reference The reference to the service being added to this
   *        <code>ctkServiceTracker</code>.
   * @return The service object to be tracked for the service added to this
   *         <code>ctlServiceTracker</code>.
   * @see ctkServiceTrackerCustomizer::addingService(const ctkServiceReference&)
   */
  T addingService(const ctkServiceReference& reference);

  /**
   * Default implementation of the
   * <code>ctkServiceTrackerCustomizer::modifiedService</code> method.
   *
   * <p>
   * This method is only called when this <code>ctkServiceTracker</code> has been
   * constructed with a <code>null</code> ctkServiceTrackerCustomizer argument.
   *
   * <p>
   * This implementation does nothing.
   *
   * @param reference The reference to modified service.
   * @param service The service object for the modified service.
   * @see ctkServiceTrackerCustomizer::modifiedService(const ctkServiceReference&, QObject*)
   */
  void modifiedService(const ctkServiceReference& reference, T service);

  /**
   * Default implementation of the
   * <code>ctkServiceTrackerCustomizer::removedService</code> method.
   *
   * <p>
   * This method is only called when this <code>ctkServiceTracker</code> has been
   * constructed with a <code>null</code> ctkServiceTrackerCustomizer argument.
   *
   * <p>
   * This implementation calls <code>ungetService</code>, on the
   * <code>ctkPluginContext</code> with which this <code>ctkServiceTracker</code>
   * was created, passing the specified <code>ctkServiceReference</code>.
   * <p>
   * This method can be overridden in a subclass. If the default
   * implementation of \link addingService(const ctkServiceReference&) addingService\endlink
   * method was used, this method must unget the service.
   *
   * @param reference The reference to removed service.
   * @param service The service object for the removed service.
   * @see ctkServiceTrackerCustomizer::removedService(const ServiceReference&, QObject*)
   */
  void removedService(const ctkServiceReference& reference, T service);

private:

  typedef ctkServiceTracker<S,T> ServiceTracker;
  typedef ctkTrackedService<S,T> TrackedService;
  typedef ctkServiceTrackerPrivate<S,T> ServiceTrackerPrivate;
  typedef ctkServiceTrackerCustomizer<T> ServiceTrackerCustomizer;

  friend class ctkTrackedService<S,T>;
  friend class ctkServiceTrackerPrivate<S,T>;

  inline ServiceTrackerPrivate* d_func()
  {
    return reinterpret_cast<ServiceTrackerPrivate*>(qGetPtrHelper(d_ptr));
  }

  inline const ServiceTrackerPrivate* d_func() const
  {
    return reinterpret_cast<const ServiceTrackerPrivate*>(qGetPtrHelper(d_ptr));
  }

  const QScopedPointer<ServiceTrackerPrivate> d_ptr;
};

#include "ctkServiceTracker.tpp"

#endif // CTKSERVICETRACKER_H
