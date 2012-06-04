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


#include "ctkEAServiceEventAdapter_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>
#include <ctkServiceEvent.h>
#include <service/event/ctkEventConstants.h>

ctkEAServiceEventAdapter::ctkEAServiceEventAdapter(ctkPluginContext* context, ctkEventAdmin* admin)
  : ctkEAAbstractAdapter(admin)
{
  context->connectServiceListener(this, "serviceChanged");
}

void ctkEAServiceEventAdapter::destroy(ctkPluginContext* context)
{
  context->disconnectServiceListener(this, "serviceChanged");
}

void ctkEAServiceEventAdapter::serviceChanged(const ctkServiceEvent& event)
{
  ctkDictionary properties;

  properties.insert(ctkEventConstants::EVENT, QVariant::fromValue(event));
  properties.insert(ctkEventConstants::SERVICE,
                    QVariant::fromValue(event.getServiceReference()));

  QVariant id = event.getServiceReference().getProperty(
        ctkEventConstants::SERVICE_ID);
  if (id.isValid())
  {
    properties.insert(ctkEventConstants::SERVICE_ID, id);
  }

  QVariant pid = event.getServiceReference().getProperty(
        ctkEventConstants::SERVICE_PID);
  if (pid.isValid())
  {
    properties.insert(ctkEventConstants::SERVICE_PID, pid);
  }

  QVariant objectClass = event.getServiceReference()
      .getProperty(ctkPluginConstants::OBJECTCLASS);

  if (objectClass.isValid())
  {
    properties.insert(ctkEventConstants::SERVICE_OBJECTCLASS,
                      objectClass);
  }

  QString topic("org/commontk/ServiceEvent/");

  switch (event.getType())
  {
  case ctkServiceEvent::REGISTERED:
    topic.append("REGISTERED");
    break;
  case ctkServiceEvent::MODIFIED:
    topic.append("MODIFIED");
    break;
  case ctkServiceEvent::UNREGISTERING:
    topic.append("UNREGISTERING");
    break;
  default:
    return; // IGNORE
  }

  try
  {
    getEventAdmin()->postEvent(ctkEvent(topic, properties));
  }
  catch(const ctkIllegalStateException& )
  {
    // This is o.k. - indicates that we are stopped.
  }
}
