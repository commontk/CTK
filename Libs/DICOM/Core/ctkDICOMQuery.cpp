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
  QString CallingAETitle;
  QString CalledAETitle;
  QString Host;
  int Port;
  DcmSCU SCU;
  DcmDataset* query;
  QStringList StudyInstanceUIDList;

};

//------------------------------------------------------------------------------
// ctkDICOMQueryPrivate methods

//------------------------------------------------------------------------------
ctkDICOMQueryPrivate::ctkDICOMQueryPrivate()
{
  query = new DcmDataset();
}

//------------------------------------------------------------------------------
ctkDICOMQueryPrivate::~ctkDICOMQueryPrivate()
{
  delete query;
}


//------------------------------------------------------------------------------
// ctkDICOMQuery methods

//------------------------------------------------------------------------------
ctkDICOMQuery::ctkDICOMQuery()
   : d_ptr(new ctkDICOMQueryPrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMQuery::~ctkDICOMQuery()
{
}

void ctkDICOMQuery::addStudyInstanceUID ( QString s )
{
  Q_D(ctkDICOMQuery);
  d->StudyInstanceUIDList.append ( s );
}

/// Set methods for connectivity
void ctkDICOMQuery::setCallingAETitle ( QString callingAETitle )
{
  Q_D(ctkDICOMQuery);
  d->CallingAETitle = callingAETitle;
}
const QString& ctkDICOMQuery::callingAETitle() 
{
  Q_D(ctkDICOMQuery);
  return d->CallingAETitle;
}
void ctkDICOMQuery::setCalledAETitle ( QString calledAETitle )
{
  Q_D(ctkDICOMQuery);
  d->CalledAETitle = calledAETitle;
}
const QString& ctkDICOMQuery::calledAETitle()
{
  Q_D(ctkDICOMQuery);
  return d->CalledAETitle;
}
void ctkDICOMQuery::setHost ( QString host )
{
  Q_D(ctkDICOMQuery);
  d->Host = host;
}
const QString& ctkDICOMQuery::host()
{
  Q_D(ctkDICOMQuery);
  return d->Host;
}
void ctkDICOMQuery::setPort ( int port ) 
{
  Q_D(ctkDICOMQuery);
  d->Port = port;
}
int ctkDICOMQuery::port()
{
  Q_D(ctkDICOMQuery);
  return d->Port;
}



//------------------------------------------------------------------------------
void ctkDICOMQuery::query(QSqlDatabase database )
{
  ctkDICOMIndexerBase::setDatabase ( database );
  Q_D(ctkDICOMQuery);
  if ( this->database().isOpen() )
    {
    logger.debug ( "DB open in Query" );
    }
  else
    {
    logger.debug ( "DB not open in Query" );
    }
  d->StudyInstanceUIDList.clear();
  d->SCU.setAETitle ( OFString(this->callingAETitle().toStdString().c_str()) );
  d->SCU.setPeerAETitle ( OFString(this->calledAETitle().toStdString().c_str()) );
  d->SCU.setPeerHostName ( OFString(this->host().toStdString().c_str()) );
  d->SCU.setPeerPort ( this->port() );

  logger.error ( "Setting Transfer Syntaxes" );
  OFList<OFString> transferSyntaxes;
  transferSyntaxes.push_back ( UID_LittleEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_BigEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_LittleEndianImplicitTransferSyntax );

  d->SCU.addPresentationContext ( UID_FINDStudyRootQueryRetrieveInformationModel, transferSyntaxes );
  // d->SCU.addPresentationContext ( UID_VerificationSOPClass, transferSyntaxes );
  if ( !d->SCU.initNetwork().good() ) 
    {
    std::cerr << "Error initializing the network" << std::endl;
    return;
    }
  logger.debug ( "Negotiating Association" );
  d->SCU.negotiateAssociation();

  // Clear the query
  unsigned long elements = d->query->card();
  // Clean it out
  for ( unsigned long i = 0; i < elements; i++ ) 
    {
    d->query->remove ( 0ul );
    }
  d->query->insertEmptyElement ( DCM_PatientID );
  d->query->insertEmptyElement ( DCM_PatientsName );
  d->query->insertEmptyElement ( DCM_PatientsBirthDate );
  d->query->insertEmptyElement ( DCM_StudyID );
  d->query->insertEmptyElement ( DCM_StudyInstanceUID );
  d->query->insertEmptyElement ( DCM_StudyDescription );
  d->query->insertEmptyElement ( DCM_StudyDate );
  d->query->insertEmptyElement ( DCM_StudyID );
  d->query->insertEmptyElement ( DCM_PatientID );
  d->query->insertEmptyElement ( DCM_PatientsName );
  d->query->insertEmptyElement ( DCM_SeriesNumber );
  d->query->insertEmptyElement ( DCM_SeriesDescription );
  d->query->insertEmptyElement ( DCM_StudyInstanceUID );
  d->query->insertEmptyElement ( DCM_SeriesInstanceUID );
  d->query->insertEmptyElement ( DCM_StudyTime );
  d->query->insertEmptyElement ( DCM_SeriesDate );
  d->query->insertEmptyElement ( DCM_SeriesTime );
  d->query->insertEmptyElement ( DCM_Modality );
  d->query->insertEmptyElement ( DCM_ModalitiesInStudy );
  d->query->insertEmptyElement ( DCM_AccessionNumber );
  d->query->insertEmptyElement ( DCM_NumberOfSeriesRelatedInstances ); // Number of images in the series
  d->query->insertEmptyElement ( DCM_NumberOfStudyRelatedInstances ); // Number of images in the series
  d->query->insertEmptyElement ( DCM_NumberOfStudyRelatedSeries ); // Number of images in the series
  d->query->putAndInsertString ( DCM_QueryRetrieveLevel, "STUDY" );

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
    }
  else
    {
    logger.info ( "Found useful presentation context" );
    }

  OFCondition status = d->SCU.sendFINDRequest ( presentationContex, d->query, responses );
  if ( status.good() )
    {
    logger.debug ( "Find succeded" );
    }
  else
    {
    logger.error ( "Find failed" );
    }

  for ( OFListIterator(FINDResponse*) it = responses->begin(); it != responses->end(); it++ )
    {
    DcmDataset *dataset = (*it)->m_dataset;
    if ( dataset != NULL )
      {
      this->insert ( dataset );
      OFString StudyInstanceUID;
      dataset->findAndGetOFString ( DCM_StudyInstanceUID, StudyInstanceUID );
      this->addStudyInstanceUID ( QString ( StudyInstanceUID.c_str() ) );
      }
    }
  delete responses;

  // Now search each Study
  d->query->putAndInsertString ( DCM_QueryRetrieveLevel, "SERIES" );
  foreach ( QString StudyInstanceUID, d->StudyInstanceUIDList )
    {
    logger.debug ( "Starting Series C-FIND for Series: " + StudyInstanceUID );
    d->query->putAndInsertString ( DCM_StudyInstanceUID, StudyInstanceUID.toStdString().c_str() );
    responses = new FINDResponses();
    status = d->SCU.sendFINDRequest ( 0, d->query, responses );
    if ( status.good() )
      {
      logger.debug ( "Find succeded for Series: " + StudyInstanceUID );
      for ( OFListIterator(FINDResponse*) it = responses->begin(); it != responses->end(); it++ )
        {
        DcmDataset *dataset = (*it)->m_dataset;
        if ( dataset != NULL )
          {
          this->insert ( dataset );
          }
        }
      }
    else
      {
      logger.error ( "Find failed for Series: " + StudyInstanceUID );
      }
    delete responses;
    }
  d->SCU.closeAssociation ( DUL_PEERREQUESTEDRELEASE );
}

