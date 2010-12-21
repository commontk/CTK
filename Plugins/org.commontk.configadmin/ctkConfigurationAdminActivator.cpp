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


#include "ctkConfigurationAdminActivator_p.h"
#include "ctkConfigurationAdminFactory_p.h"

#include <ctkServiceTracker.h>
#include <service/cm/ctkConfigurationAdmin.h>
#include <service/cm/ctkConfigurationListener.h>

#include <QtPlugin>


ctkConfigurationAdminActivator::ctkConfigurationAdminActivator()
  : logTracker(0), factory(0)
{
}

ctkConfigurationAdminActivator::~ctkConfigurationAdminActivator()
{
  delete logTracker;
  delete factory;
}

void ctkConfigurationAdminActivator::start(ctkPluginContext* context)
{
  logFileFallback.open(stdout, QIODevice::WriteOnly);
  logTracker = new ctkCMLogTracker(context, &logFileFallback);
  logTracker->open();
//  if (checkEventAdmin()) {
//     eventAdapter = new ConfigurationEventAdapter(context);
//     eventAdapter.start();
//    }
  factory = new ctkConfigurationAdminFactory(context, logTracker);
  factory->start();
  context->connectPluginListener(factory, SLOT(pluginChanged(const ctkPluginEvent&)));
  registration = context->registerService<ctkConfigurationAdmin>(factory);
}

void ctkConfigurationAdminActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  if (registration)
  {
    registration.unregister();
  }

  factory->stop();
  delete factory;
  factory = 0;

//  if (eventAdapter != null)
//  {
//    eventAdapter.stop();
//    eventAdapter = null;
//  }

  logTracker->close();
  delete logTracker;
  logTracker = 0;

  logFileFallback.close();
}

Q_EXPORT_PLUGIN2(org_commontk_configadmin, ctkConfigurationAdminActivator)
