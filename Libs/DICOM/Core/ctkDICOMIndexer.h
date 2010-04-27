/*=========================================================================

  Library:   CTK
 
  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

#ifndef __ctkDICOMIndexer_h
#define __ctkDICOMIndexer_h

// Qt includes 
#include <QSqlDatabase>

// CTK includes
#include <ctkPimpl.h>

#include "CTKDICOMCoreExport.h"

class ctkDICOMIndexerPrivate;
class CTK_DICOM_CORE_EXPORT ctkDICOMIndexer
{
public:
  explicit ctkDICOMIndexer();
  virtual ~ctkDICOMIndexer();
  /// add directory to database and optionally copy files to destinationDirectory
  void addDirectory(QSqlDatabase database, const QString& directoryName, const QString& destinationDirectoryName = "");
  void refreshDatabase(QSqlDatabase database, const QString& directoryName);

private:
  CTK_DECLARE_PRIVATE(ctkDICOMIndexer);
};

#endif
