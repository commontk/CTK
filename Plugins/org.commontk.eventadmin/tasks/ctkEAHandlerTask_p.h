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


#ifndef CTKEAHANDLERTASK_P_H
#define CTKEAHANDLERTASK_P_H

#include <QAtomicInt>

#include <ctkServiceReference.h>
#include <service/event/ctkEvent.h>

/**
 * A task that will deliver its event to its <tt>ctkEventHandler</tt> when executed
 * or blacklist the handler, respectively.
 */
template<class BlacklistingHandlerTasks>
class ctkEAHandlerTask
{

private:

  typedef ctkEAHandlerTask<BlacklistingHandlerTasks> Self;

  // The service reference of the handler
  ctkServiceReference eventHandlerRef;

  // The event to deliver to the handler
  ctkEvent event;

  // Used to blacklist the service or get the service object for the reference
  BlacklistingHandlerTasks* handlerTasks;

  class _GetAndUngetEventHandler;

public:

  /**
   * Construct a delivery task for the given service and event.
   *
   * @param eventHandlerRef The servicereference of the handler
   * @param event The event to deliver
   * @param handlerTasks Used to blacklist the service or get the service object
   *      for the reference
   */
  ctkEAHandlerTask(const ctkServiceReference& eventHandlerRef,
                   const ctkEvent& event, BlacklistingHandlerTasks* handlerTasks);

  ctkEAHandlerTask(const Self& task);

  ctkEAHandlerTask& operator=(const Self& task);

  /**
   * Return the class name of the handler
   */
  QString getHandlerClassName() const;

  /**
   * Deliver the event to the handler.
   */
  void execute();

  /**
   * Blacklist the handler.
   */
  void blackListHandler();

};

#include "ctkEAHandlerTask.tpp"

#endif // CTKEAHANDLERTASK_P_H
