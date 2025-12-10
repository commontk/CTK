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

#ifndef __ctkDICOMPatientFilterProxyModel_h
#define __ctkDICOMPatientFilterProxyModel_h

// Qt includes
#include <QSortFilterProxyModel>

#include "ctkDICOMCoreExport.h"

class ctkDICOMPatientFilterProxyModelPrivate;

/// \ingroup DICOM_Core
/// \brief Proxy model for filtering patient items based on visibility and display mode
///
/// This proxy model filters ctkDICOMPatientModel items based on:
/// - IsVisibleRole: Only shows items marked as visible
/// - Display mode: In TabMode, limits to available width
///
class CTK_DICOM_CORE_EXPORT ctkDICOMPatientFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_PROPERTY(DisplayMode displayMode READ displayMode WRITE setDisplayMode NOTIFY displayModeChanged)

public:
  typedef QSortFilterProxyModel Superclass;

  /// Display mode options for filtering behavior
  enum DisplayMode
  {
    TabMode = 0,  ///< Limit to available width
    ListMode      ///< Show all visible items
  };
  Q_ENUM(DisplayMode)

  explicit ctkDICOMPatientFilterProxyModel(QObject* parent = nullptr);
  virtual ~ctkDICOMPatientFilterProxyModel();

  /// Get/Set the display mode
  DisplayMode displayMode() const;
  void setDisplayMode(DisplayMode mode);

  /// Get/Set the current widget width for filtering in TabMode
  void setWidgetWidth(int width);
  int widgetWidth() const;

  /// Get/Set spacing between icon and text
  void setSpacing(int spacing);
  int spacing() const;

  /// Get/Set icon size
  void setIconSize(int iconSize);
  int iconSize() const;

  /// Get/Set maximum text width
  void setMaxTextWidth(int maxTextWidth);
  int maxTextWidth() const;

  /// Reimplemented from QSortFilterProxyModel
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
  bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

Q_SIGNALS:
  void displayModeChanged(DisplayMode mode);

protected:
  QScopedPointer<ctkDICOMPatientFilterProxyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMPatientFilterProxyModel);
  Q_DISABLE_COPY(ctkDICOMPatientFilterProxyModel);
};

#endif
