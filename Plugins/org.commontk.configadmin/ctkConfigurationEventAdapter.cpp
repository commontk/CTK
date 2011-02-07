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

#include "ctkConfigurationEventAdapter_p.h"

#include <service/event/ctkEvent.h>

const QString ctkConfigurationEventAdapter::TOPIC = "org/commontk/service/cm/ConfigurationEvent";
const QChar ctkConfigurationEventAdapter::TOPIC_SEPARATOR = '/';

const QString ctkConfigurationEventAdapter::CM_UPDATED = "CM_UPDATED";
const QString ctkConfigurationEventAdapter::CM_DELETED = "CM_DELETED";

const QString ctkConfigurationEventAdapter::CM_FACTORY_PID = "cm.factoryPid";
const QString ctkConfigurationEventAdapter::CM_PID = "cm.pid";
const QString ctkConfigurationEventAdapter::SERVICE = "service";
const QString ctkConfigurationEventAdapter::SERVICE_ID = "service.id";
const QString ctkConfigurationEventAdapter::SERVICE_OBJECTCLASS = "service.objectClass";
const QString ctkConfigurationEventAdapter::SERVICE_PID = "service.pid";


ctkConfigurationEventAdapter::ctkConfigurationEventAdapter(ctkPluginContext* context)
  : context(context), eventAdminTracker(context)
{

}

void ctkConfigurationEventAdapter::start()
{
  eventAdminTracker.open();
  configListenerRegistration = context->registerService<ctkConfigurationListener>(this);
}

void ctkConfigurationEventAdapter::stop()
{
  configListenerRegistration.unregister();
  configListenerRegistration = 0;
  eventAdminTracker.close();
}

void ctkConfigurationEventAdapter::configurationEvent(const ctkConfigurationEvent& event)
{
  ctkEventAdmin* eventAdmin = eventAdminTracker.getService();
  if (eventAdmin == 0)
  {
    return;
  }
  QString typeName;
  switch (event.getType())
  {
  case ctkConfigurationEvent::CM_UPDATED :
    typeName = CM_UPDATED;
    break;
  case ctkConfigurationEvent::CM_DELETED :
    typeName = CM_DELETED;
    break;
  default : // do nothing
    return;
  }
  QString topic = TOPIC + TOPIC_SEPARATOR + typeName;
  ctkServiceReference ref = event.getReference();
  if (!ref)
  {
    throw ctkRuntimeException("ctkConfigurationEvent::getServiceReference() is null");
  }
  ctkDictionary properties;
  properties.insert(CM_PID, event.getPid());
  if (event.getFactoryPid().isNull())
  {
    properties.insert(CM_FACTORY_PID, event.getFactoryPid());
  }
  putServiceReferenceProperties(properties, ref);
  ctkEvent convertedEvent(topic, properties);
  eventAdmin->postEvent(convertedEvent);
}

void ctkConfigurationEventAdapter::putServiceReferenceProperties(ctkDictionary& properties, const ctkServiceReference& ref)
{
  properties.insert(SERVICE, QVariant::fromValue(ref));
  properties.insert(SERVICE_ID, ref.getProperty(ctkPluginConstants::SERVICE_ID));
  QVariant o = ref.getProperty(ctkPluginConstants::SERVICE_PID);
  if (o.canConvert<QString>())
  {
    properties.insert(SERVICE_PID, o);
  }
  QVariant o2 = ref.getProperty(ctkPluginConstants::OBJECTCLASS);
  if (o.canConvert<QStringList>())
  {
    properties.insert(SERVICE_OBJECTCLASS, o2);
  }
}
