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

#include "ctkPlugin.h"

#include <QDebug>

class ctkPluginEventData : public QSharedData
{
public:

  ctkPluginEventData(ctkPluginEvent::Type type, QSharedPointer<ctkPlugin> plugin)
    : type(type), plugin(plugin)
  {

  }

  ctkPluginEventData(const ctkPluginEventData& other)
    : QSharedData(other), type(other.type), plugin(other.plugin)
  {

  }

  const ctkPluginEvent::Type type;
  const QSharedPointer<ctkPlugin> plugin;
};

//----------------------------------------------------------------------------
ctkPluginEvent::ctkPluginEvent()
  : d(0)
{

}

//----------------------------------------------------------------------------
ctkPluginEvent::~ctkPluginEvent()
{

}

//----------------------------------------------------------------------------
bool ctkPluginEvent::isNull() const
{
  return !d;
}

//----------------------------------------------------------------------------
ctkPluginEvent::ctkPluginEvent(Type type, QSharedPointer<ctkPlugin> plugin)
  : d(new ctkPluginEventData(type, plugin))
{

}

//----------------------------------------------------------------------------
ctkPluginEvent::ctkPluginEvent(const ctkPluginEvent& other)
  : d(other.d)
{

}

//----------------------------------------------------------------------------
ctkPluginEvent& ctkPluginEvent::operator=(const ctkPluginEvent& other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPluginEvent::getPlugin() const
{
  return d->plugin;
}

//----------------------------------------------------------------------------
ctkPluginEvent::Type ctkPluginEvent::getType() const
{
  return d->type;
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug debug, ctkPluginEvent::Type eventType)
{
  switch (eventType)
  {
  case ctkPluginEvent::INSTALLED:       return debug << "INSTALLED";
  case ctkPluginEvent::STARTED:         return debug << "STARTED";
  case ctkPluginEvent::STOPPED:         return debug << "STOPPED";
  case ctkPluginEvent::UPDATED:         return debug << "UPDATED";
  case ctkPluginEvent::UNINSTALLED:     return debug << "UNINSTALLED";
  case ctkPluginEvent::RESOLVED:        return debug << "RESOLVED";
  case ctkPluginEvent::UNRESOLVED:      return debug << "UNRESOLVED";
  case ctkPluginEvent::STARTING:        return debug << "STARTING";
  case ctkPluginEvent::STOPPING:        return debug << "STOPPING";
  case ctkPluginEvent::LAZY_ACTIVATION: return debug << "LAZY_ACTIVATION";

  default: return debug << "Unknown plugin event type (" << static_cast<int>(eventType) << ")";
  }
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug debug, const ctkPluginEvent& event)
{
  if (event.isNull()) return debug << "NONE";

  QSharedPointer<ctkPlugin> p = event.getPlugin();
  debug.nospace() << event.getType() << " #" << p->getPluginId() << " (" << p->getLocation() << ")";
  return debug.maybeSpace();
}
