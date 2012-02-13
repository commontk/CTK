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


#ifndef CTKEASYNCDELIVERTASKS_P_H
#define CTKEASYNCDELIVERTASKS_P_H

#include "ctkEADeliverTask_p.h"

#include <QMutex>

class ctkEADefaultThreadPool;
class ctkEASyncMasterThread;

/**
 * This class does the actual work of the synchronous event delivery.
 *
 * This is the heart of the event delivery. If an event is delivered
 * without timeout handling, the event is directly delivered using
 * the calling thread.
 * If timeout handling is enabled, a new thread is taken from the
 * thread pool and this thread is used to deliver the event.
 * The calling thread is blocked until either the deliver is finished
 * or the timeout occurs.
 * <p><tt>
 * Note that in case of a timeout while a task is disabled the thread
 * is released and we spin-off a new thread that resumes the disabled
 * task hence, this is the only place were we break the semantics of
 * the synchronous delivery. While the only one to notice this is the
 * timed-out handler - it is the fault of this handler too (i.e., it
 * blocked the dispatch for to long) but since it will not receive
 * events anymore it will not notice this semantic difference except
 * that it might not see events it already sent before.
 * </tt></pre>
 *
 * If during an event delivery a new event should be delivered from
 * within the event handler, the timeout handler is stopped for the
 * delivery time of the inner event!
 */
template<class HandlerTask>
class ctkEASyncDeliverTasks : public ctkEADeliverTask<ctkEASyncDeliverTasks<HandlerTask>, HandlerTask>
{

private:

  /** The thread pool used to spin-off new threads. */
  ctkEADefaultThreadPool* pool;

  /** This is a ctkEAInterruptibleThread used to execute the handlers */
  ctkEASyncMasterThread* syncMasterThread;

  /** The timeout for event handlers, 0 = disabled. */
  long timeout;

  /**
   * The matcher interface for checking if timeout handling
   * is disabled for the handler.
   * Matching is based on the class name of the event handler.
   */
  struct Matcher
  {
    virtual ~Matcher() {}
    virtual bool match(const QString& className) const = 0;
  };

  /** Match a class name. */
  struct ClassMatcher : public Matcher
  {
  private:
    const QString className;

  public:
    ClassMatcher(const QString& name)
      : className(name)
    {}

    bool match(const QString& name) const
    {
      return className == name;
    }
  };

  /** The matchers for ignore timeout handling. */
  QList<Matcher*> ignoreTimeoutMatcher;

  QMutex mutex;

public:

  /**
   * Construct a new sync deliver tasks.
   * @param pool The thread pool used to spin-off new threads.
   * @param timeout The timeout for an event handler, 0 = disabled
   */
  ctkEASyncDeliverTasks(ctkEADefaultThreadPool* pool, ctkEASyncMasterThread* syncMasterThread,
                        long timeout, const QList<QString>& ignoreTimeout);

  void update(long timeout, const QList<QString>& ignoreTimeout);

  /**
   * This blocks an unrelated thread used to send a synchronous event until the
   * event is send (or a timeout occurs).
   *
   * @param tasks The event handler dispatch tasks to execute
   *
   * @see ctkEADeliverTask#execute(const QList<ctkEAHandlerTask>&)
   */
  void execute(const QList<HandlerTask>& tasks);

  void executeInSyncMaster(const QList<HandlerTask>& tasks);

private:

  /**
   * This method defines if a timeout handling should be used for the
   * task.
   * @param tasks The event handler dispatch tasks to execute
   */
  bool useTimeout(const HandlerTask& task);

};

#include "ctkEASyncDeliverTasks.tpp"

#endif // CTKEASYNCDELIVERTASKS_P_H
