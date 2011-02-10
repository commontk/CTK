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


#ifndef CTKTRACKEDSERVICE_P_H
#define CTKTRACKEDSERVICE_P_H

#include "ctkTrackedServiceListener_p.h"
#include "ctkPluginAbstractTracked_p.h"
#include "ctkServiceEvent.h"

/**
 * \ingroup PluginFramework
 */
template<class S, class T>
class ctkTrackedService : public ctkTrackedServiceListener,
    public ctkPluginAbstractTracked<ctkServiceReference, T, ctkServiceEvent>
{

public:
  ctkTrackedService(ctkServiceTracker<S,T>* serviceTracker,
                    ctkServiceTrackerCustomizer<T>* customizer);

  /**
   * Slot connected to service events for the
   * <code>ctkServiceTracker</code> class. This method must NOT be
   * synchronized to avoid deadlock potential.
   *
   * @param event <code>ctkServiceEvent</code> object from the framework.
   */
  void serviceChanged(const ctkServiceEvent& event);

private:

  typedef ctkPluginAbstractTracked<ctkServiceReference, T, ctkServiceEvent> Superclass;

  ctkServiceTracker<S,T>* serviceTracker;
  ctkServiceTrackerCustomizer<T>* customizer;

  /**
   * Increment the tracking count and tell the tracker there was a
   * modification.
   *
   * @GuardedBy this
   */
  void modified();

  /**
   * Call the specific customizer adding method. This method must not be
   * called while synchronized on this object.
   *
   * @param item Item to be tracked.
   * @param related Action related object.
   * @return Customized object for the tracked item or <code>null</code>
   *         if the item is not to be tracked.
   */
  T customizerAdding(ctkServiceReference item, const ctkServiceEvent& related);

  /**
   * Call the specific customizer modified method. This method must not be
   * called while synchronized on this object.
   *
   * @param item Tracked item.
   * @param related Action related object.
   * @param object Customized object for the tracked item.
   */
  void customizerModified(ctkServiceReference item,
                          const ctkServiceEvent& related, T object) ;

  /**
   * Call the specific customizer removed method. This method must not be
   * called while synchronized on this object.
   *
   * @param item Tracked item.
   * @param related Action related object.
   * @param object Customized object for the tracked item.
   */
  void customizerRemoved(ctkServiceReference item,
                         const ctkServiceEvent& related, T object) ;
};

#include "ctkTrackedService.tpp"

#endif // CTKTRACKEDSERVICE_P_H
