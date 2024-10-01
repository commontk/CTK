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
#include <QDateTime>
#include <QDebug>

// CTK includes
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMStudyMergedFilterProxyModel.h"
#include "ctkDICOMStudyFilterProxyModel.h"

//------------------------------------------------------------------------------
class ctkDICOMStudyMergedFilterProxyModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMStudyMergedFilterProxyModel);

protected:
  ctkDICOMStudyMergedFilterProxyModel* const q_ptr;

public:
  ctkDICOMStudyMergedFilterProxyModelPrivate(ctkDICOMStudyMergedFilterProxyModel& object);
  ~ctkDICOMStudyMergedFilterProxyModelPrivate();

  void rebuildMergedRows();
  void connectSourceModel(ctkDICOMStudyFilterProxyModel* model);
  void disconnectSourceModel(ctkDICOMStudyFilterProxyModel* model);

  // Source models
  QList<ctkDICOMStudyFilterProxyModel*> SourceProxyFilterModels;

  // Merged rows: stores (sourceModel, sourceRow) pairs sorted by date/time
  struct MergedRow
  {
    ctkDICOMStudyFilterProxyModel* sourceProxyFilterModel;
    int sourceRow;
    QDateTime studyDateTime;
  };
  QList<MergedRow> MergedRows;
};

//------------------------------------------------------------------------------
ctkDICOMStudyMergedFilterProxyModelPrivate::ctkDICOMStudyMergedFilterProxyModelPrivate(ctkDICOMStudyMergedFilterProxyModel& object)
  : q_ptr(&object)
{
}

//------------------------------------------------------------------------------
ctkDICOMStudyMergedFilterProxyModelPrivate::~ctkDICOMStudyMergedFilterProxyModelPrivate()
{
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModelPrivate::connectSourceModel(ctkDICOMStudyFilterProxyModel* model)
{
  Q_Q(ctkDICOMStudyMergedFilterProxyModel);
  if (!model)
  {
    return;
  }

  QObject::connect(model, &QAbstractItemModel::dataChanged,
                   q, &ctkDICOMStudyMergedFilterProxyModel::onSourceDataChanged);
  QObject::connect(model, &QAbstractItemModel::rowsInserted,
                   q, &ctkDICOMStudyMergedFilterProxyModel::onSourceRowsInserted);
  QObject::connect(model, &QAbstractItemModel::rowsRemoved,
                   q, &ctkDICOMStudyMergedFilterProxyModel::onSourceRowsRemoved);
  QObject::connect(model, &QAbstractItemModel::modelReset,
                   q, &ctkDICOMStudyMergedFilterProxyModel::onSourceModelReset);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModelPrivate::disconnectSourceModel(ctkDICOMStudyFilterProxyModel* model)
{
  Q_Q(ctkDICOMStudyMergedFilterProxyModel);
  if (!model)
  {
    return;
  }

  QObject::disconnect(model, &QAbstractItemModel::dataChanged,
                      q, &ctkDICOMStudyMergedFilterProxyModel::onSourceDataChanged);
  QObject::disconnect(model, &QAbstractItemModel::rowsInserted,
                      q, &ctkDICOMStudyMergedFilterProxyModel::onSourceRowsInserted);
  QObject::disconnect(model, &QAbstractItemModel::rowsRemoved,
                      q, &ctkDICOMStudyMergedFilterProxyModel::onSourceRowsRemoved);
  QObject::disconnect(model, &QAbstractItemModel::modelReset,
                      q, &ctkDICOMStudyMergedFilterProxyModel::onSourceModelReset);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModelPrivate::rebuildMergedRows()
{
  this->MergedRows.clear();

  // Collect all rows from all source models (index-based loop for Qt container safety)
  for (int sourceModelIndex = 0; sourceModelIndex < this->SourceProxyFilterModels.size(); ++sourceModelIndex)
  {
    ctkDICOMStudyFilterProxyModel* sourceProxyFilterModel = this->SourceProxyFilterModels.at(sourceModelIndex);
    if (!sourceProxyFilterModel)
    {
      continue;
    }

    int rowCount = sourceProxyFilterModel->rowCount();
    for (int row = 0; row < rowCount; ++row)
    {
      QModelIndex sourceProxyFilterIndex = sourceProxyFilterModel->index(row, 0);
      // Get study date and time for sorting
      QString studyDate = sourceProxyFilterModel->data(sourceProxyFilterIndex, ctkDICOMStudyModel::StudyDateRole).toString(); // StudyDateRole
      QString studyTime = sourceProxyFilterModel->data(sourceProxyFilterIndex, ctkDICOMStudyModel::StudyTimeRole).toString(); // StudyTimeRole
      // Parse date/time
      QDateTime studyDateTime;
      if (!studyDate.isEmpty())
      {
        QString dateTimeStr = studyDate;
        if (!studyTime.isEmpty())
        {
          dateTimeStr += studyTime;
        }
        studyDateTime = QDateTime::fromString(dateTimeStr, "yyyyMMddHHmmss");
      }
      MergedRow mergedRow;
      mergedRow.sourceProxyFilterModel = sourceProxyFilterModel;
      mergedRow.sourceRow = row;
      mergedRow.studyDateTime = studyDateTime;
      this->MergedRows.append(mergedRow);
    }
  }

  // Sort by date/time (most recent first)
  std::sort(this->MergedRows.begin(), this->MergedRows.end(),
            [](const MergedRow& a, const MergedRow& b) {
              return a.studyDateTime > b.studyDateTime;
            });
}

//------------------------------------------------------------------------------
ctkDICOMStudyMergedFilterProxyModel::ctkDICOMStudyMergedFilterProxyModel(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMStudyMergedFilterProxyModelPrivate(*this))
{
}

//------------------------------------------------------------------------------
ctkDICOMStudyMergedFilterProxyModel::~ctkDICOMStudyMergedFilterProxyModel() = default;

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModel::setSourceProxyFilterModels(const QList<ctkDICOMStudyFilterProxyModel*>& models)
{
  Q_D(ctkDICOMStudyMergedFilterProxyModel);
  if (d->SourceProxyFilterModels == models)
  {
    return;
  }

  this->beginResetModel();

  // Disconnect old models (index-based loop for Qt container safety)
  for (int sourceModelIndex = 0; sourceModelIndex < d->SourceProxyFilterModels.size(); ++sourceModelIndex)
  {
    ctkDICOMStudyFilterProxyModel* model = d->SourceProxyFilterModels.at(sourceModelIndex);
    d->disconnectSourceModel(model);
  }

  // Set new models
  d->SourceProxyFilterModels = models;

  // Connect new models (index-based loop for Qt container safety)
  for (int sourceModelIndex = 0; sourceModelIndex < d->SourceProxyFilterModels.size(); ++sourceModelIndex)
  {
    ctkDICOMStudyFilterProxyModel* model = d->SourceProxyFilterModels.at(sourceModelIndex);
    d->connectSourceModel(model);
  }

  // Rebuild merged rows
  d->rebuildMergedRows();

  this->endResetModel();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModel::clearSourceProxyFilterModels()
{
  Q_D(ctkDICOMStudyMergedFilterProxyModel);

  this->beginResetModel();

  // Disconnect all models (index-based loop for Qt container safety)
  for (int sourceModelIndex = 0; sourceModelIndex < d->SourceProxyFilterModels.size(); ++sourceModelIndex)
  {
    ctkDICOMStudyFilterProxyModel* model = d->SourceProxyFilterModels.at(sourceModelIndex);
    d->disconnectSourceModel(model);
  }

  d->SourceProxyFilterModels.clear();
  d->MergedRows.clear();

  this->endResetModel();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModel::refreshStudies(QList<ctkDICOMStudyModel*> models)
{
  this->beginResetModel();

  QList<ctkDICOMStudyModel*> sourceStudyModels = this->sourceModels();
  int numberOfStudyModels = sourceStudyModels.count();
  for (int index = 0; index < numberOfStudyModels; ++index)
  {
    ctkDICOMStudyModel *studySourceModel = sourceStudyModels.at(index);
    if(!studySourceModel)
    {
      continue;
    }
    if (!models.isEmpty() && !models.contains(studySourceModel))
    {
      continue;
    }
    studySourceModel->refreshStudies();
  }

  this->endResetModel();
}

//------------------------------------------------------------------------------
QList<ctkDICOMStudyFilterProxyModel*> ctkDICOMStudyMergedFilterProxyModel::sourceProxyFilterModels() const
{
  Q_D(const ctkDICOMStudyMergedFilterProxyModel);
  return d->SourceProxyFilterModels;
}

//------------------------------------------------------------------------------
QList<ctkDICOMStudyModel *> ctkDICOMStudyMergedFilterProxyModel::sourceModels() const
{
  Q_D(const ctkDICOMStudyMergedFilterProxyModel);
  QList<ctkDICOMStudyModel*> sourceModels;
  for (int sourceModelIndex = 0; sourceModelIndex < d->SourceProxyFilterModels.size(); ++sourceModelIndex)
  {
    ctkDICOMStudyFilterProxyModel* proxyModel = d->SourceProxyFilterModels.at(sourceModelIndex);
    if (!proxyModel)
    {
      continue;
    }
    ctkDICOMStudyModel* sourceModel = qobject_cast<ctkDICOMStudyModel*>(proxyModel->sourceModel());
    if (sourceModel && !sourceModels.contains(sourceModel))
    {
      sourceModels.append(sourceModel);
    }
  }
  return sourceModels;
}

//------------------------------------------------------------------------------
int ctkDICOMStudyMergedFilterProxyModel::rowCount(const QModelIndex& parent) const
{
  Q_D(const ctkDICOMStudyMergedFilterProxyModel);

  if (parent.isValid())
  {
    return 0;
  }

  return d->MergedRows.count();
}

//------------------------------------------------------------------------------
QVariant ctkDICOMStudyMergedFilterProxyModel::data(const QModelIndex& index, int role) const
{
  Q_D(const ctkDICOMStudyMergedFilterProxyModel);

  if (!index.isValid() || index.row() >= d->MergedRows.count())
  {
    return QVariant();
  }

  const ctkDICOMStudyMergedFilterProxyModelPrivate::MergedRow& mergedRow = d->MergedRows.at(index.row());
  QModelIndex sourceProxyFilterIndex = mergedRow.sourceProxyFilterModel->index(mergedRow.sourceRow, 0);

  return mergedRow.sourceProxyFilterModel->data(sourceProxyFilterIndex, role);
}

//------------------------------------------------------------------------------
QPair<ctkDICOMStudyModel*, QModelIndex> ctkDICOMStudyMergedFilterProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
  Q_D(const ctkDICOMStudyMergedFilterProxyModel);

  if (!proxyIndex.isValid() || proxyIndex.row() >= d->MergedRows.count())
  {
    return QPair<ctkDICOMStudyModel*, QModelIndex>(nullptr, QModelIndex());
  }

  const ctkDICOMStudyMergedFilterProxyModelPrivate::MergedRow& mergedRow = d->MergedRows.at(proxyIndex.row());
  QModelIndex sourceProxyFilterIndex = mergedRow.sourceProxyFilterModel->index(mergedRow.sourceRow, 0);
  ctkDICOMStudyModel* sourceModel = qobject_cast<ctkDICOMStudyModel*>(mergedRow.sourceProxyFilterModel->sourceModel());
  QModelIndex sourceIndex = mergedRow.sourceProxyFilterModel->mapToSource(sourceProxyFilterIndex);
  return QPair<ctkDICOMStudyModel*, QModelIndex>(sourceModel, sourceIndex);
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMStudyMergedFilterProxyModel::mapFromSource(const QPair<ctkDICOMStudyModel*, QModelIndex>& sourceInfo) const
{
  Q_D(const ctkDICOMStudyMergedFilterProxyModel);
  if (!sourceInfo.first || !sourceInfo.second.isValid())
  {
    return QModelIndex();
  }

         // Find the corresponding proxy index in the merged model
  for (int mergedRow = 0; mergedRow < d->MergedRows.count(); ++mergedRow)
  {
    const ctkDICOMStudyMergedFilterProxyModelPrivate::MergedRow& mergedRowInfo = d->MergedRows.at(mergedRow);
    ctkDICOMStudyModel* sourceModel = qobject_cast<ctkDICOMStudyModel*>(mergedRowInfo.sourceProxyFilterModel->sourceModel());
    QModelIndex sourceIndex = mergedRowInfo.sourceProxyFilterModel->mapToSource(
        mergedRowInfo.sourceProxyFilterModel->index(mergedRowInfo.sourceRow, 0));
    if (sourceModel == sourceInfo.first && sourceIndex == sourceInfo.second)
    {
      return this->index(mergedRow, 0);
    }
  }
  return QModelIndex();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModel::onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  emit this->dataChanged(topLeft, bottomRight, roles);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModel::onSourceRowsInserted(const QModelIndex& parent, int first, int last)
{
  Q_D(ctkDICOMStudyMergedFilterProxyModel);
  this->beginInsertRows(parent, first, last);
  d->rebuildMergedRows();
  this->endInsertRows();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModel::onSourceRowsRemoved(const QModelIndex& parent, int first, int last)
{
  Q_D(ctkDICOMStudyMergedFilterProxyModel);
  this->beginRemoveRows(parent, first, last);
  d->rebuildMergedRows();
  this->endRemoveRows();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyMergedFilterProxyModel::onSourceModelReset()
{
  Q_D(ctkDICOMStudyMergedFilterProxyModel);
  this->beginResetModel();
  d->rebuildMergedRows();
  this->endResetModel();
}
