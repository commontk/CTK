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

// ctk includes
#include "ctkDICOMTableManager.h"
#include "ctkDICOMTableView.h"

// Qt includes
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

class ctkDICOMTableManagerPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMTableManager)

protected:
  ctkDICOMTableManager* const q_ptr;

public:
  ctkDICOMTableManagerPrivate(ctkDICOMTableManager& obj);
  ~ctkDICOMTableManagerPrivate();

  QVBoxLayout* layout;
  QBoxLayout* layoutTables;
  QSplitter* tableSplitter;

  ctkDICOMTableView* patientsTable;
  ctkDICOMTableView* studiesTable;
  ctkDICOMTableView* seriesTable;

  void init();
  void setCTKDICOMDatabase(QSharedPointer<ctkDICOMDatabase> db);
};

ctkDICOMTableManagerPrivate::ctkDICOMTableManagerPrivate(ctkDICOMTableManager &obj)
  : q_ptr(&obj)
{

}

ctkDICOMTableManagerPrivate::~ctkDICOMTableManagerPrivate()
{

}

void ctkDICOMTableManagerPrivate::init()
{
  //setup UI
  Q_Q(ctkDICOMTableManager);

  this->layout = new QVBoxLayout();
  this->layoutTables = new QBoxLayout(QBoxLayout::LeftToRight);
  this->patientsTable = new ctkDICOMTableView(q, "Patients");
  this->studiesTable = new ctkDICOMTableView(q, "Studies");
  this->studiesTable->setQueryForeignKey("PatientsUID");
  this->seriesTable = new ctkDICOMTableView(q, "Series");
  this->seriesTable->setQueryForeignKey("StudyInstanceUID");

  QObject::connect(this->patientsTable, SIGNAL(signalQueryChanged(QStringList)),
                   this->studiesTable, SLOT(onUpdateQuery(const QStringList&)));//any selection in the study table
  QObject::connect(this->studiesTable, SIGNAL(signalQueryChanged(QStringList)),
                   this->seriesTable, SLOT(onUpdateQuery(const QStringList&)));

  // For propagating patient selection changes
  QObject::connect(this->patientsTable, SIGNAL(signalSelectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(signalPatientsSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->patientsTable, SIGNAL(signalSelectionChanged(const QStringList&)),
                   q, SIGNAL(signalPatientsSelectionChanged(const QStringList&)));

  // For propagating study selection changes
  QObject::connect(this->studiesTable, SIGNAL(signalSelectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(signalStudiesSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->studiesTable, SIGNAL(signalSelectionChanged(const QStringList&)),
                   q, SIGNAL(signalStudiesSelectionChanged(const QStringList&)));

  // For propagating series selection changes
  QObject::connect(this->seriesTable, SIGNAL(signalSelectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(signalSeriesSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->seriesTable, SIGNAL(signalSelectionChanged(const QStringList&)),
                   q, SIGNAL(signalSeriesSelectionChanged(const QStringList&)));

  this->patientsTable->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
  this->studiesTable->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
  this->seriesTable->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

  tableSplitter = new QSplitter();

  tableSplitter->addWidget(this->patientsTable);
  tableSplitter->addWidget(this->studiesTable);
  tableSplitter->addWidget(this->seriesTable);

  tableSplitter->setStyleSheet("QSplitter::handle {background-color: rgb(200,200,200);}"
                               "QSplitter::handle:horizontal {width: 2px;}"
                               "QSplitter::handle:vertical {height: 2px;}");

  this->layout->addWidget(this->tableSplitter);

  q->setLayout(layout);
}

void ctkDICOMTableManagerPrivate::setCTKDICOMDatabase(QSharedPointer<ctkDICOMDatabase> db)
{
  this->patientsTable->setCTKDicomDataBase(db);
  this->studiesTable->setCTKDicomDataBase(db);
  this->seriesTable->setCTKDicomDataBase(db);
}

//----------------------------------------------------------------------------
// ctkDICOMTableManager methods

//----------------------------------------------------------------------------

ctkDICOMTableManager::ctkDICOMTableManager(QWidget *parent)
  :Superclass(parent)
  , d_ptr(new ctkDICOMTableManagerPrivate(*this))
{
  Q_D(ctkDICOMTableManager);
  d->init();
}

ctkDICOMTableManager::ctkDICOMTableManager(QSharedPointer<ctkDICOMDatabase> db, QWidget *parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMTableManagerPrivate(*this))
{
  Q_D(ctkDICOMTableManager);
  d->init();
  d->setCTKDICOMDatabase(db);
}

ctkDICOMTableManager::~ctkDICOMTableManager()
{

}

void ctkDICOMTableManager::setCTKDICOMDatabase(QSharedPointer<ctkDICOMDatabase> db)
{
  Q_D(ctkDICOMTableManager);
  d->setCTKDICOMDatabase(db);
}

void ctkDICOMTableManager::setOrientation(const Qt::Orientation &o)
{
  Q_D(ctkDICOMTableManager);
  d->tableSplitter->setOrientation(o);
}
