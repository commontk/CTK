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
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>

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

  ctkDICOMTableView* patientsTable;
  ctkDICOMTableView* studiesTable;
  ctkDICOMTableView* seriesTable;

  void init();
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
  this->patientsTable = new ctkDICOMTableView();
  this->studiesTable = new ctkDICOMTableView();
  this->seriesTable = new ctkDICOMTableView();
  this->layoutTables->addWidget(patientsTable);
  this->layoutTables->addWidget(studiesTable);
  this->layoutTables->addWidget(seriesTable);

  QPushButton* changeLayoutButton = new QPushButton("Change Layout");
  QObject::connect(changeLayoutButton, SIGNAL(clicked()), q, SLOT(onChangeLayoutPushed()));

  this->layout->addWidget(changeLayoutButton);
  this->layout->addLayout(this->layoutTables);

  q->setLayout(layout);
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

ctkDICOMTableManager::~ctkDICOMTableManager()
{

}

void ctkDICOMTableManager::onChangeLayoutPushed()
{
  Q_D(ctkDICOMTableManager);
  if (d->layoutTables->direction() == QBoxLayout::TopToBottom)
  {
    this->changeTableLayout(QBoxLayout::LeftToRight);
  }
  else
  {
    this->changeTableLayout(QBoxLayout::TopToBottom);
  }
}

void ctkDICOMTableManager::changeTableLayout(QBoxLayout::Direction direction)
{
  Q_D(ctkDICOMTableManager);
  d->layoutTables->removeWidget(d->patientsTable);
  d->layoutTables->removeWidget(d->studiesTable);
  d->layoutTables->removeWidget(d->seriesTable);
  delete d->layoutTables;

  d->layoutTables = new QBoxLayout(direction);
  d->layoutTables->addWidget(d->patientsTable);
  d->layoutTables->addWidget(d->studiesTable);
  d->layoutTables->addWidget(d->seriesTable);
  d->layout->addLayout(d->layoutTables);
}
