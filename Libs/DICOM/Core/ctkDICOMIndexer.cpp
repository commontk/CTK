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
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QDate>
#include <QStringList>
#include <QSet>
#include <QFile>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <QPixmap>


// ctkDICOM includes
#include "ctkLogger.h"
#include "ctkDICOMIndexer.h"
#include "ctkDICOMAbstractThumbnailGenerator.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */
#include <dcmtk/dcmimgle/dcmimage.h>  /* for class DicomImage */
#include <dcmtk/dcmimage/diregist.h>  /* include support for color images */


//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMIndexer" );
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class ctkDICOMIndexerPrivate
{
public:
  ctkDICOMIndexerPrivate();
  ~ctkDICOMIndexerPrivate();

  ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator;

};

//------------------------------------------------------------------------------
// ctkDICOMIndexerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::ctkDICOMIndexerPrivate()
{
    this->thumbnailGenerator = NULL;
}

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::~ctkDICOMIndexerPrivate()
{
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ctkDICOMIndexer methods

//------------------------------------------------------------------------------
ctkDICOMIndexer::ctkDICOMIndexer(QObject *parent):d_ptr(new ctkDICOMIndexerPrivate)
{
  Q_UNUSED(parent);
}

//------------------------------------------------------------------------------
ctkDICOMIndexer::~ctkDICOMIndexer()
{
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addFile(ctkDICOMDatabase& ctkDICOMDatabase, 
                                   const QString& filePath,
                                   const QString& destinationDirectoryName,
                                   bool createHierarchy,
                                   bool createThumbnails)
{
  // Q_D(ctkDICOMIndexer);

  logger.setDebug();




  emit indexingFilePath(filePath);

  
  ctkDICOMDatabase.insert(filePath, false, true);

}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addDirectory(ctkDICOMDatabase& ctkDICOMDatabase, 
                                   const QString& directoryName,
                                   const QString& destinationDirectoryName,
                                   bool createHierarchy,
                                   bool createThumbnails)
{
  const std::string src_directory(directoryName.toStdString());

  OFList<OFString> originalDcmtkFileNames;
  OFList<OFString> dcmtkFileNames;
  OFStandard::searchDirectoryRecursively( QDir::toNativeSeparators(src_directory.c_str()).toAscii().data(), originalDcmtkFileNames, "", "");

  // hack to reverse list of filenames (not neccessary when image loading works correctly)
  for ( OFListIterator(OFString) iter = originalDcmtkFileNames.begin(); iter != originalDcmtkFileNames.end(); ++iter )
  {
    dcmtkFileNames.push_front( *iter );
  }

  OFListIterator(OFString) iter = dcmtkFileNames.begin();

  OFListIterator(OFString) last = dcmtkFileNames.end();

  if(iter == last) return;

  emit foundFilesToIndex(dcmtkFileNames.size());

  /* iterate over all input filenames */
  int fileNumber = 0;
  while (iter != last)
  {
    emit indexingFileNumber(++fileNumber);
    QString filePath((*iter).c_str());
    this->addFile(ctkDICOMDatabase, filePath, destinationDirectoryName, createHierarchy, createThumbnails);
    ++iter;
  }
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::refreshDatabase(ctkDICOMDatabase& ctkDICOMDatabase, const QString& directoryName)
{
  /*
   * Probably this should go to the database class as well
   * Or we have to extend the interface to make possible what we do here
   * without using SQL directly
   

  /// get all filenames from the database
  QSqlQuery allFilesQuery(ctkDICOMDatabase.database());
  QStringList databaseFileNames;
  QStringList filesToRemove;
  this->loggedExec(allFilesQuery, "SELECT Filename from Images;");

  while (allFilesQuery.next())
    {
    QString fileName = allFilesQuery.value(0).toString();
    databaseFileNames.append(fileName);
    if (! QFile::exists(fileName) )
      {
      filesToRemove.append(fileName);
      }
    }

  QSet<QString> filesytemFiles;
  QDirIterator dirIt(directoryName);
  while (dirIt.hasNext())
    {
    filesytemFiles.insert(dirIt.next());
    }
  
  // TODO: it looks like this function was never finished...
  // 
  // I guess the next step is to remove all filesToRemove from the database
  // and also to add filesystemFiles into the database tables
  */ 
  }

//------------------------------------------------------------------------------
void ctkDICOMIndexer::setThumbnailGenerator(ctkDICOMAbstractThumbnailGenerator *generator){
    Q_D(ctkDICOMIndexer);
    d->thumbnailGenerator = generator;
}

//------------------------------------------------------------------------------
ctkDICOMAbstractThumbnailGenerator* ctkDICOMIndexer::thumbnailGenerator(){
    Q_D(ctkDICOMIndexer);
    return d->thumbnailGenerator;
}

