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


#include "ctkEAFrameworkEventAdapter_p.h"

#include <ctkPluginContext.h>
#include <service/event/ctkEventConstants.h>

ctkEAFrameworkEventAdapter::ctkEAFrameworkEventAdapter(ctkPluginContext* context, ctkEventAdmin* admin)
  : ctkEAAbstractAdapter(admin)
{
  context->connectFrameworkListener(this, SLOT(frameworkEvent(ctkPluginFrameworkEvent)));
}

void ctkEAFrameworkEventAdapter::destroy(ctkPluginContext* context)
{
  Q_UNUSED(context)
  //context->removeFrameworkListener(this);
}

void ctkEAFrameworkEventAdapter::frameworkEvent(const ctkPluginFrameworkEvent& event)
{
  ctkDictionary properties;

  properties.insert(ctkEventConstants::EVENT, QVariant::fromValue(event));

  QSharedPointer<ctkPlugin> plugin = event.getPlugin();

  if (plugin)
  {
    properties.insert("plugin.id", QVariant::fromValue<long>(plugin->getPluginId()));

    const QString symbolicName = plugin->getSymbolicName();

    if (!symbolicName.isEmpty())
    {
      properties.insert(ctkEventConstants::PLUGIN_SYMBOLICNAME,
                        symbolicName);
    }

    properties.insert("plugin", QVariant::fromValue(plugin));
  }

  const QString message = event.getErrorString();

  if (!message.isEmpty())
  {
//    properties.put(EventConstants.EXCEPTION_CLASS,
//                   thrown.getClass().getName());

//    final String message = thrown.getMessage();

    properties.insert(ctkEventConstants::EXCEPTION_MESSAGE,
                      message);

    //properties.put(EventConstants.EXCEPTION, thrown);
  }

  QString topic = "org/commontk/PluginFrameworkEvent/";

  switch (event.getType())
  {
  case ctkPluginFrameworkEvent::FRAMEWORK_STARTED:
    topic.append("STARTED");
    break;
  case ctkPluginFrameworkEvent::PLUGIN_ERROR:
    topic.append("ERROR");
    break;
//  case ctkPluginFrameworkEvent::STARTLEVEL_CHANGED:
//    topic.append("STARTLEVEL_CHANGED");
//    break;
  case ctkPluginFrameworkEvent::PLUGIN_WARNING:
    topic.append("WARNING");
    break;
  case ctkPluginFrameworkEvent::PLUGIN_INFO:
    topic.append("INFO");
    break;
  default:
    return; // IGNORE EVENT
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
