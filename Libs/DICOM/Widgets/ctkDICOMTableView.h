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
  Q_PROPERTY(bool filterActive READ filterActive)

public:
  typedef QWidget Superclass;

  /**
   * Constructs ctkDICOMTableView without underlying database and table name
   * @param parent parentwidget
   */
  explicit ctkDICOMTableView(QWidget* parent = 0);
  /**
   * ctor with tablename as parameter
   * @param parent the parent widget
   * @param queryTableName the name of the table of the ctkDICOMDatabase which shall be displayed
   */
  explicit ctkDICOMTableView(QString queryTableName, QWidget* parent = 0);

  /**
   * ctor with tablename and database as parameter
   * @param ctkDicomDataBase the ctkDICOMDatabase which shall be used
   * @param parent the parent widget
   * @param queryTableName the name of the table of the ctkDICOMDatabase which shall be displayed
   */
  explicit ctkDICOMTableView (ctkDICOMDatabase* dicomDataBase, QString queryTableName, QWidget* parent = 0);

  virtual ~ctkDICOMTableView();

  /**
   * @brief Setting the ctkDICOMDatabase which shall be queried
   * @param dicomDataBase the underlying database
   */
  void setDicomDataBase(ctkDICOMDatabase* dicomDatabase);

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

  /**
   * @brief Add a where condition to the usual select statement
   * @param condition std::pair with column name and a value list
   */
  void addSqlWhereCondition(const std::pair<QString, QStringList>& condition);

  /**
   * @brief Returns the uids of the current selected rows
   * @return a list containing all the uids of the selected rows
   */
  QStringList currentSelection() const;

  /**
   * @brief Getting the UIDs for all rows
   * @return a QStringList with the uids for all rows
   */
  QStringList uidsForAllRows() const;

  bool filterActive();

  void setTableSectionSize(int);
  int tableSectionSize();

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

  /**
   * @brief Called if a new instance was added to the database
   */
  void onInstanceAdded();

  void selectAll();

protected:
  virtual bool eventFilter(QObject *obj, QEvent *event);

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

  void doubleClicked(const QModelIndex&);

protected:
  QScopedPointer<ctkDICOMTableViewPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkDICOMTableView)
  Q_DISABLE_COPY(ctkDICOMTableView)
};

#endif // __ctkDICOMTableView_h
