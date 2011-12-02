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

=========================================================================*/

#ifndef __ctkDICOMFilterProxyModel_h
#define __ctkDICOMFilterProxyModel_h

// Qt includes
#include <QSortFilterProxyModel>

#include "ctkDICOMCoreExport.h"

class ctkDICOMFilterProxyModelPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMFilterProxyModel : public QSortFilterProxyModel{
    Q_OBJECT

public:
    typedef QSortFilterProxyModel Superclass;
    explicit ctkDICOMFilterProxyModel(QObject* parent = 0);
    virtual ~ctkDICOMFilterProxyModel();

    virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

protected:
    QScopedPointer<ctkDICOMFilterProxyModelPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(ctkDICOMFilterProxyModel);
    Q_DISABLE_COPY(ctkDICOMFilterProxyModel);

public Q_SLOTS:
    void setNameSearchText(const QString& text);
    void setStudySearchText(const QString& text);
    void setSeriesSearchText(const QString& text);
    void setIdSearchText(const QString& text);
};

#endif
