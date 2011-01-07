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
    context(context), signalPublisher(this)
{

}

ctkEventAdminService::~ctkEventAdminService()
{
  qDeleteAll(slotHandler);
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
                                         Qt::ConnectionType type)
{
  if (type == Qt::DirectConnection)
  {
    connect(publisher, signal, &signalPublisher, SLOT(publishSyncSignal(ctkEvent)), Qt::DirectConnection);
  }
  else if (type == Qt::QueuedConnection)
  {
    connect(publisher, signal, &signalPublisher, SLOT(publishAsyncSignal(ctkEvent)), Qt::DirectConnection);
  }
  else
  {
    throw std::invalid_argument("type must be either Qt::DirectConnection or Qt::QueuedConnection");
  }
}

qlonglong ctkEventAdminService::subscribeSlot(const QObject* subscriber, const char* member, const ctkDictionary& properties)
{
  ctkEASlotHandler* handler = new ctkEASlotHandler();
  connect(handler, SIGNAL(eventOccured(ctkEvent)), subscriber, member, Qt::DirectConnection);
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

