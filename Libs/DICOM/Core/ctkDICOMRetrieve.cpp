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
#include <QMutex>

// ctkDICOMCore includes
#include "ctkDICOMRetrieve.h"
#include "ctkErrorLogLevel.h"
#include "ctkLogger.h"
#include "ctkDICOMJobResponseSet.h"

// DCMTK includes
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/oflog/oflog.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */
#include <dcmtk/dcmjpeg/djdecode.h>  /* for dcmjpeg decoders */
#include <dcmtk/dcmjpeg/djencode.h>  /* for dcmjpeg encoders */
#include <dcmtk/dcmdata/dcrledrg.h>  /* for DcmRLEDecoderRegistration */
#include <dcmtk/dcmdata/dcrleerg.h>  /* for DcmRLEEncoderRegistration */

static ctkLogger logger("org.commontk.dicom.DICOMRetrieve");

//------------------------------------------------------------------------------
// A customized local implementation of the DcmSCU so that Qt signals can be emitted
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

  // called when a move response comes in: indicates that the
  // move request is being handled by the remote server.
  virtual OFCondition handleMOVEResponse(const T_ASC_PresentationContextID  presID,
                                         RetrieveResponse *response,
                                         OFBool &waitForNextResponse)
  {
    if (!this->retrieve)
    {
      return EC_IllegalCall;
    }

    if (this->retrieve->wasCanceled())
    {
      // send cancel can fail and be ignored (but DCMTK will report still good == true).
      // Therefore, we need to force the release of the association to cancel the worker
      this->retrieve->releaseAssociation();
      return EC_IllegalCall;
    }

    emit this->retrieve->progress(ctkDICOMRetrieve::tr("Got move request"));
    emit this->retrieve->progress(0);
    return this->DcmSCU::handleMOVEResponse(
      presID, response, waitForNextResponse);
  };

  // called when a data set is coming in from a server in
  // response to a CGET
  virtual OFCondition handleSTORERequest(const T_ASC_PresentationContextID presID,
                                         DcmDataset *incomingObject,
                                         OFBool& continueCGETSession,
                                         Uint16& cStoreReturnStatus)
  {
    if (!this->retrieve)
    {
      return EC_IllegalCall;
    }

    if (this->retrieve->wasCanceled())
    {
      // send cancel can fail and be ignored (but DCMTK will report still good == true).
      // Therefore, we need to force the release of the association to cancel the worker
      this->retrieve->releaseAssociation();
      return EC_IllegalCall;
    }

    OFString instanceUID;
    incomingObject->findAndGetOFString(DCM_SOPInstanceUID, instanceUID);
    QString qInstanceUID(instanceUID.c_str());
    emit this->retrieve->progress(
      //: %1 is an instance UID
      ctkDICOMRetrieve::tr("Got STORE request for %1").arg(qInstanceUID)
    );
    emit this->retrieve->progress(0);
    if (!this->retrieve->jobUID().isEmpty())
    {
      QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet =
        QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
      if (this->retrieve->getLastRetrieveType() == ctkDICOMRetrieve::RetrieveType::RetrieveSOPInstance)
      {
        jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance);
      }
      else if (this->retrieve->getLastRetrieveType() == ctkDICOMRetrieve::RetrieveType::RetrieveSeries)
      {
        jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveSeries);
      }
      else if (this->retrieve->getLastRetrieveType() == ctkDICOMRetrieve::RetrieveType::RetrieveStudy)
      {
        jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveStudy);
      }
      jobResponseSet->setPatientID(this->retrieve->patientID());
      jobResponseSet->setStudyInstanceUID(this->retrieve->studyInstanceUID());
      jobResponseSet->setSeriesInstanceUID(this->retrieve->seriesInstanceUID());
      jobResponseSet->setSOPInstanceUID(qInstanceUID);
      jobResponseSet->setConnectionName(this->retrieve->connectionName());
      jobResponseSet->setDataset(incomingObject);
      jobResponseSet->setJobUID(this->retrieve->jobUID());
      jobResponseSet->setCopyFile(true);

      // To Do: this should be emitted for all the RetrieveTypes, but we should change the insert in the
      // ctkDICOMRetrieveWorker to happen every 10 frames (configurable).
      // i.e. a slot in ctkDICOMRetrieveWorker with a counter. When the counter > batchLimit -> insert
      if (this->retrieve->getLastRetrieveType() == ctkDICOMRetrieve::RetrieveType::RetrieveSeries)
      {
        emit this->retrieve->progressJobDetail(jobResponseSet->toVariant());
      }

      this->retrieve->addJobResponseSet(jobResponseSet);
      return EC_Normal;
    }
    else if (this->retrieve->dicomDatabase())
    {
      this->retrieve->dicomDatabase()->insert(incomingObject, true, false);
      return EC_Normal;
    }
    else
    {
      return this->DcmSCU::handleSTORERequest(
        presID, incomingObject, continueCGETSession, cStoreReturnStatus);
    }
  };

  // called when status information from remote server
  // comes in from CGET
  virtual OFCondition handleCGETResponse(const T_ASC_PresentationContextID presID,
                                         RetrieveResponse* response,
                                         OFBool& continueCGETSession)
  {
    if (!this->retrieve)
    {
      return EC_IllegalCall;
    }

    if (this->retrieve->wasCanceled())
    {
      // send cancel can fail and be ignored (but DCMTK will report still good == true).
      // Therefore, we need to force the release of the association to cancel the worker
      this->retrieve->releaseAssociation();
      return EC_IllegalCall;
    }

    emit this->retrieve->progress(ctkDICOMRetrieve::tr("Got CGET response"));
    emit this->retrieve->progress(0);
    return this->DcmSCU::handleCGETResponse(presID, response, continueCGETSession);
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

  /// \warning: releaseAssociation is not a thread safe method.
  /// If called concurrently from different threads DCMTK can crash.
  /// Therefore use this method instead of calling directly SCU->releaseAssociation()
  OFCondition releaseAssociation();

  bool Canceled;
  bool KeepAssociationOpen;
  bool ConnectionParamsChanged;
  bool AssociationClosing;
  QMutex AssociationMutex;
  ctkDICOMRetrieve::RetrieveType LastRetrieveType;

  QString PatientID;
  QString StudyInstanceUID;
  QString SeriesInstanceUID;
  QString SOPInstanceUID;
  QString ConnectionName;
  QString JobUID;

  QSharedPointer<ctkDICOMDatabase> Database;
  ctkDICOMRetrieveSCUPrivate *SCU;
  T_ASC_PresentationContextID PresentationContext;
  QString MoveDestinationAETitle;
  QList<QSharedPointer<ctkDICOMJobResponseSet>> JobResponseSets;

  bool initializeSCU(const QString& patientID,
                     const QString& studyInstanceUID,
                     const QString& seriesInstanceUID,
                     const QString& sopInstanceUID,
                     const ctkDICOMRetrieve::RetrieveType retrieveType,
                     DcmDataset *retrieveParameters);
  bool move(const QString& patientID,
            const QString& studyInstanceUID,
            const QString& seriesInstanceUID,
            const QString& sopInstanceUID,
            const ctkDICOMRetrieve::RetrieveType retrieveType);
  bool get(const QString& patientID,
           const QString& studyInstanceUID,
           const QString& seriesInstanceUID,
           const QString& sopInstanceUID,
           const ctkDICOMRetrieve::RetrieveType retrieveType);
};

//------------------------------------------------------------------------------
// ctkDICOMRetrievePrivate methods

//------------------------------------------------------------------------------
ctkDICOMRetrievePrivate::ctkDICOMRetrievePrivate(ctkDICOMRetrieve& obj)
  : q_ptr(&obj)
{
  this->Database = QSharedPointer<ctkDICOMDatabase> (0);
  this->Canceled = false;
  this->KeepAssociationOpen = true;
  this->ConnectionParamsChanged = false;
  this->AssociationClosing = false;
  this->LastRetrieveType = ctkDICOMRetrieve::RetrieveNone;

  this->PatientID = "";
  this->StudyInstanceUID = "";
  this->SeriesInstanceUID = "";
  this->ConnectionName = "";
  this->JobUID = "";

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

  logger.debug("Setting Transfer Syntaxes");
  OFList<OFString> transferSyntaxes;
  transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
  transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
  transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);

  this->PresentationContext = 0;
  this->SCU = new ctkDICOMRetrieveSCUPrivate();
  this->SCU->addPresentationContext(
    UID_MOVEStudyRootQueryRetrieveInformationModel, transferSyntaxes);
  this->SCU->addPresentationContext(
    UID_GETStudyRootQueryRetrieveInformationModel, transferSyntaxes);

  for (Uint16 index = 0; index < numberOfDcmLongSCUStorageSOPClassUIDs; index++)
  {
    this->SCU->addPresentationContext(dcmLongSCUStorageSOPClassUIDs[index],
      transferSyntaxes, ASC_SC_ROLE_SCP);
  }

  this->SCU->setACSETimeout(3);
  this->SCU->setConnectionTimeout(3);
  this->SCU->setStorageDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation).toStdString().c_str());
}

//------------------------------------------------------------------------------
ctkDICOMRetrievePrivate::~ctkDICOMRetrievePrivate()
{
  if (this->SCU && this->SCU->isConnected())
  {
    this->releaseAssociation();
  }

  if (this->SCU)
  {
    delete this->SCU;
  }

  this->JobResponseSets.clear();
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMRetrievePrivate::releaseAssociation()
{
  OFCondition status = EC_IllegalCall;
  if (!this->SCU)
    {
    return status;
    }

  QMutexLocker locker(&this->AssociationMutex);
  if (this->AssociationClosing)
  {
    return status;
  }

  this->AssociationClosing = true;
  status = this->SCU->releaseAssociation();
  this->AssociationClosing = false;

  return status;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrievePrivate::initializeSCU(const QString& patientID,
                                            const QString& studyInstanceUID,
                                            const QString& seriesInstanceUID,
                                            const QString& sopInstanceUID,
                                            const ctkDICOMRetrieve::RetrieveType retrieveType,
                                            DcmDataset *retrieveParameters)
{
  // If we like to query another server than before, be sure to disconnect first
  if (this->SCU->isConnected() && this->ConnectionParamsChanged)
  {
    this->releaseAssociation();
  }
  // Connect to server if not already connected
  if (!this->SCU->isConnected())
  {
    // Check and initialize networking parameters in DCMTK
    if ( !this->SCU->initNetwork().good() )
    {
      logger.error ( "Error initializing the network" );
      return false;
    }
    // Negotiate (i.e. start the) association
    logger.debug ( "Negotiating Association" );

    if ( !this->SCU->negotiateAssociation().good() )
    {
      logger.error ( "Error negotiating association" );
      return false;;
    }
  }

  this->ConnectionParamsChanged = false;
  // Setup query about what to be received from the PACS
  logger.debug ( "Setting Retrieve Parameters" );
  if (retrieveType == ctkDICOMRetrieve::RetrieveSOPInstance)
  {
    retrieveParameters->putAndInsertString(DCM_QueryRetrieveLevel, "IMAGE");
    retrieveParameters->putAndInsertString(DCM_SOPInstanceUID,
                                           sopInstanceUID.toStdString().c_str());
    retrieveParameters->putAndInsertString(DCM_SeriesInstanceUID,
                                           seriesInstanceUID.toStdString().c_str());
    // Always required to send all highler level unique keys, so add study here (we are in Study Root)
    retrieveParameters->putAndInsertString(DCM_StudyInstanceUID,
                                           studyInstanceUID.toStdString().c_str());
    if (!patientID.isEmpty())
    {
      retrieveParameters->putAndInsertString(DCM_PatientID,
                                             patientID.toStdString().c_str());
    }
  }
  else if (retrieveType == ctkDICOMRetrieve::RetrieveSeries)
  {
    retrieveParameters->putAndInsertString(DCM_QueryRetrieveLevel, "SERIES");
    retrieveParameters->putAndInsertString(DCM_SeriesInstanceUID,
                                           seriesInstanceUID.toStdString().c_str());
    // Always required to send all highler level unique keys, so add study here (we are in Study Root)
    retrieveParameters->putAndInsertString(DCM_StudyInstanceUID,
                                           studyInstanceUID.toStdString().c_str());
    if (!patientID.isEmpty())
    {
      retrieveParameters->putAndInsertString(DCM_PatientID,
                                             patientID.toStdString().c_str());
    }
  }
  else
  {
    retrieveParameters->putAndInsertString ( DCM_QueryRetrieveLevel, "STUDY" );
    retrieveParameters->putAndInsertString ( DCM_StudyInstanceUID,
                                                studyInstanceUID.toStdString().c_str() );
    if (!patientID.isEmpty())
    {
      retrieveParameters->putAndInsertString(DCM_PatientID,
                                             patientID.toStdString().c_str());
    }
  }

  this->LastRetrieveType = retrieveType;
  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrievePrivate::move(const QString& patientID,
                                   const QString& studyInstanceUID,
                                   const QString& seriesInstanceUID,
                                   const QString& sopInstanceUID,
                                   const ctkDICOMRetrieve::RetrieveType retrieveType)
{
  Q_Q(ctkDICOMRetrieve);

  this->JobResponseSets.clear();
  this->PatientID = patientID;
  this->StudyInstanceUID = studyInstanceUID;
  this->SeriesInstanceUID = seriesInstanceUID;
  this->SOPInstanceUID = sopInstanceUID;

  if (this->Canceled)
  {
    return false;
  }

  DcmDataset *retrieveParameters = new DcmDataset();
  if (!this->initializeSCU(patientID,
                           studyInstanceUID,
                           seriesInstanceUID,
                           sopInstanceUID,
                           retrieveType,
                           retrieveParameters))
  {
    delete retrieveParameters;
    logger.error("MOVE Request failed: SCU initialization failed");
    return false;
  }

  // Issue request
  logger.debug ( "Sending Move Request" );
  OFList<RetrieveResponse*> responses;
  this->PresentationContext = this->SCU->findPresentationContextID(
    UID_MOVEStudyRootQueryRetrieveInformationModel,
    "" /* don't care about transfer syntax */);
  if (this->PresentationContext == 0)
  {
    logger.error ( "MOVE Request failed: No valid Study Root MOVE Presentation Context available" );
    if (!this->KeepAssociationOpen)
    {
      this->releaseAssociation();
    }
    delete retrieveParameters;
    return false;
  }

  if (this->Canceled)
  {
    return false;
  }

  // do the actual move request
  OFCondition status = this->SCU->sendMOVERequest(
    this->PresentationContext, this->MoveDestinationAETitle.toStdString().c_str(),
    retrieveParameters, &responses);

  // Close association if we do not want to explicitly keep it open
  if (!this->KeepAssociationOpen)
  {
    this->releaseAssociation();
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

  if (this->Canceled)
  {
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
        return false;
      }
    }
    else
    {
      logger.debug("MOVE request failed, server does report error");
      QString statusDetail("No details");
      if (rsp->m_statusDetail != NULL)
      {
         std::ostringstream out;
        rsp->m_statusDetail->print(out);
        statusDetail = "Status Detail: " + statusDetail.fromStdString(out.str());
      }
      statusDetail.prepend("MOVE request failed: ");
      logger.debug(statusDetail);
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
  logger.debug (
    QString("MOVE responses report for study: %1\n"
      "%2 images transferred, and\n"
      "%3 images transferred with warning, and\n"
      "%4 images transfers failed")
    .arg(studyInstanceUID)
    .arg(QString::number(static_cast<unsigned int>((*it)->m_numberOfCompletedSubops)))
    .arg(QString::number(static_cast<unsigned int>((*it)->m_numberOfWarningSubops)))
    .arg(QString::number(static_cast<unsigned int>((*it)->m_numberOfFailedSubops)))
  );

  if (this->Canceled)
  {
    return false;
  }

  // if move was successful, add a taskResults to report it
  QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet =
    QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
  if (q->getLastRetrieveType() == ctkDICOMRetrieve::RetrieveType::RetrieveSOPInstance)
  {
    jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance);
  }
  else if (q->getLastRetrieveType() == ctkDICOMRetrieve::RetrieveType::RetrieveSeries)
  {
    jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveSeries);
  }
  else if (q->getLastRetrieveType() == ctkDICOMRetrieve::RetrieveType::RetrieveStudy)
  {
    jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::RetrieveStudy);
  }
  jobResponseSet->setStudyInstanceUID(q->studyInstanceUID());
  jobResponseSet->setSeriesInstanceUID(q->seriesInstanceUID());
  jobResponseSet->setConnectionName(q->connectionName());
  jobResponseSet->setJobUID(q->jobUID());
  q->addJobResponseSet(jobResponseSet);

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrievePrivate::get(const QString& patientID,
                                  const QString& studyInstanceUID,
                                  const QString& seriesInstanceUID,
                                  const QString& sopInstanceUID,
                                  const ctkDICOMRetrieve::RetrieveType retrieveType)
{
  Q_Q(ctkDICOMRetrieve);

  this->JobResponseSets.clear();
  this->PatientID = patientID;
  this->StudyInstanceUID = studyInstanceUID;
  this->SeriesInstanceUID = seriesInstanceUID;
  this->SOPInstanceUID = sopInstanceUID;

  if (this->Canceled)
  {
    return false;
  }

  DcmDataset *retrieveParameters = new DcmDataset();
  if (!this->initializeSCU(patientID,
                           studyInstanceUID,
                           seriesInstanceUID,
                           sopInstanceUID,
                           retrieveType,
                           retrieveParameters))
  {
    delete retrieveParameters;
    logger.error("MOVE Request failed: SCU initialization failed");
    return false;
  }

  if (this->Canceled)
  {
    return false;
  }

  // Issue request
  logger.debug ( "Sending Get Request" );
  emit q->progress(ctkDICOMRetrieve::tr("Sending Get Request"));
  emit q->progress(0);
  OFList<RetrieveResponse*> responses;
  this->PresentationContext = this->SCU->findPresentationContextID(
    UID_GETStudyRootQueryRetrieveInformationModel,
    "" /* don't care about transfer syntax */ );
  if (this->PresentationContext == 0)
  {
    logger.error ( "GET Request failed: No valid Study Root GET Presentation Context available" );
    if (!this->KeepAssociationOpen)
    {
      this->releaseAssociation();
    }
    delete retrieveParameters;
    return false;
  }

  if (this->Canceled)
  {
    return false;
  }

  emit q->progress(ctkDICOMRetrieve::tr("Found Presentation Context"));
  emit q->progress(1);

  // do the actual move request
  OFCondition status = this->SCU->sendCGETRequest(this->PresentationContext, retrieveParameters, &responses);

  emit q->progress(ctkDICOMRetrieve::tr("Sent Get Request"));
  emit q->progress(2);

  // Close association if we do not want to explicitly keep it open
  if (!this->KeepAssociationOpen)
  {
    this->releaseAssociation();
  }
  // Free some (little) memory
  delete retrieveParameters;

  // If we do not receive a single response, something is fishy
  if ( responses.begin() == responses.end() )
  {
    logger.error ( "No responses received at all! (at least one empty response always expected)" );
    //throw std::runtime_error( std::string("No responses received from server!") );
    emit q->progress(ctkDICOMRetrieve::tr("No Responses from Server!"));
    return false;
  }

  if (this->Canceled)
  {
    return false;
  }

  emit q->progress(ctkDICOMRetrieve::tr("Got Responses"));
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
        return false;
      }
    }
    else
    {
      logger.debug("GET request failed, server does report error");
      QString statusDetail("No details");
      if (rsp->m_statusDetail != NULL)
      {
         std::ostringstream out;
        rsp->m_statusDetail->print(out);
        statusDetail = "Status Detail: " + statusDetail.fromStdString(out.str());
      }
      statusDetail.prepend("GET request failed: ");
      logger.debug(statusDetail);
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

  logger.debug (
    QString("GET responses report for study: %1\n"
        "%2 images transferred, and\n"
        "%3 images transferred with warning, and\n"
        "%4 images transfers failed")
    .arg(studyInstanceUID)
    .arg(QString::number(static_cast<unsigned int>((*it)->m_numberOfCompletedSubops)))
    .arg(QString::number(static_cast<unsigned int>((*it)->m_numberOfWarningSubops)))
    .arg(QString::number(static_cast<unsigned int>((*it)->m_numberOfFailedSubops)))
  );

  emit q->progress(ctkDICOMRetrieve::tr("Finished Get"));
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

  d->SCU->setVerbosePCMode(false);
  d->SCU->retrieve = this; // give the dcmtk level access to this for emitting signals
}

//------------------------------------------------------------------------------
ctkDICOMRetrieve::~ctkDICOMRetrieve()
{
}

//------------------------------------------------------------------------------
CTK_SET_CPP(ctkDICOMRetrieve, const QString&, setConnectionName, ConnectionName);
CTK_GET_CPP(ctkDICOMRetrieve, QString, connectionName, ConnectionName)
CTK_GET_CPP(ctkDICOMRetrieve, QString, moveDestinationAETitle, MoveDestinationAETitle)
CTK_SET_CPP(ctkDICOMRetrieve, const QString&, setJobUID, JobUID);
CTK_GET_CPP(ctkDICOMRetrieve, QString, jobUID, JobUID)
CTK_GET_CPP(ctkDICOMRetrieve, QString, patientID, PatientID)
CTK_GET_CPP(ctkDICOMRetrieve, QString, studyInstanceUID, StudyInstanceUID)
CTK_GET_CPP(ctkDICOMRetrieve, QString, seriesInstanceUID, SeriesInstanceUID)
CTK_GET_CPP(ctkDICOMRetrieve, QString, sopInstanceUID, SOPInstanceUID)
CTK_SET_CPP(ctkDICOMRetrieve, const bool, setKeepAssociationOpen, KeepAssociationOpen);
CTK_GET_CPP(ctkDICOMRetrieve, bool, keepAssociationOpen, KeepAssociationOpen)

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCallingAETitle(const QString& callingAETitle)
{
  Q_D(ctkDICOMRetrieve);
  if (strcmp(callingAETitle.toStdString().c_str(), d->SCU->getAETitle().c_str()))
  {
    d->SCU->setAETitle(callingAETitle.toStdString().c_str());
    d->ConnectionParamsChanged = true;
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::callingAETitle() const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU->getAETitle().c_str();
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setCalledAETitle( const QString& calledAETitle )
{
  Q_D(ctkDICOMRetrieve);
  if (strcmp(calledAETitle.toStdString().c_str(),d->SCU->getPeerAETitle().c_str()))
  {
    d->SCU->setPeerAETitle(calledAETitle.toStdString().c_str());
    d->ConnectionParamsChanged = true;
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::calledAETitle()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU->getPeerAETitle().c_str();
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setHost( const QString& host )
{
  Q_D(ctkDICOMRetrieve);
  if (strcmp(host.toStdString().c_str(), d->SCU->getPeerHostName().c_str()))
  {
    d->SCU->setPeerHostName(host.toStdString().c_str());
    d->ConnectionParamsChanged = true;
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMRetrieve::host()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU->getPeerHostName().c_str();
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::setPort( int port )
{
  Q_D(ctkDICOMRetrieve);
  if (d->SCU->getPeerPort() != port)
  {
    d->SCU->setPeerPort(port);
    d->ConnectionParamsChanged = true;
  }
}

//------------------------------------------------------------------------------
int ctkDICOMRetrieve::port()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU->getPeerPort();
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
ctkDICOMDatabase* ctkDICOMRetrieve::dicomDatabase()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->Database.data();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMRetrieve::dicomDatabaseShared()const
{
  Q_D(const ctkDICOMRetrieve);
  return d->Database;
}

//------------------------------------------------------------------------------
QList<ctkDICOMJobResponseSet *> ctkDICOMRetrieve::jobResponseSets() const
{
  Q_D(const ctkDICOMRetrieve);
  QList<ctkDICOMJobResponseSet *> jobResponseSets;
  foreach(QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet, d->JobResponseSets)
  {
    jobResponseSets.append(jobResponseSet.data());
  }

  return jobResponseSets;
}

//------------------------------------------------------------------------------
QList<QSharedPointer<ctkDICOMJobResponseSet>> ctkDICOMRetrieve::jobResponseSetsShared() const
{
  Q_D(const ctkDICOMRetrieve);
  return d->JobResponseSets;
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::addJobResponseSet(ctkDICOMJobResponseSet &jobResponseSet)
{
  this->addJobResponseSet(QSharedPointer<ctkDICOMJobResponseSet>(&jobResponseSet, skipDelete));
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::addJobResponseSet(QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet)
{
  Q_D(ctkDICOMRetrieve);
  d->JobResponseSets.append(jobResponseSet);
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::removeJobResponseSet(QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet)
{
  Q_D(ctkDICOMRetrieve);
  d->JobResponseSets.removeOne(jobResponseSet);
}

//------------------------------------------------------------------------------
ctkDICOMRetrieve::RetrieveType ctkDICOMRetrieve::getLastRetrieveType() const
{
  Q_D(const ctkDICOMRetrieve);
  return d->LastRetrieveType;
}

//-----------------------------------------------------------------------------
void ctkDICOMRetrieve::setConnectionTimeout(int timeout)
{
  Q_D(ctkDICOMRetrieve);
  d->SCU->setACSETimeout(timeout);
  d->SCU->setConnectionTimeout(timeout);
}

//-----------------------------------------------------------------------------
int ctkDICOMRetrieve::connectionTimeout() const
{
  Q_D(const ctkDICOMRetrieve);
  return d->SCU->getConnectionTimeout();
}

//-----------------------------------------------------------------------------
bool ctkDICOMRetrieve::wasCanceled()
{
  Q_D(const ctkDICOMRetrieve);
  return d->Canceled;
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::moveStudy(const QString& studyInstanceUID,
                                 const QString& patientID)
{
  if (studyInstanceUID.isEmpty())
  {
    logger.error("Cannot receive series: Study Instance UID empty.");
    return false;
  }
  Q_D(ctkDICOMRetrieve);
  logger.debug("Starting moveStudy");
  return d->move(patientID, studyInstanceUID, "", "", ctkDICOMRetrieve::RetrieveStudy);
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::getStudy(const QString& studyInstanceUID,
                                const QString& patientID)
{
  if (studyInstanceUID.isEmpty())
  {
    logger.error("Cannot receive study: Study Instance UID empty.");
    return false;
  }
  Q_D(ctkDICOMRetrieve);
  logger.debug("Starting getStudy");
  return d->get(patientID, studyInstanceUID, "", "", ctkDICOMRetrieve::RetrieveStudy);
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::moveSeries(const QString& studyInstanceUID,
                                  const QString& seriesInstanceUID,
                                  const QString& patientID)
{
  if (studyInstanceUID.isEmpty() ||
      seriesInstanceUID.isEmpty())
  {
    logger.error("Cannot receive series: Study or Series Instance UID empty.");
    return false;
  }
  Q_D(ctkDICOMRetrieve);
  logger.debug("Starting moveSeries");
  return d->move(patientID, studyInstanceUID, seriesInstanceUID, "", ctkDICOMRetrieve::RetrieveSeries);
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::getSeries(const QString& studyInstanceUID,
                                 const QString& seriesInstanceUID,
                                 const QString& patientID)
{
  if (studyInstanceUID.isEmpty() ||
      seriesInstanceUID.isEmpty())
  {
    logger.error("Cannot receive series: Study or Series Instance UID empty.");
    return false;
  }
  Q_D(ctkDICOMRetrieve);
  logger.debug("Starting getSeries");
  return d->get(patientID, studyInstanceUID, seriesInstanceUID, "", ctkDICOMRetrieve::RetrieveSeries);
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::moveSOPInstance(const QString& studyInstanceUID,
                                       const QString& seriesInstanceUID,
                                       const QString& SOPInstanceUID,
                                       const QString& patientID)
{
  Q_D(ctkDICOMRetrieve);

  if (studyInstanceUID.isEmpty() ||
      seriesInstanceUID.isEmpty() ||
      SOPInstanceUID.isEmpty())
  {
    logger.error("Cannot receive SOPInstance: Study, Series or SOP Instance UID empty.");
    return false;
  }

  logger.debug("Starting moveSOPInstance");
  return d->move(patientID, studyInstanceUID, seriesInstanceUID, SOPInstanceUID, ctkDICOMRetrieve::RetrieveSOPInstance);
}

//------------------------------------------------------------------------------
bool ctkDICOMRetrieve::getSOPInstance(const QString& studyInstanceUID,
                                      const QString& seriesInstanceUID,
                                      const QString& SOPInstanceUID,
                                      const QString& patientID)
{
  Q_D(ctkDICOMRetrieve);

  if (studyInstanceUID.isEmpty() ||
      seriesInstanceUID.isEmpty() ||
      SOPInstanceUID.isEmpty())
  {
    logger.error("Cannot receive SOPInstance: Study, Series or SOP Instance UID empty.");
    return false;
  }

  logger.debug("Starting getSOPInstance");
  return d->get(patientID, studyInstanceUID, seriesInstanceUID, SOPInstanceUID, ctkDICOMRetrieve::RetrieveSOPInstance);
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::cancel()
{
  Q_D(ctkDICOMRetrieve);
  d->Canceled = true;

  if (d->PresentationContext != 0)
  {
    d->SCU->sendCANCELRequest(d->PresentationContext);
    d->PresentationContext = 0;
  }
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieve::releaseAssociation()
{
  Q_D(ctkDICOMRetrieve);
  d->releaseAssociation();
}
