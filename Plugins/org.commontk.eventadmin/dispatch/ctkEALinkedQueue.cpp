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


#include "ctkEALinkedQueue_p.h"

#include "ctkEAInterruptibleThread_p.h"
#include "ctkEAInterruptedException_p.h"

#include <ctkHighPrecisionTimer.h>


ctkEALinkedQueue::ctkEALinkedQueue()
  : head_(new ctkEALinkedNode()), last_(head_), waitingForTake_(0)
{

}

ctkEALinkedQueue::~ctkEALinkedQueue()
{
  delete head_;
}

void ctkEALinkedQueue::put(ctkEARunnable* x)
{
  if (x == 0) throw ctkInvalidArgumentException("QRunnable cannot be null");
  if (ctkEAInterruptibleThread::interrupted()) throw ctkEAInterruptedException();
  insert(x);
}

bool ctkEALinkedQueue::offer(ctkEARunnable* x, long msecs)
{
  Q_UNUSED(msecs)

  if (x == 0) throw ctkInvalidArgumentException("QRunnable cannot be null");
  if (ctkEAInterruptibleThread::interrupted()) throw ctkEAInterruptedException();
  insert(x);
  return true;
}

ctkEARunnable* ctkEALinkedQueue::take()
{
  if (ctkEAInterruptibleThread::interrupted()) throw ctkEAInterruptedException();
  // try to extract. If fail, then enter wait-based retry loop
  ctkEARunnable* x = extract();
  if (x != 0)
    return x;
  else
  {
    {
      QMutexLocker l(&putLock_);
      try
      {
        ++waitingForTake_;
        forever
        {
          x = extract();
          if (x != 0)
          {
            --waitingForTake_;
            return x;
          }
          else
          {
            ctkEAInterruptibleThread::currentThread()->wait(&putLock_, &putLockWait_);
          }
        }
      }
      catch(const ctkEAInterruptedException& ex)
      {
        --waitingForTake_;
        if (x && x->autoDelete() && !--x->ref) delete x;
        putLockWait_.wakeOne();
        throw ex;
      }
    }
  }
}

ctkEARunnable* ctkEALinkedQueue::peek() const
{
  QMutexLocker l(&headLock_);
  ctkEALinkedNode* first = head_->next;
  if (first)
    return first->value;
  else
    return 0;
}

bool ctkEALinkedQueue::isEmpty() const
{
  QMutexLocker l(&headLock_);
  return !(head_->next);
}

ctkEARunnable* ctkEALinkedQueue::poll(long msecs)
{
  if (ctkEAInterruptibleThread::interrupted()) throw ctkEAInterruptedException();
  ctkEARunnable* x = extract();
  if (x != 0)
    return x;
  else
  {
    QMutexLocker l(&putLock_);
    try {
      qint64 waitTime = static_cast<qint64>(msecs);
      ctkHighPrecisionTimer t;
      t.start();
      ++waitingForTake_;
      forever
      {
        x = extract();
        if (x != 0 || waitTime <= 0)
        {
          --waitingForTake_;
          return x;
        }
        else
        {
          ctkEAInterruptibleThread::currentThread()->wait(&putLock_, &putLockWait_, waitTime);
          waitTime = static_cast<qint64>(msecs) - t.elapsedMilli();
        }
      }
    }
    catch(const ctkEAInterruptedException& ex)
    {
      --waitingForTake_;
      if (x && x->autoDelete() && !--x->ref) delete x;
      putLockWait_.wakeOne();
      throw ex;
    }
  }
}

void ctkEALinkedQueue::insert(ctkEARunnable* x)
{
  QMutexLocker l(&putLock_);
  ctkEALinkedNode* p = new ctkEALinkedNode(x);
  {
    QMutexLocker l2(&lastLock_);
    last_->next = p;
    last_ = p;
  }
  if (waitingForTake_ > 0)
  {
    putLockWait_.wakeOne();
  }
}

ctkEARunnable* ctkEALinkedQueue::extract()
{
  QMutexLocker lock(&mutex_);
  {
    QMutexLocker l(&headLock_);
    ctkEARunnable* x = 0;
    ctkEALinkedNode* first(head_->next);
    if (first)
    {
      x = first->value;
      first->value = 0;
      delete head_;
      head_ = first;
    }
    return x;
  }
}
