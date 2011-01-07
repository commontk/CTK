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


#ifndef CTKEAASYNCDELIVERTASKS_P_H
#define CTKEAASYNCDELIVERTASKS_P_H

#include "ctkEADeliverTask_p.h"
#include <dispatch/ctkEADefaultThreadPool_p.h>

class ctkEARunnable;

/**
 * This class does the actual work of the asynchronous event dispatch.
 */
template<class SyncDeliverTasks, class HandlerTask>
class ctkEAAsyncDeliverTasks : public ctkEADeliverTask<ctkEAAsyncDeliverTasks<SyncDeliverTasks,HandlerTask>, HandlerTask>
{

private:

  /** The thread pool to use to spin-off new threads. */
  ctkEADefaultThreadPool* pool;

  /**
   * The deliver task for actually delivering the events. This
   * is the sync deliver tasks as this has all the code for timeout
   * handling etc.
   */
  typedef ctkEADeliverTask<SyncDeliverTasks, HandlerTask> DeliverTask;
  DeliverTask* deliver_task;

  /** A map of running threads currently delivering async events. */
  QHash<QThread*, ctkEARunnable*> running_threads;
  QMutex running_threads_mutex;

public:

  /**
   * The constructor of the class that will use the asynchronous.
   *
   * @param pool The thread pool used to spin-off new asynchronous event
   *        dispatching threads in case of timeout or that the asynchronous event
   *        dispatching thread is used to send a synchronous event
   * @param deliverTask The deliver tasks for dispatching the event.
   */
  ctkEAAsyncDeliverTasks(ctkEADefaultThreadPool* pool, DeliverTask* deliverTask);

  /**
   * This does not block an unrelated thread used to send a synchronous event.
   *
   * @param tasks The event handler dispatch tasks to execute
   *
   * @see ctkEADeliverTask#execute(const QList<HandlerTask>&)
   */
  void execute(const QList<HandlerTask>& tasks);

private:

  class TaskExecuter;
};

#include "ctkEAAsyncDeliverTasks.tpp"

#endif // CTKEAASYNCDELIVERTASKS_P_H
