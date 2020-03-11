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

// ctkDICOMCore includes
#include "ctkDICOMRetrieve.h"
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
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */

#include <dcmtk/dcmjpeg/djdecode.h>  /* for dcmjpeg decoders */
#include <dcmtk/dcmjpeg/djencode.h>  /* for dcmjpeg encoders */
#include <dcmtk/dcmdata/dcrledrg.h>  /* for DcmRLEDecoderRegistration */
#include <dcmtk/dcmdata/dcrleerg.h>  /* for DcmRLEEncoderRegistration */

#include "dcmtk/oflog/oflog.h"

static ctkLogger logger("org.commontk.dicom.DICOMRetrieve");

//------------------------------------------------------------------------------
// A customized local implemenation of the DcmSCU so that Qt signals can be emitted
// when retrieve results are obtained
class ctkDICOMRetrieveSCUPrivate : public DcmSCU
{
public:
  ctkDICOMRetrieve *retrieve;
  ctkDICOMRetrieveSCUPrivate()
    {
    this->retrieve = 0;
    };
  ~ctkDICOMRetrieveSCUPrivate() {};

  // called when a move reponse comes in: indicates that the
  // move request is being handled by the remote server.
  virtual OFCondition handleMOVEResponse(const T_ASC_PresentationContextID  presID,
                                         RetrieveResponse *response,
                                         OFBool &waitForNextResponse)
    {
      if (this->retrieve)
        {
        emit this->retrieve->progress("Got move request");
        emit this->retrieve->progress(0);
        return this->DcmSCU::handleMOVEResponse(
                        presID, response, waitForNextResponse);
        }
      //return false;
      return EC_IllegalCall;
    };

  // called when a data set is coming in from a server in
  // response to a CGET
  virtual OFCondition handleSTORERequest(const T_ASC_PresentationContextID presID,
                                         DcmDataset *incomingObject,
                                         OFBool& continueCGETSession,
                                         Uint16& cStoreReturnStatus)
    {
      if (this->retrieve)
        {
        OFString instanceUID;
        incomingObject->findAndGetOFString(DCM_SOPInstanceUID, instanceUID);
        QString qInstanceUID(instanceUID.c_str());
        emit this->retrieve->progress("Got STORE request for " + qInstanceUID);
        emit this->retrieve->progress(0);
        continueCGETSession = !this->retrieve->wasCanceled();
        if (this->retrieve && this->retrieve->database())
          {
          this->retrieve->database()->insert(incomingObject);
          return EC_Normal;
          }
        else
          {
          return this->DcmSCU::handleSTORERequest(
                          presID, incomingObject, continueCGETSession, cStoreReturnStatus);
          }
        }
      //return false;
      return EC_IllegalCall;
    };

  // called when status information from remote server
  // comes in from CGET
  virtual OFCondition handleCGETResponse(const T_ASC_PresentationContextID presID,
                                         RetrieveResponse* response,
                                         OFBool& continueCGETSession)
    {
      if (this->retrieve)
        {
        emit this->retrieve->progress("Got CGET response");
        emit this->retrieve->progress(0);
        continueCGETSession = !this->retrieve->wasCanceled();
        return this->DcmSCU::handleCGETResponse(presID, response, continueCGETSession);
        }
      //return false;
      return EC_IllegalCall;
    };
};


//------------------------------------------------------------------------------
class ctkDICOMRetrievePrivate: public QObject
{
  Q_DECLARE_PUBLIC( ctkDICOMRetrieve );

protected:
  ctkDICOMRetrieve* const q_ptr;

public:
  ctkDICOMRetrievePrivate(ctkDICOMRetrieve& obj);
  ~ctkDICOMRetrievePrivate();
  /// Keep the currently negotiated connection to the 
  /// peer host open unless the connection parameters change
  bool          WasCanceled;
  bool          KeepAssociationOpen;
  bool          ConnectionParamsChanged;
  bool          LastRetrieveType;
  QSharedPointer<ctkDICOMDatabase> Database;
  ctkDICOMRetrieveSCUPrivate        SCU;
  QString MoveDestinationAETitle;
  // do the retrieve, handling both series and study retrieves
  enum RetrieveType { RetrieveNone, RetrieveSeries, RetrieveStudy };
  bool initializeSCU(const QString& studyInstanceUID,
                  const QString& seriesInstanceUID,
                  const RetrieveType retrieveType,
                  DcmDataset *retrieveParameters);
  bool move ( const QString& studyInstanceUID,
                  const QString& seriesInstanceUID,
                  const RetrieveType retrieveType );
  bool get ( const QString& studyInstanceUID,
                  const QString& seriesInstanceUID,
                  const RetrieveType retrieveType );
};

//------------------------------------------------------------------------------
// ctkDICOMRetrievePrivate methods

//------------------------------------------------------------------------------
ctkDICOMRetrievePrivate::ctkDICOMRetrievePrivate(ctkDICOMRetrieve& obj)
  : q_ptr(&obj)
{
  this->Database = QSharedPointer<ctkDICOMDatabase> (0);
  this->WasCanceled = false;
  this->KeepAssociationOpen = true;
  this->ConnectionParamsChanged = false;
  this->LastRetrieveType = RetrieveNone;

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

  logger.info ( "Setting Transfer Syntaxes" );
  OFList<OFString> transferSyntaxes;
  transferSyntaxes.push_back ( UID_LittleEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_BigEndianExplicitTransferSyntax );
  transferSyntaxes.push_back ( UID_LittleEndianImplicitTransferSyntax );
  this->SCU.addPresentationContext ( 
      UID_MOVEStudyRootQueryRetrieveInformationModel, transferSyntaxes );
  this->SCU.addPresentationContext ( 
      UID_GETStudyRootQueryRetrieveInformationModel, transferSyntaxes );

  for (Uint16 i = 0; i < numberOfDcmLongSCUStorageSOPClassUIDs; i++)
    {
    this->SCU.addPresentationContext(dcmLongSCUStorageSOPClassUIDs[i], 
        transferSyntaxes, ASC_SC_ROLE_SCP);
    }
}

//------------------------------------------------------------------------------
ctkDICOMRetrievePrivate::~ctkDICOMRetrievePrivate()
{
  // At least now be kind to the server and release association
  if (this->SCU.isConnected())
    {
    this->SCU.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
    }
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrievePrivate::initializeSCU( const QString& studyInstanceUID,
                                         const QString& seriesInstanceUID,
                                         const RetrieveType retrieveType,
                                         DcmDataset *retrieveParameters)
{

  // If we like to query another server than before, be sure to disconnect first
  if (this->SCU.isConnected() && this->ConnectionParamsChanged)
  {
    this->SCU.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
  }
  // Connect to server if not already connected
  if (!this->SCU.isConnected())
    {
    // Check and initialize networking parameters in DCMTK
    if ( !this->SCU.initNetwork().good() )
      {
      logger.error ( "Error initializing the network" );
      return false;
      }
    // Negotiate (i.e. start the) association
    logger.debug ( "Negotiating Association" );

    if ( !this->SCU.negotiateAssociation().good() )
      {
      logger.error ( "Error negotiating association" );
      return false;;
      }
    }

  this->ConnectionParamsChanged = false;
  // Setup query about what to be received from the PACS
  logger.debug ( "Setting Retrieve Parameters" );
  if ( retrieveType == RetrieveSeries )
    {
    retrieveParameters->putAndInsertString ( DCM_QueryRetrieveLevel, "SERIES" );
    retrieveParameters->putAndInsertString ( DCM_SeriesInstanceUID, 
                                                seriesInstanceUID.toStdString().c_str() );
    // Always required to send all highler level unique keys, so add study here (we are in Study Root)
    retrieveParameters->putAndInsertString ( DCM_StudyInstanceUID, 
                                                studyInstanceUID.toStdString().c_str() );  //TODO
    }
  else
    {
    retrieveParameters->putAndInsertString ( DCM_QueryRetrieveLevel, "STUDY" );
    retrieveParameters->putAndInsertString ( DCM_StudyInstanceUID, 
                                                studyInstanceUID.toStdString().c_str() );
    }
  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrievePrivate::move ( const QString& studyInstanceUID,
                                         const QString& seriesInstanceUID,
                                         const RetrieveType retrieveType )
{

  DcmDataset *retrieveParameters = new DcmDataset();
  if (! this->initializeSCU(studyInstanceUID, seriesInstanceUID, retrieveType, retrieveParameters) )
    {
    delete retrieveParameters;
    return false;
    }


  // Issue request
  logger.debug ( "Sending Move Request" );
  OFList<RetrieveResponse*> responses;
  T_ASC_PresentationContextID presID = this->SCU.findPresentationContextID(
                                          UID_MOVEStudyRootQueryRetrieveInformationModel, 
                                          "" /* don't care about transfer syntax */ );
  if (presID == 0)
    {
    logger.error ( "MOVE Request failed: No valid Study Root MOVE Presentation Context available" );
    if (!this->KeepAssociationOpen)
      {
      this->SCU.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
      }
    delete retrieveParameters;
    return false;
    }

  // do the actual move request
  OFCondition status = this->SCU.sendMOVERequest ( 
                          presID, this->MoveDestinationAETitle.toStdString().c_str(), 
                          retrieveParameters, &responses );

  // Close association if we do not want to explicitly keep it open
  if (!this->KeepAssociationOpen)
    {
    this->SCU.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
    }
  // Free some (little) memory
  delete retrieveParameters;

  // If we do not receive a single response, something is fishy
  if ( responses.begin() == responses.end() )
    {
    logger.error ( "No responses received at all! (at least one empty response always expected)" );
    //throw std::runtime_error( std::string("No responses received from server!") );
    return false;
    }

  /* The server is permitted to acknowledge every image that was received, or
   * to send a single move response.
   * If there is only a single response, this can mean the following:
   * 1) No images to transfer (Status Success and Number of Completed Subops = 0)
   * 2) All images transferred (Status Success and Number of Completed Subops > 0)
   * 3) Error code, i.e. no images transferred
   * 4) Warning (one or more failures, i.e. some images transferred)
   */
  if ( responses.size() == 1 )
    {
    RetrieveResponse* rsp = *responses.begin();
    logger.debug ( "MOVE response receveid with status: " + 
                      QString(DU_cmoveStatusString(rsp->m_status)) );

    if ( (rsp->m_status == STATUS_Success) 
            || (rsp->m_status == STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures))
      {
      if (rsp->m_numberOfCompletedSubops == 0)
        {
        logger.error ( "No images transferred by PACS!" );
        //throw std::runtime_error( std::string("No images transferred by PACS!") );
        return false;
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
      //throw std::runtime_error( statusDetail.toStdString() );
      return false;
      }
    }
    // Select the last MOVE response to output meaningful status information
    OFListIterator(RetrieveResponse*) it = responses.begin();
  size_t numResults = responses.size();
  for (size_t i = 1; i < numResults; i++)
    {
    it++;
    }
  logger.debug ( "MOVE responses report for study: " + studyInstanceUID +"\n"
    + QString::number(static_cast<unsigned int>((*it)->m_numberOfCompletedSubops))
        + " images transferred, and\n"
    + QString::number(static_cast<unsigned int>((*it)->m_numberOfWarningSubops))
        + " images transferred with warning, and\n"
    + QString::number(static_cast<unsigned int>((*it)->m_numberOfFailedSubops))
        + " images transfers failed");

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrievePrivate::get ( const QString& studyInstanceUID,
                                         const QString& seriesInstanceUID,
                                         const RetrieveType retrieveType )
{
  Q_Q(ctkDICOMRetrieve);

  DcmDataset *retrieveParameters = new DcmDataset();
  if (! this->initializeSCU(studyInstanceUID, seriesInstanceUID, retrieveType, retrieveParameters) )
    {
    delete retrieveParameters;
    return false;
    }

  // Issue request
  logger.debug ( "Sending Get Request" );
  emit q->progress("Sending Get Request");
  emit q->progress(0);
  OFList<RetrieveResponse*> responses;
  T_ASC_PresentationContextID presID = this->SCU.findPresentationContextID(
                                          UID_GETStudyRootQueryRetrieveInformationModel, 
                                          "" /* don't care about transfer syntax */ );
  if (presID == 0)
    {
    logger.error ( "GET Request failed: No valid Study Root GET Presentation Context available" );
    if (!this->KeepAssociationOpen)
      {
      this->SCU.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
      }
    delete retrieveParameters;
    return false;
    }

  emit q->progress("Found Presentation Context");
  emit q->progress(1);

  // do the actual move request
  OFCondition status = this->SCU.sendCGETRequest ( 
                          presID, retrieveParameters, &responses );

  emit q->progress("Sent Get Request");
  emit q->progress(2);

  // Close association if we do not want to explicitly keep it open
  if (!this->KeepAssociationOpen)
    {
    this->SCU.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
    }
  // Free some (little) memory
  delete retrieveParameters;

  // If we do not receive a single response, something is fishy
  if ( responses.begin() == responses.end() )
    {
    logger.error ( "No responses received at all! (at least one empty response always expected)" );
    //throw std::runtime_error( std::string("No responses received from server!") );
    emit q->progress("No Responses from Server!");
    return false;
    }

  emit q->progress("Got Responses");
  emit q->progress(3);

  /* The server is permitted to acknowledge every image that was received, or
   * to send a single move response.
   * If there is only a single response, this can mean the following:
   * 1) No images to transfer (Status Success and Number of Completed Subops = 0)
   * 2) All images transferred (Status Success and Number of Completed Subops > 0)
   * 3) Error code, i.e. no images transferred
   * 4) Warning (one or more failures, i.e. some images transferred)
   */
  if ( responses.size() == 1 )
    {
    RetrieveResponse* rsp = *responses.begin();
    logger.debug ( "GET response receveid with status: " + 
                      QString(DU_cmoveStatusString(rsp->m_status)) );

    if ( (rsp->m_status == STATUS_Success) 
            || (rsp->m_status == STATUS_GET_Warning_SubOperationsCompleteOneOrMoreFailures))
      {
      if (rsp->m_numberOfCompletedSubops == 0)
        {
        logger.error ( "No images transferred by PACS!" );
        //throw std::runtime_error( std::string("No images transferred by PACS!") );
        return false;
        }
      }
    else
      {
      logger.error("GET request failed, server does report error");
      QString statusDetail("No details");
      if (rsp->m_statusDetail != NULL)
        {
         std::ostringstream out;
        rsp->m_statusDetail->print(out);
        statusDetail = "Status Detail: " + statusDetail.fromStdString(out.str());
        }
      statusDetail.prepend("GET request failed: ");
      logger.error(statusDetail);
      //throw std::runtime_error( statusDetail.toStdString() );
      return false;
      }
    }
  // Select the last GET response to output meaningful status information
  OFListIterator(RetrieveResponse*) it = responses.begin();
  size_t numResults = responses.size();
  for (size_t i = 1; i < numResults; i++)
    {
    it++;
    }
  logger.debug ( "GET responses report for study: " + studyInstanceUID +"\n"
    + QString::number(static_cast<unsigned int>((*it)->m_numberOfCompletedSubops))
        + " images transferred, and\n"
    + QString::number(static_cast<unsigned int>((*it)->m_numberOfWarningSubops))
        + " images transferred with warning, and\n"
    + QString::number(static_cast<unsigned int>((*it)->m_numberOfFailedSubops))
        + " images transfers failed");

  emit q->progress("Finished Get");
  emit q->progress(100);

  return true;
}

//------------------------------------------------------------------------------
// ctkDICOMRetrieve methods

//------------------------------------------------------------------------------
ctkDICOMRetrieve::ctkDICOMRetrieve(QObject* parent)
  : QObject(parent),
    d_ptr(new ctkDICOMRetrievePrivate(*this))
{
  Q_D(ctkDICOMRetrieve);
  d->SCU.retrieve = this; // give the dcmtk level access to this for emitting signals
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
  if (strcmp(callingAETitle.toStdString().c_str(), d->SCU.getAETitle().c_str()))
  {
    d->SCU.setAETitle(callingAETitle.toStdString().c_str());
    d->ConnectionParamsChanged = true;
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::callingAETitle() const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU.getAETitle().c_str();
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCalledAETitle( const QString& calledAETitle )
{
  Q_D(ctkDICOMRetrieve);
  if (strcmp(calledAETitle.toStdString().c_str(),d->SCU.getPeerAETitle().c_str()))
  {
    d->SCU.setPeerAETitle(calledAETitle.toStdString().c_str());
    d->ConnectionParamsChanged = true;
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::calledAETitle()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU.getPeerAETitle().c_str();
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setHost( const QString& host )
{
  Q_D(ctkDICOMRetrieve);
  if (strcmp(host.toStdString().c_str(), d->SCU.getPeerHostName().c_str()))
  {
    d->SCU.setPeerHostName(host.toStdString().c_str());
    d->ConnectionParamsChanged = true;
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::host()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU.getPeerHostName().c_str();
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setPort( int port )
{
  Q_D(ctkDICOMRetrieve);
  if (d->SCU.getPeerPort() != port)
  {
    d->SCU.setPeerPort(port);
    d->ConnectionParamsChanged = true;
  }
}

//------------------------------------------------------------------------------
int ctkDICOMRetrieve::port()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU.getPeerPort();
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setMoveDestinationAETitle( const QString& moveDestinationAETitle )
{
  Q_D(ctkDICOMRetrieve);
  if (moveDestinationAETitle != d->MoveDestinationAETitle)
  {
    d->MoveDestinationAETitle = moveDestinationAETitle;
    d->ConnectionParamsChanged = true;
  }
}
//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::moveDestinationAETitle()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->MoveDestinationAETitle;
}

static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setDatabase(ctkDICOMDatabase& dicomDatabase)
{
  Q_D(ctkDICOMRetrieve);
  d->Database = QSharedPointer<ctkDICOMDatabase>(&dicomDatabase, skipDelete);
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase)
{
  Q_D(ctkDICOMRetrieve);
  d->Database = dicomDatabase;
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMRetrieve::database()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->Database;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setKeepAssociationOpen(const bool keepOpen)
{
  Q_D(ctkDICOMRetrieve);
  d->KeepAssociationOpen = keepOpen;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::keepAssociationOpen()
{
  Q_D(const ctkDICOMRetrieve);
  return d->KeepAssociationOpen;
}

void ctkDICOMRetrieve::setWasCanceled(const bool wasCanceled)
{
  Q_D(ctkDICOMRetrieve);
  d->WasCanceled = wasCanceled;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::wasCanceled()
{
  Q_D(const ctkDICOMRetrieve);
  return d->WasCanceled;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::moveStudy(const QString& studyInstanceUID)
{
  if (studyInstanceUID.isEmpty())
    {
    logger.error("Cannot receive series: Study Instance UID empty.");
    return false;
    }
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting moveStudy" );
  return d->move ( studyInstanceUID, "", ctkDICOMRetrievePrivate::RetrieveStudy );
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::getStudy(const QString& studyInstanceUID)
{
  if (studyInstanceUID.isEmpty())
    {
    logger.error("Cannot receive series: Study Instance UID empty.");
    return false;
    }
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting getStudy" );
  return d->get ( studyInstanceUID, "", ctkDICOMRetrievePrivate::RetrieveStudy );
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::moveSeries(const QString& studyInstanceUID,
                                      const QString& seriesInstanceUID)
{
  if (studyInstanceUID.isEmpty() || seriesInstanceUID.isEmpty())
    {
    logger.error("Cannot receive series: Either Study or Series Instance UID empty.");
    return false;
    }
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting moveSeries" );
  return d->move ( studyInstanceUID, seriesInstanceUID, ctkDICOMRetrievePrivate::RetrieveSeries );
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::getSeries(const QString& studyInstanceUID,
                                      const QString& seriesInstanceUID)
{
  if (studyInstanceUID.isEmpty() || seriesInstanceUID.isEmpty())
    {
    logger.error("Cannot receive series: Either Study or Series Instance UID empty.");
    return false;
    }
  Q_D(ctkDICOMRetrieve);
  logger.info ( "Starting getSeries" );
  return d->get ( studyInstanceUID, seriesInstanceUID, ctkDICOMRetrievePrivate::RetrieveSeries );
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::cancel()
{
  Q_D(ctkDICOMRetrieve);
  d->WasCanceled = true;
}

