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
#include "ui_ctkDICOMTableManager.h"

// Qt includes
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

class ctkDICOMTableManagerPrivate : public Ui_ctkDICOMTableManager
{
  Q_DECLARE_PUBLIC(ctkDICOMTableManager)

protected:
  ctkDICOMTableManager* const q_ptr;

public:
  ctkDICOMTableManagerPrivate(ctkDICOMTableManager& obj);
  ~ctkDICOMTableManagerPrivate();

  void init();
  void setCTKDICOMDatabase(ctkDICOMDatabase *db);
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
  this->setupUi(q);

  this->patientsTable->setQueryTableName("Patients");
  this->studiesTable->setQueryTableName("Studies");
  this->studiesTable->setQueryForeignKey("PatientsUID");
  this->seriesTable->setQueryTableName("Series");
  this->seriesTable->setQueryForeignKey("StudyInstanceUID");

  // For propagating patient selection changes
  QObject::connect(this->patientsTable, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(patientsSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->patientsTable, SIGNAL(selectionChanged(const QStringList&)),
                   q, SIGNAL(patientsSelectionChanged(const QStringList&)));

  // For propagating study selection changes
  QObject::connect(this->studiesTable, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(studiesSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->studiesTable, SIGNAL(selectionChanged(const QStringList&)),
                   q, SIGNAL(studiesSelectionChanged(const QStringList&)));

  // For propagating series selection changes
  QObject::connect(this->seriesTable, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(seriesSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->seriesTable, SIGNAL(selectionChanged(const QStringList&)),
                   q, SIGNAL(seriesSelectionChanged(const QStringList&)));
}

void ctkDICOMTableManagerPrivate::setCTKDICOMDatabase(ctkDICOMDatabase* db)
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

ctkDICOMTableManager::ctkDICOMTableManager(ctkDICOMDatabase *db, QWidget *parent)
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

void ctkDICOMTableManager::setCTKDICOMDatabase(ctkDICOMDatabase* db)
{
  Q_D(ctkDICOMTableManager);
  d->setCTKDICOMDatabase(db);
}

void ctkDICOMTableManager::setTableOrientation(const Qt::Orientation &o)
{
  Q_D(ctkDICOMTableManager);
  d->tableSplitter->setOrientation(o);
}

Qt::Orientation ctkDICOMTableManager::tableOrientation()
{
  Q_D(ctkDICOMTableManager);
  return d->tableSplitter->orientation();
}
