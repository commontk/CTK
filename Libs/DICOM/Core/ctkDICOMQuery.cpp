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
#include "ctkDICOMUtil.h"
#include "ctkLogger.h"

// DCMTK includes
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/scu.h"

#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/oflist.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */


static ctkLogger logger ( "org.commontk.dicom.DICOMQuery" );

//------------------------------------------------------------------------------
// A customized implemenation so that Qt signals can be emitted
// when query results are obtained
class ctkDICOMQuerySCUPrivate : public DcmSCU
{
public:
  ctkDICOMQuery *query;
  ctkDICOMQuerySCUPrivate()
    {
    this->query = 0;
    };
  ~ctkDICOMQuerySCUPrivate() {};
  virtual OFCondition handleFINDResponse(const T_ASC_PresentationContextID  presID,
                                         QRResponse *response,
                                         OFBool &waitForNextResponse)
    {
      if (this->query)
        {
        logger.debug ( "FIND RESPONSE" );
        emit this->query->debug("Got a find response!");
        return this->DcmSCU::handleFINDResponse(presID, response, waitForNextResponse);
        }
      return DIMSE_NULLKEY;
    };
};

//------------------------------------------------------------------------------
class ctkDICOMQueryPrivate
{
public:
  ctkDICOMQueryPrivate();
  ~ctkDICOMQueryPrivate();

  /// Add a StudyInstanceUID to be queried
  void addStudyInstanceUIDAndDataset(const QString& StudyInstanceUID, DcmDataset* dataset );

  QString                 CallingAETitle;
  QString                 CalledAETitle;
  QString                 Host;
  int                     Port;
  bool                    PreferCGET;
  QMap<QString,QVariant>  Filters;
  ctkDICOMQuerySCUPrivate SCU;
  DcmDataset*             Query;
  QStringList             StudyInstanceUIDList;
  QList<DcmDataset*>      StudyDatasetList;
  bool                    Canceled;
};

//------------------------------------------------------------------------------
// ctkDICOMQueryPrivate methods

//------------------------------------------------------------------------------
ctkDICOMQueryPrivate::ctkDICOMQueryPrivate()
{
  this->Query = new DcmDataset();
  this->Port = 0;
  this->Canceled = false;
  this->PreferCGET = false;
}

//------------------------------------------------------------------------------
ctkDICOMQueryPrivate::~ctkDICOMQueryPrivate()
{
  delete this->Query;
}

//------------------------------------------------------------------------------
void ctkDICOMQueryPrivate::addStudyInstanceUIDAndDataset( const QString& s, DcmDataset* dataset )
{
  this->StudyInstanceUIDList.append ( s );
  this->StudyDatasetList.append ( dataset );
}

//------------------------------------------------------------------------------
// ctkDICOMQuery methods

//------------------------------------------------------------------------------
ctkDICOMQuery::ctkDICOMQuery(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkDICOMQueryPrivate)
{
  Q_D(ctkDICOMQuery);
  d->SCU.query = this; // give the dcmtk level access to this for emitting signals
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
void ctkDICOMQuery::setPreferCGET ( bool preferCGET )
{
  Q_D(ctkDICOMQuery);
  d->PreferCGET = preferCGET;
}

//------------------------------------------------------------------------------
bool ctkDICOMQuery::preferCGET()const
{
  Q_D(const ctkDICOMQuery);
  return d->PreferCGET;
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
  // turn on logging if needed for debug:
  //ctk::setDICOMLogLevel(ctkErrorLogLevel::Debug);

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
  if (d->Canceled) {return false;}

  d->StudyInstanceUIDList.clear();
  d->SCU.setAETitle ( OFString(this->callingAETitle().toStdString().c_str()) );
  d->SCU.setPeerAETitle ( OFString(this->calledAETitle().toStdString().c_str()) );
  d->SCU.setPeerHostName ( OFString(this->host().toStdString().c_str()) );
  d->SCU.setPeerPort ( this->port() );

  logger.error ( "Setting Transfer Syntaxes" );
  emit progress("Setting Transfer Syntaxes");
  emit progress(10);
  if (d->Canceled) {return false;}

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
  emit progress("Negotiating Association");
  emit progress(20);
  if (d->Canceled) {return false;}

  OFCondition result = d->SCU.negotiateAssociation();
  if (result.bad())
    {
    logger.error( "Error negotiating the association: " + QString(result.text()) );
    emit progress("Error negotiating the association");
    emit progress(100);
    return false;
    }

  // Clear the query
  d->Query->clear();

  // Insert all keys that we like to receive values for
  d->Query->insertEmptyElement ( DCM_PatientID );
  d->Query->insertEmptyElement ( DCM_PatientName );
  d->Query->insertEmptyElement ( DCM_PatientBirthDate );
  d->Query->insertEmptyElement ( DCM_StudyID );
  d->Query->insertEmptyElement ( DCM_StudyInstanceUID );
  d->Query->insertEmptyElement ( DCM_StudyDescription );
  d->Query->insertEmptyElement ( DCM_StudyDate );
  d->Query->insertEmptyElement ( DCM_StudyTime );
  d->Query->insertEmptyElement ( DCM_ModalitiesInStudy );
  d->Query->insertEmptyElement ( DCM_AccessionNumber );
  d->Query->insertEmptyElement ( DCM_NumberOfStudyRelatedInstances ); // Number of images in the series
  d->Query->insertEmptyElement ( DCM_NumberOfStudyRelatedSeries ); // Number of series in the study

  // Make clear we define our search values in ISO Latin 1 (default would be ASCII)
  d->Query->putAndInsertOFStringArray(DCM_SpecificCharacterSet, "ISO_IR 100");

  d->Query->putAndInsertString ( DCM_QueryRetrieveLevel, "STUDY" );

  /* Now, for all keys that the user provided for filtering on STUDY level,
   * overwrite empty keys with value. For now, only Patient's Name, Patient ID,
   * Study Description, Modalities in Study, and Study Date are used.
   */
  QString seriesDescription;
  foreach( QString key, d->Filters.keys() )
    {
    if ( key == QString("Name") && !d->Filters[key].toString().isEmpty())
      {
      // make the filter a wildcard in dicom style
      d->Query->putAndInsertString( DCM_PatientName,
        (QString("*") + d->Filters[key].toString() + QString("*")).toLatin1().data());
      }
    else if ( key == QString("Study") && !d->Filters[key].toString().isEmpty())
      {
      // make the filter a wildcard in dicom style
      d->Query->putAndInsertString( DCM_StudyDescription,
        (QString("*") + d->Filters[key].toString() + QString("*")).toLatin1().data());
      }
    else if ( key == QString("ID") && !d->Filters[key].toString().isEmpty())
      {
      // make the filter a wildcard in dicom style
      d->Query->putAndInsertString( DCM_PatientID,
        (QString("*") + d->Filters[key].toString() + QString("*")).toLatin1().data());
      }
    else if (key == QString("AccessionNumber") && !d->Filters[key].toString().isEmpty())
      {
        // make the filter a wildcard in dicom style
        d->Query->putAndInsertString(DCM_AccessionNumber,
            (QString("*") + d->Filters[key].toString() + QString("*")).toLatin1().data());
      }
    else if ( key == QString("Modalities") && !d->Filters[key].toString().isEmpty())
      {
      // make the filter be an "OR" of modalities using backslash (dicom-style)
      QString modalitySearch("");
      foreach (const QString& modality, d->Filters[key].toStringList())
      {
        modalitySearch += modality + QString("\\");
      }
      modalitySearch.chop(1); // remove final backslash
      logger.debug("modalityInStudySearch " + modalitySearch);
      d->Query->putAndInsertString( DCM_ModalitiesInStudy, modalitySearch.toLatin1().data() );
      }
    // Rememer Series Description for later series query if we go through the keys now
    else if ( key == QString("Series") && !d->Filters[key].toString().isEmpty())
      {
      // make the filter a wildcard in dicom style
      seriesDescription = "*" + d->Filters[key].toString() + "*";
      }
    else
      {
      logger.debug("Ignoring unknown search key: " + key);
      }
    }

  if ( d->Filters.keys().contains("StartDate") && d->Filters.keys().contains("EndDate") )
    {
    QString dateRange = d->Filters["StartDate"].toString() +
                        QString("-") +
                        d->Filters["EndDate"].toString();
    d->Query->putAndInsertString ( DCM_StudyDate, dateRange.toLatin1().data() );
    logger.debug("Query on study date " + dateRange);
    }
  emit progress(30);
  if (d->Canceled) {return false;}

  OFList<QRResponse *> responses;

  Uint16 presentationContext = 0;
  // Check for any accepted presentation context for FIND in study root (dont care about transfer syntax)
  presentationContext = d->SCU.findPresentationContextID ( UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if ( presentationContext == 0 )
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
  if (d->Canceled) {return false;}

  OFCondition status = d->SCU.sendFINDRequest ( presentationContext, d->Query, &responses );
  if ( !status.good() )
    {
    logger.error ( "Find failed" );
    emit progress("Find failed");
    d->SCU.closeAssociation ( DCMSCU_RELEASE_ASSOCIATION );
    emit progress(100);
    return false;
    }
  logger.debug ( "Find succeded");
  emit progress("Find succeded");
  emit progress(50);
  if (d->Canceled) {return false;}

  for ( OFListIterator(QRResponse*) it = responses.begin(); it != responses.end(); it++ )
    {
    DcmDataset *dataset = (*it)->m_dataset;
    if ( dataset != NULL ) // the last response is always empty
      {
      database.insert ( dataset, false /* do not store to disk*/, false /* no thumbnail*/);
      OFString StudyInstanceUID;
      dataset->findAndGetOFString ( DCM_StudyInstanceUID, StudyInstanceUID );
      d->addStudyInstanceUIDAndDataset ( StudyInstanceUID.c_str(), dataset );
      emit progress(QString("Processing: ") + QString(StudyInstanceUID.c_str()));
      emit progress(50);
      if (d->Canceled) {return false;}
      }
    }

  /* Only ask for series attributes now. This requires kicking out the rest of former query. */
  d->Query->clear();
  d->Query->insertEmptyElement ( DCM_SeriesNumber );
  d->Query->insertEmptyElement ( DCM_SeriesDescription );
  d->Query->insertEmptyElement ( DCM_SeriesInstanceUID );
  d->Query->insertEmptyElement ( DCM_SeriesDate );
  d->Query->insertEmptyElement ( DCM_SeriesTime );
  d->Query->insertEmptyElement ( DCM_Modality );
  d->Query->insertEmptyElement ( DCM_NumberOfSeriesRelatedInstances ); // Number of images in the series

  /* Add user-defined filters */
  d->Query->putAndInsertOFStringArray(DCM_SeriesDescription, seriesDescription.toLatin1().data());

  // Now search each within each Study that was identified
  d->Query->putAndInsertString ( DCM_QueryRetrieveLevel, "SERIES" );
  float progressRatio = 25. / d->StudyInstanceUIDList.count();
  int i = 0; 

  QListIterator<DcmDataset*> datasetIterator(d->StudyDatasetList);
  foreach ( QString StudyInstanceUID, d->StudyInstanceUIDList )
    {
    DcmDataset *studyDataset = datasetIterator.next();
    DcmElement *patientName, *patientID;
    studyDataset->findAndGetElement(DCM_PatientName, patientName);
    studyDataset->findAndGetElement(DCM_PatientID, patientID);

    logger.debug ( "Starting Series C-FIND for Study: " + StudyInstanceUID );
    emit progress(QString("Starting Series C-FIND for Study: ") + StudyInstanceUID);
    emit progress(50 + (progressRatio * i++));
    if (d->Canceled) {return false;}

    d->Query->putAndInsertString ( DCM_StudyInstanceUID, StudyInstanceUID.toStdString().c_str() );
    OFList<QRResponse *> responses;
    status = d->SCU.sendFINDRequest ( presentationContext, d->Query, &responses );
    if ( status.good() )
      {
      for ( OFListIterator(QRResponse*) it = responses.begin(); it != responses.end(); it++ )
        {
        DcmDataset *dataset = (*it)->m_dataset;
        if ( dataset != NULL )
          {
          // add the patient elements not provided for the series level query
          dataset->insert( patientName, true );
          dataset->insert( patientID, true );
          // insert series dataset 
          database.insert ( dataset, false /* do not store */, false /* no thumbnail */ );
          }
        }
      logger.debug ( "Find succeded on Series level for Study: " + StudyInstanceUID );
      emit progress(QString("Find succeded on Series level for Study: ") + StudyInstanceUID);
      emit progress(50 + (progressRatio * i++));
      if (d->Canceled) {return false;}
      }
    else
      {
      logger.error ( "Find on Series level failed for Study: " + StudyInstanceUID );
      emit progress(QString("Find on Series level failed for Study: ") + StudyInstanceUID);
      }
    emit progress(50 + (progressRatio * i++));
    if (d->Canceled) {return false;}
    }
  d->SCU.closeAssociation ( DCMSCU_RELEASE_ASSOCIATION );
  emit progress(100);
  return true;
}

//----------------------------------------------------------------------------
void ctkDICOMQuery::cancel()
{
  Q_D(ctkDICOMQuery);
  d->Canceled = true;
}
