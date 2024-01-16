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

#ifndef __ctkDICOMScheduler_h
#define __ctkDICOMScheduler_h

// Qt includes
#include <QObject>
#include <QMap>

// ctkCore includes
#include "ctkAbstractScheduler.h"

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMDatabase.h"

class ctkDICOMJob;
class ctkDICOMIndexer;
class ctkDICOMSchedulerPrivate;
class ctkDICOMServer;
class ctkDICOMStorageListenerJob;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMScheduler : public ctkAbstractScheduler
{
  Q_OBJECT
  Q_PROPERTY(int maximumThreadCount READ maximumThreadCount WRITE setMaximumThreadCount);
  Q_PROPERTY(int maximumNumberOfRetry READ maximumNumberOfRetry WRITE setMaximumNumberOfRetry);
  Q_PROPERTY(int retryDelay READ retryDelay WRITE setRetryDelay);
  Q_PROPERTY(int maximumPatientsQuery READ maximumPatientsQuery WRITE setMaximumPatientsQuery);

public:
  typedef ctkAbstractScheduler Superclass;
  explicit ctkDICOMScheduler(QObject* parent = 0);
  virtual ~ctkDICOMScheduler();

  /// Query Patients applying filters on all servers.
  /// The method spans a ctkDICOMQueryJob for each server.
  Q_INVOKABLE void queryPatients(QThread::Priority priority = QThread::LowPriority);

  /// Query Studies applying filters on all servers.
  /// The method spans a ctkDICOMQueryJob for each server.
  Q_INVOKABLE void queryStudies(const QString& patientID,
                                QThread::Priority priority = QThread::LowPriority);

  /// Query Series applying filters on all servers.
  /// The method spans a ctkDICOMQueryJob for each server.
  Q_INVOKABLE void querySeries(const QString& patientID,
                               const QString& studyInstanceUID,
                               QThread::Priority priority = QThread::LowPriority);

  /// Query Instances applying filters on all servers.
  /// The method spans a ctkDICOMQueryJob for each server.
  Q_INVOKABLE void queryInstances(const QString& patientID,
                                  const QString& studyInstanceUID,
                                  const QString& seriesInstanceUID,
                                  QThread::Priority priority = QThread::LowPriority);

  /// Retrieve Study.
  /// The method spans a ctkDICOMRetrieveJob for each server.
  Q_INVOKABLE void retrieveStudy(const QString& patientID,
                                 const QString& studyInstanceUID,
                                 QThread::Priority priority = QThread::LowPriority);

  /// Retrieve Series.
  /// The method spans a ctkDICOMRetrieveJob for each server.
  Q_INVOKABLE void retrieveSeries(const QString& patientID,
                                  const QString& studyInstanceUID,
                                  const QString& seriesInstanceUID,
                                  QThread::Priority priority = QThread::LowPriority);

  /// Retrieve SOPInstance.
  /// The method spans a ctkDICOMRetrieveJob for each server.
  Q_INVOKABLE void retrieveSOPInstance(const QString& patientID,
                                       const QString& studyInstanceUID,
                                       const QString& seriesInstanceUID,
                                       const QString &SOPInstanceUID,
                                       QThread::Priority priority = QThread::LowPriority);

  /// Start a storage listener
  Q_INVOKABLE void startListener(const int port,
                                 const QString &AETitle,
                                 QThread::Priority priority = QThread::LowPriority);

  ///@{
  /// Insert results from a job
  void insertJobResponseSet(QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet,
                            QThread::Priority priority = QThread::HighPriority);
  void insertJobResponseSets(QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets,
                             QThread::Priority priority = QThread::HighPriority);
  ///@}

  /// Return the Dicom Database.
  Q_INVOKABLE ctkDICOMDatabase* dicomDatabase() const;
  /// Return Dicom Database as a shared pointer
  /// (not Python-wrappable).
  QSharedPointer<ctkDICOMDatabase> dicomDatabaseShared() const;

  /// Set the Dicom Database.
  Q_INVOKABLE void setDicomDatabase(ctkDICOMDatabase& dicomDatabase);
  /// Set the Dicom Database as a shared pointer
  /// (not Python-wrappable).
  void setDicomDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase);


  ///@{
  /// Filters are keyword/value pairs as generated by
  /// the ctkDICOMWidgets in a human readable (and editable)
  /// format.  The Query is responsible for converting these
  /// into the appropriate dicom syntax for the C-Find
  /// Currently supports the keys: Name, Study, Series, ID, Modalities,
  /// StartDate and EndDate
  /// Key         DICOM Tag                Type        Example
  /// -----------------------------------------------------------
  /// Name        DCM_PatientName          QString     JOHNDOE
  /// Study       DCM_StudyDescription     QString
  /// Series      DCM_SeriesDescription    QString
  /// ID          DCM_PatientID            QString
  /// Modalities  DCM_ModalitiesInStudy    QStringList CT, MR, MN
  /// StartDate   DCM_StudyDate            QString     20090101
  /// EndDate     DCM_StudyDate            QString     20091231
  /// No filter (empty) by default.
  Q_INVOKABLE void setFilters(const QMap<QString, QVariant> &filters);
  Q_INVOKABLE QMap<QString, QVariant> filters()const;
  ///@}

  ///@{
  /// Servers
  Q_INVOKABLE int getNumberOfServers();
  Q_INVOKABLE int getNumberOfQueryRetrieveServers();
  Q_INVOKABLE int getNumberOfStorageServers();
  Q_INVOKABLE ctkDICOMServer* getNthServer(int id);
  Q_INVOKABLE ctkDICOMServer* getServer(const char* connectionName);
  Q_INVOKABLE void addServer(ctkDICOMServer& server);
  void addServer(QSharedPointer<ctkDICOMServer> server);
  Q_INVOKABLE void removeServer(const char* connectionName);
  Q_INVOKABLE void removeNthServer(int id);
  Q_INVOKABLE void removeAllServers();
  Q_INVOKABLE QString getServerNameFromIndex(int id);
  Q_INVOKABLE int getServerIndexFromName(const char* connectionName);
  ///@}

  ///@{
  /// Jobs managment
  Q_INVOKABLE int numberOfJobs();
  Q_INVOKABLE int numberOfPersistentJobs();
  Q_INVOKABLE void addJob(ctkDICOMJob* job);
  Q_INVOKABLE void deleteJob(QString jobUID);
  Q_INVOKABLE void deleteWorker(QString jobUID);
  QSharedPointer<ctkDICOMJob> getJobSharedByUID(QString jobUID);
  Q_INVOKABLE ctkDICOMJob* getJobByUID(QString jobUID);
  Q_INVOKABLE void waitForFinish();
  Q_INVOKABLE void waitForDone(int msec);
  Q_INVOKABLE void waitForFinishByUIDs(const QStringList& patientIDs = {},
                                       const QStringList& studyInstanceUIDs = {},
                                       const QStringList& seriesInstanceUIDs = {},
                                       const QStringList& sopInstanceUIDs = {});
  Q_INVOKABLE void stopAllJobs(bool stopPersistentJobs = false);
  Q_INVOKABLE void stopJobsByUIDs(const QStringList& patientIDs = {},
                                  const QStringList& studyInstanceUIDs = {},
                                  const QStringList& seriesInstanceUIDs = {},
                                  const QStringList& sopInstanceUIDs = {});
  Q_INVOKABLE void raiseJobsPriorityForSeries(const QStringList& selectedSeriesInstanceUIDs,
                                              QThread::Priority priority = QThread::HighestPriority);
  ///@}

  ///@{
  /// Maximum number of concurrent QThreads spawned by the threadPool in the Job pool
  /// default: 20
  int maximumThreadCount() const;
  void setMaximumThreadCount(const int& maximumThreadCount);
  ///@}

  ///@{
  /// Maximum number of retries that the Job pool will try on each failed Job
  /// default: 3
  int maximumNumberOfRetry() const;
  void setMaximumNumberOfRetry(const int& maximumNumberOfRetry);
  ///@}

  ///@{
  /// Retry delay in millisec
  /// default: 100 msec
  int retryDelay() const;
  void setRetryDelay(const int& retryDelay);
  ///@}

  ///@{
  /// maximum number of responses allowed in one query
  /// when query is at Patient level. Default is 25.
  void setMaximumPatientsQuery(const int maximumPatientsQuery);
  int maximumPatientsQuery();
  ///@}

  ///@{
  /// Return the listener Job.
  Q_INVOKABLE ctkDICOMStorageListenerJob* listenerJob();
  Q_INVOKABLE bool isStorageListenerActive();
  ///@}

  ///@{
  /// Return the threadPool.
  Q_INVOKABLE QThreadPool* threadPool() const;
  /// Return threadPool as a shared pointer
  /// (not Python-wrappable).
  QSharedPointer<QThreadPool> threadPoolShared() const;
  ///@}

  /// Utility method to transform/pass informations between threads by qt signals
  Q_INVOKABLE QVariant jobToDetail(ctkDICOMJob* job);

Q_SIGNALS:
  void queueJobs();
  void progressJobDetail(QVariant data);

public Q_SLOTS:
  void onJobStarted() override;
  void onJobCanceled() override;
  void onJobFailed() override;
  void onJobFinished() override;
  void onQueueJobsInThreadPool();

protected:
  QScopedPointer<ctkDICOMSchedulerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMScheduler);
  Q_DISABLE_COPY(ctkDICOMScheduler);
};

#endif
