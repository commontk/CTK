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


#ifndef CTKSERVICETRACKERPRIVATE_H
#define CTKSERVICETRACKERPRIVATE_H

#include "ctkServiceReference.h"
#include "ctkLDAPSearchFilter.h"

#include <QMutex>
#include <QSharedPointer>

class ctkServiceTracker;
class ctkTrackedService;
class ctkServiceTrackerCustomizer;

class ctkServiceTrackerPrivate {

public:

  ctkServiceTrackerPrivate(ctkServiceTracker* st,
                           ctkPluginContext* context,
                           const ctkServiceReference& reference,
                           ctkServiceTrackerCustomizer* customizer);

  ctkServiceTrackerPrivate(ctkServiceTracker* st,
      ctkPluginContext* context, const QString& clazz,
      ctkServiceTrackerCustomizer* customizer);

  ctkServiceTrackerPrivate(
      ctkServiceTracker* st,
      ctkPluginContext* context, const ctkLDAPSearchFilter& filter,
      ctkServiceTrackerCustomizer* customizer);

  ~ctkServiceTrackerPrivate();

  /**
   * Returns the list of initial <code>ctkServiceReference</code>s that will be
   * tracked by this <code>ctkServiceTracker</code>.
   *
   * @param className The class name with which the service was registered, or
   *        <code>null</code> for all services.
   * @param filterString The filter criteria or <code>null</code> for all
   *        services.
   * @return The list of initial <code>ctkServiceReference</code>s.
   * @throws std::invalid_argument If the specified filterString has an
   *         invalid syntax.
   */
  QList<ctkServiceReference> getInitialReferences(const QString& className,
                                                  const QString& filterString);

  /* set this to true to compile in debug messages */
  static const bool	DEBUG; //	= false;

  /**
   * The Plugin Context used by this <code>ctkServiceTracker</code>.
   */
  ctkPluginContext* const	context;

  /**
   * The filter used by this <code>ctkServiceTracker</code> which specifies the
   * search criteria for the services to track.
   */
  ctkLDAPSearchFilter	filter;

  /**
   * The <code>ctkServiceTrackerCustomizer</code> for this tracker.
   */
  ctkServiceTrackerCustomizer* customizer;

  /**
   * Filter string for use when adding the ServiceListener. If this field is
   * set, then certain optimizations can be taken since we don't have a user
   * supplied filter.
   */
  QString listenerFilter;

  /**
   * Class name to be tracked. If this field is set, then we are tracking by
   * class name.
   */
  QString trackClass;

  /**
   * Reference to be tracked. If this field is set, then we are tracking a
   * single ctkServiceReference.
   */
  ctkServiceReference	trackReference;

  /**
   * Tracked services: <code>ctkServiceReference</code> -> customized Object and
   * <code>ctkServiceSlotEntry</code> object
   */
  QSharedPointer<ctkTrackedService> trackedService;

  /**
   * Accessor method for the current ctkTrackedService object. This method is only
   * intended to be used by the unsynchronized methods which do not modify the
   * trackedService field.
   *
   * @return The current Tracked object.
   */
  QSharedPointer<ctkTrackedService> tracked() const;

  /**
   * Called by the ctkTrackedService object whenever the set of tracked services is
   * modified. Clears the cache.
   */
  /*
   * This method must not be synchronized since it is called by ctkTrackedService while
   * ctkTrackedService is synchronized. We don't want synchronization interactions
   * between the listener thread and the user thread.
   */
  void modified();

  /**
   * Cached ctkServiceReference for getServiceReference.
   */
  mutable ctkServiceReference cachedReference;

  /**
   * Cached service object for getService.
   *
   * This field is volatile since it is accessed by multiple threads.
   */
  mutable QObject* volatile cachedService;

  mutable QMutex mutex;

private:

  Q_DECLARE_PUBLIC(ctkServiceTracker)

  ctkServiceTracker * const q_ptr;

};

#endif // CTKSERVICETRACKERPRIVATE_H
