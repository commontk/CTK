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

#include <QString>
#include <QDebug>

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

QDebug operator<<(QDebug dbg, ctkPluginFrameworkEvent::Type type)
{
  switch (type)
  {
  case ctkPluginFrameworkEvent::STARTED:        return dbg << "STARTED";
  case ctkPluginFrameworkEvent::ERROR:          return dbg << "ERROR";
  case ctkPluginFrameworkEvent::WARNING:        return dbg << "WARNING";
  case ctkPluginFrameworkEvent::INFO:           return dbg << "INFO";
  case ctkPluginFrameworkEvent::STOPPED:        return dbg << "STOPPED";
  case ctkPluginFrameworkEvent::STOPPED_UPDATE: return dbg << "STOPPED_UPDATE";
  case ctkPluginFrameworkEvent::WAIT_TIMEDOUT:  return dbg << "WATI_TIMEDOUT";

  default: return dbg << "unknown plugin framework event type (" << static_cast<int>(type) << ")";
  }
}

QDebug operator<<(QDebug dbg, const ctkPluginFrameworkEvent& event)
{
  if (event.isNull()) return dbg << "NONE";

  ctkPlugin* p = event.getPlugin();
  QString err = event.getErrorString();

  dbg.nospace() << event.getType() << " #" << p->getPluginId() << " ("
      << p->getLocation() << ")" << (err.isEmpty() ? "" : " exception: ") << err;
  return dbg.maybeSpace();
}
