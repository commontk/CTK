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


#include "ctkMetaTypeActivator_p.h"
#include "ctkMTLogTracker_p.h"
#include "ctkMetaTypeServiceImpl_p.h"
#include "ctkMTMsg_p.h"

#include <service/cm/ctkManagedService.h>
#include <service/metatype/ctkMetaTypeProvider.h>

#include <QtPlugin>

ctkMTLogTracker* ctkMetaTypeActivator::logTracker = 0;

QString ctkMetaTypeActivator::FILTER()
{
  static QString filter(QString("(|(&(") + ctkPluginConstants::OBJECTCLASS
                        + "=" + qobject_interface_iid<ctkManagedService*>() + "*)("
                        + ctkPluginConstants::SERVICE_PID + "=*))(&("
                        + ctkPluginConstants::OBJECTCLASS + '='
                        + qobject_interface_iid<ctkMetaTypeProvider*>() + ")(|("
                        + ctkMetaTypeProvider::METATYPE_PID + "=*)("
                        + ctkMetaTypeProvider::METATYPE_FACTORY_PID + "=*))))");
  return filter;
}

const QString ctkMetaTypeActivator::SERVICE_PID = "org.commontk.metatype.impl.MetaType";

ctkMetaTypeActivator::ctkMetaTypeActivator()
  : metaTypeProviderTracker(0), metaTypeService(0)
{
}

ctkMetaTypeActivator::~ctkMetaTypeActivator()
{
  delete metaTypeProviderTracker;
  delete metaTypeService;
  delete logTracker;
  logTracker = 0;
}

void ctkMetaTypeActivator::start(ctkPluginContext* context)
{
  delete metaTypeProviderTracker;
  delete metaTypeService;
  delete logTracker;

  ctkMTLogTracker* lsTracker = 0;
  logFileFallback.open(stdout, QIODevice::WriteOnly);
  ctkLDAPSearchFilter filter(FILTER());
  ctkServiceTracker<>* mtpTracker = 0;

  {
    QMutexLocker l(&mutex);
    lsTracker = logTracker = new ctkMTLogTracker(context, &logFileFallback);
    mtpTracker = metaTypeProviderTracker = new ctkServiceTracker<>(context, filter);
  }
  // Do this first to make logging available as early as possible.
  lsTracker->open();
  CTK_DEBUG(lsTracker) << "====== Meta Type Service starting ! =====";
  // Do this next to make ctkMetaTypeProviders available as early as possible.
  mtpTracker->open();

  // Register the Meta Type service
  ctkDictionary properties;
  properties.insert(ctkPluginConstants::SERVICE_VENDOR, "CommonTK");
  properties.insert(ctkPluginConstants::SERVICE_DESCRIPTION, ctkMTMsg::SERVICE_DESCRIPTION);
  properties.insert(ctkPluginConstants::SERVICE_PID, SERVICE_PID);
  metaTypeService = new ctkMetaTypeServiceImpl(lsTracker, mtpTracker);
  context->connectPluginListener(metaTypeService, SLOT(pluginChanged(ctkPluginEvent)), Qt::DirectConnection);
  metaTypeServiceRegistration = context->registerService<ctkMetaTypeService>(metaTypeService, properties);
}

void ctkMetaTypeActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  CTK_DEBUG(logTracker) << "====== Meta Type Service stopping ! =====";
  metaTypeService->disconnect();
  metaTypeProviderTracker->close();
  metaTypeServiceRegistration.unregister();
  metaTypeServiceRegistration = 0;
  // Do this last to leave logging available as long as possible.
  logTracker->close();
}

ctkLogService* ctkMetaTypeActivator::getLogService()
{
  return logTracker;
}


#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2(org_commontk_metatype, ctkMetaTypeActivator)
#endif
