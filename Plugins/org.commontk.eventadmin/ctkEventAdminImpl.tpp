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


#include "dispatch/ctkEADefaultThreadPool_p.h"


template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::ctkEventAdminImpl(
  HandlerTasksInterface* managers, ctkEADefaultThreadPool* syncPool,
  ctkEADefaultThreadPool* asyncPool, int timeout,
  const QStringList& ignoreTimeout)
  : managers(managers)
{
  checkNull(managers, "Managers");
  checkNull(syncPool, "syncPool");
  checkNull(asyncPool, "asyncPool");

  sendManager = new SyncDeliverTasks(syncPool, &syncMasterThread,
                                     (timeout > 100 ? timeout : 0),
                                     ignoreTimeout);

  postManager = new AsyncDeliverTasks(asyncPool, sendManager);
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::~ctkEventAdminImpl()
{
  delete postManager;
  delete sendManager;
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
void ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::postEvent(const ctkEvent& event)
{
  handleEvent(managers.fetchAndAddOrdered(0)->createHandlerTasks(event), postManager);
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
void ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::sendEvent(const ctkEvent& event)
{
  handleEvent(managers.fetchAndAddOrdered(0)->createHandlerTasks(event), sendManager);
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
QString ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::
subscribeSlot(const QObject *subscriber, const char *member, const ctkProperties &properties)
{
  Q_UNUSED(subscriber)
  Q_UNUSED(member)
  Q_UNUSED(properties)
  return QString();
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
void ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::
updateProperties(const QString &subsriptionId, const ctkProperties &properties)
{
  Q_UNUSED(subsriptionId)
  Q_UNUSED(properties)
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
void ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::stop()
{
  // replace the HandlerTasks with a null object that will throw an
  // IllegalStateException on a call to createHandlerTasks
  HandlerTasksInterface* oldManagers =
      this->managers.fetchAndStoreOrdered(&stoppedHandlerTasks);
  delete oldManagers;
  syncMasterThread.stop();
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
void ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::update(HandlerTasksInterface* managers, int timeout,
                               const QStringList& ignoreTimeout)
{
  HandlerTasksInterface* oldManagers = this->managers.fetchAndStoreOrdered(managers);
  delete oldManagers;
  this->sendManager->update(timeout, ignoreTimeout);
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
template<class DeliverTasks>
void ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::handleEvent(const QList<HandlerTask>& managers,
                                                  DeliverTasks* manager)
{
  if (0 < managers.size())
  {
    // This might throw an ctkIllegalStateException in case that we are stopped
    // and the null object for managers was not fast enough established
    // This is needed in the adapter/* classes due to them sending
    // events whenever they receive an event from their source.
    // Service importers that call us regardless of the fact that we are
    // stopped deserve an exception anyways
    manager->execute(managers);
  }
}

template<class HandlerTasks, class SyncDeliverTasks, class AsyncDeliverTasks>
void ctkEventAdminImpl<HandlerTasks,SyncDeliverTasks,AsyncDeliverTasks>::checkNull(void* object, const QString& name)
{
  if(0 == object)
  {
    throw ctkInvalidArgumentException(qPrintable(name + " may not be null"));
  }
}
