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


#ifndef CTKEALINKEDQUEUE_P_H
#define CTKEALINKEDQUEUE_P_H

#include "ctkEAChannel_p.h"

#include <dispatch/ctkEAInterruptibleThread_p.h>

#include <QMutex>
#include <QWaitCondition>

/** A standard linked list node used in various queue classes **/
struct ctkEALinkedNode
{
  ctkEARunnable* value;
  ctkEALinkedNode* next;

  ctkEALinkedNode(ctkEARunnable* x = 0)
    : value(x), next(0)
  { if (x && x->autoDelete()) ++x->ref; }

  ctkEALinkedNode(ctkEARunnable* x, ctkEALinkedNode* n)
    : value(x), next(n)
  { if (x && x->autoDelete()) ++x->ref; }

  ~ctkEALinkedNode()
  {
    if (value && value->autoDelete())
    {
      --value->ref;
    }
  }

};

/**
 * A linked list based channel implementation.
 * The algorithm avoids contention between puts
 * and takes when the queue is not empty.
 * Normally a put and a take can proceed simultaneously.
 * (Although it does not allow multiple concurrent puts or takes.)
 * This class tends to perform more efficently than
 * other ctkEAChannel implementations in producer/consumer
 * applications.
 */
class ctkEALinkedQueue : public ctkEAChannel
{

protected:

  /**
   * Dummy header node of list. The first actual node, if it exists, is always
   * at head_->next. After each take, the old first node becomes the head.
   **/
  ctkEALinkedNode* head_;

  mutable QMutex headLock_;
  QMutex mutex_;

  /**
   * Helper monitor for managing access to last node.
   **/
  QMutex putLock_;
  QWaitCondition putLockWait_;

  /**
   * The last node of list. put() appends to list, so modifies last_
   **/
  ctkEALinkedNode* last_;
  QMutex lastLock_;

  /**
   * The number of threads waiting for a take.
   * Notifications are provided in put only if greater than zero.
   * The bookkeeping is worth it here since in reasonably balanced
   * usages, the notifications will hardly ever be necessary, so
   * the call overhead to notify can be eliminated.
   **/
  int waitingForTake_;

public:

  ctkEALinkedQueue();
  ~ctkEALinkedQueue();

  void put(ctkEARunnable* x);

  bool offer(ctkEARunnable* x, long msecs);

  ctkEARunnable* take();

  ctkEARunnable* peek() const;

  bool isEmpty() const;

  ctkEARunnable* poll(long msecs);

protected:

  /** Main mechanics for put/offer **/
  void insert(ctkEARunnable* x);

  /** Main mechanics for take/poll **/
  ctkEARunnable* extract();

};

#endif // CTKEALINKEDQUEUE_P_H
