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

// Qt includes
#include <QDebug>
#include <QDate>
#include <QStringList>

// CTK includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModalities.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMStudyFilterProxyModel.h"
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMJob.h"

static ctkLogger logger("org.commontk.DICOM.Core.ctkDICOMPatientModel");

//------------------------------------------------------------------------------
// Helper function for setDicomDatabase/setScheduler
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//------------------------------------------------------------------------------
// ctkDICOMPatientModelPrivate

class ctkDICOMPatientModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMPatientModel);

protected:
  ctkDICOMPatientModel* const q_ptr;

public:
  ctkDICOMPatientModelPrivate(ctkDICOMPatientModel& obj);
  ~ctkDICOMPatientModelPrivate();

  void init();
  void populatePatients();
  void clean();
  bool matchesPatientIDFilter(const QString& patientID) const;
  bool matchesPatientNameFilter(const QString& patientName) const;
  int getStudyCountForPatient(const QString& patientUID) const;
  int getFilteredStudyCountForPatient(const QString& patientUID, bool filterSeries = true) const;
  int getSeriesCountForPatient(const QString& patientUID) const;
  int getFilteredSeriesCountForPatient(const QString& patientUID) const;
  void updatePatientCountsAndVisibility(int patientIndex);

  // Data storage
  struct PatientData {
    QString patientUID; // Internal database key
    QString patientID;
    QString patientName;
    QString patientBirthDate;
    QString patientSex;
    QDateTime insertDateTime;
    int studyCount;
    int filteredStudyCount;
    int seriesCount;
    int filteredSeriesCount;
    bool isVisible;
    bool isQueryResult; // Set to true when patient is from a query result
    ctkDICOMPatientModel::OperationStatus operationStatus;
    QStringList allowedServers;
    QString stoppedJobUID;
  };

  QList<PatientData> Patients;
  QHash<QString, int> PatientUIDToIndex; // For fast lookup
  QHash<QString, int> PatientIDToIndex;   // For fast lookup by patient ID

  // Study model management
  QHash<QString, ctkDICOMStudyModel*> StudyModels; // patientUID -> StudyModel
  QHash<QString, ctkDICOMStudyFilterProxyModel*> StudyFilterProxyModels; // patientUID -> StudyFilterProxyModel
  ctkDICOMStudyModel* createStudyModel(const QString& patientUID, const QString& patientID);
  void updateStudyModelFilters(ctkDICOMStudyModel* studyModel);

  // Database and scheduler
  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<ctkDICOMScheduler> Scheduler;

  // Filters
  QString PatientIDFilter;
  QString PatientNameFilter;
  QString StudyDescriptionFilter;
  QString SeriesDescriptionFilter;
  ctkDICOMPatientModel::DateType DateFilter;
  QDate CustomStartDate;
  QDate CustomEndDate;
  QStringList ModalityFilter;

  // Configuration
  int NumberOfOpenedStudiesPerPatient;
  int ThumbnailSize;

  // State
  bool IsUpdating;
  bool QueryInProgress;
};

//------------------------------------------------------------------------------
ctkDICOMPatientModelPrivate::ctkDICOMPatientModelPrivate(ctkDICOMPatientModel& obj)
  : q_ptr(&obj)
{
  this->PatientIDFilter = "";
  this->PatientNameFilter = "";
  this->StudyDescriptionFilter = "";
  this->SeriesDescriptionFilter = "";
  this->DateFilter = ctkDICOMPatientModel::DateType::Any;
  this->NumberOfOpenedStudiesPerPatient = 2;
  this->ThumbnailSize = 128;
  this->IsUpdating = false;
  this->QueryInProgress = false;

  this->ModalityFilter = ctkDICOMModalities::AllModalities;
}

//------------------------------------------------------------------------------
ctkDICOMPatientModelPrivate::~ctkDICOMPatientModelPrivate()
{

}

//------------------------------------------------------------------------------
void ctkDICOMPatientModelPrivate::init()
{
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModelPrivate::clean()
{
  Q_Q(ctkDICOMPatientModel);
  this->IsUpdating = true;

  // Clean up all study models first (which will clean series models)
  foreach (ctkDICOMStudyModel* studyModel, this->StudyModels.values())
  {
    if (studyModel)
    {
      studyModel->clean();
    }
  }

  // Delete study models and proxy models
  qDeleteAll(this->StudyModels.values());
  this->StudyModels.clear();
  qDeleteAll(this->StudyFilterProxyModels.values());
  this->StudyFilterProxyModels.clear();

  q->beginResetModel();
  this->Patients.clear();
  this->PatientUIDToIndex.clear();
  this->PatientIDToIndex.clear();
  q->endResetModel();
  this->IsUpdating = false;
  emit q->modelRefreshed();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModelPrivate::populatePatients()
{
  Q_Q(ctkDICOMPatientModel);

  if (!this->DicomDatabase)
  {
    logger.error("populatePatients: No database set");
    return;
  }

  if (this->IsUpdating)
  {
    return;
  }

  this->IsUpdating = true;

  // Get all patients from database
  QStringList patientUIDs = this->DicomDatabase->patients();
  if (patientUIDs.isEmpty())
  {
    // If no patients found, clear the model if it has data
    if (!this->Patients.isEmpty())
    {
      q->beginResetModel();
      this->Patients.clear();
      this->PatientUIDToIndex.clear();
      this->PatientIDToIndex.clear();
      q->endResetModel();
    }
    this->IsUpdating = false;
    emit q->modelRefreshed();
    return;
  }

  // Create a set of existing patient items for quick lookup
  QSet<QString> existingPatientUIDs;
  for (const PatientData& patient : this->Patients)
  {
    existingPatientUIDs.insert(patient.patientUID);
  }

  // Create a set of database patient items
  QSet<QString> validPatientUIDs = QSet<QString>(patientUIDs.begin(), patientUIDs.end());
  QList<PatientData> newPatients;

  foreach (const QString& patientUID, patientUIDs)
  {
    QString patientID = this->DicomDatabase->fieldForPatient("PatientID", patientUID);
    this->createStudyModel(patientUID, patientID);

    QString patientName = this->DicomDatabase->fieldForPatient("PatientsName", patientUID);
    patientName.replace(R"(^)", R"( )");
    int studyCount = this->getStudyCountForPatient(patientUID);
    int filteredStudyCount = this->getFilteredStudyCountForPatient(patientUID);
    int seriesCount = this->getSeriesCountForPatient(patientUID);
    int filteredSeriesCount = this->getFilteredSeriesCountForPatient(patientUID);
    // Allow query results to be visible even with zero data
    bool isQueryResult = (studyCount == 0 && seriesCount == 0);
    bool isVisible = ((studyCount > 0 && filteredStudyCount != 0 && seriesCount > 0 && filteredSeriesCount != 0) || isQueryResult) &&
                     this->matchesPatientIDFilter(patientID) &&
                     this->matchesPatientNameFilter(patientName);

    // Only add if it's a new patient
    if (!existingPatientUIDs.contains(patientUID))
    {
      PatientData data;
      data.patientUID = patientUID;
      data.patientID = patientID;
      data.patientName = patientName;
      data.patientBirthDate = this->DicomDatabase->fieldForPatient("PatientsBirthDate", patientUID);
      // Fix YYYY-MM-DD format in YYYYMMDD
      data.patientBirthDate.remove('-');
      data.patientSex = this->DicomDatabase->fieldForPatient("PatientsSex", patientUID);
      data.insertDateTime = this->DicomDatabase->insertDateTimeForPatient(patientUID);
      data.studyCount = studyCount;
      data.filteredStudyCount = filteredStudyCount;
      data.seriesCount = seriesCount;
      data.filteredSeriesCount = filteredSeriesCount;
      data.isVisible = isVisible;
      data.isQueryResult = isQueryResult;
      data.operationStatus = ctkDICOMPatientModel::NoOperation;
      newPatients.append(data);
    }
    else
    {
      // Update existing patient data (counts, visibility, filters)
      int existingIndex = this->PatientUIDToIndex.value(patientUID, -1);
      if (existingIndex >= 0 && existingIndex < this->Patients.size())
      {
        PatientData& existingPatient = this->Patients[existingIndex];

        // Only update and emit dataChanged if something actually changed
        bool hasChanges = (existingPatient.studyCount != studyCount ||
                           existingPatient.filteredStudyCount != filteredStudyCount ||
                           existingPatient.seriesCount != seriesCount ||
                           existingPatient.filteredSeriesCount != filteredSeriesCount ||
                           existingPatient.isVisible != isVisible);
        if (hasChanges)
        {
          existingPatient.studyCount = studyCount;
          existingPatient.filteredStudyCount = filteredStudyCount;
          existingPatient.seriesCount = seriesCount;
          existingPatient.filteredSeriesCount = filteredSeriesCount;
          existingPatient.isVisible = isVisible;

          // Emit data changed for updated patient
          QModelIndex idx = q->index(existingIndex);
          emit q->dataChanged(idx, idx, QVector<int>() <<
            ctkDICOMPatientModel::StudyCountRole <<
            ctkDICOMPatientModel::FilteredStudyCountRole <<
            ctkDICOMPatientModel::SeriesCountRole <<
            ctkDICOMPatientModel::FilteredSeriesCountRole <<
            ctkDICOMPatientModel::IsVisibleRole);
        }
      }
    }
  }

  // Remove patients that are no longer in the database
  QList<int> indicesToRemove;
  for (int index = 0; index < this->Patients.size(); ++index)
  {
    if (!validPatientUIDs.contains(this->Patients[index].patientUID))
    {
      indicesToRemove.append(index);
    }
  }

  // Remove invalid patients (in reverse order to maintain indices)
  for (int index = indicesToRemove.size() - 1; index >= 0; --index)
  {
    int indexToRemove = indicesToRemove[index];
    q->beginRemoveRows(QModelIndex(), indexToRemove, indexToRemove);

    // Remove from study models map
    QString patientUID = this->Patients[indexToRemove].patientUID;
    this->StudyModels.remove(patientUID);
    this->StudyFilterProxyModels.remove(patientUID);

    this->Patients.removeAt(indexToRemove);
    q->endRemoveRows();
  }

  // Add new patients if any
  if (!newPatients.isEmpty())
  {
    int firstNewRow = this->Patients.size();
    int lastNewRow = firstNewRow + newPatients.size() - 1;

    q->beginInsertRows(QModelIndex(), firstNewRow, lastNewRow);
    this->Patients.append(newPatients);
    q->endInsertRows();
  }

  // Rebuild the patientUID and PatientID to index mapping only if structure changed
  if (!indicesToRemove.isEmpty() || !newPatients.isEmpty())
  {
    this->PatientUIDToIndex.clear();
    this->PatientIDToIndex.clear();
    for (int index = 0; index < this->Patients.size(); ++index)
    {
      this->PatientUIDToIndex[this->Patients[index].patientUID] = index;
      if (!this->Patients[index].patientID.isEmpty())
      {
        this->PatientIDToIndex[this->Patients[index].patientID] = index;
      }
    }
  }

  // Update allowed servers from database for newly added patients
  if (!newPatients.isEmpty())
  {
    for (const PatientData& patientData : newPatients)
    {
      q->updateAllowedServersFromDB(patientData.patientUID);
    }
  }

  this->IsUpdating = false;

  emit q->modelRefreshed();
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModelPrivate::matchesPatientIDFilter(const QString& patientID) const
{
  if (this->PatientIDFilter.isEmpty())
  {
    return true;
  }
  return patientID.contains(this->PatientIDFilter, Qt::CaseInsensitive);
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModelPrivate::matchesPatientNameFilter(const QString& patientName) const
{
  if (this->PatientNameFilter.isEmpty())
  {
    return true;
  }
  return patientName.contains(this->PatientNameFilter, Qt::CaseInsensitive);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientModelPrivate::getStudyCountForPatient(const QString& patientUID) const
{
  if (!this->StudyModels.contains(patientUID))
  {
    return 0;
  }
  ctkDICOMStudyModel* studyModel = this->StudyModels[patientUID];
  if (!studyModel)
  {
    return 0;
  }
  return studyModel->studyInstanceUIDs().count();
}

//------------------------------------------------------------------------------
int ctkDICOMPatientModelPrivate::getFilteredStudyCountForPatient(const QString& patientUID, bool filterSeries) const
{
  if (!this->StudyModels.contains(patientUID))
  {
    return 0;
  }
  ctkDICOMStudyModel* studyModel = this->StudyModels[patientUID];
  if (!studyModel)
  {
    return 0;
  }
  return studyModel->filteredStudyInstanceUIDs(filterSeries).count();
}

//------------------------------------------------------------------------------
int ctkDICOMPatientModelPrivate::getSeriesCountForPatient(const QString& patientUID) const
{
  if (!this->StudyModels.contains(patientUID))
  {
    return 0;
  }
  ctkDICOMStudyModel* studyModel = this->StudyModels[patientUID];
  if (!studyModel)
  {
    return 0;
  }
  QStringList studyInstanceUIDs = studyModel->studyInstanceUIDs();
  int totalSeriesCount = 0;
  for (const QString& studyInstanceUID : studyInstanceUIDs)
  {
    ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyInstanceUID);
    if (seriesModel)
    {
      totalSeriesCount += seriesModel->seriesInstanceUIDs().count();
    }
  }
  return totalSeriesCount;
}

//------------------------------------------------------------------------------
int ctkDICOMPatientModelPrivate::getFilteredSeriesCountForPatient(const QString& patientUID) const
{
  if (!this->StudyModels.contains(patientUID))
  {
    return 0;
  }
  ctkDICOMStudyModel* studyModel = this->StudyModels[patientUID];
  if (!studyModel)
  {
    return 0;
  }
  QStringList filteredStudyInstanceUIDs = studyModel->filteredStudyInstanceUIDs();
  int totalFilteredSeriesCount = 0;
  for (const QString& studyInstanceUID : filteredStudyInstanceUIDs)
  {
    ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyInstanceUID);
    if (seriesModel)
    {
      totalFilteredSeriesCount += seriesModel->filteredSeriesInstanceUIDs().count();
    }
  }
  return totalFilteredSeriesCount;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModelPrivate::updatePatientCountsAndVisibility(int patientIndex)
{
  Q_Q(ctkDICOMPatientModel);
  ctkDICOMPatientModelPrivate::PatientData& patient = this->Patients[patientIndex];
  QString patientUID = patient.patientUID;
  int studyCount = this->getStudyCountForPatient(patientUID);
  int filteredStudyCount = this->getFilteredStudyCountForPatient(patientUID);
  int seriesCount = this->getSeriesCountForPatient(patientUID);
  int filteredSeriesCount = this->getFilteredSeriesCountForPatient(patientUID);

  patient.studyCount = studyCount;
  patient.filteredStudyCount = filteredStudyCount;
  patient.seriesCount = seriesCount;
  patient.filteredSeriesCount = filteredSeriesCount;
  // Allow query results to be visible even with zero data
  bool isQueryResult = (studyCount == 0 && seriesCount == 0);
  patient.isVisible = ((studyCount > 0 && filteredStudyCount != 0 && seriesCount > 0 && filteredSeriesCount != 0) || isQueryResult) &&
                      q->patientMatchesFilters(patient.patientUID);
  patient.isQueryResult = isQueryResult;
}

//------------------------------------------------------------------------------
ctkDICOMStudyModel* ctkDICOMPatientModelPrivate::createStudyModel(const QString& patientUID, const QString& patientID)
{
  Q_Q(ctkDICOMPatientModel);

  if (!this->DicomDatabase)
  {
    logger.warn("createStudyModel: No database set");
    return nullptr;
  }

  // Check if model already exists
  if (this->StudyModels.contains(patientUID))
  {
    return this->StudyModels.value(patientUID);
  }

  // Create new study model
  ctkDICOMStudyModel* studyModel = new ctkDICOMStudyModel(q);
  studyModel->setDicomDatabase(this->DicomDatabase);
  studyModel->setScheduler(this->Scheduler);
  studyModel->setStudyDescriptionFilter(this->StudyDescriptionFilter);
  studyModel->setSeriesDescriptionFilter(this->SeriesDescriptionFilter);
  studyModel->setDateFilter(static_cast<ctkDICOMStudyModel::DateType>(this->DateFilter));
  studyModel->setModalityFilter(this->ModalityFilter);
  studyModel->setAllowedServers(q->allowedServers(patientUID));
  studyModel->setNumberOfOpenedStudies(this->NumberOfOpenedStudiesPerPatient);
  studyModel->setThumbnailSize(this->ThumbnailSize);
  studyModel->setPatientID(patientID);
  studyModel->setPatientUID(patientUID);
  this->StudyModels.insert(patientUID, studyModel);

  ctkDICOMStudyFilterProxyModel* studyFilterProxyModel = new ctkDICOMStudyFilterProxyModel(q);
  studyFilterProxyModel->setSourceModel(studyModel);
  this->StudyFilterProxyModels.insert(patientUID, studyFilterProxyModel);

  emit q->studyModelCreated(patientUID, studyModel);
  return studyModel;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModelPrivate::updateStudyModelFilters(ctkDICOMStudyModel* studyModel)
{
  Q_Q(ctkDICOMPatientModel);
  if (!studyModel || studyModel->patientUID().isEmpty())
  {
    return;
  }

  studyModel->setStudyDescriptionFilter(this->StudyDescriptionFilter);
  studyModel->setSeriesDescriptionFilter(this->SeriesDescriptionFilter);
  studyModel->setDateFilter(static_cast<ctkDICOMStudyModel::DateType>(this->DateFilter));
  studyModel->setCustomDateRange(this->CustomStartDate, this->CustomEndDate);
  studyModel->setModalityFilter(this->ModalityFilter);
  studyModel->setAllowedServers(q->allowedServers(studyModel->patientUID()));
  studyModel->setNumberOfOpenedStudies(this->NumberOfOpenedStudiesPerPatient);
  studyModel->setThumbnailSize(this->ThumbnailSize);
}

//------------------------------------------------------------------------------
// ctkDICOMPatientModel

//------------------------------------------------------------------------------
ctkDICOMPatientModel::ctkDICOMPatientModel(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMPatientModelPrivate(*this))
{
  Q_D(ctkDICOMPatientModel);
  d->init();
}

//------------------------------------------------------------------------------
ctkDICOMPatientModel::~ctkDICOMPatientModel() = default;

//------------------------------------------------------------------------------
int ctkDICOMPatientModel::rowCount(const QModelIndex& parent) const
{
  Q_D(const ctkDICOMPatientModel);

  if (parent.isValid())
  {
    return 0;
  }

  return d->Patients.count();
}

//------------------------------------------------------------------------------
QVariant ctkDICOMPatientModel::data(const QModelIndex& index, int role) const
{
  Q_D(const ctkDICOMPatientModel);

  if (!index.isValid() || index.row() >= d->Patients.count())
  {
    return QVariant();
  }

  const ctkDICOMPatientModelPrivate::PatientData& patient = d->Patients.at(index.row());

  switch (role)
  {
    case Qt::DisplayRole:
      return patient.patientName;

    case Qt::ToolTipRole:
    {
      QStringList infoParts;
      if (!patient.patientName.isEmpty())
      {
        infoParts << QString("Patient name: %1").arg(patient.patientName);
      }
      if (!patient.patientID.isEmpty())
      {
        infoParts << QString("Patient MRN: %1").arg(patient.patientID);
      }
      if (!patient.patientBirthDate.isEmpty())
      {
        infoParts << QString("Birth Date: %1").arg(patient.patientBirthDate);
      }
      if (!patient.patientSex.isEmpty())
      {
        infoParts << QString("Sex: %1").arg(patient.patientSex);
      }
      infoParts << QString("Number of studies: %1").arg(patient.studyCount);
      if (patient.filteredStudyCount != patient.studyCount)
      {
        infoParts << QString("Number of filtered studies: %1").arg(patient.filteredStudyCount);
      }
      if (patient.insertDateTime.isValid())
      {
        infoParts << QString("Date added: %1").arg(patient.insertDateTime.toString("dd MMM yyyy hh:mm"));
      }

      return infoParts.join("\n");
    }

    case PatientUIDRole:
      return patient.patientUID;

    case PatientIDRole:
      return patient.patientID;

    case PatientNameRole:
      return patient.patientName;

    case PatientBirthDateRole:
      return patient.patientBirthDate;

    case PatientSexRole:
      return patient.patientSex;

    case PatientInsertDateTimeRole:
      return patient.insertDateTime;

    case StudyCountRole:
      return patient.studyCount;

    case FilteredStudyCountRole:
      return patient.filteredStudyCount;

    case SeriesCountRole:
      return patient.seriesCount;

    case FilteredSeriesCountRole:
      return patient.filteredSeriesCount;

    case IsVisibleRole:
      return patient.isVisible;

    case OperationStatusRole:
      return static_cast<int>(patient.operationStatus);

    case AllowedServersRole:
      return patient.allowedServers;

    case StoppedJobUIDRole:
      return patient.stoppedJobUID;

    default:
      return QVariant();
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  Q_D(ctkDICOMPatientModel);

  if (!index.isValid() || index.row() >= d->Patients.count())
  {
    return false;
  }

  ctkDICOMPatientModelPrivate::PatientData& patient = d->Patients[index.row()];
  bool changed = false;

  switch (role)
  {
    case Qt::DisplayRole:
    {
      // Not stored, just for display
      break;
    }
    case PatientNameRole:
    {
      if (patient.patientName != value.toString())
      {
        patient.patientName = value.toString();
        changed = true;
      }
      break;
    }
    case PatientUIDRole:
    {
      if (patient.patientUID != value.toString())
      {
        patient.patientUID = value.toString();
        changed = true;
      }
      break;
    }
    case PatientIDRole:
    {
      if (patient.patientID != value.toString())
      {
        patient.patientID = value.toString();
        changed = true;
      }
      break;
    }
    case PatientBirthDateRole:
    {
      if (patient.patientBirthDate != value.toString())
      {
        patient.patientBirthDate= value.toString();
        // Fix YYYY-MM-DD format in YYYYMMDD
        patient.patientBirthDate.remove('-');
        changed = true;
      }
      break;
    }
    case PatientSexRole:
    {
      if (patient.patientSex != value.toString())
      {
        patient.patientSex = value.toString();
        changed = true;
      }
      break;
    }
    case PatientInsertDateTimeRole:
    {
      if (patient.insertDateTime != value.toDateTime())
      {
        patient.insertDateTime = value.toDateTime();
        changed = true;
      }
      break;
    }
    case StudyCountRole:
    {
      if (patient.studyCount != value.toInt())
      {
        patient.studyCount = value.toInt();
        changed = true;
      }
      break;
    }
    case FilteredStudyCountRole:
    {
      if (patient.filteredStudyCount != value.toInt())
      {
        patient.filteredStudyCount = value.toInt();
        changed = true;
      }
      break;
    }
    case SeriesCountRole:
    {
      if (patient.seriesCount != value.toInt())
      {
        patient.seriesCount = value.toInt();
        changed = true;
      }
      break;
    }
    case FilteredSeriesCountRole:
    {
      if (patient.filteredSeriesCount != value.toInt())
      {
        patient.filteredSeriesCount = value.toInt();
        changed = true;
      }
      break;
    }
    case IsVisibleRole:
    {
      if (patient.isVisible != value.toBool())
      {
        patient.isVisible = value.toBool();
        changed = true;
      }
      break;
    }
    case OperationStatusRole:
    {
      OperationStatus newStatus = static_cast<OperationStatus>(value.toInt());
      if (patient.operationStatus != newStatus)
      {
        patient.operationStatus = newStatus;
        changed = true;
      }
      break;
    }
    case AllowedServersRole:
    {
      QStringList newServers = value.toStringList();
      if (patient.allowedServers != newServers)
      {
        patient.allowedServers = newServers;
        changed = true;
      }
      break;
    }
    case StoppedJobUIDRole:
    {
      QString newJobUID = value.toString();
      if (patient.stoppedJobUID != newJobUID)
      {
        patient.stoppedJobUID = newJobUID;
        changed = true;
      }
      break;
    }
    default:
      return false;
  }

  if (changed)
  {
    emit dataChanged(index, index, QVector<int>() << role);
  }

  return changed;
}

//------------------------------------------------------------------------------
Qt::ItemFlags ctkDICOMPatientModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMPatientModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
  {
    return tr("Patients");
  }

  return QVariant();
}

//------------------------------------------------------------------------------
QHash<int, QByteArray> ctkDICOMPatientModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[Qt::DisplayRole] = "display";
  roles[PatientUIDRole] = "patientUID";
  roles[PatientIDRole] = "patientID";
  roles[PatientNameRole] = "patientName";
  roles[PatientBirthDateRole] = "patientBirthDate";
  roles[PatientSexRole] = "patientSex";
  roles[StudyCountRole] = "studyCount";
  roles[FilteredStudyCountRole] = "filteredStudyCount";
  roles[FilteredSeriesCountRole] = "filteredSeriesCount";
  roles[IsVisibleRole] = "isVisible";
  roles[OperationStatusRole] = "operationStatus";
  roles[AllowedServersRole] = "allowedServers";
  roles[StoppedJobUIDRole] = "stoppedJobUID";
  return roles;
}

//------------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMPatientModel::dicomDatabase() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->DicomDatabase.data();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMPatientModel::dicomDatabaseShared() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->DicomDatabase;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setDicomDatabase(ctkDICOMDatabase& database)
{
  Q_D(ctkDICOMPatientModel);
  d->DicomDatabase = QSharedPointer<ctkDICOMDatabase>(&database, skipDelete);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setDicomDatabase(QSharedPointer<ctkDICOMDatabase> database)
{
  Q_D(ctkDICOMPatientModel);
  d->DicomDatabase = database;
}

//------------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMPatientModel::scheduler() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->Scheduler.data();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMPatientModel::schedulerShared() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->Scheduler;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setScheduler(ctkDICOMScheduler& scheduler)
{
  Q_D(ctkDICOMPatientModel);
  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);

  // Update all existing study models
  foreach (ctkDICOMStudyModel* studyModel, d->StudyModels.values())
  {
    if (studyModel)
    {
      studyModel->setScheduler(d->Scheduler);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMPatientModel);
  d->Scheduler = scheduler;

  // Update all existing study models
  foreach (ctkDICOMStudyModel* studyModel, d->StudyModels.values())
  {
    if (studyModel)
    {
      studyModel->setScheduler(d->Scheduler);
    }
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientModel::patientIDFilter() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->PatientIDFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setPatientIDFilter(const QString& patientID)
{
  Q_D(ctkDICOMPatientModel);
  if (d->PatientIDFilter == patientID)
  {
    return;
  }

  d->PatientIDFilter = patientID;
  this->refreshPatients();
  emit patientIDFilterChanged(patientID);
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientModel::patientNameFilter() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->PatientNameFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setPatientNameFilter(const QString& patientName)
{
  Q_D(ctkDICOMPatientModel);
  if (d->PatientNameFilter == patientName)
  {
    return;
  }

  d->PatientNameFilter = patientName;
  this->refreshPatients();
  emit patientNameFilterChanged(patientName);
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientModel::studyDescriptionFilter() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->StudyDescriptionFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setStudyDescriptionFilter(const QString& description)
{
  Q_D(ctkDICOMPatientModel);
  if (d->StudyDescriptionFilter == description)
  {
    return;
  }

  d->StudyDescriptionFilter = description;
  this->updateStudyModelsFilters();
  this->refreshPatients();
  emit studyDescriptionFilterChanged(description);
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientModel::seriesDescriptionFilter() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->SeriesDescriptionFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setSeriesDescriptionFilter(const QString& description)
{
  Q_D(ctkDICOMPatientModel);
  if (d->SeriesDescriptionFilter == description)
  {
    return;
  }

  d->SeriesDescriptionFilter = description;
  this->updateStudyModelsFilters();
  this->refreshPatients();
  emit seriesDescriptionFilterChanged(description);
}

//------------------------------------------------------------------------------
ctkDICOMPatientModel::DateType ctkDICOMPatientModel::dateFilter() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->DateFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setDateFilter(DateType dateType)
{
  Q_D(ctkDICOMPatientModel);
  if (d->DateFilter == dateType)
  {
    return;
  }

  d->DateFilter = dateType;
  this->updateStudyModelsFilters();
  this->refreshPatients();
  emit dateFilterChanged(dateType);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setCustomDateRange(const QDate& startDate, const QDate& endDate)
{
  Q_D(ctkDICOMPatientModel);
  if (d->CustomStartDate == startDate && d->CustomEndDate == endDate)
  {
    return;
  }

  d->CustomStartDate = startDate;
  d->CustomEndDate = endDate;

  if (d->DateFilter == DateType::CustomRange)
  {
    this->updateStudyModelsFilters();
    this->refreshPatients();
  }
}

//------------------------------------------------------------------------------
QDate ctkDICOMPatientModel::customStartDate() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->CustomStartDate;
}

//------------------------------------------------------------------------------
QDate ctkDICOMPatientModel::customEndDate() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->CustomEndDate;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMPatientModel::modalityFilter() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->ModalityFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setModalityFilter(const QStringList& modalities)
{
  Q_D(ctkDICOMPatientModel);
  if (d->ModalityFilter == modalities)
  {
    return;
  }

  d->ModalityFilter = modalities;
  this->updateStudyModelsFilters();
  this->refreshPatients();
  emit modalityFilterChanged(modalities);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMPatientModel::allowedServers(const QString& patientUID) const
{
  QModelIndex patientIndex = this->indexFromPatientUID(patientUID);
  return this->data(patientIndex, AllowedServersRole).toStringList();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setAllowedServers(const QString& patientUID, const QStringList& servers)
{
  QModelIndex patientIndex = this->indexFromPatientUID(patientUID);
  this->setData(patientIndex, servers, AllowedServersRole);
  this->updateStudyModelsFilters();
  emit allowedServersChanged(patientUID, servers);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientModel::numberOfOpenedStudiesPerPatient() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->NumberOfOpenedStudiesPerPatient;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setNumberOfOpenedStudiesPerPatient(int count)
{
  Q_D(ctkDICOMPatientModel);
  if (d->NumberOfOpenedStudiesPerPatient == count)
  {
    return;
  }

  d->NumberOfOpenedStudiesPerPatient = count;
  this->updateStudyModelsFilters();
  emit this->numberOfOpenedStudiesPerPatientChanged(count);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientModel::thumbnailSize() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->ThumbnailSize;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setThumbnailSize(int size)
{
  Q_D(ctkDICOMPatientModel);
  if (d->ThumbnailSize == size)
  {
    return;
  }

  d->ThumbnailSize = size;
  this->updateStudyModelsFilters();
  emit this->thumbnailSizeChanged(size);
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModel::queryInProgress() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->QueryInProgress;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::setQueryInProgress(bool inProgress)
{
  Q_D(ctkDICOMPatientModel);
  if (d->QueryInProgress == inProgress)
  {
    return;
  }

  d->QueryInProgress = inProgress;
  emit this->queryInProgressChanged(inProgress);
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientModel::patientUID(const QModelIndex& index) const
{
  return this->data(index, this->PatientUIDRole).toString();
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientModel::patientID(const QModelIndex& index) const
{
  return this->data(index, this->PatientIDRole).toString();
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientModel::patientName(const QModelIndex& index) const
{
  return this->data(index, this->PatientNameRole).toString();
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMPatientModel::indexFromPatientUID(const QString& patientUID) const
{
  Q_D(const ctkDICOMPatientModel);
  int row = d->PatientUIDToIndex.value(patientUID, -1);
  if (row < 0 || row >= d->Patients.count())
  {
    return QModelIndex();
  }
  return this->index(row, 0);
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMPatientModel::indexFromPatientID(const QString& patientID) const
{
  Q_D(const ctkDICOMPatientModel);
  int row = d->PatientIDToIndex.value(patientID, -1);
  if (row < 0 || row >= d->Patients.count())
  {
    return QModelIndex();
  }
  return this->index(row, 0);
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModel::isPatientVisible(const QModelIndex &index) const
{
  return this->data(index, this->IsVisibleRole).toBool();
}

//------------------------------------------------------------------------------
ctkDICOMStudyModel* ctkDICOMPatientModel::studyModelForPatient(const QModelIndex& patientIndex)
{
  QString patientUID = this->patientUID(patientIndex);
  if (patientUID.isEmpty())
  {
    return nullptr;
  }
  return studyModelForPatientUID(patientUID);
}

//------------------------------------------------------------------------------
ctkDICOMStudyModel* ctkDICOMPatientModel::studyModelForPatientUID(const QString& patientUID)
{
  Q_D(ctkDICOMPatientModel);
  if (patientUID.isEmpty())
  {
    return nullptr;
  }
  return d->StudyModels.value(patientUID, nullptr);
}

//------------------------------------------------------------------------------
ctkDICOMStudyModel* ctkDICOMPatientModel::studyModelForPatientID(const QString& patientID)
{
  QModelIndex idx = this->indexFromPatientID(patientID);
  return this->studyModelForPatient(idx);
}

//------------------------------------------------------------------------------
ctkDICOMStudyFilterProxyModel* ctkDICOMPatientModel::studyFilterProxyModelForPatient(const QModelIndex& patientIndex)
{
  QString patientUID = this->patientUID(patientIndex);
  if (patientUID.isEmpty())
  {
    return nullptr;
  }
  return studyFilterProxyModelForPatientUID(patientUID);
}

//------------------------------------------------------------------------------
ctkDICOMStudyFilterProxyModel* ctkDICOMPatientModel::studyFilterProxyModelForPatientUID(const QString& patientUID)
{
  Q_D(ctkDICOMPatientModel);
  if (patientUID.isEmpty())
  {
    return nullptr;
  }
  return d->StudyFilterProxyModels.value(patientUID, nullptr);
}

//------------------------------------------------------------------------------
ctkDICOMStudyFilterProxyModel* ctkDICOMPatientModel::studyFilterProxyModelForPatientID(const QString& patientID)
{
  QModelIndex idx = this->indexFromPatientID(patientID);
  return this->studyFilterProxyModelForPatient(idx);
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModel::hasStudyModel(const QModelIndex& patientIndex) const
{
  QString patientUID = this->patientUID(patientIndex);
  return this->hasStudyModel(patientUID);
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModel::hasStudyModel(const QString& patientUID) const
{
  Q_D(const ctkDICOMPatientModel);
  return d->StudyModels.contains(patientUID);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::removeStudyModel(const QModelIndex& patientIndex)
{
  QString patientUID = this->patientUID(patientIndex);
  this->removeStudyModel(patientUID);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::removeStudyModel(const QString& patientUID)
{
  Q_D(ctkDICOMPatientModel);

  if (!d->StudyModels.contains(patientUID))
  {
    return;
  }

  ctkDICOMStudyModel* studyModel = d->StudyModels.take(patientUID);
  if (studyModel)
  {
    delete studyModel;
  }

  ctkDICOMStudyFilterProxyModel* studyFilterProxyModel = d->StudyFilterProxyModels.take(patientUID);
  if (studyFilterProxyModel)
  {
    delete studyFilterProxyModel;
  }

  emit this->studyModelRemoved(patientUID);
}

//------------------------------------------------------------------------------
QList<ctkDICOMStudyModel*> ctkDICOMPatientModel::allStudyModels() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->StudyModels.values();
}

//------------------------------------------------------------------------------
QList<ctkDICOMStudyFilterProxyModel*> ctkDICOMPatientModel::allStudyFilterProxyModels() const
{
  Q_D(const ctkDICOMPatientModel);
  return d->StudyFilterProxyModels.values();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::updateStudyModelsFilters()
{
  Q_D(ctkDICOMPatientModel);

  foreach (ctkDICOMStudyModel* studyModel, d->StudyModels.values())
  {
    d->updateStudyModelFilters(studyModel);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::refreshStudyModels()
{
  Q_D(ctkDICOMPatientModel);

  foreach (ctkDICOMStudyModel* studyModel, d->StudyModels.values())
  {
    studyModel->refreshStudies();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::refreshSeriesModels()
{
  Q_D(ctkDICOMPatientModel);

  foreach (ctkDICOMStudyModel* studyModel, d->StudyModels.values())
  {
    studyModel->refreshSeriesModels();
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModel::patientMatchesFilters(const QString& patientUID) const
{
  Q_D(const ctkDICOMPatientModel);

  int row = d->PatientUIDToIndex.value(patientUID, -1);
  if (row < 0 || row >= d->Patients.count())
  {
    return false;
  }

  const ctkDICOMPatientModelPrivate::PatientData& patient = d->Patients.at(row);
  return d->matchesPatientIDFilter(patient.patientID) &&
         d->matchesPatientNameFilter(patient.patientName);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMPatientModel::filteredPatientUIDs() const
{
  Q_D(const ctkDICOMPatientModel);

  QStringList filteredUIDs;
  for (const ctkDICOMPatientModelPrivate::PatientData& patient : d->Patients)
  {
    if (d->matchesPatientIDFilter(patient.patientID) &&
        d->matchesPatientNameFilter(patient.patientName))
      {
      filteredUIDs.append(patient.patientUID);
    }
  }

  return filteredUIDs;
}

//------------------------------------------------------------------------------
int ctkDICOMPatientModel::filteredStudiesCountForPatient(const QString& patientUID) const
{
  Q_D(const ctkDICOMPatientModel);
  return d->getFilteredStudyCountForPatient(patientUID, false);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientModel::filteredSeriesCountForPatient(const QString& patientUID) const
{
  Q_D(const ctkDICOMPatientModel);
  return d->getFilteredSeriesCountForPatient(patientUID);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::refresh()
{
  Q_D(ctkDICOMPatientModel);
  d->populatePatients();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::clean()
{
  Q_D(ctkDICOMPatientModel);
  d->clean();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::refreshPatients()
{
  Q_D(ctkDICOMPatientModel);
  // Update all patients in batch and emit dataChanged only once
  if (d->Patients.isEmpty())
  {
    return;
  }

  // Update all patient data in-place
  for (int patientIndex = 0; patientIndex < d->Patients.size(); ++patientIndex)
  {
    d->updatePatientCountsAndVisibility(patientIndex);
  }

  // Emit dataChanged for the whole range
  QModelIndex top = this->index(0, 0);
  QModelIndex bottom = this->index(d->Patients.size() - 1, 0);
  emit dataChanged(top, bottom, QVector<int>()
    << ctkDICOMPatientModel::StudyCountRole
    << ctkDICOMPatientModel::FilteredStudyCountRole
    << ctkDICOMPatientModel::SeriesCountRole
    << ctkDICOMPatientModel::FilteredSeriesCountRole
    << ctkDICOMPatientModel::IsVisibleRole
  );
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::refreshPatient(const QString& patientUID)
{
  Q_D(ctkDICOMPatientModel);

  int patientIndex = d->PatientUIDToIndex.value(patientUID, -1);
  if (patientIndex < 0 || patientIndex >= d->Patients.count())
  {
    return;
  }

  d->updatePatientCountsAndVisibility(patientIndex);

  QModelIndex idx = index(patientIndex, 0);
  emit dataChanged(idx, idx, QVector<int>() <<
    ctkDICOMPatientModel::StudyCountRole <<
    ctkDICOMPatientModel::FilteredStudyCountRole <<
    ctkDICOMPatientModel::SeriesCountRole <<
    ctkDICOMPatientModel::FilteredSeriesCountRole <<
    ctkDICOMPatientModel::IsVisibleRole
  );
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientModel::queryStudies(const QString &patientID)
{
  Q_D(ctkDICOMPatientModel);
  if (!d->Scheduler)
  {
    logger.warn("queryStudies: No scheduler set");
    return false;
  }
  QString patientUID;
  int row = d->PatientIDToIndex.value(patientID, -1);
  if (row >= 0 && row < d->Patients.count())
  {
    patientUID = d->Patients.at(row).patientUID;
  }

  if (this->allowedServers(patientUID).count() == 0)
  {
    return false;
  }

  d->Scheduler->queryStudies(patientID,
                             QThread::LowPriority,
                             this->allowedServers(patientUID));
  return true;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::updateGUIFromScheduler(const QVariant& data)
{
  Q_D(ctkDICOMPatientModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.PatientID.isEmpty())
  {
    return;
  }

  int row = d->PatientIDToIndex.value(td.PatientID, -1);
  if (row < 0 || row >= d->Patients.count())
  {
    return;
  }

  const QString& patientUID = d->Patients.at(row).patientUID;
  ctkDICOMStudyModel* studyModel = d->StudyModels.value(patientUID, nullptr);
  if (!studyModel)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies &&
      td.QueriedStudyInstanceUIDs.isEmpty())
  {
    QModelIndex idx = index(row, 0);
    this->setData(idx, int(OperationStatus::Failed), ctkDICOMPatientModel::OperationStatusRole);
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
      td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
      td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::ThumbnailGenerator ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    studyModel->updateGUIFromScheduler(data);
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    this->refreshPatient(patientUID);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::onJobStarted(const QVariant& data)
{
  Q_D(ctkDICOMPatientModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.PatientID.isEmpty())
  {
    return;
  }

  int row = d->PatientIDToIndex.value(td.PatientID, -1);
  if (row < 0 || row >= d->Patients.count())
  {
    return;
  }

  const QString& patientUID = d->Patients.at(row).patientUID;
  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
  {
    QModelIndex idx = index(row, 0);
    this->setData(idx, int(OperationStatus::InProgress), ctkDICOMPatientModel::OperationStatusRole);
  }

  ctkDICOMStudyModel* studyModel = d->StudyModels.value(patientUID, nullptr);
  if (studyModel)
  {
    studyModel->onJobStarted(data);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::onJobUserStopped(const QVariant& data)
{
  Q_D(ctkDICOMPatientModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.PatientID.isEmpty())
  {
    return;
  }

  int row = d->PatientIDToIndex.value(td.PatientID, -1);
  if (row < 0 || row >= d->Patients.count())
  {
    return;
  }

  const QString& patientUID = d->Patients.at(row).patientUID;
  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
  {
    QModelIndex idx = index(row, 0);
    this->setData(idx, int(OperationStatus::Failed), ctkDICOMPatientModel::OperationStatusRole);
  }

  ctkDICOMStudyModel* studyModel = d->StudyModels.value(patientUID, nullptr);
  if (studyModel)
  {
    studyModel->onJobUserStopped(data);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::onJobFailed(const QVariant& data)
{
  Q_D(ctkDICOMPatientModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.PatientID.isEmpty())
  {
    return;
  }

  int row = d->PatientIDToIndex.value(td.PatientID, -1);
  if (row < 0 || row >= d->Patients.count())
  {
    return;
  }

  const QString& patientUID = d->Patients.at(row).patientUID;
  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
  {
    QModelIndex idx = index(row, 0);
    this->setData(idx, int(OperationStatus::Failed), ctkDICOMPatientModel::OperationStatusRole);
  }

  ctkDICOMStudyModel* studyModel = d->StudyModels.value(patientUID, nullptr);
  if (studyModel)
  {
    studyModel->onJobFailed(data);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::onJobFinished(const QVariant& data)
{
  Q_D(ctkDICOMPatientModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.PatientID.isEmpty())
  {
    return;
  }

  int row = d->PatientIDToIndex.value(td.PatientID, -1);
  if (row < 0 || row >= d->Patients.count())
  {
    return;
  }

  const QString& patientUID = d->Patients.at(row).patientUID;
  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
  {
    QModelIndex idx = index(row, 0);
    this->setData(idx, int(OperationStatus::Completed), ctkDICOMPatientModel::OperationStatusRole);
  }

  ctkDICOMStudyModel* studyModel = d->StudyModels.value(patientUID, nullptr);
  if (studyModel)
  {
    studyModel->onJobFinished(data);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::updateAllowedServersFromDB(const QString& patientUID)
{
  Q_D(ctkDICOMPatientModel);

  if (!d->DicomDatabase)
  {
    logger.error("updateAllowedServersFromDB: No database set");
    return;
  }

  if (!d->Scheduler)
  {
    logger.error("updateAllowedServersFromDB: No scheduler set");
    return;
  }

  int patientIndex = d->PatientUIDToIndex.value(patientUID, -1);
  if (patientIndex < 0 || patientIndex >= d->Patients.count())
  {
    logger.error("updateAllowedServersFromDB: Invalid patient item");
    return;
  }

  // Get all active servers (either query/retrieve or storage is toggled)
  QStringList allActiveConnectionNames = d->Scheduler->getConnectionNamesForActiveServers();
  if (allActiveConnectionNames.count() == 0)
  {
    return;
  }

  // Get connection names from database where the patient has been fetched
  // We assume that such server is allowed by default
  QMap<QString, QStringList> connectionsInformation = d->DicomDatabase->connectionsInformationForPatient(patientUID);
  QStringList allowList = connectionsInformation["allow"];
  QStringList denyList = connectionsInformation["deny"];

  QStringList allowedServers;
  for (const QString& connectionName : allActiveConnectionNames)
  {
    if (allowList.contains(connectionName))
    {
      allowedServers.append(connectionName);
    }
    else if (!denyList.contains(connectionName))
    {
      // If not explicitly denied, check if server is trusted
      ctkDICOMServer* server = d->Scheduler->server(connectionName);
      if (server && server->trustedEnabled())
      {
        allowedServers.append(connectionName);
      }
    }
  }

  // Update the patient data in the model
  QModelIndex patientModelIndex = this->indexFromPatientUID(patientUID);
  this->setData(patientModelIndex, allowedServers, AllowedServersRole);
  /// force view to update. (e.g. allowedServers is same, but all servers have changed state)
  emit dataChanged(patientModelIndex, patientModelIndex, QVector<int>() << AllowedServersRole);

  // Propagate allowed servers to the study model for this patient
  ctkDICOMStudyModel* studyModel = d->StudyModels.value(patientUID, nullptr);
  if (studyModel)
  {
    studyModel->setAllowedServers(allowedServers);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientModel::saveAllowedServersToDB(const QString& patientUID, const QStringList& allowedServers)
{
  Q_D(ctkDICOMPatientModel);

  if (!d->DicomDatabase)
  {
    logger.error("saveAllowedServersToDB: No database set");
    return;
  }

  if (!d->Scheduler)
  {
    logger.error("saveAllowedServersToDB: No scheduler set");
    return;
  }

  int patientIndex = d->PatientUIDToIndex.value(patientUID, -1);
  if (patientIndex < 0 || patientIndex >= d->Patients.count())
  {
    logger.error("saveAllowedServersToDB: Invalid patient item");
    return;
  }

  // Get all active servers
  QStringList allActiveConnectionNames = d->Scheduler->getConnectionNamesForActiveServers();

  // Build allow and deny lists based on current allowed servers
  QStringList databaseAllowList;
  QStringList databaseDenyList;

  for (const QString& connectionName : allActiveConnectionNames)
  {
    if (allowedServers.contains(connectionName))
    {
      // Explicitly allowed
      databaseAllowList.append(connectionName);
    }
    else
    {
      // Check if this is a trusted server (partially checked state)
      ctkDICOMServer* server = d->Scheduler->server(connectionName);
      if (server && server->trustedEnabled())
      {
        // Don't add to either list - let it remain in partially checked state
      }
      else
      {
        // Explicitly denied
        databaseDenyList.append(connectionName);
      }
    }
  }

  // Update database
  d->DicomDatabase->updateConnectionsForPatient(patientUID, databaseAllowList, databaseDenyList);

  // Update the model
  QModelIndex idx = this->index(patientIndex, 0);
  this->setData(idx, allowedServers, AllowedServersRole);

  // Propagate to study model
  ctkDICOMStudyModel* studyModel = d->StudyModels.value(patientUID, nullptr);
  if (studyModel)
  {
    studyModel->setAllowedServers(allowedServers);
  }
}
