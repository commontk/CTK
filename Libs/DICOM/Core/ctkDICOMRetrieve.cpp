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
  int           CallingPort; // TODO: Not used yet since C-STORE receiver must be run separately so far
  int           CalledPort;
  DcmSCU        SCU; // TODO: not used yet.
  DcmDataset*   parameters;
  QString       MoveDestinationAETitle;
  QSharedPointer<ctkDICOMDatabase> RetrieveDatabase;

  // do the retrieve, handling both series and study retrieves
  enum RetrieveType { RetrieveSeries, RetrieveStudy };
  bool retrieve ( const QString& studyInstanceUID,
                  const QString& seriesInstanceUID,
                  const RetrieveType rType );
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
bool ctkDICOMRetrievePrivate::retrieve ( const QString& studyInstanceUID,
                                         const QString& seriesInstanceUID,
                                         const RetrieveType rType )
{

  if ( !this->RetrieveDatabase )
    {
    logger.error ( "No RetrieveDatabase for retrieve transaction" );
    return false;
    }

  // Register the JPEG libraries in case we need them
  // (registration only happens once, so it's okay to call repeatedly)
  // register global JPEG decompression codecs
  DJDecoderRegistration::registerCodecs();
  // register global JPEG compression codecs
  DJEncoderRegistration::registerCodecs();
  // register RLE compression codec
  DcmRLEEncoderRegistration::registerCodecs();
  // register RLE decompression codec
  DcmRLEDecoderRegistration::registerCodecs();

  // Set the DCMTK log level
  dcmtk::log4cplus::Logger rootLogger = dcmtk::log4cplus::Logger::getRoot();
  rootLogger.setLogLevel(dcmtk::log4cplus::DEBUG_LOG_LEVEL);

  // TODO: use this->SCU instead?
  DcmSCU scu;
  scu.setAETitle ( OFString(this->CallingAETitle.toStdString().c_str()) );
  scu.setPeerAETitle ( OFString(this->CalledAETitle.toStdString().c_str()) );
  scu.setPeerHostName ( OFString(this->Host.toStdString().c_str()) );
  scu.setPeerPort ( this->CalledPort );

  logger.info ( "Setting Transfer Syntaxes" );
  OFList<OFString> transferSyntaxes;
  transferSyntaxes.push_back ( UID_LittleEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_BigEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_LittleEndianImplicitTransferSyntax );
  scu.addPresentationContext ( UID_MOVEStudyRootQueryRetrieveInformationModel, transferSyntaxes );

  // Check and initialize networking parameters in DCMTK
  if ( !scu.initNetwork().good() )
    {
    logger.error ( "Error initializing the network" );
    return false;
    }

  // Negotiate (i.e. start the) association
  logger.debug ( "Negotiating Association" );

  if ( !scu.negotiateAssociation().good() )
    {
    logger.error ( "Error negotiating association" );
    return false;;
    }

  // Setup query what to be received from the PACS
  logger.debug ( "Setting Parameters" );
  // Clear the query
  parameters->clear();
  if ( rType == RetrieveSeries )
    {
    this->parameters->putAndInsertString ( DCM_QueryRetrieveLevel, "SERIES" );
    this->parameters->putAndInsertString ( DCM_SeriesInstanceUID, seriesInstanceUID.toStdString().c_str() );
    // Always required to send all highler level unique keys, so add study here (we are in Study Root)
    this->parameters->putAndInsertString ( DCM_StudyInstanceUID, studyInstanceUID.toStdString().c_str() );  //TODO
    }
  else
    {
    this->parameters->putAndInsertString ( DCM_QueryRetrieveLevel, "STUDY" );
    this->parameters->putAndInsertString ( DCM_StudyInstanceUID, studyInstanceUID.toStdString().c_str() );
    }

  // Issue request
  logger.debug ( "Sending Move Request" );
  MOVEResponses responses;
  T_ASC_PresentationContextID presID = scu.findPresentationContextID(UID_MOVEStudyRootQueryRetrieveInformationModel, "" /* don't care about transfer syntax */ );
  if (presID == 0)
    {
    logger.error ( "MOVE Request failed: No valid Study Root MOVE Presentation Context available" );
    scu.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
    return false;
    }
  OFCondition status = scu.sendMOVERequest ( presID, this->MoveDestinationAETitle.toStdString().c_str(), this->parameters, &responses );
  // Close association, no need to keep it open
  scu.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);

  // If we do not receive a single response, something is fishy
  if ( responses.begin() == responses.end() )
    {
    logger.error ( "No responses received at all! (at least one empty response always expected)" );
    throw std::runtime_error( std::string("No responses received from server!") );
    }

  /* The server is permitted to acknowledge every image that was received, or
   * to send a single move response.
   * If there is only a single response, this can mean the following:
   * 1) No images to transfer (Status Success and Number of Completed Subops = 0)
   * 2) All images transferred (Status Success and Number of Completed Subops > 0)
   * 3) Error code, i.e. no images transferred
   * 4) Warning (one or more failures, i.e. some images transferred)
   */
  if ( responses.numResults() == 1 )
    {
    MOVEResponse* rsp = *responses.begin();
    logger.debug ( "MOVE response receveid with status: " + QString(DU_cmoveStatusString(rsp->m_status)) );
    if ((rsp->m_status == STATUS_Success) || (rsp->m_status == STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures))
      {
      if (rsp->m_numberOfCompletedSubops == 0)
        {
        logger.error ( "No images transferred by PACS!" );
        throw std::runtime_error( std::string("No images transferred by PACS!") );
        }
      }
    else
      {
      logger.error("MOVE request failed, server does report error");
      QString statusDetail("No details");
      if (rsp->m_statusDetail != NULL)
        {
         std::ostringstream out;
        rsp->m_statusDetail->print(out);
        statusDetail = "Status Detail: " + statusDetail.fromStdString(out.str());
        }
      statusDetail.prepend("MOVE request failed: ");
      logger.error(statusDetail);
      throw std::runtime_error( statusDetail.toStdString() );
      }
    }
  // Select the last MOVE response to output meaningful status information
  OFListIterator(MOVEResponse*) it = responses.begin();
  Uint32 numResults = responses.numResults();
  for (Uint32 i = 1; i < numResults; i++)
    {
    it++;
    }
  logger.debug ( "MOVE responses report for study: " + studyInstanceUID +"\n"
    + QString::number((*it)->m_numberOfCompletedSubops) + " images transferred, and\n"
    + QString::number((*it)->m_numberOfWarningSubops)   + " images transferred with warning, and\n"
    + QString::number((*it)->m_numberOfFailedSubops)    + " images transfers failed");

  /* Comment from Michael: The code below does not make sense. Using MOVE you never
   * receive the image here but only status information; thus, rsp->m_dataset is _not_
   * an image. I leave it inside since it might be moved to a location which makes more
   * sense.
   */

 // for ( OFListIterator(MOVEResponse*) it = responses.begin(); it != responses.end(); it++ )
 // {
 //    DcmDataset *dataset = (*it)->m_dataset;
 //   if ( dataset != NULL )
 //   {
 //     logger.debug ( "Got a valid dataset" );
 //     // Save in correct directory
 //     E_TransferSyntax output_transfersyntax = dataset->getOriginalXfer();
 //     dataset->chooseRepresentation( output_transfersyntax, NULL );
 //
 //     if ( !dataset->canWriteXfer( output_transfersyntax ) )
 //     {
 //       // Pick EXS_LittleEndianExplicit as our default
 //       output_transfersyntax = EXS_LittleEndianExplicit;
 //     }
 //
 //     DcmXfer opt_oxferSyn( output_transfersyntax );
 //     if ( !dataset->chooseRepresentation( opt_oxferSyn.getXfer(), NULL ).bad() )
 //     {
 //       DcmFileFormat fileformat( dataset );
 //
 //       // Follow dcmdjpeg example
 //       OFString SOPInstanceUID;
 //       dataset->findAndGetOFString ( DCM_SOPInstanceUID, SOPInstanceUID );
 //       QFileInfo fi ( directory, QString ( SOPInstanceUID.c_str() ) );
 //       logger.debug ( "Saving file: " + fi.absoluteFilePath() );
 //       status = fileformat.saveFile ( fi.absoluteFilePath().toStdString().c_str(), opt_oxferSyn.getXfer() );
 //       if ( !status.good() )
 //       {
 //         logger.error ( "Error saving file: " + fi.absoluteFilePath() + " Error is " + status.text() );
 //       }
 //      // Insert into our local database
 //       RetrieveDatabase->insert( dataset, true );
 //     }
 //   }
 // }
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
bool ctkDICOMRetrieve::retrieveSeries(const QString& studyInstanceUID,
                                      const QString& seriesInstanceUID )
{
  if (studyInstanceUID.isEmpty() || seriesInstanceUID.isEmpty())
    {
    logger.error("Cannot receive series: Either Study or Series Instance UID empty.");
    return false;
    }
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting retrieveSeries" );
  return d->retrieve ( studyInstanceUID, seriesInstanceUID, ctkDICOMRetrievePrivate::RetrieveSeries );
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::retrieveStudy( const QString& studyInstanceUID )
{
  if (studyInstanceUID.isEmpty())
    {
    logger.error("Cannot receive series: Either Study or Series Instance UID empty.");
    return false;
    }
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting retrieveStudy" );
  return d->retrieve ( studyInstanceUID, "", ctkDICOMRetrievePrivate::RetrieveStudy );
}
