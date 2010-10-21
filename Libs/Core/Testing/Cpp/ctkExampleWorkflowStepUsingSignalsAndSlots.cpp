/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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
};

//-----------------------------------------------------------------------------
// ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate methods

//-----------------------------------------------------------------------------
ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate::ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate()
{
  this->numberOfTimesRanOnEntry = 0;
  this->numberOfTimesRanOnExit = 0;
}

//-----------------------------------------------------------------------------
// ctkExampleWorkflowStepUsingSignalsAndSlots methods

//-----------------------------------------------------------------------------
ctkExampleWorkflowStepUsingSignalsAndSlots::ctkExampleWorkflowStepUsingSignalsAndSlots(QObject* _parent) : Superclass(_parent)
  , d_ptr(new ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate)
{
}

//-----------------------------------------------------------------------------
ctkExampleWorkflowStepUsingSignalsAndSlots::~ctkExampleWorkflowStepUsingSignalsAndSlots()
{
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowStepUsingSignalsAndSlots::validate(const QString& desiredBranchId)const
{
  // Always returns true in this simple example
  emit validationComplete(true, desiredBranchId);
}

//-----------------------------------------------------------------------------
void ctkExampleWorkflowStepUsingSignalsAndSlots::validateFails()const
{
  // Always returns false in this simple example
  emit validationComplete(false);
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
  emit onEntryComplete();
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
  emit onExitComplete();
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkExampleWorkflowStepUsingSignalsAndSlots, int, numberOfTimesRanOnEntry, numberOfTimesRanOnEntry);
CTK_GET_CPP(ctkExampleWorkflowStepUsingSignalsAndSlots, int, numberOfTimesRanOnExit, numberOfTimesRanOnExit);

