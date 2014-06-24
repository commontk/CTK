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


#include "ctkEventAdminActivator_p.h"

#include "util/ctkEALogTracker_p.h"
#include "ctkEAConfiguration_p.h"

#include <QtPlugin>

ctkEALogTracker* ctkEventAdminActivator::logTracker = 0;

ctkEventAdminActivator::ctkEventAdminActivator()
  : config(0)
{

}

ctkEventAdminActivator::~ctkEventAdminActivator()
{
  if (config) delete config;
}

void ctkEventAdminActivator::start(ctkPluginContext* context)
{
  logFileFallback.open(stdout, QIODevice::WriteOnly);
  logTracker = new ctkEALogTracker(context, &logFileFallback);
  logTracker->open();

  if (config) delete config;
  // this creates the event admin and starts it
  config = new ctkEAConfiguration(context);
}

void ctkEventAdminActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  if (config)
  {
    config->destroy();
  }

  logTracker->close();
  delete logTracker;
  logTracker = 0;

  logFileFallback.close();
}

ctkLogService* ctkEventAdminActivator::getLogService()
{
  return logTracker;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2(org_commontk_eventadmin, ctkEventAdminActivator)
#endif
