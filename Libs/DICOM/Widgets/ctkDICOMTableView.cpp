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


class ctkDICOMTableViewPrivate : public Ui_ctkDICOMTableView
{
  Q_DECLARE_PUBLIC (ctkDICOMTableView)

protected:
  ctkDICOMTableView* const q_ptr;

public:
  ctkDICOMTableViewPrivate(ctkDICOMTableView& obj);
  ~ctkDICOMTableViewPrivate();
  void init();

  QSharedPointer<ctkDICOMDatabase> ctkDICOMDatabase;
  QSqlQueryModel DICOMSQLModel;
  QSortFilterProxyModel* DICOMSQLFilterModel;
};

ctkDICOMTableViewPrivate::ctkDICOMTableViewPrivate(ctkDICOMTableView &obj)
  : q_ptr(&obj)
{
  this->DICOMSQLFilterModel = new QSortFilterProxyModel();
}

ctkDICOMTableViewPrivate::~ctkDICOMTableViewPrivate()
{
  delete this->DICOMSQLFilterModel;
}

void ctkDICOMTableViewPrivate::init()
{
  Q_Q(ctkDICOMTableView);
  this->setupUi(q);

  //TODO need model information
//  this->tblDicomDatabaseView->setModel(QSqlTableModel);

  this->tblDicomDatabaseView->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->tblDicomDatabaseView->setSelectionMode(QAbstractItemView::SingleSelection);
//  this->tblDicomDatabaseView->setColumnHidden(1, true);

  QObject::connect(this->tblDicomDatabaseView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                   q, SLOT(onSelectionChanged(const QItemSelection &,const QItemSelection &)));
}

//----------------------------------------------------------------------------
// ctkDICOMTableView methods

//----------------------------------------------------------------------------
ctkDICOMTableView::ctkDICOMTableView(QWidget *parent)
  :Superclass(parent)
  , d_ptr(new ctkDICOMTableViewPrivate(*this))
{
  Q_D(ctkDICOMTableView);
  d->init();
}

ctkDICOMTableView::ctkDICOMTableView(QWidget *parent, QSharedPointer<ctkDICOMDatabase> ctkDicomDataBase)
  : Superclass(parent)
  , d_ptr(new ctkDICOMTableViewPrivate(*this))
{
  Q_D(ctkDICOMTableView);
//  d->ctkDICOMDatabase = ctkDicomDataBase;
  this->setCTKDicomDataBase(ctkDicomDataBase);
}

ctkDICOMTableView::~ctkDICOMTableView()
{
}

void ctkDICOMTableView::setCTKDicomDataBase(QSharedPointer<ctkDICOMDatabase> dicomDataBase)
{
  Q_D(ctkDICOMTableView);
  d->ctkDICOMDatabase = dicomDataBase;
  if (d->ctkDICOMDatabase)
  {
    d->DICOMSQLModel.setQuery("select * from Patients", d->ctkDICOMDatabase->database());
    d->DICOMSQLFilterModel->setSourceModel(&d->DICOMSQLModel);
    d->DICOMSQLFilterModel->setFilterKeyColumn(-1);
    d->DICOMSQLFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(d->leSearchBox, SIGNAL(textChanged(QString)), d->DICOMSQLFilterModel, SLOT(setFilterWildcard(QString)));
    d->tblDicomDatabaseView->setModel(d->DICOMSQLFilterModel);

    QObject::connect(d->ctkDICOMDatabase.data(), SIGNAL(databaseChanged()), this, SLOT(onDatabaseChanged()));

//    d->tblDicomDatabaseView->setColumnHidden(0, true);
  }
}

void ctkDICOMTableView::onSelectionChanged(const QItemSelection &, const QItemSelection &)
{
  //Do something
}

void ctkDICOMTableView::onDatabaseChanged()
{
  Q_D(ctkDICOMTableView);
  d->DICOMSQLModel.setQuery("select * from Patients", d->ctkDICOMDatabase->database());

      d->tblDicomDatabaseView->setColumnHidden(0, true);
}

