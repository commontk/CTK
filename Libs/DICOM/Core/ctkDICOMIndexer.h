/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
