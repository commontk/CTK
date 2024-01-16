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

// ctkDICOMCore includes
#include "ctkDICOMInserterJob.h"
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMQueryJob.h"
#include "ctkDICOMRetrieveJob.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMScheduler_p.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMStorageListenerJob.h"
#include "ctkDICOMUtil.h"
#include "ctkDICOMWorker.h"
#include "ctkLogger.h"

// dcmtk includes
#include <dcmtk/dcmdata/dcdeftag.h>


static ctkLogger logger ( "org.commontk.dicom.DICOMJobPool" );

//------------------------------------------------------------------------------
// ctkDICOMSchedulerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMSchedulerPrivate::ctkDICOMSchedulerPrivate(ctkDICOMScheduler& obj)
  : q_ptr(&obj)
{
  ctk::setDICOMLogLevel(ctkErrorLogLevel::Warning);

  this->DicomDatabase = nullptr;
  this->ThreadPool = QSharedPointer<QThreadPool> (new QThreadPool());
  this->ThreadPool->setMaxThreadCount(20);
  this->RetryDelay = 100;
  this->MaximumNumberOfRetry = 3;
  this->MaximumPatientsQuery = 25;
}

//------------------------------------------------------------------------------
ctkDICOMSchedulerPrivate::~ctkDICOMSchedulerPrivate()
{
  Q_Q(ctkDICOMScheduler);
  q->removeAllServers();
}

//------------------------------------------------------------------------------
int ctkDICOMSchedulerPrivate::getSameTypeJobsInThreadPoolQueueOrRunning(QSharedPointer<ctkDICOMJob> job)
{
  int count = 0;
  foreach (QSharedPointer<ctkDICOMJob> queuedJob, this->JobsQueue)
    {
    if (queuedJob->jobUID() == job->jobUID())
      {
      continue;
      }

    if ((queuedJob->status() == ctkAbstractJob::JobStatus::Queued ||
         queuedJob->status() == ctkAbstractJob::JobStatus::Running) &&
        queuedJob->className() == job->className())
      {
      count++;
      }
    }

  return count;
}

//------------------------------------------------------------------------------
void ctkDICOMSchedulerPrivate::insertJob(QSharedPointer<ctkDICOMJob> job)
{
  Q_Q(ctkDICOMScheduler);

  if (!job)
    {
    return;
    }

  logger.debug(QString("ctkDICOMScheduler: creating job object %1 of type %2 in thread %3.\n")
    .arg(job->jobUID())
    .arg(job->className())
    .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

  QObject::connect(job.data(), SIGNAL(started()), q, SLOT(onJobStarted()));
  QObject::connect(job.data(), SIGNAL(canceled()), q, SLOT(onJobCanceled()));
  QObject::connect(job.data(), SIGNAL(failed()), q, SLOT(onJobFailed()));
  QObject::connect(job.data(), SIGNAL(finished()), q, SLOT(onJobFinished()));
  QObject::connect(job.data(), SIGNAL(progressJobDetail(QVariant)),
                   q, SIGNAL(progressJobDetail(QVariant)));

  QMutexLocker ml(&this->mMutex);
  this->JobsQueue.insert(job->jobUID(), job);
  emit q->queueJobs();
}

//------------------------------------------------------------------------------
void ctkDICOMSchedulerPrivate::removeJob(const QString& jobUID)
{
  Q_Q(ctkDICOMScheduler);

  logger.debug(QString("ctkDICOMScheduler: deleting job object %1 in thread %2.\n")
    .arg(jobUID)
    .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()), 16)));

  QSharedPointer<ctkDICOMJob> job = this->JobsQueue.value(jobUID);
  if (!job)
    {
    return;
    }

  QObject::disconnect(job.data(), SIGNAL(started()), q, SLOT(onJobStarted()));
  QObject::disconnect(job.data(), SIGNAL(canceled()), q, SLOT(onJobCanceled()));
  QObject::disconnect(job.data(), SIGNAL(failed()), q, SLOT(onJobFailed()));
  QObject::disconnect(job.data(), SIGNAL(finished()), q, SLOT(onJobFinished()));
  QObject::disconnect(job.data(), SIGNAL(progressJobDetail(QVariant)), q, SIGNAL(progressJobDetail(QVariant)));

  this->JobsQueue.remove(jobUID);
  emit q->queueJobs();
}

//------------------------------------------------------------------------------
QString ctkDICOMSchedulerPrivate::generateUniqueJobUID()
{
  return QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
}

//------------------------------------------------------------------------------
ctkDICOMServer *ctkDICOMSchedulerPrivate::getServerFromProxyServersByConnectionName(const QString &connectionName)
{
  foreach (QSharedPointer<ctkDICOMServer> server, this->Servers)
    {
    ctkDICOMServer* proxyServer = server->proxyServer();
    if (proxyServer && proxyServer->connectionName() == connectionName)
      {
      return proxyServer;
      }
    }

  return nullptr;
}

//------------------------------------------------------------------------------
// ctkDICOMScheduler methods

//------------------------------------------------------------------------------
ctkDICOMScheduler::ctkDICOMScheduler(QObject* parentObject)
  : Superclass(parentObject)
  , d_ptr(new ctkDICOMSchedulerPrivate(*this))
{
  QObject::connect(this, SIGNAL(queueJobs()),
                   this, SLOT(onQueueJobsInThreadPool()),
                   Qt::QueuedConnection);
}

//------------------------------------------------------------------------------
ctkDICOMScheduler::~ctkDICOMScheduler()
{
  this->stopAllJobs(true);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::queryPatients(QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach(QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (!server->queryRetrieveEnabled())
      {
      continue;
      }

    QSharedPointer<ctkDICOMQueryJob> job =
      QSharedPointer<ctkDICOMQueryJob>(new ctkDICOMQueryJob);
    job->setServer(server);
    job->setMaximumPatientsQuery(d->MaximumPatientsQuery);
    job->setFilters(d->Filters);
    job->setDICOMLevel(ctkDICOMQueryJob::DICOMLevels::Patients);
    job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
    job->setRetryDelay(d->RetryDelay);
    job->setPriority(priority);

    d->insertJob(job);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::queryStudies(const QString& patientID,
                                    QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach(QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (!server->queryRetrieveEnabled())
      {
      continue;
      }

    QSharedPointer<ctkDICOMQueryJob> job =
      QSharedPointer<ctkDICOMQueryJob>(new ctkDICOMQueryJob);
    job->setServer(server);
    job->setFilters(d->Filters);
    job->setDICOMLevel(ctkDICOMQueryJob::DICOMLevels::Studies);
    job->setPatientID(patientID);
    job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
    job->setRetryDelay(d->RetryDelay);
    job->setPriority(priority);

    d->insertJob(job);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::querySeries(const QString& patientID,
                                   const QString& studyInstanceUID,
                                   QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach(QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (!server->queryRetrieveEnabled())
      {
      continue;
      }

    QSharedPointer<ctkDICOMQueryJob> job =
      QSharedPointer<ctkDICOMQueryJob>(new ctkDICOMQueryJob);
    job->setServer(server);
    job->setFilters(d->Filters);
    job->setDICOMLevel(ctkDICOMQueryJob::DICOMLevels::Series);
    job->setPatientID(patientID);
    job->setStudyInstanceUID(studyInstanceUID);
    job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
    job->setRetryDelay(d->RetryDelay);
    job->setPriority(priority);

    d->insertJob(job);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::queryInstances(const QString& patientID,
                                      const QString& studyInstanceUID,
                                      const QString& seriesInstanceUID,
                                      QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach(QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (!server->queryRetrieveEnabled())
      {
      continue;
      }

    QSharedPointer<ctkDICOMQueryJob> job =
      QSharedPointer<ctkDICOMQueryJob>(new ctkDICOMQueryJob);
    job->setServer(server);
    job->setFilters(d->Filters);
    job->setDICOMLevel(ctkDICOMQueryJob::DICOMLevels::Instances);
    job->setPatientID(patientID);
    job->setStudyInstanceUID(studyInstanceUID);
    job->setSeriesInstanceUID(seriesInstanceUID);
    job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
    job->setRetryDelay(d->RetryDelay);
    job->setPriority(priority);

    d->insertJob(job);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::retrieveStudy(const QString &patientID,
                                     const QString &studyInstanceUID,
                                     QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach(QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (!server->queryRetrieveEnabled())
      {
      continue;
      }

    QSharedPointer<ctkDICOMRetrieveJob> job =
      QSharedPointer<ctkDICOMRetrieveJob>(new ctkDICOMRetrieveJob);
    job->setServer(server);
    job->setDICOMLevel(ctkDICOMRetrieveJob::DICOMLevels::Studies);
    job->setPatientID(patientID);
    job->setStudyInstanceUID(studyInstanceUID);
    job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
    job->setRetryDelay(d->RetryDelay);
    job->setPriority(priority);

    d->insertJob(job);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::retrieveSeries(const QString &patientID,
                                      const QString &studyInstanceUID,
                                      const QString &seriesInstanceUID,
                                      QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach(QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (!server->queryRetrieveEnabled())
      {
      continue;
      }

    QSharedPointer<ctkDICOMRetrieveJob> job =
      QSharedPointer<ctkDICOMRetrieveJob>(new ctkDICOMRetrieveJob);
    job->setServer(server);
    job->setDICOMLevel(ctkDICOMRetrieveJob::DICOMLevels::Series);
    job->setPatientID(patientID);
    job->setStudyInstanceUID(studyInstanceUID);
    job->setSeriesInstanceUID(seriesInstanceUID);
    job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
    job->setRetryDelay(d->RetryDelay);
    job->setPriority(priority);

    d->insertJob(job);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::retrieveSOPInstance(const QString &patientID,
                                           const QString &studyInstanceUID,
                                           const QString &seriesInstanceUID,
                                           const QString &SOPInstanceUID,
                                           QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach(QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (!server->queryRetrieveEnabled())
      {
      continue;
      }

    QSharedPointer<ctkDICOMRetrieveJob> job =
      QSharedPointer<ctkDICOMRetrieveJob>(new ctkDICOMRetrieveJob);
    job->setServer(server);
    job->setDICOMLevel(ctkDICOMRetrieveJob::DICOMLevels::Instances);
    job->setPatientID(patientID);
    job->setStudyInstanceUID(studyInstanceUID);
    job->setSeriesInstanceUID(seriesInstanceUID);
    job->setSOPInstanceUID(SOPInstanceUID);
    job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
    job->setRetryDelay(d->RetryDelay);
    job->setPriority(priority);

    d->insertJob(job);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::startListener(const int port,
                                     const QString &AETitle,
                                     QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  QSharedPointer<ctkDICOMStorageListenerJob> job =
    QSharedPointer<ctkDICOMStorageListenerJob>(new ctkDICOMStorageListenerJob);
  job->setPort(port);
  job->setAETitle(AETitle);
  job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
  job->setRetryDelay(d->RetryDelay);
  job->setPriority(priority);

  d->insertJob(job);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::insertJobResponseSet(QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet,
                                             QThread::Priority priority)
{
  QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets;
  jobResponseSets.append(jobResponseSet);
  this->insertJobResponseSets(jobResponseSets, priority);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::insertJobResponseSets(QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets,
                                              QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  QSharedPointer<ctkDICOMInserterJob> job =
    QSharedPointer<ctkDICOMInserterJob>(new ctkDICOMInserterJob);
  job->copyJobResponseSets(jobResponseSets);
  job->setMaximumNumberOfRetry(d->MaximumNumberOfRetry);
  job->setRetryDelay(d->RetryDelay);
  job->setDatabaseFilename(d->DicomDatabase->databaseFilename());
  job->setTagsToPrecache(d->DicomDatabase->tagsToPrecache());
  job->setTagsToExcludeFromStorage(d->DicomDatabase->tagsToExcludeFromStorage());
  job->setPriority(priority);

  d->insertJob(job);
}

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMScheduler::dicomDatabase()const
{
  Q_D(const ctkDICOMScheduler);
  return d->DicomDatabase.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMScheduler::dicomDatabaseShared()const
{
  Q_D(const ctkDICOMScheduler);
  return d->DicomDatabase;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::setDicomDatabase(ctkDICOMDatabase& dicomDatabase)
{
  Q_D(ctkDICOMScheduler);
  d->DicomDatabase = QSharedPointer<ctkDICOMDatabase>(&dicomDatabase, skipDelete);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::setDicomDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase)
{
  Q_D(ctkDICOMScheduler);
  d->DicomDatabase = dicomDatabase;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::setFilters(const QMap<QString, QVariant> &filters)
{
  Q_D(ctkDICOMScheduler);
  d->Filters = filters;
}

//----------------------------------------------------------------------------
QMap<QString, QVariant> ctkDICOMScheduler::filters() const
{
  Q_D(const ctkDICOMScheduler);
  return d->Filters;
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::getNumberOfServers()
{
  Q_D(ctkDICOMScheduler);
  return d->Servers.size();
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::getNumberOfQueryRetrieveServers()
{
  Q_D(ctkDICOMScheduler);
  int numberOfServers = 0;
  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (server && server->queryRetrieveEnabled())
      {
      numberOfServers++;
      }
    }
  return numberOfServers;
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::getNumberOfStorageServers()
{
  Q_D(ctkDICOMScheduler);
  int numberOfServers = 0;
  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
    {
    if (server && server->storageEnabled())
      {
      numberOfServers++;
      }
    }
  return numberOfServers;
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMScheduler::getNthServer(int id)
{
  Q_D(ctkDICOMScheduler);
  if (id < 0 || id > d->Servers.size() - 1)
    {
    return nullptr;
    }
  return d->Servers.at(id).data();
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMScheduler::getServer(const char *connectionName)
{
  Q_D(ctkDICOMScheduler);
  ctkDICOMServer* server = this->getNthServer(this->getServerIndexFromName(connectionName));
  if (!server)
    {
    server = d->getServerFromProxyServersByConnectionName(connectionName);
    }
  return server;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::addServer(ctkDICOMServer& server)
{
  Q_D(ctkDICOMScheduler);
  QSharedPointer<ctkDICOMServer> QSharedServer = QSharedPointer<ctkDICOMServer>(&server, skipDelete);
  d->Servers.append(QSharedServer);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::addServer(QSharedPointer<ctkDICOMServer> server)
{
  Q_D(ctkDICOMScheduler);
  d->Servers.append(server);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::removeServer(const char *connectionName)
{
  this->removeNthServer(this->getServerIndexFromName(connectionName));
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::removeNthServer(int id)
{
  Q_D(ctkDICOMScheduler);
  if (id < 0 || id > d->Servers.size() - 1)
    {
    return;
    }

  d->Servers.removeAt(id);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::removeAllServers()
{
  Q_D(ctkDICOMScheduler);
  d->Servers.clear();
}

//----------------------------------------------------------------------------
QString ctkDICOMScheduler::getServerNameFromIndex(int id)
{
  Q_D(ctkDICOMScheduler);
  if (id < 0 || id > d->Servers.size() - 1)
    {
    return "";
    }

  QSharedPointer<ctkDICOMServer> server = d->Servers.at(id);
  if (!server)
    {
    return "";
    }

  return server->connectionName();
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::getServerIndexFromName(const char *connectionName)
{
  Q_D(ctkDICOMScheduler);
  if (!connectionName)
    {
    return -1;
    }
  for(int serverIndex = 0; serverIndex < d->Servers.size(); ++serverIndex)
    {
    QSharedPointer<ctkDICOMServer> server = d->Servers.at(serverIndex);
    if (server && server->connectionName() == connectionName)
      {
      // found
      return serverIndex;
      }
    }
  return -1;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::waitForFinish()
{
  Q_D(ctkDICOMScheduler);

  int numberOfPersistentJobs = this->numberOfPersistentJobs();
  while(this->numberOfJobs() > numberOfPersistentJobs)
    {
    QCoreApplication::processEvents();
    d->ThreadPool->waitForDone(300);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::waitForDone(int msec)
{
  Q_D(ctkDICOMScheduler);

  QCoreApplication::processEvents();
  d->ThreadPool->waitForDone(msec);
}

Q_INVOKABLE void waitForFinish();
void ctkDICOMScheduler::waitForFinishByUIDs(const QStringList &patientIDs,
                                            const QStringList &studyInstanceUIDs,
                                            const QStringList &seriesInstanceUIDs,
                                            const QStringList &sopInstanceUIDs)
{
  Q_D(ctkDICOMScheduler);

  if (patientIDs.count() == 0 &&
      studyInstanceUIDs.count() == 0 &&
      seriesInstanceUIDs.count() == 0 &&
      sopInstanceUIDs.count() == 0)
    {
    return;
    }

  bool wait = true;
  while(wait)
    {
    QCoreApplication::processEvents();
    d->ThreadPool->waitForDone(300);

    wait = false;
    foreach (QSharedPointer<ctkDICOMJob> job, d->JobsQueue)
      {
      if (!job)
        {
        continue;
        }

      if (job->isPersistent())
        {
        continue;
        }

      if ((!job->patientID().isEmpty() && patientIDs.contains(job->patientID())) ||
        (!job->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(job->studyInstanceUID())) ||
        (!job->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(job->seriesInstanceUID())) ||
        (!job->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(job->sopInstanceUID())))
        {
        if (job->status() != ctkAbstractJob::JobStatus::Finished)
          {
          wait = true;
          break;
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::numberOfJobs()
{
  Q_D(ctkDICOMScheduler);
  QMutexLocker ml(&d->mMutex);
  return d->JobsQueue.count();
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::numberOfPersistentJobs()
{
  Q_D(ctkDICOMScheduler);
  int cont = 0;
  QMutexLocker ml(&d->mMutex);
  foreach (QSharedPointer<ctkDICOMJob> job, d->JobsQueue)
    {
    if (job->isPersistent())
      {
      cont++;
      }
    }

  return cont;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::addJob(ctkDICOMJob *job)
{
  Q_D(ctkDICOMScheduler);

  QSharedPointer<ctkDICOMJob> jobShared = QSharedPointer<ctkDICOMJob>(job);
  d->insertJob(jobShared);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::deleteJob(const QString& jobUID)
{
  Q_D(ctkDICOMScheduler);
  d->removeJob(jobUID);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::deleteWorker(const QString& jobUID)
{
  Q_D(ctkDICOMScheduler);

  QMap<QString, QSharedPointer<ctkDICOMWorker>>::iterator it = d->Workers.find(jobUID);
  if (it == d->Workers.end())
    {
    return;
    }

  d->Workers.remove(jobUID);
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMJob> ctkDICOMScheduler::getJobSharedByUID(const QString& jobUID)
{
  Q_D(ctkDICOMScheduler);

  QMutexLocker ml(&d->mMutex);
  QMap<QString, QSharedPointer<ctkDICOMJob>>::iterator it = d->JobsQueue.find(jobUID);
  if (it == d->JobsQueue.end())
    {
    return nullptr;
    }

  return d->JobsQueue.value(jobUID);
}

//----------------------------------------------------------------------------
ctkDICOMJob *ctkDICOMScheduler::getJobByUID(const QString& jobUID)
{
  QSharedPointer<ctkDICOMJob> job = this->getJobSharedByUID(jobUID);
  if (!job)
    {
    return nullptr;
    }

  return job.data();
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::stopAllJobs(bool stopPersistentJobs)
{
  Q_D(ctkDICOMScheduler);

  QMutexLocker ml(&d->mMutex);

  // Stops jobs without a worker (in waiting)
  foreach (QSharedPointer<ctkDICOMJob> job, d->JobsQueue)
    {
    if (job->isPersistent() && !stopPersistentJobs)
      {
      continue;
      }

    if (job->status() != ctkAbstractJob::JobStatus::Initialized)
      {
      continue;
      }

    job->setStatus(ctkAbstractJob::JobStatus::Stopped);
    this->deleteJob(job->jobUID());
    }

  // Stops queued and running jobs
  foreach (QSharedPointer<ctkDICOMWorker> worker, d->Workers)
    {
    QSharedPointer<ctkAbstractJob> job = worker->jobShared();
    if (job->isPersistent() && !stopPersistentJobs)
      {
      continue;
      }

    if (job->status() != ctkAbstractJob::JobStatus::Running &&
        job->status() != ctkAbstractJob::JobStatus::Queued)
      {
      continue;
      }

    job->setStatus(ctkAbstractJob::JobStatus::Stopped);
    worker->cancel();
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::stopJobsByUIDs(const QStringList& patientIDs,
                                       const QStringList& studyInstanceUIDs,
                                       const QStringList& seriesInstanceUIDs,
                                       const QStringList& sopInstanceUIDs)
{
  Q_D(ctkDICOMScheduler);

  if (patientIDs.count() == 0 &&
      studyInstanceUIDs.count() == 0 &&
      seriesInstanceUIDs.count() == 0 &&
      sopInstanceUIDs.count() == 0)
    {
    return;
    }

  QMutexLocker ml(&d->mMutex);

  // Stops jobs without a worker (in waiting)
  foreach (QSharedPointer<ctkDICOMJob> job, d->JobsQueue)
    {
    if (!job)
      {
      continue;
      }

    if (job->isPersistent())
      {
      continue;
      }

    if (job->status() != ctkAbstractJob::JobStatus::Initialized)
      {
      continue;
      }

    if ((!job->patientID().isEmpty() && patientIDs.contains(job->patientID())) ||
        (!job->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(job->studyInstanceUID())) ||
        (!job->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(job->seriesInstanceUID())) ||
        (!job->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(job->sopInstanceUID())))
      {
      job->setStatus(ctkAbstractJob::JobStatus::Stopped);
      this->deleteJob(job->jobUID());
      }
    }

  // Stops queued and running jobs
  foreach (QSharedPointer<ctkDICOMWorker> worker, d->Workers)
    {
    QSharedPointer<ctkDICOMJob> job =
      qobject_cast<QSharedPointer<ctkDICOMJob>>(worker->jobShared());
    if (!job)
      {
      continue;
      }

    if (job->isPersistent())
      {
      continue;
      }

    if ((!job->patientID().isEmpty() && patientIDs.contains(job->patientID())) ||
        (!job->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(job->studyInstanceUID())) ||
        (!job->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(job->seriesInstanceUID())) ||
        (!job->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(job->sopInstanceUID())))
      {
      job->setStatus(ctkAbstractJob::JobStatus::Stopped);
      worker->cancel();
      }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::raiseJobsPriorityForSeries(const QStringList& selectedSeriesInstanceUIDs,
                                                   QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  if (selectedSeriesInstanceUIDs.count() == 0)
    {
    return;
    }

  QMutexLocker ml(&d->mMutex);
  foreach (QSharedPointer<ctkDICOMJob> job, d->JobsQueue)
    {
    if (job->isPersistent())
      {
      continue;
      }

    if (!selectedSeriesInstanceUIDs.contains(job->seriesInstanceUID()))
      {
      priority = QThread::Priority::LowPriority;
      }

    job->setPriority(priority);
    }
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::maximumThreadCount() const
{
  Q_D(const ctkDICOMScheduler);
  return d->ThreadPool->maxThreadCount();
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::setMaximumThreadCount(const int &maximumThreadCount)
{
  Q_D(ctkDICOMScheduler);

  d->ThreadPool->setMaxThreadCount(maximumThreadCount);
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::maximumNumberOfRetry() const
{
  Q_D(const ctkDICOMScheduler);
  return d->MaximumNumberOfRetry;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::setMaximumNumberOfRetry(const int &maximumNumberOfRetry)
{
  Q_D(ctkDICOMScheduler);
  d->MaximumNumberOfRetry = maximumNumberOfRetry;
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::retryDelay() const
{
  Q_D(const ctkDICOMScheduler);
  return d->RetryDelay;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::setRetryDelay(const int &retryDelay)
{
  Q_D(ctkDICOMScheduler);
  d->RetryDelay = retryDelay;
}

//------------------------------------------------------------------------------
void ctkDICOMScheduler::setMaximumPatientsQuery(const int maximumPatientsQuery)
{
  Q_D(ctkDICOMScheduler);
  d->MaximumPatientsQuery = maximumPatientsQuery;
}

//------------------------------------------------------------------------------
int ctkDICOMScheduler::maximumPatientsQuery()
{
  Q_D(const ctkDICOMScheduler);
  return d->MaximumPatientsQuery;
}

//----------------------------------------------------------------------------
ctkDICOMStorageListenerJob *ctkDICOMScheduler::listenerJob()
{
  Q_D(ctkDICOMScheduler);
  QMutexLocker ml(&d->mMutex);
  foreach(QSharedPointer<ctkDICOMJob> job, d->JobsQueue)
    {
    QSharedPointer<ctkDICOMStorageListenerJob> listenerJob =
      qobject_cast<QSharedPointer<ctkDICOMStorageListenerJob>>(job);
    if (listenerJob)
      {
      return listenerJob.data();
      }
    }

  return nullptr;
}

//----------------------------------------------------------------------------
bool ctkDICOMScheduler::isStorageListenerActive()
{
  ctkDICOMStorageListenerJob* listenerJob = this->listenerJob();
  if (listenerJob &&
      listenerJob->status() == ctkAbstractJob::JobStatus::Running)
    {
    return true;
    }
  return false;
}

//----------------------------------------------------------------------------
QThreadPool *ctkDICOMScheduler::threadPool() const
{
  Q_D(const ctkDICOMScheduler);
  return d->ThreadPool.data();
}

//----------------------------------------------------------------------------
QSharedPointer<QThreadPool> ctkDICOMScheduler::threadPoolShared() const
{
  Q_D(const ctkDICOMScheduler);
  return d->ThreadPool;
}

//----------------------------------------------------------------------------
QVariant ctkDICOMScheduler::jobToDetail(ctkDICOMJob* job)
{
  ctkJobDetail td;
  td.JobClass = job->className();
  td.DICOMLevel = job->dicomLevel();
  td.JobUID = job->jobUID();
  td.PatientID = job->patientID();
  td.StudyInstanceUID = job->studyInstanceUID();
  td.SeriesInstanceUID = job->seriesInstanceUID();
  td.SOPInstanceUID = job->sopInstanceUID();
  ctkDICOMQueryJob* queryJob = qobject_cast<ctkDICOMQueryJob*>(job);
  if (queryJob && queryJob->server())
    {
    td.ConnectionName = queryJob->server()->connectionName();
    }
  ctkDICOMRetrieveJob* retrieveJob = qobject_cast<ctkDICOMRetrieveJob*>(job);
  if (retrieveJob && retrieveJob->server())
    {
    td.ConnectionName = retrieveJob->server()->connectionName();
    }

  QVariant data;
  data.setValue(td);

  return data;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onJobStarted()
{
  ctkDICOMJob* job = qobject_cast<ctkDICOMJob*>(this->sender());
  if (!job)
    {
    return;
    }

  logger.debug(job->loggerReport("started"));
  emit this->jobStarted(this->jobToDetail(job));
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onJobCanceled()
{
  ctkDICOMJob* job = qobject_cast<ctkDICOMJob*>(this->sender());
  if (!job)
    {
    return;
    }

  logger.debug(job->loggerReport("canceled"));
  emit this->jobCanceled(this->jobToDetail(job));
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onJobFailed()
{
  ctkDICOMJob* job = qobject_cast<ctkDICOMJob*>(this->sender());
  if (!job)
    {
    return;
    }

  logger.debug(job->loggerReport("failed"));

  QVariant data = this->jobToDetail(job);
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  emit this->jobFailed(data);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onJobFinished()
{
  ctkDICOMJob* job = qobject_cast<ctkDICOMJob*>(this->sender());
  if (!job)
    {
    return;
    }

  logger.debug(job->loggerReport("finished"));

  QVariant data = this->jobToDetail(job);
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  emit this->jobFinished(data);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onQueueJobsInThreadPool()
{
  Q_D(ctkDICOMScheduler);

  QList<QThread::Priority> threadPriorityEnums;
  threadPriorityEnums.append(QThread::Priority::HighestPriority);
  threadPriorityEnums.append(QThread::Priority::HighPriority);
  threadPriorityEnums.append(QThread::Priority::NormalPriority);
  threadPriorityEnums.append(QThread::Priority::LowPriority);
  threadPriorityEnums.append(QThread::Priority::LowestPriority);

  QMutexLocker ml(&d->mMutex);
  foreach (QThread::Priority priority, threadPriorityEnums)
    {
    foreach(QSharedPointer<ctkDICOMJob> job, d->JobsQueue)
      {
      if (job->priority() != priority)
        {
        continue;
        }

      if (job->status() != ctkAbstractJob::JobStatus::Initialized)
        {
        continue;
        }

      int numberOfRunningJobsWithSameType = d->getSameTypeJobsInThreadPoolQueueOrRunning(job);
      if (numberOfRunningJobsWithSameType >= job->maximumConcurrentJobsPerType())
        {
        continue;
        }

      logger.debug(QString("ctkDICOMScheduler: creating worker for job %1 in thread %2.\n")
                       .arg(job->jobUID())
                       .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

      job->setStatus(ctkAbstractJob::JobStatus::Queued);

      QSharedPointer<ctkDICOMWorker> worker =
        QSharedPointer<ctkDICOMWorker>(job->createWorker());
      worker->setScheduler(*this);

      d->Workers.insert(job->jobUID(), worker);
      d->ThreadPool->start(worker.data(), job->priority());
      }
    }
}
