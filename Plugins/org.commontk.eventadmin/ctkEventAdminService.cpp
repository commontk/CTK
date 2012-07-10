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


#include "ctkEventAdminService_p.h"

#include "handler/ctkEASlotHandler_p.h"

#include <ctkPluginConstants.h>

ctkEventAdminService::ctkEventAdminService(ctkPluginContext* context,
                                           HandlerTasksInterface* managers,
                                           ctkEADefaultThreadPool* syncPool,
                                           ctkEADefaultThreadPool* asyncPool,
                                           int timeout,
                                           const QStringList& ignoreTimeout)
  : impl(managers, syncPool, asyncPool, timeout, ignoreTimeout),
    context(context)
{

}

ctkEventAdminService::~ctkEventAdminService()
{
  qDeleteAll(slotHandler);
  foreach(QList<ctkEASignalPublisher*> l, signalPublisher.values())
  {
    qDeleteAll(l);
  }
}

void ctkEventAdminService::postEvent(const ctkEvent& event)
{
  impl.postEvent(event);
}

void ctkEventAdminService::sendEvent(const ctkEvent& event)
{
  impl.sendEvent(event);
}

void ctkEventAdminService::publishSignal(const QObject* publisher, const char* signal,
                                         const QString& topic,
                                         Qt::ConnectionType type)
{
  if (topic.isEmpty())
  {
    throw ctkInvalidArgumentException("topic must not be empty");
  }

  // check if the signal was already registered under the given topic
  if (signalPublisher.contains(publisher))
  {
    const QList<ctkEASignalPublisher*>& signalPublishers = signalPublisher[publisher];
    for(int i = 0; i < signalPublishers.size(); ++i)
    {
      if (signalPublishers[i]->getSignalName() == signal &&
          signalPublishers[i]->getTopicName() == topic)
      {
        return;
      }
    }
  }

  QList<ctkEASignalPublisher*>& signalList = signalPublisher[publisher];
  signalList.push_back(new ctkEASignalPublisher(this, signal, topic));
  if (type == Qt::DirectConnection)
  {
    connect(publisher, signal, signalList.back(), SLOT(publishSyncSignal(ctkDictionary)), Qt::DirectConnection);
  }
  else if (type == Qt::QueuedConnection)
  {
    connect(publisher, signal, signalList.back(), SLOT(publishAsyncSignal(ctkDictionary)), Qt::DirectConnection);
  }
  else
  {
    throw ctkInvalidArgumentException("type must be either Qt::DirectConnection or Qt::QueuedConnection");
  }
}

void ctkEventAdminService::unpublishSignal(const QObject* publisher, const char* signal,
                                           const QString& topic)
{
  if (!signalPublisher.contains(publisher)) return;

  if (signal == 0 && topic.isEmpty())
  {
    // unpublish everything from the given publisher
    // this automatically disconnects signals
    qDeleteAll(signalPublisher.take(publisher));
  }
  else
  {
    QList<ctkEASignalPublisher*>& list = signalPublisher[publisher];
    if (signal == 0)
    {
      for (int i = 0; i < list.size(); )
      {
        if (list[i]->getTopicName() == topic)
        {
          // this automatically disconnects the signals
          delete list.takeAt(i);
        }
        else
        {
          ++i;
        }
      }
    }
    else {
      for (int i = 0; i < list.size(); )
      {
        if (list[i]->getSignalName() == signal)
        {
          if (topic.isEmpty() || list[i]->getTopicName() == topic)
          {
            // this automatically disconnects the signals
            delete list.takeAt(i);
          }
        }
        else
        {
          ++i;
        }
      }
    }

    if (list.isEmpty())
    {
      signalPublisher.remove(publisher);
    }
  }
}

qlonglong ctkEventAdminService::subscribeSlot(const QObject* subscriber, const char* member,
                                              const ctkDictionary& properties, Qt::ConnectionType type)
{
  if (subscriber == 0) throw ctkInvalidArgumentException("subscriber cannot be NULL");
  if (member == 0) throw ctkInvalidArgumentException("slot cannot be NULL");
  if (type != Qt::AutoConnection && type != Qt::DirectConnection &&
      type != Qt::QueuedConnection && type != Qt::BlockingQueuedConnection)
  {
    throw ctkInvalidArgumentException("connection type invalid");
  }

  ctkEASlotHandler* handler = new ctkEASlotHandler();
  connect(handler, SIGNAL(eventOccured(ctkEvent)), subscriber, member, type);
  ctkServiceRegistration reg = context->registerService<ctkEventHandler>(handler, properties);
  handler->reg = reg;
  qlonglong id = reg.getReference().getProperty(ctkPluginConstants::SERVICE_ID).toLongLong();
  slotHandler.insert(id, handler);
  return id;
}

void ctkEventAdminService::unsubscribeSlot(qlonglong subscriptionId)
{
  ctkEASlotHandler* handler = slotHandler.take(subscriptionId);
  if (handler)
  {
    handler->reg.unregister();
    delete handler;
  }
}

bool ctkEventAdminService::updateProperties(qlonglong subscriptionId, const ctkDictionary& properties)
{
  ctkEASlotHandler* handler = slotHandler.value(subscriptionId);
  if (handler)
  {
    handler->updateProperties(properties);
    return true;
  }
  return false;
}

void ctkEventAdminService::stop()
{
  impl.stop();
}

void ctkEventAdminService::update(HandlerTasksInterface* managers, int timeout,
                                  const QStringList& ignoreTimeout)
{
  impl.update(managers, timeout, ignoreTimeout);
}

