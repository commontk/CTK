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


#include "ctkEAPooledExecutor_p.h"

// for ctk::msecsTo() - remove after switching to Qt 4.7
#include <ctkUtils.h>

#include "ctkEAChannel_p.h"
#include <dispatch/ctkEAInterruptibleThread_p.h>
#include <dispatch/ctkEAInterruptedException_p.h>

#include <limits>
#include <stdexcept>

#include <QDateTime>
#include <QDebug>

const int ctkEAPooledExecutor::DEFAULT_MAXIMUMPOOLSIZE = std::numeric_limits<int>::max();
const int ctkEAPooledExecutor::DEFAULT_MINIMUMPOOLSIZE = 1;
const long ctkEAPooledExecutor::DEFAULT_KEEPALIVETIME = 60 * 1000;


ctkEAPooledExecutor::ctkEAPooledExecutor(ctkEAChannel* channel, int maxPoolSize)
  : maximumPoolSize_(maxPoolSize), minimumPoolSize_(DEFAULT_MINIMUMPOOLSIZE),
    poolSize_(0), keepAliveTime_(DEFAULT_KEEPALIVETIME), shutdown_(false),
    handOff_(channel), blockedExecutionHandler_(0), waitWhenBlocked_(this),
    discardOldestWhenBlocked_(this)
{
  runWhenBlocked();
}

ctkEAPooledExecutor::~ctkEAPooledExecutor()
{
  delete handOff_;
  qDeleteAll(stoppedThreads_);
}

int ctkEAPooledExecutor::getMaximumPoolSize() const
{
  QMutexLocker lock(&mutex);
  return maximumPoolSize_;
}

void ctkEAPooledExecutor::setMaximumPoolSize(int newMaximum)
{
  QMutexLocker lock(&mutex);
  if (newMaximum <= 0) throw ctkInvalidArgumentException("maximum must be > 0");
  maximumPoolSize_ = newMaximum;
}

int ctkEAPooledExecutor::getMinimumPoolSize() const
{
  QMutexLocker lock(&mutex);
  return minimumPoolSize_;
}

void ctkEAPooledExecutor::setMinimumPoolSize(int newMinimum)
{
  QMutexLocker lock(&mutex);
  if (newMinimum < 0) throw ctkInvalidArgumentException("minimum must be >= 0");
  minimumPoolSize_ = newMinimum;
}

int ctkEAPooledExecutor::getPoolSize() const
{
  QMutexLocker lock(&mutex);
  return poolSize_;
}

long ctkEAPooledExecutor::getKeepAliveTime() const
{
  QMutexLocker lock(&mutex);
  return keepAliveTime_;
}

void ctkEAPooledExecutor::setKeepAliveTime(long msecs)
{
  QMutexLocker lock(&mutex);
  keepAliveTime_ = msecs;
}

ctkEAPooledExecutor::BlockedExecutionHandler* ctkEAPooledExecutor::getBlockedExecutionHandler() const
{
  QMutexLocker lock(&mutex);
  return blockedExecutionHandler_;
}

void ctkEAPooledExecutor::setBlockedExecutionHandler(BlockedExecutionHandler* h)
{
  QMutexLocker lock(&mutex);
  blockedExecutionHandler_ = h;
}

int ctkEAPooledExecutor::createThreads(int numberOfThreads)
{
  int ncreated = 0;
  for (int i = 0; i < numberOfThreads; ++i)
  {
    if (poolSize_ < maximumPoolSize_)
    {
      addThread(0);
      ++ncreated;
    }
    else
      break;
  }
  return ncreated;
}

void ctkEAPooledExecutor::interruptAll()
{
  QMutexLocker lock(&mutex);
  foreach (ctkEAInterruptibleThread* t, threads_)
  {
    t->interrupt();
  }
}

void ctkEAPooledExecutor::shutdownNow()
{
  shutdownNow(&discardWhenBlocked_);
}

void ctkEAPooledExecutor::shutdownNow(BlockedExecutionHandler* handler)
{
  QMutexLocker lock(&mutex);
  setBlockedExecutionHandler(handler);
  shutdown_ = true; // don't allow new tasks
  minimumPoolSize_ = maximumPoolSize_ = 0; // don't make new threads
  interruptAll(); // interrupt all existing threads
}

void ctkEAPooledExecutor::shutdownAfterProcessingCurrentlyQueuedTasks()
{
  shutdownAfterProcessingCurrentlyQueuedTasks(new DiscardWhenBlocked());
}

void ctkEAPooledExecutor::shutdownAfterProcessingCurrentlyQueuedTasks(BlockedExecutionHandler* handler)
{
  QMutexLocker lock(&mutex);
  setBlockedExecutionHandler(handler);
  shutdown_ = true;
  if (poolSize_ == 0) // disable new thread construction when idle
    minimumPoolSize_ = maximumPoolSize_ = 0;
}

bool ctkEAPooledExecutor::isTerminatedAfterShutdown() const
{
  QMutexLocker lock(&mutex);
  return shutdown_ && poolSize_ == 0;
}

bool ctkEAPooledExecutor::awaitTerminationAfterShutdown(long maxWaitTime) const
{
  QMutexLocker lock(&mutex);
  QMutexLocker shutdownLock(&shutdownMutex);
  if (!shutdown_)
    throw ctkIllegalStateException("not in shutdown state");
  if (poolSize_ == 0)
    return true;
  qint64 waitTime = static_cast<qint64>(maxWaitTime);
  if (waitTime <= 0)
    return false;
  //TODO Use Qt4.7 API
  QDateTime start = QDateTime::currentDateTime();
  forever
  {
    waitCond.wait(&shutdownMutex, waitTime);
    if (poolSize_ == 0)
      return true;
    qint64 currWait = ctk::msecsTo(start, QDateTime::currentDateTime());
    waitTime = static_cast<qint64>(maxWaitTime) - currWait;
    if (waitTime <= 0)
      return false;
  }
}

void ctkEAPooledExecutor::awaitTerminationAfterShutdown() const
{
  QMutexLocker lock(&mutex);
  if (!shutdown_)
    throw ctkIllegalStateException("not in shutdown state");
  while (poolSize_ > 0)
  {
    lock.unlock();
    QMutexLocker shutdownLock(&shutdownMutex);
    waitCond.wait(&shutdownMutex);
    lock.relock();
    // worker is done, wait for possibly not yet finished worker-thread
    foreach(QThread* t, stoppedThreads_)
    {
      t->wait();
    }
  }
}

QList<ctkEARunnable*> ctkEAPooledExecutor::drain()
{
  bool wasInterrupted = false;
  QList<ctkEARunnable*> tasks;
  forever
  {
    try
    {
      ctkEARunnable* x = handOff_->poll(0);
      if (x == 0)
        break;
      else
        tasks.push_back(x);
    }
    catch (const ctkEAInterruptedException& )
    {
      wasInterrupted = true; // postpone re-interrupt until drained
    }
  }
  if (wasInterrupted)
  {
    qobject_cast<ctkEAInterruptibleThread*>(QThread::currentThread())->interrupt();
  }
  return tasks;
}

void ctkEAPooledExecutor::runWhenBlocked()
{
  setBlockedExecutionHandler(&runWhenBlocked_);
}

void ctkEAPooledExecutor::waitWhenBlocked()
{
  setBlockedExecutionHandler(&waitWhenBlocked_);
}

void ctkEAPooledExecutor::discardWhenBlocked()
{
  setBlockedExecutionHandler(&discardWhenBlocked_);
}

void ctkEAPooledExecutor::abortWhenBlocked()
{
  setBlockedExecutionHandler(&abortWhenBlocked_);
}

void ctkEAPooledExecutor::discardOldestWhenBlocked()
{
  setBlockedExecutionHandler(&discardOldestWhenBlocked_);
}

void ctkEAPooledExecutor::execute(ctkEARunnable* command)
{
  forever
  {
    {
      QMutexLocker lock(&mutex);
      if (!shutdown_)
      {
        int size = poolSize_;

        // Ensure minimum number of threads
        if (size < minimumPoolSize_)
        {
          addThread(command);
          return;
        }

        // Try to give to existing thread
        if (handOff_->offer(command, 0))
        {
          return;
        }

        // If cannot handoff and still under maximum, create new thread
        if (size < maximumPoolSize_)
        {
          addThread(command);
          return;
        }
      }
    }

    // Cannot hand off and cannot create -- ask for help
    if (getBlockedExecutionHandler()->blockedAction(command))
    {
      return;
    }
  }
}

ctkEAPooledExecutor::Worker::Worker(ctkEAPooledExecutor* pe, ctkEARunnable* firstTask)
  : firstTask_(firstTask), pe(pe)
{
  if (firstTask) ++firstTask->ref;
}

void ctkEAPooledExecutor::Worker::run()
{
  try
  {
    ctkEARunnable* task = firstTask_;
    firstTask_ = 0;

    if (task != 0)
    {
      const bool autoDelete = task->autoDelete();
      task->run();
      if (autoDelete && !--task->ref) delete task;
    }

    while ( (task = pe->getTask()) != 0)
    {
      const bool autoDelete = task->autoDelete();
      task->run();
      if (autoDelete && !--task->ref) delete task;
    }
  }
  catch (const ctkEAInterruptedException&)
  {
    pe->workerDone(this);
    return;
  }

  pe->workerDone(this);
}

bool ctkEAPooledExecutor::RunWhenBlocked::blockedAction(ctkEARunnable* command)
{
  const bool autoDelete = command->autoDelete();
  command->run();
  if (autoDelete && !--command->ref) delete command;
  return true;
}

ctkEAPooledExecutor::WaitWhenBlocked::WaitWhenBlocked(ctkEAPooledExecutor* pe)
  : pe(pe)
{}

bool ctkEAPooledExecutor::WaitWhenBlocked::blockedAction(ctkEARunnable* command)
{
  {
    QMutexLocker lock(&pe->mutex);
    if (pe->shutdown_)
      return true;
  }
  pe->handOff_->put(command);
  return true;
}

bool ctkEAPooledExecutor::DiscardWhenBlocked::blockedAction(ctkEARunnable* command)
{
  Q_UNUSED(command)
  return true;
}

bool ctkEAPooledExecutor::AbortWhenBlocked::blockedAction(ctkEARunnable* command)
{
  Q_UNUSED(command)
  throw ctkRuntimeException("Pool is blocked");
}

ctkEAPooledExecutor::DiscardOldestWhenBlocked::DiscardOldestWhenBlocked(ctkEAPooledExecutor* pe)
  : pe(pe)
{}

bool ctkEAPooledExecutor::DiscardOldestWhenBlocked::blockedAction(ctkEARunnable* command)
{
  ctkEARunnable* tmp = pe->handOff_->poll(0);
  if (tmp && tmp->autoDelete() && !--tmp->ref) delete tmp;

  if (!pe->handOff_->offer(command, 0))
  {
    const bool autoDelete = command->autoDelete();
    command->run();
    if (autoDelete && !--command->ref) delete command;
  }
  return true;
}

void ctkEAPooledExecutor::addThread(ctkEARunnable* command)
{
  Worker* worker = new Worker(this, command);
  ++worker->ref;
  ctkEAInterruptibleThread* thread = getThreadFactory()->newThread(worker);
  threads_.insert(worker, thread);
  ++poolSize_;

  // do some garbage collection
  foreach (ctkEAInterruptibleThread* t, stoppedThreads_)
  {
    if (t != ctkEAInterruptibleThread::currentThread() && t->isFinished())
    {
      delete t;
      stoppedThreads_.removeAll(t);
    }
  }

  thread->start();
}

void ctkEAPooledExecutor::workerDone(Worker* w)
{
  QMutexLocker lock(&mutex);
  stoppedThreads_ << threads_.take(w);
  if (--poolSize_ == 0 && shutdown_)
  {
    maximumPoolSize_ = minimumPoolSize_ = 0; // disable new threads
    waitCond.wakeAll(); // notify awaitTerminationAfterShutdown
  }

  // Create a replacement if needed
  if (poolSize_ == 0 || poolSize_ < minimumPoolSize_)
  {
    try
    {
      ctkEARunnable* r = handOff_->poll(0);
      if (r != 0)
      {
        if(shutdown_) // just consume task if shut down
        {
          if (r->autoDelete() && !r->ref) delete r;
        }
        else
        {
          addThread(r);
        }
      }
    }
    catch(const ctkEAInterruptedException& ) {
      return;
    }
  }
}

ctkEARunnable* ctkEAPooledExecutor::getTask()
{
  long waitTime;
  {
    QMutexLocker lock(&mutex);
    if (poolSize_ > maximumPoolSize_) // Cause to die if too many threads
      return 0;
    waitTime = (shutdown_)? 0 : keepAliveTime_;
  }
  if (waitTime >= 0)
    return handOff_->poll(waitTime);
  else
    return handOff_->take();
}
