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


#include "ctkEAPluginEventAdapter_p.h"

#include <ctkPluginContext.h>
#include <service/event/ctkEventConstants.h>
#include <service/event/ctkEvent.h>

ctkEAPluginEventAdapter::ctkEAPluginEventAdapter(ctkPluginContext* context, ctkEventAdmin* admin)
  : ctkEAAbstractAdapter(admin)
{
  context->connectPluginListener(this, SLOT(pluginChanged(ctkPluginEvent)));
}

void ctkEAPluginEventAdapter::destroy(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

void ctkEAPluginEventAdapter::pluginChanged(const ctkPluginEvent& event)
{
  ctkDictionary properties;
  properties.insert(ctkEventConstants::EVENT, QVariant::fromValue(event));
  properties.insert("plugin.id", QVariant::fromValue<long>(event.getPlugin()->getPluginId()));

  const QString symbolicName = event.getPlugin()->getSymbolicName();

  if (!symbolicName.isEmpty())
  {
    properties.insert(ctkEventConstants::PLUGIN_SYMBOLICNAME,
                      symbolicName);
  }

  properties.insert("plugin", QVariant::fromValue(event.getPlugin()));

  QString topic("org/commontk/PluginEvent/");

  switch (event.getType())
  {
  case ctkPluginEvent::INSTALLED:
    topic.append("INSTALLED");
    break;
  case ctkPluginEvent::STARTED:
    topic.append("STARTED");
    break;
  case ctkPluginEvent::STOPPED:
    topic.append("STOPPED");
    break;
  case ctkPluginEvent::UPDATED:
    topic.append("UPDATED");
    break;
  case ctkPluginEvent::UNINSTALLED:
    topic.append("UNINSTALLED");
    break;
  case ctkPluginEvent::RESOLVED:
    topic.append("RESOLVED");
    break;
  case ctkPluginEvent::UNRESOLVED:
    topic.append("UNRESOLVED");
    break;
  default:
    return; // IGNORE EVENT
  };

  try
  {
    getEventAdmin()->postEvent(ctkEvent(topic, properties));
  }
  catch (const ctkIllegalStateException& )
  {
    // This is o.k. - indicates that we are stopped.
  }
}
