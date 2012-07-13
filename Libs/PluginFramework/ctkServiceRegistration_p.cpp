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

#include "ctkServiceRegistration_p.h"

//----------------------------------------------------------------------------
ctkServiceRegistrationPrivate::ctkServiceRegistrationPrivate(
  ctkPluginPrivate* plugin, QObject* service,
  const ctkDictionary& props)
  : ref(1), service(service), plugin(plugin), reference(this),
    properties(props), available(true), unregistering(false),
    propsLock()
{

}

//----------------------------------------------------------------------------
ctkServiceRegistrationPrivate::~ctkServiceRegistrationPrivate()
{

}

//----------------------------------------------------------------------------
bool ctkServiceRegistrationPrivate::isUsedByPlugin(QSharedPointer<ctkPlugin> p)
{
  QHash<QSharedPointer<ctkPlugin>, int> deps = dependents;
  return deps.contains(p);
}

//----------------------------------------------------------------------------
QObject* ctkServiceRegistrationPrivate::getService()
{
  return service;
}
