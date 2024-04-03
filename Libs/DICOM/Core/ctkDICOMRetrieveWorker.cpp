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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// ctkCore includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMRetrieveWorker_p.h"
#include "ctkDICOMRetrieveJob.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"

static ctkLogger logger ("org.commontk.dicom.DICOMRetrieveWorker");

//------------------------------------------------------------------------------
// ctkDICOMRetrieveWorkerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMRetrieveWorkerPrivate::ctkDICOMRetrieveWorkerPrivate(ctkDICOMRetrieveWorker* object)
 : q_ptr(object)
{
  this->Retrieve = QSharedPointer<ctkDICOMRetrieve>(new ctkDICOMRetrieve);
  this->Retrieve->setKeepAssociationOpen(false);
}

//------------------------------------------------------------------------------
ctkDICOMRetrieveWorkerPrivate::~ctkDICOMRetrieveWorkerPrivate()
{
  Q_Q(ctkDICOMRetrieveWorker);

  QSharedPointer<ctkDICOMRetrieveJob> retrieveJob =
    qSharedPointerObjectCast<ctkDICOMRetrieveJob>(q->Job);
  if (!retrieveJob)
  {
    return;
  }

  QObject::disconnect(this->Retrieve.data(), SIGNAL(progressJobDetail(QVariant)),
                      retrieveJob.data(), SIGNAL(progressJobDetail(QVariant)));
}

//------------------------------------------------------------------------------
void ctkDICOMRetrieveWorkerPrivate::setRetrieveParameters()
{
  Q_Q(ctkDICOMRetrieveWorker);

  QSharedPointer<ctkDICOMRetrieveJob> retrieveJob =
    qSharedPointerObjectCast<ctkDICOMRetrieveJob>(q->Job);
  if (!retrieveJob)
  {
    return;
  }

  ctkDICOMServer* server = retrieveJob->server();
  if (!server)
  {
    return;
  }

  this->Retrieve->setConnectionName(server->connectionName());
  this->Retrieve->setCallingAETitle(server->callingAETitle());
  this->Retrieve->setCalledAETitle(server->calledAETitle());
  this->Retrieve->setHost(server->host());
  this->Retrieve->setPort(server->port());
  this->Retrieve->setConnectionTimeout(server->connectionTimeout());
  this->Retrieve->setMoveDestinationAETitle(server->moveDestinationAETitle());
  this->Retrieve->setKeepAssociationOpen(server->keepAssociationOpen());
  this->Retrieve->setJobUID(retrieveJob->jobUID());

  QObject::connect(this->Retrieve.data(), SIGNAL(progressJobDetail(QVariant)),
                   retrieveJob.data(), SIGNAL(progressJobDetail(QVariant)), Qt::DirectConnection);

}

//------------------------------------------------------------------------------
// ctkDICOMRetrieveWorker methods

//------------------------------------------------------------------------------
ctkDICOMRetrieveWorker::ctkDICOMRetrieveWorker()
  : d_ptr(new ctkDICOMRetrieveWorkerPrivate(this))
{
}

//------------------------------------------------------------------------------
ctkDICOMRetrieveWorker::ctkDICOMRetrieveWorker(ctkDICOMRetrieveWorkerPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
ctkDICOMRetrieveWorker::~ctkDICOMRetrieveWorker() = default;

//----------------------------------------------------------------------------
void ctkDICOMRetrieveWorker::requestCancel()
{
  Q_D(const ctkDICOMRetrieveWorker);
  d->Retrieve->cancel();
}

//----------------------------------------------------------------------------
void ctkDICOMRetrieveWorker::run()
{
  Q_D(const ctkDICOMRetrieveWorker);
  QSharedPointer<ctkDICOMRetrieveJob> retrieveJob =
    qSharedPointerObjectCast<ctkDICOMRetrieveJob>(this->Job);
  if (!retrieveJob)
  {
    return;
  }

  QSharedPointer<ctkDICOMScheduler> scheduler =
      qSharedPointerObjectCast<ctkDICOMScheduler>(this->Scheduler);
  ctkDICOMServer* server = retrieveJob->server();
  if (!scheduler
      || !server
      || d->Retrieve->wasCanceled())
  {
    this->onJobCanceled(d->Retrieve->wasCanceled());
    return;
  }

  retrieveJob->setStatus(ctkAbstractJob::JobStatus::Running);

  logger.debug(QString("ctkDICOMRetrieveWorker : running job %1 in thread %2.\n")
                       .arg(retrieveJob->jobUID())
                       .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

  switch (server->retrieveProtocol())
  {
    case ctkDICOMServer::CGET:
      switch(retrieveJob->dicomLevel())
      {
        case ctkDICOMJob::DICOMLevels::None:
          logger.warn("ctkDICOMRetrieveWorker : DICOMLevels was not set.");
          this->Job->setStatus(ctkAbstractJob::JobStatus::Finished);
          return;
        case ctkDICOMJob::DICOMLevels::Patients:
          logger.warn("ctkDICOMRetrieveWorker : get operation for a full patient is not implemented.");
          this->Job->setStatus(ctkAbstractJob::JobStatus::Finished);
          return;
        case ctkDICOMJob::DICOMLevels::Studies:
          if (!d->Retrieve->getStudy(retrieveJob->studyInstanceUID(),
                                     retrieveJob->patientID()))
          {
            this->onJobCanceled(d->Retrieve->wasCanceled());
            return;
          }
          break;
        case ctkDICOMJob::DICOMLevels::Series:
          if (!d->Retrieve->getSeries(retrieveJob->studyInstanceUID(),
                                      retrieveJob->seriesInstanceUID(),
                                      retrieveJob->patientID()))
          {
            this->onJobCanceled(d->Retrieve->wasCanceled());
            return;
          }
          break;
        case ctkDICOMJob::DICOMLevels::Instances:
          if (!d->Retrieve->getSOPInstance(retrieveJob->studyInstanceUID(),
                                           retrieveJob->seriesInstanceUID(),
                                           retrieveJob->sopInstanceUID(),
                                           retrieveJob->patientID()))
          {
            this->onJobCanceled(d->Retrieve->wasCanceled());
            return;
          }
          break;
      }
      break;
    case ctkDICOMServer::CMOVE:
      switch(retrieveJob->dicomLevel())
      {
        case ctkDICOMJob::DICOMLevels::None:
          logger.warn("ctkDICOMRetrieveWorker : DICOMLevels was not set.");
          this->Job->setStatus(ctkAbstractJob::JobStatus::Finished);
          return;
        case ctkDICOMJob::DICOMLevels::Patients:
          logger.warn("ctkDICOMRetrieveTask : move operation for a full patient is not implemented.");
          retrieveJob->setStatus(ctkAbstractJob::JobStatus::Finished);
          return;
        case ctkDICOMJob::DICOMLevels::Studies:
          if (!d->Retrieve->moveStudy(retrieveJob->studyInstanceUID(),
                                      retrieveJob->patientID()))
          {
            this->onJobCanceled(d->Retrieve->wasCanceled());
            return;
          }
          break;
        case ctkDICOMJob::DICOMLevels::Series:
          if (!d->Retrieve->moveSeries(retrieveJob->studyInstanceUID(),
                                       retrieveJob->seriesInstanceUID(),
                                       retrieveJob->patientID()))
          {
            this->onJobCanceled(d->Retrieve->wasCanceled());
            return;
          }
          break;
        case ctkDICOMJob::DICOMLevels::Instances:
          if (!d->Retrieve->moveSOPInstance(retrieveJob->studyInstanceUID(),
                                            retrieveJob->seriesInstanceUID(),
                                            retrieveJob->sopInstanceUID(),
                                            retrieveJob->patientID()))
          {
            this->onJobCanceled(d->Retrieve->wasCanceled());
            return;
          }
          break;
      }
      break;
      //case ctkDICOMServer::WADO: // To Do
  }

  if (d->Retrieve->wasCanceled())
  {
    this->onJobCanceled(d->Retrieve->wasCanceled());
    return;
  }

  ctkDICOMServer* proxyServer = server->proxyServer();
  if (proxyServer && proxyServer->queryRetrieveEnabled())
  {
    ctkDICOMRetrieveJob* newJob = qobject_cast<ctkDICOMRetrieveJob*>(retrieveJob->clone());
    newJob->setRetryCounter(0);
    newJob->setServer(*proxyServer);
    scheduler->addJob(newJob);
  }
  else if (d->Retrieve->jobResponseSetsShared().count() > 0 &&
    server->retrieveProtocol() == ctkDICOMServer::RetrieveProtocol::CGET)
  {
    // To Do: this insert should happen in batch of 10 frames (configurable),
    // instead of at the end of operation (all frames requested)).
    // This would avoid memory usage spikes when requesting a series with a lot of frames.
    // NOTE: the memory should be released as soon as the response is inserted.
    retrieveJob->setReferenceInserterJobUID
      (scheduler->insertJobResponseSets(d->Retrieve->jobResponseSetsShared()));
  }

  retrieveJob->setStatus(ctkAbstractJob::JobStatus::Finished);
}

//----------------------------------------------------------------------------
void ctkDICOMRetrieveWorker::setJob(QSharedPointer<ctkAbstractJob> job)
{
  Q_D(ctkDICOMRetrieveWorker);

  QSharedPointer<ctkDICOMRetrieveJob> retrieveJob =
    qSharedPointerObjectCast<ctkDICOMRetrieveJob>(job);
  if (!retrieveJob)
  {
    return;
  }

  this->Superclass::setJob(job);
  d->setRetrieveParameters();
}

//----------------------------------------------------------------------------
ctkDICOMRetrieve* ctkDICOMRetrieveWorker::retriever() const
{
  Q_D(const ctkDICOMRetrieveWorker);
  return d->Retrieve.data();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMRetrieve> ctkDICOMRetrieveWorker::retrieverShared() const
{
  Q_D(const ctkDICOMRetrieveWorker);
  return d->Retrieve;
}
