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


#include "ctkConfigurationEvent.h"

#include <ctkException.h>

class ctkConfigurationEventData : public QSharedData
{
public:

  ctkConfigurationEventData(const ctkServiceReference& reference,
                            ctkConfigurationEvent::Type type,
                            const QString& factoryPid,
                            const QString& pid)
    : type(type), factoryPid(factoryPid), pid(pid), reference(reference)
  {

  }

  ctkConfigurationEventData(const ctkConfigurationEventData& other)
    : QSharedData(other), type(other.type), factoryPid(other.factoryPid),
      pid(other.pid), reference(other.reference)
  {

  }

  /**
   * Type of this event.
   */
  const ctkConfigurationEvent::Type type;

  /**
   * The factory pid associated with this event.
   */
  const QString factoryPid;

  /**
   * The pid associated with this event.
   */
  const QString pid;

  /**
   * The ConfigurationAdmin service which created this event.
   */
  const ctkServiceReference reference;
};

//----------------------------------------------------------------------------
ctkConfigurationEvent::ctkConfigurationEvent()
  : d(0)
{

}

//----------------------------------------------------------------------------
ctkConfigurationEvent::~ctkConfigurationEvent()
{

}

//----------------------------------------------------------------------------
bool ctkConfigurationEvent::isNull() const
{
  return !d;
}

//----------------------------------------------------------------------------
ctkConfigurationEvent::ctkConfigurationEvent(const ctkServiceReference& reference,
                      Type type, const QString& factoryPid,
                      const QString& pid)
  : d(new ctkConfigurationEventData(reference, type, factoryPid, pid))
{
  if (pid.isNull())
  {
    throw ctkInvalidArgumentException("pid must not be null");
  }
}

//----------------------------------------------------------------------------
ctkConfigurationEvent::ctkConfigurationEvent(const ctkConfigurationEvent& other)
  : d(other.d)
{

}

//----------------------------------------------------------------------------
ctkConfigurationEvent& ctkConfigurationEvent::operator=(const ctkConfigurationEvent& other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
QString ctkConfigurationEvent::getFactoryPid() const
{
  return d->factoryPid;
}

//----------------------------------------------------------------------------
QString ctkConfigurationEvent::getPid() const
{
  return d->pid;
}

//----------------------------------------------------------------------------
int ctkConfigurationEvent::getType() const
{
  return d->type;
}

//----------------------------------------------------------------------------
ctkServiceReference ctkConfigurationEvent::getReference() const
{
  return d->reference;
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, ctkConfigurationEvent::Type type)
{
  switch (type)
  {
  case ctkConfigurationEvent::CM_UPDATED: return dbg << "CM_UPDATED";
  case ctkConfigurationEvent::CM_DELETED: return dbg << "CM_DELETED";

  default: return dbg << "unknown configuration event type (" << static_cast<int>(type) << ")";
  }
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkConfigurationEvent& event)
{
  if (event.isNull()) return dbg << "NONE";

  dbg.nospace() << event.getType() << (event.getFactoryPid().isEmpty() ? " " : event.getFactoryPid() + ", ")
                   << "pid=" << event.getPid();
  return dbg.maybeSpace();
}
