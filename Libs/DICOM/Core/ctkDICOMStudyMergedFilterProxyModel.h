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

#ifndef __ctkDICOMStudyMergedFilterProxyModel_h
#define __ctkDICOMStudyMergedFilterProxyModel_h

// Qt includes
#include <QAbstractListModel>
#include <QList>

#include "ctkDICOMCoreExport.h"

class ctkDICOMStudyModel;
class ctkDICOMStudyFilterProxyModel;
class ctkDICOMStudyMergedFilterProxyModelPrivate;

/// \ingroup DICOM_Core
/// \brief Merged proxy model that combines multiple study filter proxy models
///
/// This model merges rows from multiple ctkDICOMStudyFilterProxyModel instances
/// and sorts them by study date and time (most recent first).
/// Useful for displaying studies from multiple patients in a single view.
///
class CTK_DICOM_CORE_EXPORT ctkDICOMStudyMergedFilterProxyModel : public QAbstractListModel
{
  Q_OBJECT

public:
  typedef QAbstractListModel Superclass;

  explicit ctkDICOMStudyMergedFilterProxyModel(QObject* parent = nullptr);
  virtual ~ctkDICOMStudyMergedFilterProxyModel();

  /// Set the list of source proxy models to merge
  Q_INVOKABLE void setSourceProxyFilterModels(const QList<ctkDICOMStudyFilterProxyModel*>& models);

  /// Clear all source models
  Q_INVOKABLE void clearSourceProxyFilterModels();

  /// Refresh studies by rebuilding the merged rows
  Q_INVOKABLE void refreshStudies(QList<ctkDICOMStudyModel*> models = QList<ctkDICOMStudyModel*>());

  /// Get the list of source filter proxy models
  Q_INVOKABLE QList<ctkDICOMStudyFilterProxyModel*> sourceProxyFilterModels() const;
  Q_INVOKABLE QList<ctkDICOMStudyModel*> sourceModels() const;

  /// Reimplemented from QAbstractListModel
  Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  Q_INVOKABLE QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  /// Map from merged index to source model and source index
  Q_INVOKABLE QPair<ctkDICOMStudyModel*, QModelIndex> mapToSource(const QModelIndex& proxyIndex) const;

  /// Map from source model and source index to merged index
  Q_INVOKABLE QModelIndex mapFromSource(const QPair<ctkDICOMStudyModel*, QModelIndex>& sourceInfo) const;

protected slots:
  void onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
  void onSourceRowsInserted(const QModelIndex& parent, int first, int last);
  void onSourceRowsRemoved(const QModelIndex& parent, int first, int last);
  void onSourceModelReset();

protected:
  QScopedPointer<ctkDICOMStudyMergedFilterProxyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMStudyMergedFilterProxyModel);
  Q_DISABLE_COPY(ctkDICOMStudyMergedFilterProxyModel);
};

#endif
