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

#include "ctkPluginEvent.h"

class ctkPluginEventData : public QSharedData
{
public:

  ctkPluginEventData(ctkPluginEvent::Type type, ctkPlugin* plugin)
    : type(type), plugin(plugin)
  {

  }

  ctkPluginEventData(const ctkPluginEventData& other)
    : QSharedData(other), type(other.type), plugin(other.plugin)
  {

  }

  const ctkPluginEvent::Type type;
  ctkPlugin *const plugin;
};


ctkPluginEvent::ctkPluginEvent()
  : d(0)
{

}

ctkPluginEvent::~ctkPluginEvent()
{

}

ctkPluginEvent::ctkPluginEvent(Type type, ctkPlugin* plugin)
  : d(new ctkPluginEventData(type, plugin))
{

}

ctkPluginEvent::ctkPluginEvent(const ctkPluginEvent& other)
  : d(other.d)
{

}

ctkPlugin* ctkPluginEvent::getPlugin() const
{
  return d->plugin;
}

ctkPluginEvent::Type ctkPluginEvent::getType() const
{
  return d->type;
}
