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

#ifndef __ctkDICOMStudyModel_h
#define __ctkDICOMStudyModel_h

// Qt includes
#include <QAbstractListModel>
#include <QSharedPointer>
#include <QStringList>
#include <QDate>

// CTK includes
#include "ctkDICOMCoreExport.h"

class ctkDICOMDatabase;
class ctkDICOMScheduler;
class ctkDICOMSeriesModel;
class ctkDICOMSeriesFilterProxyModel;
class ctkDICOMStudyModelPrivate;

/// \ingroup DICOM_Core
///
/// \brief Model for displaying DICOM studies within a patient.
///
/// This model manages study data for a specific patient, including:
/// - Study metadata (description, date, modalities)
/// - Collapsible state management
/// - Series count and filtering
/// - Selection and operation states
///
class CTK_DICOM_CORE_EXPORT ctkDICOMStudyModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(QString patientUID READ patientUID WRITE setPatientUID NOTIFY patientUIDChanged)
  Q_PROPERTY(QString patientID READ patientID WRITE setPatientID NOTIFY patientIDChanged)
  Q_PROPERTY(QString studyDescriptionFilter READ studyDescriptionFilter WRITE setStudyDescriptionFilter NOTIFY studyDescriptionFilterChanged)
  Q_PROPERTY(DateType dateFilter READ dateFilter WRITE setDateFilter NOTIFY dateFilterChanged)
  Q_PROPERTY(QStringList modalityFilter READ modalityFilter WRITE setModalityFilter NOTIFY modalityFilterChanged)
  Q_PROPERTY(QString seriesDescriptionFilter READ seriesDescriptionFilter WRITE setSeriesDescriptionFilter NOTIFY seriesDescriptionFilterChanged)
  Q_PROPERTY(QStringList allowedServers READ allowedServers WRITE setAllowedServers NOTIFY allowedServersChanged)
  Q_PROPERTY(int numberOfOpenedStudies READ numberOfOpenedStudies WRITE setNumberOfOpenedStudies NOTIFY numberOfOpenedStudiesChanged)
  Q_PROPERTY(int thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)

public:
  typedef QAbstractListModel Superclass;

  /// Date filtering options
  enum DateType
  {
    Any = 0,
    Today,
    Yesterday,
    LastWeek,
    LastMonth,
    LastYear,
    CustomRange
  };
  Q_ENUM(DateType)

  /// Operation status for studies
  enum OperationStatus
  {
    NoOperation = 0,
    InProgress,
    Completed,
    Failed,
  };
  Q_ENUM(OperationStatus)

  /// Custom data roles for study information
  enum DataRoles {
    // Basic study information
    StudyInstanceUIDRole = Qt::UserRole + 1,
    StudyIDRole,                    ///< Study ID
    StudyDescriptionRole,           ///< Study description
    StudyDateRole,                  ///< Study date
    StudyTimeRole,                  ///< Study time
    AccessionNumberRole,            ///< Accession number
    ModalitiesInStudyRole,          ///< List of modalities in study

    // Series information
    SeriesCountRole,                ///< Total number of series in study
    FilteredSeriesCountRole,        ///< Number of series after filtering

    // Visual state
    IsCollapsedRole,                ///< Whether study is collapsed
    IsVisibleRole,                  ///< Whether study is visible (based on filters)

    // Status information
    OperationStatusRole,            ///< Current operation status
    StoppedJobUIDRole,              ///< UID of last stopped job

    // Patient information (for convenience)
    PatientUIDRole,                ///< Patient item
    PatientIDRole,                  ///< Patient ID
    PatientNameRole,                ///< Patient name
    PatientBirthDateRole,           ///< Patient birth date
  };

  explicit ctkDICOMStudyModel(QObject* parent = nullptr);
  virtual ~ctkDICOMStudyModel();

  /// \name QAbstractListModel interface
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
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

  /// \name Filtering
  /// Get/Set patientUID
  QString patientUID() const;
  void setPatientUID(const QString& patientUID);

  /// Get/Set patient ID
  QString patientID() const;
  void setPatientID(const QString& patientID);

  /// Get/Set study description filter (filters studies by StudyDescription)
  QString studyDescriptionFilter() const;
  void setStudyDescriptionFilter(const QString& description);

  /// Get/Set date filter
  DateType dateFilter() const;
  void setDateFilter(DateType dateType);

  /// Set custom date range filter (when dateFilter is CustomRange)
  void setCustomDateRange(const QDate& startDate, const QDate& endDate);
  QDate customStartDate() const;
  QDate customEndDate() const;

  /// Get/Set modality filter for series within studies
  QStringList modalityFilter() const;
  void setModalityFilter(const QStringList& modalities);

  /// Get/Set series description filter (propagated to all series models)
  QString seriesDescriptionFilter() const;
  void setSeriesDescriptionFilter(const QString& description);

  /// Get/Set allowed servers
  QStringList allowedServers() const;
  void setAllowedServers(const QStringList& servers);

  /// \name Study Management
  /// Get/Set number of studies that should be opened by default
  int numberOfOpenedStudies() const;
  void setNumberOfOpenedStudies(int count);

  /// Get/Set thumbnail size for series models in pixels
  int thumbnailSize() const;
  void setThumbnailSize(int size);

  /// Get study item at index
  Q_INVOKABLE QString studyInstanceUID(const QModelIndex& index) const;
  Q_INVOKABLE QString studyID(const QModelIndex& index) const;

  /// Find index by study instance UID
  Q_INVOKABLE QModelIndex indexFromStudyInstanceUID(const QString& studyInstanceUID) const;

  /// Collapse/expand study
  Q_INVOKABLE void setAllStudiesCollapsed(bool collapsed);
  Q_INVOKABLE void setStudyCollapsed(const QModelIndex& index, bool collapsed);
  Q_INVOKABLE bool isStudyCollapsed(const QModelIndex& index) const;

  /// \name Series Model Management
  /// Get series model for a study (returns nullptr if not created yet)
  /// Note: This does NOT automatically create the model. Use refresh() to create the series models.
  Q_INVOKABLE ctkDICOMSeriesModel* seriesModelForStudy(const QModelIndex& studyIndex);
  Q_INVOKABLE ctkDICOMSeriesModel* seriesModelForStudyInstanceUID(const QString& studyInstanceUID);
  Q_INVOKABLE ctkDICOMSeriesFilterProxyModel* seriesFilterProxyModelForStudy(const QModelIndex& studyIndex);
  Q_INVOKABLE ctkDICOMSeriesFilterProxyModel* seriesFilterProxyModelForStudyInstanceUID(const QString& studyInstanceUID);

  /// Check if series model exists for study
  Q_INVOKABLE bool hasSeriesModel(const QModelIndex& studyIndex) const;
  Q_INVOKABLE bool hasSeriesModel(const QString& studyInstanceUID) const;

  /// Remove series model for study
  Q_INVOKABLE void removeSeriesModel(const QModelIndex& studyIndex);
  Q_INVOKABLE void removeSeriesModel(const QString& studyInstanceUID);

  /// Get list of all series models
  Q_INVOKABLE QList<ctkDICOMSeriesModel*> allSeriesModels() const;
  Q_INVOKABLE QList<ctkDICOMSeriesFilterProxyModel*> allSeriesFilterProxyModels() const;

  /// Update all series models with new filters
  Q_INVOKABLE void updateSeriesModelsFilters();
  Q_INVOKABLE void refreshSeriesModels();

  /// \name Utility
  /// Convert date filter to number of days
  Q_INVOKABLE static int daysFromDateFilter(DateType dateFilter);

  /// Check if study matches current filters
  Q_INVOKABLE bool studyMatchesFilters(const QString& studyInstanceUID) const;

  /// Get list of study instance UIDs
  Q_INVOKABLE QStringList studyInstanceUIDs() const;

  /// Get list of filtered study instance UIDs (after applying all filters)
  Q_INVOKABLE QStringList filteredStudyInstanceUIDs(bool filterSeries = true) const;

  /// Update model from database
  Q_INVOKABLE void refresh();

  /// Clean the model and all child series models
  Q_INVOKABLE void clean();

  /// Force update of specific study
  Q_INVOKABLE void refreshStudies();
  Q_INVOKABLE void refreshStudy(const QString& studyInstanceUID);

  /// Force query stop/retry of specific studyInstanceUID
  Q_INVOKABLE void forceUpdateStudyJobs(const QString& studyInstanceUID);

public Q_SLOTS:
  /// Handle scheduler updates
  void updateGUIFromScheduler(const QVariant&);
  void onJobStarted(const QVariant&);
  void onJobUserStopped(const QVariant&);
  void onJobFailed(const QVariant&);
  void onJobFinished(const QVariant&);

Q_SIGNALS:
  /// Emitted when filters change
  void patientUIDChanged(const QString& patientUID);
  void patientIDChanged(const QString& patientID);
  void studyDescriptionFilterChanged(const QString& description);
  void dateFilterChanged(DateType dateType);
  void modalityFilterChanged(const QStringList& modalities);
  void seriesDescriptionFilterChanged(const QString& description);
  void allowedServersChanged(const QStringList& servers);
  void numberOfOpenedStudiesChanged(int count);
  void thumbnailSizeChanged(int size);

  /// Emitted when study state changes
  void studyCollapsedChanged(const QModelIndex& index, bool collapsed);
  void studySelectedChanged(const QModelIndex& index, bool selected);

  /// Emitted when series models are created/removed
  void seriesModelCreated(const QString& studyInstanceUID, ctkDICOMSeriesModel* seriesModel);
  void seriesModelRemoved(const QString& studyInstanceUID);

  /// Emitted when model is refreshed
  void modelRefreshed();

protected:
  QScopedPointer<ctkDICOMStudyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMStudyModel);
  Q_DISABLE_COPY(ctkDICOMStudyModel);
};

#endif
