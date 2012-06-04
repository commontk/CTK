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


#ifndef CTKEACYCLICBARRIER_P_H
#define CTKEACYCLICBARRIER_P_H

#include <QMutex>
#include <QWaitCondition>

class ctkEARunnable;

/**
 * A cyclic barrier is a reasonable choice for a barrier in contexts
 * involving a fixed sized group of threads that
 * must occasionally wait for each other.
 * <p>
 * ctkEACyclicBarriers use an all-or-none breakage model
 * for failed synchronization attempts: If threads
 * leave a barrier point prematurely because of timeout
 * or interruption, others will also leave abnormally
 * (via ctkEABrokenBarrierException), until
 * the barrier is <code>restart</code>ed. This is usually
 * the simplest and best strategy for sharing knowledge
 * about failures among cooperating threads in the most
 * common usages contexts of Barriers.
 * This implementation has the property that interruptions
 * among newly arriving threads can cause as-yet-unresumed
 * threads from a previous barrier cycle to return out
 * as broken. This transmits breakage
 * as early as possible, but with the possible byproduct that
 * only some threads returning out of a barrier will realize
 * that it is newly broken. (Others will not realize this until a
 * future cycle.)
 * <p>
 * Barriers support an optional ctkEARunnable command
 * that is run once per barrier point.
 * <p>
 * <b>Sample usage</b> Here is a code sketch of
 *  a barrier in a parallel decomposition design.
 * \code
 * class Solver
 * {
 *   int N;
 *   float** data;
 *   ctkEACyclicBarrier* barrier;
 *
 *   class Worker : public ctkEARunnable
 *   {
 *     int myRow;
 *
 *   public:
 *     Worker(int row) : myRow(row) {}
 *     void run()
 *     {
 *       while (!done())
 *       {
 *         processRow(myRow);
 *
 *         try
 *         {
 *           barrier.barrier();
 *         }
 *         catch (const ctkEAInterruptedException& ex) { return; }
 *         catch (const ctkEABrokenBarrierException& ex) { return; }
 *       };
 *     }
 *   };
 *
 *   class MyRunnable : public ctkEARunnable
 *   {
 *   public: void run() { mergeRows(...); }
 *   };
 *
 * public:
 *   Solver(float** matrix)
 *    : data(matrix)
 *   {
 *     N = sizeof(matrix)/sizeof(float);
 *     barrier = new ctkEACyclicBarrier(N);
 *     barrier->setBarrierCommand(new MyRunnable());
 *     for (int i = 0; i < N; ++i)
 *     {
 *       new ctkEAInterruptibleThread(new Worker(i))->start();
 *       waitUntilDone();
 *     }
 *   }
 * };
 * \endcode
 *
 * The design of this class was inspired by:
 * http://gee.cs.oswego.edu/dl/classes/EDU/oswego/cs/dl/util/concurrent/intro.html
 **/
class ctkEACyclicBarrier
{

public:

  /**
   * Create a ctkEACyclicBarrier for the indicated number of parties.
   * and the given command to run at each barrier point.
   * @throws ctkInvalidArgumentException if parties less than or equal to zero.
   **/
  ctkEACyclicBarrier(int parties, ctkEARunnable* command = 0);

  /**
   * Set the command to run at the point at which all threads reach the
   * barrier. This command is run exactly once, by the thread
   * that trips the barrier. The command is not run if the barrier is
   * broken.
   * @param command the command to run. If null, no command is run.
   * @return the previous command
   **/
  ctkEARunnable* setBarrierCommand(ctkEARunnable* command);

  /**
   * Returns true if the barrier has been compromised
   * by threads leaving the barrier before a synchronization
   * point (normally due to interruption or timeout).
   * Barrier methods in implementation classes throw
   * ctkEABrokenBarrierException upon detection of breakage.
   * Implementations may also support some means
   * to clear this status.
   **/
  bool broken() const;

  /**
   * Reset to initial state. Clears both the broken status
   * and any record of waiting threads, and releases all
   * currently waiting threads with indeterminate return status.
   * This method is intended only for use in recovery actions
   * in which it is somehow known
   * that no thread could possibly be relying on the
   * the synchronization properties of this barrier.
   **/
  void restart();

  /**
   * Return the number of parties that must meet per barrier
   * point. The number of parties is always at least 1.
   **/
  int parties() const;

  /**
   * Enter barrier and wait for the other parties()-1 threads.
   *
   * @return the arrival index: the number of other parties
   *         that were still waiting
   *         upon entry. This is a unique value from zero to parties()-1.
   *         If it is zero, then the current
   *         thread was the last party to hit barrier point
   *         and so was responsible for releasing the others.
   * @throws ctkEABrokenBarrierException if any other thread
   *         in any previous or current barrier
   *         since either creation or the last <code>restart</code>
   *         operation left the barrier
   *         prematurely due to interruption or time-out. (If so,
   *         the <code>broken</code> status is also set.)
   *         Threads that are notified to have been
   *         interrupted <em>after</em> being released are not considered
   *         to have broken the barrier.
   *         In all cases, the interruption
   *         status of the current thread is preserved, so can be tested
   *         by checking <code>ctkEAInterruptibleThread::interrupted()</code>.
   * @throws ctkEAInterruptedException if this thread was interrupted
   *         during the barrier, and was the one causing breakage.
   *         If so, <code>broken</code> status is also set.
   **/
  int barrier();

  /**
   * Enter barrier and wait at most msecs for the other parties()-1 threads.
   *
   * @return if not timed out, the arrival index: the number of other parties
   *         that were still waiting
   *         upon entry. This is a unique value from zero to parties()-1.
   *         If it is zero, then the current
   *         thread was the last party to hit barrier point
   *         and so was responsible for releasing the others.
   * @throws ctkEABrokenBarrierException if any other thread
   *         in any previous or current barrier
   *         since either creation or the last <code>restart</code>
   *         operation left the barrier
   *         prematurely due to interruption or time-out. (If so,
   *         the <code>broken</code> status is also set.)
   *         Threads that are noticed to have been
   *         interrupted <em>after</em> being released are not considered
   *         to have broken the barrier.
   *         In all cases, the interruption
   *         status of the current thread is preserved, so can be tested
   *         by checking <code>ctkEAInterruptibleThread::interrupted()</code>.
   * @throws ctkEAInterruptedException if this thread was interrupted
   *         during the barrier. If so, <code>broken</code> status is also set.
   * @throws ctkEATimeoutException if this thread timed out waiting for
   *         the barrier. If the timeout occured while already in the
   *         barrier, <code>broken</code> status is also set.
   **/
  int attemptBarrier(long msecs);

protected:

  int doBarrier(bool timed, long msecs);

protected:

  const int parties_;
  bool broken_;
  ctkEARunnable* barrierCommand_;
  int count_; // number of parties still waiting
  int resets_; // incremented on each release
  mutable QMutex mutex;
  mutable QMutex monitor;
  QWaitCondition waitCond;

};

#endif // CTKEACYCLICBARRIER_P_H
