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
#include <QDateTime>
#include <QFont>
#include <QFontMetrics>

// CTK includes
#include "ctkPimpl.h"
#include "ctkDICOMPatientFilterProxyModel.h"
#include "ctkDICOMPatientModel.h"

//------------------------------------------------------------------------------
class ctkDICOMPatientFilterProxyModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMPatientFilterProxyModel);

protected:
  ctkDICOMPatientFilterProxyModel* const q_ptr;

public:
  ctkDICOMPatientFilterProxyModelPrivate(ctkDICOMPatientFilterProxyModel& object);
  ~ctkDICOMPatientFilterProxyModelPrivate();

  ctkDICOMPatientFilterProxyModel::DisplayMode DisplayMode;
  int WidgetWidth = 0;
  int MaxTextWidth = 200;
  int IconSize = 24;
  int Spacing = 4;
  mutable int FirstOutOfBoundsRow = -1;

  QFont ViewWidgetFont;
};

//------------------------------------------------------------------------------
ctkDICOMPatientFilterProxyModelPrivate::ctkDICOMPatientFilterProxyModelPrivate(
  ctkDICOMPatientFilterProxyModel& object)
  : q_ptr(&object)
{
  this->DisplayMode = ctkDICOMPatientFilterProxyModel::TabMode;
}

//------------------------------------------------------------------------------
ctkDICOMPatientFilterProxyModelPrivate::~ctkDICOMPatientFilterProxyModelPrivate() = default;

//------------------------------------------------------------------------------
// ctkDICOMPatientFilterProxyModel methods

CTK_SET_CPP(ctkDICOMPatientFilterProxyModel, const QFont&, setViewWidgetFont, ViewWidgetFont);
CTK_GET_CPP(ctkDICOMPatientFilterProxyModel, QFont, viewWidgetFont, ViewWidgetFont)

//------------------------------------------------------------------------------
ctkDICOMPatientFilterProxyModel::ctkDICOMPatientFilterProxyModel(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMPatientFilterProxyModelPrivate(*this))
{
  // Enable dynamic sorting/filtering
  this->setDynamicSortFilter(true);

  // Enable sorting and set sort by insert date time
  this->setSortRole(ctkDICOMPatientModel::PatientInsertDateTimeRole);
  this->sort(0, Qt::DescendingOrder);
}

//------------------------------------------------------------------------------
ctkDICOMPatientFilterProxyModel::~ctkDICOMPatientFilterProxyModel() = default;

//------------------------------------------------------------------------------
ctkDICOMPatientFilterProxyModel::DisplayMode ctkDICOMPatientFilterProxyModel::displayMode() const
{
  Q_D(const ctkDICOMPatientFilterProxyModel);
  return d->DisplayMode;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientFilterProxyModel::setDisplayMode(DisplayMode mode)
{
  Q_D(ctkDICOMPatientFilterProxyModel);

  if (d->DisplayMode == mode)
  {
    return;
  }

  d->DisplayMode = mode;

  // Trigger re-filtering
  this->invalidateFilter();

  emit displayModeChanged(mode);
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientFilterProxyModel::filterAcceptsRow(int source_row,
                                                       const QModelIndex& source_parent) const
{
  Q_D(const ctkDICOMPatientFilterProxyModel);

  if (!this->sourceModel())
  {
    return false;
  }

  QModelIndex sourceIndex = this->sourceModel()->index(source_row, 0, source_parent);
  if (!sourceIndex.isValid())
  {
    return false;
  }

  // Check IsVisibleRole - always filter out invisible items
  bool isVisible = sourceIndex.data(ctkDICOMPatientModel::IsVisibleRole).toBool();
  if (!isVisible)
  {
    return false;
  }

  // In TabMode, filter out items that do not fit the widget width
  if (d->DisplayMode == ctkDICOMPatientFilterProxyModel::TabMode)
  {
    // Optimization: if we've already found the first out-of-bounds row, skip all after
    if (d->FirstOutOfBoundsRow >= 0 && source_row >= d->FirstOutOfBoundsRow)
    {
      return false;
    }

    // Use default font for font metrics calculation
    QFontMetrics fm(d->ViewWidgetFont);
    int iconSize = d->IconSize * 2;
    int spacing = d->Spacing * 3;
    int maxTextWidth = d->MaxTextWidth;
    int minTabWidth = 80;
    int leftMargin = static_cast<int>(spacing * 1.5);

    // Calculate cumulative width of all previous visible items
    int cumulativeWidth = leftMargin;
    for (int row = 0; row < source_row; ++row)
    {
      QModelIndex prevIndex = this->sourceModel()->index(row, 0, source_parent);
      if (!prevIndex.isValid())
      {
        continue;
      }
      bool prevVisible = prevIndex.data(ctkDICOMPatientModel::IsVisibleRole).toBool();
      if (!prevVisible)
      {
        continue;
      }
      QString prevName = prevIndex.data(ctkDICOMPatientModel::PatientNameRole).toString();
      if (prevName.isEmpty())
      {
        prevName = "Anonymous";
      }
      int prevTextWidth = fm.horizontalAdvance(prevName);
      if (prevTextWidth > maxTextWidth)
      {
        prevTextWidth = maxTextWidth;
      }
      int prevWidth = iconSize + spacing + prevTextWidth;
      prevWidth = qMax(prevWidth, minTabWidth);
      cumulativeWidth += prevWidth;
    }

    cumulativeWidth += iconSize;
    // Calculate this item's width
    QString patientName = sourceIndex.data(ctkDICOMPatientModel::PatientNameRole).toString();
    if (patientName.isEmpty())
    {
      patientName = "Anonymous";
    }
    int textWidth = fm.horizontalAdvance(patientName);
    if (textWidth > maxTextWidth)
    {
      textWidth = maxTextWidth;
    }
    int width = iconSize + spacing + textWidth;
    width = qMax(width, minTabWidth);

    // Accept only if this item fits in the widget width
    if (d->WidgetWidth > 0 && (cumulativeWidth + width) > d->WidgetWidth)
    {
      d->FirstOutOfBoundsRow = source_row;
      return false;
    }
  }

  // In ListMode, accept all visible items
  return true;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientFilterProxyModel::setWidgetWidth(int width)
{
  Q_D(ctkDICOMPatientFilterProxyModel);
  if (d->WidgetWidth != width)
  {
    d->WidgetWidth = width;
    d->FirstOutOfBoundsRow = -1;
    this->invalidateFilter();
  }
}

//------------------------------------------------------------------------------
int ctkDICOMPatientFilterProxyModel::widgetWidth() const
{
  Q_D(const ctkDICOMPatientFilterProxyModel);
  return d->WidgetWidth;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientFilterProxyModel::setMaxTextWidth(int width)
{
  Q_D(ctkDICOMPatientFilterProxyModel);
  if (d->MaxTextWidth != width)
  {
    d->MaxTextWidth = width;
    d->FirstOutOfBoundsRow = -1;
    this->invalidateFilter();
  }
}

//------------------------------------------------------------------------------
int ctkDICOMPatientFilterProxyModel::maxTextWidth() const
{
  Q_D(const ctkDICOMPatientFilterProxyModel);
  return d->MaxTextWidth;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientFilterProxyModel::setIconSize(int size)
{
  Q_D(ctkDICOMPatientFilterProxyModel);
  if (d->IconSize != size)
  {
    d->IconSize = size;
    d->FirstOutOfBoundsRow = -1;
    this->invalidateFilter();
  }
}

//------------------------------------------------------------------------------
int ctkDICOMPatientFilterProxyModel::iconSize() const
{
  Q_D(const ctkDICOMPatientFilterProxyModel);
  return d->IconSize;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientFilterProxyModel::setSpacing(int spacing)
{
  Q_D(ctkDICOMPatientFilterProxyModel);
  if (d->Spacing != spacing)
  {
    d->Spacing = spacing;
    d->FirstOutOfBoundsRow = -1;
    this->invalidateFilter();
  }
}

//------------------------------------------------------------------------------
int ctkDICOMPatientFilterProxyModel::spacing() const
{
  Q_D(const ctkDICOMPatientFilterProxyModel);
  return d->Spacing;
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientFilterProxyModel::lessThan(const QModelIndex& source_left,
                                               const QModelIndex& source_right) const
{
  if (!this->sourceModel())
  {
    return false;
  }

  // Sort by insert date time in descending order (most recent first)
  QDateTime leftDateTime = source_left.data(ctkDICOMPatientModel::PatientInsertDateTimeRole).toDateTime();
  QDateTime rightDateTime = source_right.data(ctkDICOMPatientModel::PatientInsertDateTimeRole).toDateTime();

  // If either date is invalid, put it at the end
  if (!leftDateTime.isValid() && !rightDateTime.isValid())
  {
    return false;
  }
  if (!leftDateTime.isValid())
  {
    return false; // Invalid left goes after valid right
  }
  if (!rightDateTime.isValid())
  {
    return true; // Valid left goes before invalid right
  }

  // Sort in descending order (newer first)
  return leftDateTime > rightDateTime;
}
