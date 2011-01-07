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


#ifndef CTKEAHANDLERTASKS_P_H
#define CTKEAHANDLERTASKS_P_H

#include <QList>

#include <service/event/ctkEvent.h>

template<class HandlerTasks> class ctkEAHandlerTask;

/**
 * The factory for event handler tasks. Implementations of this interface can be
 * used to create tasks that handle the delivery of events to event handlers.
 */
template<class Impl>
struct ctkEAHandlerTasks
{
  /**
   * Create the handler tasks for the event. All matching event handlers must
   * be determined and delivery tasks for them returned.
   *
   * @param event The event for which' handlers delivery tasks must be created
   *
   * @return A delivery task for each handler that matches the given event
   */
  QList<ctkEAHandlerTask<Impl> > createHandlerTasks(const ctkEvent& event)
  {
    return static_cast<Impl*>(this)->createHandlerTasks(event);
  }

  virtual ~ctkEAHandlerTasks() {}

};

#endif // CTKEAHANDLERTASKS_P_H
