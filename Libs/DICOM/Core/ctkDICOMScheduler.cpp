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
#include "ctkDICOMEchoJob.h"
#include "ctkDICOMThumbnailGeneratorJob.h"
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
#include <dcmtk/oflog/layout.h>
#include <dcmtk/oflog/oflog.h>
#include "dcmtk/oflog/spi/logevent.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMScheduler" );

//------------------------------------------------------------------------------
// JobAppender class (custom DCMTK Appender)

//------------------------------------------------------------------------------
class ctkDICOMJobsAppender : public dcmtk::log4cplus::Appender {
public:
  ctkDICOMJobsAppender() {};

  virtual void close() {};

  QMap<QString, QString> messagesPerThread() const
  {
    return this->MessagesPerThread;
  }

  QString messageByThreadID(QString threadID)
  {
    if (!this->MessagesPerThread.contains(threadID))
    {
      return "";
    }

    return this->MessagesPerThread.take(threadID);
  }

  void clearMessagesPerThread()
  {
    this->MessagesPerThread.clear();
  }

protected:
  void append(const dcmtk::log4cplus::spi::InternalLoggingEvent& event)
  {
    dcmtk::log4cplus::PatternLayout *layout =
      dynamic_cast<dcmtk::log4cplus::PatternLayout*>(this->getLayout());
    if (!layout)
    {
      return;
    }

    dcmtk::log4cplus::tostringstream oss;
    layout->formatAndAppend(oss, event);

    QString message = QString::fromLatin1(oss.str().c_str());
    if (message.isEmpty())
    {
      return;
    }

    QString messageThread = event.getThread().c_str();
    this->MessagesPerThread[messageThread] += message;
  }

private:
  QMap<QString, QString> MessagesPerThread;
};

//------------------------------------------------------------------------------
// ctkDICOMSchedulerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMSchedulerPrivate::ctkDICOMSchedulerPrivate(ctkDICOMScheduler& obj)
  : ctkJobSchedulerPrivate(obj)
{
  ctk::setDICOMLogLevel(ctkErrorLogLevel::Warning);

  OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout("%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n"));
  this->Appender = (new ctkDICOMJobsAppender());
  this->Appender->setName("ctkDICOM");
  this->Appender->setLayout(OFmove(layout));
  this->Appender->setThreshold(ctk::dicomLogLevel());

  dcmtk::log4cplus::Logger rootLog = dcmtk::log4cplus::Logger::getRoot();
  rootLog.addAppender(this->Appender);
}

//------------------------------------------------------------------------------
ctkDICOMSchedulerPrivate::~ctkDICOMSchedulerPrivate()
{
  Q_Q(ctkDICOMScheduler);
  q->removeAllServers();

  if (this->Appender)
  {
    dcmtk::log4cplus::Logger rootLog = dcmtk::log4cplus::Logger::getRoot();
    rootLog.removeAppender(this->Appender);
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMSchedulerPrivate::isServerAllowed(ctkDICOMServer *server,
                                               const QStringList& allowedSeversForPatient)
{
  if (!server)
  {
    return false;
  }

  if (!server->queryRetrieveEnabled())
  {
    return false;
  }
  else if (server->trustedEnabled())
  {
    return true;
  }
  else if (allowedSeversForPatient.contains(server->connectionName()))
  {
    return true;
  }
  else
  {
    return false;
  }
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
ctkDICOMScheduler::~ctkDICOMScheduler() = default;

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
    job->setServer(*server);
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
                                     QThread::Priority priority,
                                     const QStringList& allowedSeversForPatient)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
  {
    if (!d->isServerAllowed(server.data(), allowedSeversForPatient))
    {
      continue;
    }

    QSharedPointer<ctkDICOMQueryJob> job =
      QSharedPointer<ctkDICOMQueryJob>(new ctkDICOMQueryJob);
    job->setServer(*server);
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
                                    QThread::Priority priority,
                                    const QStringList& allowedSeversForPatient)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
  {
    if (!d->isServerAllowed(server.data(), allowedSeversForPatient))
    {
      continue;
    }

    QSharedPointer<ctkDICOMQueryJob> job =
      QSharedPointer<ctkDICOMQueryJob>(new ctkDICOMQueryJob);
    job->setServer(*server);
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
                                       QThread::Priority priority,
                                       const QStringList& allowedSeversForPatient)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
  {
    if (!d->isServerAllowed(server.data(), allowedSeversForPatient))
    {
      continue;
    }

    QSharedPointer<ctkDICOMQueryJob> job =
      QSharedPointer<ctkDICOMQueryJob>(new ctkDICOMQueryJob);
    job->setServer(*server);
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
                                      QThread::Priority priority,
                                      const QStringList& allowedSeversForPatient)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
  {
    if (!d->isServerAllowed(server.data(), allowedSeversForPatient))
    {
      continue;
    }

    QSharedPointer<ctkDICOMRetrieveJob> job =
      QSharedPointer<ctkDICOMRetrieveJob>(new ctkDICOMRetrieveJob);
    job->setServer(*server);
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
                                       QThread::Priority priority,
                                       const QStringList& allowedSeversForPatient)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
  {
    if (!d->isServerAllowed(server.data(), allowedSeversForPatient))
    {
      continue;
    }

    QSharedPointer<ctkDICOMRetrieveJob> job =
      QSharedPointer<ctkDICOMRetrieveJob>(new ctkDICOMRetrieveJob);
    job->setServer(*server);
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
                                            QThread::Priority priority,
                                            const QStringList& allowedSeversForPatient)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
  {
    if (!d->isServerAllowed(server.data(), allowedSeversForPatient))
    {
      continue;
    }

    QSharedPointer<ctkDICOMRetrieveJob> job =
      QSharedPointer<ctkDICOMRetrieveJob>(new ctkDICOMRetrieveJob);
    job->setServer(*server);
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
void ctkDICOMScheduler::echo(const QString &connectionName,
                             QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  foreach (QSharedPointer<ctkDICOMServer> server, d->Servers)
  {
    if (server->connectionName() != connectionName)
    {
      continue;
    }

    QSharedPointer<ctkDICOMEchoJob> job =
      QSharedPointer<ctkDICOMEchoJob>(new ctkDICOMEchoJob);

    job->setServer(*server);
    job->setMaximumNumberOfRetry(0);
    job->setPriority(priority);

    d->insertJob(job);
    break;
  }
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::echo(ctkDICOMServer &server, QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  QSharedPointer<ctkDICOMEchoJob> job =
    QSharedPointer<ctkDICOMEchoJob>(new ctkDICOMEchoJob);

  job->setServer(server);
  job->setMaximumNumberOfRetry(0);
  job->setPriority(priority);

  d->insertJob(job);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::generateThumbnail(const QString &originalFilePath,
                                          const QString &patientID,
                                          const QString &studyInstanceUID,
                                          const QString &seriesInstanceUID,
                                          const QString &sopInstanceUID,
                                          const QString &modality,
                                          QColor backgroundColor,
                                          QThread::Priority priority)
{
  Q_D(ctkDICOMScheduler);

  QSharedPointer<ctkDICOMThumbnailGeneratorJob> job =
    QSharedPointer<ctkDICOMThumbnailGeneratorJob>(new ctkDICOMThumbnailGeneratorJob);
  job->setDatabaseFilename(d->DicomDatabase->databaseFilename());
  job->setDicomFilePath(originalFilePath);
  job->setModality(modality);
  job->setBackgroundColor(backgroundColor);
  job->setPatientID(patientID);
  job->setStudyInstanceUID(studyInstanceUID);
  job->setSeriesInstanceUID(seriesInstanceUID);
  job->setSOPInstanceUID(sopInstanceUID);
  job->setMaximumNumberOfRetry(0);
  job->setPriority(priority);

  d->insertJob(job);
}

//----------------------------------------------------------------------------
QString ctkDICOMScheduler::insertJobResponseSet(const QSharedPointer<ctkDICOMJobResponseSet>& jobResponseSet,
                                                QThread::Priority priority)
{
  QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets;
  jobResponseSets.append(jobResponseSet);
  return this->insertJobResponseSets(jobResponseSets, priority);
}

//----------------------------------------------------------------------------
QString ctkDICOMScheduler::insertJobResponseSets(const QList<QSharedPointer<ctkDICOMJobResponseSet>>& jobResponseSets,
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

  QString inserterJobUID = job->jobUID();
  d->insertJob(job);
  return inserterJobUID;
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
int ctkDICOMScheduler::serversCount()
{
  Q_D(ctkDICOMScheduler);
  return d->Servers.size();
}

//----------------------------------------------------------------------------
int ctkDICOMScheduler::queryRetrieveServersCount()
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
int ctkDICOMScheduler::storageServersCount()
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
ctkDICOMServer* ctkDICOMScheduler::server(int id)
{
  Q_D(ctkDICOMScheduler);
  if (id < 0 || id > d->Servers.size() - 1)
  {
    return nullptr;
  }
  return d->Servers.at(id).data();
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMScheduler::server(const QString& connectionName)
{
  Q_D(ctkDICOMScheduler);
  ctkDICOMServer* server = this->server(this->getServerIndexFromName(connectionName));
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

  QMetaObject::Connection serverModifiedConnection =
    this->connect(QSharedServer.data(), SIGNAL(serverModified(QString)),
                  this, SIGNAL(serverModified(QString)));
  d->ServersConnections.insert(QSharedServer->connectionName(), serverModifiedConnection);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::addServer(QSharedPointer<ctkDICOMServer> server)
{
  Q_D(ctkDICOMScheduler);
  d->Servers.append(server);

  QMetaObject::Connection serverModifiedConnection =
    this->connect(server.data(), SIGNAL(serverModified(QString)),
                  this, SIGNAL(serverModified(QString)));
  d->ServersConnections.insert(server->connectionName(), serverModifiedConnection);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::removeServer(const QString& connectionName)
{
  this->removeServer(this->getServerIndexFromName(connectionName));
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::removeServer(int id)
{
  Q_D(ctkDICOMScheduler);
  if (id < 0 || id > d->Servers.size() - 1)
  {
    return;
  }

  ctkDICOMServer* server = this->server(id);
  if (!server)
  {
    return;
  }

  QMetaObject::Connection connection = d->ServersConnections.value(server->connectionName());
  QObject::disconnect(connection);

  d->Servers.removeAt(id);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::removeAllServers()
{
  Q_D(ctkDICOMScheduler);
  d->Servers.clear();
  for (QMap<QString, QMetaObject::Connection>::iterator it = d->ServersConnections.begin();
       it != d->ServersConnections.end(); ++it)
  {
    QObject::disconnect(it.value());
  }
  d->ServersConnections.clear();
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
QStringList ctkDICOMScheduler::getAllServersConnectionNames()
{
  Q_D(ctkDICOMScheduler);

  QStringList connectionNames;
  for (int serverIndex = 0; serverIndex < d->Servers.size(); ++serverIndex)
  {
    QSharedPointer<ctkDICOMServer> server = d->Servers.at(serverIndex);
    if (!server)
    {
      continue;
    }

    connectionNames.append(server->connectionName());
  }

  return connectionNames;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMScheduler::getConnectionNamesForActiveServers()
{
Q_D(ctkDICOMScheduler);

  QStringList connectionNames;
  for (int serverIndex = 0; serverIndex < d->Servers.size(); ++serverIndex)
  {
    QSharedPointer<ctkDICOMServer> server = d->Servers.at(serverIndex);
    if (!server || (!server->queryRetrieveEnabled() && !server->storageEnabled()))
    {
      continue;
    }

    connectionNames.append(server->connectionName());
  }

  return connectionNames;
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

  int numberOfInputLists = 0;
  foreach (int numberOfIDs, numberOfIDsPerLevel)
  {
    if (numberOfIDs != 0)
    {
      numberOfInputLists++;
    }
  }

  if (numberOfInputLists == 0)
  {
    logger.warn("ctkDICOMScheduler::waitForFinishByDICOMUIDs failed: all the provided lists with UIDs are empty.");
    return;
  }

  {
    // The QReadLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QReadLockers within the scheduler's methods.
    QReadLocker locker(&d->QueueLock);
    bool wait = true;
    while (wait)
    {
      qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
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
}

//----------------------------------------------------------------------------
QList<QSharedPointer<ctkAbstractJob>> ctkDICOMScheduler::getJobsByDICOMUIDs(const QStringList &patientIDs,
                                                                            const QStringList &studyInstanceUIDs,
                                                                            const QStringList &seriesInstanceUIDs,
                                                                            const QStringList &sopInstanceUIDs,
                                                                            QList<ctkAbstractJob::JobStatus> statusFilters)
{
  Q_D(ctkDICOMScheduler);

  QList<QSharedPointer<ctkAbstractJob>> jobs;

  QList<int> numberOfIDsPerLevel;
  numberOfIDsPerLevel.append(patientIDs.count());
  numberOfIDsPerLevel.append(studyInstanceUIDs.count());
  numberOfIDsPerLevel.append(seriesInstanceUIDs.count());
  numberOfIDsPerLevel.append(sopInstanceUIDs.count());

  int numberOfInputLists = 0;
  foreach (int numberOfIDs, numberOfIDsPerLevel)
  {
    if (numberOfIDs != 0)
    {
      numberOfInputLists++;
    }
  }

  if (numberOfInputLists == 0)
  {
    logger.warn("ctkDICOMScheduler::getJobsByDICOMUIDs failed: all the provided lists with UIDs are empty.");
    return jobs;
  }

  {
    // The QReadLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QReadLockers within the scheduler's methods.
    QReadLocker locker(&d->QueueLock);
    foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
      if (!job)
      {
        continue;
      }

      ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
      if (!dicomJob)
      {
        logger.debug("ctkDICOMScheduler::getJobsByDICOMUIDs: unexpected type of job.");
        continue;
      }

      if (
        (
          (!dicomJob->patientID().isEmpty() && patientIDs.contains(dicomJob->patientID())) ||
          (!dicomJob->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(dicomJob->studyInstanceUID())) ||
          (!dicomJob->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(dicomJob->seriesInstanceUID())) ||
          (!dicomJob->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(dicomJob->sopInstanceUID()))
        )
        && (statusFilters.isEmpty() || statusFilters.contains(job->status()))
      )
      {
        jobs.push_back(job);
      }
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

  int numberOfInputLists = 0;
  foreach (int numberOfIDs, numberOfIDsPerLevel)
  {
    if (numberOfIDs != 0)
    {
      numberOfInputLists++;
    }
  }

  if (numberOfInputLists == 0)
  {
    logger.debug("ctkDICOMScheduler::stopJobsByDICOMUIDs: all the provided lists with UIDs are empty.");
    return;
  }

  QStringList jobsUIDs;
  {
    // The QReadLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QReadLockers within the scheduler's methods.
    QReadLocker locker(&d->QueueLock);
    // Stops jobs without a worker (in waiting, still in main thread)
    foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
      if (!job)
      {
        continue;
      }

      ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
      if (!dicomJob)
      {
        logger.debug("ctkDICOMScheduler::stopJobsByDICOMUIDs: unexpected type of job.");
        continue;
      }

      if ((!dicomJob->patientID().isEmpty() && patientIDs.contains(dicomJob->patientID())) ||
          (!dicomJob->studyInstanceUID().isEmpty() && studyInstanceUIDs.contains(dicomJob->studyInstanceUID())) ||
          (!dicomJob->seriesInstanceUID().isEmpty() && seriesInstanceUIDs.contains(dicomJob->seriesInstanceUID())) ||
          (!dicomJob->sopInstanceUID().isEmpty() && sopInstanceUIDs.contains(dicomJob->sopInstanceUID())))
      {
        jobsUIDs.append(dicomJob->jobUID());
      }
    }
  }

  this->stopJobsByJobUIDs(jobsUIDs);
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

  {
    // The QWriteLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QWriteLockers within the scheduler's methods.
    QWriteLocker locker(&d->QueueLock);
    foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
      if (job->isPersistent())
      {
        continue;
      }

      ctkDICOMJob* dicomJob = qobject_cast<ctkDICOMJob*>(job.data());
      if (!dicomJob)
      {
        logger.debug("ctkDICOMScheduler::raiseJobsPriorityForSeries: unexpected type of job.");
        continue;
      }

      if (!selectedSeriesInstanceUIDs.contains(dicomJob->seriesInstanceUID()))
      {
        priority = QThread::Priority::LowPriority;
      }

      job->setPriority(priority);
    }
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
  ctkDICOMStorageListenerJob* listenerJobRaw = nullptr;

  {
    // The QWriteLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QWriteLockers within the scheduler's methods.
    QWriteLocker locker(&d->QueueLock);
    foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
      QSharedPointer<ctkDICOMStorageListenerJob> listenerJob =
        qSharedPointerObjectCast<ctkDICOMStorageListenerJob>(job);
      if (listenerJob)
      {
        listenerJobRaw = listenerJob.data();
        break;
      }
    }
  }

  return listenerJobRaw;
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
void ctkDICOMScheduler::onJobStarted(ctkAbstractJob* job)
{
  Q_D(ctkDICOMScheduler);
  if (!job)
  {
    return;
  }

  ctkDICOMJobsAppender* appender = dynamic_cast<ctkDICOMJobsAppender*>(d->Appender.get());
  if (appender)
  {
    job->addLog(appender->messageByThreadID(job->runningThreadID()));
  }

  ctkJobScheduler::onJobStarted(job);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onJobUserStopped(ctkAbstractJob* job)
{
  Q_D(ctkDICOMScheduler);
  if (!job)
  {
    return;
  }

  ctkDICOMJobsAppender* appender = dynamic_cast<ctkDICOMJobsAppender*>(d->Appender.get());
  if (appender)
  {
    job->addLog(appender->messageByThreadID(job->runningThreadID()));
  }

  ctkJobScheduler::onJobUserStopped(job);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onJobFinished(ctkAbstractJob* job)
{
  Q_D(ctkDICOMScheduler);
  if (!job)
  {
    return;
  }

  ctkDICOMJobsAppender* appender = dynamic_cast<ctkDICOMJobsAppender*>(d->Appender.get());
  if (appender)
  {
    job->addLog(appender->messageByThreadID(job->runningThreadID()));
  }

  ctkJobScheduler::onJobFinished(job);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onJobAttemptFailed(ctkAbstractJob* job)
{
  Q_D(ctkDICOMScheduler);
  if (!job)
  {
    return;
  }

  ctkDICOMJobsAppender* appender = dynamic_cast<ctkDICOMJobsAppender*>(d->Appender.get());
  if (appender)
  {
    job->addLog(appender->messageByThreadID(job->runningThreadID()));
  }

  ctkJobScheduler::onJobAttemptFailed(job);
}

//----------------------------------------------------------------------------
void ctkDICOMScheduler::onJobFailed(ctkAbstractJob* job)
{
  Q_D(ctkDICOMScheduler);
  if (!job)
  {
    return;
  }

  ctkDICOMJobsAppender* appender = dynamic_cast<ctkDICOMJobsAppender*>(d->Appender.get());
  if (appender)
  {
    job->addLog(appender->messageByThreadID(job->runningThreadID()));
  }

  ctkJobScheduler::onJobFailed(job);
}
