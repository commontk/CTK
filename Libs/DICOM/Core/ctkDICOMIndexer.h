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

#ifndef __ctkDICOMIndexer_h
#define __ctkDICOMIndexer_h

// Qt includes 
#include <QObject>
#include <QSqlDatabase>

#include "ctkDICOMCoreExport.h"
#include "ctkDICOMDatabase.h"

class ctkDICOMIndexerPrivate;
class ctkDICOMAbstractThumbnailGenerator;

/**
    \brief Indexes DICOM images located in local directory into an Sql database
*/
class CTK_DICOM_CORE_EXPORT ctkDICOMIndexer : public QObject
{
  Q_OBJECT
public:
  explicit ctkDICOMIndexer(QObject *parent = 0);
  virtual ~ctkDICOMIndexer();
  
  /**
      \brief Adds directory to database and optionally copies files to
      destinationDirectory.
      
      Scan the directory using Dcmtk and populate the database with all the
      DICOM images accordingly.
  */
  Q_INVOKABLE void addDirectory(ctkDICOMDatabase& database, const QString& directoryName,
                    const QString& destinationDirectoryName = "",
                    bool createHierarchy = true, bool createThumbnails = true);

  /**
      \brief Adds a file to database and optionally copies the file to
      destinationDirectory.
 
      Scan the file using Dcmtk and populate the database with all the
      DICOM fields accordingly.
  */
  Q_INVOKABLE void addFile(ctkDICOMDatabase& database, const QString& filePath,
                    const QString& destinationDirectoryName = "",
                    bool createHierarchy = true, bool createThumbnails = true);

  Q_INVOKABLE void refreshDatabase(ctkDICOMDatabase& database, const QString& directoryName);

  /**
      \brief runs a query and prints debug output of status
 
  */
  bool loggedExec(QSqlQuery& query);
  bool loggedExec(QSqlQuery& query, const QString& queryString);


  ///
  /// set thumbnail generator object
  void setThumbnailGenerator(ctkDICOMAbstractThumbnailGenerator* generator);
  ///
  /// get thumbnail genrator object
  ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator();

signals:
  void foundFilesToIndex(int);
  void indexingFileNumber(int);
  void indexingFilePath(QString);

protected:
  QScopedPointer<ctkDICOMIndexerPrivate> d_ptr;
  
private:
  Q_DECLARE_PRIVATE(ctkDICOMIndexer);
  Q_DISABLE_COPY(ctkDICOMIndexer);

};

#endif
