/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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


ctkServiceEvent::ctkServiceEvent()
  : d(0)
{

}

ctkServiceEvent::~ctkServiceEvent()
{

}

ctkServiceEvent::ctkServiceEvent(Type type, const ctkServiceReference& reference)
  : d(new ctkServiceEventData(type, reference))
{

}

ctkServiceEvent::ctkServiceEvent(const ctkServiceEvent& other)
  : d(other.d)
{

}

ctkServiceReference ctkServiceEvent::getServiceReference() const
{
  return d->reference;
}

ctkServiceEvent::Type ctkServiceEvent::getType() const
{
  return d->type;
}

QDebug operator<<(QDebug dbg, ctkServiceEvent::Type type)
{
  switch(type)
  {
  case ctkServiceEvent::MODIFIED:
    dbg.nospace() << "MODIFIED";
    break;
  case ctkServiceEvent::MODIFIED_ENDMATCH:
    dbg.nospace() << "MODIFIED_ENDMATCH";
    break;
  case ctkServiceEvent::REGISTERED:
    dbg.nospace() << "REGISTERED";
    break;
  case ctkServiceEvent::UNREGISTERING:
    dbg.nospace() << "UNREGISTERING";
    break;
  default:
    dbg.nospace() << "unknown event type " << static_cast<int>(type);
  }

  return dbg.maybeSpace();
}
