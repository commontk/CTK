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

#ifndef __ctkDICOMSeriesFilterProxyModel_h
#define __ctkDICOMSeriesFilterProxyModel_h

// Qt includes
#include <QSortFilterProxyModel>

#include "ctkDICOMCoreExport.h"

class ctkDICOMSeriesFilterProxyModelPrivate;

/// \ingroup DICOM_Core
/// \brief Proxy model for filtering series items based on visibility
///
/// This proxy model filters ctkDICOMSeriesModel items based on IsVisibleRole
///
class CTK_DICOM_CORE_EXPORT ctkDICOMSeriesFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_PROPERTY(int gridColumns READ gridColumns WRITE setGridColumns NOTIFY gridColumnsChanged)

public:
  typedef QSortFilterProxyModel Superclass;

  explicit ctkDICOMSeriesFilterProxyModel(QObject* parent = nullptr);
  virtual ~ctkDICOMSeriesFilterProxyModel();

  /// Grid layout configuration for table display
  void setGridColumns(int columns);
  int gridColumns() const;

  /// Find the grid-based index for a specific series instance UID
  QModelIndex indexForSeriesInstanceUID(const QString& seriesInstanceUID) const;

  /// Reimplemented from QSortFilterProxyModel
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

  /// Reimplemented from QSortFilterProxyModel
  /// Sorts series by series number (ascending order)
  bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

  /// Reimplemented from QAbstractItemModel to handle grid layout
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& child) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;

signals:
  /// Emitted when grid columns change
  void gridColumnsChanged(int columns);

protected slots:
  /// Override to properly handle dataChanged signals from source model
  /// and emit with grid-based coordinates
  void onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

protected:
  QScopedPointer<ctkDICOMSeriesFilterProxyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMSeriesFilterProxyModel);
  Q_DISABLE_COPY(ctkDICOMSeriesFilterProxyModel);
};

#endif
