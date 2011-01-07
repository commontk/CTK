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


#ifndef CTKEACHANNEL_P_H
#define CTKEACHANNEL_P_H

class ctkEARunnable;

/**
 * Main interface for buffers, queues, pipes, conduits, etc.
 * <p>
 * A ctkEAChannel represents anything that you can put items
 * into and take them out of. Both blocking (put(x), take),
 * and timeouts (offer(x, msecs), poll(msecs)) policies
 * are provided. Using a zero timeout for offer and poll
 * results in a pure balking policy.
 *
 * <p>
 * A given channel implementation might or might not have bounded
 * capacity or other insertion constraints, so in general,
 * you cannot tell if a given put will block.
 *
 * <p>
 * By design, the ctkEAChannel interface does not support any methods
 * to determine the current number of elements being held in the channel.
 * This decision reflects the fact that in concurrent programming,
 * such methods are so rarely useful that including them invites misuse;
 * at best they could provide a snapshot of current
 * state, that could change immediately after being reported.
 * It is better practice to instead use poll and offer to try
 * to take and put elements without blocking. For example,
 * to empty out the current contents of a channel, you could write:
 * \code
 * try
 * {
 *   forever
 *   {
 *     ctkEARunnable* item = channel->poll(0);
 *     if (item)
 *       process(item);
 *     else
 *       break;
 *   }
 *  }
 *  catch(const ctkEAInterruptedException& ex) { ... }
 * \endcode
 *
 * <p>
 * However, it is possible to determine whether an item
 * exists in a ctkEAChannel via <code>peek</code>, which returns
 * but does NOT remove the next item that can be taken (or null
 * if there is no such item). The peek operation has a limited
 * range of applicability, and must be used with care. Unless it
 * is known that a given thread is the only possible consumer
 * of a channel, and that no time-out-based <code>offer</code> operations
 * are ever invoked, there is no guarantee that the item returned
 * by peek will be available for a subsequent take.
 *
 * <p>
 * When appropriate, you can define an isEmpty method to
 * return whether <code>peek</code> returns null.
 *
 * <p>
 * Also, as a compromise, even though it does not appear in the interface,
 * implementation classes that can readily compute the number
 * of elements support a <code>size()</code> method. This allows careful
 * use, for example in queue length monitors, appropriate to the
 * particular implementation constraints and properties.
 *
 * <p>
 * All channels allow multiple producers and/or consumers.
 * They do not support any kind of <em>close</em> method
 * to shut down operation or indicate completion of particular
 * producer or consumer threads.
 * If you need to signal completion, one way to do it is to
 * create a class such as
 * \code
 * class EndOfStream : public ctkEARunnable
 * {
 *    // Application-dependent field/methods
 * };
 * \endcode
 * And to have producers put an instance of this class into
 * the channel when they are done. The consumer side can then
 * check this via
 * \code
 * ctkEARunnable* x = aChannel->take();
 * if (dynamic_cast<EndOfStream*>(x))
 *   // special actions; perhaps terminate
 * else
 *   // process normally
 * \endcode
 *
 * <p>
 * In time-out based methods (poll(msecs) and offer(x, msecs),
 * time bounds are interpreted in
 * a coarse-grained, best-effort fashion. Since there is no
 * way to escape out of a wait for a mutex, time bounds
 * can sometimes be exceeded when there is a lot contention
 * for the channel. Additionally, some Channel semantics entail
 * a ``point of no return'' where, once some parts of the operation
 * have completed, others must follow, regardless of time bound.
 *
 * <p>
 * Interruptions are in general handled as early as possible
 * in all methods. Normally, ctkEAInterruptionExceptions are thrown
 * in put/take and offer(msec)/poll(msec) if interruption
 * is detected upon entry to the method, as well as in any
 * later context surrounding waits.
 *
 * <p>
 * If a put returns normally, an offer
 * returns true, or a put or poll returns non-null, the operation
 * completed successfully.
 * In all other cases, the operation fails cleanly -- the
 * element is not put or taken.
 *
 * The design of this class was inspired from:
 * http://gee.cs.oswego.edu/dl/classes/EDU/oswego/cs/dl/util/concurrent/intro.html
 */
struct ctkEAChannel
{
  virtual ~ctkEAChannel() {}

  /**
   * Place item in the channel, possibly waiting indefinitely until
   * it can be accepted.
   *
   * @param item the element to be inserted. Should be non-null.
   * @throws ctkEAInterruptedException if the current thread has
   *         been interrupted at a point at which interruption
   *         is detected, in which case the element is guaranteed not
   *         to be inserted. Otherwise, on normal return, the element is guaranteed
   *         to have been inserted.
   */
  virtual void put(ctkEARunnable* item) = 0;

  /**
   * Place item in channel only if it can be accepted within
   * msecs milliseconds. The time bound is interpreted in
   * a coarse-grained, best-effort fashion.
   *
   * @param item the element to be inserted. Should be non-null.
   * @param msecs the number of milliseconds to wait. If less than
   *        or equal to zero, the method does not perform any
   *        timed waits, but might still require access to a
   *        synchronization lock, which can impose unbounded
   *        delay if there is a lot of contention for the channel.
   * @return true if accepted, else false
   * @throws ctkEAInterruptedException if the current thread has
   *         been interrupted at a point at which interruption
   *         is detected, in which case the element is guaranteed not
   *         to be inserted (i.e., is equivalent to a false return).
   */
  virtual bool offer(ctkEARunnable* item, long msecs) = 0;

  /**
   * Return and remove an item from channel, possibly waiting
   * indefinitely until such an item exists.
   *
   * @return some item from the channel. Different implementations
   *         may guarantee various properties (such as FIFO) about that item
   * @throws ctkEAInterruptedException if the current thread has
   *         been interrupted at a point at which interruption
   *         is detected, in which case state of the channel is unchanged.
   */
  virtual ctkEARunnable* take() = 0;

  /**
   * Return and remove an item from channel only if one is available within
   * msecs milliseconds. The time bound is interpreted in a coarse
   * grained, best-effort fashion.

   * @param msecs the number of milliseconds to wait. If less than
   *        or equal to zero, the operation does not perform any timed waits,
   *        but might still require access to a synchronization lock,
   *        which can impose unbounded delay if there is a lot of contention
   *        for the channel.
   * @return some item, or null if the channel is empty.
   * @throws ctkEAInterruptedException if the current thread has
   *         been interrupted at a point at which interruption
   *         is detected, in which case state of the channel is unchanged
   *         (i.e., equivalent to a null return).
   */
  virtual ctkEARunnable* poll(long msecs) = 0;

  /**
   * Return, but do not remove object at head of Channel,
   * or null if it is empty.
   */
  virtual ctkEARunnable* peek() const = 0;

};

#endif // CTKEACHANNEL_P_H
