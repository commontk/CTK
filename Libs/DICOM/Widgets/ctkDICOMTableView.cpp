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

// ctkDICOMWidget includes
#include "ctkDICOMTableView.h"
#include "ui_ctkDICOMTableView.h"

// Qt includes
#include <QJsonObject>
#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>

//------------------------------------------------------------------------------
class ctkDICOMTableViewPrivate : public Ui_ctkDICOMTableView
{
  Q_DECLARE_PUBLIC (ctkDICOMTableView)

protected:
  ctkDICOMTableView* const q_ptr;

public:
  ctkDICOMTableViewPrivate(ctkDICOMTableView& obj);
  ctkDICOMTableViewPrivate(ctkDICOMTableView& obj, ctkDICOMDatabase* db);
  ~ctkDICOMTableViewPrivate();

  /// Initialize UI and tableview with tablemodel
  void init();

  void showFilterActiveWarning(bool);

  void applyColumnProperties();

  ctkDICOMDatabase* dicomDatabase;
  QSqlQueryModel dicomSQLModel;
  QSortFilterProxyModel* dicomSQLFilterModel;
  QString queryTableName;
  QString queryForeignKey;

  QStringList currentSelection;

  bool batchUpdate;
  /// Set to true if database modification is notified while in batch update mode
  bool batchUpdateModificationPending;
  bool batchUpdateInstanceAddedPending;

  /// Key = QString for columns, Values = QStringList
  /// Specified columns need to have one of the given values
  QHash<QString, QStringList> sqlWhereConditions;

  /// Greater than SQL where conditions
  QMap<QString, QVariant> sqlGreaterWhereConditions;

  /// Less than SQL where conditions
  QMap<QString, QVariant> sqlLessWhereConditions;

  /// Greater than or equal to SQL where conditions
  QMap<QString, QVariant> sqlGreaterEqualWhereConditions;

  /// Less than or equal to SQL where conditions
  QMap<QString, QVariant> sqlLessEqualWhereConditions;
};

//------------------------------------------------------------------------------
ctkDICOMTableViewPrivate::ctkDICOMTableViewPrivate(ctkDICOMTableView &obj)
  : q_ptr(&obj)
{
  this->dicomSQLFilterModel = new QSortFilterProxyModel(&obj);
  this->dicomDatabase = new ctkDICOMDatabase(&obj);
  this->batchUpdate = false;
  this->batchUpdateModificationPending = false;
  this->batchUpdateInstanceAddedPending = false;
}

//------------------------------------------------------------------------------
ctkDICOMTableViewPrivate::ctkDICOMTableViewPrivate(ctkDICOMTableView &obj, ctkDICOMDatabase* db)
  : q_ptr(&obj)
  , dicomDatabase(db)
{
  this->dicomSQLFilterModel = new QSortFilterProxyModel(&obj);
}

//------------------------------------------------------------------------------
ctkDICOMTableViewPrivate::~ctkDICOMTableViewPrivate()
{
}

//------------------------------------------------------------------------------
void ctkDICOMTableViewPrivate::init()
{
  Q_Q(ctkDICOMTableView);
  this->setupUi(q);

  this->leSearchBox->setAlwaysShowClearIcon(true);
  this->leSearchBox->setShowSearchIcon(true);

  this->tblDicomDatabaseView->viewport()->installEventFilter(q);

  this->dicomSQLFilterModel->setSourceModel(&this->dicomSQLModel);
  this->dicomSQLFilterModel->setFilterKeyColumn(-1);
  this->dicomSQLFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->tblDicomDatabaseView->setModel(this->dicomSQLFilterModel);
  this->tblDicomDatabaseView->setSortingEnabled(true);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
  this->tblDicomDatabaseView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  this->tblDicomDatabaseView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#else
  this->tblDicomDatabaseView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
  this->tblDicomDatabaseView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#endif
  this->tblDicomDatabaseView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

  QObject::connect(this->tblDicomDatabaseView->selectionModel(),
                   SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                   q, SLOT(onSelectionChanged()));

  QObject::connect(this->tblDicomDatabaseView->selectionModel(),
                   SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                   q, SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)));

  QObject::connect(this->tblDicomDatabaseView, SIGNAL(doubleClicked(const QModelIndex&)),
                   q, SIGNAL(doubleClicked(const QModelIndex&)));

  // enable right click menu, with mapping to global position (for use within the DICOM table manager)
  this->tblDicomDatabaseView->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this->tblDicomDatabaseView,
                   SIGNAL(customContextMenuRequested(const QPoint&)),
                   q, SLOT(onCustomContextMenuRequested(const QPoint&)));

  QObject::connect(this->leSearchBox, SIGNAL(textChanged(QString)), q, SLOT(onFilterChanged(QString)));
}

//----------------------------------------------------------------------------
void ctkDICOMTableViewPrivate::showFilterActiveWarning(bool showWarning)
{
  QPalette palette;
  if (showWarning)
  {
    palette.setColor(QPalette::Base,Qt::yellow);
    palette.setColor(QPalette::Text, Qt::black);
  }
  this->leSearchBox->setPalette(palette);
}

//----------------------------------------------------------------------------
void ctkDICOMTableViewPrivate::applyColumnProperties()
{
  if (!this->dicomDatabase || !this->dicomDatabase->isOpen()
    || !this->dicomDatabase->isDisplayedFieldsTableAvailable())
  {
    return;
  }

  bool stretchedColumnFound = false;
  int defaultSortByColumn = -1;
  Qt::SortOrder defaultSortOrder = Qt::AscendingOrder;

  QHeaderView* header = this->tblDicomDatabaseView->horizontalHeader();
  int columnCount = this->dicomSQLModel.columnCount();
  QList<int> columnWeights;
  QMap<int,int> visualIndexToColumnIndexMap;
  for (int col=0; col<columnCount; ++col)
  {
    QString columnName = this->dicomSQLModel.headerData(col, Qt::Horizontal).toString();
    QString originalColumnName = this->dicomSQLModel.headerData(col, Qt::Horizontal, Qt::WhatsThisRole).toString();
    if (originalColumnName.isEmpty())
    {
      // Save original column name for future referencing the database fields
      this->dicomSQLModel.setHeaderData(col, Qt::Horizontal, columnName, Qt::WhatsThisRole);
    }
    else
    {
      columnName = originalColumnName;
      visualIndexToColumnIndexMap[header->visualIndex(col)] = col;
    }

    // Apply displayed name
    QString displayedName = this->dicomDatabase->displayedNameForField(this->queryTableName, columnName);
    this->dicomSQLModel.setHeaderData(col, Qt::Horizontal, displayedName, Qt::DisplayRole);

    // Apply visibility
    bool visibility = this->dicomDatabase->visibilityForField(this->queryTableName, columnName);
    this->tblDicomDatabaseView->setColumnHidden(col, !visibility);

    // Save weight to apply later
    int weight = this->dicomDatabase->weightForField(this->queryTableName, columnName);
    columnWeights << weight;

    QString fieldFormat = this->dicomDatabase->formatForField(this->queryTableName, columnName);
    QHeaderView::ResizeMode columnResizeMode = QHeaderView::Interactive;
    if (!fieldFormat.isEmpty())
    {
      QJsonDocument fieldFormatDoc = QJsonDocument::fromJson(fieldFormat.toUtf8());
      QJsonObject fieldFormatObj;
      if (!fieldFormatDoc.isNull())
      {
        if (fieldFormatDoc.isObject())
        {
          fieldFormatObj = fieldFormatDoc.object();
        }
      }
      if (!fieldFormatObj.isEmpty())
      {
        // format string successfully decoded from json

        // resize mode
        QString resizeModeStr = fieldFormatObj.value(QString("resizeMode")).toString("interactive");
        if (resizeModeStr == "interactive")
        {
          columnResizeMode = QHeaderView::Interactive;
        }
        else if (resizeModeStr == "stretch")
        {
          columnResizeMode = QHeaderView::Stretch;
        }
        else if (resizeModeStr == "resizeToContents")
        {
          columnResizeMode = QHeaderView::ResizeToContents;
        }
        else
        {
          qWarning() << "Invalid ColumnDisplayProperties Format string for column " << columnName << ": resizeMode must be interactive, stretch, or resizeToContents";
        }

        // default sort order
        QString sortStr = fieldFormatObj.value(QString("sort")).toString();
        if (!sortStr.isEmpty())
        {
          defaultSortByColumn = col;
          if (sortStr == "ascending")
          {
            defaultSortOrder = Qt::AscendingOrder;
          }
          else if (sortStr == "descending")
          {
            defaultSortOrder = Qt::DescendingOrder;
          }
          else
          {
            qWarning() << "Invalid ColumnDisplayProperties Format string for column " << columnName << ": sort must be ascending or descending";
          }
        }

      }
      else
      {
        // format string is specified but failed to be decoded from json
        qWarning() << "Invalid ColumnDisplayProperties Format string for column " << columnName << ": " << fieldFormat;
      }
    }
    this->tblDicomDatabaseView->horizontalHeader()->setSectionResizeMode(col, columnResizeMode);
    if (columnResizeMode == QHeaderView::Stretch && visibility)
    {
      stretchedColumnFound = true;
    }

  }

  // If no stretched column is shown then stretch the last column to make the table look nicely aligned
  this->tblDicomDatabaseView->horizontalHeader()->setStretchLastSection(!stretchedColumnFound);

  if (defaultSortByColumn >= 0)
  {
    this->tblDicomDatabaseView->sortByColumn(defaultSortByColumn, defaultSortOrder);
  }

  // First restore original order of the columns so that it can be sorted by weights (use bubble sort).
  // This extra complexity is needed because the only mechanism for column order is by moving or swapping
  bool wasBlocked = header->blockSignals(true);
  if (!visualIndexToColumnIndexMap.isEmpty())
  {
    QList<int> columnIndicesByVisualIndex = visualIndexToColumnIndexMap.values();
    int columnIndicesCount = columnIndicesByVisualIndex.size();
    for (int i=0; i<columnIndicesCount-1; ++i)
    {
      // Last i elements are already in place    
      for (int j=0; j<columnIndicesCount -i-1; ++j)
      {
        if (columnIndicesByVisualIndex[j] > columnIndicesByVisualIndex[j+1])
        {
          columnIndicesByVisualIndex.swap(j, j+1);
          header->swapSections(j, j+1);
        }
      }
    }
  }
  // Change column order according to weights (use bubble sort)
  for (int i=0; i<columnCount-1; ++i)
  {
    // Last i elements are already in place    
    for (int j=0; j<columnCount-i-1; ++j)
    {
      if (columnWeights[j] > columnWeights[j+1])
      {
        columnWeights.swap(j, j+1);
        header->swapSections(j, j+1);
      }
    }
  }
  header->blockSignals(wasBlocked);
  header->updateGeometry();
}


//----------------------------------------------------------------------------
// ctkDICOMTableView methods

//----------------------------------------------------------------------------
ctkDICOMTableView::ctkDICOMTableView(QWidget *parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMTableViewPrivate(*this))
{
  Q_D(ctkDICOMTableView);
  d->init();
}

//----------------------------------------------------------------------------
ctkDICOMTableView::ctkDICOMTableView(QString queryTableName, QWidget *parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMTableViewPrivate(*this))
{
  Q_D(ctkDICOMTableView);
  d->init();
  this->setQueryTableName(queryTableName);
}

//------------------------------------------------------------------------------
ctkDICOMTableView::ctkDICOMTableView (ctkDICOMDatabase* dicomDataBase, QString queryTableName, QWidget* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMTableViewPrivate(*this))
{
  this->setDicomDataBase(dicomDataBase);
  Q_D(ctkDICOMTableView);
  d->init();
  this->setQueryTableName(queryTableName);
}

//------------------------------------------------------------------------------
ctkDICOMTableView::~ctkDICOMTableView()
{
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::setDicomDataBase(ctkDICOMDatabase *dicomDatabase)
{
  Q_D(ctkDICOMTableView);

  if (d->dicomDatabase == dicomDatabase)
  {
    // no change
    return;
  }

  if (d->dicomDatabase)
  {
    QObject::disconnect(d->dicomDatabase, SIGNAL(instanceAdded(const QString&)), this, SLOT(onInstanceAdded()));
    QObject::disconnect(d->dicomDatabase, SIGNAL(databaseChanged()), this, SLOT(onDatabaseChanged()));
    QObject::disconnect(d->dicomDatabase, SIGNAL(opened()), this, SLOT(onDatabaseOpened()));
    QObject::disconnect(d->dicomDatabase, SIGNAL(closed()), this, SLOT(onDatabaseClosed()));
    QObject::disconnect(d->dicomDatabase, SIGNAL(schemaUpdated()), this, SLOT(onDatabaseSchemaUpdated()));
  }

  d->dicomDatabase = dicomDatabase;
  if (d->dicomDatabase)
  {
    //Create connections for new database
    QObject::connect(d->dicomDatabase, SIGNAL(instanceAdded(const QString&)), this, SLOT(onInstanceAdded()));
    QObject::connect(d->dicomDatabase, SIGNAL(databaseChanged()), this, SLOT(onDatabaseChanged()));
    QObject::connect(d->dicomDatabase, SIGNAL(opened()), this, SLOT(onDatabaseOpened()));
    QObject::connect(d->dicomDatabase, SIGNAL(closed()), this, SLOT(onDatabaseClosed()));
    QObject::connect(d->dicomDatabase, SIGNAL(schemaUpdated()), this, SLOT(onDatabaseSchemaUpdated()));
  }

  this->setQuery();
  d->applyColumnProperties();

  this->setEnabled(d->dicomDatabase && d->dicomDatabase->isOpen());
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::setQueryTableName(const QString &tableName)
{
  Q_D(ctkDICOMTableView);
  d->queryTableName = tableName;
  d->lblTableName->setText(d->queryTableName);
}

//------------------------------------------------------------------------------
QString ctkDICOMTableView::queryTableName() const
{
  Q_D(const ctkDICOMTableView);
  return d->queryTableName;
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::setQueryForeignKey(const QString &foreignKey)
{
  Q_D(ctkDICOMTableView);
  d->queryForeignKey = foreignKey;
}

//------------------------------------------------------------------------------
QString ctkDICOMTableView::queryForeignKey() const
{
  Q_D(const ctkDICOMTableView);
  return d->queryForeignKey;
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onSelectionChanged()
{
  emit selectionChanged(currentSelection());
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onDatabaseOpened()
{
  Q_D(ctkDICOMTableView);
  this->setQuery();
  if (!d->dicomDatabase || !d->dicomDatabase->isDisplayedFieldsTableAvailable())
  {
    // invalid or outdated database schema
    this->setEnabled(false);
    return;
  }
  d->applyColumnProperties();
  this->setEnabled(true);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onDatabaseClosed()
{
  Q_D(ctkDICOMTableView);
  this->setQuery();
  this->setEnabled(false);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onDatabaseSchemaUpdated()
{
  Q_D(ctkDICOMTableView);
  this->setQuery();
  d->applyColumnProperties();
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onDatabaseChanged()
{
  Q_D(ctkDICOMTableView);
  if (d->batchUpdate)
  {
    d->batchUpdateModificationPending = true;
    return;
  }
  this->setQuery();
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onUpdateQuery(const QStringList& uids)
{
  Q_D(ctkDICOMTableView);

  this->setQuery(uids);

  bool showWarning = d->dicomSQLFilterModel->rowCount() == 0 &&
    d->leSearchBox->text().length() != 0;
  d->showFilterActiveWarning(showWarning);
  emit showFilterActiveWarning(showWarning);

  const QStringList& newUIDS = this->uidsForAllRows();
  emit queryChanged(newUIDS);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::setFilterText(const QString& filterText)
{
  Q_D(ctkDICOMTableView);
  d->leSearchBox->setText(filterText);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onFilterChanged(const QString& filterText)
{
  Q_D(ctkDICOMTableView);

  d->dicomSQLFilterModel->setFilterWildcard(filterText);

  const QStringList uids = this->uidsForAllRows();

  bool showWarning = d->dicomSQLFilterModel->rowCount() == 0 &&
    d->dicomSQLModel.rowCount() != 0;
  d->showFilterActiveWarning(showWarning);
  emit showFilterActiveWarning(showWarning);

  d->tblDicomDatabaseView->clearSelection();
  emit queryChanged(uids);
  emit filterTextChanged(filterText);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onInstanceAdded()
{
  Q_D(ctkDICOMTableView);
  if (d->batchUpdate)
  {
    d->batchUpdateInstanceAddedPending = true;
    return;
  }
  d->sqlWhereConditions.clear();
  d->sqlGreaterWhereConditions.clear();
  d->sqlLessWhereConditions.clear();
  d->sqlGreaterEqualWhereConditions.clear();
  d->sqlLessEqualWhereConditions.clear();
  d->tblDicomDatabaseView->clearSelection();
  d->leSearchBox->clear();
  this->setQuery();
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::selectAll()
{
  Q_D(ctkDICOMTableView);
  d->tblDicomDatabaseView->selectAll();
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::selectFirst()
{
  Q_D(ctkDICOMTableView);
  QModelIndex firstIndex = d->tblDicomDatabaseView->model()->index(0,0);
  QItemSelectionModel* selectionModel = d->tblDicomDatabaseView->selectionModel();
  selectionModel->setCurrentIndex(firstIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::clearSelection()
{
  Q_D(ctkDICOMTableView);
  d->tblDicomDatabaseView->clearSelection();
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::eventFilter(QObject *obj, QEvent *event)
{
  Q_D(ctkDICOMTableView);
  if (obj == d->tblDicomDatabaseView->viewport())
  {
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonDblClick)
    {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      QPoint pos = mouseEvent->pos();
      if (!d->tblDicomDatabaseView->indexAt(pos).isValid())
      {
        return true;
      }
    }
  }
  return QObject::eventFilter(obj, event);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::setQuery(const QStringList &uids)
{
  Q_D(ctkDICOMTableView);

  QString queryString = ("SELECT DISTINCT %1.* FROM Patients, Series, Studies WHERE "
                   "Patients.UID = Studies.PatientsUID AND Studies.StudyInstanceUID = Series.StudyInstanceUID");
  QList<QVariant> boundValues;
  int columnCountBefore = d->dicomSQLModel.columnCount();

  if (!uids.empty() && d->queryForeignKey.length() != 0)
  {
    queryString += " AND %1." + d->queryForeignKey + " IN (";
    for (int i=0; i<uids.count(); ++i)
    {
      queryString += (i == uids.count()-1 ? "?)" : "?,");
      boundValues << uids[i];
    }
  }

  // Add where statements
  QHash<QString, QStringList>::const_iterator filterIt = d->sqlWhereConditions.begin();
  while (filterIt != d->sqlWhereConditions.end())
  {
    if (!filterIt.value().empty())
    {
      queryString += " AND " + filterIt.key() + " IN (";
      for (int i=0; i<filterIt.value().count(); ++i)
      {
        queryString += (i == filterIt.value().count()-1 ? "?)" : "?,");
        boundValues << filterIt.value()[i];
      }
    }
    ++filterIt;
  }
  foreach (const QString& column, d->sqlGreaterWhereConditions.keys())
  {
    queryString += " AND " + column + " > ?" ;
    boundValues << d->sqlGreaterWhereConditions[column];
  }
  foreach (const QString& column, d->sqlLessWhereConditions.keys())
  {
    queryString += " AND " + column + " < ?" ;
    boundValues << d->sqlLessWhereConditions[column];
  }
  foreach (const QString& column, d->sqlGreaterEqualWhereConditions.keys())
  {
    queryString += " AND " + column + " >= ?" ;
    boundValues << d->sqlGreaterEqualWhereConditions[column];
  }
  foreach (const QString& column, d->sqlLessEqualWhereConditions.keys())
  {
    queryString += " AND " + column + " <= ?" ;
    boundValues << d->sqlLessEqualWhereConditions[column];
  }

  if (d->dicomDatabase != 0 && d->dicomDatabase->isOpen()
    && (d->queryForeignKey.isEmpty() || !uids.empty()) )
  {
    QSqlQuery query(d->dicomDatabase->database());
    query.prepare(queryString.arg(d->queryTableName));
    foreach (QVariant value, boundValues)
    {
      query.addBindValue(value);
    }
    if (!query.exec())
    {
      qCritical() << Q_FUNC_INFO << "failed: Failed to execute query " << query.lastQuery() << " : " << query.lastError().text();
    }

    d->dicomSQLModel.setQuery(query);
    if (columnCountBefore == 0)
    {
      // columns have not been initialized yet
      d->applyColumnProperties();
    }
  }
  else
  {
    d->dicomSQLModel.clear();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::addSqlWhereCondition(const std::pair<QString, QStringList> &condition)
{
  Q_D(ctkDICOMTableView);
  d->sqlWhereConditions.insert(condition.first, condition.second);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::addSqlWhereCondition(const QString& column, const QStringList& values)
{
  Q_D(ctkDICOMTableView);
  d->sqlWhereConditions.insert(column, values);
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::removeSqlWhereCondition(const QString& column)
{
  Q_D(ctkDICOMTableView);
  return d->sqlWhereConditions.remove(column) > 0;
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::addSqlEqualWhereCondition(const QString& column, const QStringList& values)
{
  this->addSqlWhereCondition(column, values);
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::removeSqlEqualWhereCondition(const QString& column)
{
  return this->removeSqlWhereCondition(column);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::addSqlGreaterWhereCondition(const QString& column, const QVariant value)
{
  Q_D(ctkDICOMTableView);
  d->sqlGreaterWhereConditions[column] = value;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::removeSqlGreaterWhereCondition(const QString& column)
{
  Q_D(ctkDICOMTableView);
  return d->sqlGreaterWhereConditions.remove(column) > 0;
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::addSqlLessWhereCondition(const QString& column, const QVariant value)
{
  Q_D(ctkDICOMTableView);
  d->sqlLessWhereConditions[column] = value;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::removeSqlLessWhereCondition(const QString& column)
{
  Q_D(ctkDICOMTableView);
  return d->sqlLessWhereConditions.remove(column) > 0;
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::addSqlGreaterEqualWhereCondition(const QString& column, const QVariant value)
{
  Q_D(ctkDICOMTableView);
  d->sqlGreaterEqualWhereConditions[column] = value;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::removeSqlGreaterEqualWhereCondition(const QString& column)
{
  Q_D(ctkDICOMTableView);
  return d->sqlGreaterEqualWhereConditions.remove(column) > 0;
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::addSqlLessEqualWhereCondition(const QString& column, const QVariant value)
{
  Q_D(ctkDICOMTableView);
  d->sqlLessEqualWhereConditions[column] = value;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::removeSqlLessEqualWhereCondition(const QString& column)
{
  Q_D(ctkDICOMTableView);
  return d->sqlLessEqualWhereConditions.remove(column) > 0;
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::clearSqlWhereConditions()
{
  Q_D(ctkDICOMTableView);
  d->sqlWhereConditions.clear();
  d->sqlGreaterWhereConditions.clear();
  d->sqlLessWhereConditions.clear();
  d->sqlGreaterEqualWhereConditions.clear();
  d->sqlLessEqualWhereConditions.clear();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMTableView::uidsForAllRows() const
{
  Q_D(const ctkDICOMTableView);
  QAbstractItemModel* tableModel = d->tblDicomDatabaseView->model();
  int numberOfRows = tableModel->rowCount();
  QStringList uids;
  if (numberOfRows == 0)
  {
    //Return invalid UID if there are no rows
    uids << QString("#");
  }
  else
  {
    for(int i = 0; i < numberOfRows; ++i)
    {
      uids << QString("%1").arg(tableModel->index(i,0).data().toString());
    }
  }
  return uids;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMTableView::currentSelection() const
{
  Q_D(const ctkDICOMTableView);

  QModelIndexList currentSelection = d->tblDicomDatabaseView->selectionModel()->selectedRows(0);
  QStringList uids;

  foreach(QModelIndex i, currentSelection)
  {
    uids << i.data().toString();
  }

  return  uids;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::filterActive()
{
  Q_D(ctkDICOMTableView);
  return (d->leSearchBox->text().length() != 0);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onCustomContextMenuRequested(const QPoint &point)
{
  Q_D(ctkDICOMTableView);

  // translate the local point to a global
  QPoint globalPosition = d->tblDicomDatabaseView->mapToGlobal(point);

  emit customContextMenuRequested(globalPosition);
}

//------------------------------------------------------------------------------
QTableView* ctkDICOMTableView::tableView()
{
  Q_D( ctkDICOMTableView );
  return d->tblDicomDatabaseView;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::isBatchUpdate()const
{
  Q_D(const ctkDICOMTableView);
  return d->batchUpdate;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::setBatchUpdate(bool enable)
{
  Q_D(ctkDICOMTableView);
  if (enable == d->batchUpdate)
  {
    return d->batchUpdate;
  }
  d->batchUpdate = enable;
  if (!d->batchUpdate)
  {
    // Process pending modification events
    if (d->batchUpdateModificationPending)
    {
      this->onDatabaseChanged();
    }
    if (d->batchUpdateInstanceAddedPending)
    {
      this->onInstanceAdded();
    }
  }  
  d->batchUpdateModificationPending = false;
  d->batchUpdateInstanceAddedPending = false;
  return !d->batchUpdate;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableView::isHeaderVisible()const
{
  Q_D(const ctkDICOMTableView);
  return d->headerWidget->isVisible();
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::setHeaderVisible(bool visible)
{
  Q_D(ctkDICOMTableView);
  return d->headerWidget->setVisible(visible);
}

//------------------------------------------------------------------------------
QString ctkDICOMTableView::queryString()
{
  Q_D(ctkDICOMTableView);
  return d->dicomSQLModel.query().executedQuery();
}
