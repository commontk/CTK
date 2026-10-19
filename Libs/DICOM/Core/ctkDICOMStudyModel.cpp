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
#include <QRegularExpression>
#include <QStringList>

// CTK includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModalities.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMServer.h"
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
  struct StudyData;
  /// The study matches the study filters (date and description)
  bool matchesStudyFilters(const StudyData& study) const;
  /// The study is shown: it matches the study filters, and its series match the series
  /// filters. When its series are not known yet (e.g. a study found by a query), the
  /// modality filter is checked on the modalities of the study, and a series description
  /// filter, which cannot be checked, hides it until its series are known.
  bool isStudyVisible(const StudyData& study, int seriesCount, int filteredSeriesCount) const;
  int getSeriesCountForStudy(const QString& studyInstanceUID) const;
  int getFilteredSeriesCountForStudy(const QString& studyInstanceUID) const;
  QString getPatientUIDFromPatientID(const QString& patientID) const;
  void updateFilteredSeriesCounts();
  void updateStudyCountsAndVisibility(int studyIndex);
  void clean();

  /// Sort the given studies by date and time, the most recent first
  QStringList sortStudyInstanceUIDsByDate(const QStringList& studyInstanceUIDs) const;
  /// All the studies sorted by date and time, the most recent first. The order is
  /// cached until studies are added or removed: it was computed again for each series
  /// query, which took a while for a patient with hundreds of studies.
  QStringList allStudyInstanceUIDsSortedByDate();

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
    bool isQueryResult;
    ctkDICOMStudyModel::OperationStatus operationStatus;
    QString stoppedJobUID;
  };

  QList<StudyData> Studies;
  QStringList StudyInstanceUIDsSortedByDate;
  bool StudyInstanceUIDsSortedByDateNeedUpdate;

  // Series model management
  QHash<QString, ctkDICOMSeriesModel*> SeriesModels; // StudyInstanceUID -> SeriesModel
  QHash<QString, ctkDICOMSeriesFilterProxyModel*> SeriesFilterProxyModels; // StudyInstanceUID -> SeriesFilterProxyModel
  ctkDICOMSeriesModel* createSeriesModel(const QString& studyInstanceUID);
  /// Filter proxy model of the series model of a study, created the first time it is
  /// asked for: only the studies that a view shows need one, and creating one for
  /// each study of each patient slowed down the population of the models.
  ctkDICOMSeriesFilterProxyModel* seriesFilterProxyModel(const QString& studyInstanceUID);
  void updateSeriesModelFilters(ctkDICOMSeriesModel* seriesModel);
  /// True if the jobs of the server \a connectionName are reported for the patient: an
  /// allowed server, or the proxy of an allowed server (which retrieves its series
  /// with C-GET after their C-MOVE, without being a server of the scheduler).
  bool isAllowedServerOrProxy(const QString& connectionName) const;

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
  bool AutoOpenStudiesEnabled;
  bool AutoRetrieveFullSeries;
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
  this->AutoOpenStudiesEnabled = true;
  this->AutoRetrieveFullSeries = true;
  this->ThumbnailSize = 128;
  this->IsUpdating = false;
  this->ModalityFilter = ctkDICOMModalities::AllModalities;
  this->StudyInstanceUIDsSortedByDateNeedUpdate = true;
}

//------------------------------------------------------------------------------
ctkDICOMStudyModelPrivate::~ctkDICOMStudyModelPrivate()
{
  // Clean up series models
  for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = this->SeriesModels.begin();
       it != this->SeriesModels.end(); ++it)
  {
    delete it.value();
  }
  this->SeriesModels.clear();
  for (QHash<QString, ctkDICOMSeriesFilterProxyModel*>::iterator it = this->SeriesFilterProxyModels.begin();
       it != this->SeriesFilterProxyModels.end(); ++it)
  {
    delete it.value();
  }
  this->SeriesFilterProxyModels.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModelPrivate::clean()
{
  Q_Q(ctkDICOMStudyModel);
  this->IsUpdating = true;

  // Delete series models and proxy models
  for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = this->SeriesModels.begin();
       it != this->SeriesModels.end(); ++it)
  {
    delete it.value();
  }
  this->SeriesModels.clear();
  for (QHash<QString, ctkDICOMSeriesFilterProxyModel*>::iterator it = this->SeriesFilterProxyModels.begin();
       it != this->SeriesFilterProxyModels.end(); ++it)
  {
    delete it.value();
  }
  this->SeriesFilterProxyModels.clear();

  // Clear the studies list
  q->beginResetModel();
  this->Studies.clear();
  this->StudyInstanceUIDsSortedByDateNeedUpdate = true;
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
      for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = this->SeriesModels.begin();
           it != this->SeriesModels.end(); ++it)
      {
        delete it.value();
      }
      this->SeriesModels.clear();
      for (QHash<QString, ctkDICOMSeriesFilterProxyModel*>::iterator it = this->SeriesFilterProxyModels.begin();
           it != this->SeriesFilterProxyModels.end(); ++it)
      {
        delete it.value();
      }
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
      for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = this->SeriesModels.begin();
           it != this->SeriesModels.end(); ++it)
      {
        delete it.value();
      }
      this->SeriesModels.clear();
      for (QHash<QString, ctkDICOMSeriesFilterProxyModel*>::iterator it = this->SeriesFilterProxyModels.begin();
           it != this->SeriesFilterProxyModels.end(); ++it)
      {
        delete it.value();
      }
      this->SeriesFilterProxyModels.clear();
      q->endResetModel();
    }
    this->IsUpdating = false;
    emit q->modelRefreshed();
    return;
  }

  // Index of the existing studies, for quick lookup
  QHash<QString, int> existingStudyIndexes;
  for (int index = 0; index < this->Studies.size(); ++index)
  {
    existingStudyIndexes.insert(this->Studies[index].studyInstanceUID, index);
  }

  // Create a set of database study UIDs that match filters
  QSet<QString> validStudyUIDs;
  QList<StudyData> newStudies;

  QString patientID = this->PatientID;
  QString patientName = this->DicomDatabase->fieldForPatient("PatientsName", this->PatientUID);
  patientName.replace(R"(^)", R"( )");
  QString patientBirthDate = this->DicomDatabase->fieldForPatient("PatientsBirthDate", this->PatientUID);
  // Fix YYYY-MM-DD format in YYYYMMDD
  patientBirthDate.remove('-');

  foreach (const QString& studyInstanceUID, studiesList)
  {
    this->createSeriesModel(studyInstanceUID);
    validStudyUIDs.insert(studyInstanceUID);

    int filteredSeriesCount = this->getFilteredSeriesCountForStudy(studyInstanceUID);
    int seriesCount = this->getSeriesCountForStudy(studyInstanceUID);
    // A study found by a query has no series until they are queried
    bool isQueryResult = (seriesCount == 0);

    // Only add if it's a new study
    const int existingIndex = existingStudyIndexes.value(studyInstanceUID, -1);
    if (existingIndex < 0)
    {
      StudyData study;
      study.studyInstanceUID = studyInstanceUID;
      study.studyID = this->DicomDatabase->fieldForStudy("StudyID", studyInstanceUID);
      study.studyDescription = this->DicomDatabase->fieldForStudy("StudyDescription", studyInstanceUID);
      study.studyDate = this->DicomDatabase->fieldForStudy("StudyDate", studyInstanceUID);
      // Fix YYYY-MM-DD format in YYYYMMDD
      study.studyDate.remove('-');
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
      study.isVisible = this->isStudyVisible(study, seriesCount, filteredSeriesCount);
      study.isQueryResult = isQueryResult;
      study.operationStatus = ctkDICOMStudyModel::NoOperation;
      study.stoppedJobUID = "";
      study.isCollapsed = true;

      newStudies.append(study);
    }
    else
    {
      // Update series counts for existing studies
      StudyData& study = this->Studies[existingIndex];
      const bool isVisible = this->isStudyVisible(study, seriesCount, filteredSeriesCount);
      // Only update and emit dataChanged if something actually changed
      bool hasChanges = (study.seriesCount != seriesCount ||
                         study.filteredSeriesCount != filteredSeriesCount ||
                         study.isVisible != isVisible);

      if (hasChanges)
      {
        study.seriesCount = seriesCount;
        study.filteredSeriesCount = filteredSeriesCount;
        study.isVisible = isVisible;

        // Emit data changed for updated counts and visibility
        QModelIndex idx = q->index(existingIndex);
        emit q->dataChanged(idx, idx, QVector<int>() <<
          ctkDICOMStudyModel::SeriesCountRole <<
          ctkDICOMStudyModel::FilteredSeriesCountRole <<
          ctkDICOMStudyModel::IsVisibleRole);
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
    this->StudyInstanceUIDsSortedByDateNeedUpdate = true;

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
    this->StudyInstanceUIDsSortedByDateNeedUpdate = true;
    q->endInsertRows();
  }

  this->IsUpdating = false;

  emit q->modelRefreshed();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyModelPrivate::sortStudyInstanceUIDsByDate(const QStringList& studyInstanceUIDs) const
{
  QHash<QString, int> rowsByStudyInstanceUID;
  for (int row = 0; row < this->Studies.size(); ++row)
  {
    rowsByStudyInstanceUID.insert(this->Studies[row].studyInstanceUID, row);
  }

  QList<QPair<QDateTime, QString>> studyDateTimePairs;
  for (const QString& studyInstanceUID : studyInstanceUIDs)
  {
    QDateTime studyDateTime;
    int row = rowsByStudyInstanceUID.value(studyInstanceUID, -1);
    if (row >= 0 && !this->Studies[row].studyDate.isEmpty())
    {
      QString dateTimeStr = this->Studies[row].studyDate + this->Studies[row].studyTime;
      studyDateTime = QDateTime::fromString(dateTimeStr, "yyyyMMddHHmmss");
    }
    studyDateTimePairs.append(qMakePair(studyDateTime, studyInstanceUID));
  }

  // Most recent first
  std::sort(studyDateTimePairs.begin(), studyDateTimePairs.end(),
            [](const QPair<QDateTime, QString>& a, const QPair<QDateTime, QString>& b) {
              return a.first > b.first;
            });

  QStringList sortedStudyInstanceUIDs;
  for (const QPair<QDateTime, QString>& pair : std::as_const(studyDateTimePairs))
  {
    sortedStudyInstanceUIDs.append(pair.second);
  }
  return sortedStudyInstanceUIDs;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyModelPrivate::allStudyInstanceUIDsSortedByDate()
{
  if (this->StudyInstanceUIDsSortedByDateNeedUpdate)
  {
    QStringList studyInstanceUIDs;
    for (const StudyData& study : std::as_const(this->Studies))
    {
      studyInstanceUIDs.append(study.studyInstanceUID);
    }
    this->StudyInstanceUIDsSortedByDate = this->sortStudyInstanceUIDsByDate(studyInstanceUIDs);
    this->StudyInstanceUIDsSortedByDateNeedUpdate = false;
  }
  return this->StudyInstanceUIDsSortedByDate;
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
      return studyDate.toString("d MMM yyyy");
    }
  }

  // Try YYYY-MM-DD format (10 characters)
  if (date.length() == 10)
  {
    QDate studyDate = QDate::fromString(date, "yyyy-MM-dd");
    if (studyDate.isValid())
    {
      return studyDate.toString("d MMM yyyy");
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

  return seriesModel->rowCount();
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

  return seriesModel->filteredSeriesCount();
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModelPrivate::matchesStudyFilters(const StudyData& study) const
{
  return this->matchesDateFilter(study.studyDate) &&
         this->matchesDescriptionFilter(study.studyDescription);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModelPrivate::isStudyVisible(const StudyData& study, int seriesCount,
                                               int filteredSeriesCount) const
{
  if (!this->matchesStudyFilters(study))
  {
    return false;
  }

  if (seriesCount > 0)
  {
    return filteredSeriesCount > 0;
  }

  // The series are not known yet
  if (!this->SeriesDescriptionFilter.isEmpty())
  {
    return false;
  }
  if (this->ModalityFilter.isEmpty() || this->ModalityFilter.contains("Any"))
  {
    return true;
  }
  const QStringList modalities = study.modalitiesInStudy.split(QRegularExpression("[\\\\,]"), Qt::SkipEmptyParts);
  foreach (const QString& modality, modalities)
  {
    if (this->ModalityFilter.contains(modality.trimmed()))
    {
      return true;
    }
  }
  return false;
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
    study.seriesCount = this->getSeriesCountForStudy(study.studyInstanceUID);
    study.filteredSeriesCount = this->getFilteredSeriesCountForStudy(study.studyInstanceUID);
    study.isVisible = this->isStudyVisible(study, study.seriesCount, study.filteredSeriesCount);
    study.isQueryResult = (study.seriesCount == 0);
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
  study.isVisible = this->isStudyVisible(study, study.seriesCount, study.filteredSeriesCount);
  study.isQueryResult = (study.seriesCount == 0);
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
  seriesModel->setAutoRetrieveFullSeries(this->AutoRetrieveFullSeries);
  seriesModel->setPatientID(this->PatientID);
  seriesModel->setStudyFilter(studyInstanceUID);
  this->SeriesModels.insert(studyInstanceUID, seriesModel);

  emit q->seriesModelCreated(studyInstanceUID, seriesModel);
  return seriesModel;
}

//------------------------------------------------------------------------------
ctkDICOMSeriesFilterProxyModel* ctkDICOMStudyModelPrivate::seriesFilterProxyModel(const QString& studyInstanceUID)
{
  Q_Q(ctkDICOMStudyModel);
  ctkDICOMSeriesFilterProxyModel* seriesFilterProxyModel = this->SeriesFilterProxyModels.value(studyInstanceUID, nullptr);
  ctkDICOMSeriesModel* seriesModel = this->SeriesModels.value(studyInstanceUID, nullptr);
  if (!seriesFilterProxyModel && seriesModel)
  {
    seriesFilterProxyModel = new ctkDICOMSeriesFilterProxyModel(q);
    seriesFilterProxyModel->setSourceModel(seriesModel);
    this->SeriesFilterProxyModels.insert(studyInstanceUID, seriesFilterProxyModel);
  }
  return seriesFilterProxyModel;
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
  QSignalBlocker blocker(this);
  this->clean();
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
        study.studyDate  = value.toString();
        // Fix YYYY-MM-DD format in YYYYMMDD
        study.studyDate .remove('-');
        d->StudyInstanceUIDsSortedByDateNeedUpdate = true;
        changed = true;
      }
      break;
    }
    case StudyTimeRole:
    {
      if (study.studyTime != value.toString())
      {
        study.studyTime = value.toString();
        d->StudyInstanceUIDsSortedByDateNeedUpdate = true;
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
        // Fix YYYY-MM-DD format in YYYYMMDD
        study.patientBirthDate.remove('-');
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

  for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = d->SeriesModels.begin();
       it != d->SeriesModels.end(); ++it)
  {
    ctkDICOMSeriesModel* seriesModel = it.value();
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
void ctkDICOMStudyModel::setAutoOpenStudiesEnabled(bool enabled)
{
  Q_D(ctkDICOMStudyModel);
  d->AutoOpenStudiesEnabled = enabled;
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModel::autoOpenStudiesEnabled() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->AutoOpenStudiesEnabled;
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
bool ctkDICOMStudyModel::autoRetrieveFullSeries() const
{
  Q_D(const ctkDICOMStudyModel);
  return d->AutoRetrieveFullSeries;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::setAutoRetrieveFullSeries(bool enabled)
{
  Q_D(ctkDICOMStudyModel);
  if (d->AutoRetrieveFullSeries == enabled)
  {
    return;
  }

  d->AutoRetrieveFullSeries = enabled;
  for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = d->SeriesModels.begin();
       it != d->SeriesModels.end(); ++it)
  {
    ctkDICOMSeriesModel* seriesModel = it.value();
    if (seriesModel)
    {
      seriesModel->setAutoRetrieveFullSeries(enabled);
    }
  }
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
  for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = d->SeriesModels.begin();
       it != d->SeriesModels.end(); ++it)
  {
    ctkDICOMSeriesModel* seriesModel = it.value();
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
      return d->matchesStudyFilters(d->Studies.at(row));
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
    const bool accepted = filterSeries ?
      d->isStudyVisible(study, d->getSeriesCountForStudy(study.studyInstanceUID),
                        d->getFilteredSeriesCountForStudy(study.studyInstanceUID)) :
      d->matchesStudyFilters(study);
    if (accepted)
    {
      filteredUIDs.append(study.studyInstanceUID);
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
    for (const QSharedPointer<ctkAbstractJob>& job : std::as_const(runningJobs))
    {
      jobUIDs.append(job->jobUID());
    }
    d->Scheduler->stopJobsByJobUIDs(jobUIDs);
  }
  else if (!failedJobs.isEmpty())
  {
    QStringList jobUIDs;
    // Restart the failed job
    for (const QSharedPointer<ctkAbstractJob>& job : std::as_const(failedJobs))
    {
      jobUIDs.append(job->jobUID());
    }
    d->Scheduler->retryJobs(jobUIDs);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::updateGUIFromScheduler(const QVariant& data)
{
  Q_D(ctkDICOMStudyModel);
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

    // Sort study UIDs by date/time (most recent first) using the same logic as proxy models
    QStringList sortedStudyUIDs = d->sortStudyInstanceUIDsByDate(td.QueriedStudyInstanceUIDs);

    // Emit the sorted list so views can determine which studies to open
    emit this->studiesSortedByDate(sortedStudyUIDs);

    // Query series for each study, using HighPriority for the first N studies
    int studyIndex = 0;
    for (const QString& studyInstanceUID : std::as_const(sortedStudyUIDs))
    {
      ctkDICOMSeriesModel* seriesModel = this->seriesModelForStudyInstanceUID(studyInstanceUID);
      if (!seriesModel)
      {
        continue;
      }

      // Use HighPriority for the first N studies (that will be opened), LowPriority for the rest
      QThread::Priority priority = (studyIndex < d->NumberOfOpenedStudies) ?
                                    QThread::HighPriority : QThread::LowPriority;

      // Set the priority on the series model for its QueryInstances and Retrieve operations
      seriesModel->setJobPriority(priority);

      d->Scheduler->querySeries(d->PatientID,
                                studyInstanceUID,
                                priority,
                                d->AllowedServers);
      studyIndex++;
    }
  }

  QModelIndex idx;
  bool studyIsCollapsed = true;
  ctkDICOMSeriesModel* seriesModel = nullptr;

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries &&
      !td.StudyInstanceUID.isEmpty() &&
      !td.QueriedSeriesInstanceUIDs.isEmpty())
  {
    idx = this->indexFromStudyInstanceUID(td.StudyInstanceUID);
    if (!idx.isValid())
    {
      this->refreshStudy(td.StudyInstanceUID);
      idx = this->indexFromStudyInstanceUID(td.StudyInstanceUID);
    }

    QStringList sortedStudyUIDs = d->allStudyInstanceUIDsSortedByDate();

    emit this->studiesSortedByDate(sortedStudyUIDs);

    const int studyOpenIndex = sortedStudyUIDs.indexOf(td.StudyInstanceUID);
    // Opening a study retrieves its series, so it is only done on its own for the
    // patient being looked at. For the others the study stays collapsed until its
    // patient is selected, which opens it explicitly.
    const bool shouldOpenStudy = d->AutoOpenStudiesEnabled &&
                                 (studyOpenIndex >= 0 && studyOpenIndex < d->NumberOfOpenedStudies);
    if (shouldOpenStudy && idx.isValid())
    {
      this->setStudyCollapsed(idx, false);
      studyIsCollapsed = false;
    }
    else if (idx.isValid())
    {
      studyIsCollapsed = this->data(idx, ctkDICOMStudyModel::IsCollapsedRole).toBool();
    }

    seriesModel = this->seriesModelForStudyInstanceUID(td.StudyInstanceUID);
    if (!seriesModel)
    {
      seriesModel = d->createSeriesModel(td.StudyInstanceUID);
    }
  }
  else
  {
    idx = this->indexFromStudyInstanceUID(td.StudyInstanceUID);
    studyIsCollapsed = this->data(idx, ctkDICOMStudyModel::IsCollapsedRole).toBool();
    seriesModel = this->seriesModelForStudyInstanceUID(td.StudyInstanceUID);
  }

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

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    // The series found decide whether the study is shown
    this->refreshStudy(td.StudyInstanceUID);
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances)
  {
    this->refreshStudy(td.StudyInstanceUID);
    // Emit signal to notify that this study is ready to be opened
    emit this->studyReadyToOpen(td.StudyInstanceUID);
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyModelPrivate::isAllowedServerOrProxy(const QString& connectionName) const
{
  if (this->AllowedServers.contains(connectionName))
  {
    return true;
  }
  if (!this->Scheduler)
  {
    return false;
  }
  for (const QString& allowedServer : std::as_const(this->AllowedServers))
  {
    ctkDICOMServer* server = this->Scheduler->server(allowedServer);
    ctkDICOMServer* proxyServer = server ? server->proxyServer() : nullptr;
    if (proxyServer && proxyServer->connectionName() == connectionName)
    {
      return true;
    }
  }
  return false;
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

  // The retrieve of a series through a proxy runs on the proxy: without it, the retry of
  // a stopped or failed retrieve did not reach the series, which kept the alert icon and
  // the progress of the previous attempt.
  if ((td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
       td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries) &&
      !d->isAllowedServerOrProxy(td.ConnectionName))
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

  return d->seriesFilterProxyModel(studyInstanceUID);
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
  QList<ctkDICOMSeriesModel*> models;
  for (QHash<QString, ctkDICOMSeriesModel*>::const_iterator it = d->SeriesModels.constBegin();
       it != d->SeriesModels.constEnd(); ++it)
  {
    models.append(it.value());
  }
  return models;
}

//------------------------------------------------------------------------------
QList<ctkDICOMSeriesFilterProxyModel*> ctkDICOMStudyModel::allSeriesFilterProxyModels() const
{
  // The proxy models are created when first asked for: create the missing ones
  ctkDICOMStudyModelPrivate* d = const_cast<ctkDICOMStudyModelPrivate*>(this->d_func());
  QList<ctkDICOMSeriesFilterProxyModel*> models;
  for (QHash<QString, ctkDICOMSeriesModel*>::const_iterator it = d->SeriesModels.constBegin();
       it != d->SeriesModels.constEnd(); ++it)
  {
    models.append(d->seriesFilterProxyModel(it.key()));
  }
  return models;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyModel::updateSeriesModelsFilters()
{
  Q_D(ctkDICOMStudyModel);

  // Update all existing series models with current filters
  for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = d->SeriesModels.begin();
       it != d->SeriesModels.end(); ++it)
  {
    ctkDICOMSeriesModel* seriesModel = it.value();
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

  for (QHash<QString, ctkDICOMSeriesModel*>::iterator it = d->SeriesModels.begin();
       it != d->SeriesModels.end(); ++it)
  {
    ctkDICOMSeriesModel* seriesModel = it.value();
    if (seriesModel)
    {
      seriesModel->refreshSeriesList();
    }
  }
}
