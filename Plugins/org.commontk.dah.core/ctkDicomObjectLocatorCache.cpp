/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QHash>
#include <QUuid>
#include <QSet>
#include <QDebug>

// CTK includes
#include "ctkDicomAppHostingTypes.h"
#include "ctkDicomObjectLocatorCache.h"

namespace
{
struct ObjectLocatorCacheItem
{
  ObjectLocatorCacheItem():RefCount(1){}
  ctkDicomAppHosting::ObjectLocator ObjectLocator;
  int RefCount;
};
}

class ctkDicomObjectLocatorCachePrivate
{
public:
  ctkDicomObjectLocatorCachePrivate();

  bool find(const QString& objectUuid, ObjectLocatorCacheItem& objectLocatorCacheItem)const;

  QHash<QString, ObjectLocatorCacheItem> ObjectLocatorMap;
  QSet<QString> TemporaryObjectLocatorSet;
};

//----------------------------------------------------------------------------
// ctkDicomObjectLocatorCachePrivate methods

//----------------------------------------------------------------------------
ctkDicomObjectLocatorCachePrivate::ctkDicomObjectLocatorCachePrivate()
{
}

//----------------------------------------------------------------------------
bool ctkDicomObjectLocatorCachePrivate::find(const QString& objectUuid,
                                             ObjectLocatorCacheItem& objectLocatorCacheItem)const
{
  if (!this->ObjectLocatorMap.contains(objectUuid))
    {
    return false;
    }
  objectLocatorCacheItem = this->ObjectLocatorMap[objectUuid];
  return true;
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
  bool hasCachedData = false;
  QList<QString> uuids = d->ObjectLocatorMap.keys();
  // Loop over top level object descriptors
  foreach(const ctkDicomAppHosting::ObjectDescriptor& objectDescriptor, availableData.objectDescriptors)
    {
    hasCachedData = true;
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
      hasCachedData = true;
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
        hasCachedData = true;
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
          hasCachedData = true;
          if (!uuids.contains(objectDescriptor.descriptorUUID))
            {
            return false;
            }
          }
        }
      }
    }
  return hasCachedData;
}

//----------------------------------------------------------------------------
bool ctkDicomObjectLocatorCache::find(const QString& objectUuid,
                                      ctkDicomAppHosting::ObjectLocator& objectLocator)const
{
  Q_D(const ctkDicomObjectLocatorCache);

  ObjectLocatorCacheItem item;
  bool found = d->find(objectUuid, item);
  if (!found)
    {
    return false;
    }
  objectLocator = item.ObjectLocator;
  return true;
}

//----------------------------------------------------------------------------
void ctkDicomObjectLocatorCache::insert(const QString& objectUuid,
                                        const ctkDicomAppHosting::ObjectLocator& objectLocator,
                                        bool temporary)
{
  Q_D(ctkDicomObjectLocatorCache);
  ObjectLocatorCacheItem item;
  d->find(objectUuid, item);
  if(d->ObjectLocatorMap.contains(objectUuid))
    {
    Q_ASSERT(objectLocator == item.ObjectLocator); // ObjectLocator are expected to match
    item.RefCount++;
    d->ObjectLocatorMap.insert(objectUuid, item);
    return;
    }
  item.ObjectLocator = objectLocator;
  d->ObjectLocatorMap.insert(objectUuid, item);

  if (temporary)
    {
    d->TemporaryObjectLocatorSet.insert(objectUuid);
    }
}

//----------------------------------------------------------------------------
bool ctkDicomObjectLocatorCache::remove(const QString& objectUuid)
{
  Q_D(ctkDicomObjectLocatorCache);
  ObjectLocatorCacheItem item;
  bool found = d->find(objectUuid, item);
  if (!found)
    {
    return false;
    }
  Q_ASSERT(item.RefCount > 0);
  item.RefCount--;
  d->ObjectLocatorMap.insert(objectUuid, item);
  if (item.RefCount == 0)
    {
    if (d->TemporaryObjectLocatorSet.contains(objectUuid))
      {
      // Not implemented - Delete the object
      qDebug() << "ctkDicomObjectLocatorCache::remove - RefCount [1] - Temporary [True] - Not implemented";

      bool removed = d->TemporaryObjectLocatorSet.remove(objectUuid);
      Q_ASSERT(removed);
      }
    int removed = d->ObjectLocatorMap.remove(objectUuid);
    Q_ASSERT(removed == 1);
    }
  return true;
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkDicomObjectLocatorCache::getData(const QList<QUuid>& objectUUIDs)
{
  QList<ctkDicomAppHosting::ObjectLocator> objectLocators;
  foreach(const QUuid& uuid, objectUUIDs)
    {
    ctkDicomAppHosting::ObjectLocator objectLocator;
    bool found = this->find(uuid.toString(), objectLocator);
    if (!found)
      {
      // Use the empty objectLocator
      // TODO Source should be set to NULL
      }
    objectLocators << objectLocator;
    }
  return objectLocators;
}
