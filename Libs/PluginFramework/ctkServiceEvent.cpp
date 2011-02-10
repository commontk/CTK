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

#include "ctkServiceEvent.h"

#include "ctkServiceReference.h"
#include "ctkPluginConstants.h"

#include <QStringList>

//----------------------------------------------------------------------------
class ctkServiceEventData : public QSharedData
{
public:

  ctkServiceEventData(ctkServiceEvent::Type type, const ctkServiceReference& reference)
    : type(type), reference(reference)
  {

  }

  ctkServiceEventData(const ctkServiceEventData& other)
    : QSharedData(other), type(other.type), reference(other.reference)
  {

  }

  const ctkServiceEvent::Type type;
  const ctkServiceReference reference;
};

//----------------------------------------------------------------------------
ctkServiceEvent::ctkServiceEvent()
  : d(0)
{

}

//----------------------------------------------------------------------------
ctkServiceEvent::~ctkServiceEvent()
{

}

//----------------------------------------------------------------------------
bool ctkServiceEvent::isNull() const
{
  return !d;
}

//----------------------------------------------------------------------------
ctkServiceEvent::ctkServiceEvent(Type type, const ctkServiceReference& reference)
  : d(new ctkServiceEventData(type, reference))
{

}

//----------------------------------------------------------------------------
ctkServiceEvent::ctkServiceEvent(const ctkServiceEvent& other)
  : d(other.d)
{

}

//----------------------------------------------------------------------------
ctkServiceEvent& ctkServiceEvent::operator=(const ctkServiceEvent& other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
ctkServiceReference ctkServiceEvent::getServiceReference() const
{
  return d->reference;
}

//----------------------------------------------------------------------------
ctkServiceEvent::Type ctkServiceEvent::getType() const
{
  return d->type;
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, ctkServiceEvent::Type type)
{
  switch(type)
  {
  case ctkServiceEvent::MODIFIED:          return dbg << "MODIFIED";
  case ctkServiceEvent::MODIFIED_ENDMATCH: return dbg << "MODIFIED_ENDMATCH";
  case ctkServiceEvent::REGISTERED:        return dbg << "REGISTERED";
  case ctkServiceEvent::UNREGISTERING:     return dbg << "UNREGISTERING";

  default: return dbg << "unknown service event type (" << static_cast<int>(type) << ")";
  }
}

//----------------------------------------------------------------------------
QDebug operator<<(QDebug dbg, const ctkServiceEvent& event)
{
  if (event.isNull()) return dbg << "NONE";

  ctkServiceReference sr = event.getServiceReference();
  // Some events will not have a service reference
  qlonglong sid = sr.getProperty(ctkPluginConstants::SERVICE_ID).toLongLong();
  QStringList classes = sr.getProperty(ctkPluginConstants::OBJECTCLASS).toStringList();

  dbg.nospace() << event.getType() << " " << sid << " objectClass=" << classes;
  return dbg.maybeSpace();
}
