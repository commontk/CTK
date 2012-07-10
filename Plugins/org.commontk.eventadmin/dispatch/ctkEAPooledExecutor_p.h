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


#ifndef CTKEAPOOLEDEXECUTOR_P_H
#define CTKEAPOOLEDEXECUTOR_P_H

#include "ctkEAThreadFactoryUser_p.h"

#include <QHash>
#include <QRunnable>
#include <QWaitCondition>

struct ctkEAChannel;
class ctkEAInterruptibleThread;

/**
 * A tunable, extensible thread pool class. The main supported public
 * method is <code>execute(ctkEARunnable* command)</code>, which can be
 * called instead of directly creating threads to execute commands.
 *
 * <p>
 * Thread pools can be useful for several, usually intertwined
 * reasons:
 *
 * <ul>
 *
 *    <li> To bound resource use. A limit can be placed on the maximum
 *    number of simultaneously executing threads.
 *
 *    <li> To manage concurrency levels. A targeted number of threads
 *    can be allowed to execute simultaneously.
 *
 *    <li> To manage a set of threads performing related tasks.
 *
 *    <li> To minimize overhead, by reusing previously constructed
 *    Thread objects rather than creating new ones.  (Note however
 *    that pools are hardly ever cure-alls for performance problems
 *    associated with thread construction, especially in programs that
 *    themselves internally pool or recycle threads.)
 *
 * </ul>
 *
 * These goals introduce a number of policy parameters that are
 * encapsulated in this class. All of these parameters have defaults
 * and are tunable, either via get/set methods, or, in cases where
 * decisions should hold across lifetimes, via methods that can be
 * easily overridden in subclasses. The main, most commonly set
 * parameters can be established in constructors. Policy choices
 * across these dimensions can and do interact. Be careful, and
 * please read this documentation completely before using!  See also
 * the usage examples below.
 *
 * <dl>
 *   <dt> Queueing
 *
 *   <dd> By default, this pool uses queueless synchronous channels to
 *   to hand off work to threads. This is a safe, conservative policy
 *   that avoids lockups when handling sets of requests that might
 *   have internal dependencies. (In these cases, queuing one task
 *   could lock up another that would be able to continue if the
 *   queued task were to run.)  If you are sure that this cannot
 *   happen, then you can instead supply a queue of some sort (for
 *   example, a BoundedBuffer or LinkedQueue) in the constructor.
 *   This will cause new commands to be queued in cases where all
 *   MaximumPoolSize threads are busy. Queues are sometimes
 *   appropriate when each task is completely independent of others,
 *   so tasks cannot affect each others execution.  For example, in an
 *   http server.  <p>
 *
 *   When given a choice, this pool always prefers adding a new thread
 *   rather than queueing if there are currently fewer than the
 *   current getMinimumPoolSize threads running, but otherwise always
 *   prefers queuing a request rather than adding a new thread. Thus,
 *   if you use an unbounded buffer, you will never have more than
 *   getMinimumPoolSize threads running. (Since the default
 *   minimumPoolSize is one, you will probably want to explicitly
 *   setMinimumPoolSize.)  <p>
 *
 *   While queuing can be useful in smoothing out transient bursts of
 *   requests, especially in socket-based services, it is not very
 *   well behaved when commands continue to arrive on average faster
 *   than they can be processed.  Using bounds for both the queue and
 *   the pool size, along with run-when-blocked policy is often a
 *   reasonable response to such possibilities.  <p>
 *
 *   Queue sizes and maximum pool sizes can often be traded off for
 *   each other. Using large queues and small pools minimizes CPU
 *   usage, OS resources, and context-switching overhead, but can lead
 *   to artifically low throughput. Especially if tasks frequently
 *   block (for example if they are I/O bound), the underlying
 *   OS may be able to schedule time for more threads than you
 *   otherwise allow. Use of small queues or queueless handoffs
 *   generally requires larger pool sizes, which keeps CPUs busier but
 *   may encounter unacceptable scheduling overhead, which also
 *   decreases throughput.  <p>
 *
 *   <dt> Maximum Pool size
 *
 *   <dd> The maximum number of threads to use, when needed. The pool
 *   does not by default preallocate threads.  Instead, a thread is
 *   created, if necessary and if there are fewer than the maximum,
 *   only when an <code>execute</code> request arrives. The default
 *   value is (for all practical purposes) infinite --
 *   <code>std::numeric_limits<int>::max()</code>, so should be set in the
 *   constructor or the set method unless you are just using the pool
 *   to minimize construction overhead. Because task handoffs to idle
 *   worker threads require synchronization that in turn relies on OS
 *   scheduling policies to ensure progress, it is possible that a new
 *   thread will be created even though an existing worker thread has
 *   just become idle but has not progressed to the point at which it
 *   can accept a new task. This phenomenon tends to occur when bursts
 *   of short tasks are executed.  <p>
 *
 *   <dt> Minimum Pool size
 *
 *   <dd> The minimum number of threads to use, when needed (default
 *   1). When a new request is received, and fewer than the minimum
 *   number of threads are running, a new thread is always created to
 *   handle the request even if other worker threads are idly waiting
 *   for work. Otherwise, a new thread is created only if there are
 *   fewer than the maximum and the request cannot immediately be
 *   queued.  <p>
 *
 *   <dt> Preallocation
 *
 *   <dd> You can override lazy thread construction policies via
 *   method createThreads, which establishes a given number of warm
 *   threads. Be aware that these preallocated threads will time out
 *   and die (and later be replaced with others if needed) if not used
 *   within the keep-alive time window. If you use preallocation, you
 *   probably want to increase the keepalive time. The difference
 *   between setMinimumPoolSize and createThreads is that
 *   createThreads immediately establishes threads, while setting the
 *   minimum pool size waits until requests arrive.  <p>
 *
 *   <dt> Keep-alive time
 *
 *   <dd> If the pool maintained references to a fixed set of threads
 *   in the pool, then it would impede garbage collection of otherwise
 *   idle threads. This would defeat the resource-management aspects
 *   of pools. One solution would be to use weak references. However,
 *   this would impose costly and difficult synchronization issues.
 *   Instead, threads are simply allowed to terminate and thus be
 *   destroyed if they have been idle for the given keep-alive time. The
 *   value of this parameter represents a trade-off between destruction
 *   and construction time. The default keep-alive value is one minute, which
 *   means that the time needed to construct and then destroy a thread is
 *   expended at most once per minute.
 *   <p>
 *
 *   To establish worker threads permanently, use a <em>negative</em>
 *   argument to setKeepAliveTime.  <p>
 *
 *   <dt> Blocked execution policy
 *
 *   <dd> If the maximum pool size or queue size is bounded, then it
 *   is possible for incoming <code>execute</code> requests to
 *   block. There are four supported policies for handling this
 *   problem, and mechanics (based on the Strategy Object pattern) to
 *   allow others in subclasses: <p>
 *
 *   <dl>
 *     <dt> Run (the default)
 *     <dd> The thread making the <code>execute</code> request
 *          runs the task itself. This policy helps guard against lockup.
 *     <dt> Wait
 *     <dd> Wait until a thread becomes available.  This
 *          policy should, in general, not be used if the minimum number of
 *          of threads is zero, in which case a thread may never become
 *          available.
 *     <dt> Abort
 *     <dd> Throw a ctkRuntimeException
 *     <dt> Discard
 *     <dd> Throw away the current request and return.
 *     <dt> DiscardOldest
 *     <dd> Throw away the oldest request and return.
 *   </dl>
 *
 *   Other plausible policies include raising the maximum pool size
 *   after checking with some other objects that this is OK.  <p>
 *
 *   These cases can never occur if the maximum pool size is unbounded
 *   or the queue is unbounded.  In these cases you instead face
 *   potential resource exhaustion.)  The execute method does not
 *   throw any checked exceptions in any of these cases since any
 *   errors associated with them must normally be dealt with via
 *   handlers or callbacks. (Although in some cases, these might be
 *   associated with throwing unchecked exceptions.)  You may wish to
 *   add special implementations even if you choose one of the listed
 *   policies. For example, the supplied Discard policy does not
 *   inform the caller of the drop. You could add your own version
 *   that does so.  Since choice of policies is normally a system-wide
 *   decision, selecting a policy affects all calls to
 *   <code>execute</code>.  If for some reason you would instead like
 *   to make per-call decisions, you could add variant versions of the
 *   <code>execute</code> method (for example,
 *   <code>executeIfWouldNotBlock</code>) in subclasses.  <p>
 *
 *   <dt> Thread construction parameters
 *
 *   <dd> A settable ctkEAThreadFactory establishes each new thread.  By
 *   default, it merely generates a new instance of class ctkEAInterruptibleThread, but
 *   can be changed to use a ctkEAInterruptibleThread subclass, to set priorities,
 *   ThreadLocals, etc.  <p>
 *
 *   <dt> Interruption policy
 *
 *   <dd> Worker threads check for interruption after processing each
 *   command, and terminate upon interruption.  Fresh threads will
 *   replace them if needed. Thus, new tasks will not start out in an
 *   interrupted state due to an uncleared interruption in a previous
 *   task. Also, unprocessed commands are never dropped upon
 *   interruption. It would conceptually suffice simply to clear
 *   interruption between tasks, but implementation characteristics of
 *   interruption-based methods are uncertain enough to warrant this
 *   conservative strategy. It is a good idea to be equally
 *   conservative in your code for the tasks running within pools.
 *   <p>
 *
 *   <dt> Shutdown policy
 *
 *   <dd> The interruptAll method interrupts, but does not disable the
 *   pool. Two different shutdown methods are supported for use when
 *   you do want to (permanently) stop processing tasks. Method
 *   shutdownAfterProcessingCurrentlyQueuedTasks waits until all
 *   current tasks are finished. The shutDownNow method interrupts
 *   current threads and leaves other queued requests unprocessed.
 *   <p>
 *
 *   <dt> Handling requests after shutdown
 *
 *   <dd> When the pool is shutdown, new incoming requests are handled
 *   by the blockedExecutionHandler. By default, the handler is set to
 *   discard new requests, but this can be set with an optional
 *   argument to method
 *   shutdownAfterProcessingCurrentlyQueuedTasks. <p> Also, if you are
 *   using some form of queuing, you may wish to call method drain()
 *   to remove (and return) unprocessed commands from the queue after
 *   shutting down the pool and its clients. If you need to be sure
 *   these commands are processed, you can then run() each of the
 *   commands in the list returned by drain().
 *
 * </dl>
 * <p>
 *
 * <b>Usage examples.</b>
 * <p>
 *
 * Probably the most common use of pools is in statics or singletons
 * accessible from a number of classes in a library; for example:
 *
 * \code
 * class MyPool
 * {
 * public:
 *   static ctkEAPooledExecutor pool;
 * };
 *
 * // initialize to use a maximum of 8 threads.
 * ctkEAPooledExecutor MyPool::pool(8);
 * \endcode
 *
 * Here are some sample variants in initialization:
 * <ol>
 *  <li> Using a bounded buffer of 10 tasks, at least 4 threads (started only
 *       when needed due to incoming requests), but allowing
 *       up to 100 threads if the buffer gets full.
 *     \code
 *        pool(new ctkEABoundedBuffer(10), 100);
 *        pool.setMinimumPoolSize(4);
 *     \endcode
 *  <li> Same as (1), except pre-start 9 threads, allowing them to
 *        die if they are not used for five minutes.
 *     \code
 *        pool(new ctkEABoundedBuffer(10), 100);
 *        pool.setMinimumPoolSize(4);
 *        pool.setKeepAliveTime(1000 * 60 * 5);
 *        pool.createThreads(9);
 *     \endcode
 *  <li> Same as (2) except clients abort if both the buffer is full and
 *       all 100 threads are busy:
 *     \code
 *        pool(new ctkEABoundedBuffer(10), 100);
 *        pool.setMinimumPoolSize(4);
 *        pool.setKeepAliveTime(1000 * 60 * 5);
 *        pool.abortWhenBlocked();
 *        pool.createThreads(9);
 *     \endcode
 *  <li> An unbounded queue serviced by exactly 5 threads:
 *     \code
 *        pool(new ctkEALinkedQueue());
 *        pool.setKeepAliveTime(-1); // live forever
 *        pool.createThreads(5);
 *     \endcode
 *  </ol>
 *
 * <p>
 * <b>Usage notes.</b>
 * <p>
 *
 * Pools do not mesh well with using thread-specific storage. Thread local
 * data usually relies on the identity of a
 * thread executing a particular task. Pools use the same thread to
 * perform different tasks.  <p>
 *
 * If you need a policy not handled by the parameters in this class
 * consider writing a subclass.  <p>
 *
 * The design of this class was inspired by:
 * href="http://gee.cs.oswego.edu/dl/classes/EDU/oswego/cs/dl/util/concurrent/intro.html
 **/
class ctkEAPooledExecutor : public ctkEAThreadFactoryUser
{

public:

  /**
   * The maximum pool size; used if not otherwise specified.  Default
   * value is essentially infinite (std::numeric_limits<int>::max())
   **/
  static const int DEFAULT_MAXIMUMPOOLSIZE; // = std::numeric_limits<int>::max()

  /**
   * The minimum pool size; used if not otherwise specified. Default
   * value is 1.
   **/
  static const int  DEFAULT_MINIMUMPOOLSIZE; // = 1

  /**
   * The maximum time to keep worker threads alive waiting for new
   * tasks; used if not otherwise specified. Default value is one
   * minute (60000 milliseconds).
   **/
  static const long DEFAULT_KEEPALIVETIME; // = 60 * 1000

  /**
   * Class for actions to take when execute() blocks. Uses Strategy
   * pattern to represent different actions. You can add more in
   * subclasses, and/or create subclasses of these. If so, you will
   * also want to add or modify the corresponding methods that set the
   * current blockedExectionHandler_.
   **/
  struct BlockedExecutionHandler
  {
    virtual ~BlockedExecutionHandler() {}

    /**
     * Return true if successfully handled so, execute should
     * terminate; else return false if execute loop should be retried.
     **/
    virtual bool blockedAction(ctkEARunnable* command) = 0;
  };

protected:

  class Worker;

  mutable QMutex shutdownMutex;
  mutable QWaitCondition waitCond;

  /** The maximum number of threads allowed in pool. **/
  int maximumPoolSize_;

  /** The minumum number of threads to maintain in pool. **/
  int minimumPoolSize_;

  /**  Current pool size.  **/
  int poolSize_;

  /** The maximum time for an idle thread to wait for new task. **/
  long keepAliveTime_;

  /**
   * Shutdown flag - latches true when a shutdown method is called
   * in order to disable queuing/handoffs of new tasks.
   **/
  bool shutdown_;

  /**
   * The channel used to hand off the command to a thread in the pool.
   **/
  ctkEAChannel* handOff_;

  /**
   * The set of active threads, declared as a map from workers to
   * their threads. This is needed by the interruptAll() method.  It
   * may also be useful in subclasses that need to perform other
   * thread management chores.
   **/
  QHash<Worker*, ctkEAInterruptibleThread*> threads_;

  /**
   * Keeps a list of stopped threads which will be deleted later.
   */
  QList<ctkEAInterruptibleThread*> stoppedThreads_;

  /** The current handler for unserviceable requests. **/
  BlockedExecutionHandler* blockedExecutionHandler_;


public:

  /**
   * Create a new pool that uses the supplied Channel for queuing, and
   * with all default parameter settings except for maximum pool size.
   **/
  ctkEAPooledExecutor(ctkEAChannel* channel, int maxPoolSize = DEFAULT_MAXIMUMPOOLSIZE);

  ~ctkEAPooledExecutor();

  /**
   * Return the maximum number of threads to simultaneously execute
   * New unqueued requests will be handled according to the current
   * blocking policy once this limit is exceeded.
   **/
  int getMaximumPoolSize() const;

  /**
   * Set the maximum number of threads to use. Decreasing the pool
   * size will not immediately kill existing threads, but they may
   * later die when idle.
   *
   * @throws ctkInvalidArgumentException if less or equal to zero.
   *         (It is not considered an error to set the maximum to be less than than
   *         the minimum. However, in this case there are no guarantees
   *         about behavior.)
   **/
  void setMaximumPoolSize(int newMaximum);

  /**
   * Return the minimum number of threads to simultaneously execute.
   * (Default value is 1).  If fewer than the mininum number are
   * running upon reception of a new request, a new thread is started
   * to handle this request.
   **/
  int getMinimumPoolSize() const;

  /**
   * Set the minimum number of threads to use.
   *
   * @throws ctkInvalidArgumentException if less than zero. (It is not
   *         considered an error to set the minimum to be greater than the
   *         maximum. However, in this case there are no guarantees about
   *         behavior.)
   **/
  void setMinimumPoolSize(int newMinimum);

  /**
   * Return the current number of active threads in the pool.  This
   * number is just a snaphot, and may change immediately upon
   * returning
   **/
  int getPoolSize() const;

  /**
   * Return the number of milliseconds to keep threads alive waiting
   * for new commands. A negative value means to wait forever. A zero
   * value means not to wait at all.
   **/
  long getKeepAliveTime() const;

  /**
   * Set the number of milliseconds to keep threads alive waiting for
   * new commands. A negative value means to wait forever. A zero
   * value means not to wait at all.
   **/
  void setKeepAliveTime(long msecs);

  /** Get the handler for blocked execution **/
  BlockedExecutionHandler* getBlockedExecutionHandler() const;

  /** Set the handler for blocked execution **/
  void setBlockedExecutionHandler(BlockedExecutionHandler* h);

  /**
   * Create and start up to numberOfThreads threads in the pool.
   * Return the number created. This may be less than the number
   * requested if creating more would exceed maximum pool size bound.
   **/
  int createThreads(int numberOfThreads);

  /**
   * Interrupt all threads in the pool, causing them all to
   * terminate. Assuming that executed tasks do not disable (clear)
   * interruptions, each thread will terminate after processing its
   * current task. Threads will terminate sooner if the executed tasks
   * themselves respond to interrupts.
   **/
  void interruptAll();

  /**
   * Interrupt all threads and disable construction of new
   * threads. Any tasks entered after this point will be discarded. A
   * shut down pool cannot be restarted.
   */
  void shutdownNow();

  /**
   * Interrupt all threads and disable construction of new
   * threads. Any tasks entered after this point will be handled by
   * the given BlockedExecutionHandler. A shut down pool cannot be
   * restarted.
   */
  void shutdownNow(BlockedExecutionHandler* handler);

  /**
   * Terminate threads after processing all elements currently in
   * queue. Any tasks entered after this point will be discarded. A
   * shut down pool cannot be restarted.
   **/
  void shutdownAfterProcessingCurrentlyQueuedTasks();

  /**
   * Terminate threads after processing all elements currently in
   * queue. Any tasks entered after this point will be handled by the
   * given BlockedExecutionHandler. A shut down pool cannot be
   * restarted.
   **/
  void shutdownAfterProcessingCurrentlyQueuedTasks(BlockedExecutionHandler* handler);

  /**
   * Return true if a shutDown method has succeeded in terminating all
   * threads.
   */
  bool isTerminatedAfterShutdown() const;

  /**
   * Wait for a shutdown pool to fully terminate, or until the timeout
   * has expired. This method may only be called <em>after</em>
   * invoking shutdownNow or
   * shutdownAfterProcessingCurrentlyQueuedTasks.
   *
   * @param maxWaitTime  the maximum time in milliseconds to wait
   * @return true if the pool has terminated within the max wait period
   * @throws ctkIllegalStateException if shutdown has not been requested
   * @throws ctkEAInterruptedException if the current thread has been interrupted in the course of waiting
   */
  bool awaitTerminationAfterShutdown(long maxWaitTime) const;

  /**
   * Wait for a shutdown pool to fully terminate.  This method may
   * only be called <em>after</em> invoking shutdownNow or
   * shutdownAfterProcessingCurrentlyQueuedTasks.
   *
   * @throws ctkIllegalStateException if shutdown has not been requested
   * @throws ctkEAInterruptedException if the current thread has been interrupted in the course of waiting
   */
  void awaitTerminationAfterShutdown() const;

  /**
   * Remove all unprocessed tasks from pool queue, and return them in
   * a QList. This method should be used only when there are
   * not any active clients of the pool. Otherwise you face the
   * possibility that the method will loop pulling out tasks as
   * clients are putting them in. This method can be useful after
   * shutting down a pool (via shutdownNow) to determine whether there
   * are any pending tasks that were not processed. You can then, for
   * example execute all unprocessed commands via code along the lines
   * of:
   *
   * \code
   *   QList<ctkEARunnable*> tasks = pool.drain();
   *   foreach (ctkEARunnable* runnable, tasks)
   *     runnable->run();
   * \endcode
   **/
  QList<ctkEARunnable*> drain();

  /**
   * Set the policy for blocked execution to be that the current
   * thread executes the command if there are no available threads in
   * the pool.
   **/
  void runWhenBlocked();

  /**
   * Set the policy for blocked execution to be to wait until a thread
   * is available, unless the pool has been shut down, in which case
   * the action is discarded.
   **/
  void waitWhenBlocked();

  /**
   * Set the policy for blocked execution to be to return without
   * executing the request.
   **/
  void discardWhenBlocked();

  /**
   * Set the policy for blocked execution to be to
   * throw a RuntimeException.
   **/
  void abortWhenBlocked();

  /**
   * Set the policy for blocked execution to be to discard the oldest
   * unhandled request
   **/
  void discardOldestWhenBlocked();

  /**
   * Arrange for the given command to be executed by a thread in this
   * pool. The method normally returns when the command has been
   * handed off for (possibly later) execution.
   **/
  void execute(ctkEARunnable* command);


protected:

  /**
   * Class defining the basic run loop for pooled threads.
   **/
  class Worker : public ctkEARunnable
  {
  protected:
    ctkEARunnable* firstTask_;

  public:
    Worker(ctkEAPooledExecutor* pe, ctkEARunnable* firstTask);

    void run();

  private:

    ctkEAPooledExecutor* pe;
  };

  /** Class defining Run action. **/
  struct RunWhenBlocked : public BlockedExecutionHandler
  {
    bool blockedAction(ctkEARunnable* command);
  };

  RunWhenBlocked runWhenBlocked_;

  /** Class defining Wait action. **/
  struct WaitWhenBlocked : public BlockedExecutionHandler
  {
    WaitWhenBlocked(ctkEAPooledExecutor* pe);

    bool blockedAction(ctkEARunnable* command);

  private:
    ctkEAPooledExecutor* pe;
  };

  WaitWhenBlocked waitWhenBlocked_;

  /** Class defining Discard action. **/
  struct DiscardWhenBlocked : public BlockedExecutionHandler
  {
    bool blockedAction(ctkEARunnable* command);
  };

  DiscardWhenBlocked discardWhenBlocked_;

  /** Class defining Abort action. **/
  struct AbortWhenBlocked : public BlockedExecutionHandler
  {
    bool blockedAction(ctkEARunnable* command);
  };

  AbortWhenBlocked abortWhenBlocked_;

  /**
   * Class defining DiscardOldest action. Under this policy, at most
   * one old unhandled task is discarded. If the new task can then be
   * handed off, it is. Otherwise, the new task is run in the current
   * thread (i.e., RunWhenBlocked is used as a backup policy.)
   **/
  struct DiscardOldestWhenBlocked : public BlockedExecutionHandler
  {
    DiscardOldestWhenBlocked(ctkEAPooledExecutor* pe);

    bool blockedAction(ctkEARunnable* command);

  private:
    ctkEAPooledExecutor* pe;
  };

  DiscardOldestWhenBlocked discardOldestWhenBlocked_;


  /**
   * Create and start a thread to handle a new command. Call only
   * when holding lock.
   **/
  void addThread(ctkEARunnable* command);

  /**
   * Cleanup method called upon termination of worker thread.
   **/
  void workerDone(Worker* w);

  /**
   * Get a task from the handoff queue, or null if shutting down.
   **/
  ctkEARunnable* getTask();

};

#endif // CTKEAPOOLEDEXECUTOR_P_H
