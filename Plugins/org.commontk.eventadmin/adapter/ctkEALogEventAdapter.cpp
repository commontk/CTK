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


#include "ctkEALogEventAdapter_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>
#include <service/event/ctkEventConstants.h>
#include <service/log/ctkLogService.h>

_LogListener::_LogListener(ctkEALogEventAdapter* adapter)
  : adapter(adapter)
{

}

void _LogListener::logged(ctkLogEntryPtr entry)
{
  // This is where the assembly as specified in 133.6.6 OSGi R4
  // compendium is taking place (i.e., the log entry is adapted to
  // an event and posted via the ctkEventAdmin)

  ctkDictionary properties;

  QSharedPointer<ctkPlugin> plugin = entry->getPlugin();
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

  properties.insert("log.level", entry->getLevel());
  properties.insert(ctkEventConstants::MESSAGE, entry->getMessage());
  properties.insert(ctkEventConstants::TIMESTAMP, entry->getTime());
  properties.insert("log.entry", QVariant::fromValue(entry));

  const std::exception* exc = entry->getException();
  if (exc)
  {
    //properties.insert(ctkEventConstants::EXCEPTION_CLASS,
    //        exception.getClass().getName());

    const QString message(exc->what());
    if (!message.isEmpty())
    {
      properties.insert(ctkEventConstants::EXCEPTION_MESSAGE,
                        message);
    }

    properties.insert(ctkEventConstants::EXCEPTION, exc);
  }

  ctkServiceReference service = entry->getServiceReference();
  if (service)
  {
    properties.insert(ctkEventConstants::SERVICE, QVariant::fromValue(service));

    QVariant id = service.getProperty(ctkEventConstants::SERVICE_ID);

    if (id.isValid())
    {
      properties.insert(ctkEventConstants::SERVICE_ID, id);
      QVariant pid = service.getProperty(ctkEventConstants::SERVICE_PID);
      if (pid.isValid())
      {
        properties.insert(ctkEventConstants::SERVICE_PID, pid);
      }

      QVariant objectClass = service.getProperty(ctkPluginConstants::OBJECTCLASS);
      if (objectClass.isValid())
      {
        properties.insert(ctkEventConstants::SERVICE_OBJECTCLASS, objectClass);
      }
    }
  }

  QString topic("org/commontk/service/log/LogEntry/");

  int level = entry->getLevel();
  if (level == ctkLogService::LOG_ERROR)
  {
    topic.append("LOG_ERROR");
  }
  else if (level == ctkLogService::LOG_WARNING)
  {
    topic.append("LOG_WARNING");
  }
  else if (level == ctkLogService::LOG_INFO)
  {
    topic.append("LOG_INFO");
  }
  else if (level == ctkLogService::LOG_DEBUG)
  {
    topic.append("LOG_DEBUG");
  }
  else
  {
    topic.append("LOG_OTHER");
  }

  try
  {
    adapter->getEventAdmin()->postEvent(ctkEvent(topic, properties));
  }
  catch(const ctkIllegalStateException&)
  {
    // This is o.k. - indicates that we are stopped.
  }
}

ctkEALogEventAdapter::ctkEALogEventAdapter(ctkPluginContext* context, ctkEventAdmin* admin)
  : ctkEAAbstractAdapter(admin), logListener(new _LogListener(this))
{
  reg = context->registerService<ctkLogListener>(logListener);
}

ctkEALogEventAdapter::~ctkEALogEventAdapter()
{
  delete logListener;
}

void ctkEALogEventAdapter::destroy(ctkPluginContext* context)
{
  Q_UNUSED(context)

  reg.unregister();
}
