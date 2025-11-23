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
#include <QPixmap>
#include <QTimer>
#include <QThread>
#include <QPainter>
#include <QFile>

// STL includes
#include <algorithm>
#include <cmath>

// CTK includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMModalities.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMDatabase.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMJob.h"

static ctkLogger logger("org.commontk.DICOM.Core.DICOMSeriesModel");

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
class ctkDICOMSeriesModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMSeriesModel);

protected:
  ctkDICOMSeriesModel* const q_ptr;

public:
  ctkDICOMSeriesModelPrivate(ctkDICOMSeriesModel& obj);
  ~ctkDICOMSeriesModelPrivate();

  // Data storage
  struct SeriesData {
    QString seriesInstanceUID; // DICOM Database ID
    QString centerInstanceUID;
    QString studyInstanceUID;
    QString patientName;
    QString patientID;
    QString patientBirthDate;
    QString seriesNumber;
    QString modality;
    QString seriesDescription;
    int instanceCount;
    int instancesLoaded;
    int rows;
    int columns;
    bool isCloud;
    bool isLoaded;
    bool isVisible;
    int operationProgress;
    int operationStatus;
    QString jobUID;
    QString thumbnailPath;
    bool thumbnailGenerated;
  };

  void populateSeriesData();
  void loadSeriesForStudy();
  void clean();
  void generateThumbnailForSeries(const QString& seriesInstanceUID);
  int findSeriesLinearIndex(const QString& seriesInstanceUID) const;
  bool seriesMatchesFilters(const ctkDICOMSeriesModelPrivate::SeriesData& series) const;
  bool matchesModalityFilter(const QString& modality) const;
  bool matchesDescriptionFilter(const QString& description) const;
  QString getDICOMCenterFrameFromInstances(QStringList instancesList);
  void updateSeriesVisibility(int seriesIndex);

  QList<SeriesData> SeriesList;

  // Database and scheduler
  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<ctkDICOMScheduler> Scheduler;

  // Filters
  QString PatientID;
  QString StudyFilter;
  QStringList ModalityFilter;
  QString SeriesDescriptionFilter;
  QStringList AllowedServers;

  // Settings
  int ThumbnailSize;
  bool IsUpdating;
  bool AutoGenerateThumbnails;
};

//----------------------------------------------------------------------------
// ctkDICOMSeriesModelPrivate methods

//----------------------------------------------------------------------------
ctkDICOMSeriesModelPrivate::ctkDICOMSeriesModelPrivate(ctkDICOMSeriesModel& obj)
  : q_ptr(&obj)
{
  this->ThumbnailSize = 128;
  this->IsUpdating = false;
  this->AutoGenerateThumbnails = false;
  this->DicomDatabase = nullptr;
  this->Scheduler = nullptr;
  this->PatientID = "";
  this->StudyFilter = "";
  this->ModalityFilter = ctkDICOMModalities::AllModalities;
  this->SeriesDescriptionFilter = "";
}

//----------------------------------------------------------------------------
ctkDICOMSeriesModelPrivate::~ctkDICOMSeriesModelPrivate()
{
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModelPrivate::clean()
{
  Q_Q(ctkDICOMSeriesModel);
  this->IsUpdating = true;

  // Clear all series data
  q->beginResetModel();
  this->SeriesList.clear();
  q->endResetModel();

  this->IsUpdating = false;
  emit q->modelRefreshed();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModelPrivate::populateSeriesData()
{
  Q_Q(ctkDICOMSeriesModel);

  if (!this->DicomDatabase)
  {
    logger.error("populateSeriesData: No database set");
    return;
  }

  if (this->IsUpdating)
  {
    return;
  }

  this->IsUpdating = true;

  if (this->StudyFilter.isEmpty())
  {
    // If no study filter, clear the model if it has data
    if (!this->SeriesList.isEmpty())
    {
      q->beginResetModel();
      this->SeriesList.clear();
      q->endResetModel();
    }
    this->IsUpdating = false;
    emit q->modelRefreshed();
    return;
  }

  this->loadSeriesForStudy();

  this->IsUpdating = false;

  emit q->modelRefreshed();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModelPrivate::loadSeriesForStudy()
{
  Q_Q(ctkDICOMSeriesModel);

  if (!this->DicomDatabase)
  {
    return;
  }

  QStringList databaseSeriesList = this->DicomDatabase->seriesForStudy(this->StudyFilter);
  if (databaseSeriesList.isEmpty())
  {
    // If no series found, clear the model if it has data
    if (!this->SeriesList.isEmpty())
    {
      q->beginResetModel();
      this->SeriesList.clear();
      q->endResetModel();
    }
    return;
  }

  // Create a map of existing series for quick lookup
  QMap<QString, int> existingSeriesMap;
  for (int seriesIndex = 0; seriesIndex < this->SeriesList.size(); ++seriesIndex)
  {
    existingSeriesMap[this->SeriesList[seriesIndex].seriesInstanceUID] = seriesIndex;
  }

  // Create a set of valid series UIDs
  QSet<QString> validSeriesUIDs;
  QList<QString> newSeriesUIDs;

  foreach (const QString& seriesInstanceUID, databaseSeriesList)
  {
    validSeriesUIDs.insert(seriesInstanceUID);

    // Check if this is an existing series
    if (existingSeriesMap.contains(seriesInstanceUID))
    {
      // Update existing series data (instance counts, cloud status, etc.)
      int existingIndex = existingSeriesMap[seriesInstanceUID];
      SeriesData& existingSeries = this->SeriesList[existingIndex];

      // Update dynamic fields
      QStringList instances = this->DicomDatabase->instancesForSeries(seriesInstanceUID);
      int newInstanceCount = instances.count();

      QStringList files = this->DicomDatabase->filesForSeries(seriesInstanceUID);
      files.removeAll(QString(""));
      int newInstancesLoaded = files.count();

      QStringList urls = this->DicomDatabase->urlsForSeries(seriesInstanceUID);
      urls.removeAll(QString(""));

      bool newIsCloud = (newInstanceCount > 0 && urls.count() > 0 && newInstancesLoaded < newInstanceCount);
      bool newIsLoaded = false;
      bool newIsVisible = this->seriesMatchesFilters(existingSeries);

      // Only emit dataChanged if something actually changed
      if (existingSeries.instanceCount != newInstanceCount ||
          existingSeries.instancesLoaded != newInstancesLoaded ||
          existingSeries.isCloud != newIsCloud ||
          existingSeries.isLoaded != newIsLoaded ||
          existingSeries.isVisible != newIsVisible)
      {
        existingSeries.instanceCount = newInstanceCount;
        existingSeries.instancesLoaded = newInstancesLoaded;
        existingSeries.isCloud = newIsCloud;
        existingSeries.isLoaded = newIsLoaded;
        existingSeries.isVisible = newIsVisible;

        QModelIndex topLeft = q->index(existingIndex, 0);
        QModelIndex bottomRight = q->index(existingIndex, q->columnCount() - 1);
        emit q->dataChanged(topLeft, bottomRight);
      }
    }
    else
    {
      // New series - add to list for insertion
      newSeriesUIDs.append(seriesInstanceUID);
    }
  }

  // Remove series that are no longer valid (don't match filters or study)
  QList<int> indicesToRemove;
  for (int index = 0; index < this->SeriesList.size(); ++index)
  {
    if (!validSeriesUIDs.contains(this->SeriesList[index].seriesInstanceUID))
    {
      indicesToRemove.append(index);
    }
  }

  // Remove invalid series (in reverse order to maintain indices)
  for (int index = indicesToRemove.size() - 1; index >= 0; --index)
  {
    int indexToRemove = indicesToRemove[index];
    q->beginRemoveRows(QModelIndex(), indexToRemove, indexToRemove);
    this->SeriesList.removeAt(indexToRemove);
    q->endRemoveRows();
  }

  // Add new series if any
  if (!newSeriesUIDs.isEmpty())
  {
    // Cache patientUID lookup (same for all series in this study)
    QString patientUID = this->DicomDatabase->patientForStudy(this->StudyFilter);
    QString patientID = this->PatientID;
    QString patientName = this->DicomDatabase->fieldForPatient("PatientsName", patientUID);
    patientName.replace(R"(^)", R"( )");
    QString patientBirthDate = this->DicomDatabase->fieldForPatient("PatientsBirthDate", patientUID);
    QList<SeriesData> newSeriesData;

    // Load series data for new series
    foreach (const QString& seriesInstanceUID, newSeriesUIDs)
    {
      SeriesData series;
      series.seriesInstanceUID = seriesInstanceUID;
      series.studyInstanceUID = this->StudyFilter;
      series.patientName = patientName;
      series.patientID = patientID;
      series.patientBirthDate = patientBirthDate;
      series.seriesNumber = this->DicomDatabase->fieldForSeries("SeriesNumber", seriesInstanceUID);
      series.modality = this->DicomDatabase->fieldForSeries("Modality", seriesInstanceUID);
      series.seriesDescription = this->DicomDatabase->fieldForSeries("SeriesDescription", seriesInstanceUID);
      if (series.seriesDescription.isEmpty())
      {
        series.seriesDescription = "UNDEFINED";
      }
      series.isVisible = this->seriesMatchesFilters(series);

      // Get instance counts
      QStringList instances = this->DicomDatabase->instancesForSeries(series.seriesInstanceUID);
      series.instanceCount = instances.count();

      QStringList files = this->DicomDatabase->filesForSeries(series.seriesInstanceUID);
      files.removeAll(QString(""));
      series.instancesLoaded = files.count();

      QStringList urls = this->DicomDatabase->urlsForSeries(series.seriesInstanceUID);
      urls.removeAll(QString(""));

      series.rows = 0;
      series.columns = 0;

      // Determine cloud status
      series.isCloud = series.instanceCount > 0 && urls.count() > 0 && series.instancesLoaded < series.instanceCount;
      series.isLoaded = this->DicomDatabase->loadedSeriesInstanceUIDs().contains(seriesInstanceUID);

      // Initialize other fields
      series.operationProgress = 0;
      series.operationStatus = ctkDICOMSeriesModel::NoOperation;
      series.thumbnailGenerated = false;

      // Check for existing cached thumbnail first
      if (series.centerInstanceUID.isEmpty())
      {
        series.centerInstanceUID = this->getDICOMCenterFrameFromInstances(instances);
      }

      if (!series.centerInstanceUID.isEmpty())
      {
        // Get DICOM Rows/Columns from first instance (if available)
        QString rowsStr = this->DicomDatabase->instanceValue(series.centerInstanceUID, "0028,0010"); // Rows
        QString colsStr = this->DicomDatabase->instanceValue(series.centerInstanceUID, "0028,0011"); // Columns
        series.rows = rowsStr.toInt();
        series.columns = colsStr.toInt();
      }
      else if (!instances.isEmpty())
      {
        // Get DICOM Rows/Columns from first instance (if available)
        QString rowsStr = this->DicomDatabase->instanceValue(instances.first(), "0028,0010"); // Rows
        QString colsStr = this->DicomDatabase->instanceValue(instances.first(), "0028,0011"); // Columns
        series.rows = rowsStr.toInt();
        series.columns = colsStr.toInt();
      }

      if (series.centerInstanceUID.isEmpty() ||
          series.seriesInstanceUID.isEmpty() ||
          series.studyInstanceUID.isEmpty() ||
          !this->AutoGenerateThumbnails)
      {
        newSeriesData.append(series);
        continue;
      }

      QString thumbnailPath = this->DicomDatabase->thumbnailPathForInstance(
        series.studyInstanceUID, series.seriesInstanceUID, series.centerInstanceUID);
      if (!thumbnailPath.isEmpty() && QFile::exists(thumbnailPath))
      {
        QPixmap cachedThumbnail(thumbnailPath);
        if (!cachedThumbnail.isNull())
        {
          // Use cached thumbnail
          series.thumbnailPath = thumbnailPath;
          series.thumbnailGenerated = true;
        }
      }

      newSeriesData.append(series);
    }

    // Insert all new series at the end
    // The proxy model will handle sorting by series number
    int insertPos = this->SeriesList.size();

    q->beginInsertRows(QModelIndex(), insertPos, insertPos + newSeriesData.size() - 1);
    for (const SeriesData& newSeries : newSeriesData)
    {
      this->SeriesList.append(newSeries);
    }
    q->endInsertRows();
  }
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesModelPrivate::seriesMatchesFilters(const ctkDICOMSeriesModelPrivate::SeriesData& series) const
{
  return this->matchesModalityFilter(series.modality) &&
         this->matchesDescriptionFilter(series.seriesDescription);
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesModelPrivate::matchesModalityFilter(const QString& modality) const
{
  // If "Any" is present in the modality filter, do not apply modality filtering
  if (this->ModalityFilter.isEmpty() || this->ModalityFilter.contains("Any"))
  {
    return true;
  }

  return this->ModalityFilter.contains(modality);
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesModelPrivate::matchesDescriptionFilter(const QString& description) const
{
  if (this->SeriesDescriptionFilter.isEmpty())
  {
    return true;
  }

  return description.contains(this->SeriesDescriptionFilter, Qt::CaseInsensitive);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModelPrivate::updateSeriesVisibility(int seriesIndex)
{
  ctkDICOMSeriesModelPrivate::SeriesData& series = this->SeriesList[seriesIndex];
  series.isVisible = this->seriesMatchesFilters(series);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModelPrivate::generateThumbnailForSeries(const QString& seriesInstanceUID)
{
  Q_Q(ctkDICOMSeriesModel);
  if (!this->DicomDatabase || seriesInstanceUID.isEmpty())
  {
    return;
  }

  if (this->ThumbnailSize == 0)
  {
    return;
  }

  int linearIndex = this->findSeriesLinearIndex(seriesInstanceUID);
  if (linearIndex < 0 || linearIndex >= this->SeriesList.size())
  {
    return;
  }

  SeriesData& series = this->SeriesList[linearIndex];
  if (series.thumbnailGenerated)
  {
    return;
  }
  if (!series.isVisible)
  {
    return;
  }

  // Get a representative instance from the series
  if (series.centerInstanceUID.isEmpty())
  {
    QStringList instancesList = this->DicomDatabase->instancesForSeries(series.seriesInstanceUID);
    series.centerInstanceUID = this->getDICOMCenterFrameFromInstances(instancesList);
  }
  if (series.centerInstanceUID.isEmpty())
  {
    return;
  }

  QString thumbnailPath = this->DicomDatabase->thumbnailPathForInstance(
    series.studyInstanceUID, series.seriesInstanceUID, series.centerInstanceUID);
  if (!thumbnailPath.isEmpty() && QFile::exists(thumbnailPath))
  {
    QPixmap cachedThumbnail(thumbnailPath);
    if (!cachedThumbnail.isNull())
    {
      // Use cached thumbnail
      series.thumbnailPath = thumbnailPath;
      series.thumbnailGenerated = true;
      QModelIndex index = q->createIndex(linearIndex, 0);
      emit q->dataChanged(index, index, {q->ThumbnailPathRole, q->ThumbnailGeneratedRole});
      q->onThumbnailGenerated(series.seriesInstanceUID, thumbnailPath);
      return;
    }
  }

  // Get the file path for this instance
  QString dicomFilePath = this->DicomDatabase->fileForInstance(series.centerInstanceUID);
  if (dicomFilePath.isEmpty() || !QFile::exists(dicomFilePath))
  {
    return;
  }

  // Request thumbnail generation from scheduler directly
  if (this->Scheduler)
  {
    this->Scheduler->generateThumbnail(dicomFilePath,
                                       series.patientID,
                                       series.studyInstanceUID,
                                       series.seriesInstanceUID,
                                       series.centerInstanceUID,
                                       series.modality);
  }
}

//----------------------------------------------------------------------------
QString ctkDICOMSeriesModelPrivate::getDICOMCenterFrameFromInstances(QStringList instancesList)
{
  if (instancesList.isEmpty())
  {
    return QString();
  }

  QString instanceUID;

  // Use bulk query approach for better performance while maintaining proper DICOM ordering
  if (instancesList.isEmpty())
  {
    return QString();
  }

  if (instancesList.size() == 1)
  {
    return instancesList.first();
  }

  // Use the new efficient bulk query method to get all instance numbers at once
  QMap<int, QString> sortedInstancesMap;

  if (this->DicomDatabase && this->DicomDatabase->tagCacheExists())
  {
    // Get all instance numbers in a single efficient database query
    QMap<QString, QString> instanceNumbers = this->DicomDatabase->instanceValues(instancesList, "0020,0013");

    // Build the sorted map
    for (QMap<QString, QString>::const_iterator it = instanceNumbers.constBegin(); it != instanceNumbers.constEnd(); ++it)
    {
      const QString& sopInstanceUID = it.key();
      const QString& instanceNumberStr = it.value();

      int instanceNumber = 0;
      if (!instanceNumberStr.isEmpty())
      {
        instanceNumber = instanceNumberStr.toInt();
      }

      sortedInstancesMap[instanceNumber] = sopInstanceUID;
    }
  }

  // If bulk query didn't work or returned no results, fall back to middle selection
  if (sortedInstancesMap.isEmpty())
  {
    std::sort(instancesList.begin(), instancesList.end());
    instanceUID = instancesList[floor(instancesList.size() * 0.5)];
  }
  else
  {
    // Sort by instance number and select the center one
    QList<int> keys = sortedInstancesMap.keys();
    std::sort(keys.begin(), keys.end());

    int centerInstanceIndex = floor(keys.count() * 0.5);
    int centerInstanceNumber = keys[centerInstanceIndex];
    instanceUID = sortedInstancesMap[centerInstanceNumber];
  }

  return instanceUID;
}

//----------------------------------------------------------------------------
int ctkDICOMSeriesModelPrivate::findSeriesLinearIndex(const QString& seriesInstanceUID) const
{
  for (int index = 0; index < this->SeriesList.size(); ++index)
  {
    if (this->SeriesList[index].seriesInstanceUID == seriesInstanceUID)
    {
      return index;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
ctkDICOMSeriesModel::ctkDICOMSeriesModel(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMSeriesModelPrivate(*this))
{
}

//----------------------------------------------------------------------------
ctkDICOMSeriesModel::~ctkDICOMSeriesModel()
{
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setDicomDatabase(ctkDICOMDatabase& database)
{
  this->setDicomDatabase(QSharedPointer<ctkDICOMDatabase>(&database, skipDelete));
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setDicomDatabase(QSharedPointer<ctkDICOMDatabase> database)
{
  Q_D(ctkDICOMSeriesModel);
  if (!database.data())
  {
    logger.error("setDicomDatabase: Invalid (null) database pointer");
    return;
  }
  if (d->DicomDatabase == database)
  {
    return;
  }
  if (d->DicomDatabase)
  {
    this->disconnect(d->DicomDatabase.data(), &ctkDICOMDatabase::loadedSeriesInstanceUIDsChanged,
                     this, &ctkDICOMSeriesModel::onLoadedSeriesChanged);
  }
  d->DicomDatabase = database;
  this->connect(d->DicomDatabase.data(), &ctkDICOMDatabase::loadedSeriesInstanceUIDsChanged,
                this, &ctkDICOMSeriesModel::onLoadedSeriesChanged);
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMSeriesModel::dicomDatabase() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->DicomDatabase.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMSeriesModel::dicomDatabaseShared() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->DicomDatabase;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setScheduler(ctkDICOMScheduler& scheduler)
{
  Q_D(ctkDICOMSeriesModel);
  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMSeriesModel);
  d->Scheduler = scheduler;
}

//----------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMSeriesModel::scheduler() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMSeriesModel::schedulerShared() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->Scheduler;
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesModel::patientID() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->PatientID;
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesModel::setPatientID(const QString& patientID)
{
  Q_D(ctkDICOMSeriesModel);

  if (d->PatientID == patientID)
  {
    return;
  }

  d->PatientID = patientID;
  emit this->patientIDChanged(patientID);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setStudyFilter(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->StudyFilter == studyInstanceUID)
  {
    return;
  }

  d->StudyFilter = studyInstanceUID;
  this->refresh();
  emit studyFilterChanged(studyInstanceUID);
}

//----------------------------------------------------------------------------
QString ctkDICOMSeriesModel::studyFilter() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->StudyFilter;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setModalityFilter(const QStringList& modalities)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->ModalityFilter == modalities)
  {
    return;
  }

  d->ModalityFilter = modalities;
  this->refreshSeriesList();
  emit modalityFilterChanged(modalities);
}

//----------------------------------------------------------------------------
QStringList ctkDICOMSeriesModel::modalityFilter() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->ModalityFilter;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setSeriesDescriptionFilter(const QString& description)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->SeriesDescriptionFilter == description)
  {
    return;
  }

  d->SeriesDescriptionFilter = description;
  this->refreshSeriesList();
  emit seriesDescriptionFilterChanged(description);
}

//----------------------------------------------------------------------------
QString ctkDICOMSeriesModel::seriesDescriptionFilter() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->SeriesDescriptionFilter;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setThumbnailSize(int size)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->ThumbnailSize == size)
  {
    return;
  }

  d->ThumbnailSize = size;

  // Only regenerate thumbnails if auto-generation is enabled
  if (d->AutoGenerateThumbnails)
  {
    this->generateThumbnails(true);
  }

  emit this->dataChanged(createIndex(0, 0), createIndex(d->SeriesList.size() - 1, 0), {ThumbnailSizeRole});
  emit this->thumbnailSizeChanged(size);
}

//----------------------------------------------------------------------------
int ctkDICOMSeriesModel::thumbnailSize() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->ThumbnailSize;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setAutoGenerateThumbnails(bool enable)
{
  Q_D(ctkDICOMSeriesModel);
  d->AutoGenerateThumbnails = enable;
  emit this->autoGenerateThumbnailsChanged(enable);
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesModel::autoGenerateThumbnails() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->AutoGenerateThumbnails;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::setAllowedServers(const QStringList& servers)
{
  Q_D(ctkDICOMSeriesModel);
  d->AllowedServers = servers;
  emit this->allowedServersChanged(servers);
}

//----------------------------------------------------------------------------
QStringList ctkDICOMSeriesModel::allowedServers() const
{
  Q_D(const ctkDICOMSeriesModel);
  return d->AllowedServers;
}

//----------------------------------------------------------------------------
QString ctkDICOMSeriesModel::seriesInstanceUID(const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesModel);
  if (!index.isValid())
  {
    return QString();
  }

  // Linear model: row is the series index, column should always be 0
  if (index.column() != 0 || index.row() >= d->SeriesList.size())
  {
    return QString();
  }

  return d->SeriesList[index.row()].seriesInstanceUID;
}

//----------------------------------------------------------------------------
QModelIndex ctkDICOMSeriesModel::indexForSeriesInstanceUID(const QString& seriesInstanceUID) const
{
  Q_D(const ctkDICOMSeriesModel);
  int linearIndex = d->findSeriesLinearIndex(seriesInstanceUID);
  if (linearIndex >= 0 && linearIndex < d->SeriesList.size())
  {
    // Linear model: return index with row = linearIndex, column = 0
    return this->createIndex(linearIndex, 0);
  }
  return QModelIndex();
}

//----------------------------------------------------------------------------
QStringList ctkDICOMSeriesModel::seriesInstanceUIDs() const
{
  Q_D(const ctkDICOMSeriesModel);

  QStringList seriesInstanceUIDs;
  for (const ctkDICOMSeriesModelPrivate::SeriesData& series : d->SeriesList)
  {
    seriesInstanceUIDs.append(series.seriesInstanceUID);
  }

  return seriesInstanceUIDs;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMSeriesModel::filteredSeriesInstanceUIDs() const
{
  Q_D(const ctkDICOMSeriesModel);

  QStringList filteredUIDs;
  for (const ctkDICOMSeriesModelPrivate::SeriesData& series : d->SeriesList)
  {
    QString seriesInstanceUID = series.seriesInstanceUID;
    if (d->seriesMatchesFilters(series))
    {
      filteredUIDs.append(seriesInstanceUID);
    }
  }

  return filteredUIDs;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::refresh()
{
  Q_D(ctkDICOMSeriesModel);
  d->populateSeriesData();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::refreshSeriesList()
{
  Q_D(ctkDICOMSeriesModel);
  // Update all series in batch and emit dataChanged only once
  if (d->SeriesList.isEmpty())
  {
    return;
  }

  // Update all series data in-place
  for (int seriesIndex = 0; seriesIndex < d->SeriesList.size(); ++seriesIndex)
  {
    d->updateSeriesVisibility(seriesIndex);
  }

  // Emit dataChanged for the whole range
  QModelIndex topLeft = this->index(0, 0);
  QModelIndex bottomRight = this->index(d->SeriesList.size() - 1, 0);
  emit dataChanged(topLeft, bottomRight, QVector<int>() << ctkDICOMSeriesModel::IsVisibleRole);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::clean()
{
  Q_D(ctkDICOMSeriesModel);
  d->clean();
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::refreshSeries(const QString &seriesInstanceUID)
{
  Q_D(ctkDICOMSeriesModel);
  QModelIndex index = this->indexForSeriesInstanceUID(seriesInstanceUID);
  if (!index.isValid())
  {
    return;
  }

  int seriesIndex = d->findSeriesLinearIndex(seriesInstanceUID);
  d->updateSeriesVisibility(seriesIndex);

  emit this->dataChanged(index, index, QVector<int>() << IsVisibleRole);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::generateThumbnails(bool regenerate)
{
  Q_D(ctkDICOMSeriesModel);
  for (ctkDICOMSeriesModelPrivate::SeriesData& series : d->SeriesList)
  {
    if (regenerate)
    {
      series.thumbnailGenerated = false;
      emit this->dataChanged(createIndex(0, 0), createIndex(d->SeriesList.size() - 1, 0), {ThumbnailGeneratedRole});
    }
    d->generateThumbnailForSeries(series.seriesInstanceUID);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::generateThumbnail(const QModelIndex& index)
{
  Q_D(ctkDICOMSeriesModel);
  if (!index.isValid() || index.row() >= d->SeriesList.size())
  {
    return;
  }

  const auto& info = d->SeriesList[index.row()];
  d->generateThumbnailForSeries(info.seriesInstanceUID);
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::retrieveAllSeries()
{
  Q_D(ctkDICOMSeriesModel);
  for (ctkDICOMSeriesModelPrivate::SeriesData& series : d->SeriesList)
  {
    ctkDICOMJobDetail newTd;
    newTd.JobType = ctkDICOMJobResponseSet::JobType::QueryInstances;
    newTd.JobUID = series.seriesInstanceUID;
    newTd.PatientID = series.patientID;
    newTd.StudyInstanceUID = series.studyInstanceUID;
    newTd.SeriesInstanceUID = series.seriesInstanceUID;
    this->updateGUIFromScheduler(QVariant::fromValue(newTd), false);
  }
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesModel::isSeriesCloud(const QString &seriesInstanceUID)
{
  Q_D(ctkDICOMSeriesModel);
  int linearIndex = d->findSeriesLinearIndex(seriesInstanceUID);
  if (linearIndex < 0 || linearIndex >= d->SeriesList.size())
  {
    return false;
  }

  ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
  return seriesData.isCloud;
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::forceUpdateSeriesJobs(const QString &seriesInstanceUID)
{
  Q_D(const ctkDICOMSeriesModel);
  if (!d->Scheduler)
  {
    return;
  }

  QList<QSharedPointer<ctkAbstractJob>> runningJobs = d->Scheduler->getJobsByDICOMUIDs({}, {}, {seriesInstanceUID});
  QList<ctkAbstractJob::JobStatus> statusFilters =
  {
    ctkAbstractJob::JobStatus::UserStopped,
    ctkAbstractJob::JobStatus::Failed
  };
  QList<QSharedPointer<ctkAbstractJob>> failedJobs = d->Scheduler->getJobsByDICOMUIDs({}, {}, {seriesInstanceUID}, {}, statusFilters);

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

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::forceRetrieveSeries(const QString &seriesInstanceUID)
{
  Q_D(ctkDICOMSeriesModel);
  if (!d->Scheduler)
  {
    return;
  }

  int linearIndex = d->findSeriesLinearIndex(seriesInstanceUID);
  if (linearIndex < 0 || linearIndex >= d->SeriesList.size())
  {
    return;
  }

  ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
  if (d->AllowedServers.isEmpty())
  {
    logger.warn("ctkDICOMSeriesModel::forceRetrieveSeries: No allowed servers specified, cannot retrieve series.");
    seriesData.operationStatus = ctkDICOMSeriesModel::Failed;
  }
  else
  {
    seriesData.operationStatus = ctkDICOMSeriesModel::NoOperation;
  }
  seriesData.operationProgress = 0;
  seriesData.isCloud = true;
  seriesData.instancesLoaded = 0;
  seriesData.thumbnailPath = "";
  seriesData.thumbnailGenerated = false;
  QModelIndex index = this->createIndex(linearIndex, 0);
  emit this->dataChanged(index, index,
    {
      IsCloudRole,
      OperationStatusRole,
      OperationProgressRole,
      InstancesLoadedRole,
      ThumbnailPathRole,
      ThumbnailGeneratedRole
    }
  );

  if (!d->AllowedServers.isEmpty())
  {
    d->Scheduler->queryInstances(d->PatientID,
                                 d->StudyFilter,
                                 seriesInstanceUID,
                                 QThread::HighPriority,
                                 d->AllowedServers);
  }
}

//----------------------------------------------------------------------------
int ctkDICOMSeriesModel::rowCount(const QModelIndex& parent) const
{
  Q_D(const ctkDICOMSeriesModel);
  Q_UNUSED(parent);

  // Return the total number of series as rows (linear model)
  return d->SeriesList.size();
}

//----------------------------------------------------------------------------
int ctkDICOMSeriesModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);

  // Always single column for linear model
  return 1;
}

//----------------------------------------------------------------------------
QVariant ctkDICOMSeriesModel::data(const QModelIndex& index, int role) const
{
  Q_D(const ctkDICOMSeriesModel);

  if (!index.isValid())
  {
    return QVariant();
  }

  // Linear model: row is the series index, column should always be 0
  if (index.column() != 0 || index.row() >= d->SeriesList.size())
  {
    return QVariant();
  }

  const ctkDICOMSeriesModelPrivate::SeriesData& series = d->SeriesList[index.row()];

  switch (role)
  {
    case Qt::DisplayRole:
      return QString("Series %1: %2").arg(series.seriesNumber).arg(series.seriesDescription);

    case Qt::ToolTipRole:
    {
      QStringList infoParts;
      if (!series.patientName.isEmpty())
      {
        infoParts << QString("Patient name: %1").arg(series.patientName);
      }
      if (!series.patientID.isEmpty())
      {
        infoParts << QString("Patient MRN: %1").arg(series.patientID);
      }
      if (!series.patientBirthDate.isEmpty())
      {
        infoParts << QString("Patient birth date: %1").arg(series.patientBirthDate);
      }
      if (!series.studyInstanceUID.isEmpty())
      {
        infoParts << QString("StudyInstanceUID: %1").arg(series.studyInstanceUID);
      }
      if (!series.seriesInstanceUID.isEmpty())
      {
        infoParts << QString("SeriesInstanceUID: %1").arg(series.seriesInstanceUID);
      }
      if (!series.seriesNumber.isEmpty())
      {
        infoParts << QString("Number: %1").arg(series.seriesNumber);
      }
      if (!series.seriesDescription.isEmpty())
      {
        infoParts << QString("Description: %1").arg(series.seriesDescription);
      }
      if (!series.modality.isEmpty())
      {
        infoParts << QString("Modality: %1").arg(series.modality);
      }
      return infoParts.join("\n");
    }

    case SeriesInstanceUIDRole:
      return series.seriesInstanceUID;

    case SeriesNumberRole:
      return series.seriesNumber;

    case ModalityRole:
      return series.modality;

    case SeriesDescriptionRole:
      return series.seriesDescription;

    case InstanceCountRole:
      return series.instanceCount;

    case InstancesLoadedRole:
      return series.instancesLoaded;

    case RowsRole:
      return series.rows;

    case ColumnsRole:
      return series.columns;

    case ThumbnailPathRole:
      return series.thumbnailPath;

    case ThumbnailSizeRole:
      return QSize(d->ThumbnailSize, d->ThumbnailSize);

    case ThumbnailGeneratedRole:
      return series.thumbnailGenerated;

    case IsCloudRole:
      return series.isCloud;

    case IsLoadedRole:
      return series.isLoaded;

    case IsVisibleRole:
      return series.isVisible;

    case OperationProgressRole:
      return series.operationProgress;

    case OperationStatusRole:
      return series.operationStatus;

    case JobUIDRole:
      return series.jobUID;

    case PatientIDRole:
      return series.patientID;

    case PatientNameRole:
      return series.patientName;

    case PatientBirthDateRole:
      return series.patientBirthDate;

    case StudyInstanceUIDRole:
      return series.studyInstanceUID;

    default:
      return QVariant();
  }
}

//----------------------------------------------------------------------------
QVariant ctkDICOMSeriesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(section);
  Q_UNUSED(orientation);
  Q_UNUSED(role);

  // No headers needed for the series grid
  return QVariant();
}

//----------------------------------------------------------------------------
bool ctkDICOMSeriesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  Q_D(ctkDICOMSeriesModel);

  if (!index.isValid())
  {
    return false;
  }

  // Linear model: row is the series index, column should always be 0
  if (index.column() != 0 || index.row() >= d->SeriesList.size())
  {
    return false;
  }

  ctkDICOMSeriesModelPrivate::SeriesData& info = d->SeriesList[index.row()];
  bool changed = false;

  switch (role)
  {
    case Qt::DisplayRole:
    {
      // Not stored, just for display
      break;
    }
    case SeriesInstanceUIDRole:
    {
      if (info.seriesInstanceUID != value.toString())
      {
        info.seriesInstanceUID = value.toString();
        changed = true;
      }
      break;
    }
    case SeriesNumberRole:
    {
      if (info.seriesNumber != value.toString())
      {
        info.seriesNumber = value.toString();
        changed = true;
      }
      break;
    }
    case ModalityRole:
    {
      if (info.modality != value.toString())
      {
        info.modality = value.toString();
        changed = true;
      }
      break;
    }
    case SeriesDescriptionRole:
    {
      if (info.seriesDescription != value.toString())
      {
        info.seriesDescription = value.toString();
        changed = true;
      }
      break;
    }
    case InstanceCountRole:
    {
      if (info.instanceCount != value.toInt())
      {
        info.instanceCount = value.toInt();
        changed = true;
      }
      break;
    }
    case InstancesLoadedRole:
    {
      if (info.instancesLoaded != value.toInt())
      {
        info.instancesLoaded = value.toInt();
        changed = true;
      }
      break;
    }
    case RowsRole:
    {
      if (info.rows != value.toInt())
      {
        info.rows = value.toInt();
        changed = true;
      }
      break;
    }
    case ColumnsRole:
    {
      if (info.columns != value.toInt())
      {
        info.columns = value.toInt();
        changed = true;
      }
      break;
    }
    case ThumbnailPathRole:
    {
      if (info.thumbnailPath != value.toString())
      {
        info.thumbnailPath = value.toString();
        changed = true;
      }
      break;
    }
    case ThumbnailGeneratedRole:
    {
      if (info.thumbnailGenerated != value.toBool())
      {
        info.thumbnailGenerated = value.toBool();
        changed = true;
      }
      break;
    }
    case IsCloudRole:
    {
      if (info.isCloud != value.toBool())
      {
        info.isCloud = value.toBool();
        changed = true;
      }
      break;
    }
    case IsLoadedRole:
    {
      if (info.isLoaded != value.toBool())
      {
        info.isLoaded = value.toBool();
        changed = true;
      }
      break;
    }
    case IsVisibleRole:
    {
      if (info.isVisible != value.toBool())
      {
        info.isVisible = value.toBool();
        changed = true;
      }
      break;
    }
    case OperationProgressRole:
    {
      if (info.operationProgress != value.toInt())
      {
        info.operationProgress = value.toInt();
        changed = true;
        emit operationProgressChanged(index, info.operationProgress);
      }
      break;
    }
    case OperationStatusRole:
    {
      if (info.operationStatus != value.toInt())
      {
        info.operationStatus = value.toInt();
        changed = true;
      }
      break;
    }
    case JobUIDRole:
    {
      if (info.jobUID != value.toString())
      {
        info.jobUID = value.toString();
        changed = true;
      }
      break;
    }
    case PatientIDRole:
    {
      if (info.patientID != value.toString())
      {
        info.patientID = value.toString();
        changed = true;
      }
      break;
    }
    case PatientNameRole:
    {
      if (info.patientBirthDate != value.toString())
      {
        info.patientBirthDate = value.toString();
        changed = true;
      }
      break;
    }
    case PatientBirthDateRole:
    {
      if (info.patientName != value.toString())
      {
        info.patientName = value.toString();
        changed = true;
      }
      break;
    }
    case StudyInstanceUIDRole:
    {
      if (info.studyInstanceUID != value.toString())
      {
        info.studyInstanceUID = value.toString();
        changed = true;
      }
      break;
    }
    default:
      return false;
  }

  if (changed)
  {
    emit this->dataChanged(index, index, {role});
  }

  return changed;
}

//----------------------------------------------------------------------------
Qt::ItemFlags ctkDICOMSeriesModel::flags(const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesModel);

  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }

  // Linear model: row is the series index, column should always be 0
  if (index.column() != 0 || index.row() >= d->SeriesList.size())
  {
    return Qt::NoItemFlags;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//----------------------------------------------------------------------------
QHash<int, QByteArray> ctkDICOMSeriesModel::roleNames() const
{
  QHash<int, QByteArray> roles = Superclass::roleNames();
  roles[SeriesInstanceUIDRole] = "seriesInstanceUID";
  roles[SeriesNumberRole] = "seriesNumber";
  roles[ModalityRole] = "modality";
  roles[SeriesDescriptionRole] = "seriesDescription";
  roles[InstanceCountRole] = "instanceCount";
  roles[InstancesLoadedRole] = "instancesLoaded";
  roles[RowsRole] = "rows";
  roles[ColumnsRole] = "columns";
  roles[ThumbnailPathRole] = "thumbnailPath";
  roles[ThumbnailGeneratedRole] = "thumbnailGenerated";
  roles[IsCloudRole] = "isCloud";
  roles[IsLoadedRole] = "isLoaded";
  roles[IsVisibleRole] = "isVisible";
  roles[OperationProgressRole] = "operationProgress";
  roles[OperationStatusRole] = "operationStatus";
  roles[JobUIDRole] = "jobUID";
  roles[PatientIDRole] = "patientID";
  roles[StudyInstanceUIDRole] = "studyInstanceUID";
  return roles;
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesModel::updateGUIFromScheduler(const QVariant& data, const bool& studyIsCollapsed)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->IsUpdating)
  {
    return;
  }

  if (!d->Scheduler)
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

  if (td.StudyInstanceUID != "" && td.StudyInstanceUID != d->StudyFilter)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    if (td.QueriedSeriesInstanceUIDs.isEmpty())
    {
      return;
    }

    this->refresh();

    for (const QString& seriesInstanceUID : td.QueriedSeriesInstanceUIDs)
    {
      int linearIndex = d->findSeriesLinearIndex(seriesInstanceUID);
      if (linearIndex < 0)
      {
        continue;
      }
      QStringList instancesList = d->DicomDatabase->instancesForSeries(seriesInstanceUID);
      if (!instancesList.isEmpty())
      {
        ctkDICOMJobDetail newTd = td;
        newTd.JobType = ctkDICOMJobResponseSet::JobType::QueryInstances;
        newTd.SeriesInstanceUID = seriesInstanceUID;
        this->updateGUIFromScheduler(QVariant::fromValue(newTd), studyIsCollapsed);
        continue;
      }

      if (d->AllowedServers.isEmpty())
      {
        ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
        seriesData.operationStatus = ctkDICOMSeriesModel::Failed;
        seriesData.operationProgress = 0;
        QModelIndex index = this->createIndex(linearIndex, 0);
        emit this->dataChanged(index, index, {OperationStatusRole, OperationProgressRole});
      }
      else
      {
        d->Scheduler->queryInstances(d->PatientID,
                                     d->StudyFilter,
                                     seriesInstanceUID,
                                     QThread::NormalPriority,
                                     d->AllowedServers);
      }
      continue;
    }
  }

  int linearIndex = d->findSeriesLinearIndex(td.SeriesInstanceUID);
  if (linearIndex < 0 || linearIndex >= d->SeriesList.size())
  {
    return;
  }

  QStringList instancesList = d->DicomDatabase->instancesForSeries(td.SeriesInstanceUID);
  if (instancesList.isEmpty())
  {
    return;
  }

  ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
  QModelIndex index = this->createIndex(linearIndex, 0);
  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances &&
      !seriesData.isCloud && seriesData.instanceCount != 0)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances)
  {
    if (td.QueriedSOPInstanceUIDs.size() == 0 && td.SeriesInstanceUID.isEmpty())
    {
      seriesData.operationStatus = OperationStatus::Failed;
      emit this->dataChanged(index, index, {OperationStatusRole});
      return;
    }

    if (studyIsCollapsed)
    {
      // Do not proceed with retrieval if the study is collapsed
      return;
    }

    // update number of instance and render
    seriesData.instanceCount = instancesList.count();
    seriesData.instancesLoaded = 0;
    seriesData.isCloud = true;
    this->refreshSeries(seriesData.seriesInstanceUID);

    // Get the central frame instance UID
    if (seriesData.centerInstanceUID.isEmpty())
    {
      seriesData.centerInstanceUID = d->getDICOMCenterFrameFromInstances(instancesList);
    }
    if (!seriesData.centerInstanceUID.isEmpty())
    {
      // Check if the central instance is already retrieved
      QString centralInstanceFile = d->DicomDatabase->fileForInstance(seriesData.centerInstanceUID);
      if (centralInstanceFile.isEmpty())
      {
        // Retrieve the central instance for thumbnail generation
        d->Scheduler->retrieveSOPInstance(seriesData.patientID,
                                          seriesData.studyInstanceUID,
                                          seriesData.seriesInstanceUID,
                                          seriesData.centerInstanceUID,
                                          QThread::HighestPriority,
                                          d->AllowedServers);
        return;
      }
      else if (!centralInstanceFile.isEmpty())
      {
        // Central instance is already available, generate thumbnail
        d->generateThumbnailForSeries(seriesData.seriesInstanceUID);
      }
    }
  }

  // After central instance is retrieved, generate thumbnail
  if (((td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance) &&
       td.SOPInstanceUID == seriesData.centerInstanceUID &&
       seriesData.operationStatus != ctkDICOMSeriesModel::InProgress) ||
      td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances)
  {
    QString rowsStr = d->DicomDatabase->instanceValue(seriesData.centerInstanceUID, "0028,0010"); // Rows
    QString colsStr = d->DicomDatabase->instanceValue(seriesData.centerInstanceUID, "0028,0011"); // Columns
    seriesData.rows = rowsStr.toInt();
    seriesData.columns = colsStr.toInt();
    this->refreshSeries(seriesData.seriesInstanceUID);

    // Central instance has been retrieved, generate thumbnail
    d->generateThumbnailForSeries(seriesData.seriesInstanceUID);

    // retrieve the full series if needed
    if (seriesData.instanceCount > 1 && seriesData.isCloud)
    {
      // Retrieve the full series
      d->Scheduler->retrieveSeries(seriesData.patientID,
                                   seriesData.studyInstanceUID,
                                   seriesData.seriesInstanceUID,
                                   QThread::LowPriority,
                                   d->AllowedServers);
    }
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
      td.JobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance)
  {
    d->generateThumbnailForSeries(seriesData.seriesInstanceUID);
    seriesData.operationProgress++;
    seriesData.operationProgress = qMin(seriesData.operationProgress, seriesData.instanceCount);

    emit this->dataChanged(index, index, {OperationProgressRole});
    emit this->operationProgressChanged(index, seriesData.operationProgress);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::onJobStarted(const QVariant& data)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.SeriesInstanceUID.isEmpty())
  {
    return;
  }

  int linearIndex = d->findSeriesLinearIndex(td.SeriesInstanceUID);
  if (linearIndex < 0 || linearIndex >= d->SeriesList.size())
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
    seriesData.operationStatus = ctkDICOMSeriesModel::InProgress;
    seriesData.operationProgress = 0;
    QModelIndex index = this->createIndex(linearIndex, 0);
    emit this->dataChanged(index, index, {OperationStatusRole, OperationProgressRole});
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::onJobFinished(const QVariant& data)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.SeriesInstanceUID.isEmpty())
  {
    return;
  }

  int linearIndex = d->findSeriesLinearIndex(td.SeriesInstanceUID);
  if (linearIndex < 0 || linearIndex >= d->SeriesList.size())
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
  {
    ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
    if (seriesData.instanceCount == 1)
    {
      seriesData.isCloud = false;
      seriesData.operationStatus = ctkDICOMSeriesModel::Completed;
      seriesData.operationProgress = 0;
      seriesData.instancesLoaded = seriesData.instanceCount;
      QModelIndex index = this->createIndex(linearIndex, 0);
      emit this->dataChanged(index, index, {IsCloudRole, OperationStatusRole, OperationProgressRole});
    }
  }
  else if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
    seriesData.isCloud = false;
    seriesData.operationStatus = ctkDICOMSeriesModel::Completed;
    seriesData.operationProgress = 0;
    seriesData.instancesLoaded = seriesData.instanceCount;
    QModelIndex index = this->createIndex(linearIndex, 0);
    emit this->dataChanged(index, index, {IsCloudRole, OperationStatusRole, OperationProgressRole, InstancesLoadedRole});
  }
  else if (td.JobType == ctkDICOMJobResponseSet::JobType::ThumbnailGenerator)
  {
    QString thumbnailPath = d->DicomDatabase->thumbnailPathForInstance(
      td.StudyInstanceUID, td.SeriesInstanceUID, td.SOPInstanceUID);
    this->onThumbnailGenerated(td.SeriesInstanceUID, thumbnailPath);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::onJobFailed(const QVariant& data)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.SeriesInstanceUID.isEmpty())
  {
    return;
  }

  int linearIndex = d->findSeriesLinearIndex(td.SeriesInstanceUID);
  if (linearIndex < 0 || linearIndex >= d->SeriesList.size())
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
    seriesData.isCloud = true;
    seriesData.operationStatus = OperationStatus::Failed;
    QModelIndex index = this->createIndex(linearIndex, 0);
    emit this->dataChanged(index, index, {IsCloudRole, OperationStatusRole});
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::onJobUserStopped(const QVariant& data)
{
  Q_D(ctkDICOMSeriesModel);
  if (d->IsUpdating)
  {
    return;
  }

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() || td.SeriesInstanceUID.isEmpty())
  {
    return;
  }

  int linearIndex = d->findSeriesLinearIndex(td.SeriesInstanceUID);
  if (linearIndex < 0 || linearIndex >= d->SeriesList.size())
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
      td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
    seriesData.isCloud = true;
    seriesData.operationStatus = OperationStatus::Failed;
    QModelIndex index = this->createIndex(linearIndex, 0);
    emit this->dataChanged(index, index, {IsCloudRole, OperationStatusRole});
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::onThumbnailGenerated(const QString& seriesInstanceUID, const QString& thumbnailPath)
{
  Q_D(ctkDICOMSeriesModel);

  int linearIndex = d->findSeriesLinearIndex(seriesInstanceUID);
  if (linearIndex < 0)
  {
    return;
  }

  ctkDICOMSeriesModelPrivate::SeriesData& seriesData = d->SeriesList[linearIndex];
  if (!thumbnailPath.isEmpty() && QFile::exists(thumbnailPath))
  {
    QPixmap thumbnail(thumbnailPath);
    if (!thumbnail.isNull())
    {
      // Scale and cache the thumbnail
      seriesData.thumbnailPath = thumbnailPath;
      seriesData.thumbnailGenerated = true;

      // Notify views that thumbnail is ready - linear model uses row = linearIndex, column = 0
      QModelIndex index = this->createIndex(linearIndex, 0);
      emit this->dataChanged(index, index, {IsCloudRole, ThumbnailPathRole, ThumbnailGeneratedRole});
      emit this->thumbnailReady(index);
    }
  }
  else
  {
    seriesData.thumbnailGenerated = false;
    QModelIndex index = this->createIndex(linearIndex, 0);
    emit this->dataChanged(index, index, {ThumbnailGeneratedRole});
  }
}

//----------------------------------------------------------------------------
void ctkDICOMSeriesModel::onLoadedSeriesChanged(const QStringList &seriesInstanceUIDs)
{
  Q_D(ctkDICOMSeriesModel);

  // Track the range of indices that changed
  int firstChangedIndex = -1;
  int lastChangedIndex = -1;

  for (int index = 0; index < d->SeriesList.size(); ++index)
  {
    ctkDICOMSeriesModelPrivate::SeriesData& series = d->SeriesList[index];
    bool hasBeenLoaded = seriesInstanceUIDs.contains(series.seriesInstanceUID);
    bool isLoaded = series.isLoaded;
    if (hasBeenLoaded == isLoaded)
    {
      continue;
    }
    series.isLoaded = hasBeenLoaded;

    // Track the range of changed indices
    if (firstChangedIndex == -1)
    {
      firstChangedIndex = index;
    }
    lastChangedIndex = index;
  }

  // Emit a single dataChanged signal for the entire range of changed indices
  if (firstChangedIndex != -1 && lastChangedIndex != -1)
  {
    QModelIndex topLeft = this->createIndex(firstChangedIndex, 0);
    QModelIndex bottomRight = this->createIndex(lastChangedIndex, 0);
    emit this->dataChanged(topLeft, bottomRight, {IsLoadedRole});
  }
}
