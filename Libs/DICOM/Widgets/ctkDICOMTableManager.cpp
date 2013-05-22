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

// Qt includes
#include <QHBoxLayout>
#include <QPushButton>

class ctkDICOMTableManagerPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMTableManager)

protected:
  ctkDICOMTableManager* const q_ptr;

public:
  ctkDICOMTableManagerPrivate(ctkDICOMTableManager& obj);
  ~ctkDICOMTableManagerPrivate();

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
  QHBoxLayout layout;
  QPushButton button("Test");
  layout.addWidget(&button);
  q->setLayout(&layout);
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

void ctkDICOMTableManager::onChangeLayout()
{

}
