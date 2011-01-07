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


#include "ctkEASyncThread_p.h"

#include <dispatch/ctkEADefaultThreadPool_p.h>
#include <dispatch/ctkEASyncMasterThread_p.h>
#include <util/ctkEARendezvous_p.h>
#include <util/ctkEATimeoutException_p.h>

#include <QDateTime>

template<class HandlerTask>
class _TimeoutRunnable : public ctkEARunnable
{
public:

  ctkEARendezvous timerBarrier;
  ctkEARendezvous startBarrier;

  _TimeoutRunnable(ctkEARendezvous* cascadingBarrier, HandlerTask* task)
    : cascadingBarrier(cascadingBarrier), task(task)
  {

  }

  void run()
  {
    ctkEASyncThread* myThread = qobject_cast<ctkEASyncThread*>(QThread::currentThread());
    Q_ASSERT(myThread != 0);
    myThread->init(&timerBarrier, cascadingBarrier);
    try
    {
      // notify the outer thread to start the timer
      startBarrier.waitForRendezvous();
      // execute the task
      task->execute();
      // stop the timer
      timerBarrier.waitForRendezvous();
    }
    catch (const std::logic_error& )
    {
      // this can happen on shutdown, so we ignore it
    }
    catch (...)
    {
      myThread->uninit();
      throw;
    }
    myThread->uninit();
  }

private:

  ctkEARendezvous* cascadingBarrier;
  HandlerTask* task;
};

template<class HandlerTask>
class _RunInSyncMaster : public QRunnable
{
public:

  _RunInSyncMaster(ctkEASyncDeliverTasks<HandlerTask>* handlerTasks,
                   const QList<HandlerTask>& tasks)
    : handlerTasks(handlerTasks), tasks(tasks)
  {

  }

  void run()
  {
    handlerTasks->executeInSyncMaster(tasks);
  }

private:

  ctkEASyncDeliverTasks<HandlerTask>* handlerTasks;
  const QList<HandlerTask>& tasks;
};

template<class HandlerTask>
ctkEASyncDeliverTasks<HandlerTask>::ctkEASyncDeliverTasks(
  ctkEADefaultThreadPool* pool, ctkEASyncMasterThread* syncMasterThread,
  long timeout, const QList<QString>& ignoreTimeout)
  : pool(pool), syncMasterThread(syncMasterThread)
{
  update(timeout, ignoreTimeout);
}

template<class HandlerTask>
void ctkEASyncDeliverTasks<HandlerTask>::update(long timeout, const QList<QString>& ignoreTimeout)
{
  {
    QMutexLocker l(&mutex);
    this->timeout = timeout;
  }

  if (ignoreTimeout.isEmpty())
  {
    QMutexLocker l(&mutex);
    qDeleteAll(ignoreTimeoutMatcher);
    ignoreTimeoutMatcher.clear();
  }
  else
  {
    QList<Matcher*> newMatcherList;
    foreach(QString value, ignoreTimeout)
    {
      value = value.trimmed();
      if (!value.isEmpty())
      {
        newMatcherList.push_back(new ClassMatcher(value));
      }
    }

    {
      QMutexLocker l(&mutex);
      qDeleteAll(ignoreTimeoutMatcher);
      ignoreTimeoutMatcher = newMatcherList;
    }
  }
}

template<class HandlerTask>
void ctkEASyncDeliverTasks<HandlerTask>::execute(const QList<HandlerTask>& tasks)
{
  _RunInSyncMaster<HandlerTask> runnable(this, tasks);
  runnable.setAutoDelete(false);
  syncMasterThread->syncRun(&runnable);
}

template<class HandlerTask>
void ctkEASyncDeliverTasks<HandlerTask>::executeInSyncMaster(const QList<HandlerTask>& tasks)
{
  QThread* sleepingThread = QThread::currentThread();
  ctkEASyncThread* syncThread = qobject_cast<ctkEASyncThread*>(sleepingThread);
  ctkEARendezvous cascadingBarrier;
  // check if this is a cascaded event sending
  if (syncThread)
  {
    // wake up outer thread
    if (syncThread->isTopMostHandler())
    {
      syncThread->getTimerBarrier()->waitForRendezvous();
    }
    syncThread->innerEventHandlingStart();
  }

  foreach(HandlerTask task, tasks)
  {
    if (!useTimeout(task))
    {
      // no timeout, we can directly execute
      task.execute();
    }
    else
    {
      _TimeoutRunnable<HandlerTask>* timeoutRunnable
          = new _TimeoutRunnable<HandlerTask>(&cascadingBarrier, &task);
      ++timeoutRunnable->ref;

      ctkEARendezvous* startBarrier = &timeoutRunnable->startBarrier;
      ctkEARendezvous* timerBarrier = &timeoutRunnable->timerBarrier;
      pool->executeTask(timeoutRunnable);

      // we wait for the inner thread to start
      startBarrier->waitForRendezvous();

      // timeout handling
      bool finished = true;
      long sleepTime = timeout;
      do {
        finished = true;
        // we sleep for the sleep time
        // if someone wakes us up it's the inner task who either
        // has finished or a cascading event
        //TODO use Qt4.7 API
        //long startTime = System.currentTimeMillis();
        QDateTime startTime = QDateTime::currentDateTime();
        try
        {
          timerBarrier->waitAttemptForRendezvous(sleepTime);
          // if this occurs no timeout occured or we have a cascaded event
          if (!task.finished())
          {
            // adjust remaining sleep time
            //TODO use Qt4.7 API
            //sleepTime = timeout - (System.currentTimeMillis() - startTime);
            sleepTime = timeout - startTime.time().msecsTo(QDateTime::currentDateTime().time());
            cascadingBarrier.waitForRendezvous();
            finished = task.finished();
          }
        }
        catch (const ctkEATimeoutException& )
        {
          // if we timed out, we have to blacklist the handler
          task.blackListHandler();
        }
      }
      while ( !finished );

      if (!--timeoutRunnable->ref) delete timeoutRunnable;
    }
  }

  // wake up outer thread again if cascaded
  if (syncThread)
  {
    syncThread->innerEventHandlingStopped();
    if (syncThread->isTopMostHandler())
    {
      if (!syncThread->getTimerBarrier()->isTimedOut())
      {
        syncThread->getCascadingBarrier()->waitForRendezvous();
      }
    }
  }

}

template<class HandlerTask>
bool ctkEASyncDeliverTasks<HandlerTask>::useTimeout(const HandlerTask& task)
{
  // we only check the classname if a timeout is configured
  long t = 0;
  {
    QMutexLocker l(&mutex);
    t = timeout;
  }

  if (t > 0)
  {
    QList<Matcher*> currMatcherList;
    {
      QMutexLocker l(&mutex);
      currMatcherList = ignoreTimeoutMatcher;
    }
    if (!currMatcherList.isEmpty())
    {
      QString className = task.getHandlerClassName();
      foreach(Matcher* matcher, currMatcherList)
      {
        if (matcher)
        {
          if (matcher->match(className))
          {
            return false;
          }
        }
      }
    }
    return true;
  }
  return false;
}
