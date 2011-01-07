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


#ifndef CTKEADELIVERTASK_P_H
#define CTKEADELIVERTASK_P_H

#include <QList>

#include "ctkEAHandlerTask_p.h"

/**
 * Dispatch given event dispatch tasks.
 */
template<class Impl, class HandlerTask>
struct ctkEADeliverTask
{
  virtual ~ctkEADeliverTask() {}

  /**
   * Dispatch the given event dispatch tasks.
   *
   * @param handlerTasks The event dispatch tasks to execute
   */
  void execute(const QList<HandlerTask>& handlerTasks)
  {
    static_cast<Impl*>(this)->execute(handlerTasks);
  }
};

#endif // CTKEADELIVERTASK_P_H
