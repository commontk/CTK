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

// STD includes
#include <climits>

// CTK includes
#include "ctkDICOMSeriesFilterProxyModel.h"
#include "ctkDICOMSeriesModel.h"

//------------------------------------------------------------------------------
class ctkDICOMSeriesFilterProxyModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMSeriesFilterProxyModel);

protected:
  ctkDICOMSeriesFilterProxyModel* const q_ptr;
  int GridColumns;

public:
  ctkDICOMSeriesFilterProxyModelPrivate(ctkDICOMSeriesFilterProxyModel& object);
  ~ctkDICOMSeriesFilterProxyModelPrivate();
};

//------------------------------------------------------------------------------
ctkDICOMSeriesFilterProxyModelPrivate::ctkDICOMSeriesFilterProxyModelPrivate(
  ctkDICOMSeriesFilterProxyModel& object)
  : q_ptr(&object)
{
  // Default to a large number of columns (effectively single row)
  // ctkDICOMSeriesTableView::calculateOptimalColumns will adjust this based on viewport width
  this->GridColumns = 1000;
}

//------------------------------------------------------------------------------
ctkDICOMSeriesFilterProxyModelPrivate::~ctkDICOMSeriesFilterProxyModelPrivate() = default;

//------------------------------------------------------------------------------
// ctkDICOMSeriesFilterProxyModel methods

//------------------------------------------------------------------------------
ctkDICOMSeriesFilterProxyModel::ctkDICOMSeriesFilterProxyModel(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMSeriesFilterProxyModelPrivate(*this))
{
  // Enable dynamic sorting/filtering
  this->setDynamicSortFilter(true);

  // Enable sorting by series number (ascending order)
  this->setSortRole(ctkDICOMSeriesModel::SeriesNumberRole);
  this->sort(0, Qt::AscendingOrder);

  // Connect to handle dataChanged from source model
  // We need to do this to transform linear indices to grid indices
  connect(this, &QAbstractProxyModel::sourceModelChanged, this, [this]() {
    if (this->sourceModel())
    {
      connect(this->sourceModel(), &QAbstractItemModel::dataChanged,
              this, &ctkDICOMSeriesFilterProxyModel::onSourceDataChanged);
    }
  });
}

//------------------------------------------------------------------------------
ctkDICOMSeriesFilterProxyModel::~ctkDICOMSeriesFilterProxyModel() = default;

//----------------------------------------------------------------------------
void ctkDICOMSeriesFilterProxyModel::setGridColumns(int columns)
{
  Q_D(ctkDICOMSeriesFilterProxyModel);
  if (d->GridColumns == columns)
  {
    return;
  }

  // Since changing columns affects row/column count, we need to reset the model
  this->beginResetModel();
  d->GridColumns = columns;
  this->endResetModel();

  emit this->gridColumnsChanged(columns);
}

//----------------------------------------------------------------------------
int ctkDICOMSeriesFilterProxyModel::gridColumns() const
{
  Q_D(const ctkDICOMSeriesFilterProxyModel);
  return d->GridColumns;
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesFilterProxyModel::filterAcceptsRow(int source_row,
                                                      const QModelIndex& source_parent) const
{
  if (!this->sourceModel())
  {
    return false;
  }

  QModelIndex sourceIndex = this->sourceModel()->index(source_row, 0, source_parent);
  if (!sourceIndex.isValid())
  {
    return false;
  }

  // Check IsVisibleRole - only show visible items
  bool isVisible = sourceIndex.data(ctkDICOMSeriesModel::IsVisibleRole).toBool();
  return isVisible;
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesFilterProxyModel::lessThan(
  const QModelIndex& source_left, const QModelIndex& source_right) const
{
  if (!this->sourceModel())
  {
    return false;
  }

  // Get series numbers for comparison
  QString leftSeriesNumber = source_left.data(ctkDICOMSeriesModel::SeriesNumberRole).toString();
  QString rightSeriesNumber = source_right.data(ctkDICOMSeriesModel::SeriesNumberRole).toString();

  // Convert to integers for proper numerical sorting
  bool leftOk = false;
  bool rightOk = false;
  int leftNumber = leftSeriesNumber.toInt(&leftOk);
  int rightNumber = rightSeriesNumber.toInt(&rightOk);

  // Handle cases where conversion fails
  if (!leftOk && !rightOk)
  {
    // Both are not valid numbers, compare as strings
    return leftSeriesNumber < rightSeriesNumber;
  }
  if (!leftOk)
  {
    // Left is not a valid number, put it at the end
    return false;
  }
  if (!rightOk)
  {
    // Right is not a valid number, put it at the end
    return true;
  }

  // Both are valid numbers, compare numerically (ascending order)
  return leftNumber < rightNumber;
}

//------------------------------------------------------------------------------
int ctkDICOMSeriesFilterProxyModel::rowCount(const QModelIndex& parent) const
{
  Q_D(const ctkDICOMSeriesFilterProxyModel);

  if (parent.isValid())
  {
    return 0; // Only support flat models
  }

  // Get the total number of items from the base proxy model
  int totalItems = Superclass::rowCount(parent);
  if (d->GridColumns <= 0)
  {
    return 0;
  }

  // Calculate rows needed for grid layout
  return (totalItems + d->GridColumns - 1) / d->GridColumns; // Ceiling division
}

//------------------------------------------------------------------------------
int ctkDICOMSeriesFilterProxyModel::columnCount(const QModelIndex& parent) const
{
  Q_D(const ctkDICOMSeriesFilterProxyModel);

  if (parent.isValid())
  {
    return 0; // Only support flat models
  }

  return d->GridColumns;
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMSeriesFilterProxyModel::index(int row, int column, const QModelIndex& parent) const
{
  Q_D(const ctkDICOMSeriesFilterProxyModel);

  if (parent.isValid() || row < 0 || column < 0 || column >= d->GridColumns)
  {
    return QModelIndex();
  }

  // Convert grid coordinates to linear index
  int linearIndex = row * d->GridColumns + column;

  // Check if this linear index is valid
  int totalItems = Superclass::rowCount(QModelIndex());
  if (linearIndex >= totalItems)
  {
    return QModelIndex(); // Beyond available items
  }

  // Get the source index for this linear position
  QModelIndex proxyLinearIndex = Superclass::index(linearIndex, 0, QModelIndex());
  if (!proxyLinearIndex.isValid())
  {
    return QModelIndex();
  }

  // Return a grid-based index that maps to this linear position
  return createIndex(row, column, proxyLinearIndex.internalPointer());
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMSeriesFilterProxyModel::parent(const QModelIndex& child) const
{
  Q_UNUSED(child);
  return QModelIndex(); // Flat model, no parent
}

//------------------------------------------------------------------------------
QVariant ctkDICOMSeriesFilterProxyModel::data(const QModelIndex& index, int role) const
{
  Q_D(const ctkDICOMSeriesFilterProxyModel);

  if (!index.isValid())
  {
    return QVariant();
  }

  // Convert grid coordinates to linear index
  int linearIndex = index.row() * d->GridColumns + index.column();

  // Get the corresponding linear index from the sorted proxy model
  QModelIndex proxyLinearIndex = Superclass::index(linearIndex, 0, QModelIndex());
  if (!proxyLinearIndex.isValid())
  {
    return QVariant(); // Beyond available items
  }

  // Delegate to the base proxy model
  return Superclass::data(proxyLinearIndex, role);
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesFilterProxyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  Q_D(const ctkDICOMSeriesFilterProxyModel);

  if (!index.isValid())
  {
    return false;
  }

  // Convert grid coordinates to linear index
  int linearIndex = index.row() * d->GridColumns + index.column();

  // Get the corresponding linear index from the sorted proxy model
  QModelIndex proxyLinearIndex = Superclass::index(linearIndex, 0, QModelIndex());
  if (!proxyLinearIndex.isValid())
  {
    return false; // Beyond available items
  }

  // Delegate to the base proxy model
  return Superclass::setData(proxyLinearIndex, value, role);
}

//------------------------------------------------------------------------------
Qt::ItemFlags ctkDICOMSeriesFilterProxyModel::flags(const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesFilterProxyModel);

  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }

  // Convert grid coordinates to linear index
  int linearIndex = index.row() * d->GridColumns + index.column();

  // Get the corresponding linear index from the sorted proxy model
  QModelIndex proxyLinearIndex = Superclass::index(linearIndex, 0, QModelIndex());
  if (!proxyLinearIndex.isValid())
  {
    return Qt::NoItemFlags; // Beyond available items
  }

  // Delegate to the base proxy model
  return Superclass::flags(proxyLinearIndex);
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMSeriesFilterProxyModel::indexForSeriesInstanceUID(const QString& seriesInstanceUID) const
{
  Q_D(const ctkDICOMSeriesFilterProxyModel);

  if (!this->sourceModel())
  {
    return QModelIndex();
  }

  // First, find the linear index in our sorted proxy model
  int totalItems = Superclass::rowCount(QModelIndex());
  for (int linearIndex = 0; linearIndex < totalItems; ++linearIndex)
  {
    QModelIndex proxyLinearIndex = Superclass::index(linearIndex, 0, QModelIndex());
    if (proxyLinearIndex.isValid())
    {
      QString uid = Superclass::data(proxyLinearIndex, ctkDICOMSeriesModel::SeriesInstanceUIDRole).toString();
      if (uid == seriesInstanceUID)
      {
        // Convert linear index to grid coordinates
        int row = linearIndex / d->GridColumns;
        int column = linearIndex % d->GridColumns;
        return this->index(row, column); // Use our own index method
      }
    }
  }

  return QModelIndex();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesFilterProxyModel::onSourceDataChanged(
  const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight, const QVector<int>& roles)
{
  Q_D(ctkDICOMSeriesFilterProxyModel);

  // The source model emits linear indices (single column, multiple rows)
  // We need to map each changed source index to its grid position and emit
  // dataChanged with the grid coordinates

  if (!sourceTopLeft.isValid() || !sourceBottomRight.isValid())
  {
    return;
  }

  // Collect all affected grid indices
  int minRow = INT_MAX;
  int maxRow = -1;
  int minCol = INT_MAX;
  int maxCol = -1;

  // Iterate through all source rows that changed
  for (int sourceRow = sourceTopLeft.row(); sourceRow <= sourceBottomRight.row(); ++sourceRow)
  {
    QModelIndex sourceIndex = this->sourceModel()->index(sourceRow, 0);
    if (!sourceIndex.isValid())
    {
      continue;
    }

    // Map source index through the filter/sort proxy
    QModelIndex proxyLinearIndex = this->mapFromSource(sourceIndex);
    if (!proxyLinearIndex.isValid())
    {
      continue; // Filtered out
    }

    // Get the linear index in the sorted/filtered list
    int linearIndex = proxyLinearIndex.row();

    // Convert linear index to grid coordinates
    int gridRow = linearIndex / d->GridColumns;
    int gridCol = linearIndex % d->GridColumns;

    // Track the bounding box of affected grid cells
    minRow = qMin(minRow, gridRow);
    maxRow = qMax(maxRow, gridRow);
    minCol = qMin(minCol, gridCol);
    maxCol = qMax(maxCol, gridCol);
  }

  // Emit dataChanged with grid coordinates if we have valid bounds
  if (minRow != INT_MAX && maxRow != -1 && minCol != INT_MAX && maxCol != -1)
  {
    QModelIndex gridTopLeft = this->index(minRow, minCol);
    QModelIndex gridBottomRight = this->index(maxRow, maxCol);

    if (gridTopLeft.isValid() && gridBottomRight.isValid())
    {
      emit this->dataChanged(gridTopLeft, gridBottomRight, roles);
    }
  }
}
