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
#include "ctkDICOMRetrieve.h"
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

#include <dcmtk/dcmjpeg/djdecode.h>  /* for dcmjpeg decoders */
#include <dcmtk/dcmjpeg/djencode.h>  /* for dcmjpeg encoders */
#include <dcmtk/dcmdata/dcrledrg.h>  /* for DcmRLEDecoderRegistration */
#include <dcmtk/dcmdata/dcrleerg.h>  /* for DcmRLEEncoderRegistration */

#include <dcmtk/dcmnet/scu.h>

#include "dcmtk/oflog/oflog.h"

static ctkLogger logger("org.commontk.dicom.DICOMRetrieve");

//------------------------------------------------------------------------------
class ctkDICOMRetrievePrivate
{
public:
  ctkDICOMRetrievePrivate();
  ~ctkDICOMRetrievePrivate();
  QString CallingAETitle;
  QString CalledAETitle;
  QString Host;
  int CallingPort;
  int CalledPort;
  DcmSCU SCU;
  DcmDataset* parameters;
  QString MoveDestinationAETitle;

  // do the retrieve, handling both series and study retrieves
  enum RetrieveType { RetrieveSeries, RetrieveStudy };
  void retrieve ( QString UID, QDir directory, RetrieveType retriveType );
};

//------------------------------------------------------------------------------
// ctkDICOMRetrievePrivate methods

//------------------------------------------------------------------------------
ctkDICOMRetrievePrivate::ctkDICOMRetrievePrivate()
{
  parameters = new DcmDataset();
}

//------------------------------------------------------------------------------
ctkDICOMRetrievePrivate::~ctkDICOMRetrievePrivate()
{
  delete parameters;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrievePrivate::retrieve ( QString UID, QDir directory, RetrieveType retriveType ) {

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

  // Set the DCMTK log level
  log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();
  rootLogger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);

  DcmSCU scu;
  scu.setAETitle ( OFString(CallingAETitle.toStdString().c_str()) );
  scu.setPort ( CallingPort );
  scu.setPeerAETitle ( OFString(CalledAETitle.toStdString().c_str()) );
  scu.setPeerHostName ( OFString(Host.toStdString().c_str()) );
  scu.setPeerPort ( CalledPort );
  scu.setMoveDestinationAETitle ( OFString(MoveDestinationAETitle.toStdString().c_str()) );

  logger.info ( "Setting Transfer Syntaxes" );
  OFList<OFString> transferSyntaxes;
  transferSyntaxes.push_back ( UID_LittleEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_BigEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_LittleEndianImplicitTransferSyntax );
  scu.addPresentationContext ( UID_FINDStudyRootQueryRetrieveInformationModel, transferSyntaxes );
  scu.addPresentationContext ( UID_MOVEStudyRootQueryRetrieveInformationModel, transferSyntaxes );


  if ( !scu.initNetwork().good() ) 
    {
    logger.error ( "Error initializing the network" );
    return;
    }
  logger.debug ( "Negotiating Association" );
  if ( !scu.negotiateAssociation().good() )
    {
    logger.error ( "Error negotiating association" );
    return;
    }

  logger.debug ( "Setting Parameters" );
  // Clear the query
  unsigned long elements = this->parameters->card();
  // Clean it out
  for ( unsigned long i = 0; i < elements; i++ ) 
    {
    this->parameters->remove ( 0ul );
    }
  if ( retriveType == RetrieveSeries )
    {
    this->parameters->putAndInsertString ( DCM_QueryRetrieveLevel, "SERIES" );
    this->parameters->putAndInsertString ( DCM_SeriesInstanceUID, UID.toStdString().c_str() );
    } 
  else
    {
    this->parameters->putAndInsertString ( DCM_QueryRetrieveLevel, "STUDY" );
    this->parameters->putAndInsertString ( DCM_StudyInstanceUID, UID.toStdString().c_str() );  
    }

  logger.debug ( "Sending Move Request" );
  MOVEResponses *responses = new MOVEResponses();
  OFCondition status = scu.sendMOVERequest ( 0, this->parameters, responses );
  if ( status.good() )
    {
    logger.debug ( "Find succeded" );

    logger.debug ( "Making Output Directory" );
    // Try to create the directory
    directory.mkpath ( directory.absolutePath() );

    if ( responses->begin() == responses->end() )
      {
      logger.error ( "No responses!" );
      }

    // Write the responses out to disk
    for ( OFListIterator(FINDResponse*) it = responses->begin(); it != responses->end(); it++ )
      {
      DcmDataset *dataset = (*it)->m_dataset;
      if ( dataset != NULL )
        {
        logger.debug ( "Got a valid dataset" );
        // Save in correct directory
        E_TransferSyntax output_transfersyntax = dataset->getOriginalXfer();
        dataset->chooseRepresentation( output_transfersyntax, NULL );
        
        if ( !dataset->canWriteXfer( output_transfersyntax ) )
          {
          // Pick EXS_LittleEndianExplicit as our default
          output_transfersyntax = EXS_LittleEndianExplicit;
          }
        
        DcmXfer opt_oxferSyn( output_transfersyntax );
        if ( !dataset->chooseRepresentation( opt_oxferSyn.getXfer(), NULL ).bad() )
          {
          DcmFileFormat* fileformat = new DcmFileFormat ( dataset );
          
          // Follow dcmdjpeg example
          fileformat->loadAllDataIntoMemory();
          OFString SOPInstanceUID;
          dataset->findAndGetOFString ( DCM_SOPInstanceUID, SOPInstanceUID );
          QFileInfo fi ( directory, QString ( SOPInstanceUID.c_str() ) );
          logger.debug ( "Saving file: " + fi.absoluteFilePath() );
          status = fileformat->saveFile ( fi.absoluteFilePath().toStdString().c_str(), opt_oxferSyn.getXfer() );
          if ( !status.good() )
            {
            logger.error ( "Error saving file: " + fi.absoluteFilePath() + " Error is " + status.text() );
            }
          
          delete fileformat;
          }
        }
      }
    }
  else
    {
    logger.error ( "MOVE Request failed: " + QString ( status.text() ) );
    }
  delete responses;
}

//------------------------------------------------------------------------------
// ctkDICOMRetrieve methods

//------------------------------------------------------------------------------
ctkDICOMRetrieve::ctkDICOMRetrieve()
   : d_ptr(new ctkDICOMRetrievePrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMRetrieve::~ctkDICOMRetrieve()
{
}

//------------------------------------------------------------------------------
/// Set methods for connectivity
void ctkDICOMRetrieve::setCallingAETitle ( QString callingAETitle )
{
  Q_D(ctkDICOMRetrieve);
  d->CallingAETitle = callingAETitle;
}

//------------------------------------------------------------------------------
const QString& ctkDICOMRetrieve::callingAETitle() 
{
  Q_D(ctkDICOMRetrieve);
  return d->CallingAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCalledAETitle ( QString calledAETitle )
{
  Q_D(ctkDICOMRetrieve);
  d->CalledAETitle = calledAETitle;
}

//------------------------------------------------------------------------------
const QString& ctkDICOMRetrieve::calledAETitle()
{
  Q_D(ctkDICOMRetrieve);
  return d->CalledAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setHost ( QString host )
{
  Q_D(ctkDICOMRetrieve);
  d->Host = host;
}

//------------------------------------------------------------------------------
const QString& ctkDICOMRetrieve::host()
{
  Q_D(ctkDICOMRetrieve);
  return d->Host;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCallingPort ( int port ) 
{
  Q_D(ctkDICOMRetrieve);
  d->CallingPort = port;
}

//------------------------------------------------------------------------------
int ctkDICOMRetrieve::callingPort()
{
  Q_D(ctkDICOMRetrieve);
  return d->CallingPort;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCalledPort ( int port ) 
{
  Q_D(ctkDICOMRetrieve);
  d->CalledPort = port;
}

//------------------------------------------------------------------------------
int ctkDICOMRetrieve::calledPort()
{
  Q_D(ctkDICOMRetrieve);
  return d->CalledPort;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setMoveDestinationAETitle ( QString moveDestinationAETitle )
{
  Q_D(ctkDICOMRetrieve);
  d->MoveDestinationAETitle = moveDestinationAETitle;
}
//------------------------------------------------------------------------------
const QString& ctkDICOMRetrieve::moveDestinationAETitle()
{
  Q_D(ctkDICOMRetrieve);
  return d->MoveDestinationAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::retrieveSeries ( QString seriesInstanceUID, QDir directory ) {
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting retrieveSeries" );
  d->retrieve ( seriesInstanceUID, directory, ctkDICOMRetrievePrivate::RetrieveSeries );
  return;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::retrieveStudy ( QString studyInstanceUID, QDir directory ) {
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting retrieveStudy" );
  d->retrieve ( studyInstanceUID, directory, ctkDICOMRetrievePrivate::RetrieveStudy );
  return;
}

