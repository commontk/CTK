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

// Qt includes
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

// CTK includes
#include "ctkExampleDerivedWorkflowWidgetStep.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflow.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class ctkExampleDerivedWorkflowWidgetStepPrivate
{
public:
  ctkExampleDerivedWorkflowWidgetStepPrivate();
  ~ctkExampleDerivedWorkflowWidgetStepPrivate(){}

  /// elements of this step's user interface
  QLabel* label;
  QLineEdit* lineEdit;
  int defaultLineEditValue;

  // counters of the number of times we have run the onEntry()
  // and onExit() functions
  int numberOfTimesRanOnEntry;
  int numberOfTimesRanOnExit;
};

//-----------------------------------------------------------------------------
// ctkExampleDerivedWorkflowWidgetStepPrivate methods

//-----------------------------------------------------------------------------
ctkExampleDerivedWorkflowWidgetStepPrivate::ctkExampleDerivedWorkflowWidgetStepPrivate()
{
  this->label = 0;
  this->lineEdit = 0;

  this->defaultLineEditValue = 10;

  this->numberOfTimesRanOnEntry = 0;
  this->numberOfTimesRanOnExit = 0;
}

//-----------------------------------------------------------------------------
// ctkExampleDerivedWorkflowWidgetStep methods

//-----------------------------------------------------------------------------
ctkExampleDerivedWorkflowWidgetStep::ctkExampleDerivedWorkflowWidgetStep(const QString& newId) :
  Superclass(newId)
  , d_ptr(new ctkExampleDerivedWorkflowWidgetStepPrivate)
{
}

//-----------------------------------------------------------------------------
ctkExampleDerivedWorkflowWidgetStep::~ctkExampleDerivedWorkflowWidgetStep()
{
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkExampleDerivedWorkflowWidgetStep, QLabel*, label, label);
CTK_SET_CPP(ctkExampleDerivedWorkflowWidgetStep, QLabel*, setLabel, label);
CTK_GET_CPP(ctkExampleDerivedWorkflowWidgetStep, QLineEdit*, lineEdit, lineEdit);
CTK_SET_CPP(ctkExampleDerivedWorkflowWidgetStep, QLineEdit*, setLineEdit, lineEdit);
CTK_GET_CPP(ctkExampleDerivedWorkflowWidgetStep, int, numberOfTimesRanOnEntry, numberOfTimesRanOnEntry);
CTK_GET_CPP(ctkExampleDerivedWorkflowWidgetStep, int, numberOfTimesRanOnExit, numberOfTimesRanOnExit);

//-----------------------------------------------------------------------------
void ctkExampleDerivedWorkflowWidgetStep::onEntry(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(comingFrom);
  Q_UNUSED(transitionType);

  // simply implements our counter of the number of times we have run
  // this function
  Q_D(ctkExampleDerivedWorkflowWidgetStep);
  d->numberOfTimesRanOnEntry++;

  // signals that we are finished
  this->onEntryComplete();
}

//-----------------------------------------------------------------------------
void ctkExampleDerivedWorkflowWidgetStep::onExit(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(goingTo);
  Q_UNUSED(transitionType);

  // simply implements our counter of the number of times we have run
  // this function
  Q_D(ctkExampleDerivedWorkflowWidgetStep);
  d->numberOfTimesRanOnExit++;

  // signals that we are finished
  this->onExitComplete();
}

//-----------------------------------------------------------------------------
void ctkExampleDerivedWorkflowWidgetStep::createUserInterface()
{
  Q_D(ctkExampleDerivedWorkflowWidgetStep);

  // create widgets the first time through
  if (!this->layout())
    {
    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);
    }

  if (!d->label)
    {
    d->label = new QLabel();
    d->label->setText(this->name() + ": enter a number greater than or equal to 10");
    this->layout()->addWidget(d->label);
    }

  if (!d->lineEdit)
    {
    d->lineEdit = new QLineEdit();
    d->lineEdit->setInputMask("000");
    d->lineEdit->setText(QString::number(d->defaultLineEditValue));
    this->layout()->addWidget(d->lineEdit);
    }

  // signals that we are finished
  this->createUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
void ctkExampleDerivedWorkflowWidgetStep::validate(const QString& desiredBranchId)
{
  Q_D(const ctkExampleDerivedWorkflowWidgetStep);
  bool retVal = 0;

  int val = 0;
  bool ok = false;
  if (d->lineEdit)
    {
    QString text = d->lineEdit->text();
    val = text.toInt(&ok);
    }
  // used when going to a finish step
  else
    {
    val = d->defaultLineEditValue;
    ok = true;
    }

  if (!ok)
    {
    this->setStatusText("invalid (not an integer or empty)");
    retVal = false;
    }
  else if (val < 10)
    {
    this->setStatusText("invalid (invalid number)");
    retVal = false;
    }
  else
    {
    this->setStatusText("");
    retVal = true;
    }

  // return the validation results
  this->validationComplete(retVal, desiredBranchId);
}
