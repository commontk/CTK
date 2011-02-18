/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

// Qt includes
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QDate>
#include <QStringList>
#include <QSet>
#include <QFile>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

// ctkDICOMCore includes
#include "ctkDICOMQuery.h"
#include "ctkLogger.h"

// DCMTK includes
#ifndef WIN32
  #define HAVE_CONFIG_H 
#endif
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"

#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */

#include <dcmtk/dcmnet/scu.h>

static ctkLogger logger ( "org.commontk.dicom.DICOMQuery" );

//------------------------------------------------------------------------------
class ctkDICOMQueryPrivate
{
public:
  ctkDICOMQueryPrivate();
  ~ctkDICOMQueryPrivate();

  /// Add a StudyInstanceUID to be queried
  void addStudyInstanceUID(const QString& StudyInstanceUID );

  QString                 CallingAETitle;
  QString                 CalledAETitle;
  QString                 Host;
  int                     Port;
  QMap<QString,QVariant>  Filters;
  DcmSCU                  SCU;
  DcmDataset*             Query;
  QStringList             StudyInstanceUIDList;
};

//------------------------------------------------------------------------------
// ctkDICOMQueryPrivate methods

//------------------------------------------------------------------------------
ctkDICOMQueryPrivate::ctkDICOMQueryPrivate()
{
  this->Query = new DcmDataset();
  this->Port = 0;
}

//------------------------------------------------------------------------------
ctkDICOMQueryPrivate::~ctkDICOMQueryPrivate()
{
  delete this->Query;
}

//------------------------------------------------------------------------------
void ctkDICOMQueryPrivate::addStudyInstanceUID( const QString& s )
{
  this->StudyInstanceUIDList.append ( s );
}

//------------------------------------------------------------------------------
// ctkDICOMQuery methods

//------------------------------------------------------------------------------
ctkDICOMQuery::ctkDICOMQuery(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkDICOMQueryPrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMQuery::~ctkDICOMQuery()
{
}

/// Set methods for connectivity
//------------------------------------------------------------------------------
void ctkDICOMQuery::setCallingAETitle( const QString& callingAETitle )
{
  Q_D(ctkDICOMQuery);
  d->CallingAETitle = callingAETitle;
}

//------------------------------------------------------------------------------
QString ctkDICOMQuery::callingAETitle() const
{
  Q_D(const ctkDICOMQuery);
  return d->CallingAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMQuery::setCalledAETitle( const QString& calledAETitle )
{
  Q_D(ctkDICOMQuery);
  d->CalledAETitle = calledAETitle;
}

//------------------------------------------------------------------------------
QString ctkDICOMQuery::calledAETitle()const
{
  Q_D(const ctkDICOMQuery);
  return d->CalledAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMQuery::setHost( const QString& host )
{
  Q_D(ctkDICOMQuery);
  d->Host = host;
}

//------------------------------------------------------------------------------
QString ctkDICOMQuery::host() const
{
  Q_D(const ctkDICOMQuery);
  return d->Host;
}

//------------------------------------------------------------------------------
void ctkDICOMQuery::setPort ( int port ) 
{
  Q_D(ctkDICOMQuery);
  d->Port = port;
}

//------------------------------------------------------------------------------
int ctkDICOMQuery::port()const
{
  Q_D(const ctkDICOMQuery);
  return d->Port;
}

//------------------------------------------------------------------------------
void ctkDICOMQuery::setFilters( const QMap<QString,QVariant>& filters ) 
{
  Q_D(ctkDICOMQuery);
  d->Filters = filters;
}

//------------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMQuery::filters()const
{
  Q_D(const ctkDICOMQuery);
  return d->Filters;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMQuery::studyInstanceUIDQueried()const
{
  Q_D(const ctkDICOMQuery);
  return d->StudyInstanceUIDList;
}

//------------------------------------------------------------------------------
bool ctkDICOMQuery::query(ctkDICOMDatabase& database )
{
  // ctkDICOMDatabase::setDatabase ( database );
  Q_D(ctkDICOMQuery);
  // In the following, we emit progress(int) after progress(QString), this
  // is in case the connected object doesn't refresh its ui when the progress
  // message is updated but only if the progress value is (e.g. QProgressDialog)
  if ( database.database().isOpen() )
    {
    logger.debug ( "DB open in Query" );
    emit progress("DB open in Query");
    }
  else
    {
    logger.debug ( "DB not open in Query" );
    emit progress("DB not open in Query");
    }
  emit progress(0);

  d->StudyInstanceUIDList.clear();
  d->SCU.setAETitle ( OFString(this->callingAETitle().toStdString().c_str()) );
  d->SCU.setPeerAETitle ( OFString(this->calledAETitle().toStdString().c_str()) );
  d->SCU.setPeerHostName ( OFString(this->host().toStdString().c_str()) );
  d->SCU.setPeerPort ( this->port() );

  logger.error ( "Setting Transfer Syntaxes" );
  emit progress("Setting Transfer Syntaxes");
  emit progress(10);

  OFList<OFString> transferSyntaxes;
  transferSyntaxes.push_back ( UID_LittleEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_BigEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_LittleEndianImplicitTransferSyntax );

  d->SCU.addPresentationContext ( UID_FINDStudyRootQueryRetrieveInformationModel, transferSyntaxes );
  // d->SCU.addPresentationContext ( UID_VerificationSOPClass, transferSyntaxes );
  if ( !d->SCU.initNetwork().good() ) 
    {
    logger.error( "Error initializing the network" );
    emit progress("Error initializing the network");
    emit progress(100);
    return false;
    }
  logger.debug ( "Negotiating Association" );
  emit progress("Negatiating Association");
  emit progress(20);

  d->SCU.negotiateAssociation();

  // Clear the query
  unsigned long elements = d->Query->card();
  // Clean it out
  for ( unsigned long i = 0; i < elements; i++ ) 
    {
    d->Query->remove ( 0ul );
    }
  d->Query->insertEmptyElement ( DCM_PatientID );
  d->Query->insertEmptyElement ( DCM_PatientsName );
  d->Query->insertEmptyElement ( DCM_PatientsBirthDate );
  d->Query->insertEmptyElement ( DCM_StudyID );
  d->Query->insertEmptyElement ( DCM_StudyInstanceUID );
  d->Query->insertEmptyElement ( DCM_StudyDescription );
  d->Query->insertEmptyElement ( DCM_StudyDate );
  d->Query->insertEmptyElement ( DCM_SeriesNumber );
  d->Query->insertEmptyElement ( DCM_SeriesDescription );
  d->Query->insertEmptyElement ( DCM_SeriesInstanceUID );
  d->Query->insertEmptyElement ( DCM_StudyTime );
  d->Query->insertEmptyElement ( DCM_SeriesDate );
  d->Query->insertEmptyElement ( DCM_SeriesTime );
  d->Query->insertEmptyElement ( DCM_Modality );
  d->Query->insertEmptyElement ( DCM_ModalitiesInStudy );
  d->Query->insertEmptyElement ( DCM_AccessionNumber );
  d->Query->insertEmptyElement ( DCM_NumberOfSeriesRelatedInstances ); // Number of images in the series
  d->Query->insertEmptyElement ( DCM_NumberOfStudyRelatedInstances ); // Number of images in the series
  d->Query->insertEmptyElement ( DCM_NumberOfStudyRelatedSeries ); // Number of images in the series

  d->Query->putAndInsertString ( DCM_QueryRetrieveLevel, "STUDY" );

  foreach( QString key, d->Filters.keys() )
    {
    if ( key == QString("Name") )
      {
      // make the filter a wildcard in dicom style
      d->Query->putAndInsertString( DCM_PatientsName,
        (QString("*") + d->Filters[key].toString() + QString("*")).toAscii().data());
      }
    if ( key == QString("Study") )
      {
      // make the filter a wildcard in dicom style
      d->Query->putAndInsertString( DCM_StudyDescription,
        (QString("*") + d->Filters[key].toString() + QString("*")).toAscii().data());
      }
    if ( key == QString("Series") )
      {
      // make the filter a wildcard in dicom style
      d->Query->putAndInsertString( DCM_SeriesDescription,
        (QString("*") + d->Filters[key].toString() + QString("*")).toAscii().data());
      }
    if ( key == QString("ID") )
      {
      // make the filter a wildcard in dicom style
      d->Query->putAndInsertString( DCM_PatientID,
        (QString("*") + d->Filters[key].toString() + QString("*")).toAscii().data());
      }
    if ( key == QString("Modalities") )
      {
      // make the filter be an "OR" of modalities using backslash (dicom-style)
      QString modalitySearch("");
      foreach (const QString& modality, d->Filters[key].toStringList())
        {
        modalitySearch += modality + QString("\\");
        }
      modalitySearch.chop(1); // remove final backslash
      logger.debug("modalitySearch " + modalitySearch);
      d->Query->putAndInsertString( DCM_ModalitiesInStudy, modalitySearch.toAscii().data() );
      }
    }

  if ( d->Filters.keys().contains("StartDate") && d->Filters.keys().contains("EndDate") )
    {
    QString dateRange = d->Filters["StartDate"].toString() + 
                          QString("-") + 
                              d->Filters["EndDate"].toString();
    d->Query->putAndInsertString ( DCM_StudyDate, dateRange.toAscii().data() );
    logger.debug("Query on study time " + dateRange);
    }

  emit progress(30);

  FINDResponses *responses = new FINDResponses();

  Uint16 presentationContex = 0;
  presentationContex = d->SCU.findPresentationContextID ( UID_FINDStudyRootQueryRetrieveInformationModel, UID_LittleEndianExplicitTransferSyntax );
  if ( presentationContex == 0 )
    {
    presentationContex = d->SCU.findPresentationContextID ( UID_FINDStudyRootQueryRetrieveInformationModel, UID_BigEndianExplicitTransferSyntax );
    }
  if ( presentationContex == 0 )
    {
    presentationContex = d->SCU.findPresentationContextID ( UID_FINDStudyRootQueryRetrieveInformationModel, UID_LittleEndianImplicitTransferSyntax );
    }

  if ( presentationContex == 0 )
    {
    logger.error ( "Failed to find acceptable presentation context" );
    emit progress("Failed to find acceptable presentation context");
    }
  else
    {
    logger.info ( "Found useful presentation context" );
    emit progress("Found useful presentation context");
    }
  emit progress(40);

  OFCondition status = d->SCU.sendFINDRequest ( presentationContex, d->Query, responses );
  if ( !status.good() )
    {
    logger.error ( "Find failed" );
    emit progress("Find failed");
    d->SCU.closeAssociation ( DUL_PEERREQUESTEDRELEASE );
    emit progress(100);
    return false;
    }
  logger.debug ( "Find succeded" );
  emit progress("Find succeded");
  emit progress(50);

  for ( OFListIterator(FINDResponse*) it = responses->begin(); it != responses->end(); it++ )
    {
    DcmDataset *dataset = (*it)->m_dataset;
    if ( dataset != NULL )
      {
      database.insert ( dataset );
      OFString StudyInstanceUID;
      dataset->findAndGetOFString ( DCM_StudyInstanceUID, StudyInstanceUID );
      d->addStudyInstanceUID ( QString ( StudyInstanceUID.c_str() ) );
      }
    }
  delete responses;

  // Now search each Study
  d->Query->putAndInsertString ( DCM_QueryRetrieveLevel, "SERIES" );
  float progressRatio = 25. / d->StudyInstanceUIDList.count();
  int i = 0;
  foreach ( QString StudyInstanceUID, d->StudyInstanceUIDList )
    {
    logger.debug ( "Starting Series C-FIND for Series: " + StudyInstanceUID );
    emit progress(QString("Starting Series C-FIND for Series: ") + StudyInstanceUID);
    emit progress(50 + (progressRatio * i++));

    d->Query->putAndInsertString ( DCM_StudyInstanceUID, StudyInstanceUID.toStdString().c_str() );
    responses = new FINDResponses();
    status = d->SCU.sendFINDRequest ( 0, d->Query, responses );
    if ( status.good() )
      {
      for ( OFListIterator(FINDResponse*) it = responses->begin(); it != responses->end(); it++ )
        {
        DcmDataset *dataset = (*it)->m_dataset;
        if ( dataset != NULL )
          {
          database.insert ( dataset );
          }
        }
      logger.debug ( "Find succeded for Series: " + StudyInstanceUID );
      emit progress(QString("Find succeded for Series: ") + StudyInstanceUID);
      }
    else
      {
      logger.error ( "Find failed for Series: " + StudyInstanceUID );
      emit progress(QString("Find failed for Series: ") + StudyInstanceUID);
      }
    emit progress(50 + (progressRatio * i++));
    delete responses;
    }
  d->SCU.closeAssociation ( DUL_PEERREQUESTEDRELEASE );
  emit progress(100);
  return true;
}

