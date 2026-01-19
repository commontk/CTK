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

// CTK includes
#include "ctkDICOMStudyFilterProxyModel.h"
#include "ctkDICOMStudyModel.h"

//------------------------------------------------------------------------------
class ctkDICOMStudyFilterProxyModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMStudyFilterProxyModel);

protected:
  ctkDICOMStudyFilterProxyModel* const q_ptr;

public:
  ctkDICOMStudyFilterProxyModelPrivate(ctkDICOMStudyFilterProxyModel& object);
  ~ctkDICOMStudyFilterProxyModelPrivate();
};

//------------------------------------------------------------------------------
ctkDICOMStudyFilterProxyModelPrivate::ctkDICOMStudyFilterProxyModelPrivate(
  ctkDICOMStudyFilterProxyModel& object)
  : q_ptr(&object)
{
}

//------------------------------------------------------------------------------
ctkDICOMStudyFilterProxyModelPrivate::~ctkDICOMStudyFilterProxyModelPrivate() = default;

//------------------------------------------------------------------------------
// ctkDICOMStudyFilterProxyModel methods

//------------------------------------------------------------------------------
ctkDICOMStudyFilterProxyModel::ctkDICOMStudyFilterProxyModel(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMStudyFilterProxyModelPrivate(*this))
{
  // Enable dynamic sorting/filtering
  this->setDynamicSortFilter(true);

  // Enable sorting and set to descending order (most recent first)
  this->setSortRole(ctkDICOMStudyModel::StudyDateRole);
  this->sort(0, Qt::DescendingOrder);
}

//------------------------------------------------------------------------------
ctkDICOMStudyFilterProxyModel::~ctkDICOMStudyFilterProxyModel() = default;

//------------------------------------------------------------------------------
bool ctkDICOMStudyFilterProxyModel::filterAcceptsRow(
  int source_row, const QModelIndex& source_parent) const
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
  bool isVisible = sourceIndex.data(ctkDICOMStudyModel::IsVisibleRole).toBool();
  return isVisible;
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyFilterProxyModel::lessThan(
  const QModelIndex& source_left, const QModelIndex& source_right) const
{
  if (!this->sourceModel())
  {
    return false;
  }

  // Get study dates and times for comparison
  QString leftDate = source_left.data(ctkDICOMStudyModel::StudyDateRole).toString();
  QString rightDate = source_right.data(ctkDICOMStudyModel::StudyDateRole).toString();
  QString leftTime = source_left.data(ctkDICOMStudyModel::StudyTimeRole).toString();
  QString rightTime = source_right.data(ctkDICOMStudyModel::StudyTimeRole).toString();

  // Handle empty dates by putting them at the end
  if (leftDate.isEmpty() && rightDate.isEmpty())
  {
    return false;
  }
  if (leftDate.isEmpty())
  {
    return true; // left is "greater" (goes to end), so return true for lessThan
  }
  if (rightDate.isEmpty())
  {
    return false; // right is "greater" (goes to end), so left is less
  }

  // Compare dates first (YYYYMMDD format)
  if (leftDate != rightDate)
  {
    // For descending order (most recent first), we want left > right to return false
    // This makes newer dates appear first
    return leftDate < rightDate;
  }

  // If dates are equal, compare times (HHMMSS format)
  // Handle empty times by putting them at the end for the same date
  if (leftTime.isEmpty() && rightTime.isEmpty())
  {
    return false;
  }
  if (leftTime.isEmpty())
  {
    return true; // left is "greater" (goes to end)
  }
  if (rightTime.isEmpty())
  {
    return false; // right is "greater" (goes to end)
  }

  // For descending order (most recent first)
  return leftTime < rightTime;
}
