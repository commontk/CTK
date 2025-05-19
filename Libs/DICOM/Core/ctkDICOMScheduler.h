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
#include <ctkJobScheduler.h>
#include <ctkAbstractJob.h>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMDatabase.h"
class ctkDICOMJob;
class ctkDICOMIndexer;
class ctkDICOMSchedulerPrivate;
class ctkDICOMServer;
class ctkDICOMStorageListenerJob;
struct ctkDICOMJobDetail;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMScheduler : public ctkJobScheduler
{
  Q_OBJECT
  Q_PROPERTY(int maximumPatientsQuery READ maximumPatientsQuery WRITE setMaximumPatientsQuery);

public:
  typedef ctkJobScheduler Superclass;
  explicit ctkDICOMScheduler(QObject* parent = 0);
  virtual ~ctkDICOMScheduler();

  /// Query Patients applying filters on all servers.
  /// The method spans a ctkDICOMQueryJob for each server.
  Q_INVOKABLE void queryPatients(QThread::Priority priority = QThread::LowPriority);

  /// Query Studies applying filters on all servers.
  /// The method spans a ctkDICOMQueryJob for each server.
  Q_INVOKABLE void queryStudies(const QString& patientID,
                                QThread::Priority priority = QThread::LowPriority,
                                const QStringList& allowedSeversForPatient = QStringList());

  /// Query Series applying filters on all servers.
  /// The method spans a ctkDICOMQueryJob for each server.
  Q_INVOKABLE void querySeries(const QString& patientID,
                               const QString& studyInstanceUID,
                               QThread::Priority priority = QThread::LowPriority,
                               const QStringList& allowedSeversForPatient = QStringList());

  /// Query Instances applying filters on all servers.
  /// The method spans a ctkDICOMQueryJob for each server.
  Q_INVOKABLE void queryInstances(const QString& patientID,
                                  const QString& studyInstanceUID,
                                  const QString& seriesInstanceUID,
                                  QThread::Priority priority = QThread::LowPriority,
                                  const QStringList& allowedSeversForPatient = QStringList());

  /// Retrieve Study.
  /// The method spans a ctkDICOMRetrieveJob for each server.
  Q_INVOKABLE void retrieveStudy(const QString& patientID,
                                 const QString& studyInstanceUID,
                                 QThread::Priority priority = QThread::LowPriority,
                                 const QStringList& allowedSeversForPatient = QStringList());

  /// Retrieve Series.
  /// The method spans a ctkDICOMRetrieveJob for each server.
  Q_INVOKABLE void retrieveSeries(const QString& patientID,
                                  const QString& studyInstanceUID,
                                  const QString& seriesInstanceUID,
                                  QThread::Priority priority = QThread::LowPriority,
                                  const QStringList& allowedSeversForPatient = QStringList());

  /// Retrieve SOPInstance.
  /// The method spans a ctkDICOMRetrieveJob for each server.
  Q_INVOKABLE void retrieveSOPInstance(const QString& patientID,
                                       const QString& studyInstanceUID,
                                       const QString& seriesInstanceUID,
                                       const QString& SOPInstanceUID,
                                       QThread::Priority priority = QThread::LowPriority,
                                       const QStringList& allowedSeversForPatient = QStringList());

  /// Start a storage listener
  Q_INVOKABLE void startListener(int port,
                                 const QString &AETitle,
                                 QThread::Priority priority = QThread::LowPriority);

  /// Echo a server
  Q_INVOKABLE void echo(const QString& connectionName,
                        QThread::Priority priority = QThread::LowPriority);
  Q_INVOKABLE void echo(ctkDICOMServer& server,
                        QThread::Priority priority = QThread::LowPriority);

  /// Generate thumbnail and save it as png on local disk
  Q_INVOKABLE void generateThumbnail(const QString &originalFilePath,
                                     const QString &patientID,
                                     const QString &studyInstanceUID,
                                     const QString &seriesInstanceUID,
                                     const QString &sopInstanceUID,
                                     const QString& modality,
                                     QColor backgroundColor,
                                     QThread::Priority priority = QThread::HighPriority);

  ///@{
  /// Insert results from a job
  QString insertJobResponseSet(const QSharedPointer<ctkDICOMJobResponseSet>& jobResponseSet,
                               QThread::Priority priority = QThread::HighPriority);
  QString insertJobResponseSets(const QList<QSharedPointer<ctkDICOMJobResponseSet>>& jobResponseSets,
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
  Q_INVOKABLE void setFilters(const QMap<QString, QVariant>& filters);
  Q_INVOKABLE QMap<QString, QVariant> filters() const;
  ///@}

  ///@{
  /// Servers
  Q_INVOKABLE int serversCount();
  Q_INVOKABLE int queryRetrieveServersCount();
  Q_INVOKABLE int storageServersCount();
  Q_INVOKABLE ctkDICOMServer* server(int id);
  Q_INVOKABLE ctkDICOMServer* server(const QString& connectionName);
  Q_INVOKABLE void addServer(ctkDICOMServer& server);
  void addServer(QSharedPointer<ctkDICOMServer> server);
  Q_INVOKABLE void removeServer(const QString& connectionName);
  Q_INVOKABLE void removeServer(int id);
  Q_INVOKABLE void removeAllServers();
  Q_INVOKABLE QString getServerNameFromIndex(int id);
  Q_INVOKABLE int getServerIndexFromName(const QString& connectionName);
  Q_INVOKABLE QStringList getAllServersConnectionNames();
  Q_INVOKABLE QStringList getConnectionNamesForActiveServers();
  ///@}

  ///@{
  /// Jobs managment
  Q_INVOKABLE void waitForFinishByDICOMUIDs(const QStringList& patientIDs = {},
                                            const QStringList& studyInstanceUIDs = {},
                                            const QStringList& seriesInstanceUIDs = {},
                                            const QStringList& sopInstanceUIDs = {});
  Q_INVOKABLE void stopJobsByDICOMUIDs(const QStringList& patientIDs = {},
                                       const QStringList& studyInstanceUIDs = {},
                                       const QStringList& seriesInstanceUIDs = {},
                                       const QStringList& sopInstanceUIDs = {});
  QList<QSharedPointer<ctkAbstractJob>> getJobsByDICOMUIDs(const QStringList& patientIDs = {},
                                                           const QStringList& studyInstanceUIDs = {},
                                                           const QStringList& seriesInstanceUIDs = {},
                                                           const QStringList& sopInstanceUIDs = {},
                                                           QList<ctkAbstractJob::JobStatus> statusFilters =
                                                           {
                                                             ctkAbstractJob::JobStatus::Initialized,
                                                             ctkAbstractJob::JobStatus::Queued,
                                                             ctkAbstractJob::JobStatus::Running
                                                           });
  Q_INVOKABLE void raiseJobsPriorityForSeries(const QStringList& selectedSeriesInstanceUIDs,
                                              QThread::Priority priority = QThread::HighestPriority);
  ///@}

  ///@{
  /// maximum number of responses allowed in one query
  /// when query is at Patient level. Default is 25.
  void setMaximumPatientsQuery(int maximumPatientsQuery);
  int maximumPatientsQuery();
  ///@}

  ///@{
  /// Return the listener Job.
  Q_INVOKABLE ctkDICOMStorageListenerJob* listenerJob();
  Q_INVOKABLE bool isStorageListenerActive();
  ///@}

public Q_SLOTS:
  virtual void onJobStarted(ctkAbstractJob* job);
  virtual void onJobUserStopped(ctkAbstractJob* job);
  virtual void onJobFinished(ctkAbstractJob* job);
  virtual void onJobAttemptFailed(ctkAbstractJob* job);
  virtual void onJobFailed(ctkAbstractJob* job);

Q_SIGNALS:
  /// Emitted when a server is modified
  void serverModified(const QString&);

protected:
  ctkDICOMScheduler(ctkDICOMSchedulerPrivate* pimpl, QObject* parent);

private:
  Q_DECLARE_PRIVATE(ctkDICOMScheduler);
  Q_DISABLE_COPY(ctkDICOMScheduler);
};

#endif
