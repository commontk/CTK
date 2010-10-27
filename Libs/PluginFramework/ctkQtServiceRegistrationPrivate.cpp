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

#include "ctkQtServiceRegistrationPrivate.h"

#include "ctkPluginPrivate_p.h"
#include "ctkPluginFrameworkContext_p.h"

#include "ctkServices_p.h"
#include "ctkQtServiceRegistration_p.h"


ctkQtServiceRegistrationPrivate::ctkQtServiceRegistrationPrivate(ctkQtServiceRegistration* sr,
                             ctkPluginPrivate* plugin,
                             QtMobility::QServiceInterfaceDescriptor serviceDescriptor,
                             const ServiceProperties& props)
  : ctkServiceRegistrationPrivate(sr, plugin, 0, props),
    serviceDescriptor(serviceDescriptor)
{

}

QObject* ctkQtServiceRegistrationPrivate::getService()
{
  if (this->plugin->state != ctkPlugin::ACTIVE)
  {
    try
    {
      this->plugin->q_func()->start(0);
    }
    catch (const ctkPluginException& e)
    {
      qDebug() << e;
      throw;
    }
  }
  return this->plugin->fwCtx->services->qServiceManager.loadInterface(serviceDescriptor);
}
