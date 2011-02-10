/*=========================================================================

  Library:   CTK

  Copyright (c) 2010

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

#ifndef __ctkDICOMDatabase_h
#define __ctkDICOMDatabase_h

// Qt includes 
#include <QObject>
#include <QSqlDatabase>

#include "ctkDICOMCoreExport.h"

class ctkDICOMDatabasePrivate;
class DcmDataset;

class CTK_DICOM_CORE_EXPORT ctkDICOMDatabase : public QObject
{
  Q_OBJECT
public:
  explicit ctkDICOMDatabase();
  explicit ctkDICOMDatabase(QString databaseFile);
  virtual ~ctkDICOMDatabase();

  const QSqlDatabase& database() const;
  const QString lastError() const;
  const QString databaseFilename() const;
  const QString databaseDirectory() const;

  ///
  /// open the SQLite database in @param file. If the file does not
  /// exist, a new database is created and initialized with the
  /// default schema
  virtual void openDatabase(const QString file);

  ///
  /// close the database. It must not be used afterwards.
  void closeDatabase();
  ///
  /// delete all data and reinitialize the database.
  bool initializeDatabase(const char* schemaFile = ":/dicom/dicom-schema.sql");

  /**
   * Will create an entry in the appropriate tables for this dataset.
   */
  void insert ( DcmDataset* dataset, QString filename );
  /**
   * Insert into the database if not already exsting.
   */
  void insert ( DcmDataset *dataset );

protected:
  QScopedPointer<ctkDICOMDatabasePrivate> d_ptr;



private:
  Q_DECLARE_PRIVATE(ctkDICOMDatabase);
  Q_DISABLE_COPY(ctkDICOMDatabase);
};

#endif
