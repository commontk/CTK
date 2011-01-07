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

template<class SyncDeliverTasks, class HandlerTask>
class ctkEAAsyncDeliverTasks<SyncDeliverTasks, HandlerTask>::TaskExecuter
    : public ctkEARunnable
{

private:

  typedef ctkEAAsyncDeliverTasks<SyncDeliverTasks, HandlerTask> TopClass;

  TopClass* tc;

  QList<HandlerTask> tasks;
  QMutex tasksMutex;
  QThread* key;

public:

  TaskExecuter(TopClass* tc, const QList<HandlerTask>& tasks, QThread* key)
    : tc(tc), tasks(tasks), key(key)
  {
  }

  void run()
  {
    bool running = true;
    do
    {
      QList<HandlerTask> currTasks;

      {
        QMutexLocker l(&tasksMutex);
        currTasks.push_back(tasks.takeFirst());
      }
      tc->deliver_task->execute(currTasks);
      {
        QMutexLocker l(&tc->running_threads_mutex);
        running = tasks.size() > 0;
        if (!running)
        {
          ctkEARunnable* runnable = tc->running_threads.take(key);
          if (runnable->autoDelete() && !--runnable->ref) delete runnable;
        }
      }
    } while (running);
  }

  void add(const QList<HandlerTask>& newTasks)
  {
    QMutexLocker l(&tasksMutex);
    tasks.append(newTasks);
  }
};

template<class SyncDeliverTasks, class HandlerTask>
ctkEAAsyncDeliverTasks<SyncDeliverTasks, HandlerTask>::ctkEAAsyncDeliverTasks(ctkEADefaultThreadPool* pool, DeliverTask* deliverTask)
 : pool(pool), deliver_task(deliverTask)
{
}

template<class SyncDeliverTasks, class HandlerTask>
void ctkEAAsyncDeliverTasks<SyncDeliverTasks, HandlerTask>::execute(const QList<HandlerTask>& tasks)
{
  QThread* currentThread = QThread::currentThread();
  TaskExecuter* executer = 0;
  {
    QMutexLocker l(&running_threads_mutex);
    TaskExecuter* runningExecutor = dynamic_cast<TaskExecuter*>(running_threads.value(currentThread));
    if (runningExecutor)
    {
      runningExecutor->add(tasks);
    }
    else
    {
      executer = new TaskExecuter(this, tasks, currentThread);
      ++executer->ref;
      running_threads.insert(currentThread, executer);
    }
  }
  if (executer)
  {
    pool->executeTask(executer);
  }
}

