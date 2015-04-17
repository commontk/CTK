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

//Qt includes
#include <QAbstractButton>
#include <QDebug>

// CTK includes
#include "ctkButtonGroup.h"

//-----------------------------------------------------------------------------
class ctkButtonGroupPrivate
{
public:
  bool IsLastButtonPressedChecked;
};

//------------------------------------------------------------------------------
ctkButtonGroup::ctkButtonGroup(QObject* _parent)
  :QButtonGroup(_parent)
  , d_ptr(new ctkButtonGroupPrivate)
{
  Q_D(ctkButtonGroup);
  d->IsLastButtonPressedChecked = false;
  // we need to connect to button{Clicked,Pressed}(int) instead of
  // button{Clicked,Pressed}(QAbstractButton*) in order to be first to catch the
  // signals
  connect(this, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClicked(int)));
  connect(this, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
}

//------------------------------------------------------------------------------
void ctkButtonGroup::setChecked(QAbstractButton* button, bool checked)
{
  if (!button)
    {
    return;
    }
  bool wasExclusive = this->exclusive();
  if (!checked)
    {
    this->setExclusive(false);
    }
  button->setChecked(checked);
  if (!checked)
    {
    this->setExclusive(wasExclusive);
    }
}

//------------------------------------------------------------------------------
ctkButtonGroup::~ctkButtonGroup()
{
}

//------------------------------------------------------------------------------
void ctkButtonGroup::onButtonClicked(int buttonId)
{
  Q_D(ctkButtonGroup);
  QAbstractButton* clickedButton = this->button(buttonId);
  Q_ASSERT(clickedButton);
  if (!this->exclusive() || !d->IsLastButtonPressedChecked)
    {
    return;
    }
  // here the button is clicked and we click it again... so we want to
  // uncheck, a behavior not supported by QButtonGroup.
  // The only way to uncheck the button is to remove it from the group, and put it back
  const int oldId = this->id(clickedButton);
  this->removeButton(clickedButton);
  clickedButton->setChecked(false);
  this->addButton(clickedButton, oldId);
  d->IsLastButtonPressedChecked = false;
#if QT_VERSION >= 0x050200
  emit buttonToggled(oldId, false);
  emit buttonToggled(clickedButton, false);
#endif
}

//------------------------------------------------------------------------------
void ctkButtonGroup::onButtonPressed(int buttonId)
{
  Q_D(ctkButtonGroup);
  QAbstractButton* pressedButton = this->button(buttonId);
  Q_ASSERT(pressedButton);
  d->IsLastButtonPressedChecked = pressedButton->isChecked();
}
