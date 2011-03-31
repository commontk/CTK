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

#include <stdexcept>

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
  QString       CallingAETitle;
  QString       CalledAETitle;
  QString       Host;
  int           CallingPort;
  int           CalledPort;
  DcmSCU        SCU; // TODO: not used yet.
  DcmDataset*   parameters;
  QString       MoveDestinationAETitle;
  QSharedPointer<ctkDICOMDatabase> RetrieveDatabase;

  // do the retrieve, handling both series and study retrieves
  enum RetrieveType { RetrieveSeries, RetrieveStudy };
  bool retrieve ( QString UID, RetrieveType retriveType );
};

//------------------------------------------------------------------------------
// ctkDICOMRetrievePrivate methods

//------------------------------------------------------------------------------
ctkDICOMRetrievePrivate::ctkDICOMRetrievePrivate()
{
  this->parameters = new DcmDataset();
  this->RetrieveDatabase = QSharedPointer<ctkDICOMDatabase> (0);
  this->CallingPort = 0;
  this->CalledPort = 0;
}

//------------------------------------------------------------------------------
ctkDICOMRetrievePrivate::~ctkDICOMRetrievePrivate()
{
  delete this->parameters;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrievePrivate::retrieve ( QString UID, RetrieveType retriveType ) {

  if ( !this->RetrieveDatabase )
    {
    logger.error ( "Must have RetrieveDatabase for retrieve transaction" );
    return false;
    }

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

  // TODO: use this->SCU instead ?
  DcmSCU scu;
  scu.setAETitle ( OFString(this->CallingAETitle.toStdString().c_str()) );
  scu.setPort ( this->CallingPort );
  scu.setPeerAETitle ( OFString(this->CalledAETitle.toStdString().c_str()) );
  scu.setPeerHostName ( OFString(this->Host.toStdString().c_str()) );
  scu.setPeerPort ( this->CalledPort );
  scu.setMoveDestinationAETitle ( OFString(this->MoveDestinationAETitle.toStdString().c_str()) );

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
    return false;
    }
  logger.debug ( "Negotiating Association" );
  if ( !scu.negotiateAssociation().good() )
    {
    logger.error ( "Error negotiating association" );
    return false;;
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
  if (!status.good())
    {
    logger.error ( "MOVE Request failed: " + QString ( status.text() ) );
    return false;
    }

  logger.debug ( "Find succeded" );

  logger.debug ( "Making Output Directory" );
  QDir directory = QDir( RetrieveDatabase->databaseDirectory() );

  if ( responses->begin() == responses->end() )
    {
    logger.error ( "No responses!" );
    throw std::runtime_error( std::string("No responses!") );
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

        RetrieveDatabase->insert( dataset, true );
          
        delete fileformat;
        }
      }
    }


  delete responses;
  if ( !scu.dropNetwork().good() ) 
    {
    logger.error ( "Error dropping the network" );
    return false;
    }
  return true;
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
void ctkDICOMRetrieve::setCallingAETitle( const QString& callingAETitle )
{
  Q_D(ctkDICOMRetrieve);
  d->CallingAETitle = callingAETitle;
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::callingAETitle() const
{
  Q_D(const ctkDICOMRetrieve);
  return d->CallingAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCalledAETitle( const QString& calledAETitle )
{
  Q_D(ctkDICOMRetrieve);
  d->CalledAETitle = calledAETitle;
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::calledAETitle()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->CalledAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setHost( const QString& host )
{
  Q_D(ctkDICOMRetrieve);
  d->Host = host;
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::host()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->Host;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCallingPort( int port )
{
  Q_D(ctkDICOMRetrieve);
  d->CallingPort = port;
}

//------------------------------------------------------------------------------
int ctkDICOMRetrieve::callingPort()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->CallingPort;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCalledPort( int port )
{
  Q_D(ctkDICOMRetrieve);
  d->CalledPort = port;
}

//------------------------------------------------------------------------------
int ctkDICOMRetrieve::calledPort()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->CalledPort;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setMoveDestinationAETitle( const QString& moveDestinationAETitle )
{
  Q_D(ctkDICOMRetrieve);
  d->MoveDestinationAETitle = moveDestinationAETitle;
}
//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::moveDestinationAETitle()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->MoveDestinationAETitle;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setRetrieveDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase)
{
  Q_D(ctkDICOMRetrieve);
  d->RetrieveDatabase = dicomDatabase;
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMRetrieve::retrieveDatabase()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->RetrieveDatabase;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::retrieveSeries( const QString& seriesInstanceUID )
{
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting retrieveSeries" );
  return d->retrieve ( seriesInstanceUID, ctkDICOMRetrievePrivate::RetrieveSeries );
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::retrieveStudy( const QString& studyInstanceUID )
{
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting retrieveStudy" );
  return d->retrieve ( studyInstanceUID, ctkDICOMRetrievePrivate::RetrieveStudy );
}
