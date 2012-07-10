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

#include "ctkPlugin.h"

#include <ctkException.h>

#include <QString>
#include <QDebug>

class ctkPluginFrameworkEventData : public QSharedData
{
public:

  ctkPluginFrameworkEventData(ctkPluginFrameworkEvent::Type type, QSharedPointer<ctkPlugin> plugin, const QString& exc)
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
  const QSharedPointer<ctkPlugin>	plugin;

  /**
   * Exception related to the event.
   */
  const QString errorString;

  /**
   * Type of event.
   */
  const ctkPluginFrameworkEvent::Type type;
};

//----------------------------------------------------------------------------
ctkPluginFrameworkEvent::ctkPluginFrameworkEvent()
  : d(0)
{

}

//----------------------------------------------------------------------------
ctkPluginFrameworkEvent::~ctkPluginFrameworkEvent()
{

}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkEvent::isNull() const
{
  return !d;
}

//----------------------------------------------------------------------------
ctkPluginFrameworkEvent::ctkPluginFrameworkEvent(Type type, QSharedPointer<ctkPlugin> plugin, const ctkException &fwException)
  : d(new ctkPluginFrameworkEventData(type, plugin, fwException.what()))
{

}

//----------------------------------------------------------------------------
ctkPluginFrameworkEvent::ctkPluginFrameworkEvent(Type type, QSharedPointer<ctkPlugin> plugin)
  : d(new ctkPluginFrameworkEventData(type, plugin, QString()))
{

}

//----------------------------------------------------------------------------
ctkPluginFrameworkEvent::ctkPluginFrameworkEvent(const ctkPluginFrameworkEvent& other)
  : d(other.d)
{

}

//----------------------------------------------------------------------------
ctkPluginFrameworkEvent& ctkPluginFrameworkEvent::operator=(const ctkPluginFrameworkEvent& other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
QString ctkPluginFrameworkEvent::getErrorString() const
{
  return d->errorString;
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPluginFrameworkEvent::getPlugin() const
{
  return d->plugin;
}

//----------------------------------------------------------------------------
ctkPluginFrameworkEvent::Type ctkPluginFrameworkEvent::getType() const
{
  return d->type;
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, ctkPluginFrameworkEvent::Type type)
{
  switch (type)
  {
  case ctkPluginFrameworkEvent::FRAMEWORK_STARTED:        return dbg << "FRAMEWORK_STARTED";
  case ctkPluginFrameworkEvent::PLUGIN_ERROR:          return dbg << "PLUGIN_ERROR";
  case ctkPluginFrameworkEvent::PLUGIN_WARNING:        return dbg << "PLUGIN_WARNING";
  case ctkPluginFrameworkEvent::PLUGIN_INFO:           return dbg << "PLUGIN_INFO";
  case ctkPluginFrameworkEvent::FRAMEWORK_STOPPED:        return dbg << "FRAMEWORK_STOPPED";
  case ctkPluginFrameworkEvent::FRAMEWORK_STOPPED_UPDATE: return dbg << "FRAMEWORK_STOPPED_UPDATE";
  case ctkPluginFrameworkEvent::FRAMEWORK_WAIT_TIMEDOUT:  return dbg << "FRAMEWORK_WATI_TIMEDOUT";

  default: return dbg << "unknown plugin framework event type (" << static_cast<int>(type) << ")";
  }
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkPluginFrameworkEvent& event)
{
  if (event.isNull()) return dbg << "NONE";

  ctkPlugin* p = event.getPlugin().data();
  QString err = event.getErrorString();

  dbg.nospace() << event.getType() << " #" << p->getPluginId() << " ("
      << p->getLocation() << ")" << (err.isEmpty() ? "" : " exception: ") << err;
  return dbg.maybeSpace();
}
