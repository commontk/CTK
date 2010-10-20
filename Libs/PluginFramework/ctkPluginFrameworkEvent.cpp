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

#include "ctkPluginFrameworkEvent.h"

#include <QString>

class ctkPluginFrameworkEventData : public QSharedData
{
public:

  ctkPluginFrameworkEventData(ctkPluginFrameworkEvent::Type type, ctkPlugin* plugin, const QString& exc)
    : plugin(plugin), errorString(exc), type(type)
  {

  }

  ctkPluginFrameworkEventData(const ctkPluginFrameworkEventData& other)
    : QSharedData(other), plugin(other.plugin), errorString(other.errorString),
      type(other.type)
  {

  }

  /**
   * Plugin related to the event.
   */
  ctkPlugin* const	plugin;

  /**
   * Exception related to the event.
   */
  const QString errorString;

  /**
   * Type of event.
   */
  const ctkPluginFrameworkEvent::Type type;
};


ctkPluginFrameworkEvent::ctkPluginFrameworkEvent()
  : d(0)
{

}

ctkPluginFrameworkEvent::~ctkPluginFrameworkEvent()
{

}

bool ctkPluginFrameworkEvent::isNull() const
{
  return !d;
}

ctkPluginFrameworkEvent::ctkPluginFrameworkEvent(Type type, ctkPlugin* plugin, const std::exception& fwException)
  : d(new ctkPluginFrameworkEventData(type, plugin, fwException.what()))
{

}

ctkPluginFrameworkEvent::ctkPluginFrameworkEvent(Type type, ctkPlugin* plugin)
  : d(new ctkPluginFrameworkEventData(type, plugin, QString()))
{

}

ctkPluginFrameworkEvent::ctkPluginFrameworkEvent(const ctkPluginFrameworkEvent& other)
  : d(other.d)
{

}

ctkPluginFrameworkEvent& ctkPluginFrameworkEvent::operator=(const ctkPluginFrameworkEvent& other)
{
  d = other.d;
  return *this;
}

QString ctkPluginFrameworkEvent::getErrorString() const
{
  return d->errorString;
}

ctkPlugin* ctkPluginFrameworkEvent::getPlugin() const
{
  return d->plugin;
}

ctkPluginFrameworkEvent::Type ctkPluginFrameworkEvent::getType() const
{
  return d->type;
}
