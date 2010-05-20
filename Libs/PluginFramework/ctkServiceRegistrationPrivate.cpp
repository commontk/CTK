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

#include "ctkServiceRegistrationPrivate.h"


  ServiceRegistrationPrivate::ServiceRegistrationPrivate(ServiceRegistration* sr,
                                                         PluginPrivate* plugin, QObject* service,
                                                         const ServiceProperties& props)
                               : q_ptr(sr), plugin(plugin), service(service), reference(new ServiceReference(this)),
                               properties(props), available(true), unregistering(false)
  {

  }

  ServiceRegistrationPrivate::~ServiceRegistrationPrivate()
  {
    delete reference;
  }

  bool ServiceRegistrationPrivate::isUsedByPlugin(Plugin* p)
  {
    QHash<Plugin*, int> deps = dependents;
    return deps.contains(p);
  }

  QObject* ServiceRegistrationPrivate::getService()
  {
    return service;
  }


