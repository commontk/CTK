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

// CTK includes
#include "ctkDicomAbstractExchangeCache.h"
#include "ctkDicomAppHostingTypesHelper.h"
#include "ctkDicomAvailableDataHelper.h"
#include <ctkDicomObjectLocatorCache.h>

class ctkDicomAbstractExchangeCachePrivate
{
public:

  ctkDicomAbstractExchangeCachePrivate();
  ~ctkDicomAbstractExchangeCachePrivate();

  ctkDicomObjectLocatorCache ObjectLocatorCache;

  ctkDicomAppHosting::AvailableData IncomingAvailableData;
  bool lastIncomingData ;
};

//----------------------------------------------------------------------------
// ctkDicomAbstractExchangeCachePrivate methods

//----------------------------------------------------------------------------
ctkDicomAbstractExchangeCachePrivate::ctkDicomAbstractExchangeCachePrivate() : lastIncomingData(false)
{

}

//----------------------------------------------------------------------------
ctkDicomAbstractExchangeCachePrivate::~ctkDicomAbstractExchangeCachePrivate()
{

}

//----------------------------------------------------------------------------
// ctkDicomAbstractExchangeCache methods

//----------------------------------------------------------------------------
ctkDicomAbstractExchangeCache::ctkDicomAbstractExchangeCache() :
  d_ptr(new ctkDicomAbstractExchangeCachePrivate)
{
  connect(this, SIGNAL(internalDataAvailable()), SIGNAL(dataAvailable()), Qt::QueuedConnection);
}

//----------------------------------------------------------------------------
ctkDicomAbstractExchangeCache::~ctkDicomAbstractExchangeCache()
{
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkDicomAbstractExchangeCache::getData(
  const QList<QUuid>& objectUUIDs,
  const QList<QString>& acceptableTransferSyntaxUIDs,
  bool includeBulkData)
{
  Q_UNUSED(acceptableTransferSyntaxUIDs);
  Q_UNUSED(includeBulkData);
  return this->objectLocatorCache()->getData(objectUUIDs);
}

//----------------------------------------------------------------------------
ctkDicomObjectLocatorCache* ctkDicomAbstractExchangeCache::objectLocatorCache() const
{
  Q_D(const ctkDicomAbstractExchangeCache);
  return const_cast<ctkDicomObjectLocatorCache*>(&d->ObjectLocatorCache);
}

//----------------------------------------------------------------------------
bool ctkDicomAbstractExchangeCache::publishData(const ctkDicomAppHosting::AvailableData& availableData, bool lastData)
{
  if (!this->objectLocatorCache()->isCached(availableData))
  {
    return false;
  }
  bool success = this->getOtherSideExchangeService()->notifyDataAvailable(availableData, lastData);
  if(!success)
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
void ctkDicomAbstractExchangeCache::releaseData(const QList<QUuid>& objectUUIDs)
{
  Q_UNUSED(objectUUIDs)
}

//----------------------------------------------------------------------------
const ctkDicomAppHosting::AvailableData& ctkDicomAbstractExchangeCache::getIncomingAvailableData() const
{
  Q_D(const ctkDicomAbstractExchangeCache);
  return d->IncomingAvailableData;
}

//----------------------------------------------------------------------------
bool ctkDicomAbstractExchangeCache::lastIncomingData() const
{
  Q_D(const ctkDicomAbstractExchangeCache);
  return d->lastIncomingData;
}

//----------------------------------------------------------------------------
bool ctkDicomAbstractExchangeCache::notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData)
{
  Q_D(ctkDicomAbstractExchangeCache);
  ctkDicomAvailableDataHelper::appendToAvailableData(d->IncomingAvailableData, data);
  d->lastIncomingData = lastData;
  emit internalDataAvailable();
  return true;
}

//----------------------------------------------------------------------------
void ctkDicomAbstractExchangeCache::cleanIncomingData()
{
  Q_D(ctkDicomAbstractExchangeCache);
  d->IncomingAvailableData = ctkDicomAppHosting::AvailableData();
  d->lastIncomingData = false;
}