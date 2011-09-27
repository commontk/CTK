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

// QT includes
#include <QDebug>
#include <QEvent>
#include <QIcon>

// CTK includes
#include "ctkButton.h"

class ctkButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkButton);
protected:
  ctkButton* const q_ptr;
public:
  ctkButtonPrivate(ctkButton& object);
  void init();

};

ctkButtonPrivate::ctkButtonPrivate(ctkButton &object)
  : q_ptr(&object)
{
}

void ctkButtonPrivate::init()
{
  Q_Q(ctkButton);
  q->setIcon(QIcon(":/Icons/left.png"));
  q->setFlat(true);
  q->setCheckable(true);
  q->setFixedWidth(10);
  q->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
}

//-----------------------------------------------------------------------------
// ctkButton methods

//-----------------------------------------------------------------------------
ctkButton::ctkButton(QWidget *_parent)
  :Superclass(_parent)
  , d_ptr(new ctkButtonPrivate(*this))
{
  Q_D(ctkButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkButton::~ctkButton()
{
}

bool ctkButton::event(QEvent *e)
{
  if (e->type() == QEvent::Leave)
    {
    this->setFlat(true);
    }
  if (e->type() == QEvent::Enter)
    {
    this->setFlat(false);
    }

  return this->Superclass::event(e);
}
