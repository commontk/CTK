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


#include "ctkQtMobilityServiceFactory_p.h"

#include "ctkQtMobilityServiceRuntime_p.h"
#include "ctkQtMobilityServiceActivator_p.h"

#include <service/log/ctkLogService.h>

#include <ctkPluginException.h>
#include <ctkPluginConstants.h>

#include <QBuffer>

ctkQtMobilityServiceFactory::ctkQtMobilityServiceFactory(
  const QServiceInterfaceDescriptor& descr,
  ctkQtMobilityServiceRuntime* sr, QSharedPointer<ctkPlugin> p)
  : activeCount(0), serviceDescriptor(descr), sr(sr), servicePlugin(p)
{
}

QObject* ctkQtMobilityServiceFactory::getService(
  QSharedPointer<ctkPlugin> usingPlugin, ctkServiceRegistration registration)
{
  Q_UNUSED(usingPlugin)
  Q_UNUSED(registration)

  CTK_DEBUG(ctkQtMobilityServiceActivator::getLogService())
      << "QtMobSF::getService(), " << serviceDescriptor.serviceName() << " "
      << serviceDescriptor.interfaceName() << serviceDescriptor.majorVersion()
      << "." << serviceDescriptor.minorVersion() << ", active = " << activeCount;

  if (servicePlugin->getState() != ctkPlugin::ACTIVE)
  {
    try
    {
      servicePlugin->start(0);
    }
    catch (const ctkPluginException* e)
    {
      CTK_ERROR_EXC(ctkQtMobilityServiceActivator::getLogService(), e)
          << "Delayed plugin activation failed.";
      return 0;
    }
  }
  ++activeCount;
  return sr->getQServiceManager().loadInterface(serviceDescriptor);
}

void ctkQtMobilityServiceFactory::ungetService(
  QSharedPointer<ctkPlugin> usingPlugin, ctkServiceRegistration registration, QObject* service)
{
  Q_UNUSED(usingPlugin)
  Q_UNUSED(registration)
  Q_UNUSED(service)

  CTK_DEBUG(ctkQtMobilityServiceActivator::getLogService())
      << "QtMobSF::ungetService(), " << serviceDescriptor.serviceName() << " "
      << serviceDescriptor.interfaceName() << serviceDescriptor.majorVersion()
      << "." << serviceDescriptor.minorVersion() << ", active = " << activeCount;
  --activeCount;
}
