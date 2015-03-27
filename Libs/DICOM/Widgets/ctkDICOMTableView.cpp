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
#include <QMouseEvent>
#include <QSortFilterProxyModel>
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
  // Initialize UI
  void init();
  // Setup tableview with tablemodel if database is available
  void setUpTableView();
  //Temporay solution to hide UID columns
  void hideUIDColumns();

  void showFilterActiveWarning(bool);

  QString queryTableName() const;

  ctkDICOMDatabase* dicomDatabase;
  QSqlQueryModel dicomSQLModel;
  QSortFilterProxyModel* dicomSQLFilterModel;
  QString queryForeignKey;

  QStringList currentSelection;
  //Key = QString for columns, Values = QStringList
  QHash<QString, QStringList> sqlWhereConditions;

};

//------------------------------------------------------------------------------
ctkDICOMTableViewPrivate::ctkDICOMTableViewPrivate(ctkDICOMTableView &obj)
  : q_ptr(&obj)
{
  this->dicomSQLFilterModel = new QSortFilterProxyModel(&obj);
  this->dicomDatabase = new ctkDICOMDatabase(&obj);
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

  if (this->dicomDatabase != 0)
    {
      q->setDicomDataBase(this->dicomDatabase);
    }

  this->tblDicomDatabaseView->viewport()->installEventFilter(q);
}

//------------------------------------------------------------------------------
void ctkDICOMTableViewPrivate::setUpTableView()
{
  Q_Q(ctkDICOMTableView);
  if (this->dicomDatabase != 0)
    {
      q->setQuery();
      this->dicomSQLFilterModel->setSourceModel(&this->dicomSQLModel);
      this->dicomSQLFilterModel->setFilterKeyColumn(-1);
      this->dicomSQLFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
      this->tblDicomDatabaseView->setModel(this->dicomSQLFilterModel);
      this->tblDicomDatabaseView->setColumnHidden(0, true);
      this->tblDicomDatabaseView->setSortingEnabled(true);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
      this->tblDicomDatabaseView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#else
      this->tblDicomDatabaseView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#endif
      this->hideUIDColumns();

      QObject::connect(this->tblDicomDatabaseView->selectionModel(),
                       SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                       q, SLOT(onSelectionChanged()));

      QObject::connect(this->tblDicomDatabaseView->selectionModel(),
                       SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                       q, SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)));

      QObject::connect(this->tblDicomDatabaseView, SIGNAL(doubleClicked(const QModelIndex&)),
                       q, SIGNAL(doubleClicked(const QModelIndex&)));

      QObject::connect(this->leSearchBox, SIGNAL(textChanged(QString)),
                       this->dicomSQLFilterModel, SLOT(setFilterWildcard(QString)));

      QObject::connect(this->leSearchBox, SIGNAL(textChanged(QString)), q, SLOT(onFilterChanged()));

      QObject::connect(this->dicomDatabase, SIGNAL(databaseChanged()), q, SLOT(onDatabaseChanged()));
      QObject::connect(this->dicomDatabase, SIGNAL(instanceAdded(QString)),
                       q, SLOT(onInstanceAdded()));
    }
}

//------------------------------------------------------------------------------
//Temporay solution to hide UID columns
void ctkDICOMTableViewPrivate::hideUIDColumns()
{
  const int numberOfColumns = this->tblDicomDatabaseView->model()->columnCount();
  for (int i = 0; i < numberOfColumns; ++i)
    {
    QString columnName = this->tblDicomDatabaseView->model()->headerData(i, Qt::Horizontal).toString();
    if (columnName.contains("UID"))
      {
        this->tblDicomDatabaseView->hideColumn(i);
      }
    }
}

//----------------------------------------------------------------------------
QString ctkDICOMTableViewPrivate::queryTableName() const
{
  return this->lblTableName->text();
}

//----------------------------------------------------------------------------
void ctkDICOMTableViewPrivate::showFilterActiveWarning(bool showWarning)
{
  QPalette palette;
  if (showWarning)
    {
      palette.setColor(QPalette::Base,Qt::yellow);
    }
  else
    {
      palette.setColor(QPalette::Base,Qt::white);
    }
  this->leSearchBox->setPalette(palette);
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

  //Do nothing if no database is set
  if (!dicomDatabase)
    return;

  d->dicomDatabase = dicomDatabase;
  d->setUpTableView();
  //Create connections for new database
  QObject::connect(d->dicomDatabase, SIGNAL(instanceAdded(const QString&)),
                   this, SLOT(onInstanceAdded()));
  QObject::connect(d->dicomDatabase, SIGNAL(databaseChanged()), this, SLOT(onDatabaseChanged()));
  this->setQuery();
  d->hideUIDColumns();
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::setQueryTableName(const QString &tableName)
{
  Q_D(ctkDICOMTableView);
  d->lblTableName->setText(tableName);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::setQueryForeignKey(const QString &foreignKey)
{
  Q_D(ctkDICOMTableView);
  d->queryForeignKey = foreignKey;
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onSelectionChanged()
{
  emit selectionChanged(currentSelection());
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onDatabaseChanged()
{
  setQuery();
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onUpdateQuery(const QStringList& uids)
{
  Q_D(ctkDICOMTableView);

  setQuery(uids);

  d->showFilterActiveWarning( d->dicomSQLFilterModel->rowCount() == 0 &&
                              d->leSearchBox->text().length() != 0 );

  const QStringList& newUIDS = this->uidsForAllRows();
  emit queryChanged(newUIDS);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onFilterChanged()
{
  Q_D(ctkDICOMTableView);

  const QStringList uids = this->uidsForAllRows();

  d->showFilterActiveWarning( d->dicomSQLFilterModel->rowCount() == 0 &&
                              d->dicomSQLModel.rowCount() != 0);

  d->tblDicomDatabaseView->clearSelection();
  emit queryChanged(uids);
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::onInstanceAdded()
{
  Q_D(ctkDICOMTableView);
  d->sqlWhereConditions.clear();
  d->tblDicomDatabaseView->clearSelection();
  d->leSearchBox->clear();
  setQuery();
}

//------------------------------------------------------------------------------
void ctkDICOMTableView::selectAll()
{
  Q_D(ctkDICOMTableView);
  d->tblDicomDatabaseView->selectAll();
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
  QString query = ("select distinct %1.* from Patients, Series, Studies where "
                   "Patients.UID = Studies.PatientsUID and Studies.StudyInstanceUID = Series.StudyInstanceUID");

  if (!uids.empty() && d->queryForeignKey.length() != 0)
    {
      query += " and %1."+d->queryForeignKey+" in ( '";
      query.append(uids.join("','")).append("')");
    }
  if (!d->sqlWhereConditions.empty())
    {
      QHash<QString, QStringList>::const_iterator i = d->sqlWhereConditions.begin();
      while (i != d->sqlWhereConditions.end())
        {
          if (!i.value().empty())
            {
              query += " and "+i.key()+" in ( '";
              query.append(i.value().join("','")).append("')");
            }
          ++i;
        }
    }
  if (d->dicomDatabase != 0)
    d->dicomSQLModel.setQuery(query.arg(d->queryTableName()), d->dicomDatabase->database());
}

void ctkDICOMTableView::addSqlWhereCondition(const std::pair<QString, QStringList> &condition)
{
  Q_D(ctkDICOMTableView);
  d->sqlWhereConditions.insert(condition.first, condition.second);
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
      uids<< i.data().toString();
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
void ctkDICOMTableView::setTableSectionSize(int size)
{
  Q_D(ctkDICOMTableView);
  d->tblDicomDatabaseView->verticalHeader()->setDefaultSectionSize(size);
}

//------------------------------------------------------------------------------
int ctkDICOMTableView::tableSectionSize()
{
  Q_D(ctkDICOMTableView);
  return d->tblDicomDatabaseView->verticalHeader()->defaultSectionSize();
}
