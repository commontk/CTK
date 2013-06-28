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
#include <QApplication>

// CTK includes
#include "ctkDoubleSpinBox.h"

// ----------------------------------------------------------------------------
class ctkDoubleSpinBoxPrivate
{
public:
  ctkDoubleSpinBoxPrivate();

  bool InvertedControls;
};

// ----------------------------------------------------------------------------
//  Methods ctkDoubleSpinBoxPrivate

// ----------------------------------------------------------------------------
ctkDoubleSpinBoxPrivate::ctkDoubleSpinBoxPrivate()
{
  this->InvertedControls = false;
}

// ----------------------------------------------------------------------------
//  Methods ctkDoubleSpinBox

// ----------------------------------------------------------------------------
ctkDoubleSpinBox::ctkDoubleSpinBox(QWidget *_parent)
  :Superclass(_parent)
  , d_ptr(new ctkDoubleSpinBoxPrivate())
{
}

// ----------------------------------------------------------------------------
ctkDoubleSpinBox::~ctkDoubleSpinBox()
{
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBox::setInvertedControls(bool invertedControls)
{
  Q_D(ctkDoubleSpinBox);
  d->InvertedControls = invertedControls;
}

// ----------------------------------------------------------------------------
bool ctkDoubleSpinBox::invertedControls() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->InvertedControls;
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBox::stepBy(int steps)
{
  Q_D(const ctkDoubleSpinBox);
  if (d->InvertedControls)
    {
    steps = -steps;
    }
  Superclass::stepBy(steps);
}

// ----------------------------------------------------------------------------
QAbstractSpinBox::StepEnabled ctkDoubleSpinBox::stepEnabled() const
{
  Q_D(const ctkDoubleSpinBox);
  
  if (!d->InvertedControls)
    {
    return Superclass::stepEnabled();
    }

  if (this->isReadOnly())
    {
    return StepNone;
    }

  if (this->wrapping())
    {
    return StepEnabled(StepUpEnabled | StepDownEnabled);
    }
    
  StepEnabled ret = StepNone;
  double value = this->value();
  if (value < this->maximum())
    {
    ret |= StepDownEnabled;
    }
  if (value > this->minimum())
    {
    ret |= StepUpEnabled;
    }
  return ret;
}
