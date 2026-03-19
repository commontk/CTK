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

#ifndef __ctkDICOMPatientModel_h
#define __ctkDICOMPatientModel_h

// Qt includes
#include <QAbstractListModel>
#include <QSharedPointer>
#include <QStringList>

// CTK includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMStudyFilterProxyModel.h"

class ctkDICOMDatabase;
class ctkDICOMScheduler;
class ctkDICOMPatientModelPrivate;

/// \ingroup DICOM_Core
///
/// \brief Model for displaying DICOM patients.
///
/// This model manages patient data from the DICOM database, including:
/// - Patient metadata (ID, name, birth date, sex)
/// - Study count per patient
/// - Selection and operation states
///
/// The model supports filtering by patient ID and name, and manages
/// study models on-demand when a patient is selected/expanded.
///
/// \endcode
class CTK_DICOM_CORE_EXPORT ctkDICOMPatientModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(QString patientIDFilter READ patientIDFilter WRITE setPatientIDFilter NOTIFY patientIDFilterChanged)
  Q_PROPERTY(QString patientNameFilter READ patientNameFilter WRITE setPatientNameFilter NOTIFY patientNameFilterChanged)
  Q_PROPERTY(QString studyDescriptionFilter READ studyDescriptionFilter WRITE setStudyDescriptionFilter NOTIFY studyDescriptionFilterChanged)
  Q_PROPERTY(DateType dateFilter READ dateFilter WRITE setDateFilter NOTIFY dateFilterChanged)
  Q_PROPERTY(QString seriesDescriptionFilter READ seriesDescriptionFilter WRITE setSeriesDescriptionFilter NOTIFY seriesDescriptionFilterChanged)
  Q_PROPERTY(QStringList modalityFilter READ modalityFilter WRITE setModalityFilter NOTIFY modalityFilterChanged)
  Q_PROPERTY(int numberOfOpenedStudiesPerPatient READ numberOfOpenedStudiesPerPatient WRITE setNumberOfOpenedStudiesPerPatient NOTIFY numberOfOpenedStudiesPerPatientChanged)
  Q_PROPERTY(int thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)
  Q_PROPERTY(bool queryInProgress READ queryInProgress WRITE setQueryInProgress NOTIFY queryInProgressChanged)

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

  /// Operation status for patients
  enum OperationStatus
  {
    NoOperation = 0,
    InProgress,
    Completed,
    Failed,
  };
  Q_ENUM(OperationStatus)

  /// Custom data roles for patient information
  enum DataRoles {
    // Basic patient information
    PatientUIDRole = Qt::UserRole + 1,  ///< Internal database patient item key
    PatientIDRole,                        ///< Patient ID (DICOM tag 0010,0020)
    PatientNameRole,                      ///< Patient Name (DICOM tag 0010,0010)
    PatientBirthDateRole,                 ///< Patient Birth Date (DICOM tag 0010,0030)
    PatientSexRole,                       ///< Patient Sex (DICOM tag 0010,0040)
    PatientInsertDateTimeRole,            ///< Date and time when patient was added to database

    // Study information
    StudyCountRole,                       ///< Total number of studies for this patient
    FilteredStudyCountRole,               ///< Number of studies after filtering
    SeriesCountRole,                      ///< Total number of series across all studies
    FilteredSeriesCountRole,              ///< Number of series after filtering across all studies

    // Visual state
    IsVisibleRole,                        ///< Whether patient is visible (based on filters)

    // Status information
    OperationStatusRole,                  ///< Current operation status
    AllowedServersRole,                   ///< List of allowed servers for this patient
    StoppedJobUIDRole,                    ///< UID of last stopped job
  };

  explicit ctkDICOMPatientModel(QObject* parent = nullptr);
  virtual ~ctkDICOMPatientModel();

  /// \name QAbstractListModel interface
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  virtual QHash<int, QByteArray> roleNames() const override;

  /// \name Database and Scheduler
  /// Get/Set the DICOM database
  Q_INVOKABLE ctkDICOMDatabase* dicomDatabase() const;
  QSharedPointer<ctkDICOMDatabase> dicomDatabaseShared() const;
  Q_INVOKABLE void setDicomDatabase(ctkDICOMDatabase& database);
  void setDicomDatabase(QSharedPointer<ctkDICOMDatabase> database);

  /// Get/Set the DICOM scheduler
  Q_INVOKABLE ctkDICOMScheduler* scheduler() const;
  QSharedPointer<ctkDICOMScheduler> schedulerShared() const;
  Q_INVOKABLE void setScheduler(ctkDICOMScheduler& scheduler);
  void setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler);

  /// \name Filtering
  /// Get/Set patient ID filter (filters patients by Patient ID)
  QString patientIDFilter() const;
  void setPatientIDFilter(const QString& patientID);

  /// Get/Set patient name filter (filters patients by Patient Name)
  QString patientNameFilter() const;
  void setPatientNameFilter(const QString& patientName);

  /// Get/Set study description filter (propagated to all study models)
  QString studyDescriptionFilter() const;
  void setStudyDescriptionFilter(const QString& description);

  /// Get/Set series description filter (propagated to all study models)
  QString seriesDescriptionFilter() const;
  void setSeriesDescriptionFilter(const QString& description);

  /// Get/Set date filter (propagated to all study models)
  DateType dateFilter() const;
  void setDateFilter(DateType dateType);

  /// Set custom date range filter (when dateFilter is CustomRange)
  void setCustomDateRange(const QDate& startDate, const QDate& endDate);
  QDate customStartDate() const;
  QDate customEndDate() const;

  /// Get/Set modality filter (propagated to all study models)
  QStringList modalityFilter() const;
  void setModalityFilter(const QStringList& modalities);

  /// Get/Set allowed servers
  Q_INVOKABLE QStringList allowedServers(const QString& patientUID) const;
  Q_INVOKABLE void setAllowedServers(const QString& patientUID, const QStringList& servers);

  /// \name Patient Management
  /// Get/Set number of studies that should be opened by default per patient
  int numberOfOpenedStudiesPerPatient() const;
  void setNumberOfOpenedStudiesPerPatient(int count);

  /// Get/Set thumbnail size for series in pixels
  int thumbnailSize() const;
  void setThumbnailSize(int size);

  /// Check if a query operation is currently in progress
  bool queryInProgress() const;
  void setQueryInProgress(bool inProgress);

  /// Get patient item at index (internal database key)
  Q_INVOKABLE QString patientUID(const QModelIndex& index) const;
  Q_INVOKABLE QString patientID(const QModelIndex& index) const;
  Q_INVOKABLE QString patientName(const QModelIndex& index) const;

  /// Find index by patient item (internal database key)
  Q_INVOKABLE QModelIndex indexFromPatientUID(const QString& patientUID) const;

  /// Find index by patient ID
  Q_INVOKABLE QModelIndex indexFromPatientID(const QString& patientID) const;

  /// get patient visibility
  Q_INVOKABLE bool isPatientVisible(const QModelIndex& index) const;

  /// \name Study Model Management
  /// Get study model for a patient (returns nullptr if not created yet)
  /// Note: This does NOT automatically create the model. Use refresh() to create the study models.
  Q_INVOKABLE ctkDICOMStudyModel* studyModelForPatient(const QModelIndex& patientIndex);
  Q_INVOKABLE ctkDICOMStudyModel* studyModelForPatientUID(const QString& patientUID);
  Q_INVOKABLE ctkDICOMStudyModel* studyModelForPatientID(const QString& patientID);
  Q_INVOKABLE ctkDICOMStudyFilterProxyModel* studyFilterProxyModelForPatient(const QModelIndex& patientIndex);
  Q_INVOKABLE ctkDICOMStudyFilterProxyModel* studyFilterProxyModelForPatientUID(const QString& patientUID);
  Q_INVOKABLE ctkDICOMStudyFilterProxyModel* studyFilterProxyModelForPatientID(const QString& patientID);

  /// Check if study model exists for patient
  Q_INVOKABLE bool hasStudyModel(const QModelIndex& patientIndex) const;
  Q_INVOKABLE bool hasStudyModel(const QString& patientUID) const;

  /// Remove study model for patient (when no longer needed)
  Q_INVOKABLE void removeStudyModel(const QModelIndex& patientIndex);
  Q_INVOKABLE void removeStudyModel(const QString& patientUID);

  /// Get list of all study models (for cleanup/management)
  Q_INVOKABLE QList<ctkDICOMStudyModel*> allStudyModels() const;

  /// Get list of all study filter proxy models (for cleanup/management)
  Q_INVOKABLE QList<ctkDICOMStudyFilterProxyModel*> allStudyFilterProxyModels() const;

  /// Update all study/series models
  Q_INVOKABLE void updateStudyModelsFilters();
  Q_INVOKABLE void refreshStudyModels();
  Q_INVOKABLE void refreshSeriesModels();

  /// \name Utility
  /// Check if patient matches current filters
  Q_INVOKABLE bool patientMatchesFilters(const QString& patientUID) const;

  /// Get list of filtered patient (after applying all filters)
  Q_INVOKABLE QStringList filteredPatientUIDs() const;
  Q_INVOKABLE int filteredStudiesCountForPatient(const QString& patientUID) const;
  Q_INVOKABLE int filteredSeriesCountForPatient(const QString& patientUID) const;

  /// Update model from database
  Q_INVOKABLE void refresh();
  Q_INVOKABLE void clean();

  /// Force update patient
  Q_INVOKABLE void refreshPatients();
  Q_INVOKABLE void refreshPatient(const QString& patientUID);

  /// Query studies
  Q_INVOKABLE bool queryStudies(const QString& patientID);

  /// Update allowed servers for a patient from database
  Q_INVOKABLE void updateAllowedServersFromDB(const QString& patientUID);

  /// Update allowed servers for a patient and save to database
  Q_INVOKABLE void saveAllowedServersToDB(const QString& patientUID, const QStringList& allowedServers);

public Q_SLOTS:
  /// Handle scheduler updates
  void updateGUIFromScheduler(const QVariant&);
  void onJobStarted(const QVariant&);
  void onJobUserStopped(const QVariant&);
  void onJobFailed(const QVariant&);
  void onJobFinished(const QVariant&);

Q_SIGNALS:
  /// Emitted when filters change
  void patientIDFilterChanged(const QString& patientID);
  void patientNameFilterChanged(const QString& patientName);
  void studyDescriptionFilterChanged(const QString& description);
  void seriesDescriptionFilterChanged(const QString& description);
  void dateFilterChanged(DateType dateType);
  void modalityFilterChanged(const QStringList& modalities);
  void allowedServersChanged(const QString& patientUID, const QStringList& servers);
  void numberOfOpenedStudiesPerPatientChanged(int count);
  void thumbnailSizeChanged(int size);
  void queryInProgressChanged(bool inProgress);

  /// Emitted when study models are created/removed
  void studyModelCreated(const QString& patientUID, ctkDICOMStudyModel* studyModel);
  void studyModelRemoved(const QString& patientUID);

  /// Emitted when model is refreshed
  void modelRefreshed();

protected:
  QScopedPointer<ctkDICOMPatientModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMPatientModel);
  Q_DISABLE_COPY(ctkDICOMPatientModel);
};

#endif
