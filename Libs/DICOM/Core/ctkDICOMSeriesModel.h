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
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

#ifndef __ctkDICOMSeriesModel_h
#define __ctkDICOMSeriesModel_h

// Qt includes
#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QStringList>
#include <QPixmap>
#include <QThread>

// CTK includes
#include "ctkDICOMCoreExport.h"

class ctkDICOMDatabase;
class ctkDICOMScheduler;
class ctkDICOMSeriesModelPrivate;
class QItemSelectionModel;

/// \ingroup DICOM_Core
///
/// \brief Model for displaying DICOM series within a study.
///
/// This model manages series data for a specific study, including:
/// - Series metadata (description, modality, number, instance count)
/// - Thumbnail generation and caching
/// - Cloud status and download progress
/// - Selection and operation states
///
/// The model supports lazy loading and asynchronous thumbnail generation
/// for optimal performance with large datasets.
///
class CTK_DICOM_CORE_EXPORT ctkDICOMSeriesModel : public QAbstractTableModel
{
  Q_OBJECT
  Q_PROPERTY(QString patientID READ patientID WRITE setPatientID NOTIFY patientIDChanged)
  Q_PROPERTY(QString studyFilter READ studyFilter WRITE setStudyFilter NOTIFY studyFilterChanged)
  Q_PROPERTY(QStringList modalityFilter READ modalityFilter WRITE setModalityFilter NOTIFY modalityFilterChanged)
  Q_PROPERTY(QString seriesDescriptionFilter READ seriesDescriptionFilter WRITE setSeriesDescriptionFilter NOTIFY seriesDescriptionFilterChanged)
  Q_PROPERTY(int thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)
  Q_PROPERTY(QStringList allowedServers READ allowedServers WRITE setAllowedServers NOTIFY allowedServersChanged)
  Q_PROPERTY(bool autoGenerateThumbnails READ autoGenerateThumbnails WRITE setAutoGenerateThumbnails NOTIFY autoGenerateThumbnailsChanged)
  Q_PROPERTY(QThread::Priority jobPriority READ jobPriority WRITE setJobPriority NOTIFY jobPriorityChanged)

public:
  typedef QAbstractTableModel Superclass;

  /// Custom data roles for series information
  enum DataRoles {
    // Basic series information
    SeriesInstanceUIDRole = Qt::UserRole + 1,   ///< Database series item ID
    SeriesNumberRole,                           ///< Series number
    ModalityRole,                               ///< Modality (CT, MR, etc.)
    SeriesDescriptionRole,                      ///< Series description

    // Instance information
    InstanceCountRole,                          ///< Number of instances in series
    InstancesLoadedRole,                        ///< Number of instances downloaded locally
    RowsRole,                                   ///< DICOM Rows (image height)
    ColumnsRole,                                ///< DICOM Columns (image width)

    // Visual data
    ThumbnailPathRole,                          ///< Path to cached thumbnail file
    ThumbnailSizeRole,                          ///< Thumbnail size as QSize
    ThumbnailGeneratedRole,                     ///< Whether thumbnail has been generated

    // Status information
    IsCloudRole,                                ///< Whether series is stored in cloud
    IsLoadedRole,                               ///< Whether series has been loaded locally
    IsVisibleRole,                              ///< Whether series is marked as visible

    // Operation status
    OperationProgressRole,                      ///< Progress of current operation
    OperationStatusRole,                        ///< Status for current operation

    // Job tracking
    JobUIDRole,                                 ///< UID of associated job

    // Internal data
    PatientIDRole,                              ///< Patient ID
    PatientNameRole,                            ///< Patient Name
    PatientBirthDateRole,                       ///< Patient birth date
    StudyInstanceUIDRole                        ///< Study Instance UID
  };
  Q_ENUM(DataRoles)

  /// Operation status for series
  enum OperationStatus {
    NoOperation,
    InProgress,
    Failed,
    Completed
  };
  Q_ENUM(OperationStatus)

  explicit ctkDICOMSeriesModel(QObject* parent = nullptr);
  virtual ~ctkDICOMSeriesModel();

  // QAbstractTableModel interface
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  virtual QHash<int, QByteArray> roleNames() const override;

  /// DICOM Database
  Q_INVOKABLE void setDicomDatabase(ctkDICOMDatabase& database);
  void setDicomDatabase(QSharedPointer<ctkDICOMDatabase> database);
  Q_INVOKABLE ctkDICOMDatabase* dicomDatabase() const;
  QSharedPointer<ctkDICOMDatabase> dicomDatabaseShared() const;

  /// DICOM Scheduler for background operations
  Q_INVOKABLE void setScheduler(ctkDICOMScheduler& scheduler);
  void setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler);
  Q_INVOKABLE ctkDICOMScheduler* scheduler() const;
  QSharedPointer<ctkDICOMScheduler> schedulerShared() const;

  /// Get/Set patient ID
  QString patientID() const;
  void setPatientID(const QString& patientID);

  /// Study filter - only series from this study will be shown
  void setStudyFilter(const QString& studyInstanceUID);
  QString studyFilter() const;

  /// Modality filter - only series with these modalities will be shown
  void setModalityFilter(const QStringList& modalities);
  QStringList modalityFilter() const;

  /// Series description filter - only series containing this text will be shown
  void setSeriesDescriptionFilter(const QString& description);
  QString seriesDescriptionFilter() const;

  /// Thumbnail size in pixels
  void setThumbnailSize(int size);
  int thumbnailSize() const;

  /// Enable/disable automatic thumbnail generation
  void setAutoGenerateThumbnails(bool enable);
  bool autoGenerateThumbnails() const;

  /// Allowed servers for query/retrieve operations
  void setAllowedServers(const QStringList& servers);
  QStringList allowedServers() const;

  /// Set job priority for QueryInstances and Retrieve operations (default: NormalPriority)
  /// Use HighPriority for studies that should be opened immediately
  void setJobPriority(QThread::Priority priority);
  QThread::Priority jobPriority() const;

  /// Get series instance UID for model index
  Q_INVOKABLE QString seriesInstanceUID(const QModelIndex& index) const;

  /// Find model index for series instance UID
  Q_INVOKABLE QModelIndex indexForSeriesInstanceUID(const QString& seriesInstanceUID) const;

  // Get list of series instance UIDs
  Q_INVOKABLE QStringList seriesInstanceUIDs() const;

  /// Get list of filtered series instance UIDs (after applying all filters)
  Q_INVOKABLE QStringList filteredSeriesInstanceUIDs() const;

  /// Refresh the model data
  Q_INVOKABLE void refresh();

  /// Force update of specific series
  Q_INVOKABLE void refreshSeriesList();
  Q_INVOKABLE void refreshSeries(const QString& seriesInstanceUID);

  /// Clean the model data
  Q_INVOKABLE void clean();

  /// Generate thumbnails for all visible series
  Q_INVOKABLE void generateThumbnails(bool regenerate = false);

  /// Generate thumbnail for specific series
  Q_INVOKABLE void generateThumbnail(const QModelIndex& index);

  /// Retrieve all series from the DICOM server
  Q_INVOKABLE void retrieveAllSeries();

  /// Force query stop/retry of specific seriesInstanceUID
  Q_INVOKABLE bool isSeriesCloud(const QString& seriesInstanceUID);
  Q_INVOKABLE void forceUpdateSeriesJobs(const QString& seriesInstanceUID);
  Q_INVOKABLE void forceRetrieveSeries(const QString& seriesInstanceUID);

public slots:
  /// Update GUI from scheduler progress
  void updateGUIFromScheduler(const QVariant&, const bool&);
  void onJobStarted(const QVariant& data);
  void onJobFinished(const QVariant& data);
  void onJobFailed(const QVariant& data);
  void onJobUserStopped(const QVariant& data);
  void onThumbnailGenerated(const QString& seriesInstanceUID, const QString& thumbnailPath);
  void onLoadedSeriesChanged(const QStringList& seriesInstanceUIDs);

signals:
  /// Emitted when patient ID changes
  void patientIDChanged(const QString& patientID);

  /// Emitted when study filter changes
  void studyFilterChanged(const QString& studyInstanceUID);

  /// Emitted when modality filter changes
  void modalityFilterChanged(const QStringList& modalities);

  /// Emitted when series description filter changes
  void seriesDescriptionFilterChanged(const QString& description);

  /// Emitted when thumbnail size changes
  void thumbnailSizeChanged(int size);

  /// Emitted when auto-generate thumbnails setting changes
  void autoGenerateThumbnailsChanged(bool enable);

  /// Emitted when allowed servers change
  void allowedServersChanged(const QStringList& servers);

  /// Emitted when job priority changes
  void jobPriorityChanged(QThread::Priority priority);

  /// Emitted when series selection changes
  void seriesSelectionChanged(const QStringList& selectedSeriesInstanceUIDs);

  /// Emitted when thumbnail is ready
  void thumbnailReady(const QModelIndex& index);

  /// Emitted when operation progress changes
  void operationProgressChanged(const QModelIndex& index, int progress);

  /// Emitted when all data is loaded and ready
  void modelRefreshed();

protected:
  QScopedPointer<ctkDICOMSeriesModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMSeriesModel);
  Q_DISABLE_COPY(ctkDICOMSeriesModel);
};

#endif
