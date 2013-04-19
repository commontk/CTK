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


#include "ctkEACyclicBarrier_p.h"

#include <QRunnable>
#include <QDebug>

#include <dispatch/ctkEAInterruptibleThread_p.h>
#include <dispatch/ctkEAInterruptedException_p.h>

#include "ctkEATimeoutException_p.h"
#include "ctkEABrokenBarrierException_p.h"

#include "ctkHighPrecisionTimer.h"

ctkEACyclicBarrier::ctkEACyclicBarrier(int parties, ctkEARunnable* command)
  : parties_(parties), broken_(false), barrierCommand_(command),
    count_(parties), resets_(0)
{
  if (parties <= 0) throw ctkInvalidArgumentException("parties cannot be negative");
  if (barrierCommand_) ++barrierCommand_->ref;
}

ctkEARunnable* ctkEACyclicBarrier::setBarrierCommand(ctkEARunnable* command)
{
  QMutexLocker lock(&mutex);
  ctkEARunnable* old = barrierCommand_;
  --old->ref;
  barrierCommand_ = command;
  ++barrierCommand_->ref;
  return old;
}

bool ctkEACyclicBarrier::broken() const
{
  QMutexLocker lock(&mutex);
  return broken_;
}

void ctkEACyclicBarrier::restart()
{
  QMutexLocker lock(&mutex);
  broken_ = false;
  ++resets_;
  count_ = parties_;
  waitCond.wakeAll();
}

int ctkEACyclicBarrier::parties() const
{
  return parties_;
}

int ctkEACyclicBarrier::barrier()
{
  return doBarrier(false, 0);
}

int ctkEACyclicBarrier::attemptBarrier(long msecs)
{
  return doBarrier(true, msecs);
}

int ctkEACyclicBarrier::doBarrier(bool timed, long msecs)
{
  QMutexLocker lock(&mutex);
  int index = --count_;

  ctkEAInterruptibleThread* currThread = ctkEAInterruptibleThread::currentThread();
  Q_ASSERT(currThread != 0); // ctkEACyclicBarrier can only be used with ctkEAInterruptibleThread

  if (broken_)
  {
    throw ctkEABrokenBarrierException(index);
  }
  else if (ctkEAInterruptibleThread::interrupted())
  {
    broken_ = true;
    waitCond.wakeAll();
    throw ctkEAInterruptedException();
  }
  else if (index == 0)
  { // tripped
    count_ = parties_;
    ++resets_;
    waitCond.wakeAll();
    try
    {
      if (barrierCommand_)
      {
        const bool autoDelete = barrierCommand_->autoDelete();
        barrierCommand_->run();
        if (autoDelete && !--barrierCommand_->ref) delete barrierCommand_;
      }
      return 0;
    }
    catch (...)
    {
      broken_ = true;
      return 0;
    }
  }
  else if (timed && msecs <= 0)
  {
    broken_ = true;
    waitCond.wakeAll();
    throw ctkEATimeoutException(msecs);
  }
  else
  { // wait until next reset
    int r = resets_;
    ctkHighPrecisionTimer t;
    t.start();
    qint64 waitTime = static_cast<qint64>(msecs);
    forever
    {
      try
      {
        currThread->wait(&mutex, &waitCond, waitTime);
      }
      catch (const ctkEAInterruptedException& ex)
      {
        mutex.lock();
        // Only claim that broken if interrupted before reset
        if (resets_ == r)
        {
          broken_ = true;
          waitCond.wakeAll();
          throw ex;
        }
        else
        {
          currThread->interrupt(); // propagate
        }
      }

      if (broken_)
      {
        throw ctkEABrokenBarrierException(index);
      }
      else if (r != resets_)
      {
        return index;
      }
      else if (timed)
      {
        waitTime = static_cast<qint64>(msecs) - t.elapsedMilli();
        if  (waitTime <= 0)
        {
          broken_ = true;
          waitCond.wakeAll();
          throw ctkEATimeoutException(msecs);
        }
      }
    }
  }
  return 0; // will never be reached
}
