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
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

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

  QStringList uidsForAllRows();

  ctkDICOMDatabase* dicomDatabase;
  QSqlQueryModel dicomSQLModel;
  QSortFilterProxyModel* dicomSQLFilterModel;
  QString queryTableName;
  QString queryForeignKey;

};
//------------------------------------------------------------------------------

ctkDICOMTableViewPrivate::ctkDICOMTableViewPrivate(ctkDICOMTableView &obj)
  : q_ptr(&obj)
{
  this->dicomSQLFilterModel = new QSortFilterProxyModel();
}

ctkDICOMTableViewPrivate::ctkDICOMTableViewPrivate(ctkDICOMTableView &obj, ctkDICOMDatabase* db)
  : q_ptr(&obj)
  , dicomDatabase(db)
{
  this->dicomSQLFilterModel = new QSortFilterProxyModel();
}

ctkDICOMTableViewPrivate::~ctkDICOMTableViewPrivate()
{
  delete this->dicomSQLFilterModel;
}

void ctkDICOMTableViewPrivate::init()
{
  Q_Q(ctkDICOMTableView);
  this->setupUi(q);

  this->tblDicomDatabaseView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

  this->leSearchBox->setAlwaysShowClearIcon(true);
  this->leSearchBox->setShowSearchIcon(true);

  this->lblTableName->setText(this->queryTableName);

  if (this->dicomDatabase)
    {
      this->setUpTableView();
    }
}

void ctkDICOMTableViewPrivate::setUpTableView()
{
  Q_Q(ctkDICOMTableView);
  if (this->dicomDatabase)
    {
      q->setQuery();
      this->dicomSQLFilterModel->setSourceModel(&this->dicomSQLModel);
      this->dicomSQLFilterModel->setFilterKeyColumn(-1);
      this->dicomSQLFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
      this->tblDicomDatabaseView->setModel(this->dicomSQLFilterModel);
      this->tblDicomDatabaseView->setColumnHidden(0, true);
      this->tblDicomDatabaseView->setSortingEnabled(true);
      this->hideUIDColumns();

      QObject::connect(this->tblDicomDatabaseView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                       q, SLOT(onSelectionChanged()));
      QObject::connect(this->tblDicomDatabaseView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                       q, SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)));
      QObject::connect(this->leSearchBox, SIGNAL(textChanged(QString)), this->dicomSQLFilterModel, SLOT(setFilterWildcard(QString)));
      QObject::connect(this->leSearchBox, SIGNAL(textChanged(QString)), q, SLOT(onFilterChanged()));
      QObject::connect(this->dicomDatabase, SIGNAL(schemaUpdated()), q, SLOT(onDatabaseChanged()));
      QObject::connect(this->dicomDatabase, SIGNAL(databaseChanged()), q, SLOT(onDatabaseChanged()));
    }
}

//Temporay solution to hide UID columns
void ctkDICOMTableViewPrivate::hideUIDColumns()
{
  int numberOfColumns = this->tblDicomDatabaseView->horizontalHeader()->count();
  QString columnName = " ";
  for (int i = 0; i < numberOfColumns; ++i)
    {
      columnName = this->tblDicomDatabaseView->model()->headerData(i, Qt::Horizontal).toString();
      if (columnName.contains("UID"))
        {
          this->tblDicomDatabaseView->hideColumn(i);
        }
    }
}

QStringList ctkDICOMTableViewPrivate::uidsForAllRows()
{
  QAbstractItemModel* tableModel = this->tblDicomDatabaseView->model();
  int numberOfRows = tableModel->rowCount();
  QStringList uids;
  if (numberOfRows == 0)
    {
      //Return invalid UID if there are no rows
      uids << QString("'#'");
    }
  else
    {
      for(int i = 0; i < numberOfRows; ++i)
        {
          uids << (QString("'") + tableModel->index(i,0).data().toString() +"'");
        }
    }
  return uids;
}


//----------------------------------------------------------------------------
// ctkDICOMTableView methods

//----------------------------------------------------------------------------
ctkDICOMTableView::ctkDICOMTableView(QWidget *parent, QString queryTableName)
  : Superclass(parent)
  , d_ptr(new ctkDICOMTableViewPrivate(*this))
{
  Q_D(ctkDICOMTableView);
  d->queryTableName = queryTableName;
  d->dicomDatabase = 0;
  d->init();
  d->lblTableName->setText(queryTableName);
}

ctkDICOMTableView::ctkDICOMTableView(ctkDICOMDatabase* ctkDicomDataBase, QWidget *parent, QString queryTableName)
  : Superclass(parent)
  , d_ptr(new ctkDICOMTableViewPrivate(*this))
{
  this->setCTKDicomDataBase(ctkDicomDataBase);
  Q_D(ctkDICOMTableView);
  d->queryTableName = queryTableName;
  d->init();
}

ctkDICOMTableView::~ctkDICOMTableView()
{
}

void ctkDICOMTableView::setCTKDicomDataBase(ctkDICOMDatabase *dicomDatabase)
{
  Q_D(ctkDICOMTableView);
  d->dicomDatabase = dicomDatabase;
  d->setUpTableView();
}

void ctkDICOMTableView::setQueryTableName(const QString &tableName)
{
  Q_D(ctkDICOMTableView);
  d->queryTableName = tableName;
  d->lblTableName->setText(tableName);
}

void ctkDICOMTableView::setQueryForeignKey(const QString &foreignKey)
{
  Q_D(ctkDICOMTableView);
  d->queryForeignKey = foreignKey;
}

void ctkDICOMTableView::onSelectionChanged()
{
  Q_D(ctkDICOMTableView);

  QModelIndexList currentSelection = d->tblDicomDatabaseView->selectionModel()->selectedRows(0);
  QStringList uids;

  foreach(QModelIndex i, currentSelection)
    {
      uids << (QString("'") + i.data().toString() +"'");
    }
  emit queryChanged(uids);
}

void ctkDICOMTableView::onDatabaseChanged()
{
  Q_D(ctkDICOMTableView);
  setQuery();
}

void ctkDICOMTableView::onUpdateQuery(const QStringList& uids)
{
  Q_D(ctkDICOMTableView);

  setQuery(uids);

  QStringList newUIDS = d->uidsForAllRows();
  emit queryChanged(newUIDS);
}

void ctkDICOMTableView::onFilterChanged()
{
  Q_D(ctkDICOMTableView);

  QStringList uids = d->uidsForAllRows();
  emit queryChanged(uids);
}

void ctkDICOMTableView::setQuery(const QStringList &uids)
{
  Q_D(ctkDICOMTableView);
  QString query;

  if (uids.empty() || d->queryForeignKey.length() == 0)
    {
      query = "select * from " + d->queryTableName;
    }
  else
    {
      query = "select * from "+d->queryTableName+" where "+d->queryForeignKey+" in ( ";
      query.append(uids.join(",")).append(");");
    }
  if (d->dicomDatabase != 0)
    d->dicomSQLModel.setQuery(query, d->dicomDatabase->database());
}
