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

/*
 * ctkQtMobilityServiceActivator.cxx
 *
 *  Created on: Mar 29, 2010
 *      Author: zelzer
 */


#include "ctkQtMobilityServiceActivator_p.h"
#include "ctkQtMobilityServiceRuntime_p.h"

#include <ctkServiceTracker.h>
#include <ctkServiceException.h>
#include <service/log/ctkLogService.h>

#include <QtPlugin>

ctkQtMobilityServiceActivator::LogTracker* ctkQtMobilityServiceActivator::logTracker = 0;

void ctkQtMobilityServiceActivator::start(ctkPluginContext* context)
{
  logTracker = new LogTracker(context);
  logTracker->open();
  mobsr = new ctkQtMobilityServiceRuntime(context);
  mobsr->start();
}

void ctkQtMobilityServiceActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  if (mobsr)
  {
    mobsr->stop();
    delete mobsr;
    mobsr = 0;
  }

  if (logTracker)
  {
    logTracker->close();
    delete logTracker;
    logTracker = 0;
  }
}

ctkLogService* ctkQtMobilityServiceActivator::getLogService()
{
  if (logTracker == 0)
  {
    return 0;
  }

  return logTracker->getService();
}

void ctkQtMobilityServiceActivator::logError(ctkPluginContext* pc, const QString& msg, std::exception* e)
{
  logPluginContext(pc, ctkLogService::LOG_ERROR, msg, e);
}

void ctkQtMobilityServiceActivator::logWarning(ctkPluginContext* pc, const QString& msg, std::exception* e)
{
  logPluginContext(pc, ctkLogService::LOG_WARNING, msg, e);
}

void ctkQtMobilityServiceActivator::logInfo(ctkPluginContext* pc, const QString& msg, std::exception* e)
{
  logPluginContext(pc, ctkLogService::LOG_INFO, msg, e);
}

void ctkQtMobilityServiceActivator::logPluginContext(ctkPluginContext* pc, int level, const QString& msg, const std::exception* e)
{
  try
  {
    ctkServiceReference sr = pc->getServiceReference("ctkLogService");
    ctkLogService* log = qobject_cast<ctkLogService*>(pc->getService(sr));
    if (log)
    {
      log->log(level, msg, e);
      pc->ungetService(sr);
    }
  }
  catch (const ctkServiceException&)
  { }
}


Q_EXPORT_PLUGIN2(org_commontk_qtmobility_service, ctkQtMobilityServiceActivator)
