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

#ifndef CTKDICOMTABLEVIEW_H
#define CTKDICOMTABLEVIEW_H

#include "ctkDICOMWidgetsExport.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// Qt includes
#include <QWidget>
#include <QItemSelection>

class ctkDICOMTableViewPrivate;

/// \ingroup DICOM_Widgets

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMTableView : public QWidget
{
  Q_OBJECT

public:
  typedef QWidget Superclass;

  explicit ctkDICOMTableView(QWidget* parent = 0, QString queryTableName = "Patients");
  ctkDICOMTableView (QSharedPointer<ctkDICOMDatabase> ctkDicomDataBase, QWidget* parent = 0, QString queryTableName = "Patients");
  virtual ~ctkDICOMTableView();

  void setCTKDicomDataBase(QSharedPointer<ctkDICOMDatabase> dicomDataBase);

  // Settings for querying the database
  void setQueryTableName(const QString &tableName);
  void setQueryForeignKey(const QString &foreignKey);
  void setQueryPrimaryKey(const QString &primaryKey);

public Q_SLOTS:
  void onSelectionChanged();
  void onUpdateQuery(const QStringList &uids);

protected Q_SLOTS:
  void onDatabaseChanged();
  void onFilterChanged();

Q_SIGNALS:
  void signalSelectionChanged(const QStringList &uids);
  void signalFilterChanged(const QStringList &uids);
  void signalQueryChanged(const QStringList &uids);

protected:
  QScopedPointer<ctkDICOMTableViewPrivate> d_ptr;

private:
  QStringList getUIDsForAllRows();

  Q_DECLARE_PRIVATE(ctkDICOMTableView)
  Q_DISABLE_COPY(ctkDICOMTableView)
};

#endif // CTKDICOMTABLEVIEW_H
