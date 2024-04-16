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
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QUuid>
#include <QVariant>

// ctkDICOM includes
#include "ctkDICOMDatabase_p.h"
#include "ctkDICOMAbstractThumbnailGenerator.h"
#include "ctkDICOMItem.h"
#include "ctkDICOMJobResponseSet.h"

#include "ctkLogger.h"
#include "ctkUtils.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */
#include <dcmtk/dcmimgle/dcmimage.h>

#include <dcmtk/dcmjpeg/djdecode.h>  /* for dcmjpeg decoders */
#include <dcmtk/dcmjpeg/djencode.h>  /* for dcmjpeg encoders */
#include <dcmtk/dcmdata/dcrledrg.h>  /* for DcmRLEDecoderRegistration */
#include <dcmtk/dcmdata/dcrleerg.h>  /* for DcmRLEEncoderRegistration */

//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMDatabase" );
//------------------------------------------------------------------------------

/// Flag for tag cache to avoid repeated searches for tags that do no exist
static QString TagNotInInstance("__TAG_NOT_IN_INSTANCE__");
/// Flag for tag cache indicating that the value really is the empty string
static QString ValueIsEmptyString("__VALUE_IS_EMPTY_STRING__");
/// Tag exists in the instance and non-empty but its value is not stored (e.g., because it is too long)
static QString ValueIsNotStored("__VALUE_IS_NOT_STORED__");
/// Separator character for table and field names to be used in display rules manager
static QString TableFieldSeparator(":");

//------------------------------------------------------------------------------
// ctkDICOMDatabasePrivate methods

//------------------------------------------------------------------------------
ctkDICOMDatabasePrivate::ctkDICOMDatabasePrivate(ctkDICOMDatabase& o)
  : q_ptr(&o)
  , DisplayedFieldsTableAvailable(false)
  , UseShortStoragePath(true)
  , ThumbnailGenerator(nullptr)
  , TagCacheVerified(false)
  , SchemaVersion("0.8.1")
{
  this->resetLastInsertedValues();
  this->DisplayedFieldGenerator = new ctkDICOMDisplayedFieldGenerator(q_ptr);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::resetLastInsertedValues()
{
  this->InsertedPatientsCompositeIDCache.clear();
  this->InsertedConnectionsIDCache.clear();
  this->InsertedStudyUIDsCache.clear();
  this->InsertedSeriesUIDsCache.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::init(QString databaseFilename)
{
  Q_Q(ctkDICOMDatabase);

  q->openDatabase(databaseFilename);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::registerCompressionLibraries()
{
  // Register the JPEG libraries in case we need them
  //   (registration only happens once, so it's okay to call repeatedly)
  // register global JPEG decompression codecs
  DJDecoderRegistration::registerCodecs();
  // register global JPEG compression codecs
  DJEncoderRegistration::registerCodecs();
  // register RLE compression codec
  DcmRLEEncoderRegistration::registerCodecs();
  // register RLE decompression codec
  DcmRLEDecoderRegistration::registerCodecs();
}

//------------------------------------------------------------------------------
ctkDICOMDatabasePrivate::~ctkDICOMDatabasePrivate()
{
}

//------------------------------------------------------------------------------
int ctkDICOMDatabasePrivate::rowCount(const QString& tableName)
{
  QSqlQuery numberOfItemsQuery(this->Database);
  numberOfItemsQuery.prepare(QString("SELECT COUNT(*) FROM %1;").arg(tableName));
  int numberOfItems = 0;
  if (numberOfItemsQuery.exec())
  {
    numberOfItemsQuery.first();
    numberOfItems = numberOfItemsQuery.value(0).toInt();
  }
  else
  {
    logger.error("SQLITE ERROR: " + numberOfItemsQuery.lastError().driverText());
  }
  return numberOfItems;
}


//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::loggedExec(QSqlQuery& query)
{
  return (loggedExec(query, QString("")));
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::loggedExec(QSqlQuery& query, const QString& queryString)
{
  bool success;
  if (queryString.compare(""))
  {
    success = query.exec(queryString);
  }
  else
  {
    success = query.exec();
  }

  if (!success)
  {
    QString sqlError = query.lastError().text();
    QString lastQuery = query.lastQuery();
    logger.error(QString("SQL failed: \n%1 \nError: \n%2")
      .arg(lastQuery).arg(sqlError));
  }

  return (success);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::loggedExecBatch(QSqlQuery& query)
{
  bool success;
  success = query.execBatch();
  if (!success)
  {
    QString sqlError = query.lastError().text();
    QString lastQuery = query.lastQuery();
    logger.error(QString("SQL failed: \n%1 \nError: \n%2")
      .arg(lastQuery).arg(sqlError));
  }
  return (success);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabasePrivate::allFilesInDatabase()
{
  // Get all filenames from the database
  QSqlQuery allFilesQuery(this->Database);
  QStringList allFileNames;
  loggedExec(allFilesQuery,QString("SELECT Filename from Images;"));

  while (allFilesQuery.next())
  {
    allFileNames << this->absolutePathFromInternal(allFilesQuery.value(0).toString());
  }
  return allFileNames;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabasePrivate::readValueFromFile(const QString& fileName, const QString& sopInstanceUID, const QString& tag)
{
  Q_Q(ctkDICOMDatabase);

  // Here is where the real lookup happens
  // - we create a ctkDICOMItem and extract the value from there
  // - then we convert to the appropriate type of string
  //
  // An optimization we could consider
  // - check if we are currently looking at the dataset for this fileName
  // - if so, are we looking for a group/element that is past the last one
  //   accessed
  //   -- if so, keep looking for the requested group/element
  //   -- if not, start again from the beginning

  ctkDICOMItem dataset;
  dataset.InitializeFromFile(fileName);
  if (!dataset.IsInitialized())
  {
    logger.error("File " + fileName + " could not be initialized.");
    return "";
  }

  QString upperTag = tag.toUpper();
  QString value;
  unsigned short group, element;
  q->tagToGroupElement(upperTag, group, element);
  DcmTagKey tagKey(group, element);
  if (this->TagsToExcludeFromStorage.contains(upperTag))
  {
    if (dataset.TagExists(tagKey))
    {
      value = ValueIsNotStored;
    }
    else
    {
      value = TagNotInInstance;
    }
  }
  else
  {
    value = dataset.GetAllElementValuesAsString(tagKey);
  }

  // Store result in cache
  q->cacheTag(sopInstanceUID, upperTag, value);

  return value;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::executeScript(const QString script)
{
  QFile scriptFile(script);
  scriptFile.open(QIODevice::ReadOnly);
  if  ( !scriptFile.isOpen() )
  {
    qDebug() << "Script file " << script << " could not be opened!\n";
    return false;
  }

  QString sqlCommands( QTextStream(&scriptFile).readAll() );
  sqlCommands.replace( '\n', ' ' );
  sqlCommands.remove( '\r' );
  sqlCommands.replace("; ", ";\n");

  QStringList sqlCommandsLines = sqlCommands.split('\n');

  QSqlQuery query(Database);

  for (QStringList::iterator it = sqlCommandsLines.begin(); it != sqlCommandsLines.end()-1; ++it)
  {
    if (! (*it).startsWith("--") )
    {
      query.exec(*it);
      if (query.lastError().type())
      {
        qDebug() << "There was an error during execution of the statement: " << (*it);
        qDebug() << "Error message: " << query.lastError().text();
        return false;
      }
    }
  }
  return true;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabasePrivate::filenames(QString table)
{
  /// get all filenames from the database
  QSqlQuery allFilesQuery(this->Database);
  QStringList allFileNames;
  loggedExec(allFilesQuery,QString("SELECT Filename from %1 ;").arg(table) );

  while (allFilesQuery.next())
  {
    allFileNames << this->absolutePathFromInternal(allFilesQuery.value(0).toString());
  }
  return allFileNames;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::insertPatient(const ctkDICOMItem& dataset,
                                            const QString& patientID,
                                            const QString& patientsName,
                                            int& dbPatientID)
{
  // Check if patient is already present in the db
  QString tempPatientID(patientID), tempPatientsName(patientsName), patientsBirthDate;
  if (tempPatientID.isEmpty())
  {
    tempPatientID = dataset.GetElementAsString(DCM_PatientID);
  }
  if (tempPatientsName.isEmpty())
  {
    tempPatientsName = dataset.GetElementAsString(DCM_PatientName);
  }
  patientsBirthDate = dataset.GetElementAsString(DCM_PatientBirthDate);

  QSqlQuery checkPatientExistsQuery(this->Database);
  checkPatientExistsQuery.prepare("SELECT * FROM Patients WHERE PatientID = ? AND PatientsName = ?");
  checkPatientExistsQuery.bindValue(0, tempPatientID);
  checkPatientExistsQuery.bindValue(1, tempPatientsName);
  loggedExec(checkPatientExistsQuery);

  QString compositeID = ctkDICOMDatabase::compositePatientID(tempPatientID, tempPatientsName, patientsBirthDate);
  bool patientFound = checkPatientExistsQuery.next();
  if (patientFound)
  {
    // patient found
    dbPatientID = checkPatientExistsQuery.value(checkPatientExistsQuery.record().indexOf("UID")).toInt();
    logger.debug("Found patient in the database as UID: " + QString::number(dbPatientID));
    logger.debug("New patient ID cache item: " + compositeID + "->" + dbPatientID);
  }
  else
  {
    // insert patient
    QString patientsBirthTime(dataset.GetElementAsString(DCM_PatientBirthTime));
    QString patientsSex(dataset.GetElementAsString(DCM_PatientSex));
    QString patientsAge(dataset.GetElementAsString(DCM_PatientAge));
    QString patientComments(dataset.GetElementAsString(DCM_PatientComments));

    QSqlQuery insertPatientStatement(this->Database);
    insertPatientStatement.prepare("INSERT INTO Patients "
      "('UID', 'PatientsName', 'PatientID', 'PatientsBirthDate', 'PatientsBirthTime', 'PatientsSex', 'PatientsAge', 'PatientsComments', "
      "'InsertTimestamp', 'DisplayedPatientsName', 'DisplayedNumberOfStudies', 'DisplayedFieldsUpdatedTimestamp', 'Connections')"
      "VALUES ( NULL, ?, ?, ?, ?, ?, ?, ?, ?, NULL, NULL, NULL, NULL)");
    insertPatientStatement.bindValue(0, tempPatientsName);
    insertPatientStatement.bindValue(1, tempPatientID);
    insertPatientStatement.bindValue(2, QDate::fromString(patientsBirthDate, "yyyyMMdd"));
    insertPatientStatement.bindValue(3, patientsBirthTime);
    insertPatientStatement.bindValue(4, patientsSex);
    // TODO: shift patient's age to study,
    // since this is not a patient level attribute in images
    // insertPatientStatement.bindValue(5, patientsAge);
    insertPatientStatement.bindValue(6, patientComments);
    insertPatientStatement.bindValue(7, QDateTime::currentDateTime());
    if (!loggedExec(insertPatientStatement))
    {
      return false;
    }
    dbPatientID = insertPatientStatement.lastInsertId().toInt();

    logger.debug("New patient inserted: database item ID = " + QString().setNum(dbPatientID));
  }

  this->InsertedPatientsCompositeIDCache[compositeID] = dbPatientID;
  return !patientFound;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::insertConnectionName(const int& dbPatientID,
                                                   const QString& connectionName)
{
  Q_Q(ctkDICOMDatabase);

  // check if connection name is already stored
  QMap<QString, QStringList> connectionsInformation = q->connectionsInformationForPatient(QString::number(dbPatientID));
  QStringList allowList = connectionsInformation["allow"];
  QStringList denyList = connectionsInformation["deny"];

  bool connectionNameFound = allowList.contains(connectionName);
  if (connectionNameFound)
  {
    // connection name found
    logger.debug("Found connection name in the patient database as UID: " + QString::number(dbPatientID));
    logger.debug("New connection name ID cache item: " + QString::number(dbPatientID) + "->" + connectionName);
  }
  else if (!denyList.contains(connectionName))
  {
    allowList.append(connectionName);
    QString connectionsData = this->convertConnectionInfoToJson(allowList, denyList);

    // insert connection name
    QSqlQuery updateConnectionsStatement(this->Database);
    updateConnectionsStatement.prepare("UPDATE Patients SET Connections = :connectionsData WHERE UID = :uid");
    updateConnectionsStatement.bindValue(":connectionsData", connectionsData);
    updateConnectionsStatement.bindValue(":uid", dbPatientID);

    if (!loggedExec(updateConnectionsStatement))
    {
      return false;
    }
    logger.debug("New connection name inserted: patient database item ID = " + QString().setNum(dbPatientID));
  }

  if (this->InsertedConnectionsIDCache.contains(dbPatientID))
  {
    if (!this->InsertedConnectionsIDCache[dbPatientID].contains(connectionName))
    {
      this->InsertedConnectionsIDCache[dbPatientID].append(connectionName);
    }
  }
  else
  {
    this->InsertedConnectionsIDCache[dbPatientID] = QStringList(connectionName);
  }

  return !connectionNameFound;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::updateConnections(const QString& dbPatientID,
                                                const QStringList& allowList,
                                                const QStringList& denyList)
{
  QString connectionsData = this->convertConnectionInfoToJson(allowList, denyList);

  QSqlQuery updateConnectionsStatement(this->Database);
  updateConnectionsStatement.prepare("UPDATE Patients SET Connections = :connectionsData WHERE UID = :uid");
  updateConnectionsStatement.bindValue(":connectionsData", connectionsData);
  updateConnectionsStatement.bindValue(":uid", dbPatientID);

  if (!loggedExec(updateConnectionsStatement))
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::insertStudy(const ctkDICOMItem& dataset, const int& dbPatientID)
{
  QString studyInstanceUID(dataset.GetElementAsString(DCM_StudyInstanceUID) );
  QSqlQuery checkStudyExistsQuery(this->Database);
  checkStudyExistsQuery.prepare( "SELECT * FROM Studies WHERE StudyInstanceUID = ?" );
  checkStudyExistsQuery.bindValue( 0, studyInstanceUID );
  if (!loggedExec(checkStudyExistsQuery))
  {
    return false;
  }
  if (!checkStudyExistsQuery.next())
  {
    logger.debug("Need to insert new study: " + studyInstanceUID);

    QString studyID(dataset.GetElementAsString(DCM_StudyID) );
    QString studyDate(dataset.GetElementAsString(DCM_StudyDate) );
    QString studyTime(dataset.GetElementAsString(DCM_StudyTime) );
    QString accessionNumber(dataset.GetElementAsString(DCM_AccessionNumber) );
    QString modalitiesInStudy(dataset.GetElementAsString(DCM_ModalitiesInStudy) );
    QString institutionName(dataset.GetElementAsString(DCM_InstitutionName) );
    QString performingPhysiciansName(dataset.GetElementAsString(DCM_PerformingPhysicianName) );
    QString referringPhysician(dataset.GetElementAsString(DCM_ReferringPhysicianName) );
    QString studyDescription(dataset.GetElementAsString(DCM_StudyDescription) );

    QSqlQuery insertStudyStatement(this->Database);
    insertStudyStatement.prepare("INSERT INTO Studies "
      "( 'StudyInstanceUID', 'PatientsUID', 'StudyID', 'StudyDate', 'StudyTime', 'AccessionNumber', 'ModalitiesInStudy', 'InstitutionName', 'ReferringPhysician', 'PerformingPhysiciansName', "
        "'StudyDescription', 'InsertTimestamp', 'DisplayedNumberOfSeries', 'DisplayedFieldsUpdatedTimestamp' ) "
      "VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, NULL, NULL)");
    insertStudyStatement.addBindValue(studyInstanceUID);
    insertStudyStatement.addBindValue(dbPatientID);
    insertStudyStatement.addBindValue(studyID);
    insertStudyStatement.addBindValue(QDate::fromString(studyDate, "yyyyMMdd"));
    insertStudyStatement.addBindValue(studyTime);
    insertStudyStatement.addBindValue(accessionNumber);
    insertStudyStatement.addBindValue(modalitiesInStudy);
    insertStudyStatement.addBindValue(institutionName);
    insertStudyStatement.addBindValue(referringPhysician );
    insertStudyStatement.addBindValue(performingPhysiciansName);
    insertStudyStatement.addBindValue(studyDescription);
    insertStudyStatement.addBindValue(QDateTime::currentDateTime());
    if (!insertStudyStatement.exec())
    {
      logger.error("Error executing statement: " + insertStudyStatement.lastQuery() + " Error: " + insertStudyStatement.lastError().text() );
      return false;
    }
    else
    {
      this->InsertedStudyUIDsCache.insert(studyInstanceUID);
    }

    return true;
  }
  else
  {
    logger.debug("Used existing study: " + studyInstanceUID);
    this->InsertedStudyUIDsCache.insert(studyInstanceUID);
    return false;
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::insertSeries(const ctkDICOMItem& dataset, const QString& studyInstanceUID)
{
  QString seriesInstanceUID(dataset.GetElementAsString(DCM_SeriesInstanceUID) );
  QSqlQuery checkSeriesExistsQuery(this->Database);
  checkSeriesExistsQuery.prepare( "SELECT * FROM Series WHERE SeriesInstanceUID = ?" );
  checkSeriesExistsQuery.bindValue( 0, seriesInstanceUID );
  logger.debug("Statement: " + checkSeriesExistsQuery.lastQuery() );
  if (!loggedExec(checkSeriesExistsQuery))
  {
    return false;
  }
  if (!checkSeriesExistsQuery.next())
  {
    logger.debug("Need to insert new series: " + seriesInstanceUID);

    QString seriesDate(dataset.GetElementAsString(DCM_SeriesDate) );
    QString seriesTime(dataset.GetElementAsString(DCM_SeriesTime) );
    QString seriesDescription(dataset.GetElementAsString(DCM_SeriesDescription) );
    QString modality(dataset.GetElementAsString(DCM_Modality) );
    QString bodyPartExamined(dataset.GetElementAsString(DCM_BodyPartExamined) );
    QString frameOfReferenceUID(dataset.GetElementAsString(DCM_FrameOfReferenceUID) );
    QString contrastAgent(dataset.GetElementAsString(DCM_ContrastBolusAgent) );
    QString scanningSequence(dataset.GetElementAsString(DCM_ScanningSequence) );
    long seriesNumber(dataset.GetElementAsInteger(DCM_SeriesNumber) );
    long acquisitionNumber(dataset.GetElementAsInteger(DCM_AcquisitionNumber) );
    long echoNumber(dataset.GetElementAsInteger(DCM_EchoNumbers) );
    long temporalPosition(dataset.GetElementAsInteger(DCM_TemporalPositionIdentifier) );

    QSqlQuery insertSeriesStatement(this->Database);
    insertSeriesStatement.prepare( "INSERT INTO Series "
      "( 'SeriesInstanceUID', 'StudyInstanceUID', 'SeriesNumber', 'SeriesDate', 'SeriesTime', 'SeriesDescription', 'Modality', 'BodyPartExamined', "
        "'FrameOfReferenceUID', 'AcquisitionNumber', 'ContrastAgent', 'ScanningSequence', 'EchoNumber', 'TemporalPosition', 'InsertTimestamp' ) "
      "VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )" );
    insertSeriesStatement.addBindValue(seriesInstanceUID);
    insertSeriesStatement.addBindValue(studyInstanceUID);
    insertSeriesStatement.addBindValue(static_cast<int>(seriesNumber));
    insertSeriesStatement.addBindValue(QDate::fromString ( seriesDate, "yyyyMMdd" ));
    insertSeriesStatement.addBindValue(seriesTime);
    insertSeriesStatement.addBindValue(seriesDescription);
    insertSeriesStatement.addBindValue(modality);
    insertSeriesStatement.addBindValue(bodyPartExamined);
    insertSeriesStatement.addBindValue(frameOfReferenceUID);
    insertSeriesStatement.addBindValue(static_cast<int>(acquisitionNumber));
    insertSeriesStatement.addBindValue(contrastAgent);
    insertSeriesStatement.addBindValue(scanningSequence);
    insertSeriesStatement.addBindValue(static_cast<int>(echoNumber));
    insertSeriesStatement.addBindValue(static_cast<int>(temporalPosition));
    insertSeriesStatement.addBindValue(QDateTime::currentDateTime());
    if (!insertSeriesStatement.exec())
    {
      logger.error("Error executing statement: "
                     + insertSeriesStatement.lastQuery()
                     + " Error: " + insertSeriesStatement.lastError().text());
      return false;
    }
    else
    {
      this->InsertedSeriesUIDsCache.insert(seriesInstanceUID);
    }

    return true;
  }
  else
  {
    logger.debug("Used existing series: " + seriesInstanceUID);
    this->InsertedSeriesUIDsCache.insert(seriesInstanceUID);
    return false;
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::openTagCacheDatabase()
{
  // try to open the database if it's not already open
  if ( this->TagCacheDatabase.isOpen() )
  {
    return true;
  }
  QString tagCacheConnectionName = this->Database.connectionName() + "TagCache";
  if (QSqlDatabase::contains(tagCacheConnectionName))
  {
    QSqlDatabase::removeDatabase(tagCacheConnectionName);
  }

  this->TagCacheDatabase = QSqlDatabase::addDatabase(
        "QSQLITE", tagCacheConnectionName);
  this->TagCacheDatabase.setDatabaseName(this->TagCacheDatabaseFilename);
  if ( !this->TagCacheDatabase.open() )
  {
    qDebug() << "TagCacheDatabase would not open!\n";
    qDebug() << "TagCacheDatabaseFilename is: " << this->TagCacheDatabaseFilename << "\n";
    return false;
  }

  // Disable synchronous writing to make modifications faster
  QSqlQuery pragmaSyncQuery(this->TagCacheDatabase);
  pragmaSyncQuery.exec("PRAGMA synchronous = OFF");
  pragmaSyncQuery.finish();

  return true;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::precacheTags(const ctkDICOMItem& dataset, const QString sopInstanceUID)
{
  Q_Q(ctkDICOMDatabase);

  QStringList sopInstanceUIDs, tags, values;
  foreach (const QString &tag, this->TagsToPrecache)
  {
    QString upperTag = tag.toUpper();
    unsigned short group, element;
    q->tagToGroupElement(upperTag, group, element);
    DcmTagKey tagKey(group, element);
    QString value;
    if (this->TagsToExcludeFromStorage.contains(upperTag))
    {
      if (dataset.TagExists(tagKey))
      {
        value = ValueIsNotStored;
      }
      else
      {
        value = TagNotInInstance;
      }
    }
    else
    {
      value = dataset.GetAllElementValuesAsString(tagKey);
    }

    sopInstanceUIDs << sopInstanceUID;
    tags << upperTag;
    values << value;
  }

  q->cacheTags(sopInstanceUIDs, tags, values);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::removeImage(const QString& sopInstanceUID)
{
  QSqlQuery deleteFile(Database);
  deleteFile.prepare("DELETE FROM Images WHERE SOPInstanceUID == :sopInstanceUID");
  deleteFile.bindValue(":sopInstanceUID", sopInstanceUID);
  bool success = deleteFile.exec();
  if (!success)
  {
    logger.error("SQLITE ERROR deleting old image row: " + deleteFile.lastError().driverText());
  }
  return success;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabasePrivate::internalStoragePath(const QString& studyInstanceUID,
  const QString& seriesInstanceUID, const QString& sopInstanceUID)
{
  QString studyComponent;
  QString seriesComponent;
  QString instanceComponent;
  if (this->UseShortStoragePath)
  {
    // MD5 hash is chosen because it is short (its hex digest is 16 characters) and we do not need cryptographically strong hash.
    // It is not a problem if clash occurs in study or series folders (it would just mean that multiple studies or series would be stored in a folder),
    // therefore we just use the first 8 characters of the digest.
    // Since in a series there are typically a few hundred, maybe a few thousand files, the chances that there are two different SOP instance UIDs
    // with the same hash is practically impossible.
    studyComponent = QString(QCryptographicHash::hash(studyInstanceUID.toUtf8(), QCryptographicHash::Md5).toHex()).left(8);
    seriesComponent = QString(QCryptographicHash::hash(seriesInstanceUID.toUtf8(), QCryptographicHash::Md5).toHex()).left(8);
    instanceComponent = QString(QCryptographicHash::hash(sopInstanceUID.toUtf8(), QCryptographicHash::Md5).toHex());
  }
  else
  {
    // Use original IDs (each may be 40-60 characters)
    studyComponent = studyInstanceUID;
    seriesComponent = seriesInstanceUID;
    instanceComponent = sopInstanceUID;
  }
  QString path = studyComponent + "/" + seriesComponent + "/" + instanceComponent;
  return path;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::storeDatasetFile(const ctkDICOMItem& dataset, const QString& originalFilePath,
  const QString& studyInstanceUID, const QString& seriesInstanceUID, const QString& sopInstanceUID,
  QString& storedFilePath)
{
  Q_Q(ctkDICOMDatabase);

  if (sopInstanceUID.isEmpty())
  {
    storedFilePath.clear();
    return false;
  }

  storedFilePath = q->databaseDirectory() + "/dicom/"
    + this->internalStoragePath(studyInstanceUID, seriesInstanceUID, sopInstanceUID) + ".dcm";

  QDir destinationDir(QFileInfo(storedFilePath).dir());
  if (!destinationDir.exists())
  {
    destinationDir.mkpath(".");
  }

  if (originalFilePath.isEmpty())
  {
    logger.debug("Saving file: " + storedFilePath);
    if (!dataset.SaveToFile(storedFilePath))
    {
      logger.error("Error saving file: " + storedFilePath);
      return false;
    }
  }
  else
  {
    // we're inserting an existing file
    QFile currentFile(originalFilePath);
    currentFile.copy(storedFilePath);
    logger.debug("Copy file from: " + originalFilePath + " to: " + storedFilePath);
  }

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::indexingStatusForFile(const QString& filePath, const QString& sopInstanceUID,
  bool& datasetInDatabase, bool& datasetUpToDate, QString& databaseFilename)
{
  datasetInDatabase = false;
  datasetUpToDate = false;
  databaseFilename.clear();

  QSqlQuery fileExistsQuery(Database);
  fileExistsQuery.prepare("SELECT InsertTimestamp,Filename FROM Images WHERE SOPInstanceUID == :sopInstanceUID");
  fileExistsQuery.bindValue(":sopInstanceUID", sopInstanceUID);
  bool success = fileExistsQuery.exec();
  if (!success)
  {
    logger.error("SQLITE ERROR: " + fileExistsQuery.lastError().driverText());
    return false;
  }
  bool foundSOPInstanceUID = fileExistsQuery.next();
  if (!foundSOPInstanceUID)
  {
    // this data set is not in the database yet
    return true;
  }

  datasetInDatabase = true;

  // The SOP instance UID exists in the database. In theory, new SOP instance UID must be generated if
  // a file is modified, but some software may not respect this, so check if the file was modified.
  databaseFilename = fileExistsQuery.value(1).toString();
  QFileInfo databaseFileInfo(databaseFilename);
  if (!databaseFileInfo.isRelative())
  {
    // database stores a link to an external file, if it is the same filename and the file has not changed
    // since insertion date then it means that the dataset is up-to-date
    QDateTime fileLastModified(databaseFileInfo.lastModified());
    QDateTime databaseInsertTimestamp(QDateTime::fromString(fileExistsQuery.value(0).toString(), Qt::ISODate));
    // Compare QFileInfo objects instead of path strings to ensure equivalent file names
    // (such as same file name in uppercase/lowercase on Windows) are considered as equal.
    if (databaseFileInfo == QFileInfo(filePath) && fileLastModified < databaseInsertTimestamp)
    {
      // this file is already added and database is up-to-date
      datasetUpToDate = true;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::insertPatientStudySeries(const ctkDICOMItem& dataset,
  const QString& patientID, const QString& patientsName, const QString& connectionName)
{
  Q_Q(ctkDICOMDatabase);
  bool databaseWasChanged = false;

  // Insert new patient if needed
  // Generate composite patient ID
  QString patientsBirthDate(dataset.GetElementAsString(DCM_PatientBirthDate));
  QString compositePatientId = ctkDICOMDatabase::compositePatientID(patientID, patientsName, patientsBirthDate);
  // The dbPatientID  is a unique number within the database, generated by the sqlite autoincrement.
  // The patientID  is the (non-unique) DICOM patient id.
  QMap<QString, int>::iterator dbPatientIDit =
    this->InsertedPatientsCompositeIDCache.find(compositePatientId);
  int dbPatientID = -1;
  if (dbPatientIDit != this->InsertedPatientsCompositeIDCache.end())
  {
    // already in database
    dbPatientID = *dbPatientIDit;
  }
  else
  {
    logger.debug("Insert new patient if not already in database: " + patientID + " " + patientsName);
    if (this->insertPatient(dataset, patientID, patientsName, dbPatientID))
    {
      databaseWasChanged = true;
      emit q->patientAdded(dbPatientID, patientID, patientsName, patientsBirthDate);
    }
  }

  QMap<int, QStringList>::iterator dbConnectionsit =
    this->InsertedConnectionsIDCache.find(dbPatientID);
  QStringList connections;
  if (dbConnectionsit != this->InsertedConnectionsIDCache.end())
  {
    connections = dbConnectionsit.value();
  }
  if (connections.count() == 0 || !connections.contains(connectionName))
  {
    logger.debug("Insert new connection name if not already in database: " + connectionName);
    if (this->insertConnectionName(dbPatientID, connectionName))
    {
      databaseWasChanged = true;
      emit q->connectionNameAdded(dbPatientID, patientID, patientsName, patientsBirthDate, connectionName);
    }
  }

  logger.debug("Going to insert this instance with dbPatientID: " + QString::number(dbPatientID));

  // Insert new study if needed
  QString studyInstanceUID(dataset.GetElementAsString(DCM_StudyInstanceUID));
  if (!this->InsertedStudyUIDsCache.contains(studyInstanceUID))
  {
    if (this->insertStudy(dataset, dbPatientID))
    {
      logger.debug("Study Added");
      databaseWasChanged = true;
      // let users of this class track when things happen
      emit q->studyAdded(studyInstanceUID);
    }
  }

  QString seriesInstanceUID(dataset.GetElementAsString(DCM_SeriesInstanceUID));
  if (!seriesInstanceUID.isEmpty() && !this->InsertedSeriesUIDsCache.contains(seriesInstanceUID))
  {
    if (this->insertSeries(dataset, studyInstanceUID))
    {
      logger.debug("Series Added");
      databaseWasChanged = true;
      emit q->seriesAdded(seriesInstanceUID);
    }
  }

  return databaseWasChanged;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::storeThumbnailFile(const QString& originalFilePath,
  const QString& studyInstanceUID, const QString& seriesInstanceUID, const QString& sopInstanceUID)
{
  Q_Q(ctkDICOMDatabase);
  if (!this->ThumbnailGenerator)
  {
    return false;
  }
  // Create thumbnail here
  QString thumbnailPath = q->databaseDirectory() +
    "/thumbs/" + this->internalStoragePath(studyInstanceUID, seriesInstanceUID, sopInstanceUID) + ".png";
  QFileInfo thumbnailInfo(thumbnailPath);
  if (thumbnailInfo.exists() && (thumbnailInfo.lastModified() > QFileInfo(originalFilePath).lastModified()))
  {
    // thumbnail already exists and up-to-date
    return true;
  }
  QDir destinationDir(thumbnailInfo.dir());
  if (!destinationDir.exists())
  {
    destinationDir.mkpath(".");
  }
  DicomImage dcmImage(QDir::toNativeSeparators(originalFilePath).toUtf8());
  return this->ThumbnailGenerator->generateThumbnail(&dcmImage, thumbnailPath);
}


//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::uidsForDataSet(const ctkDICOMItem& dataset,
  QString& patientsName, QString& patientID, QString& studyInstanceUID, QString& seriesInstanceUID)
{
  // If the following fields can not be evaluated, cancel evaluation of the DICOM file
  patientsName = dataset.GetElementAsString(DCM_PatientName);
  patientID = dataset.GetElementAsString(DCM_PatientID);
  studyInstanceUID = dataset.GetElementAsString(DCM_StudyInstanceUID);
  seriesInstanceUID = dataset.GetElementAsString(DCM_SeriesInstanceUID);
  return this->uidsForDataSet(patientsName, patientID, studyInstanceUID);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::uidsForDataSet(QString& patientsName, QString& patientID, QString& studyInstanceUID)
{
  if (patientID.isEmpty() && !studyInstanceUID.isEmpty())
  {
    // Use study instance uid as patient id if patient id is empty - can happen on anonymized datasets
    // see: http://www.na-mic.org/Bug/view.php?id=2040
    logger.warn(QString("Patient ID is empty, using studyInstanceUID (%1) as patient ID").arg(studyInstanceUID));
    patientID = studyInstanceUID;
  }
  if (patientsName.isEmpty() && !patientID.isEmpty())
  {
    // Use patient id as name if name is empty - can happen on anonymized datasets
    // see: http://www.na-mic.org/Bug/view.php?id=1643
    patientsName = patientID;
  }
  // We accept the dataset without seriesInstanceUID, as query/retrieve result may not contain it
  if (patientsName.isEmpty() || studyInstanceUID.isEmpty() || patientID.isEmpty())
  {
    logger.error("Required information (patient name, patient ID, study instance UID) is missing from dataset");
    return false;
  }
  // Valid data set
  return true;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::insert(const ctkDICOMItem& dataset, const QString& filePath, bool storeFile, bool generateThumbnail)
{
  Q_Q(ctkDICOMDatabase);

  // this is the method that all other insert signatures end up calling
  // after they have pre-parsed their arguments

  QString sopInstanceUID(dataset.GetElementAsString(DCM_SOPInstanceUID));

  // Check to see if the file has already been loaded
  // Check if the file has been already indexed and skip indexing if it is
  bool datasetInDatabase = false;
  bool datasetUpToDate = false;
  QString databaseFilename;

  if (!indexingStatusForFile(filePath, sopInstanceUID, datasetInDatabase, datasetUpToDate, databaseFilename))
  {
    // error occurred, message is already logged
    return;
  }

  if (datasetInDatabase)
  {
    if (datasetUpToDate)
    {
      logger.debug("File " + databaseFilename + " already added");
      return;
    }
    // File is updated, delete record and re-index
    if (!this->removeImage(sopInstanceUID))
    {
      logger.debug("File " + filePath + " cannot be added, failed to update existing values in the database");
      return;
    }
  }

  // Verify that minimum required fields are present
  QString patientsName, patientID, studyInstanceUID, seriesInstanceUID;
  if (!this->uidsForDataSet(dataset, patientsName, patientID, studyInstanceUID, seriesInstanceUID))
  {
    // error occurred, message is already logged
    return;
  }

  // Store a copy of the dataset
  QString storedFilePath = filePath;
  if (storeFile && !seriesInstanceUID.isEmpty() && !q->isInMemory())
  {
    if (!this->storeDatasetFile(dataset, filePath, studyInstanceUID, seriesInstanceUID, sopInstanceUID, storedFilePath))
    {
      logger.error("Error saving file: " + filePath);
      return;
    }
  }

  bool databaseWasChanged = this->insertPatientStudySeries(dataset, patientID, patientsName);
  if (!sopInstanceUID.isEmpty() && !seriesInstanceUID.isEmpty() && !storedFilePath.isEmpty())
  {
    bool alreadyInserted = false;
    if (!storeFile)
    {
      // file is linked, maybe it is already inserted
      QSqlQuery checkImageExistsQuery(Database);
      checkImageExistsQuery.prepare("SELECT * FROM Images WHERE SOPInstanceUID = ?");
      checkImageExistsQuery.addBindValue(sopInstanceUID);
      if (!loggedExec(checkImageExistsQuery))
      {
        return;
      }
      alreadyInserted = checkImageExistsQuery.next();
    }
    if (!alreadyInserted)
    {
      // Get filename that will be stored in the database.
      // Use relative path if a copy is stored in the database to make the database relocatable.
      QString storedFilePathInDatabase;
      if (storeFile)
      {
        QDir databaseDirectory(q->databaseDirectory());
        storedFilePathInDatabase = databaseDirectory.relativeFilePath(storedFilePath);
      }
      else
      {
        storedFilePathInDatabase = storedFilePath;
      }

      QSqlQuery insertImageStatement(Database);
      insertImageStatement.prepare("INSERT INTO Images ( 'SOPInstanceUID', 'Filename', 'URL', 'SeriesInstanceUID', 'InsertTimestamp' ) VALUES ( ?, ?, ?, ?, ? )");
      insertImageStatement.addBindValue(sopInstanceUID);
      insertImageStatement.addBindValue(storedFilePathInDatabase);
      insertImageStatement.addBindValue(QString(""));
      insertImageStatement.addBindValue(seriesInstanceUID);
      insertImageStatement.addBindValue(QDateTime::currentDateTime());

      if (!insertImageStatement.exec())
      {
        logger.error("Error executing statement: "
                     + insertImageStatement.lastQuery()
                     + " Error: " + insertImageStatement.lastError().text());
      }
      else
      {
        // insert was needed, so cache any application-requested tags
        this->precacheTags(dataset, sopInstanceUID);
      }

      // let users of this class track when things happen
      emit q->instanceAdded(sopInstanceUID);

      databaseWasChanged = true;
    }
    if (generateThumbnail)
    {
      this->storeThumbnailFile(storedFilePath, studyInstanceUID, seriesInstanceUID, sopInstanceUID);
    }
  }
  if (q->isInMemory() && databaseWasChanged)
  {
    emit q->databaseChanged();
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabasePrivate::getDisplayPatientFieldsKey(const QString& patientID,
  const QString& patientsName, const QString& patientsBirthDate,
  QMap<QString, QMap<QString, QString> >& displayedFieldsMapPatient)
{
  QString compositeID = ctkDICOMDatabase::compositePatientID(patientID, patientsName, patientsBirthDate);

  // Look for the patient in the displayed fields cache first
  if (displayedFieldsMapPatient.find(compositeID) != displayedFieldsMapPatient.end())
  {
    return compositeID;
  }

  // Look for the patient in the display database
  QSqlQuery displayPatientsQuery(this->Database);
  displayPatientsQuery.prepare( "SELECT * FROM Patients WHERE PatientID = :patientID AND PatientsName = :patientsName ;" );
  displayPatientsQuery.bindValue(":patientID", patientID);
  displayPatientsQuery.bindValue(":patientsName", patientsName);
  if (!displayPatientsQuery.exec())
  {
    logger.error("SQLITE ERROR: " + displayPatientsQuery.lastError().driverText());
    return QString();
  }
  if (displayPatientsQuery.size() > 1)
  {
    logger.warn("Multiple patients found with PatientsName=" + patientsName + " and PatientID=" + patientID);
  }
  if (displayPatientsQuery.next())
  {
    QSqlRecord patientRecord = displayPatientsQuery.record();
    QMap<QString, QString> patientFieldsMap;
    for (int fieldIndex=0; fieldIndex<patientRecord.count(); ++fieldIndex)
    {
      patientFieldsMap.insert(patientRecord.fieldName(fieldIndex), patientRecord.value(fieldIndex).toString());
    }
    displayedFieldsMapPatient[compositeID] = patientFieldsMap;
    return compositeID;
  }

  logger.error("Failed to find patient with PatientsName=" + patientsName + " and PatientID=" + patientID);
  return QString();
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabasePrivate::getDisplayStudyFieldsKey(QString studyInstanceUID, QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy)
{
  // Look for the study in the displayed fields cache first
  foreach (QString currentStudyInstanceUid, displayedFieldsMapStudy.keys())
  {
    if ( !displayedFieldsMapStudy[currentStudyInstanceUid]["StudyInstanceUID"].compare(studyInstanceUID) )
    {
      return studyInstanceUID;
    }
  }

  // Look for the study in the display database
  QSqlQuery displayStudiesQuery(this->Database);
  displayStudiesQuery.prepare( QString("SELECT StudyInstanceUID FROM Studies WHERE StudyInstanceUID = :studyInstanceUID ;") );
  displayStudiesQuery.bindValue(":studyInstanceUID", studyInstanceUID);
  if (!displayStudiesQuery.exec())
  {
    logger.error("SQLITE ERROR: " + displayStudiesQuery.lastError().driverText());
    return QString();
  }
  if (displayStudiesQuery.size() > 1)
  {
    logger.warn("Multiple studies found with StudyInstanceUID=" + studyInstanceUID);
  }
  if (displayStudiesQuery.next())
  {
    QSqlRecord studyRecord = displayStudiesQuery.record();
    QMap<QString, QString> studyFieldsMap;
    for (int fieldIndex=0; fieldIndex<studyRecord.count(); ++fieldIndex)
    {
      studyFieldsMap.insert(studyRecord.fieldName(fieldIndex), studyRecord.value(fieldIndex).toString());
    }
    displayedFieldsMapStudy.insert(studyInstanceUID, studyFieldsMap);
    return studyInstanceUID;
  }

  logger.error("Failed to find study with StudyInstanceUID=" + studyInstanceUID);
  return QString();
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabasePrivate::getDisplaySeriesFieldsKey(QString seriesInstanceUID, QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries)
{
  // Look for the series in the displayed fields cache first
  foreach (QString currentSeriesInstanceUid, displayedFieldsMapSeries.keys())
  {
    if ( !displayedFieldsMapSeries[currentSeriesInstanceUid]["SeriesInstanceUID"].compare(seriesInstanceUID) )
    {
      return seriesInstanceUID;
    }
  }

  // Look for the series in the display database
  QSqlQuery displaySeriesQuery(this->Database);
  displaySeriesQuery.prepare( QString("SELECT SeriesInstanceUID FROM Series WHERE SeriesInstanceUID = :seriesInstanceUID ;") );
  displaySeriesQuery.bindValue(":seriesInstanceUID", seriesInstanceUID);
  if (!displaySeriesQuery.exec())
  {
    logger.error("SQLITE ERROR: " + displaySeriesQuery.lastError().driverText());
    return QString();
  }
  if (displaySeriesQuery.size() > 1)
  {
    logger.warn("Multiple series found with SeriesInstanceUID=" + seriesInstanceUID);
  }
  if (displaySeriesQuery.next())
  {
    QSqlRecord seriesRecord = displaySeriesQuery.record();
    QMap<QString, QString> seriesFieldsMap;
    for (int fieldIndex=0; fieldIndex<seriesRecord.count(); ++fieldIndex)
    {
      seriesFieldsMap.insert(seriesRecord.fieldName(fieldIndex), seriesRecord.value(fieldIndex).toString());
    }
    displayedFieldsMapSeries.insert(seriesInstanceUID, seriesFieldsMap);
    return seriesInstanceUID;
  }

  logger.error("in getDisplaySeriesFieldsKey: Failed to find series with SeriesInstanceUID=" + seriesInstanceUID);
  return QString();
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::applyDisplayedFieldsChanges( QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
                                                           QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
                                                           QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient)
{
  QMap<QString, int> patientCompositeIdToPatientUidMap;

  // Update patient fields

  foreach (QString compositeID, displayedFieldsMapPatient.keys())
  {
    if (compositeID.isEmpty())
    {
      continue;
    }

    QMap<QString, QString> currentPatient = displayedFieldsMapPatient[compositeID];
    if (currentPatient["PatientID"].isEmpty() || currentPatient["PatientsName"].isEmpty())
    {
      logger.error("Unable to locate the patient due to missing values for PatientsName and/or PatientID. "
                   "If this error recurs, please manually remove the patient from the database, ensuring to"
                   " address the missing PatientsName/PatientID.");
      continue;
    }

    QSqlQuery displayPatientsQuery(this->Database);
    displayPatientsQuery.prepare( "SELECT * FROM Patients WHERE PatientID=:patientID AND PatientsName=:patientsName ;" );
    displayPatientsQuery.bindValue(":patientID", currentPatient["PatientID"]);
    displayPatientsQuery.bindValue(":patientsName", currentPatient["PatientsName"]);
    if (!displayPatientsQuery.exec())
    {
      logger.error("SQLITE ERROR: " + displayPatientsQuery.lastError().driverText());
      return false;
    }
    if (displayPatientsQuery.next())
    {
      QString displayPatientsFieldUpdateString;
      QList<QString> boundValues;
      foreach (QString tagName, currentPatient.keys())
      {
        if (tagName == "PatientCompositeID")
        {
          continue; // Do not write patient index that is only used internally and temporarily
        }
        displayPatientsFieldUpdateString.append( tagName + " = ? , " );
        boundValues << currentPatient[tagName];
      }

      // Trim the separators from the end
      displayPatientsFieldUpdateString = displayPatientsFieldUpdateString.left(displayPatientsFieldUpdateString.size() - 3);

      QSqlRecord patientRecord = displayPatientsQuery.record();
      int patientUID = patientRecord.value("UID").toInt();

      QSqlQuery updateDisplayPatientStatement(this->Database);
      updateDisplayPatientStatement.prepare( QString("UPDATE Patients SET %1 WHERE UID = ? ;").arg(displayPatientsFieldUpdateString) );
      foreach (QString boundValue, boundValues)
      {
        updateDisplayPatientStatement.addBindValue(boundValue);
      }
      updateDisplayPatientStatement.addBindValue(patientUID);
      this->loggedExec(updateDisplayPatientStatement);

      QSqlQuery updateDisplayedFieldsUpdatedTimestampStatement(this->Database);
      updateDisplayedFieldsUpdatedTimestampStatement.prepare("UPDATE Patients SET DisplayedFieldsUpdatedTimestamp=CURRENT_TIMESTAMP WHERE UID = ? ;");
      updateDisplayedFieldsUpdatedTimestampStatement.addBindValue(patientUID);
      this->loggedExec(updateDisplayedFieldsUpdatedTimestampStatement);

      patientCompositeIdToPatientUidMap[compositeID] = patientUID;
    }
    else
    {
      logger.error("Failed to find patient with PatientsName=" + currentPatient["PatientsName"] + " and PatientID=" + currentPatient["PatientID"]);
      continue;
    }
  } // For each patient in displayedFieldsVectorPatient

  // Update study fields
  foreach (QString currentStudyInstanceUid, displayedFieldsMapStudy.keys())
  {
    if (currentStudyInstanceUid.isEmpty())
    {
      continue;
    }
    QMap<QString, QString> currentStudy = displayedFieldsMapStudy[currentStudyInstanceUid];
    QSqlQuery displayStudiesQuery(this->Database);
    displayStudiesQuery.prepare("SELECT StudyInstanceUID FROM Studies WHERE StudyInstanceUID = ? ;");
    displayStudiesQuery.addBindValue(currentStudyInstanceUid);
    if (!displayStudiesQuery.exec())
    {
      logger.error("SQLITE ERROR: " + displayStudiesQuery.lastError().driverText());
      return false;
    }
    if (displayStudiesQuery.next())
    {
      QString displayStudiesFieldUpdateString;
      QList<QString> boundValues;
      foreach (QString tagName, currentStudy.keys())
      {
        if (!tagName.compare("PatientCompositeID"))
        {
          displayStudiesFieldUpdateString.append( "PatientsUID = ? , " );
          boundValues << QString::number(patientCompositeIdToPatientUidMap[currentStudy["PatientCompositeID"]]);
        }
        else
        {
          displayStudiesFieldUpdateString.append( tagName + " = ? , " );
          boundValues << currentStudy[tagName];
        }
      }
      // Trim the separators from the end
      displayStudiesFieldUpdateString = displayStudiesFieldUpdateString.left(displayStudiesFieldUpdateString.size() - 3);

      QSqlQuery updateDisplayStudyStatement(this->Database);
      updateDisplayStudyStatement.prepare( QString("UPDATE Studies SET %1 WHERE StudyInstanceUID = ? ;").arg(displayStudiesFieldUpdateString) );
      foreach (QString boundValue, boundValues)
      {
        updateDisplayStudyStatement.addBindValue(boundValue);
      }
      updateDisplayStudyStatement.addBindValue(currentStudy["StudyInstanceUID"]);
      this->loggedExec(updateDisplayStudyStatement);

      QSqlQuery updateDisplayedFieldsUpdatedTimestampStatement(this->Database);
      updateDisplayedFieldsUpdatedTimestampStatement.prepare("UPDATE Studies SET DisplayedFieldsUpdatedTimestamp=CURRENT_TIMESTAMP WHERE StudyInstanceUID = ? ;");
      updateDisplayedFieldsUpdatedTimestampStatement.addBindValue(currentStudy["StudyInstanceUID"]);
      this->loggedExec(updateDisplayedFieldsUpdatedTimestampStatement);
    }
    else
    {
      logger.error("in applyDisplayedFieldsChanges: Failed to find study with StudyInstanceUID=" + currentStudyInstanceUid);
      continue;
    }
  } // For each study in displayedFieldsMapStudy

  // Update series fields
  foreach (QString currentSeriesInstanceUid, displayedFieldsMapSeries.keys())
  {
    if (currentSeriesInstanceUid.isEmpty())
    {
      continue;
    }
    // Insert row into Series if does not exist
    QMap<QString, QString> currentSeries = displayedFieldsMapSeries[currentSeriesInstanceUid];
    QSqlQuery displaySeriesQuery(this->Database);
    displaySeriesQuery.prepare("SELECT SeriesInstanceUID FROM Series WHERE SeriesInstanceUID = ? ;");
    displaySeriesQuery.addBindValue(currentSeriesInstanceUid);
    if (!displaySeriesQuery.exec())
    {
      logger.error("SQLITE ERROR: " + displaySeriesQuery.lastError().driverText());
      return false;
    }
    if (displaySeriesQuery.next())
    {
      QString displaySeriesFieldUpdateString;
      QList<QString> boundValues;
      foreach (QString tagName, currentSeries.keys())
      {
        displaySeriesFieldUpdateString.append( tagName + " = ? , " );
        boundValues << currentSeries[tagName];
      }
      // Trim the separators from the end
      displaySeriesFieldUpdateString = displaySeriesFieldUpdateString.left(displaySeriesFieldUpdateString.size() - 3);

      QSqlQuery updateDisplaySeriesStatement(this->Database);
      updateDisplaySeriesStatement.prepare( QString("UPDATE Series SET %1 WHERE SeriesInstanceUID = ? ;").arg(displaySeriesFieldUpdateString) );
      foreach (QString boundValue, boundValues)
      {
        updateDisplaySeriesStatement.addBindValue(boundValue);
      }
      updateDisplaySeriesStatement.addBindValue(currentSeries["SeriesInstanceUID"]);
      this->loggedExec(updateDisplaySeriesStatement);

      QSqlQuery updateDisplayedFieldsUpdatedTimestampStatement(this->Database);
      updateDisplayedFieldsUpdatedTimestampStatement.prepare("UPDATE Series SET DisplayedFieldsUpdatedTimestamp=CURRENT_TIMESTAMP WHERE SeriesInstanceUID = ? ;");
      updateDisplayedFieldsUpdatedTimestampStatement.addBindValue(currentSeries["SeriesInstanceUID"]);
      this->loggedExec(updateDisplayedFieldsUpdatedTimestampStatement);
    }
    else
    {
      logger.error("in applyDisplayedFieldsChanges: Failed to find series with SeriesInstanceUID=" + currentSeriesInstanceUid);
      continue;
    }
  } // For each series in displayedFieldsMapSeries

  return true;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabasePrivate::absolutePathFromInternal(const QString& filename)
{
  Q_Q(ctkDICOMDatabase);
  return ctk::absolutePathFromInternal(filename, q->databaseDirectory());
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabasePrivate::internalPathFromAbsolute(const QString& filename)
{
  Q_Q(ctkDICOMDatabase);
  // Make it a relative path if it is within the database folder
  return ctk::internalPathFromAbsolute(filename, q->databaseDirectory());
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabasePrivate::jsonArrayToStringList(const QJsonArray& jsonArray)
{
  QStringList stringList;
  for (const QJsonValue& jsonValue : jsonArray)
  {
    if (jsonValue.isString())
    {
      stringList << jsonValue.toString();
    }
  }
  return stringList;
}

//------------------------------------------------------------------------------
QJsonArray ctkDICOMDatabasePrivate::stringListToJsonArray(const QStringList& stringList)
{
  QJsonArray jsonArray;
  for (const QString& str : stringList)
  {
    jsonArray.append(str);
  }
  return jsonArray;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabasePrivate::convertConnectionInfoToJson(const QStringList& allowList,
                                                             const QStringList& denyList)
{
  // Create a JSON object
  QJsonObject jsonObject;
  jsonObject["allow"] = this->stringListToJsonArray(allowList);
  jsonObject["deny"] = this->stringListToJsonArray(denyList);

  // Serialize the JSON object to a string
  QJsonDocument jsonDoc(jsonObject);
  return jsonDoc.toJson(QJsonDocument::Compact);
}

//------------------------------------------------------------------------------
CTK_GET_CPP(ctkDICOMDatabase, bool, isDisplayedFieldsTableAvailable, DisplayedFieldsTableAvailable);
CTK_GET_CPP(ctkDICOMDatabase, bool, useShortStoragePath, UseShortStoragePath);
CTK_SET_CPP(ctkDICOMDatabase, bool, setUseShortStoragePath, UseShortStoragePath);

//------------------------------------------------------------------------------
// ctkDICOMDatabase methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ctkDICOMDatabase::ctkDICOMDatabase(QString databaseFile)
  : d_ptr(new ctkDICOMDatabasePrivate(*this))
{
  Q_D(ctkDICOMDatabase);
  d->TagsToExcludeFromStorage << groupElementToTag(0x7FE0,0x0010); // pixel data
  d->registerCompressionLibraries();
  d->init(databaseFile);
}

ctkDICOMDatabase::ctkDICOMDatabase(QObject* parent)
  : d_ptr(new ctkDICOMDatabasePrivate(*this))
{
  Q_UNUSED(parent);
  Q_D(ctkDICOMDatabase);
  d->TagsToExcludeFromStorage << groupElementToTag(0x7FE0, 0x0010); // pixel data
  d->registerCompressionLibraries();
}

//------------------------------------------------------------------------------
ctkDICOMDatabase::~ctkDICOMDatabase()
{
  this->closeDatabase();
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::openDatabase(const QString& databaseFile,
                                    const QString& connectionName)
{
  Q_D(ctkDICOMDatabase);
  bool wasOpen = this->isOpen();
  d->DatabaseFileName = databaseFile;

  if (this->isInMemory())
  {
    d->DatabaseDirectory.clear();
  }
  else
  {
    QString databaseFileAbsolute = d->DatabaseFileName;
    if (!QFileInfo(databaseFileAbsolute).isAbsolute())
    {
      databaseFileAbsolute.prepend(QDir::currentPath() + "/");
    }
    d->DatabaseDirectory = QFileInfo(databaseFileAbsolute).absoluteDir().path();
  }

  QString verifiedConnectionName = connectionName;
  if (verifiedConnectionName.isEmpty())
  {
    verifiedConnectionName = QUuid::createUuid().toString();
  }

  if (QSqlDatabase::contains(verifiedConnectionName))
  {
    QSqlDatabase::removeDatabase(verifiedConnectionName);
  }

  d->Database = QSqlDatabase::addDatabase("QSQLITE", verifiedConnectionName);
  d->Database.setDatabaseName(databaseFile);
  if (!(d->Database.open()))
  {
    d->LastError = d->Database.lastError().text();
    if (wasOpen)
    {
      emit closed();
    }
    return false;
  }

  // Disable synchronous writing to make modifications faster
  QSqlQuery pragmaSyncQuery(d->Database);
  pragmaSyncQuery.exec("PRAGMA synchronous = OFF");
  pragmaSyncQuery.finish();

  if ( d->Database.tables().empty() )
  {
    if (!this->initializeDatabase())
    {
      d->LastError = QString("Unable to initialize DICOM database!");
      if (wasOpen)
      {
        emit closed();
      }
      return false;
    }
  }
  d->resetLastInsertedValues();

  d->DisplayedFieldsTableAvailable = d->Database.tables().contains("ColumnDisplayProperties");

  if (!isInMemory())
  {
    QFileSystemWatcher* watcher = new QFileSystemWatcher(QStringList(databaseFile),this);
    connect(watcher, SIGNAL(fileChanged(QString)),this, SIGNAL (databaseChanged()) );
  }

  // Set up the tag cache for use later
  QFileInfo fileInfo(d->DatabaseFileName);
  d->TagCacheDatabaseFilename = QString( fileInfo.dir().path() + "/ctkDICOMTagCache.sql" );
  d->TagCacheVerified = false;
  if ( !this->tagCacheExists() )
  {
    this->initializeTagCache();
  }

  // Add displayed field generator's required tags to the pre-cached list to make
  // displayed field updates fast.
  QStringList tags = this->tagsToPrecache();
  d->DisplayedFieldGenerator->setDatabase(this);
  tags << d->DisplayedFieldGenerator->getRequiredTags();
  tags.removeDuplicates();
  this->setTagsToPrecache(tags);

  emit opened();
  return true;
}

//------------------------------------------------------------------------------
const QString ctkDICOMDatabase::lastError() const {
  Q_D(const ctkDICOMDatabase);
  return d->LastError;
}

//------------------------------------------------------------------------------
const QString ctkDICOMDatabase::databaseFilename() const {
  Q_D(const ctkDICOMDatabase);
  return d->DatabaseFileName;
}

//------------------------------------------------------------------------------
const QString ctkDICOMDatabase::databaseDirectory() const {
  Q_D(const ctkDICOMDatabase);
  return d->DatabaseDirectory;
}

//------------------------------------------------------------------------------
const QSqlDatabase& ctkDICOMDatabase::database() const {
  Q_D(const ctkDICOMDatabase);
  return d->Database;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setThumbnailGenerator(ctkDICOMAbstractThumbnailGenerator *generator){
  Q_D(ctkDICOMDatabase);
  d->ThumbnailGenerator = generator;
}

//------------------------------------------------------------------------------
ctkDICOMAbstractThumbnailGenerator* ctkDICOMDatabase::thumbnailGenerator(){
  Q_D(const ctkDICOMDatabase);
  return d->ThumbnailGenerator;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::initializeDatabase(const char* sqlFileName/* = ":/dicom/dicom-schema.sql" */)
{
  Q_D(ctkDICOMDatabase);

  d->resetLastInsertedValues();

  // remove any existing schema info - this handles the case where an
  // old schema should be loaded for testing.
  QSqlQuery dropSchemaInfo(d->Database);
  d->loggedExec( dropSchemaInfo, QString("DROP TABLE IF EXISTS 'SchemaInfo';") );
  const bool r = d->executeScript(sqlFileName);
  emit databaseChanged();
  return r;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::schemaVersionLoaded()
{
  Q_D(ctkDICOMDatabase);
  /// look for the version info in the database
  QSqlQuery versionQuery(d->Database);
  if ( !d->loggedExec( versionQuery, QString("SELECT Version from SchemaInfo;") ) )
  {
    return QString("");
  }

  if (versionQuery.next())
  {
    return versionQuery.value(0).toString();
  }

  return QString("");
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setSchemaVersion(QString schemaVersion)
{
  Q_D(ctkDICOMDatabase);
  d->SchemaVersion = schemaVersion;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::schemaVersion()
{
  // When changing schema version:
  // * make sure this matches the Version value in the
  //   SchemaInfo table defined in Resources/dicom-schema.sql
  // * make sure the 'Images' contains a 'Filename' column
  //   so that the ctkDICOMDatabasePrivate::filenames method
  //   still works.
  Q_D(ctkDICOMDatabase);
  return d->SchemaVersion;
};

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::updateSchemaIfNeeded(
  const char* schemaFile/* = ":/dicom/dicom-schema.sql" */,
  const char* newDatabaseDir/* = nullptr*/)
{
  Q_UNUSED(newDatabaseDir);
  if ( schemaVersionLoaded() != schemaVersion() )
  {
    return this->updateSchema(schemaFile);
  }
  else
  {
    emit schemaUpdateStarted(0);
    emit schemaUpdated();
    return false;
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::updateSchema(
  const char* schemaFile/* = ":/dicom/dicom-schema.sql" */,
  const char* newDatabaseDir/* = nullptr*/)
{
  // backup filelist
  // reinit with the new schema
  // reinsert everything

  Q_D(ctkDICOMDatabase);
  QStringList allFiles = d->allFilesInDatabase();

  if (newDatabaseDir && strlen(newDatabaseDir) > 0)
  {
    // If needed, create database directory
    if (!QDir(newDatabaseDir).exists())
    {
      QDir().mkdir(newDatabaseDir);
    }

    // Close the active DICOM database
    this->closeDatabase();

    // Open new database so that the re-insertions can be done there
    try
    {
      QString databaseFileName = newDatabaseDir + QString("/ctkDICOM.sql");
      this->openDatabase(databaseFileName);
    }
    catch (const std::exception& e)
    {
      Q_UNUSED(e);
      std::cerr << "Database error: " << qPrintable(this->lastError()) << "\n";
      this->closeDatabase();
      return false;
    }
  }

  this->initializeDatabase(schemaFile);

  emit schemaUpdateStarted(allFiles.length());

  int progressValue = 0;
  foreach(QString file, allFiles)
  {
    emit schemaUpdateProgress(progressValue);
    emit schemaUpdateProgress(file);

    // TODO: use QFuture
    this->insert(file,false,false,true);

    progressValue++;
  }

  // Update displayed fields in the updated database
  emit displayedFieldsUpdateStarted();
  this->updateDisplayedFields();

  // TODO: check better that everything is ok
  emit schemaUpdated();
  return true;
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGenerator* ctkDICOMDatabase::displayedFieldGenerator() const
{
  Q_D(const ctkDICOMDatabase);
  return d->DisplayedFieldGenerator;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::closeDatabase()
{
  Q_D(ctkDICOMDatabase);
  bool wasOpen = this->isOpen();
  d->Database.close();
  d->TagCacheDatabase.close();
  if (wasOpen)
  {
    emit closed();
  }
}

//
// Patient/study/series convenience methods
//

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::patients()
{
  Q_D(ctkDICOMDatabase);
  QStringList result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT UID FROM Patients" );
  if (!d->loggedExec(query))
  {
    return result;
  }
  while (query.next())
  {
    result << query.value(0).toString();
  }
  return( result );
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::studiesForPatient(QString dbPatientID)
{
  Q_D(ctkDICOMDatabase);
  QStringList result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT StudyInstanceUID FROM Studies WHERE PatientsUID = ?" );
  query.addBindValue( dbPatientID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  while (query.next())
  {
    result << query.value(0).toString();
  }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::studyForSeries(QString seriesUID)
{
  Q_D(ctkDICOMDatabase);
  QString result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT StudyInstanceUID FROM Series WHERE SeriesInstanceUID= ?" );
  query.addBindValue( seriesUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::patientForStudy(QString studyUID)
{
  Q_D(ctkDICOMDatabase);
  QString result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT PatientsUID FROM Studies WHERE StudyInstanceUID= ?" );
  query.addBindValue( studyUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return( result );
}

//------------------------------------------------------------------------------
QHash<QString,QString> ctkDICOMDatabase::descriptionsForFile(QString fileName)
{
  Q_D(ctkDICOMDatabase);

  QString seriesUID(this->seriesForFile(fileName));
  QString studyUID(this->studyForSeries(seriesUID));
  QString patientID(this->patientForStudy(studyUID));

  QHash<QString,QString> result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT SeriesDescription FROM Series WHERE SeriesInstanceUID= ?" );
  query.addBindValue( seriesUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result["SeriesDescription"] =  query.value(0).toString();
  }
  query.prepare( "SELECT StudyDescription FROM Studies WHERE StudyInstanceUID= ?" );
  query.addBindValue( studyUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result["StudyDescription"] =  query.value(0).toString();
  }
  query.prepare( "SELECT PatientsName FROM Patients WHERE UID= ?" );
  query.addBindValue( patientID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result["PatientsName"] = query.value(0).toString();
  }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::descriptionForSeries(const QString seriesUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT SeriesDescription FROM Series WHERE SeriesInstanceUID= ?" );
  query.addBindValue( seriesUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::descriptionForStudy(const QString studyUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT StudyDescription FROM Studies WHERE StudyInstanceUID= ?" );
  query.addBindValue( studyUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::nameForPatient(const QString patientUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT PatientsName FROM Patients WHERE UID= ?" );
  query.addBindValue( patientUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return result;
}

//------------------------------------------------------------------------------
QMap<QString, QStringList> ctkDICOMDatabase::connectionsInformationForPatient(const QString patientUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;
  QMap<QString, QStringList> connectionsInformation;

  QSqlQuery query(d->Database);
  query.prepare("SELECT Connections FROM Patients WHERE UID= ?");
  query.addBindValue(patientUID);
  if (!d->loggedExec(query))
  {
    return connectionsInformation;
  }
  if (query.next())
  {
    result.append(query.value(0).toString());
  }

  // Parse the JSON string
  QJsonDocument jsonDoc = QJsonDocument::fromJson(result.toUtf8());
  if (!jsonDoc.isNull() && jsonDoc.isObject())
  {
    QJsonObject jsonObject = jsonDoc.object();
    connectionsInformation["allow"] = d->jsonArrayToStringList(jsonObject["allow"].toArray());
    connectionsInformation["deny"] = d->jsonArrayToStringList(jsonObject["deny"].toArray());
  }

  return connectionsInformation;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::updateConnectionsForPatient(const QString patientUID,
                                                   const QStringList allowList,
                                                   const QStringList denyList)
{
  Q_D(ctkDICOMDatabase);
  return d->updateConnections(patientUID, allowList, denyList);
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::displayedNameForPatient(const QString patientUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;

  QSqlQuery query(d->Database);
  query.prepare( "SELECT DisplayedPatientsName FROM Patients WHERE UID= ?" );
  query.addBindValue( patientUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }

  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fieldForPatient(const QString field, const QString patientUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;

  QSqlQuery query(d->Database);
  QString queryStr = QString("SELECT %1 FROM Patients WHERE UID= ?" ).arg(field);
  query.prepare(queryStr);
  query.addBindValue( patientUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }

  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fieldForStudy(const QString field, const QString studyInstanceUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;

  QSqlQuery query(d->Database);
  QString queryStr = QString("SELECT %1 FROM Studies WHERE StudyInstanceUID= ?" ).arg(field);
  query.prepare(queryStr);
  query.addBindValue( studyInstanceUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }

  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fieldForSeries(const QString field, const QString seriesInstanceUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;

  QSqlQuery query(d->Database);
  QString queryStr = QString("SELECT %1 FROM Series WHERE SeriesInstanceUID= ?" ).arg(field);
  query.prepare(queryStr);
  query.addBindValue( seriesInstanceUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }

  return result;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::patientFieldNames() const
{
  Q_D(const ctkDICOMDatabase);

  QStringList fieldNames;
  QSqlQuery query(d->Database);
  query.prepare("SELECT name FROM PRAGMA_TABLE_INFO('Patients')");
  if (!query.exec())
  {
    return fieldNames;
  }
  while (query.next())
  {
    fieldNames << query.value(0).toString();
  }
  return fieldNames;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::studyFieldNames() const
{
  Q_D(const ctkDICOMDatabase);

  QStringList fieldNames;
  QSqlQuery query(d->Database);
  query.prepare("SELECT name FROM PRAGMA_TABLE_INFO('Studies')");
  if (!query.exec())
  {
    return fieldNames;
  }
  while (query.next())
  {
    fieldNames << query.value(0).toString();
  }
  return fieldNames;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::seriesFieldNames() const
{
  Q_D(const ctkDICOMDatabase);

  QStringList fieldNames;
  QSqlQuery query(d->Database);
  query.prepare("SELECT name FROM PRAGMA_TABLE_INFO('Series')");
  if (!query.exec())
  {
    return fieldNames;
  }
  while (query.next())
  {
    fieldNames << query.value(0).toString();
  }
  return fieldNames;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::seriesForStudy(QString studyUID)
{
  Q_D(ctkDICOMDatabase);

  QStringList result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT SeriesInstanceUID FROM Series WHERE StudyInstanceUID=?");
  query.addBindValue( studyUID );
  if (!d->loggedExec(query))
  {
    return result;
  }
  while (query.next())
  {
    result << query.value(0).toString();
  }
  return( result );
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::instancesForSeries(const QString seriesUID, int hits/*=-1*/)
{
  Q_D(ctkDICOMDatabase);

  QStringList result;
  QSqlQuery query(d->Database);
  query.prepare("SELECT SOPInstanceUID FROM Images WHERE SeriesInstanceUID= ?");
  query.addBindValue(seriesUID);
  if (!d->loggedExec(query))
  {
    return result;
  }
  while (query.next())
  {
    result << query.value(0).toString();
    if (hits > 0 && result.size() >= hits)
    {
      // reached the number of requested instances
      break;
    }
  }
  return result;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::filesForSeries(QString seriesUID, int hits/*=-1*/)
{
  Q_D(ctkDICOMDatabase);

  QStringList allFileNames;
  QSqlQuery query(d->Database);
  query.prepare("SELECT Filename FROM Images WHERE SeriesInstanceUID=?");
  query.addBindValue(seriesUID);
  if (!d->loggedExec(query))
  {
    return allFileNames;
  }
  while (query.next())
  {
    QString fileName = query.value(0).toString();
    fileName = d->absolutePathFromInternal(fileName);
    allFileNames << fileName;
    if (hits > 0 && allFileNames.size() >= hits)
    {
      // reached the number of requested files
      break;
    }
  }
  return allFileNames;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::urlsForSeries(QString seriesUID, int hits/*=-1*/)
{
  Q_D(ctkDICOMDatabase);

  QStringList allURLs;
  QSqlQuery query(d->Database);
  query.prepare("SELECT URL FROM Images WHERE SeriesInstanceUID=?");
  query.addBindValue(seriesUID);
  if (!d->loggedExec(query))
  {
    return allURLs;
  }
  while (query.next())
  {
    QString url = query.value(0).toString();
    allURLs << url;
    if (hits > 0 && allURLs.size() >= hits)
    {
      // reached the number of requested files
      break;
    }
  }
  return allURLs;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fileForInstance(QString sopInstanceUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;
  QSqlQuery query(d->Database);
  query.prepare("SELECT Filename FROM Images WHERE SOPInstanceUID=?");
  query.addBindValue(sopInstanceUID);
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = d->absolutePathFromInternal(query.value(0).toString());
  }
  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::urlForInstance(QString sopInstanceUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;
  QSqlQuery query(d->Database);
  query.prepare("SELECT URL FROM Images WHERE SOPInstanceUID=?");
  query.addBindValue(sopInstanceUID);
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::seriesForFile(QString fileName)
{
  Q_D(ctkDICOMDatabase);

  QString result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT SeriesInstanceUID FROM Images WHERE Filename=?");
  query.addBindValue(d->internalPathFromAbsolute(fileName));
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::instanceForFile(QString fileName)
{
  Q_D(ctkDICOMDatabase);
  QString result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT SOPInstanceUID FROM Images WHERE Filename=?");
  query.addBindValue(d->internalPathFromAbsolute(fileName));
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::instanceForURL(QString url)
{
  Q_D(ctkDICOMDatabase);

  QString result;
  QSqlQuery query(d->Database);
  query.prepare( "SELECT SOPInstanceUID FROM Images WHERE URL=?");
  query.addBindValue(url);
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = query.value(0).toString();
  }
  return result;
}

//------------------------------------------------------------------------------
QDateTime ctkDICOMDatabase::insertDateTimeForInstance(QString sopInstanceUID)
{
  Q_D(ctkDICOMDatabase);
  QDateTime result;
  QSqlQuery query(d->Database);
  query.prepare("SELECT InsertTimestamp FROM Images WHERE SOPInstanceUID=?");
  query.addBindValue(sopInstanceUID);
  if (!d->loggedExec(query))
  {
    return result;
  }
  if (query.next())
  {
    result = QDateTime::fromString(query.value(0).toString(), Qt::ISODate);
  }
  return result;
}

//------------------------------------------------------------------------------
int ctkDICOMDatabase::patientsCount()
{
  Q_D(ctkDICOMDatabase);
  return d->rowCount("Patients");
}

//------------------------------------------------------------------------------
int ctkDICOMDatabase::studiesCount()
{
  Q_D(ctkDICOMDatabase);
  return d->rowCount("Studies");
}

//------------------------------------------------------------------------------
int ctkDICOMDatabase::seriesCount()
{
  Q_D(ctkDICOMDatabase);
  return d->rowCount("Series");
}

//------------------------------------------------------------------------------
int ctkDICOMDatabase::imagesCount()
{
  Q_D(ctkDICOMDatabase);
  return d->rowCount("Images");
}

//
// instance header methods
//

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::allFiles()
{
  Q_D(ctkDICOMDatabase);
  return d->filenames("Images");
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::loadInstanceHeader(QString sopInstanceUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare("SELECT Filename FROM Images WHERE SOPInstanceUID=?");
  query.addBindValue(sopInstanceUID);
  if (!d->loggedExec(query))
  {
    return;
  }
  if (query.next())
  {
    QString fileName = d->absolutePathFromInternal(query.value(0).toString());
    this->loadFileHeader(fileName);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::loadFileHeader(QString fileName)
{
  Q_D(ctkDICOMDatabase);
  d->LoadedHeader.clear();
  DcmFileFormat fileFormat;
  OFCondition status = fileFormat.loadFile(fileName.toUtf8().data());
  if (status.good())
  {
    DcmDataset *dataset = fileFormat.getDataset();
    DcmStack stack;
    while (dataset->nextObject(stack, true) == EC_Normal)
    {
      DcmObject *dO = stack.top();
      if (dO)
      {
        QString tag = QString("%1,%2").arg(dO->getGTag(),4,16,QLatin1Char('0')).arg(dO->getETag(),4,16,QLatin1Char('0'));
        tag = tag.toUpper();
        std::ostringstream s;
        dO->print(s);
        d->LoadedHeader[tag] = QString(s.str().c_str());
      }
    }
  }
  return;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::headerKeys ()
{
  Q_D(ctkDICOMDatabase);
  return (d->LoadedHeader.keys());
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::headerValue (QString key)
{
  Q_D(ctkDICOMDatabase);
  return (d->LoadedHeader[key]);
}

//
// instanceValue and fileValue methods
//

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::instanceValue(QString sopInstanceUID, QString tag)
{
  Q_D(ctkDICOMDatabase);
  tag = tag.toUpper();
  // Read from cache, if available
  QString value = this->cachedTag(sopInstanceUID, tag);
  if (value == TagNotInInstance || value == ValueIsEmptyString || value == ValueIsNotStored)
  {
    return "";
  }
  if (!value.isEmpty())
  {
    return value;
  }

  // Read value from file
  QString filePath = this->fileForInstance(sopInstanceUID);
  if (filePath.isEmpty())
  {
    return "";
  }
  value = d->readValueFromFile(filePath, sopInstanceUID, tag);
  return value;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::instanceValue(const QString sopInstanceUID, const unsigned short group, const unsigned short element)
{
  QString tag = this->groupElementToTag(group,element);
  return instanceValue(sopInstanceUID, tag);
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fileValue(const QString fileName, QString tag)
{
  Q_D(ctkDICOMDatabase);

  if (fileName.isEmpty())
  {
    return "";
  }

  // Read from cache, if available

  // first, try treating argument as filePath
  QString sopInstanceUID = this->instanceForFile(fileName);

  // second, try treating argument as a url
  bool isUrl = false;
  if (sopInstanceUID.isEmpty())
  {
    isUrl = true;
    sopInstanceUID = this->instanceForURL(fileName);
  }

  if (sopInstanceUID.isEmpty())
  {
    return "";
  }

  // third, look for the value
  tag = tag.toUpper();
  QString value = this->cachedTag(sopInstanceUID, tag);
  if (value == TagNotInInstance || value == ValueIsEmptyString || value == ValueIsNotStored)
  {
    return "";
  }
  if (!value.isEmpty())
  {
    return value;
  }

  if (isUrl)
  {
    return "";
  }

  // Read value from file as a fallback
  value = d->readValueFromFile(fileName, sopInstanceUID, tag);
  return value;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fileValue(const QString fileName, const unsigned short group, const unsigned short element)
{
  QString tag = this->groupElementToTag(group, element);
  return this->fileValue(fileName, tag);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::instanceValueExists(const QString sopInstanceUID, const QString tag)
{
  Q_D(ctkDICOMDatabase);
  QString upperTag = tag.toUpper();
  QString value = this->cachedTag(sopInstanceUID, upperTag);
  if (value == TagNotInInstance || value == ValueIsEmptyString)
  {
    return false;
  }
  if (value == ValueIsNotStored || !value.isEmpty())
  {
    return true;
  }

  // Read value from file
  QString filePath = this->fileForInstance(sopInstanceUID);
  if (filePath.isEmpty())
  {
    return false;
  }
  value = d->readValueFromFile(filePath, sopInstanceUID, upperTag);
  return (value != TagNotInInstance && value != ValueIsEmptyString);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::instanceValueExists(const QString sopInstanceUID, const unsigned short group, const unsigned short element)
{
  QString tag = this->groupElementToTag(group, element);
  return this->instanceValueExists(sopInstanceUID, tag);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::fileValueExists(const QString fileName, QString tag)
{
  Q_D(ctkDICOMDatabase);

  if (fileName.isEmpty())
  {
    return false;
  }

  tag = tag.toUpper();

  QString sopInstanceUID = this->instanceForFile(fileName);
  QString value = this->cachedTag(sopInstanceUID, tag);
  if (value == TagNotInInstance || value == ValueIsEmptyString)
  {
    return false;
  }
  if (value == ValueIsNotStored || !value.isEmpty())
  {
    return true;
  }

  // Read value from file
  value = d->readValueFromFile(fileName, sopInstanceUID, tag);
  return (value != TagNotInInstance && value != ValueIsEmptyString);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::fileValueExists(const QString fileName, const unsigned short group, const unsigned short element)
{
  QString tag = this->groupElementToTag(group, element);
  return this->fileValueExists(fileName, tag);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::tagToGroupElement(const QString tag, unsigned short& group, unsigned short& element)
{
  QStringList groupElement = tag.split(",");
  bool groupOK, elementOK;
  if (groupElement.length() != 2)
  {
    return false;
  }
  group = groupElement[0].toUInt(&groupOK, 16);
  element = groupElement[1].toUInt(&elementOK, 16);

  return( groupOK && elementOK );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::groupElementToTag(const unsigned short& group, const unsigned short& element)
{
  QString groupElement = QString("%1,%2").arg(group,4,16,QLatin1Char('0')).arg(element,4,16,QLatin1Char('0'));
  return groupElement.toUpper();
}

//
// methods related to insert
//

//------------------------------------------------------------------------------
void ctkDICOMDatabase::prepareInsert()
{
  Q_D(ctkDICOMDatabase);
  // Although resetLastInsertedValues is called when items are deleted through
  // this database connection, there may be concurrent database modifications
  // (even in the same application) through other connections.
  // Therefore, we cannot be sure that the last added patient, study, series,
  // items are still in the database. We clear cached Last... IDs to make sure
  // the patient, study, series items are created.
  d->resetLastInsertedValues();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert( DcmItem *item, bool storeFile, bool generateThumbnail)
{
  if (!item)
  {
    return;
  }
  ctkDICOMItem dataset;
  dataset.InitializeFromItem(item, false /* do not take ownership */);
  this->insert(dataset,storeFile,generateThumbnail);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert( const ctkDICOMItem& dataset, bool storeFile, bool generateThumbnail)
{
  Q_D(ctkDICOMDatabase);
  d->insert(dataset, QString(), storeFile, generateThumbnail);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert( const QString& filePath, bool storeFile, bool generateThumbnail, bool createHierarchy, const QString& destinationDirectoryName)
{
  Q_D(ctkDICOMDatabase);
  Q_UNUSED(createHierarchy);
  Q_UNUSED(destinationDirectoryName);

  /// first we check if the file is already in the database
  if (fileExistsAndUpToDate(filePath))
  {
    logger.debug( "File " + filePath + " already added.");
    return;
  }

  ctkDICOMItem dataset;

  dataset.InitializeFromFile(filePath);
  if ( dataset.IsInitialized() )
  {
    d->insert( dataset, filePath, storeFile, generateThumbnail );
  }
  else
  {
    logger.warn(QString("Could not read DICOM file:") + filePath);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert(const QList<ctkDICOMDatabase::IndexingResult>& indexingResults)
{
  Q_D(ctkDICOMDatabase);
  bool databaseWasChanged = false;

  d->TagCacheDatabase.transaction();
  d->Database.transaction();

  QDir databaseDirectory(this->databaseDirectory());
  foreach(const ctkDICOMDatabase::IndexingResult & indexingResult, indexingResults)
  {
    const ctkDICOMItem& dataset = *indexingResult.dataset.data();
    QString filePath = indexingResult.filePath;
    bool generateThumbnail = false; // thumbnail will be generated when needed, don't slow down import with that
    bool storeFile = indexingResult.copyFile;

    // Check to see if the file has already been loaded
    QString sopInstanceUID(dataset.GetElementAsString(DCM_SOPInstanceUID));
    bool datasetInDatabase = false;
    bool datasetUpToDate = false;
    if (indexingResult.overwriteExistingDataset)
    {
      // overwrite was requested based on exact file match
      datasetInDatabase = true;
      datasetUpToDate = false;
    }
    else
    {
      // there is no exact file match, but there may be still a different file in the database
      // for the same SOP instance UID
      QString databaseFilename;
      if (!d->indexingStatusForFile(filePath, sopInstanceUID, datasetInDatabase, datasetUpToDate, databaseFilename))
      {
        // error occurred, message is already logged
        continue;
      }
    }

    if (datasetInDatabase)
    {
      if (datasetUpToDate)
      {
        continue;
      }
      // File is updated, delete record and re-index
      if (!d->removeImage(sopInstanceUID))
      {
        logger.error("Failed to insert file into database (cannot update pre-existing item): " + filePath);
        continue;
      }
    }

    // Verify that minimum required fields are present
    QString patientsName, patientID, studyInstanceUID, seriesInstanceUID;
    if (!d->uidsForDataSet(dataset, patientsName, patientID, studyInstanceUID, seriesInstanceUID))
    {
      logger.error("Failed to insert file into database (required fields missing): " + filePath);
      continue;
    }

    // Store a copy of the dataset
    QString storedFilePath = filePath;
    if (storeFile && !seriesInstanceUID.isEmpty() && !this->isInMemory())
    {
      if (!d->storeDatasetFile(dataset, filePath, studyInstanceUID, seriesInstanceUID, sopInstanceUID, storedFilePath))
      {
        continue;
      }
    }

    if (d->insertPatientStudySeries(dataset, patientID, patientsName))
    {
      databaseWasChanged = true;
    }

    if (!storedFilePath.isEmpty() && !seriesInstanceUID.isEmpty())
    {
      // Insert all pre-cached fields into tag cache
      QSqlQuery insertTags(d->TagCacheDatabase);
      insertTags.prepare("INSERT OR REPLACE INTO TagCache VALUES(?,?,?)");
      insertTags.bindValue(0, sopInstanceUID);
      foreach(const QString & tag, d->TagsToPrecache)
      {
        unsigned short group, element;
        this->tagToGroupElement(tag, group, element);
        DcmTagKey tagKey(group, element);
        QString value;
        if (d->TagsToExcludeFromStorage.contains(tag))
        {
          if (dataset.TagExists(tagKey))
          {
            value = ValueIsNotStored;
          }
          else
          {
            value = TagNotInInstance;
          }
        }
        else
        {
          value = dataset.GetAllElementValuesAsString(tagKey);
        }
        insertTags.bindValue(1, tag);
        if (value.isEmpty())
        {
          insertTags.bindValue(2, TagNotInInstance);
        }
        else
        {
          insertTags.bindValue(2, value);
        }
        insertTags.exec();
      }

      // Insert image files
      QSqlQuery insertImageStatement(d->Database);
      insertImageStatement.prepare("INSERT INTO Images ( 'SOPInstanceUID', 'Filename', 'URL', 'SeriesInstanceUID', 'InsertTimestamp' ) VALUES ( ?, ?, ?, ?, ? )");
      insertImageStatement.addBindValue(sopInstanceUID);
      insertImageStatement.addBindValue(d->internalPathFromAbsolute(storedFilePath));
      insertImageStatement.addBindValue(QString(""));
      insertImageStatement.addBindValue(seriesInstanceUID);
      insertImageStatement.addBindValue(QDateTime::currentDateTime());
      insertImageStatement.exec();
      emit instanceAdded(sopInstanceUID);
      databaseWasChanged = true;

      if (generateThumbnail)
      {
        d->storeThumbnailFile(storedFilePath, studyInstanceUID, seriesInstanceUID, sopInstanceUID);
      }
    }
  }

  d->Database.commit();
  d->TagCacheDatabase.commit();

  if (databaseWasChanged && this->isInMemory())
  {
    emit this->databaseChanged();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert(QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets)
{
  Q_D(ctkDICOMDatabase);

  bool databaseWasChanged = false;

  d->TagCacheDatabase.transaction();
  d->Database.transaction();
  QDir databaseDirectory(this->databaseDirectory());
  foreach (QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet, jobResponseSets)
  {
    ctkDICOMJobResponseSet::JobType jobType = jobResponseSet->jobType();
    QString filePath = jobResponseSet->filePath();
    QString url;
    QString connectionName = jobResponseSet->connectionName();
    bool generateThumbnail = false; // thumbnail will be generated when needed, don't slow down import with that
    bool storeFile = jobResponseSet->copyFile();

    QMap<QString, ctkDICOMItem*> datasets = jobResponseSet->datasets();
    for(QString key : datasets.keys())
    {
      ctkDICOMItem* dataset = datasets.value(key);
      if (!dataset)
      {
        continue;
      }
      QString patientID, patientName, studyInstanceUID, seriesInstanceUID, sopInstanceUID;
      patientName = dataset->GetElementAsString(DCM_PatientName);
      patientID = dataset->GetElementAsString(DCM_PatientID);
      studyInstanceUID = dataset->GetElementAsString(DCM_StudyInstanceUID);
      seriesInstanceUID = dataset->GetElementAsString(DCM_SeriesInstanceUID);
      sopInstanceUID = dataset->GetElementAsString(DCM_SOPInstanceUID);
      if (patientID.isEmpty())
      {
        if (jobType == ctkDICOMJobResponseSet::JobType::QueryPatients)
        {
          patientID = key;
        }
        else if (jobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
                 jobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
                 jobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveStudy ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
        {
          patientID = jobResponseSet->patientID();
        }

        dataset->SetElementAsString(DCM_PatientID, patientID);
      }

      if (studyInstanceUID.isEmpty())
      {
        if (jobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
        {
          studyInstanceUID = key;
        }
        else if (jobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
                 jobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveStudy ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
        {
          studyInstanceUID = jobResponseSet->studyInstanceUID();
        }

        dataset->SetElementAsString(DCM_StudyInstanceUID, studyInstanceUID);
      }

      if (patientName.isEmpty() && !studyInstanceUID.isEmpty())
      {
        QString patientUID = this->patientForStudy(studyInstanceUID);
        patientName = this->nameForPatient(patientUID);
        dataset->SetElementAsString(DCM_PatientName, patientName);
      }

      if (seriesInstanceUID.isEmpty())
      {
        if (jobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
        {
          seriesInstanceUID = key;
        }
        else if (jobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
        {
          seriesInstanceUID = jobResponseSet->seriesInstanceUID();
        }

        dataset->SetElementAsString(DCM_SeriesInstanceUID, seriesInstanceUID);
      }

      if (sopInstanceUID.isEmpty())
      {
        if (jobType == ctkDICOMJobResponseSet::JobType::QueryInstances)
        {
          sopInstanceUID = key;
        }
        else if (jobType == ctkDICOMJobResponseSet::JobType::RetrieveStudy ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
                 jobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
                 jobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance)
        {
        sopInstanceUID = jobResponseSet->sopInstanceUID();
        }

        dataset->SetElementAsString(DCM_SOPInstanceUID, sopInstanceUID);
      }

      if (patientID.isEmpty())
      {
        logger.error("ctkDICOMDatabase::insert: dataset has no patientID");
        continue;
      }

      if (patientName.isEmpty())
      {
        logger.error("ctkDICOMDatabase::insert: dataset has no patientName");
        continue;
      }

      if (studyInstanceUID.isEmpty() && jobType != ctkDICOMJobResponseSet::JobType::QueryPatients)
      {
        logger.error("ctkDICOMDatabase::insert: dataset has no studyInstanceUID");
        continue;
      }

      if (jobType == ctkDICOMJobResponseSet::JobType::QueryInstances)
      {
        url = "dimse+ctk://" + connectionName;
        if (!studyInstanceUID.isEmpty())
        {
          url += "/" + studyInstanceUID;
        }
        if (!seriesInstanceUID.isEmpty())
        {
          url += "/" + seriesInstanceUID;
        }
        if (!sopInstanceUID.isEmpty())
        {
          url += "/" + sopInstanceUID;
        }
      }

      // Check to see if the file has already been loaded
      bool datasetInDatabase = false;
      bool datasetUpToDate = false;
      if (jobResponseSet->overwriteExistingDataset())
      {
        // overwrite was requested based on exact file match
        datasetInDatabase = true;
        datasetUpToDate = false;
      }
      else
      {
        // there is no exact file match, but there may be still a different file in the database
        // for the same SOP instance UID
        QString databaseFilename;
        if (!d->indexingStatusForFile(filePath, sopInstanceUID, datasetInDatabase, datasetUpToDate, databaseFilename))
        {
          // error occurred, message is already logged
          continue;
        }
      }

      if (datasetInDatabase)
      {
        if (datasetUpToDate)
        {
          continue;
        }
        // File is updated, delete record and re-index
        if (!d->removeImage(sopInstanceUID))
        {
          logger.error("Failed to insert file into database (cannot update pre-existing item): " + filePath);
          continue;
        }
      }

      // Store a copy of the dataset
      QString storedFilePath = filePath;
      if (storeFile && !seriesInstanceUID.isEmpty() && !this->isInMemory())
      {
        if (!d->storeDatasetFile(*dataset, filePath, studyInstanceUID, seriesInstanceUID, sopInstanceUID, storedFilePath))
        {
          continue;
        }
      }

      if (d->insertPatientStudySeries(*dataset, patientID, patientName, connectionName))
      {
        databaseWasChanged = true;
      }

      if (!sopInstanceUID.isEmpty() &&
          !seriesInstanceUID.isEmpty() &&
          (!storedFilePath.isEmpty() ||
          !url.isEmpty()))
      {
        bool alreadyInserted = false;
        if (!storeFile)
        {
          // file is linked, maybe it is already inserted
          QSqlQuery checkImageExistsQuery(d->Database);
          checkImageExistsQuery.prepare("SELECT * FROM Images WHERE SOPInstanceUID = ?");
          checkImageExistsQuery.addBindValue(sopInstanceUID);
          if (!d->loggedExec(checkImageExistsQuery))
          {
            return;
          }
          alreadyInserted = checkImageExistsQuery.next();
        }
        if (!alreadyInserted)
        {
          // Get filename that will be stored in the database.
          // Use relative path if a copy is stored in the database to make the database relocatable.
          QString storedFilePathInDatabase;
          if (storeFile)
          {
            QDir databaseDirectory(this->databaseDirectory());
            storedFilePathInDatabase = databaseDirectory.relativeFilePath(storedFilePath);
          }
          else
          {
            storedFilePathInDatabase = storedFilePath;
          }

          QSqlQuery insertImageStatement(d->Database);
          insertImageStatement.prepare("INSERT INTO Images ( 'SOPInstanceUID', 'Filename', 'URL', 'SeriesInstanceUID', 'InsertTimestamp' ) VALUES ( ?, ?, ?, ?, ? )");
          insertImageStatement.addBindValue(sopInstanceUID);
          insertImageStatement.addBindValue(storedFilePathInDatabase);
          insertImageStatement.addBindValue(url);
          insertImageStatement.addBindValue(seriesInstanceUID);
          insertImageStatement.addBindValue(QDateTime::currentDateTime());

          if (!insertImageStatement.exec())
          {
            logger.error( "Error executing statement: "
                         + insertImageStatement.lastQuery()
                         + " Error: " + insertImageStatement.lastError().text() );
          }
          else
          {
            // insert was needed, so cache any application-requested tags
            d->precacheTags(*dataset, sopInstanceUID);
          }

          // let users of this class track when things happen
          emit instanceAdded(sopInstanceUID);
          databaseWasChanged = true;
        }
        if (generateThumbnail)
        {
          d->storeThumbnailFile(storedFilePath, studyInstanceUID, seriesInstanceUID, sopInstanceUID);
        }
      }
    }
  }

  d->Database.commit();
  d->TagCacheDatabase.commit();

  if (databaseWasChanged && this->isInMemory())
  {
    emit this->databaseChanged();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setTagsToPrecache(const QStringList tags)
{
  Q_D(ctkDICOMDatabase);
  if (d->TagsToPrecache == tags)
  {
    return;
  }
  d->TagsToPrecache = tags;
  emit tagsToPrecacheChanged();
}

//------------------------------------------------------------------------------
const QStringList ctkDICOMDatabase::tagsToPrecache()
{
  Q_D(ctkDICOMDatabase);
  return d->TagsToPrecache;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setTagsToExcludeFromStorage(const QStringList tags)
{
  Q_D(ctkDICOMDatabase);
  QStringList upperTags;
  foreach (const QString &tag, tags)
  {
    upperTags << tag.toUpper();
  }
  if (d->TagsToExcludeFromStorage == upperTags)
  {
    return;
  }
  d->TagsToExcludeFromStorage = upperTags;
  emit tagsToExcludeFromStorageChanged();
}

//------------------------------------------------------------------------------
const QStringList ctkDICOMDatabase::tagsToExcludeFromStorage()
{
  Q_D(ctkDICOMDatabase);
  return d->TagsToExcludeFromStorage;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::fileExistsAndUpToDate(const QString& filePath)
{
  Q_D(ctkDICOMDatabase);
  bool result(false);

  QSqlQuery check_filename_query(database());
  check_filename_query.prepare("SELECT InsertTimestamp FROM Images WHERE Filename == ?");
  check_filename_query.bindValue(0, d->internalPathFromAbsolute(filePath));
  d->loggedExec(check_filename_query);
  if ( check_filename_query.next() &&
       QFileInfo(filePath).lastModified() < QDateTime::fromString(check_filename_query.value(0).toString(), Qt::ISODate) )
  {
    result = true;
  }
  check_filename_query.finish();
  return result;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::allFilesModifiedTimes(QMap<QString, QDateTime>& modifiedTimeForFilepath)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery allFilesModifiedQuery(database());
  allFilesModifiedQuery.prepare("SELECT Filename, InsertTimestamp FROM Images;");
  bool success = d->loggedExec(allFilesModifiedQuery);
  while (allFilesModifiedQuery.next())
  {
    QString filename = d->absolutePathFromInternal(allFilesModifiedQuery.value(0).toString());
    QDateTime modifiedTime = QDateTime::fromString(allFilesModifiedQuery.value(1).toString(), Qt::ISODate);
    if (modifiedTimeForFilepath.contains(filename) && modifiedTimeForFilepath[filename] <= modifiedTime)
    {
      continue;
    }
    modifiedTimeForFilepath[filename] = modifiedTime;
  }
  allFilesModifiedQuery.finish();
  return success;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::isOpen() const
{
  Q_D(const ctkDICOMDatabase);
  return d->Database.isOpen();
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::isInMemory() const
{
  Q_D(const ctkDICOMDatabase);
  return d->DatabaseFileName == ":memory:";
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::removeSeries(const QString& seriesInstanceUID, bool clearCachedTags/*=false*/, bool cleanup/*=true*/)
{
  Q_D(ctkDICOMDatabase);

  // get all images from series
  QSqlQuery fileExistsQuery(d->Database);
  fileExistsQuery.prepare("SELECT Filename, SOPInstanceUID, StudyInstanceUID FROM Images,Series WHERE Series.SeriesInstanceUID = Images.SeriesInstanceUID AND Images.SeriesInstanceUID = :seriesID");
  fileExistsQuery.bindValue(":seriesID", seriesInstanceUID);
  bool success = fileExistsQuery.exec();
  if (!success)
  {
    logger.error("SQLITE ERROR: " + fileExistsQuery.lastError().driverText());
    return false;
  }

  QList< QPair<QString, QString> > removeList;
  QStringList removeTagCacheSOPInstanceUIDs;
  while (fileExistsQuery.next())
  {
    QString dbFilePath = fileExistsQuery.value(fileExistsQuery.record().indexOf("Filename")).toString();
    QString studyInstanceUID = fileExistsQuery.value(fileExistsQuery.record().indexOf("StudyInstanceUID")).toString();
    QString sopInstanceUID = fileExistsQuery.value(fileExistsQuery.record().indexOf("SOPInstanceUID")).toString();
    QString thumbnailPath = "thumbs/" + d->internalStoragePath(studyInstanceUID, seriesInstanceUID, sopInstanceUID) + ".png";
    removeList << qMakePair(dbFilePath, thumbnailPath);
    if (clearCachedTags)
    {
      removeTagCacheSOPInstanceUIDs << sopInstanceUID;
    }
  }

  QSqlQuery fileRemove(d->Database);
  fileRemove.prepare("DELETE FROM Images WHERE SeriesInstanceUID == :seriesID");
  fileRemove.bindValue(":seriesID", seriesInstanceUID);
  logger.debug("SQLITE: removing seriesInstanceUID " + seriesInstanceUID);
  success = fileRemove.exec();
  if (!success)
  {
    logger.error("SQLITE ERROR: could not remove seriesInstanceUID " + seriesInstanceUID);
    logger.error("SQLITE ERROR: " + fileRemove.lastError().driverText());
  }

  if (!removeTagCacheSOPInstanceUIDs.isEmpty())
  {
    d->TagCacheDatabase.transaction();
    // Remove values from tag cache (may be important for patient confidentiality)
    foreach(QString sopInstanceUID, removeTagCacheSOPInstanceUIDs)
    {
      removeCachedTags(sopInstanceUID);
    }
    d->TagCacheDatabase.commit();
  }

  QPair<QString,QString> fileToRemove;
  QStringList foldersToRemove;
  foreach (fileToRemove, removeList)
  {
    QString dbFilePath = fileToRemove.first;
    QString thumbnailPath = fileToRemove.second;

    // check that the file is below our internal storage
    if (QFileInfo(dbFilePath).isRelative())
    {
      QString absPath = d->absolutePathFromInternal(dbFilePath);
      QFile file(absPath);
      if (file.exists())
      {
        if (file.remove())
        {
          QString fileFolder = QFileInfo(absPath).absoluteDir().path();
          if (foldersToRemove.isEmpty() || foldersToRemove.last() != fileFolder)
          {
            foldersToRemove << fileFolder;
          }
        }
        else
        {
          logger.warn("Failed to remove file " + absPath);
        }
      }
    }
    // Remove thumbnail (if exists)
    QFile thumbnailFile(d->absolutePathFromInternal(thumbnailPath));
    if (thumbnailFile.exists())
    {
      if (!thumbnailFile.remove())
      {
      logger.warn("Failed to remove thumbnail " + thumbnailFile.fileName());
      }
      QString fileFolder = QFileInfo(thumbnailFile).absoluteDir().path();
      if (foldersToRemove.isEmpty() || foldersToRemove.last() != fileFolder)
      {
        foldersToRemove << fileFolder;
      }
    }
  }

  // Delete all empty folders that are left after removing DICOM files
  // (folders that still contain files are not removed)
  foreach (QString folderToRemove, foldersToRemove)
  {
    QDir().rmpath(folderToRemove);
  }

  if (cleanup)
  {
    this->cleanup();
  }

  d->resetLastInsertedValues();

  emit seriesRemoved(seriesInstanceUID);

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::cleanup(bool vacuum/*=false*/)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery seriesCleanup ( d->Database );
  seriesCleanup.exec("DELETE FROM Series WHERE ( SELECT COUNT(*) FROM Images WHERE Images.SeriesInstanceUID = Series.SeriesInstanceUID ) = 0;");
  seriesCleanup.exec("DELETE FROM Studies WHERE ( SELECT COUNT(*) FROM Series WHERE Series.StudyInstanceUID = Studies.StudyInstanceUID ) = 0;");
  seriesCleanup.exec("DELETE FROM Patients WHERE ( SELECT COUNT(*) FROM Studies WHERE Studies.PatientsUID = Patients.UID ) = 0;");
  if (vacuum)
  {
    seriesCleanup.exec("VACUUM;");
    QSqlQuery tagcacheCleanup(d->TagCacheDatabase);
    seriesCleanup.exec("VACUUM;");
  }
  d->resetLastInsertedValues();
  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::removeStudy(const QString& studyInstanceUID, bool cleanup/*=true*/)
{
  Q_D(ctkDICOMDatabase);

  QSqlQuery seriesForStudy( d->Database );
  seriesForStudy.prepare("SELECT SeriesInstanceUID FROM Series WHERE StudyInstanceUID = :studyID");
  seriesForStudy.bindValue(":studyID", studyInstanceUID);
  bool success = seriesForStudy.exec();
  if (!success)
  {
    logger.error("SQLITE ERROR: " + seriesForStudy.lastError().driverText());
    return false;
  }
  bool result = true;
  while ( seriesForStudy.next() )
  {
    QString seriesInstanceUID = seriesForStudy.value(seriesForStudy.record().indexOf("SeriesInstanceUID")).toString();
    if ( ! this->removeSeries(seriesInstanceUID, false, cleanup) )
    {
      result = false;
    }
  }
  d->resetLastInsertedValues();

  if(result)
  {
    emit studyRemoved(studyInstanceUID);
  }

  return result;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::removePatient(const QString& patientID, bool cleanup/*=true*/)
{
  Q_D(ctkDICOMDatabase);

  QSqlQuery studiesForPatient( d->Database );
  studiesForPatient.prepare("SELECT StudyInstanceUID FROM Studies WHERE PatientsUID = :patientsID");
  studiesForPatient.bindValue(":patientsID", patientID);
  bool success = studiesForPatient.exec();
  if (!success)
  {
    logger.error("SQLITE ERROR: " + studiesForPatient.lastError().driverText());
    return false;
  }
  bool result = true;
  while ( studiesForPatient.next() )
  {
    QString studyInstanceUID = studiesForPatient.value(studiesForPatient.record().indexOf("StudyInstanceUID")).toString();
    if ( ! this->removeStudy(studyInstanceUID, cleanup) )
    {
      result = false;
    }
  }
  d->resetLastInsertedValues();

  if(result)
  {
    emit patientRemoved(patientID);
  }

  return result;
}

///
/// Code related to the tagCache
///

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::tagCacheExists()
{
  Q_D(ctkDICOMDatabase);

  if (d->TagCacheVerified)
  {
    return true;
  }

  if (!d->openTagCacheDatabase())
  {
    return false;
  }

  if (d->TagCacheDatabase.tables().count() == 0)
  {
    return false;
  }

  // check that the table exists
  QSqlQuery cacheExists( d->TagCacheDatabase );
  cacheExists.prepare("SELECT * FROM TagCache LIMIT 1");
  bool success = d->loggedExec(cacheExists);
  if (success)
  {
    d->TagCacheVerified = true;
    return true;
  }
  qDebug() << "TagCacheDatabase NOT verified based on table check!\n";
  return false;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::initializeTagCache()
{
  Q_D(ctkDICOMDatabase);

  // First, drop any existing table
  if ( this->tagCacheExists() )
  {
    qDebug() << "TagCacheDatabase drop existing table\n";
    QSqlQuery dropCacheTable( d->TagCacheDatabase );
    dropCacheTable.prepare( "DROP TABLE TagCache" );
    d->loggedExec(dropCacheTable);
  }

  // now create a table
  qDebug() << "TagCacheDatabase adding table\n";
  QSqlQuery createCacheTable( d->TagCacheDatabase );
  createCacheTable.prepare(
    "CREATE TABLE TagCache (SOPInstanceUID, Tag, Value, PRIMARY KEY (SOPInstanceUID, Tag))" );
  bool success = d->loggedExec(createCacheTable);
  if (!success)
  {
    return false;
  }

  d->TagCacheVerified = true;
  return true;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::cachedTag(const QString sopInstanceUID, const QString tag)
{
  Q_D(ctkDICOMDatabase);
  if ( !this->tagCacheExists() )
  {
    if ( !this->initializeTagCache() )
    {
      return( "" );
    }
  }
  QSqlQuery selectValue( d->TagCacheDatabase );
  selectValue.prepare( "SELECT Value FROM TagCache WHERE SOPInstanceUID = :sopInstanceUID AND Tag = :tag" );
  selectValue.bindValue(":sopInstanceUID",sopInstanceUID);
  selectValue.bindValue(":tag",tag.toUpper());
  d->loggedExec(selectValue);
  QString result("");
  if (selectValue.next())
  {
    result = selectValue.value(0).toString();
    if (result == QString(""))
    {
      result = ValueIsEmptyString;
    }
  }
  return( result );
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::getCachedTags(const QString sopInstanceUID, QMap<QString, QString> &cachedTags)
{
  Q_D(ctkDICOMDatabase);
  cachedTags.clear();
  if ( !this->tagCacheExists() )
  {
    if ( !this->initializeTagCache() )
    {
      // cache is empty
      return;
    }
  }
  QSqlQuery selectValue( d->TagCacheDatabase );
  selectValue.prepare( "SELECT Tag, Value FROM TagCache WHERE SOPInstanceUID = :sopInstanceUID" );
  selectValue.bindValue(":sopInstanceUID",sopInstanceUID);
  d->loggedExec(selectValue);
  QString tag;
  QString value;
  while (selectValue.next())
  {
    tag = selectValue.value(0).toString().toUpper();
    value = selectValue.value(1).toString();
    if (value == TagNotInInstance || value == ValueIsEmptyString || value == ValueIsNotStored)
    {
      value = QString("");
    }
    cachedTags.insert(tag, value);
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::cacheTag(const QString sopInstanceUID, const QString tag, const QString value)
{
  QStringList sopInstanceUIDs, tags, values;
  sopInstanceUIDs << sopInstanceUID;
  tags << tag.toUpper();
  values << value;
  return this->cacheTags(sopInstanceUIDs, tags, values);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::cacheTags(const QStringList sopInstanceUIDs, const QStringList tags, QStringList values)
{
  Q_D(ctkDICOMDatabase);
  int itemCount = sopInstanceUIDs.size();
  if (tags.size() != itemCount || values.size() != itemCount)
  {
    logger.error("Failed to cache tags: number of inputs do not match");
    return false;
  }

  if (!this->tagCacheExists())
  {
    if (!this->initializeTagCache())
    {
      return false;
    }
  }

  d->TagCacheDatabase.transaction();

  QSqlQuery insertTags(d->TagCacheDatabase);
  insertTags.prepare( "INSERT OR REPLACE INTO TagCache VALUES(?,?,?)" );

  QStringList::const_iterator sopInstanceUIDsIt = sopInstanceUIDs.begin();
  QStringList::const_iterator tagsIt = tags.begin();
  QStringList::const_iterator valuesIt = values.begin();
  bool success = true;
  for (int i = 0; i<itemCount; ++i)
  {
    insertTags.bindValue(0, *sopInstanceUIDsIt);
    insertTags.bindValue(1, (*tagsIt).toUpper());
    if (valuesIt->isEmpty())
    {
      // replace empty strings with special flag string
      insertTags.bindValue(2, TagNotInInstance);
    }
    else
    {
      insertTags.bindValue(2, *valuesIt);
    }
    if (!insertTags.exec())
    {
      success = false;
    }
    ++sopInstanceUIDsIt;
    ++tagsIt;
    ++valuesIt;
  }

  d->TagCacheDatabase.commit();

  return success;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::removeCachedTags(const QString sopInstanceUID)
{
  Q_D(ctkDICOMDatabase);
  if (!this->tagCacheExists())
  {
    return;
  }
  QSqlQuery deleteFile(d->TagCacheDatabase);
  deleteFile.prepare("DELETE FROM TagCache WHERE SOPInstanceUID == :sopInstanceUID");
  deleteFile.bindValue(":sopInstanceUID", sopInstanceUID);
  bool success = deleteFile.exec();
  if (!success)
  {
    logger.error("SQLITE ERROR deleting tag cache row: " + deleteFile.lastError().driverText());
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::updateDisplayedFields()
{
  Q_D(ctkDICOMDatabase);

  // Get the files for which the displayed fields have not been created yet (DisplayedFieldsUpdatedTimestamp is NULL)
  // Note: The per-instance update only covers insertion and schema update. If fields on the series/study/patient level need to be
  // updated on the insertion of a new instance, then it can be handled using the startUpdate/endUpdate functions of the rules.
  QSqlQuery newFilesQuery(d->Database);
  d->loggedExec(newFilesQuery,QString("SELECT SOPInstanceUID, SeriesInstanceUID FROM Images WHERE DisplayedFieldsUpdatedTimestamp IS NULL;"));

  // Populate displayed fields maps from the current display tables
  QMap<QString /*SeriesInstanceUID*/, QMap<QString /*DisplayField*/, QString /*Value*/> > displayedFieldsMapSeries;
  QMap<QString /*StudyInstanceUID*/, QMap<QString /*DisplayField*/, QString /*Value*/> > displayedFieldsMapStudy;
  QMap<QString /*CompositePatientID*/, QMap<QString /*DisplayField*/, QString /*Value*/> > displayedFieldsMapPatient;

  int progressValue = 0;
  emit displayedFieldsUpdateProgress(++progressValue);

  // Initialize rules for starting the update
  d->DisplayedFieldGenerator->startUpdate();

  // Get display names for newly added files and add them into the display tables
  while (newFilesQuery.next())
  {
    QString sopInstanceUID = newFilesQuery.value(0).toString();
    QString seriesInstanceUID = newFilesQuery.value(1).toString();
    QMap<QString, QString> cachedTags;
    this->getCachedTags(sopInstanceUID, cachedTags);

    // Patient
    QString patientsName = cachedTags[ctkDICOMItem::TagKeyStripped(DCM_PatientName)];
    QString patientID = cachedTags[ctkDICOMItem::TagKeyStripped(DCM_PatientID)];
    QString studyInstanceUID = cachedTags[ctkDICOMItem::TagKeyStripped(DCM_StudyInstanceUID)];
    if (!d->uidsForDataSet(patientsName, patientID, studyInstanceUID))
    {
      // UIDs not valid, message is already logged
      continue;
    }

    QString patientsBirthDate = cachedTags[ctkDICOMItem::TagKeyStripped(DCM_PatientBirthDate)];

    QString compositeId = d->getDisplayPatientFieldsKey(patientID, patientsName, patientsBirthDate, displayedFieldsMapPatient);
    if (compositeId.isEmpty())
    {
      logger.error("Failed to find patient for SOP Instance UID = " + sopInstanceUID);
      continue;
    }
    QMap<QString, QString> displayedFieldsForCurrentPatient = displayedFieldsMapPatient[compositeId];

    // Study
    QString displayedFieldsKeyForCurrentStudy = d->getDisplayStudyFieldsKey(
      cachedTags[ctkDICOMItem::TagKeyStripped(DCM_StudyInstanceUID)], displayedFieldsMapStudy );
    if (displayedFieldsKeyForCurrentStudy.isEmpty())
    {
      logger.error("Failed to find study for SOP Instance UID = " + sopInstanceUID);
      continue;
    }
    QMap<QString, QString> displayedFieldsForCurrentStudy = displayedFieldsMapStudy[ displayedFieldsKeyForCurrentStudy ];
    displayedFieldsForCurrentStudy["PatientCompositeID"] = compositeId;

    // Series
    QString displayedFieldsKeyForCurrentSeries = d->getDisplaySeriesFieldsKey(seriesInstanceUID, displayedFieldsMapSeries);
    if (displayedFieldsKeyForCurrentSeries.isEmpty())
    {
      logger.error("Failed to find series for SOP Instance UID = " + sopInstanceUID);
      continue;
    }
    QMap<QString, QString> displayedFieldsForCurrentSeries = displayedFieldsMapSeries[ displayedFieldsKeyForCurrentSeries ];

    // Do the update of the displayed fields using the rules
    d->DisplayedFieldGenerator->updateDisplayedFieldsForInstance(sopInstanceUID, cachedTags,
      displayedFieldsForCurrentSeries, displayedFieldsForCurrentStudy, displayedFieldsForCurrentPatient);

    // Set updated fields to the series / study / patient displayed fields maps
    displayedFieldsMapSeries[ displayedFieldsKeyForCurrentSeries ] = displayedFieldsForCurrentSeries;
    displayedFieldsMapStudy[ displayedFieldsKeyForCurrentStudy ] = displayedFieldsForCurrentStudy;
    displayedFieldsMapPatient[ compositeId ] = displayedFieldsForCurrentPatient;
  } // For each instance

  emit displayedFieldsUpdateProgress(++progressValue);

  // Finalize update by giving the rules the chance to write the final results in the maps
  d->DisplayedFieldGenerator->endUpdate(displayedFieldsMapSeries, displayedFieldsMapStudy, displayedFieldsMapPatient);

  emit displayedFieldsUpdateProgress(++progressValue);

  // Update/insert the display values
  if (displayedFieldsMapSeries.count() > 0)
  {
    d->Database.transaction();

    if (d->applyDisplayedFieldsChanges(displayedFieldsMapSeries, displayedFieldsMapStudy, displayedFieldsMapPatient))
    {
      // Update image timestamp
      newFilesQuery.first();
      newFilesQuery.previous(); // Need to go one before the first record
      while (newFilesQuery.next())
      {
        QSqlQuery updateDisplayedFieldsUpdatedTimestampStatement(d->Database);
        QString updateDisplayedFieldsUpdatedTimestampStatementString =
          QString("UPDATE IMAGES SET DisplayedFieldsUpdatedTimestamp=CURRENT_TIMESTAMP WHERE SOPInstanceUID='%1';").arg(newFilesQuery.value(0).toString());
        d->loggedExec(updateDisplayedFieldsUpdatedTimestampStatement, updateDisplayedFieldsUpdatedTimestampStatementString);
      }
    }

    d->Database.commit();
  }

  emit displayedFieldsUpdated();
  emit databaseChanged();
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::displayedNameForField(QString table, QString field) const
{
  Q_D(const ctkDICOMDatabase);

  QSqlQuery query(d->Database);
  query.prepare("SELECT DisplayedName FROM ColumnDisplayProperties WHERE TableName = ? AND FieldName = ? ;");
  query.addBindValue(table);
  query.addBindValue(field);
  if (!query.exec())
  {
    logger.error("SQLITE ERROR: " + query.lastError().driverText());
    return QString();
  }

  query.first();
  return query.value(0).toString();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setDisplayedNameForField(QString table, QString field, QString displayedName)
{
  Q_D(ctkDICOMDatabase);

  if (!this->isOpen())
  {
    logger.warn("Database needs to be open to set column display properties");
    return;
  }

  QSqlQuery query(d->Database);
  query.prepare("UPDATE ColumnDisplayProperties SET DisplayedName = ? WHERE TableName = ? AND FieldName = ? ;");
  query.addBindValue(displayedName);
  query.addBindValue(table);
  query.addBindValue(field);
  if (!d->loggedExec(query))
  {
    logger.error("SQLITE ERROR: " + query.lastError().driverText());
    return;
  }

  emit databaseChanged();
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::visibilityForField(QString table, QString field) const
{
  Q_D(const ctkDICOMDatabase);

  QSqlQuery query(d->Database);
  query.prepare("SELECT Visibility FROM ColumnDisplayProperties WHERE TableName = ? AND FieldName = ? ;");
  query.addBindValue(table);
  query.addBindValue(field);
  if (!query.exec())
  {
    logger.error("SQLITE ERROR: " + query.lastError().driverText());
    return false;
  }

  query.first();
  return (query.value(0).toInt() != 0);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setVisibilityForField(QString table, QString field, bool visibility)
{
  Q_D(ctkDICOMDatabase);

  if (!this->isOpen())
  {
    logger.warn("Database needs to be open to set column display properties");
    return;
  }

  QSqlQuery query(d->Database);
  query.prepare("UPDATE ColumnDisplayProperties SET Visibility = ? WHERE TableName = ? AND FieldName = ? ;");
  query.addBindValue(static_cast<int>(visibility));
  query.addBindValue(table);
  query.addBindValue(field);
  if (!d->loggedExec(query))
  {
    logger.error("SQLITE ERROR: " + query.lastError().driverText());
    return;
  }

  emit databaseChanged();
}

//------------------------------------------------------------------------------
int ctkDICOMDatabase::weightForField(QString table, QString field) const
{
  Q_D(const ctkDICOMDatabase);

  QSqlQuery query(d->Database);
  query.prepare("SELECT Weight FROM ColumnDisplayProperties WHERE TableName = ? AND FieldName = ? ;");
  query.addBindValue(table);
  query.addBindValue(field);
  if (!query.exec())
  {
    logger.error("SQLITE ERROR: " + query.lastError().driverText());
    return INT_MAX;
  }

  query.first();
  return query.value(0).toInt();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setWeightForField(QString table, QString field, int weight)
{
  Q_D(ctkDICOMDatabase);

  if (!this->isOpen())
  {
    logger.warn("Database needs to be open to set column display properties");
    return;
  }

  QSqlQuery query(d->Database);
  query.prepare("UPDATE ColumnDisplayProperties SET Weight = ? WHERE TableName = ? AND FieldName = ? ;");
  query.addBindValue(weight);
  query.addBindValue(table);
  query.addBindValue(field);
  if (!d->loggedExec(query))
  {
    logger.error("SQLITE ERROR: " + query.lastError().driverText());
    return;
  }

  emit databaseChanged();
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::formatForField(QString table, QString field) const
{
  Q_D(const ctkDICOMDatabase);

  QSqlQuery query(d->Database);
  query.prepare("SELECT Format FROM ColumnDisplayProperties WHERE TableName = ? AND FieldName = ? ;");
  query.addBindValue(table);
  query.addBindValue(field);
  if (!query.exec())
  {
    logger.error("SQLITE ERROR: " + query.lastError().driverText());
    return QString();
  }

  query.first();
  return query.value(0).toString();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setFormatForField(QString table, QString field, QString format)
{
  Q_D(ctkDICOMDatabase);

  if (!this->isOpen())
  {
    logger.warn("Database needs to be open to set column display properties");
    return;
  }

  QSqlQuery query(d->Database);
  query.prepare("UPDATE ColumnDisplayProperties SET Format = ? WHERE TableName = ? AND FieldName = ? ;");
  query.addBindValue(format);
  query.addBindValue(table);
  query.addBindValue(field);
  if (!d->loggedExec(query))
  {
    logger.error("SQLITE ERROR: " + query.lastError().driverText());
    return;
  }

  emit databaseChanged();
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::compositePatientID(const QString& patientID, const QString& patientsName, const QString& patientsBirthDate)
{
  return QString("%1~%2~%3").arg(patientID).arg(patientsBirthDate).arg(patientsName);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setLoadedSeries(const QStringList &seriesList)
{
  Q_D(ctkDICOMDatabase);
  d->LoadedSeries = seriesList;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::loadedSeries() const
{
  Q_D(const ctkDICOMDatabase);
  return d->LoadedSeries;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setVisibleSeries(const QStringList &seriesList)
{
  Q_D(ctkDICOMDatabase);
  d->VisibleSeries = seriesList;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::visibleSeries() const
{
  Q_D(const ctkDICOMDatabase);
  return d->VisibleSeries;
}
