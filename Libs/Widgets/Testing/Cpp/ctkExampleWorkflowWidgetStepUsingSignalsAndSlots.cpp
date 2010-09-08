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

// Qt includes
#include <QWidget>
#include <QLabel>
#include <QLineEdit>

// CTK includes
#include "ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h"
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowWidget.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class ctkExampleWorkflowWidgetStepUsingSignalsAndSlotsPrivate : public ctkPrivate<ctkExampleWorkflowWidgetStepUsingSignalsAndSlots>
{
public:
  CTK_DECLARE_PUBLIC(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots);
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlotsPrivate();
  ~ctkExampleWorkflowWidgetStepUsingSignalsAndSlotsPrivate(){}

  /// elements of this step's user interface
  QWidget* widget;
  QLabel* label;
  QLineEdit* lineEdit;
  int defaultLineEditValue;

  // counters of the number of times we have run the onEntry()
  // and onExit() functions
  int numberOfTimesRanOnEntry;
  int numberOfTimesRanOnExit;

};

//-----------------------------------------------------------------------------
// ctkExampleWorkflowWidgetStepUsingSignalsAndSlotsPrivate methods

//-----------------------------------------------------------------------------
ctkExampleWorkflowWidgetStepUsingSignalsAndSlotsPrivate::ctkExampleWorkflowWidgetStepUsingSignalsAndSlotsPrivate()
{
  this->widget = 0;
  this->label = 0;
  this->lineEdit = 0;

  this->defaultLineEditValue = 10;

  this->numberOfTimesRanOnEntry = 0;
  this->numberOfTimesRanOnExit = 0;
}

//-----------------------------------------------------------------------------
// ctkExampleWorkflowWidgetStepUsingSignalsAndSlots methods

//-----------------------------------------------------------------------------
ctkExampleWorkflowWidgetStepUsingSignalsAndSlots::ctkExampleWorkflowWidgetStepUsingSignalsAndSlots(QObject* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots);
}

//-----------------------------------------------------------------------------
CTK_GET_CXX(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots, QWidget*, widget, widget);
CTK_SET_CXX(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots, QWidget*, setWidget, widget);
CTK_GET_CXX(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots, QLabel*, label, label);
CTK_SET_CXX(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots, QLabel*, setLabel, label);
CTK_GET_CXX(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots, QLineEdit*, lineEdit, lineEdit);
CTK_SET_CXX(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots, QLineEdit*, setLineEdit, lineEdit);
CTK_GET_CXX(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots, int, numberOfTimesRanOnEntry, numberOfTimesRanOnEntry);
CTK_GET_CXX(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots, int, numberOfTimesRanOnExit, numberOfTimesRanOnExit);

//-----------------------------------------------------------------------------
void ctkExampleWorkflowWidgetStepUsingSignalsAndSlots::onEntry(
    const ctkWorkflowStep* comingFrom,
    const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(comingFrom);
  Q_UNUSED(transitionType);

  // simply implements our counter of the number of times we have run
  // this function
  CTK_D(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots);
  d->numberOfTimesRanOnEntry++;

  // signals that we are finished
  emit onEntryComplete();
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowWidgetStepUsingSignalsAndSlots::onExit(
    const ctkWorkflowStep* goingTo,
    const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(goingTo);
  Q_UNUSED(transitionType);

  // simply implements our counter of the number of times we have run
  // this function
  CTK_D(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots);
  d->numberOfTimesRanOnExit++;

  // signals that we are finished
  emit onExitComplete();
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowWidgetStepUsingSignalsAndSlots::createUserInterface()
{
  CTK_D(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots);

  if (!d->widget)
    {
    return;
    }

  if (!d->widget->layout())
    {
    QVBoxLayout* layout = new QVBoxLayout();
    d->widget->setLayout(layout);
    }

  if (!d->label)
    {
    d->label = new QLabel();
    d->label->setText("enter a number greater than or equal to 10");
    d->widget->layout()->addWidget(d->label);
    }

  if (!d->lineEdit)
    {
    d->lineEdit = new QLineEdit();
    d->lineEdit->setInputMask("000");
    d->lineEdit->setText("10");
    d->widget->layout()->addWidget(d->lineEdit);
    }

  // signals that we are finished
  emit createUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowWidgetStepUsingSignalsAndSlots::validate(const QString& desiredBranchId)
{
  CTK_D(const ctkExampleWorkflowWidgetStepUsingSignalsAndSlots);
  bool retVal;
  int val;
  bool ok;
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
    //this->setStatusText("invalid (not an integer or empty)");
    retVal = false;
    }
  else if (val < 10)
    {
    //this->setStatusText("invalid (invalid number)");
    retVal = false;
    }
  else
    {
    //this->setStatusText("");
    retVal = true;
    }

  // return the validation results
  emit validationComplete(retVal, desiredBranchId);
}

