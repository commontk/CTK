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


#ifndef CTKEVENTADMINIMPL_P_H
#define CTKEVENTADMINIMPL_P_H

#include "handler/ctkEAHandlerTasks_p.h"
#include "tasks/ctkEADeliverTask_p.h"
#include "dispatch/ctkEASyncMasterThread_p.h"

class ctkEADefaultThreadPool;

/**
 * This is the actual implementation of the OSGi R4 Event Admin Service (see the
 * Compendium 113 for details). The implementation uses a <tt>ctkEAHandlerTasks</tt>
 * in order to determine applicable <tt>ctkEventHandler</tt> for a specific event and
 * subsequently dispatches the event to the handlers via <tt>ctkEADeliverTask</tt>s.
 * To do this, it uses two different <tt>ctkEADeliverTask</tt>s one for asynchronous and
 * one for synchronous event delivery depending on whether its <tt>post()</tt> or
 * its <tt>send()</tt> method is called. Note that the actual work is done in the
 * implementations of the <tt>ctkEADeliverTask</tt>s. Additionally, a stop method is
 * provided that prevents subsequent events to be delivered.
 */
template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
class ctkEventAdminImpl
{

private:

  typedef ctkEAHandlerTask<HandlerTasks> HandlerTask;

  typedef ctkEADeliverTask<SyncDeliverTasks, HandlerTask> SyncDeliverTaskInterface;
  typedef ctkEADeliverTask<AsyncDeliverTasks, HandlerTask> AsyncDeliverTaskInterface;

  typedef ctkEAHandlerTasks<HandlerTasks> HandlerTasksInterface;

  // The factory used to determine applicable ctkEventHandlers - this will be replaced
  // by a null object in stop() that subsequently throws an IllegalStateException
  QAtomicPointer<HandlerTasksInterface> managers;

  // The asynchronous event dispatcher
  AsyncDeliverTaskInterface* postManager;

  // The (interruptible) thread where sync events are handled
  ctkEASyncMasterThread syncMasterThread;

  // The synchronous event dispatcher
  SyncDeliverTasks* sendManager;

  struct StoppedHandlerTasks : public ctkEAHandlerTasks<HandlerTasks>
  {
    /**
     * This is a null object and this method will throw an
     * ctkIllegalStateException due to the plugin being stopped.
     *
     * @param event An event that is not used.
     *
     * @return This method does not return normally
     *
     * @throws ctkIllegalStateException - This is a null object and this method
     *          will always throw an ctkIllegalStateException
     */
    QList<ctkEAHandlerTask<HandlerTasks> > createHandlerTasks(const ctkEvent&)
    {
      throw ctkIllegalStateException("The EventAdmin is stopped");
    }
  };

  StoppedHandlerTasks stoppedHandlerTasks;

public:

  /**
   * The constructor of the <tt>ctkEventAdmin</tt> implementation. The
   * <tt>HandlerTasksInterface</tt> factory is used to determine applicable
   * <tt>ctkEventHandler</tt> for a given event. Additionally, the two
   * <tt>ctkEADeliverTasks</tt> are used to dispatch the event.
   *
   * @param managers The factory used to determine applicable <tt>ctkEventHandler</tt>
   * @param syncPool The synchronous thread pool
   * @param asyncPool The asynchronous thread pool
   */
  ctkEventAdminImpl(HandlerTasksInterface* managers,
                    ctkEADefaultThreadPool* syncPool,
                    ctkEADefaultThreadPool* asyncPool,
                    int timeout,
                    const QStringList& ignoreTimeout);

  ~ctkEventAdminImpl();

  /**
   * Post an asynchronous event.
   *
   * @param event The event to be posted by this service
   *
   * @throws ctkIllegalStateException - In case we are stopped
   *
   * @see ctkEventAdmin#postEvent(const ctkEvent&)
   */
  void postEvent(const ctkEvent& event);

  /**
   * Send a synchronous event.
   *
   * @param event The event to be send by this service
   *
   * @throws ctkIllegalStateException - In case we are stopped
   *
   * @see ctkEventAdmin#sendEvent(const ctkEvent&)
   */
  void sendEvent(const ctkEvent& event);

  QString subscribeSlot(const QObject *subscriber, const char *member, const ctkProperties &properties);

  void updateProperties(const QString &subsriptionId, const ctkProperties &properties);

  /**
   * This method can be used to stop the delivery of events. The managers variable is
   * replaced with a null object that throws an ctkIllegalStateException on a call
   * to <tt>createHandlerTasks()</tt>.
   */
  void stop();

  /**
   * Update the event admin with new configuration.
   */
  void update(HandlerTasksInterface* managers, int timeout,
              const QStringList& ignoreTimeout);

private:

  /**
   * This is a utility method that uses the given ctkEADeliverTasks to create a
   * dispatch task that subsequently is used to dispatch the given ctkEAHandlerTasks.
   */
  template<class DeliverTasks>
  void handleEvent(const QList<HandlerTask>& managers,
                   DeliverTasks* manager);

  /**
   * This is a utility method that will throw a <tt>ctkInvalidArgumentException</tt>
   * in case that the given object is null. The message will be of the form
   * "${name} + may not be null".
   */
  void checkNull(void* object, const QString& name);
};

#include "ctkEventAdminImpl.tpp"

#endif // CTKEVENTADMINIMPL_P_H
