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


#include "ctkMetaTypeServiceImpl_p.h"

#include "ctkMetaTypeInformationImpl_p.h"
#include "ctkMTProviderTracker_p.h"
#include "ctkObjectClassDefinitionImpl_p.h"
#include "ctkAttributeDefinitionImpl_p.h"

#include <service/log/ctkLogService.h>

ctkMetaTypeServiceImpl::ctkMetaTypeServiceImpl(ctkLogService* logger, ctkServiceTracker<>* metaTypeProviderTracker)
  : logger(logger), metaTypeProviderTracker(metaTypeProviderTracker)
{
}

ctkMetaTypeInformationPtr ctkMetaTypeServiceImpl::getMetaTypeInformation(const QSharedPointer<ctkPlugin>& plugin)
{
  ctkMetaTypeInformationPtr mti;
  try
  {
    mti = getMetaTypeProvider(plugin);
  }
  catch (const ctkRuntimeException& )
  {
    CTK_ERROR(logger) << "ctkRuntimeException in ctkMetaTypeInformation::getMetaTypeInformation(QSharedPointer<ctkPlugin> plugin)";
    mti.clear();
  }
  return mti;
}

ctkMetaTypeInformationPtr ctkMetaTypeServiceImpl::getMetaTypeProvider(const QSharedPointer<ctkPlugin>& p)
{
  qlonglong pID = p->getPluginId();
  {
    QMutexLocker lock(&_mtpsMutex);
    if (_mtps.contains(pID))
    {
      return _mtps.value(pID);
    }

    ctkMetaTypeInformationImpl* impl = new ctkMetaTypeInformationImpl(p, logger);
    ctkMetaTypeInformation* mti = impl;
    if (!impl->_isThereMeta)
    {
      delete impl;
      mti = new ctkMTProviderTracker(p, logger, metaTypeProviderTracker);
    }
    ctkMetaTypeInformationPtr mtiPtr(mti);
    _mtps.insert(pID, mtiPtr);
    return mtiPtr;
  }
}

void ctkMetaTypeServiceImpl::pluginChanged(const ctkPluginEvent& event)
{
  ctkPluginEvent::Type type = event.getType();
  qlonglong pID = event.getPlugin()->getPluginId();

  switch (type)
  {
    case ctkPluginEvent::UPDATED:
    case ctkPluginEvent::UNINSTALLED:
      _mtps.remove(pID);
      break;
    default :
      break;
  }
}
