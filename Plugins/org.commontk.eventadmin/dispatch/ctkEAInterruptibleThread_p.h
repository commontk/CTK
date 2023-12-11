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


#ifndef CTKEAINTERRUPTIBLETHREAD_P_H
#define CTKEAINTERRUPTIBLETHREAD_P_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QAtomicInt>
#include <QRunnable>
#include <climits>

/**
 * A QRunnable subclass with simple reference counting.
 */
class ctkEARunnable : public QRunnable
{

public:

  int ref;

  ctkEARunnable() : ref(0) {}
  bool autoDelete() const { return ref != -1; }
  void setAutoDelete(bool autoDelete) { ref = autoDelete ? 0 : -1; }
};

class ctkEAScopedRunnableReference
{

public:

  ctkEAScopedRunnableReference(ctkEARunnable* runnable)
    : runnable(runnable)
  {
    ++runnable->ref;
  }

  ~ctkEAScopedRunnableReference()
  {
    if (!--runnable->ref) delete runnable;
  }

private:

  ctkEARunnable* runnable;
};

/**
 * A QThread subclass which can be interrupted when waiting
 * on a wait condition.
 */
class ctkEAInterruptibleThread : public QThread, public QMutex
{
  Q_OBJECT

public:

  /**
   * Creates a new interruptible thread object. If <code>command</code>
   * is not null, it's run() method will be executed when the thread
   * is started.
   *
   * @param command The runnable to run when starting this thread.
   * @param parent The parent object.
   * @see run()
   */
  ctkEAInterruptibleThread(ctkEARunnable* command = 0, QObject* parent = 0);

  /**
   * Returns a pointer to the currently executing interruptible
   * thread object.
   *
   * @return The currently executing ctkEAInterruptibleThread object
   */
  static ctkEAInterruptibleThread* currentThread();

  /**
   * Waits on the wait condition <code>waitCond</code> for the specified
   * amount of msecs <code>time</code> until another thread invokes the
   * wake() or wakeAll() method on the given wait condition.
   * If <code>time</code> is ULONG_MAX
   * or 0, this method waits forever (unless interrupted).
   *
   * <p>
   * The given <code>mutex</code> must be locked prior to calling this method.
   * This method will unlock the mutex and wait on the wait condition. After
   * returning, the mutex will be in its original state.
   *
   * @see interrupt()
   */
  void wait(QMutex* mutex, QWaitCondition* waitCond, unsigned long time = ULONG_MAX);

  /**
   * Waits for this thread to finish execution.
   */
  void join();

  /**
   * Interrupts this thread.
   *
   * <p>
   * If this thread is blocked in an invocation of the wait() method, then its
   * interrupt status will be cleared and it will receive a ctkEAInterruptedException.
   * Otherwise this thread's interrupt status will be set.
   */
  void interrupt();

  /**
   * Tests whether the current thread has been interrupted. The interrupted status
   * of the thread is  cleared by this method. In other words, if this method
   * were to be called twice in succession, the second call would return false
   * (unless the current thread were interrupted again, after the first call had
   * cleared its interrupted status and before the second call had examined it).
   *
   * @return <code>true</code> if the current thread has been interrupted;
   *         <code>false</code> otherwise.
   */
  static bool interrupted();

  /**
   * Tests whether this thread has been interrupted. The interrupted status
   * of the thread is unaffected by this method.
   *
   * @return <code>true</code> if this thread has been interrupted;
   *         <code>false</code> otherwise.
   */
  bool isInterrupted() const;

  /**
   * If this thread was constructed using a separate ctkEARunnable run object,
   * then that ctkEARunnable object's run method is called; otherwise, this
   * method calls QThread::run() which will start a local event loop.
   */
  void run();

private:

  ctkEARunnable* command;
  bool deleteCmd;

  QAtomicPointer<QWaitCondition> currWaitCond_;
  mutable QAtomicInt interrupted_;
  mutable QAtomicInt isWaiting_;
};

#endif // CTKEAINTERRUPTIBLETHREAD_P_H
