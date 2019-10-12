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

#ifndef CTKDICOMEXCHANGEIMPL_H
#define CTKDICOMEXCHANGEIMPL_H

// Qt includes
#include <QScopedPointer>

// CTK includes
#include "ctkDicomAppHostingTypes.h"
#include <org_commontk_dah_core_Export.h>

class ctkDicomObjectLocatorCachePrivate;
class QUuid;

/**
  *
  */
class org_commontk_dah_core_EXPORT ctkDicomObjectLocatorCache
{

public:

  ctkDicomObjectLocatorCache();
  virtual ~ctkDicomObjectLocatorCache();

  bool isCached(const ctkDicomAppHosting::AvailableData& availableData)const;

  bool find(const QString& objectUuid, ctkDicomAppHosting::ObjectLocator& objectLocator)const;

  void insert(
    const QString& objectUuid, const ctkDicomAppHosting::ObjectLocator& objectLocator, bool temporary = false);

  bool remove(const QString& objectUuid);

  QList<ctkDicomAppHosting::ObjectLocator> getData(const QList<QUuid>& objectUUIDs);

private:
  Q_DECLARE_PRIVATE(ctkDicomObjectLocatorCache)
  const QScopedPointer<ctkDicomObjectLocatorCachePrivate> d_ptr;
};

#endif // CTKDICOMEXCHANGEIMPL_H
