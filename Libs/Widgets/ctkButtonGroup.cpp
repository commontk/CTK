/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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


