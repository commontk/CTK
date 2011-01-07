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


#ifndef CTKEASYNCTHREAD_P_H
#define CTKEASYNCTHREAD_P_H

#include <QThread>
#include <QAtomicPointer>
#include <QAtomicInt>

#include "../dispatch/ctkEAInterruptibleThread_p.h"
#include "../util/ctkEARendezvous_p.h"

/**
 * This thread class is used for sending the events
 * synchronously.
 */
class ctkEASyncThread : public ctkEAInterruptibleThread
{

  Q_OBJECT

private:

  /** Counter to track the nesting level. */
  mutable QAtomicInt counter;

  /** The barriers for synchronizing. */
  mutable QAtomicPointer<ctkEARendezvous> timerBarrier;
  mutable QAtomicPointer<ctkEARendezvous> cascadingBarrier;

public:

  /**
   * Constructor used by the thread pool.
   */
  ctkEASyncThread(ctkEARunnable* target, QObject* parent = 0);

  void init(ctkEARendezvous* timerBarrier, ctkEARendezvous* cascadingBarrier);
  void uninit();

  ctkEARendezvous* getTimerBarrier() const;
  ctkEARendezvous* getCascadingBarrier() const;

  bool isTopMostHandler() const;

  void innerEventHandlingStart();
  void innerEventHandlingStopped();
};

#endif // CTKEASYNCTHREAD_P_H
