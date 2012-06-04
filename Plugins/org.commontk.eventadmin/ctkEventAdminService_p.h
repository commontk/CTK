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


#ifndef CTKEVENTADMINSERVICE_P_H
#define CTKEVENTADMINSERVICE_P_H

#include <QObject>

#include <service/event/ctkEventAdmin.h>

#include "ctkEventAdminImpl_p.h"

#include "handler/ctkEACleanBlackList_p.h"
#include "util/ctkEALeastRecentlyUsedCacheMap_p.h"
#include "handler/ctkEACacheTopicHandlerFilters_p.h"
#include "handler/ctkEACacheFilters_p.h"
#include "tasks/ctkEASyncDeliverTasks_p.h"
#include "tasks/ctkEAAsyncDeliverTasks_p.h"
#include "dispatch/ctkEASignalPublisher_p.h"

class ctkEASlotHandler;

class ctkEventAdminService : public QObject, public ctkEventAdmin
{
  Q_OBJECT
  Q_INTERFACES(ctkEventAdmin)

public:

  typedef ctkEACleanBlackList BlackList;
  typedef ctkEABlackList<BlackList> BlackListInterface;

  typedef ctkEALeastRecentlyUsedCacheMap<QString, QString> TopicCacheMap;
  typedef ctkEACacheTopicHandlerFilters<TopicCacheMap> TopicHandlerFilters;
  typedef ctkEATopicHandlerFilters<TopicHandlerFilters> TopicHandlerFiltersInterface;
  typedef ctkEALeastRecentlyUsedCacheMap<QString, ctkLDAPSearchFilter> LDAPCacheMap;
  typedef ctkEACacheFilters<LDAPCacheMap> Filters;
  typedef ctkEAFilters<Filters> FiltersInterface;

  typedef ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters> BlacklistingHandlerTasks;
  typedef ctkEAHandlerTasks<BlacklistingHandlerTasks> HandlerTasksInterface;

  typedef ctkEAHandlerTask<BlacklistingHandlerTasks> HandlerTask;
  typedef HandlerTask HandlerTaskInterface;
  typedef ctkEASyncDeliverTasks<HandlerTask> SyncDeliverTasks;
  typedef ctkEADeliverTask<SyncDeliverTasks, HandlerTask> SyncDeliverTasksInterface;
  typedef ctkEAAsyncDeliverTasks<SyncDeliverTasks, HandlerTask> AsyncDeliverTasks;
  typedef ctkEADeliverTask<AsyncDeliverTasks, HandlerTask> AsyncDeliverTasksInterface;

private:

  ctkEventAdminImpl<BlacklistingHandlerTasks, SyncDeliverTasks, AsyncDeliverTasks> impl;

  ctkPluginContext* context;
  QHash<const QObject*, QList<ctkEASignalPublisher*> > signalPublisher;
  QHash<qlonglong, ctkEASlotHandler*> slotHandler;

public:
  ctkEventAdminService(ctkPluginContext* context,
                       HandlerTasksInterface* managers,
                       ctkEADefaultThreadPool* syncPool,
                       ctkEADefaultThreadPool* asyncPool,
                       int timeout,
                       const QStringList& ignoreTimeout);

  ~ctkEventAdminService();

  void postEvent(const ctkEvent& event);

  void sendEvent(const ctkEvent& event);

  void publishSignal(const QObject* publisher, const char* signal,
                     const QString& topic,
                     Qt::ConnectionType type = Qt::QueuedConnection);

  void unpublishSignal(const QObject* publisher, const char* signal = 0,
                       const QString& topic = "");

  qlonglong subscribeSlot(const QObject* subscriber, const char* member,
                          const ctkDictionary& properties, Qt::ConnectionType type = Qt::AutoConnection);

  void unsubscribeSlot(qlonglong subscriptionId);

  bool updateProperties(qlonglong subscriptionId, const ctkDictionary& properties);

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

};

#endif // CTKEVENTADMINSERVICE_P_H
