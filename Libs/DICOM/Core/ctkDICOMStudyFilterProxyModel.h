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

#ifndef __ctkDICOMStudyFilterProxyModel_h
#define __ctkDICOMStudyFilterProxyModel_h

// Qt includes
#include <QSortFilterProxyModel>

#include "ctkDICOMCoreExport.h"

class ctkDICOMStudyFilterProxyModelPrivate;

/// \ingroup DICOM_Core
/// \brief Proxy model for filtering study items based on visibility
///
/// This proxy model filters ctkDICOMStudyModel items based on IsVisibleRole
///
class CTK_DICOM_CORE_EXPORT ctkDICOMStudyFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  typedef QSortFilterProxyModel Superclass;

  explicit ctkDICOMStudyFilterProxyModel(QObject* parent = nullptr);
  virtual ~ctkDICOMStudyFilterProxyModel();

  /// Reimplemented from QSortFilterProxyModel
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

  /// Reimplemented from QSortFilterProxyModel
  /// Sorts studies by date and time (most recent first)
  bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

protected:
  QScopedPointer<ctkDICOMStudyFilterProxyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMStudyFilterProxyModel);
  Q_DISABLE_COPY(ctkDICOMStudyFilterProxyModel);
};

#endif
