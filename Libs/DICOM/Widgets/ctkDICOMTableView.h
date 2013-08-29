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

#ifndef __ctkDICOMTableView_h
#define __ctkDICOMTableView_h

// Qt includes
#include <QItemSelection>
#include <QWidget>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMTableViewPrivate;

/**
 * @brief The ctkDICOMTableView displays the content of a specific table of the ctkDICOMDatabase
 *
 * The ctkDICOMTableView holds a QTableView which displays the content of the selected
 * ctkDICOMDatabase. It also holds a ctkSearchBox which allows filtering of the table content.
 *
 * @ingroup DICOM_Widgets
 */
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMTableView : public QWidget
{
  Q_OBJECT

public:
  typedef QWidget Superclass;

  /**
   * ctor with tablename as parameter
   * @param parent the parent widget
   * @param queryTableName the name of the table of the ctkDICOMDatabase which shall be displayed
   */
  explicit ctkDICOMTableView(QWidget* parent = 0, QString queryTableName = "Patients");

  /**
   * ctor with tablename and database as parameter
   * @param ctkDicomDataBase the ctkDICOMDatabase which shall be used
   * @param parent the parent widget
   * @param queryTableName the name of the table of the ctkDICOMDatabase which shall be displayed
   */
  explicit ctkDICOMTableView (QSharedPointer<ctkDICOMDatabase> ctkDicomDataBase, QWidget* parent = 0, QString queryTableName = "Patients");

  virtual ~ctkDICOMTableView();

  /**
   * @brief Setting the ctkDICOMDatabase which shall be queried
   * @param dicomDataBase the underlying database
   */
  void setCTKDicomDataBase(QSharedPointer<ctkDICOMDatabase> dicomDatabase);

  /**
   * Setting the table name which shall be used for the database query
   * @param tableName the name of the database table
   */
  void setQueryTableName(const QString &tableName);

  /**
   * Setting the foreign key for the database query. This is usefull if e.g. you
   * want to select the studies for a certain patient
   * @param foreignKey the foreign key which will be used for the query
   */
  void setQueryForeignKey(const QString &foreignKey);

  /**
   * Set the query for the underlying database. If the uid list is not empty just the
   * entries with the according uids are selected
   * @param uids a list of uids which should be selected
   */
  void setQuery (const QStringList &uids = QStringList());

public Q_SLOTS:

  /**
   * @brief slot is called if the selection of the tableview is changed
   * Within this slot the signal signalSelectionChanged is emitted
   */
  void onSelectionChanged();

  /**
   * @brief Updates the query which is used for displaying the table content
   * @param uids the uids of the table entries which shall be displayed
   */
  void onUpdateQuery(const QStringList &uids);

protected Q_SLOTS:
  /**
   * @brief Called when the underlying database changes
   */
  void onDatabaseChanged();

  /**
   * @brief Called when the text of the ctkSearchBox has changed
   */
  void onFilterChanged();

Q_SIGNALS:
  /**
   * @brief Is emitted when the selection in the tableview has changed
   * @param uids the list of uids of the selected objects
   */
  void selectionChanged(const QStringList &uids);

  /**
   * @brief Is emitted when the data selection has changed
   */
  void selectionChanged(const QItemSelection&,const QItemSelection&);

  /**
   * @brief Is emitted when the query text has changed
   * @param uids the list of uids of the objects included in the query
   */
  void queryChanged(const QStringList &uids);


protected:
  QScopedPointer<ctkDICOMTableViewPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkDICOMTableView)
  Q_DISABLE_COPY(ctkDICOMTableView)
};

#endif // CTKDICOMTABLEVIEW_H
