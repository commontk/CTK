/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

//Qt includes  
#include <QAbstractButton>
#include <QDebug>
#include <QWeakPointer>

// CTK includes
#include "ctkButtonGroup.h"

//-----------------------------------------------------------------------------
class ctkButtonGroupPrivate : public ctkPrivate<ctkButtonGroup>
{
public:
  CTK_DECLARE_PUBLIC(ctkButtonGroup);
  bool IsLastButtonPressedChecked;
};

//------------------------------------------------------------------------------
ctkButtonGroup::ctkButtonGroup(QObject* _parent)
  :QButtonGroup(_parent)
{
  CTK_INIT_PRIVATE(ctkButtonGroup);
  connect(this, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
  connect(this, SIGNAL(buttonPressed(QAbstractButton*)), this, SLOT(onButtonPressed(QAbstractButton*)));
}

//------------------------------------------------------------------------------
void ctkButtonGroup::onButtonClicked(QAbstractButton *clickedButton)
{
  CTK_D(ctkButtonGroup);
  if (!this->exclusive() || !d->IsLastButtonPressedChecked)
    {
    return;
    }
  this->removeButton(clickedButton);
  clickedButton->setChecked(false);
  this->addButton(clickedButton);
}

//------------------------------------------------------------------------------
void ctkButtonGroup::onButtonPressed(QAbstractButton *pressedButton)
{
  CTK_D(ctkButtonGroup);
  Q_ASSERT(pressedButton);
  d->IsLastButtonPressedChecked = pressedButton->isChecked();
}


