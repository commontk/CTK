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

// CTK includes
#include "ctkExampleWorkflowStepUsingSignalsAndSlots.h"
#include "ctkWorkflowStep.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate
{
public:
  ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate();

  // counters of the number of times we have run the onEntry()
  // and onExit() functions
  int numberOfTimesRanOnEntry;
  int numberOfTimesRanOnExit;

  ctkWorkflowStep * Step;
};

//-----------------------------------------------------------------------------
// ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate methods

//-----------------------------------------------------------------------------
ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate::ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate()
{
  this->numberOfTimesRanOnEntry = 0;
  this->numberOfTimesRanOnExit = 0;
  this->Step = 0;
}

//-----------------------------------------------------------------------------
// ctkExampleWorkflowStepUsingSignalsAndSlots methods

//-----------------------------------------------------------------------------
ctkExampleWorkflowStepUsingSignalsAndSlots::ctkExampleWorkflowStepUsingSignalsAndSlots(
    ctkWorkflowStep * newStep, QObject* newParent) : Superclass(newParent)
  , d_ptr(new ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate)
{
  Q_D(ctkExampleWorkflowStepUsingSignalsAndSlots);
  d->Step = newStep;
}

//-----------------------------------------------------------------------------
ctkExampleWorkflowStepUsingSignalsAndSlots::~ctkExampleWorkflowStepUsingSignalsAndSlots()
{
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowStepUsingSignalsAndSlots::validate(const QString& desiredBranchId)const
{
  Q_D(const ctkExampleWorkflowStepUsingSignalsAndSlots);
  // Always returns true in this simple example

  QObject::staticMetaObject.invokeMethod(
      d->Step->ctkWorkflowStepQObject(), "validationComplete",
      Q_ARG(bool, true), Q_ARG(QString, desiredBranchId));
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowStepUsingSignalsAndSlots::validateFails()const
{
  Q_D(const ctkExampleWorkflowStepUsingSignalsAndSlots);

  // Always returns false in this simple example
  QObject::staticMetaObject.invokeMethod(
      d->Step->ctkWorkflowStepQObject(), "validationComplete",
        Q_ARG(bool, false), Q_ARG(QString, ""));
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowStepUsingSignalsAndSlots::onEntry(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(comingFrom);
  Q_UNUSED(transitionType);

  // simply implements our counter of the number of times we have run
  // this function
  Q_D(ctkExampleWorkflowStepUsingSignalsAndSlots);
  d->numberOfTimesRanOnEntry++;

  // signals that we are finished
  QObject::staticMetaObject.invokeMethod(
      d->Step->ctkWorkflowStepQObject(), "onEntryComplete");
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowStepUsingSignalsAndSlots::onExit(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(goingTo);
  Q_UNUSED(transitionType);

  // simply implements our counter of the number of times we have run
  // this function
  Q_D(ctkExampleWorkflowStepUsingSignalsAndSlots);
  d->numberOfTimesRanOnExit++;

  // signals that we are finished
  QObject::staticMetaObject.invokeMethod(
      d->Step->ctkWorkflowStepQObject(), "onExitComplete");
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkExampleWorkflowStepUsingSignalsAndSlots, int, numberOfTimesRanOnEntry, numberOfTimesRanOnEntry);
CTK_GET_CPP(ctkExampleWorkflowStepUsingSignalsAndSlots, int, numberOfTimesRanOnExit, numberOfTimesRanOnExit);

