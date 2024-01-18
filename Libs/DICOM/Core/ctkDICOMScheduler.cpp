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
#include <ctkJobScheduler_p.h>
#include <ctkAbstractWorker.h>

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

// dcmtk includes
#include <dcmtk/dcmdata/dcdeftag.h>


static ctkLogger logger ( "org.commontk.dicom.DICOMScheduler" );

//------------------------------------------------------------------------------
// ctkDICOMSchedulerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMSchedulerPrivate::ctkDICOMSchedulerPrivate(ctkDICOMScheduler& obj)
  : ctkJobSchedulerPrivate(obj)
{
  ctk::setDICOMLogLevel(ctkErrorLogLevel::Warning);
}

//------------------------------------------------------------------------------
ctkDICOMSchedulerPrivate::~ctkDICOMSchedulerPrivate()
{
  Q_Q(ctkDICOMScheduler);
  q->removeAllServers();
}

//------------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMSchedulerPrivate::getServerFromProxyServersByConnectionName(const QString& connectionName)
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
  : Superclass(new ctkDICOMSchedulerPrivate(*this), parentObject)
{
  Q_D(ctkDICOMScheduler);
  d->init();
}

// --------------------------------------------------------------------------
ctkDICOMScheduler::ctkDICOMScheduler(ctkDICOMSchedulerPrivate* pimpl, QObject* parentObject)
  : Superclass(pimpl, parentObject)
{
  // derived classes must call init manually. Calling init() here may results in
  // actions on a derived public class not yet finished to be created
}

//------------------------------------------------------------------------------
ctkDICOMScheduler::~ctkDICOMScheduler()
{
  this->stopAllJobs(true);
  this->waitForDone(2000);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::queryPatients(QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
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

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
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

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
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

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
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
void ctkDICOMScheduler::retrieveStudy(const QString& patientID,
                                      const QString& studyInstanceUID,
                                      QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
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
void ctkDICOMScheduler::retrieveSeries(const QString& patientID,
                                       const QString& studyInstanceUID,
                                       const QString& seriesInstanceUID,
                                       QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
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
void ctkDICOMScheduler::retrieveSOPInstance(const QString& patientID,
                                            const QString& studyInstanceUID,
                                            const QString& seriesInstanceUID,
                                            const QString& SOPInstanceUID,
                                            QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
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
void ctkDICOMScheduler::startListener(int port,
                                      const QString& AETitle,
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
void ctkDICOMScheduler::insertJobResponseSet(const QSharedPointer<ctkDICOMJobResponseSet>& jobResponseSet,
                                             QThread::Priority priority)
{
  QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets;
  jobResponseSets.append(jobResponseSet);
  this->insertJobResponseSets(jobResponseSets, priority);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::insertJobResponseSets(const QList<QSharedPointer<ctkDICOMJobResponseSet>>& jobResponseSets,
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
ctkDICOMDatabase* ctkDICOMScheduler::dicomDatabase() const
{
  Q_D(const ctkDICOMScheduler);
  return d->DicomDatabase.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMScheduler::dicomDatabaseShared() const
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
void ctkDICOMScheduler::setFilters(const QMap<QString, QVariant>& filters)
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
ctkDICOMServer* ctkDICOMScheduler::getServer(const QString& connectionName)
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
void ctkDICOMScheduler::removeServer(const QString& connectionName)
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
int ctkDICOMScheduler::getServerIndexFromName(const QString& connectionName)
{
  Q_D(ctkDICOMScheduler);
  for (int serverIndex = 0; serverIndex < d->Servers.size(); ++serverIndex)
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
void ctkDICOMScheduler::waitForFinishByDICOMUIDs(const QStringList& patientIDs,
                                                 const QStringList& studyInstanceUIDs,
                                                 const QStringList& seriesInstanceUIDs,
                                                 const QStringList& sopInstanceUIDs)
{
  Q_D(ctkDICOMScheduler);

  QList<int> numberOfIDsPerLevel;
  numberOfIDsPerLevel.append(patientIDs.count());
  numberOfIDsPerLevel.append(studyInstanceUIDs.count());
  numberOfIDsPerLevel.append(seriesInstanceUIDs.count());
  numberOfIDsPerLevel.append(sopInstanceUIDs.count());

  int count = 0;
  foreach (int numberOfIDs, numberOfIDsPerLevel)
    {
    if (numberOfIDs == 0)
      {
      count++;
      }
    }

  if (count !=3 )
    {
    logger.warn("ctkDICOMScheduler::waitForFinishByDICOMUIDs failed: provide only one list to the method.");
    return;
    }

  bool wait = true;
  while (wait)
    {
    QCoreApplication::processEvents();
    d->ThreadPool->waitForDone(300);

    wait = false;
    foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
      {
      if (!job)
        {
        continue;
        }

      if (job->isPersistent())
        {
        continue;
        }
      ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
      if (!dicomJob)
        {
        continue;
        }

      if ((!dicomJob->patientID().isEmpty() && patientIDs.contains(dicomJob->patientID())) ||
        (!dicomJob->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(dicomJob->studyInstanceUID())) ||
        (!dicomJob->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(dicomJob->seriesInstanceUID())) ||
        (!dicomJob->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(dicomJob->sopInstanceUID())))
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
QList<QSharedPointer<ctkAbstractJob>> ctkDICOMScheduler::getJobsByDICOMUIDs(const QStringList &patientIDs,
                                                                            const QStringList &studyInstanceUIDs,
                                                                            const QStringList &seriesInstanceUIDs,
                                                                            const QStringList &sopInstanceUIDs)
{
  Q_D(ctkDICOMScheduler);

  QList<QSharedPointer<ctkAbstractJob>> jobs;

  QList<int> numberOfIDsPerLevel;
  numberOfIDsPerLevel.append(patientIDs.count());
  numberOfIDsPerLevel.append(studyInstanceUIDs.count());
  numberOfIDsPerLevel.append(seriesInstanceUIDs.count());
  numberOfIDsPerLevel.append(sopInstanceUIDs.count());

  int count = 0;
  foreach (int numberOfIDs, numberOfIDsPerLevel)
    {
    if (numberOfIDs == 0)
      {
      count++;
      }
    }

  if (count !=3 )
    {
    logger.warn("ctkDICOMScheduler::getJobsByDICOMUIDs failed: provide only one list to the method.");
    return jobs;
    }

  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
    if (!job)
      {
      continue;
      }

    ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
    if (!dicomJob)
      {
      qCritical() << Q_FUNC_INFO << " failed: unexpected type of job";
      continue;
      }

    if ((!dicomJob->patientID().isEmpty() && patientIDs.contains(dicomJob->patientID())) ||
        (!dicomJob->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(dicomJob->studyInstanceUID())) ||
        (!dicomJob->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(dicomJob->seriesInstanceUID())) ||
        (!dicomJob->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(dicomJob->sopInstanceUID())))
      {
      jobs.push_back(job);
      }
    }

  return jobs;
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::stopJobsByDICOMUIDs(const QStringList& patientIDs,
                                            const QStringList& studyInstanceUIDs,
                                            const QStringList& seriesInstanceUIDs,
                                            const QStringList& sopInstanceUIDs)
{
  Q_D(ctkDICOMScheduler);

  QList<int> numberOfIDsPerLevel;
  numberOfIDsPerLevel.append(patientIDs.count());
  numberOfIDsPerLevel.append(studyInstanceUIDs.count());
  numberOfIDsPerLevel.append(seriesInstanceUIDs.count());
  numberOfIDsPerLevel.append(sopInstanceUIDs.count());

  int count = 0;
  foreach (int numberOfIDs, numberOfIDsPerLevel)
    {
    if (numberOfIDs == 0)
      {
      count++;
      }
    }

  if (count !=3 )
    {
    logger.warn("ctkDICOMScheduler::stopJobsByDICOMUIDs failed: provide only one list to the method.");
    return;
    }

  QMutexLocker ml(&d->mMutex);

  // Stops jobs without a worker (in waiting)
  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
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

    ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
    if (!dicomJob)
      {
      qCritical() << Q_FUNC_INFO << " failed: unexpected type of job";
      continue;
      }

    if ((!dicomJob->patientID().isEmpty() && patientIDs.contains(dicomJob->patientID())) ||
        (!dicomJob->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(dicomJob->studyInstanceUID())) ||
        (!dicomJob->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(dicomJob->seriesInstanceUID())) ||
        (!dicomJob->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(dicomJob->sopInstanceUID())))
      {
      job->setStatus(ctkAbstractJob::JobStatus::Stopped);
      job->canceled();
      this->deleteJob(job->jobUID());
      }
    }

  // Stops queued and running jobs
  foreach (QSharedPointer<ctkAbstractWorker> worker, d->Workers)
    {
    QSharedPointer<ctkAbstractJob> job = worker->jobShared();
    if (!job)
      {
      continue;
      }

    if (job->isPersistent())
      {
      continue;
      }

    ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
    if (!dicomJob)
      {
      qCritical() << Q_FUNC_INFO << " failed: unexpected type of job";
      continue;
      }

    if ((!dicomJob->patientID().isEmpty() && patientIDs.contains(dicomJob->patientID())) ||
        (!dicomJob->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(dicomJob->studyInstanceUID())) ||
        (!dicomJob->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(dicomJob->seriesInstanceUID())) ||
        (!dicomJob->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(dicomJob->sopInstanceUID())))
      {
      job->setStatus(ctkAbstractJob::JobStatus::Stopped);
      worker->cancel();
      }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::stopJobsByJobUIDs(const QStringList &jobUIDs)
{
  Q_D(ctkDICOMScheduler);

  if (jobUIDs.count() == 0)
    {
    return;
    }

  QMutexLocker ml(&d->mMutex);

  // Stops jobs without a worker (in waiting)
  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
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

    ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
    if (!dicomJob)
      {
      qCritical() << Q_FUNC_INFO << " failed: unexpected type of job";
      continue;
      }

    if ((!dicomJob->jobUID().isEmpty() && jobUIDs.contains(dicomJob->jobUID())))
      {
      job->setStatus(ctkAbstractJob::JobStatus::Stopped);
      job->canceled();
      this->deleteJob(job->jobUID());
      }
    }

  // Stops queued and running jobs
  foreach (QSharedPointer<ctkAbstractWorker> worker, d->Workers)
    {
    QSharedPointer<ctkAbstractJob> job = worker->jobShared();
    if (!job)
      {
      continue;
      }

    if (job->isPersistent())
      {
      continue;
      }

    ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
    if (!dicomJob)
      {
      qCritical() << Q_FUNC_INFO << " failed: unexpected type of job";
      continue;
      }

    if ((!dicomJob->jobUID().isEmpty() && jobUIDs.contains(dicomJob->jobUID())))
      {
      job->setStatus(ctkAbstractJob::JobStatus::Stopped);
      worker->cancel();
      }
  }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::runJobs(const QMap<QString, ctkDICOMJobDetail> &jobDetails)
{
  for(QString jobUID : jobDetails.keys())
    {
    ctkDICOMJobDetail jd = jobDetails.value(jobUID);
    if (jd.JobClass == "ctkDICOMQueryJob")
      {
      switch (jd.DICOMLevel)
        {
        case ctkDICOMJob::DICOMLevels::Patients:
          this->queryPatients();
        break;
        case ctkDICOMJob::DICOMLevels::Studies:
          this->queryStudies(jd.PatientID);
        break;
        case ctkDICOMJob::DICOMLevels::Series:
          this->querySeries(jd.PatientID,
                            jd.StudyInstanceUID);
        break;
        case ctkDICOMJob::DICOMLevels::Instances:
          this->queryInstances(jd.PatientID,
                               jd.StudyInstanceUID,
                               jd.SeriesInstanceUID);
        break;
        }
      }
    else if (jd.JobClass == "ctkDICOMRetrieveJob")
      {
      switch (jd.DICOMLevel)
        {
        case ctkDICOMJob::DICOMLevels::Patients:
          logger.warn("Retrieve Patient is not implemented");
        break;
        case ctkDICOMJob::DICOMLevels::Studies:
          this->retrieveStudy(jd.PatientID,
                              jd.StudyInstanceUID);
        break;
        case ctkDICOMJob::DICOMLevels::Series:
          this->retrieveSeries(jd.PatientID,
                               jd.StudyInstanceUID,
                               jd.SeriesInstanceUID);
        break;
        case ctkDICOMJob::DICOMLevels::Instances:
          this->retrieveSOPInstance(jd.PatientID,
                                    jd.StudyInstanceUID,
                                    jd.SeriesInstanceUID,
                                    jd.SOPInstanceUID);
        break;
        }
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
  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
    if (job->isPersistent())
      {
      continue;
      }

    ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
    if (!dicomJob)
      {
      qCritical() << Q_FUNC_INFO << " failed: unexpected type of job";
      continue;
      }

    if (!selectedSeriesInstanceUIDs.contains(dicomJob->seriesInstanceUID()))
      {
      priority = QThread::Priority::LowPriority;
      }

    job->setPriority(priority);
    }
}

//------------------------------------------------------------------------------
void ctkDICOMScheduler::setMaximumPatientsQuery(int maximumPatientsQuery)
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
ctkDICOMStorageListenerJob* ctkDICOMScheduler::listenerJob()
{
  Q_D(ctkDICOMScheduler);
  QMutexLocker ml(&d->mMutex);
  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
    QSharedPointer<ctkDICOMStorageListenerJob> listenerJob =
      qSharedPointerObjectCast<ctkDICOMStorageListenerJob>(job);
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
