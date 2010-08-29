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
#include <QState>

// CTK includes
#include "ctkLogger.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflow.h"

// STD includes
#include <iostream>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.core.ctkWorkflowStep");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkWorkflowStepPrivate: public ctkPrivate<ctkWorkflowStep>
{
public:
  CTK_DECLARE_PUBLIC(ctkWorkflowStep);
  ctkWorkflowStepPrivate();
  ~ctkWorkflowStepPrivate();

  ctkWorkflow* Workflow;

  QString      Id;
  QString      Name;
  QString      Description;
  QString      StatusText;

  QState* ProcessingState;
  QState* ValidationState;

  ctkWorkflowIntrastepTransition* ValidationTransition;
  ctkWorkflowIntrastepTransition* ValidationFailedTransition;

  bool HasValidateCommand;
  bool HasOnEntryCommand;
  bool HasOnExitCommand;
};

// --------------------------------------------------------------------------
// ctkWorkflowStepPrivate methods

// --------------------------------------------------------------------------
ctkWorkflowStepPrivate::ctkWorkflowStepPrivate()
{
  this->Workflow = 0;

  this->HasValidateCommand = false;
  this->HasOnEntryCommand = false;
  this->HasOnExitCommand = false;

  // Create state
  this->ProcessingState = new QState();
  this->ValidationState = new QState();

  // Create 'validation' transition
  this->ValidationTransition =
      new ctkWorkflowIntrastepTransition(ctkWorkflowIntrastepTransition::ValidationTransition);
  this->ValidationTransition->setTargetState(this->ValidationState);
  this->ProcessingState->addTransition(this->ValidationTransition);

  // Create 'failed validation' transation
  this->ValidationFailedTransition = 0;
  this->ValidationFailedTransition =
      new ctkWorkflowIntrastepTransition(ctkWorkflowIntrastepTransition::ValidationFailedTransition);
  this->ValidationFailedTransition->setTargetState(this->ProcessingState);
  this->ValidationState->addTransition(this->ValidationFailedTransition);
}

// --------------------------------------------------------------------------
ctkWorkflowStepPrivate::~ctkWorkflowStepPrivate()
{
  delete this->ValidationState;
  delete this->ProcessingState;

  // If we delete the states, then Qt will handle deleting the transitions
}

// --------------------------------------------------------------------------
// ctkWorkflowStep methods

// --------------------------------------------------------------------------
ctkWorkflowStep::ctkWorkflowStep(ctkWorkflow* newWorkflow, const QString& newId) : Superclass()
{
  CTK_INIT_PRIVATE(ctkWorkflowStep);
  CTK_D(ctkWorkflowStep);

  if (newId.isEmpty())
    {
    d->Id = this->metaObject()->className();
    }
  else
    {
    d->Id = newId;
    }

  d->Workflow = newWorkflow;
}

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, ctkWorkflow*, workflow, Workflow);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, QString, id, Id);
CTK_SET_CXX(ctkWorkflowStep, const QString&, setId, Id);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, QString, name, Name);
CTK_SET_CXX(ctkWorkflowStep, const QString&, setName, Name);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, QString, description, Description);
CTK_SET_CXX(ctkWorkflowStep, const QString&, setDescription, Description);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, QString, statusText, StatusText);
CTK_SET_CXX(ctkWorkflowStep, const QString&, setStatusText, StatusText);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, bool, hasValidateCommand, HasValidateCommand);
CTK_SET_CXX(ctkWorkflowStep, bool, setHasValidateCommand, HasValidateCommand);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, bool, hasOnEntryCommand, HasOnEntryCommand);
CTK_SET_CXX(ctkWorkflowStep, bool, setHasOnEntryCommand, HasOnEntryCommand);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, bool, hasOnExitCommand, HasOnExitCommand);
CTK_SET_CXX(ctkWorkflowStep, bool, setHasOnExitCommand, HasOnExitCommand);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, QState*, processingState, ProcessingState);
CTK_GET_CXX(ctkWorkflowStep, QState*, validationState, ValidationState);

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowStep, ctkWorkflowIntrastepTransition*, validationTransition, ValidationTransition);
CTK_GET_CXX(ctkWorkflowStep, ctkWorkflowIntrastepTransition*,
            validationFailedTransition, ValidationFailedTransition);

// --------------------------------------------------------------------------
void ctkWorkflowStep::validate(const QString& desiredBranchId)
{
  CTK_D(ctkWorkflowStep);
  logger.info(QString("validate - validating the input from %1").arg(d->Name));

  emit this->validationComplete(true, desiredBranchId);
}


// --------------------------------------------------------------------------
void ctkWorkflowStep::onEntry(const ctkWorkflowStep* comingFrom,
                              const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(comingFrom);
  Q_UNUSED(transitionType);

  // Signals that we are finished
  emit this->onEntryComplete();
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::onExit(const ctkWorkflowStep* goingTo,
                             const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(goingTo);
  Q_UNUSED(transitionType);

  // Signals that we are finished
  emit this->onExitComplete();
}
