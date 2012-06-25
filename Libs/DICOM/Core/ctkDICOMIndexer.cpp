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
    bool                    Canceled;

};

//------------------------------------------------------------------------------
// ctkDICOMIndexerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::ctkDICOMIndexerPrivate()
{
    this->Canceled = false;
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
                              const QString& destinationDirectoryName)
{
    if (!destinationDirectoryName.isEmpty())
    {
        logger.warn("Ignoring destinationDirectoryName parameter, just taking it as indication we should copy!");
    }

    emit indexingFilePath(filePath);

    ctkDICOMDatabase.insert(filePath, !destinationDirectoryName.isEmpty(), true);
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addDirectory(ctkDICOMDatabase& ctkDICOMDatabase, 
                                   const QString& directoryName,
                                   const QString& destinationDirectoryName)
{
    Q_D(ctkDICOMIndexer);

    const std::string src_directory(directoryName.toStdString());

    std::string dcmFilePath = directoryName.toStdString() + "/DICOMDIR";
    const char* dcmDirFilePath = dcmFilePath.c_str();

    //if(OFStandard::fileExists(dcmDirFilePath))
    //{
    //    //this->addFromDICOMDIR(ctkDICOMDatabase, directoryName, destinationDirectoryName);
    //}else{

        OFList<OFString> originalDcmtkFileNames;
        OFList<OFString> dcmtkFileNames;
        OFStandard::searchDirectoryRecursively( QDir::toNativeSeparators(src_directory.c_str()).toAscii().data(), originalDcmtkFileNames, "", "");

        int totalNumberOfFiles = originalDcmtkFileNames.size();

        // hack to reverse list of filenames (not neccessary when image loading works correctly)
        for ( OFListIterator(OFString) iter = originalDcmtkFileNames.begin(); iter != originalDcmtkFileNames.end(); ++iter )
        {
            dcmtkFileNames.push_front( *iter );
        }

        OFListIterator(OFString) iter = dcmtkFileNames.begin();

        OFListIterator(OFString) last = dcmtkFileNames.end();

        if(iter == last) return;

        emit foundFilesToIndex(totalNumberOfFiles);

        /* iterate over all input filenames */
        int fileNumber = 0;
        int currentProgress = -1;
        d->Canceled = false;
        while (iter != last)
        {
            if (d->Canceled)
            {
                break;
            }
            emit indexingFileNumber(++fileNumber);
            int newProgress = ( fileNumber * 100 ) / totalNumberOfFiles;
            if (newProgress != currentProgress)
            {
                currentProgress = newProgress;
                emit progress( currentProgress );
            }
            QString filePath((*iter).c_str());
            this->addFile(ctkDICOMDatabase, filePath, destinationDirectoryName);
            ++iter;
        }
    //}
}
//------------------------------------------------------------------------------
void ctkDICOMIndexer::addFromDICOMDIR(ctkDICOMDatabase& ctkDICOMDatabase,
                                      const QString& directoryName,
                                      const QString& destinationDirectoryName
                                      )
{
    logger.debug( "############## addFromDICOMDIR ####################" );


    int fileNumber = 0;

    std::string dcmFilePath = directoryName.toStdString() + "/DICOMDIR";
    const char* dcmDirFilePath = dcmFilePath.c_str();


    DcmDicomDir* dicomDir = new DcmDicomDir(dcmDirFilePath);
    DcmDirectoryRecord* rootRecord = &(dicomDir->getRootRecord());
    DcmDirectoryRecord* patientRecord = NULL;
    DcmDirectoryRecord* studyRecord = NULL;
    DcmDirectoryRecord* seriesRecord = NULL;
    DcmDirectoryRecord* fileRecord = NULL;


    QString finalFilePath=destinationDirectoryName + "/dicom";
    ////QDir destinationDir;
    ////QFile currentFile;

    ////Values to fill database
    //OFString patientsName, patientID, patientsBirthDate, patientsBirthTime, patientsSex,
    //    patientComments, patientsAge;

    //OFString studyInstanceUID, studyID, studyDate, studyTime,
    //    accessionNumber, modalitiesInStudy, institutionName, performingPhysiciansName, referringPhysician, studyDescription;

    //OFString seriesInstanceUID, seriesDate, seriesTime,
    //    seriesDescription, bodyPartExamined, frameOfReferenceUID,
    //    contrastAgent, scanningSequence;
    //OFString instanceNumber, sopInstanceUID, referencedFileName ;

    //Sint32 seriesNumber = 0, acquisitionNumber = 0, echoNumber = 0, temporalPosition = 0;

    ////The patient UID is a unique number within the database, generated by the sqlite autoincrement
    ////Thus, this is _not_ the DICOM Patient ID.
    ////int patientUID(-1);
    ////int nrOfFiles(0);

    if(rootRecord != NULL)
    {
        //DcmDataset *patientDataset;

        while (((patientRecord = rootRecord->nextSub(patientRecord)) != NULL))
        {

            //if (patientRecord->findAndGetOFString(DCM_PatientName, patientsName).good()) {
            //    logger.debug( "Reading new Patients:" );
            //    logger.debug( "Patient's Name: " + QString(patientsName.c_str()) );

            //    patientDataset = new DcmDataset();

            //    patientRecord->findAndGetOFString(DCM_PatientID, patientID);
            //    patientRecord->findAndGetOFString(DCM_PatientBirthDate, patientsBirthDate);
            //    patientRecord->findAndGetOFString(DCM_PatientBirthTime, patientsBirthTime);
            //    patientRecord->findAndGetOFString(DCM_PatientSex, patientsSex);
            //    patientRecord->findAndGetOFString(DCM_PatientAge, patientsAge);
            //    patientRecord->findAndGetOFString(DCM_PatientComments, patientComments);

            //    patientDataset->putAndInsertOFStringArray(DCM_PatientName, patientsName);
            //    patientDataset->putAndInsertOFStringArray(DCM_PatientID, patientID);
            //    patientDataset->putAndInsertOFStringArray(DCM_PatientBirthDate, patientsBirthDate);
            //    patientDataset->putAndInsertOFStringArray(DCM_PatientBirthTime, patientsBirthTime);
            //    patientDataset->putAndInsertOFStringArray(DCM_PatientSex, patientsSex);
            //    patientDataset->putAndInsertOFStringArray(DCM_PatientAge, patientsAge);
            //    patientDataset->putAndInsertOFStringArray(DCM_PatientComments, patientComments);


            //}

            while (((studyRecord = patientRecord->nextSub(studyRecord)) != NULL))
            {

                //if (studyRecord->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID).good()) {
                //    logger.debug( "Reading new Studys:" );
                //    logger.debug( "Studies Name: " + QString(studyInstanceUID.c_str()) );

                //    studyRecord->findAndGetOFString(DCM_StudyID, studyID);
                //    studyRecord->findAndGetOFString(DCM_StudyDate, studyDate);
                //    studyRecord->findAndGetOFString(DCM_StudyTime, studyTime);
                //    studyRecord->findAndGetOFString(DCM_AccessionNumber, accessionNumber);
                //    studyRecord->findAndGetOFString(DCM_ModalitiesInStudy, modalitiesInStudy);
                //    studyRecord->findAndGetOFString(DCM_InstitutionName, institutionName);
                //    studyRecord->findAndGetOFString(DCM_PerformingPhysicianName, performingPhysiciansName);
                //    studyRecord->findAndGetOFString(DCM_ReferringPhysicianName, referringPhysician);
                //    studyRecord->findAndGetOFString(DCM_StudyDescription, studyDescription);

                //    patientDataset->putAndInsertOFStringArray(DCM_StudyInstanceUID, studyInstanceUID);
                //    patientDataset->putAndInsertOFStringArray(DCM_StudyID, studyID);
                //    patientDataset->putAndInsertOFStringArray(DCM_StudyDate, studyDate);
                //    patientDataset->putAndInsertOFStringArray(DCM_StudyTime, studyTime);
                //    patientDataset->putAndInsertOFStringArray(DCM_AccessionNumber, accessionNumber);
                //    patientDataset->putAndInsertOFStringArray(DCM_ModalitiesInStudy, modalitiesInStudy);
                //    patientDataset->putAndInsertOFStringArray(DCM_InstitutionName, institutionName);
                //    patientDataset->putAndInsertOFStringArray(DCM_PerformingPhysicianName, performingPhysiciansName);
                //    patientDataset->putAndInsertOFStringArray(DCM_ReferringPhysicianName, referringPhysician);
                //    patientDataset->putAndInsertOFStringArray(DCM_StudyDescription, studyDescription);

                    while (((seriesRecord = studyRecord->nextSub(seriesRecord)) != NULL))
                    {

                        //if (seriesRecord->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID).good()) {
                        //    logger.debug( "Reading new Series:" );
                        //    logger.debug( "Series Instance Name: " + QString(seriesInstanceUID.c_str()) );

                        //    seriesRecord->findAndGetOFString(DCM_SeriesDate, seriesDate);
                        //    seriesRecord->findAndGetOFString(DCM_SeriesTime, seriesTime);
                        //    seriesRecord->findAndGetOFString(DCM_SeriesDescription, seriesDescription);
                        //    seriesRecord->findAndGetOFString(DCM_BodyPartExamined, bodyPartExamined);
                        //    seriesRecord->findAndGetOFString(DCM_FrameOfReferenceUID, frameOfReferenceUID);
                        //    seriesRecord->findAndGetOFString(DCM_ContrastBolusAgent, contrastAgent);
                        //    seriesRecord->findAndGetOFString(DCM_ScanningSequence, scanningSequence);
                        //    seriesRecord->findAndGetSint32(DCM_SeriesNumber, seriesNumber);
                        //    seriesRecord->findAndGetSint32(DCM_AcquisitionNumber, acquisitionNumber);
                        //    seriesRecord->findAndGetSint32(DCM_EchoNumbers, echoNumber);
                        //    seriesRecord->findAndGetSint32(DCM_TemporalPositionIdentifier, temporalPosition);

                        //    patientDataset->putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID);
                        //    patientDataset->putAndInsertOFStringArray(DCM_SeriesDate, seriesDate);
                        //    patientDataset->putAndInsertOFStringArray(DCM_SeriesTime, seriesTime);
                        //    patientDataset->putAndInsertOFStringArray(DCM_SeriesDescription, seriesDescription);
                        //    patientDataset->putAndInsertOFStringArray(DCM_BodyPartExamined, bodyPartExamined);
                        //    patientDataset->putAndInsertOFStringArray(DCM_FrameOfReferenceUID, frameOfReferenceUID);
                        //    patientDataset->putAndInsertOFStringArray(DCM_ContrastBolusAgent, contrastAgent);
                        //    patientDataset->putAndInsertOFStringArray(DCM_ScanningSequence, scanningSequence);
                        //    patientDataset->putAndInsertSint16(DCM_SeriesNumber, seriesNumber);
                        //    patientDataset->putAndInsertSint16(DCM_AcquisitionNumber, acquisitionNumber);
                        //    patientDataset->putAndInsertSint16(DCM_EchoNumbers, echoNumber);
                        //    patientDataset->putAndInsertSint16(DCM_TemporalPositionIdentifier, temporalPosition);

                        //}


                        while (((fileRecord = seriesRecord->nextSub(fileRecord)) != NULL))
                        {
                            //if (fileRecord->findAndGetOFStringArray(DCM_ReferencedSOPInstanceUIDInFile, sopInstanceUID).good()) {

                            //    fileRecord->findAndGetOFString(DCM_ReferencedFileID,referencedFileName);

                            //    patientDataset->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUIDInFile, sopInstanceUID);
                            //    patientDataset->putAndInsertOFStringArray(DCM_ReferencedFileID, referencedFileName);


                            //    const char* filename = fileRecord->getRecordsOriginFile();
                            //    if(filename)
                            //    {
                            //        this->addFile();
                            //    }

                            //}

                            
                            emit indexingFileNumber(++fileNumber);
                            //TODO insert Record into Database Info: the database.insert() interface must change.
                            //it must be possible to add records and dataset
                            //suggestion: change the interface to dcmItem
                        }

                    }
                }
            }

        emit foundFilesToIndex(fileNumber);
    }
}


//------------------------------------------------------------------------------
void ctkDICOMIndexer::refreshDatabase(ctkDICOMDatabase& dicomDatabase, const QString& directoryName)
{
    Q_UNUSED(dicomDatabase);
    Q_UNUSED(directoryName);
    /*
    * Probably this should go to the database class as well
    * Or we have to extend the interface to make possible what we do here
    * without using SQL directly


    /// get all filenames from the database
    QSqlQuery allFilesQuery(dicomDatabase.database());
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

//----------------------------------------------------------------------------
void ctkDICOMIndexer::cancel()
{
    Q_D(ctkDICOMIndexer);
    d->Canceled = true;
}
