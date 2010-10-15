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


#ifndef CTKSERVICETRACKER_H
#define CTKSERVICETRACKER_H

#include <QScopedPointer>

#include "CTKPluginFrameworkExport.h"

#include "ctkServiceReference.h"
#include "ctkServiceTrackerCustomizer.h"
#include "ctkLDAPSearchFilter.h"

class ctkPluginContext;
class ctkServiceTrackerPrivate;

class CTK_PLUGINFW_EXPORT ctkServiceTracker : protected ctkServiceTrackerCustomizer
{
public:

  ~ctkServiceTracker();

  /**
   * Create a <code>ServiceTracker</code> on the specified
   * <code>ServiceReference</code>.
   *
   * <p>
   * The service referenced by the specified <code>ServiceReference</code>
   * will be tracked by this <code>ServiceTracker</code>.
   *
   * @param context The <code>BundleContext</code> against which the tracking
   *        is done.
   * @param reference The <code>ServiceReference</code> for the service to be
   *        tracked.
   * @param customizer The customizer object to call when services are added,
   *        modified, or removed in this <code>ServiceTracker</code>. If
   *        customizer is <code>null</code>, then this
   *        <code>ServiceTracker</code> will be used as the
   *        <code>ServiceTrackerCustomizer</code> and this
   *        <code>ServiceTracker</code> will call the
   *        <code>ServiceTrackerCustomizer</code> methods on itself.
   */
  ctkServiceTracker(ctkPluginContext* context,
                    const ctkServiceReference& reference,
                    ctkServiceTrackerCustomizer* customizer = 0);

  /**
   * Create a <code>ServiceTracker</code> on the specified class name.
   *
   * <p>
   * Services registered under the specified class name will be tracked by
   * this <code>ServiceTracker</code>.
   *
   * @param context The <code>BundleContext</code> against which the tracking
   *        is done.
   * @param clazz The class name of the services to be tracked.
   * @param customizer The customizer object to call when services are added,
   *        modified, or removed in this <code>ServiceTracker</code>. If
   *        customizer is <code>null</code>, then this
   *        <code>ServiceTracker</code> will be used as the
   *        <code>ServiceTrackerCustomizer</code> and this
   *        <code>ServiceTracker</code> will call the
   *        <code>ServiceTrackerCustomizer</code> methods on itself.
   */
  ctkServiceTracker(ctkPluginContext* context, const QString& clazz,
                        ctkServiceTrackerCustomizer* customizer = 0);

  /**
   * Create a <code>ServiceTracker</code> on the specified <code>Filter</code>
   * object.
   *
   * <p>
   * Services which match the specified <code>Filter</code> object will be
   * tracked by this <code>ServiceTracker</code>.
   *
   * @param context The <code>BundleContext</code> against which the tracking
   *        is done.
   * @param filter The <code>Filter</code> to select the services to be
   *        tracked.
   * @param customizer The customizer object to call when services are added,
   *        modified, or removed in this <code>ServiceTracker</code>. If
   *        customizer is null, then this <code>ServiceTracker</code> will be
   *        used as the <code>ServiceTrackerCustomizer</code> and this
   *        <code>ServiceTracker</code> will call the
   *        <code>ServiceTrackerCustomizer</code> methods on itself.
   * @since 1.1
   */
  ctkServiceTracker(ctkPluginContext* context, const ctkLDAPSearchFilter& filter,
                        ctkServiceTrackerCustomizer* customizer = 0);

  /**
   * Open this <code>ServiceTracker</code> and begin tracking services.
   *
   * <p>
   * Services which match the search criteria specified when this
   * <code>ServiceTracker</code> was created are now tracked by this
   * <code>ServiceTracker</code>.
   *
   * @throws java.lang.IllegalStateException If the <code>BundleContext</code>
   *         with which this <code>ServiceTracker</code> was created is no
   *         longer valid.
   * @since 1.3
   */
  void open();

  /**
   * Close this <code>ServiceTracker</code>.
   *
   * <p>
   * This method should be called when this <code>ServiceTracker</code> should
   * end the tracking of services.
   *
   * <p>
   * This implementation calls {@link #getServiceReferences()} to get the list
   * of tracked services to remove.
   */
  void close();

  /**
   * Wait for at least one service to be tracked by this
   * <code>ServiceTracker</code>. This method will also return when this
   * <code>ServiceTracker</code> is closed.
   *
   * <p>
   * It is strongly recommended that <code>waitForService</code> is not used
   * during the calling of the <code>BundleActivator</code> methods.
   * <code>BundleActivator</code> methods are expected to complete in a short
   * period of time.
   *
   * <p>
   * This implementation calls {@link #getService()} to determine if a service
   * is being tracked.
   *
   * @param timeout The time interval in milliseconds to wait. If zero, the
   *        method will wait indefinitely.
   * @return Returns the result of {@link #getService()}.
   * @throws InterruptedException If another thread has interrupted the
   *         current thread.
   * @throws IllegalArgumentException If the value of timeout is negative.
   */
  QObject* waitForService(unsigned long timeout);

  /**
   * Return an array of <code>ServiceReference</code>s for all services being
   * tracked by this <code>ServiceTracker</code>.
   *
   * @return Array of <code>ServiceReference</code>s or <code>null</code> if
   *         no services are being tracked.
   */
  QList<ctkServiceReference> getServiceReferences() const;

  /**
   * Returns a <code>ServiceReference</code> for one of the services being
   * tracked by this <code>ServiceTracker</code>.
   *
   * <p>
   * If multiple services are being tracked, the service with the highest
   * ranking (as specified in its <code>service.ranking</code> property) is
   * returned. If there is a tie in ranking, the service with the lowest
   * service ID (as specified in its <code>service.id</code> property); that
   * is, the service that was registered first is returned. This is the same
   * algorithm used by <code>BundleContext.getServiceReference</code>.
   *
   * <p>
   * This implementation calls {@link #getServiceReferences()} to get the list
   * of references for the tracked services.
   *
   * @return A <code>ServiceReference</code> or <code>null</code> if no
   *         services are being tracked.
   * @since 1.1
   */
  ctkServiceReference getServiceReference() const;

  /**
   * Returns the service object for the specified
   * <code>ServiceReference</code> if the specified referenced service is
   * being tracked by this <code>ServiceTracker</code>.
   *
   * @param reference The reference to the desired service.
   * @return A service object or <code>null</code> if the service referenced
   *         by the specified <code>ServiceReference</code> is not being
   *         tracked.
   */
  QObject* getService(const ctkServiceReference& reference) const;

  /**
   * Return an array of service objects for all services being tracked by this
   * <code>ServiceTracker</code>.
   *
   * <p>
   * This implementation calls {@link #getServiceReferences()} to get the list
   * of references for the tracked services and then calls
   * {@link #getService(ServiceReference)} for each reference to get the
   * tracked service object.
   *
   * @return An array of service objects or <code>null</code> if no services
   *         are being tracked.
   */
  QList<QObject*> getServices() const;

  /**
   * Returns a service object for one of the services being tracked by this
   * <code>ServiceTracker</code>.
   *
   * <p>
   * If any services are being tracked, this implementation returns the result
   * of calling <code>getService(getServiceReference())</code>.
   *
   * @return A service object or <code>null</code> if no services are being
   *         tracked.
   */
  QObject* getService() const;

  /**
   * Remove a service from this <code>ServiceTracker</code>.
   *
   * The specified service will be removed from this
   * <code>ServiceTracker</code>. If the specified service was being tracked
   * then the <code>ServiceTrackerCustomizer.removedService</code> method will
   * be called for that service.
   *
   * @param reference The reference to the service to be removed.
   */
  void remove(const ctkServiceReference& reference);

  /**
   * Return the number of services being tracked by this
   * <code>ServiceTracker</code>.
   *
   * @return The number of services being tracked.
   */
  int size() const;

  /**
   * Returns the tracking count for this <code>ServiceTracker</code>.
   *
   * The tracking count is initialized to 0 when this
   * <code>ServiceTracker</code> is opened. Every time a service is added,
   * modified or removed from this <code>ServiceTracker</code>, the tracking
   * count is incremented.
   *
   * <p>
   * The tracking count can be used to determine if this
   * <code>ServiceTracker</code> has added, modified or removed a service by
   * comparing a tracking count value previously collected with the current
   * tracking count value. If the value has not changed, then no service has
   * been added, modified or removed from this <code>ServiceTracker</code>
   * since the previous tracking count was collected.
   *
   * @since 1.2
   * @return The tracking count for this <code>ServiceTracker</code> or -1 if
   *         this <code>ServiceTracker</code> is not open.
   */
  int getTrackingCount() const;

protected:

  /**
   * Default implementation of the
   * <code>ServiceTrackerCustomizer.addingService</code> method.
   *
   * <p>
   * This method is only called when this <code>ServiceTracker</code> has been
   * constructed with a <code>null ServiceTrackerCustomizer</code> argument.
   *
   * <p>
   * This implementation returns the result of calling <code>getService</code>
   * on the <code>BundleContext</code> with which this
   * <code>ServiceTracker</code> was created passing the specified
   * <code>ServiceReference</code>.
   * <p>
   * This method can be overridden in a subclass to customize the service
   * object to be tracked for the service being added. In that case, take care
   * not to rely on the default implementation of
   * {@link #removedService(ServiceReference, Object) removedService} to unget
   * the service.
   *
   * @param reference The reference to the service being added to this
   *        <code>ServiceTracker</code>.
   * @return The service object to be tracked for the service added to this
   *         <code>ServiceTracker</code>.
   * @see ServiceTrackerCustomizer#addingService(ServiceReference)
   */
  QObject* addingService(const ctkServiceReference& reference);

  /**
   * Default implementation of the
   * <code>ServiceTrackerCustomizer.modifiedService</code> method.
   *
   * <p>
   * This method is only called when this <code>ServiceTracker</code> has been
   * constructed with a <code>null ServiceTrackerCustomizer</code> argument.
   *
   * <p>
   * This implementation does nothing.
   *
   * @param reference The reference to modified service.
   * @param service The service object for the modified service.
   * @see ServiceTrackerCustomizer#modifiedService(ServiceReference, Object)
   */
  void modifiedService(const ctkServiceReference& reference, QObject* service);

  /**
   * Default implementation of the
   * <code>ServiceTrackerCustomizer.removedService</code> method.
   *
   * <p>
   * This method is only called when this <code>ServiceTracker</code> has been
   * constructed with a <code>null ServiceTrackerCustomizer</code> argument.
   *
   * <p>
   * This implementation calls <code>ungetService</code>, on the
   * <code>BundleContext</code> with which this <code>ServiceTracker</code>
   * was created, passing the specified <code>ServiceReference</code>.
   * <p>
   * This method can be overridden in a subclass. If the default
   * implementation of {@link #addingService(ServiceReference) addingService}
   * method was used, this method must unget the service.
   *
   * @param reference The reference to removed service.
   * @param service The service object for the removed service.
   * @see ServiceTrackerCustomizer#removedService(ServiceReference, Object)
   */
  void removedService(const ctkServiceReference& reference, QObject* service);

private:

  friend class ctkTrackedService;

  Q_DECLARE_PRIVATE(ctkServiceTracker)

  const QScopedPointer<ctkServiceTrackerPrivate> d_ptr;
};

#endif // CTKSERVICETRACKER_H
