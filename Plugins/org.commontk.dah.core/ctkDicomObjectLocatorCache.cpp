/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QHash>

// CTK includes
#include "ctkDicomAppHostingTypes.h"
#include "ctkDicomObjectLocatorCache.h"

class ctkDicomObjectLocatorCachePrivate
{
public:
  ctkDicomObjectLocatorCachePrivate();

  QHash<QString, ctkDicomAppHosting::ObjectLocator> ObjectLocatorMap;
};

//----------------------------------------------------------------------------
// ctkDicomObjectLocatorCachePrivate methods

//----------------------------------------------------------------------------
ctkDicomObjectLocatorCachePrivate::ctkDicomObjectLocatorCachePrivate()
{
}

//----------------------------------------------------------------------------
// ctkDicomObjectLocatorCache methods

//----------------------------------------------------------------------------
ctkDicomObjectLocatorCache::ctkDicomObjectLocatorCache() : d_ptr(new ctkDicomObjectLocatorCachePrivate())
{
}

//----------------------------------------------------------------------------
ctkDicomObjectLocatorCache::~ctkDicomObjectLocatorCache()
{
}

//----------------------------------------------------------------------------
bool ctkDicomObjectLocatorCache::isCached(const ctkDicomAppHosting::AvailableData& availableData)const
{
  Q_D(const ctkDicomObjectLocatorCache);
  QList<QString> uuids = d->ObjectLocatorMap.keys();
  // Loop over top level object descriptors
  foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, availableData.objectDescriptors)
    {
    if (!uuids.contains(objectDescriptor.descriptorUUID))
      {
      return false;
      }
    }
  // Loop over patients
  foreach(const ctkDicomAppHosting::Patient& patient, availableData.patients)
    {
    // Loop over patient level object descriptors
    foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, patient.objectDescriptors)
      {
      if (!uuids.contains(objectDescriptor.descriptorUUID))
        {
        return false;
        }
      }
    // Loop over studies
    foreach(const ctkDicomAppHosting::Study& study, patient.studies)
      {
      // Loop over study level object descriptors
      foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, study.objectDescriptors)
        {
        if (!uuids.contains(objectDescriptor.descriptorUUID))
          {
          return false;
          }
        }
      // Loop over series
      foreach(const ctkDicomAppHosting::Series& series, study.series)
        {
        // Loop over series level object descriptors
        foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, series.objectDescriptors)
          {
          if (!uuids.contains(objectDescriptor.descriptorUUID))
            {
            return false;
            }
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
bool ctkDicomObjectLocatorCache::find(const QString& objectUuid,
                                         ctkDicomAppHosting::ObjectLocator& objectLocator)const
{
  Q_D(const ctkDicomObjectLocatorCache);
  if (!d->ObjectLocatorMap.contains(objectUuid))
    {
    return false;
    }
  objectLocator = d->ObjectLocatorMap.value(objectUuid);
  return true;
}

//----------------------------------------------------------------------------
void ctkDicomObjectLocatorCache::insert(const QString& objectUuid,
                                        const ctkDicomAppHosting::ObjectLocator& objectLocator)
{
  Q_D(ctkDicomObjectLocatorCache);
  if(d->ObjectLocatorMap.contains(objectUuid))
    {
    return;
    }
  d->ObjectLocatorMap.insert(objectUuid, objectLocator);
}

//----------------------------------------------------------------------------
bool ctkDicomObjectLocatorCache::remove(const QString& objectUuid)
{
  Q_D(ctkDicomObjectLocatorCache);
  int removed = d->ObjectLocatorMap.remove(objectUuid);
  Q_ASSERT(removed > 1);
  return (removed == 1);
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkDicomObjectLocatorCache::getData(
  const QList<QUuid>& objectUUIDs,
  const QList<QString>& acceptableTransferSyntaxUIDs,
  bool includeBulkData)
{
  QList<ctkDicomAppHosting::ObjectLocator> objectLocators;
  foreach(const QUuid& uuid, objectUUIDs)
    {
    ctkDicomAppHosting::ObjectLocator objectLocator;
    bool found = this->find(uuid, objectLocator);
    if (!found)
      {
      // What to do .. ? Create an empty objectLocator ...
      continue;
      }
    if (includeBulkData)
      {
      Q_UNUSED(acceptableTransferSyntaxUIDs);
      // Not implemented
      }
    objectLocators << objectLocator;
    }
  return objectLocators;
}
