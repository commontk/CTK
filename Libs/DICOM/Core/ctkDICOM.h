/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

#ifndef __ctkDICOM_h
#define __ctkDICOM_h

// Qt includes 
#include <QObject>
#include <QSqlDatabase>

// CTK includes
#include <ctkPimpl.h>

#include "CTKDICOMCoreExport.h"

class ctkDICOMPrivate;
class CTK_DICOM_CORE_EXPORT ctkDICOM : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit ctkDICOM(QObject* parent = 0);
  virtual ~ctkDICOM();
  
  ///
  /// open the SQLite database in @param file. If the file does not
  /// exist, a new database is created and initialized with the
  /// default schema
  virtual bool openDatabase(const QString& file);

  const QSqlDatabase& database() const;
  const QString& GetLastError() const; 
  
  ///
  /// close the database. It must not be used afterwards.
  void closeDatabase();  
  ///
  /// delete all data and reinitialize the database.
  bool initializeDatabase(const char* schemaFile = ":/dicom/dicom-schema.sql");
private:
  CTK_DECLARE_PRIVATE(ctkDICOM);
};

#endif
