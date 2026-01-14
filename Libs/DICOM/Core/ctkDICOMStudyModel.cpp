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
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModalities.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMSeriesFilterProxyModel.h"
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMJob.h"

static ctkLogger logger("org.commontk.DICOM.Core.ctkDICOMStudyModel");

//------------------------------------------------------------------------------
// Helper function for setDicomDatabase/setScheduler
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//------------------------------------------------------------------------------
// ctkDICOMStudyModelPrivate

class ctkDICOMStudyModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMStudyModel);

protected:
  ctkDICOMStudyModel* const q_ptr;

public:
  ctkDICOMStudyModelPrivate(ctkDICOMStudyModel& obj);
  ~ctkDICOMStudyModelPrivate();

  void populateStudies();
  QString formatDate(const QString& date) const;
  QString formatTime(const QString& time) const;
  bool matchesDateFilter(const QString& studyDate) const;
  bool matchesDescriptionFilter(const QString& description) const;
  int getSeriesCountForStudy(const QString& studyInstanceUID) const;
  int getFilteredSeriesCountForStudy(const QString& studyInstanceUID) const;
  QString getPatientUIDFromPatientID(const QString& patientID) const;
  void updateFilteredSeriesCounts();
  void updateStudyCountsAndVisibility(int studyIndex);
  void clean();

  // Data storage
  struct StudyData {
    QString studyInstanceUID;
    QString studyID;
    QString studyDescription;
    QString studyDate;
    QString studyTime;
    QString accessionNumber;
    QString modalitiesInStudy;
    QString patientUID;
    QString patientID;
    QString patientName;
    QString patientBirthDate;
    int seriesCount;
    int filteredSeriesCount;
    bool isCollapsed;
    bool isVisible;
    ctkDICOMStudyModel::OperationStatus operationStatus;
    QString stoppedJobUID;
  };

  QList<StudyData> Studies;

  // Series model management
  QHash<QString, ctkDICOMSeriesModel*> SeriesModels; // StudyInstanceUID -> SeriesModel
  QHash<QString, ctkDICOMSeriesFilterProxyModel*> SeriesFilterProxyModels; // StudyInstanceUID -> SeriesFilterProxyModel
  ctkDICOMSeriesModel* createSeriesModel(const QString& studyInstanceUID);
  void updateSeriesModelFilters(ctkDICOMSeriesModel* seriesModel);

  // Database and scheduler
  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<ctkDICOMScheduler> Scheduler;

  // Filters
  QString PatientUID;
  QString PatientID;
  QString StudyDescriptionFilter;  // Study description filter
  ctkDICOMStudyModel::DateType DateFilter;
  QDate CustomStartDate;
  QDate CustomEndDate;
  QStringList ModalityFilter;
  QString SeriesDescriptionFilter; // Series description filter (propagated to series models)
  QStringList AllowedServers;

  // Configuration
  int NumberOfOpenedStudies;
  int ThumbnailSize;

  // State
  bool IsUpdating;
};

//------------------------------------------------------------------------------
ctkDICOMStudyModelPrivate::ctkDICOMStudyModelPrivate(ctkDICOMStudyModel& obj)
  : q_ptr(&obj)
{
  this->PatientUID = "";
  this->PatientID = "";
  this->StudyDescriptionFilter = "";
  this->SeriesDescriptionFilter = "";
  this->DateFilter = ctkDICOMStudyModel::Any;
  this->NumberOfOpenedStudies = 2;
  this->ThumbnailSize = 128;
  this->IsUpdating = false;
  this->ModalityFilter = ctkDICOMModalities::AllModalities;
}

//------------------------------------------------------------------------------
ctkDICOMStudyModelPrivate::~ctkDICOMStudyModelPrivate()
{
  // Clean up series models
  qDeleteAll(this->SeriesModels.values());
  this->SeriesModels.clear();
  qDeleteAll(this->SeriesFilterProxyModels.values());
  this->SeriesFilterProxyModels.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModelPrivate::clean()
{
  Q_Q(ctkDICOMStudyModel);
  this->IsUpdating = true;

  // Clean up all series models first
  foreach (ctkDICOMSeriesModel* seriesModel, this->SeriesModels.values())
  {
    if (seriesModel)
    {
      seriesModel->clean();
    }
  }

  // Delete series models and proxy models
  qDeleteAll(this->SeriesModels.values());
  this->SeriesModels.clear();
  qDeleteAll(this->SeriesFilterProxyModels.values());
  this->SeriesFilterProxyModels.clear();

  // Clear the studies list
  q->beginResetModel();
  this->Studies.clear();
  q->endResetModel();

  this->IsUpdating = false;
  emit q->modelRefreshed();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModelPrivate::populateStudies()
{
  Q_Q(ctkDICOMStudyModel);

  if (!this->DicomDatabase)
  {
    logger.error("populateStudies: No database set");
    return;
  }

  if (this->IsUpdating)
  {
    return;
  }

  this->IsUpdating = true;

  if (this->PatientUID.isEmpty())
  {
    // If no patient found, clear the model if it has data
    if (!this->Studies.isEmpty())
    {
      q->beginResetModel();
      qDeleteAll(this->SeriesModels.values());
      this->SeriesModels.clear();
      qDeleteAll(this->SeriesFilterProxyModels.values());
      this->SeriesFilterProxyModels.clear();
      q->endResetModel();
    }
    this->IsUpdating = false;
    return;
  }

  // Get studies for this patient
  QStringList studiesList = this->DicomDatabase->studiesForPatient(this->PatientUID);
  studiesList.removeAll("");
  if (studiesList.isEmpty())
  {
    // If no studies found, clear the model if it has data
    if (!this->Studies.isEmpty())
    {
      q->beginResetModel();
      qDeleteAll(this->SeriesModels.values());
      this->SeriesModels.clear();
      qDeleteAll(this->SeriesFilterProxyModels.values());
      this->SeriesFilterProxyModels.clear();
      q->endResetModel();
    }
    this->IsUpdating = false;
    emit q->modelRefreshed();
    return;
  }

  // Create a set of existing study instance UIDs for quick lookup
  QSet<QString> existingStudyUIDs;
  for (const StudyData& study : this->Studies)
  {
    existingStudyUIDs.insert(study.studyInstanceUID);
  }

  // Create a set of database study UIDs that match filters
  QSet<QString> validStudyUIDs;
  QList<StudyData> newStudies;

  QString patientID = this->PatientID;
  QString patientName = this->DicomDatabase->fieldForPatient("PatientsName", this->PatientUID);
  patientName.replace(R"(^)", R"( )");
  QString patientBirthDate = this->DicomDatabase->fieldForPatient("PatientsBirthDate", this->PatientUID);

  foreach (const QString& studyInstanceUID, studiesList)
  {
    this->createSeriesModel(studyInstanceUID);
    validStudyUIDs.insert(studyInstanceUID);

    int filteredSeriesCount = this->getFilteredSeriesCountForStudy(studyInstanceUID);
    int seriesCount = this->getSeriesCountForStudy(studyInstanceUID);
    bool isVisible = q->studyMatchesFilters(studyInstanceUID) &&
                      (seriesCount == 0 || (seriesCount > 0 && filteredSeriesCount != 0));

    // Only add if it's a new study
    if (!existingStudyUIDs.contains(studyInstanceUID))
    {
      StudyData study;
      study.studyInstanceUID = studyInstanceUID;
      study.studyID = this->DicomDatabase->fieldForStudy("StudyID", studyInstanceUID);
      study.studyDescription = this->DicomDatabase->fieldForStudy("StudyDescription", studyInstanceUID);
      study.studyDate = this->DicomDatabase->fieldForStudy("StudyDate", studyInstanceUID);
      study.studyTime = this->DicomDatabase->fieldForStudy("StudyTime", studyInstanceUID);
      study.accessionNumber = this->DicomDatabase->fieldForStudy("AccessionNumber", studyInstanceUID);
      study.modalitiesInStudy = this->DicomDatabase->fieldForStudy("ModalitiesInStudy", studyInstanceUID);

      // Get patient information
      // First get the patient UID for this study
      study.patientUID = this->PatientUID;
      study.patientID = patientID;
      study.patientName = patientName;
      study.patientBirthDate = patientBirthDate;

      // Get series counts using database methods
      study.filteredSeriesCount = filteredSeriesCount;
      study.seriesCount = seriesCount;
      study.isVisible = isVisible;
      study.operationStatus = ctkDICOMStudyModel::NoOperation;
      study.stoppedJobUID = "";
      study.isCollapsed = true;

      newStudies.append(study);
    }
    else
    {
      // Update series counts for existing studies
      for (int existingIndex = 0; existingIndex < this->Studies.size(); ++existingIndex)
      {
        StudyData study = this->Studies[existingIndex];
        if (study.studyInstanceUID == studyInstanceUID)
        {
          // Only update and emit dataChanged if something actually changed
          bool hasChanges = (study.seriesCount != seriesCount ||
                             study.filteredSeriesCount != filteredSeriesCount ||
                             study.isVisible != isVisible);

          if (hasChanges)
          {
            this->Studies[existingIndex].seriesCount = seriesCount;
            this->Studies[existingIndex].filteredSeriesCount = filteredSeriesCount;
            this->Studies[existingIndex].isVisible = isVisible;

            // Emit data changed for updated counts and visibility
            QModelIndex idx = q->index(existingIndex);
            emit q->dataChanged(idx, idx, QVector<int>() <<
              ctkDICOMStudyModel::SeriesCountRole <<
              ctkDICOMStudyModel::FilteredSeriesCountRole <<
              ctkDICOMStudyModel::IsVisibleRole);
          }
          break;
        }
      }
    }
  }

  // Remove studies that are no longer valid (don't match current filters or patient)
  QList<int> indicesToRemove;
  for (int index = 0; index < this->Studies.size(); ++index)
  {
    if (!validStudyUIDs.contains(this->Studies[index].studyInstanceUID))
    {
      indicesToRemove.append(index);
    }
  }

  // Remove invalid studies (in reverse order to maintain indices)
  for (int index = indicesToRemove.size() - 1; index >= 0; --index)
  {
    int indexToRemove = indicesToRemove[index];
    QString studyInstanceUID = this->Studies[indexToRemove].studyInstanceUID;
    q->beginRemoveRows(QModelIndex(), indexToRemove, indexToRemove);

    // Delete the series models before removing from maps
    delete this->SeriesModels.take(studyInstanceUID);
    delete this->SeriesFilterProxyModels.take(studyInstanceUID);

    this->Studies.removeAt(indexToRemove);

    q->endRemoveRows();
  }

  // Add new studies if any
  if (!newStudies.isEmpty())
  {
    // Insert new studies at the end
    // The proxy model will handle sorting by date/time
    int insertPos = this->Studies.size();

    q->beginInsertRows(QModelIndex(), insertPos, insertPos + newStudies.size() - 1);
    for (const StudyData& newStudy : newStudies)
    {
      this->Studies.append(newStudy);
    }
    q->endInsertRows();
  }

  this->IsUpdating = false;

  emit q->modelRefreshed();
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModelPrivate::formatDate(const QString& date) const
{
  if (date.isEmpty())
  {
    return date;
  }

  // Try YYYYMMDD format first (8 characters)
  if (date.length() == 8)
  {
    QDate studyDate = QDate::fromString(date, "yyyyMMdd");
    if (studyDate.isValid())
    {
      return studyDate.toString("dd MMM yyyy");
    }
  }
  
  // Try YYYY-MM-DD format (10 characters, from SQLite DATE fields)
  if (date.length() == 10)
  {
    QDate studyDate = QDate::fromString(date, "yyyy-MM-dd");
    if (studyDate.isValid())
    {
      return studyDate.toString("dd MMM yyyy");
    }
  }
  
  return date;
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModelPrivate::formatTime(const QString& time) const
{
  if (time.length() >= 6) // HHMMSS or HHMMSS.fff
  {
    QTime studyTime = QTime::fromString(time.left(6), "hhmmss");
    if (studyTime.isValid())
    {
      return studyTime.toString("hh:mm:ss");
    }
  }
  return time;
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModelPrivate::matchesDateFilter(const QString& studyDate) const
{
  if (this->DateFilter == ctkDICOMStudyModel::Any || studyDate.isEmpty())
  {
    return true;
  }

  QDate date = QDate::fromString(studyDate, "yyyyMMdd");
  if (!date.isValid())
  {
    return true; // Don't filter out invalid dates
  }

  QDate currentDate = QDate::currentDate();
  int daysFromNow = currentDate.daysTo(date);

  switch (this->DateFilter)
  {
    case ctkDICOMStudyModel::Any:
      return true;
    case ctkDICOMStudyModel::Today:
      return daysFromNow == 0;
    case ctkDICOMStudyModel::Yesterday:
      return daysFromNow == -1;
    case ctkDICOMStudyModel::LastWeek:
      return daysFromNow >= -7 && daysFromNow <= 0;
    case ctkDICOMStudyModel::LastMonth:
      return daysFromNow >= -30 && daysFromNow <= 0;
    case ctkDICOMStudyModel::LastYear:
      return daysFromNow >= -365 && daysFromNow <= 0;
    case ctkDICOMStudyModel::CustomRange:
      if (this->CustomStartDate.isValid() && this->CustomEndDate.isValid())
      {
        return date >= this->CustomStartDate && date <= this->CustomEndDate;
      }
      return true; // If custom dates are not valid, don't filter
    default:
      return true;
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModelPrivate::matchesDescriptionFilter(const QString& description) const
{
  if (this->StudyDescriptionFilter.isEmpty())
  {
    return true;
  }

  return description.contains(this->StudyDescriptionFilter, Qt::CaseInsensitive);
}

//------------------------------------------------------------------------------
int ctkDICOMStudyModelPrivate::getSeriesCountForStudy(const QString& studyInstanceUID) const
{
  if (!this->SeriesModels.contains(studyInstanceUID))
  {
    return 0;
  }
  ctkDICOMSeriesModel* seriesModel = this->SeriesModels[studyInstanceUID];
  if (!seriesModel)
  {
    return 0;
  }

  return seriesModel->seriesInstanceUIDs().count();
}

//------------------------------------------------------------------------------
int ctkDICOMStudyModelPrivate::getFilteredSeriesCountForStudy(const QString& studyInstanceUID) const
{
  if (!this->SeriesModels.contains(studyInstanceUID))
  {
    return 0;
  }
  ctkDICOMSeriesModel* seriesModel = this->SeriesModels[studyInstanceUID];
  if (!seriesModel)
  {
    return 0;
  }

  return seriesModel->filteredSeriesInstanceUIDs().count();
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModelPrivate::getPatientUIDFromPatientID(const QString& patientID) const
{
  if (!this->DicomDatabase || patientID.isEmpty())
  {
    return "";
  }

  QStringList patientList = this->DicomDatabase->patients();
  foreach (const QString& patientUID, patientList)
  {
    QString newPatientID = this->DicomDatabase->fieldForPatient("PatientID", patientUID);
    if (patientID == newPatientID)
    {
      return patientUID;
    }
  }

  return "";
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModelPrivate::updateFilteredSeriesCounts()
{
  Q_Q(ctkDICOMStudyModel);

  if (this->Studies.isEmpty())
  {
    return;
  }

  // Emit dataChanged for the entire range at once instead of per-item
  QModelIndex topLeft = q->index(0);
  QModelIndex bottomRight = q->index(this->Studies.size() - 1);

  for (int index = 0; index < this->Studies.size(); ++index)
  {
    ctkDICOMStudyModelPrivate::StudyData& study = this->Studies[index];
    int seriesCount = this->getSeriesCountForStudy(study.studyInstanceUID);
    study.filteredSeriesCount = this->getFilteredSeriesCountForStudy(study.studyInstanceUID);
    study.isVisible = q->studyMatchesFilters(study.studyInstanceUID) &&
                      (seriesCount > 0 && study.filteredSeriesCount != 0);
  }

  q->emit dataChanged(topLeft, bottomRight, QVector<int>() << q->FilteredSeriesCountRole << q->IsVisibleRole);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModelPrivate::updateStudyCountsAndVisibility(int studyIndex)
{
  Q_Q(ctkDICOMStudyModel);
  ctkDICOMStudyModelPrivate::StudyData& study = this->Studies[studyIndex];

  study.seriesCount = this->getSeriesCountForStudy(study.studyInstanceUID);
  study.filteredSeriesCount = this->getFilteredSeriesCountForStudy(study.studyInstanceUID);
  study.isVisible = q->studyMatchesFilters(study.studyInstanceUID) &&
                    (study.seriesCount > 0 && study.filteredSeriesCount != 0);
}

//------------------------------------------------------------------------------
ctkDICOMSeriesModel* ctkDICOMStudyModelPrivate::createSeriesModel(const QString& studyInstanceUID)
{
  Q_Q(ctkDICOMStudyModel);

  if (!this->DicomDatabase)
  {
    logger.warn("createSeriesModel: No database set");
    return nullptr;
  }

  if (this->SeriesModels.contains(studyInstanceUID))
  {
    return this->SeriesModels.value(studyInstanceUID);
  }

  ctkDICOMSeriesModel* seriesModel = new ctkDICOMSeriesModel(q);
  seriesModel->setDicomDatabase(this->DicomDatabase);
  seriesModel->setScheduler(this->Scheduler);
  seriesModel->setModalityFilter(this->ModalityFilter);
  seriesModel->setSeriesDescriptionFilter(this->SeriesDescriptionFilter);
  seriesModel->setAllowedServers(this->AllowedServers);
  seriesModel->setThumbnailSize(this->ThumbnailSize);
  seriesModel->setPatientID(this->PatientID);
  seriesModel->setStudyFilter(studyInstanceUID);
  this->SeriesModels.insert(studyInstanceUID, seriesModel);

  ctkDICOMSeriesFilterProxyModel* seriesFilterProxyModel = new ctkDICOMSeriesFilterProxyModel(q);
  seriesFilterProxyModel->setSourceModel(seriesModel);
  this->SeriesFilterProxyModels.insert(studyInstanceUID, seriesFilterProxyModel);

  emit q->seriesModelCreated(studyInstanceUID, seriesModel);
  return seriesModel;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModelPrivate::updateSeriesModelFilters(ctkDICOMSeriesModel* seriesModel)
{
  if (!seriesModel)
  {
    return;
  }

  // Apply modality filter (this affects series filtering)
  seriesModel->setModalityFilter(this->ModalityFilter);
  // Apply series description filter
  seriesModel->setSeriesDescriptionFilter(this->SeriesDescriptionFilter);
}

//------------------------------------------------------------------------------
ctkDICOMStudyModel::ctkDICOMStudyModel(QObject* parent)
  : QAbstractListModel(parent)
  , d_ptr(new ctkDICOMStudyModelPrivate(*this))
{
}

//------------------------------------------------------------------------------
ctkDICOMStudyModel::~ctkDICOMStudyModel()
{
}

//------------------------------------------------------------------------------
int ctkDICOMStudyModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  Q_D(const ctkDICOMStudyModel);
  return d->Studies.size();
}

//------------------------------------------------------------------------------
QVariant ctkDICOMStudyModel::data(const QModelIndex& index, int role) const
{
  Q_D(const ctkDICOMStudyModel);

  if (!index.isValid() || index.row() >= d->Studies.size())
  {
    return QVariant();
  }

  const ctkDICOMStudyModelPrivate::StudyData& study = d->Studies.at(index.row());

  switch (role)
  {
    case Qt::DisplayRole:
      return study.studyDescription.isEmpty() ?
             QString("Study %1").arg(study.studyInstanceUID) : study.studyDescription;

    case Qt::ToolTipRole:
    {
      QStringList infoParts;
      if (!study.patientName.isEmpty())
      {
        infoParts << QString("Patient name: %1").arg(study.patientName);
      }
      if (!study.patientID.isEmpty())
      {
        infoParts << QString("Patient MRN: %1").arg(study.patientID);
      }
      if (!study.patientBirthDate.isEmpty())
      {
        infoParts << QString("Patient birth date: %1").arg(study.patientBirthDate);
      }
      if (!study.studyInstanceUID.isEmpty())
      {
        infoParts << QString("StudyInstanceUID: %1").arg(study.studyInstanceUID);
      }
      if (!study.studyDate.isEmpty())
      {
        infoParts << QString("Date: %1").arg(d->formatDate(study.studyDate));
      }
      if (!study.studyTime.isEmpty())
      {
        infoParts << QString("Time: %1").arg(d->formatTime(study.studyTime));
      }
      if (!study.studyDescription.isEmpty())
      {
        infoParts << QString("Description: %1").arg(study.studyDescription);
      }
      if (!study.modalitiesInStudy.isEmpty())
      {
        infoParts << QString("Modalities: %1").arg(study.modalitiesInStudy);
      }
      infoParts << QString("Number of series: %1").arg(study.seriesCount);
      infoParts << QString("Number of filtered series: %1").arg(study.filteredSeriesCount);

      return infoParts.join("\n");
    }

    case StudyInstanceUIDRole:
      return study.studyInstanceUID;

    case StudyIDRole:
      return study.studyID;

    case StudyDescriptionRole:
      return study.studyDescription;

    case StudyDateRole:
      return study.studyDate;

    case StudyTimeRole:
      return study.studyTime;

    case AccessionNumberRole:
      return study.accessionNumber;

    case ModalitiesInStudyRole:
      return study.modalitiesInStudy.split("\\");

    case SeriesCountRole:
      return study.seriesCount;

    case FilteredSeriesCountRole:
      return study.filteredSeriesCount;

    case IsCollapsedRole:
      return study.isCollapsed;

    case IsVisibleRole:
      return study.isVisible;

    case OperationStatusRole:
      return static_cast<int>(study.operationStatus);

    case StoppedJobUIDRole:
      return study.stoppedJobUID;

    case PatientUIDRole:
      return study.patientUID;

    case PatientIDRole:
      return study.patientID;

    case PatientNameRole:
      return study.patientName;

    case PatientBirthDateRole:
      return study.patientBirthDate;

    default:
      return QVariant();
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  Q_D(ctkDICOMStudyModel);

  if (!index.isValid() || index.row() >= d->Studies.size())
  {
    return false;
  }

  ctkDICOMStudyModelPrivate::StudyData& study = d->Studies[index.row()];
  bool changed = false;

  switch (role)
  {
    case Qt::DisplayRole:
    {
      // Not stored, just for display
      break;
    }
    case StudyDescriptionRole:
    {
      if (study.studyDescription != value.toString())
      {
        study.studyDescription = value.toString();
        changed = true;
      }
      break;
    }
    case StudyInstanceUIDRole:
    {
      if (study.studyInstanceUID != value.toString())
      {
        study.studyInstanceUID = value.toString();
        changed = true;
      }
      break;
    }
    case StudyIDRole:
    {
      if (study.studyID != value.toString())
      {
        study.studyID = value.toString();
        changed = true;
      }
      break;
    }
    case StudyDateRole:
    {
      if (study.studyDate != value.toString())
      {
        study.studyDate = value.toString();
        changed = true;
      }
      break;
    }
    case StudyTimeRole:
    {
      if (study.studyTime != value.toString())
      {
        study.studyTime = value.toString();
        changed = true;
      }
      break;
    }
    case AccessionNumberRole:
    {
      if (study.accessionNumber != value.toString())
      {
        study.accessionNumber = value.toString();
        changed = true;
      }
      break;
    }
    case ModalitiesInStudyRole:
    {
      if (study.modalitiesInStudy != value.toStringList().join("\\"))
      {
        study.modalitiesInStudy = value.toStringList().join("\\");
        changed = true;
      }
      break;
    }
    case SeriesCountRole:
    {
      if (study.seriesCount != value.toInt())
      {
        study.seriesCount = value.toInt();
        changed = true;
      }
      break;
    }
    case FilteredSeriesCountRole:
    {
      if (study.filteredSeriesCount != value.toInt())
      {
        study.filteredSeriesCount = value.toInt();
        changed = true;
      }
      break;
    }
    case IsCollapsedRole:
    {
      if (study.isCollapsed != value.toBool())
      {
        study.isCollapsed = value.toBool();
        changed = true;
      }
      break;
    }
    case IsVisibleRole:
    {
      if (study.isVisible != value.toBool())
      {
        study.isVisible = value.toBool();
        changed = true;
      }
      break;
    }
    case OperationStatusRole:
    {
      OperationStatus newStatus = static_cast<OperationStatus>(value.toInt());
      if (study.operationStatus != newStatus)
      {
        study.operationStatus = newStatus;
        changed = true;
      }
      break;
    }
    case StoppedJobUIDRole:
    {
      QString newJobUID = value.toString();
      if (study.stoppedJobUID != newJobUID)
      {
        study.stoppedJobUID = newJobUID;
        changed = true;
      }
      break;
    }
    case PatientIDRole:
    {
      if (study.patientID != value.toString())
      {
        study.patientID = value.toString();
        changed = true;
      }
      break;
    }
    case PatientNameRole:
    {
      if (study.patientName != value.toString())
      {
        study.patientName = value.toString();
        changed = true;
      }
      break;
    }
    case PatientBirthDateRole:
    {
      if (study.patientBirthDate != value.toString())
      {
        study.patientBirthDate = value.toString();
        changed = true;
      }
      break;
    }
    default:
      return false;
  }

  if (changed)
  {
    emit this->dataChanged(index, index, QVector<int>() << role);
  }

  return changed;
}

//------------------------------------------------------------------------------
Qt::ItemFlags ctkDICOMStudyModel::flags(const QModelIndex& index) const
{
  Q_UNUSED(index);
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMStudyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(section);
  Q_UNUSED(orientation);
  Q_UNUSED(role);
  return QVariant();
}

//------------------------------------------------------------------------------
QHash<int, QByteArray> ctkDICOMStudyModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[Qt::DisplayRole] = "display";
  roles[Qt::ToolTipRole] = "toolTip";
  roles[StudyInstanceUIDRole] = "studyInstanceUID";
  roles[StudyIDRole] = "studyID";
  roles[StudyDescriptionRole] = "studyDescription";
  roles[StudyDateRole] = "studyDate";
  roles[StudyTimeRole] = "studyTime";
  roles[AccessionNumberRole] = "accessionNumber";
  roles[ModalitiesInStudyRole] = "modalitiesInStudy";
  roles[SeriesCountRole] = "seriesCount";
  roles[FilteredSeriesCountRole] = "filteredSeriesCount";
  roles[IsCollapsedRole] = "isCollapsed";
  roles[IsVisibleRole] = "isVisible";
  roles[OperationStatusRole] = "operationStatus";
  roles[StoppedJobUIDRole] = "stoppedJobUID";
  roles[PatientUIDRole] = "patientUID";
  roles[PatientIDRole] = "patientID";
  roles[PatientNameRole] = "patientName";
  return roles;
}

//------------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMStudyModel::dicomDatabase() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->DicomDatabase.data();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMStudyModel::dicomDatabaseShared() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->DicomDatabase;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setDicomDatabase(ctkDICOMDatabase& database)
{
  Q_D(ctkDICOMStudyModel);
  d->DicomDatabase = QSharedPointer<ctkDICOMDatabase>(&database, skipDelete);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setDicomDatabase(QSharedPointer<ctkDICOMDatabase> database)
{
  Q_D(ctkDICOMStudyModel);
  d->DicomDatabase = database;
}

//------------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMStudyModel::scheduler() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->Scheduler.data();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMStudyModel::schedulerShared() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->Scheduler;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setScheduler(ctkDICOMScheduler& scheduler)
{
  Q_D(ctkDICOMStudyModel);
  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMStudyModel);
  d->Scheduler = scheduler;
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModel::patientUID() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->PatientUID;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setPatientUID(const QString& patientUID)
{
  Q_D(ctkDICOMStudyModel);

  if (d->PatientUID == patientUID)
  {
    return;
  }

  d->PatientUID = patientUID;
  this->refresh();
  emit this->patientUIDChanged(patientUID);
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModel::patientID() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->PatientID;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setPatientID(const QString& patientID)
{
  Q_D(ctkDICOMStudyModel);

  if (d->PatientID == patientID)
  {
    return;
  }

  d->PatientID = patientID;
  emit this->patientIDChanged(patientID);
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModel::studyDescriptionFilter() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->StudyDescriptionFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setStudyDescriptionFilter(const QString& description)
{
  Q_D(ctkDICOMStudyModel);

  if (d->StudyDescriptionFilter == description)
  {
    return;
  }

  d->StudyDescriptionFilter = description;
  this->refreshStudies();
  emit this->studyDescriptionFilterChanged(description);
}

//------------------------------------------------------------------------------
ctkDICOMStudyModel::DateType ctkDICOMStudyModel::dateFilter() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->DateFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setDateFilter(DateType dateType)
{
  Q_D(ctkDICOMStudyModel);

  if (d->DateFilter == dateType)
  {
    return;
  }

  d->DateFilter = dateType;
  this->refreshStudies();
  emit this->dateFilterChanged(dateType);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setCustomDateRange(const QDate& startDate, const QDate& endDate)
{
  Q_D(ctkDICOMStudyModel);

  if (d->CustomStartDate == startDate && d->CustomEndDate == endDate)
  {
    return;
  }

  d->CustomStartDate = startDate;
  d->CustomEndDate = endDate;

  if (d->DateFilter == DateType::CustomRange)
  {
    this->refreshStudies();
  }
}

//------------------------------------------------------------------------------
QDate ctkDICOMStudyModel::customStartDate() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->CustomStartDate;
}

//------------------------------------------------------------------------------
QDate ctkDICOMStudyModel::customEndDate() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->CustomEndDate;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyModel::modalityFilter() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->ModalityFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setModalityFilter(const QStringList& modalities)
{
  Q_D(ctkDICOMStudyModel);

  if (d->ModalityFilter == modalities)
  {
    return;
  }

  d->ModalityFilter = modalities;
  this->updateSeriesModelsFilters();
  this->refreshStudies();
  emit this->modalityFilterChanged(modalities);
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModel::seriesDescriptionFilter() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->SeriesDescriptionFilter;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setSeriesDescriptionFilter(const QString& description)
{
  Q_D(ctkDICOMStudyModel);

  if (d->SeriesDescriptionFilter == description)
  {
    return;
  }

  d->SeriesDescriptionFilter = description;
  this->updateSeriesModelsFilters();
  this->refreshStudies();
  emit this->seriesDescriptionFilterChanged(description);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyModel::allowedServers() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->AllowedServers;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setAllowedServers(const QStringList& servers)
{
  Q_D(ctkDICOMStudyModel);

  if (d->AllowedServers == servers)
  {
    return;
  }

  d->AllowedServers = servers;
  emit allowedServersChanged(servers);

  foreach (ctkDICOMSeriesModel* seriesModel, d->SeriesModels.values())
  {
    if (seriesModel)
    {
      seriesModel->setAllowedServers(d->AllowedServers);
    }
  }
}

//------------------------------------------------------------------------------
int ctkDICOMStudyModel::numberOfOpenedStudies() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->NumberOfOpenedStudies;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setNumberOfOpenedStudies(int count)
{
  Q_D(ctkDICOMStudyModel);

  if (d->NumberOfOpenedStudies == count)
  {
    return;
  }

  d->NumberOfOpenedStudies = count;

  // Emit signal to let views handle the collapsed state based on their proxy models
  emit this->numberOfOpenedStudiesChanged(count);
}

//------------------------------------------------------------------------------
int ctkDICOMStudyModel::thumbnailSize() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->ThumbnailSize;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setThumbnailSize(int size)
{
  Q_D(ctkDICOMStudyModel);

  if (d->ThumbnailSize == size)
  {
    return;
  }

  d->ThumbnailSize = size;
  foreach (ctkDICOMSeriesModel* seriesModel, d->SeriesModels.values())
  {
    if (seriesModel)
    {
      seriesModel->setThumbnailSize(size);
    }
  }

  this->refreshStudies();
  emit this->thumbnailSizeChanged(size);
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModel::studyInstanceUID(const QModelIndex& index) const
{
  return this->data(index, this->StudyInstanceUIDRole).toString();
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyModel::studyID(const QModelIndex& index) const
{
  return this->data(index, this->StudyIDRole).toString();
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMStudyModel::indexFromStudyInstanceUID(const QString& studyInstanceUID) const
{
  Q_D(const ctkDICOMStudyModel);

  for (int row = 0; row < d->Studies.size(); ++row)
  {
    if (d->Studies[row].studyInstanceUID == studyInstanceUID)
    {
      return this->index(row);
    }
  }

  return QModelIndex();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setAllStudiesCollapsed(bool collapsed)
{
  Q_D(ctkDICOMStudyModel);
  if (d->Studies.size() > 0)
  {
    QModelIndex topLeft = this->index(0);
    QModelIndex bottomRight = this->index(d->Studies.size() - 1);
    for (int row = 0; row < d->Studies.size(); ++row)
    {
      d->Studies[row].isCollapsed = collapsed;
    }
    emit this->dataChanged(topLeft, bottomRight, QVector<int>() << IsCollapsedRole);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setStudyCollapsed(const QModelIndex& index, bool collapsed)
{
  Q_D(const ctkDICOMStudyModel);
  if (!index.isValid() || index.row() >= this->rowCount())
  {
    return;
  }
  if (this->isStudyCollapsed(index) == collapsed)
  {
    return;
  }

  this->setData(index, collapsed, IsCollapsedRole);
  ctkDICOMSeriesModel* seriesModel = this->seriesModelForStudy(index);
  if (!seriesModel)
  {
    return;
  }

  if (!collapsed)
  {
    seriesModel->setAutoGenerateThumbnails(true);
    seriesModel->generateThumbnails();
    if (!d->AllowedServers.isEmpty())
    {
      seriesModel->retrieveAllSeries();
    }
  }
  else
  {
    seriesModel->setAutoGenerateThumbnails(false);
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModel::isStudyCollapsed(const QModelIndex& index) const
{
  if (!index.isValid() || index.row() >= this->rowCount())
  {
    return false;
  }
  return this->data(index, IsCollapsedRole).toBool();
}

//------------------------------------------------------------------------------
int ctkDICOMStudyModel::daysFromDateFilter(DateType dateFilter)
{
  switch (dateFilter)
  {
    case Today:
      return 0;
    case Yesterday:
      return 1;
    case LastWeek:
      return 7;
    case LastMonth:
      return 30;
    case LastYear:
      return 365;
    default:
      return -1; // Any
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModel::studyMatchesFilters(const QString& studyInstanceUID) const
{
  Q_D(const ctkDICOMStudyModel);

  for (int row = 0; row < d->Studies.size(); ++row)
  {
    if (d->Studies[row].studyInstanceUID == studyInstanceUID)
    {
      const ctkDICOMStudyModelPrivate::StudyData& study = d->Studies.at(row);
      return d->matchesDateFilter(study.studyDate) &&
             d->matchesDescriptionFilter(study.studyDescription);
    }
  }

  return false;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyModel::studyInstanceUIDs() const
{
  Q_D(const ctkDICOMStudyModel);

  QStringList studyUIDs;
  for (const ctkDICOMStudyModelPrivate::StudyData& study : d->Studies)
  {
    studyUIDs.append(study.studyInstanceUID);
  }

  return studyUIDs;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyModel::filteredStudyInstanceUIDs(bool filterSeries) const
{
  Q_D(const ctkDICOMStudyModel);

  QStringList filteredUIDs;
  for (const ctkDICOMStudyModelPrivate::StudyData& study : d->Studies)
  {
    if (d->matchesDateFilter(study.studyDate) &&
        d->matchesDescriptionFilter(study.studyDescription))
    {
      if (filterSeries)
      {
        int filteredSeriesCount = d->getFilteredSeriesCountForStudy(study.studyInstanceUID);
        if (filteredSeriesCount > 0)
        {
          filteredUIDs.append(study.studyInstanceUID);
        }
      }
      else
      {
        filteredUIDs.append(study.studyInstanceUID);
      }
    }
  }

  return filteredUIDs;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::refresh()
{
  Q_D(ctkDICOMStudyModel);
  d->populateStudies();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::clean()
{
  Q_D(ctkDICOMStudyModel);
  d->clean();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::refreshStudies()
{
  Q_D(ctkDICOMStudyModel);
  // Update all studies in batch and emit dataChanged only once
  if (d->Studies.isEmpty())
  {
    return;
  }

  // Update all study data in-place
  for (int studyIndex = 0; studyIndex < d->Studies.size(); ++studyIndex)
  {
    d->updateStudyCountsAndVisibility(studyIndex);
  }

  // Emit dataChanged for the whole range
  QModelIndex top = this->index(0, 0);
  QModelIndex bottom = this->index(d->Studies.size() - 1, 0);
  emit dataChanged(top, bottom, QVector<int>()
    << ctkDICOMStudyModel::SeriesCountRole
    << ctkDICOMStudyModel::FilteredSeriesCountRole
    << ctkDICOMStudyModel::IsVisibleRole
  );
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::refreshStudy(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMStudyModel);

  QModelIndex idx = this->indexFromStudyInstanceUID(studyInstanceUID);
  if (!idx.isValid())
  {
    return;
  }

  int studyIndex = idx.row();
  d->updateStudyCountsAndVisibility(studyIndex);

  emit this->dataChanged(idx, idx, QVector<int>() << SeriesCountRole << FilteredSeriesCountRole << IsVisibleRole);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::forceUpdateStudyJobs(const QString &studyInstanceUID)
{
  Q_D(const ctkDICOMStudyModel);
  if (!d->Scheduler)
  {
    return;
  }

  QList<QSharedPointer<ctkAbstractJob>> runningJobs = d->Scheduler->getJobsByDICOMUIDs({}, {studyInstanceUID});
  QList<ctkAbstractJob::JobStatus> statusFilters =
    {
      ctkAbstractJob::JobStatus::UserStopped,
      ctkAbstractJob::JobStatus::Failed
    };
  QList<QSharedPointer<ctkAbstractJob>> failedJobs = d->Scheduler->getJobsByDICOMUIDs({}, {studyInstanceUID}, {}, {}, statusFilters);

  if (!runningJobs.isEmpty())
  {
    // Stop running or waiting jobs
    QStringList jobUIDs;
    // Restart the failed job
    for (QSharedPointer<ctkAbstractJob> job : runningJobs)
    {
      jobUIDs.append(job->jobUID());
    }
    d->Scheduler->stopJobsByJobUIDs(jobUIDs);
  }
  else if (!failedJobs.isEmpty())
  {
    QStringList jobUIDs;
    // Restart the failed job
    for (QSharedPointer<ctkAbstractJob> job : failedJobs)
    {
      jobUIDs.append(job->jobUID());
    }
    d->Scheduler->retryJobs(jobUIDs);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::updateGUIFromScheduler(const QVariant& data)
{
  Q_D(const ctkDICOMStudyModel);
  if (d->IsUpdating || !d->Scheduler)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty())
  {
    return;
  }

  if (td.PatientID != "" && td.PatientID != d->PatientID)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
  {
    if (td.QueriedStudyInstanceUIDs.isEmpty())
    {
      return;
    }
    this->refresh();

    for (const QString& studyInstanceUID : td.QueriedStudyInstanceUIDs)
    {
      ctkDICOMSeriesModel* seriesModel = this->seriesModelForStudyInstanceUID(studyInstanceUID);
      if (!seriesModel)
      {
        continue;
      }
      d->Scheduler->querySeries(d->PatientID,
                                studyInstanceUID,
                                QThread::NormalPriority,
                                d->AllowedServers);
    }
  }

  QModelIndex idx = this->indexFromStudyInstanceUID(td.StudyInstanceUID);
  bool studyIsCollapsed = this->data(idx, ctkDICOMStudyModel::IsCollapsedRole).toBool();

  ctkDICOMSeriesModel* seriesModel = this->seriesModelForStudyInstanceUID(td.StudyInstanceUID);
  if (!seriesModel)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries &&
      td.QueriedSeriesInstanceUIDs.isEmpty())
  {
    this->setData(idx, int(OperationStatus::Failed), ctkDICOMStudyModel::OperationStatusRole);
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries||
      td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::ThumbnailGenerator ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    seriesModel->updateGUIFromScheduler(data, studyIsCollapsed);
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances)
  {
    this->refreshStudy(td.StudyInstanceUID);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::onJobStarted(const QVariant& data)
{
  Q_D(const ctkDICOMStudyModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.StudyInstanceUID.isEmpty())
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    QModelIndex idx = this->indexFromStudyInstanceUID(td.StudyInstanceUID);
    this->setData(idx, int(OperationStatus::InProgress), ctkDICOMStudyModel::OperationStatusRole);
  }

  if ((td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
       td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries) &&
      !d->AllowedServers.contains(td.ConnectionName))
  {
    return;
  }

  ctkDICOMSeriesModel* seriesModel = this->seriesModelForStudyInstanceUID(td.StudyInstanceUID);
  if (seriesModel)
  {
    seriesModel->onJobStarted(data);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::onJobFinished(const QVariant& data)
{
  Q_D(const ctkDICOMStudyModel);
  if (d->IsUpdating || !d->Scheduler)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.StudyInstanceUID.isEmpty())
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    QModelIndex idx = this->indexFromStudyInstanceUID(td.StudyInstanceUID);
    this->setData(idx, int(OperationStatus::Completed), ctkDICOMStudyModel::OperationStatusRole);
  }

  if ((td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
       td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries) &&
      d->Scheduler->serverHasProxy(td.ConnectionName))
  {
    return;
  }

  ctkDICOMSeriesModel* seriesModel = this->seriesModelForStudyInstanceUID(td.StudyInstanceUID);
  if (seriesModel)
  {
    seriesModel->onJobFinished(data);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::onJobFailed(const QVariant& data)
{
  Q_D(const ctkDICOMStudyModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.StudyInstanceUID.isEmpty())
    {
      return;
    }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    QModelIndex idx = this->indexFromStudyInstanceUID(td.StudyInstanceUID);
    this->setData(idx, int(OperationStatus::Failed), ctkDICOMStudyModel::OperationStatusRole);
  }

  ctkDICOMSeriesModel* seriesModel = this->seriesModelForStudyInstanceUID(td.StudyInstanceUID);
  if (seriesModel)
  {
    seriesModel->onJobFailed(data);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::onJobUserStopped(const QVariant& data)
{
  Q_D(const ctkDICOMStudyModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.StudyInstanceUID.isEmpty())
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    QModelIndex idx = this->indexFromStudyInstanceUID(td.StudyInstanceUID);
    this->setData(idx, int(OperationStatus::Failed), ctkDICOMStudyModel::OperationStatusRole);
  }

  ctkDICOMSeriesModel* seriesModel = this->seriesModelForStudyInstanceUID(td.StudyInstanceUID);
  if (seriesModel)
  {
    seriesModel->onJobUserStopped(data);
  }
}

//------------------------------------------------------------------------------
ctkDICOMSeriesModel* ctkDICOMStudyModel::seriesModelForStudy(const QModelIndex& studyIndex)
{
  QString studyInstanceUID = this->studyInstanceUID(studyIndex);
  return this->seriesModelForStudyInstanceUID(studyInstanceUID);
}

//------------------------------------------------------------------------------
ctkDICOMSeriesModel* ctkDICOMStudyModel::seriesModelForStudyInstanceUID(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMStudyModel);

  if (studyInstanceUID.isEmpty())
  {
    return nullptr;
  }

  return d->SeriesModels.value(studyInstanceUID, nullptr);
}

//------------------------------------------------------------------------------
ctkDICOMSeriesFilterProxyModel* ctkDICOMStudyModel::seriesFilterProxyModelForStudy(const QModelIndex& studyIndex)
{
  QString studyInstanceUID = this->studyInstanceUID(studyIndex);
  return this->seriesFilterProxyModelForStudyInstanceUID(studyInstanceUID);
}

//------------------------------------------------------------------------------
ctkDICOMSeriesFilterProxyModel* ctkDICOMStudyModel::seriesFilterProxyModelForStudyInstanceUID(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMStudyModel);

  if (studyInstanceUID.isEmpty())
  {
    return nullptr;
  }

  return d->SeriesFilterProxyModels.value(studyInstanceUID, nullptr);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModel::hasSeriesModel(const QModelIndex& studyIndex) const
{
  QString studyInstanceUID = this->studyInstanceUID(studyIndex);
  return this->hasSeriesModel(studyInstanceUID);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModel::hasSeriesModel(const QString& studyInstanceUID) const
{
  Q_D(const ctkDICOMStudyModel);
  return d->SeriesModels.contains(studyInstanceUID);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::removeSeriesModel(const QModelIndex& studyIndex)
{
  QString studyInstanceUID = this->studyInstanceUID(studyIndex);
  this->removeSeriesModel(studyInstanceUID);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::removeSeriesModel(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMStudyModel);

  if (d->SeriesFilterProxyModels.contains(studyInstanceUID))
  {
    ctkDICOMSeriesFilterProxyModel* model = d->SeriesFilterProxyModels.take(studyInstanceUID);
    model->deleteLater();
  }

  if (d->SeriesModels.contains(studyInstanceUID))
  {
    ctkDICOMSeriesModel* model = d->SeriesModels.take(studyInstanceUID);
    model->deleteLater();
    emit seriesModelRemoved(studyInstanceUID);
  }
}

//------------------------------------------------------------------------------
QList<ctkDICOMSeriesModel*> ctkDICOMStudyModel::allSeriesModels() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->SeriesModels.values();
}

//------------------------------------------------------------------------------
QList<ctkDICOMSeriesFilterProxyModel*> ctkDICOMStudyModel::allSeriesFilterProxyModels() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->SeriesFilterProxyModels.values();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::updateSeriesModelsFilters()
{
  Q_D(ctkDICOMStudyModel);

  // Update all existing series models with current filters
  foreach (ctkDICOMSeriesModel* seriesModel, d->SeriesModels.values())
  {
    if (seriesModel)
    {
      d->updateSeriesModelFilters(seriesModel);
    }
  }

  d->updateFilteredSeriesCounts();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::refreshSeriesModels()
{
  Q_D(ctkDICOMStudyModel);

  foreach (ctkDICOMSeriesModel* seriesModel, d->SeriesModels.values())
  {
    if (seriesModel)
    {
      seriesModel->refreshSeriesList();
    }
  }
}
