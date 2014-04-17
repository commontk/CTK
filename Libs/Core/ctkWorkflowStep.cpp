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
#include <QDebug>
#include <QMetaType>
#include <QObject>
#include <QState>

// CTK includes
#include "ctkWorkflowStep.h"
#include "ctkWorkflowStep_p.h"
#include "ctkWorkflow.h"

// STD includes
#include <iostream>

// --------------------------------------------------------------------------
// ctkWorkflowStepPrivate methods

// --------------------------------------------------------------------------
ctkWorkflowStepPrivate::ctkWorkflowStepPrivate(ctkWorkflowStep& object)
  :q_ptr(&object)
{
  qRegisterMetaType<ctkWorkflowStep*>("ctkWorkflowStep*");
  this->Workflow = 0;

  this->WidgetType = false;

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
  if (!this->ValidationState.isNull())
    {
    delete this->ValidationState;
    }
  if (!this->ProcessingState.isNull())
    {
    delete this->ProcessingState;
    }

  // If we delete the states, then Qt will handle deleting the transitions
}

// --------------------------------------------------------------------------
void ctkWorkflowStepPrivate::validationCompleteInternal(bool validationResults, const QString& branchId)const
{
  emit validationComplete(validationResults, branchId);
}

// --------------------------------------------------------------------------
void ctkWorkflowStepPrivate::onEntryCompleteInternal()const
{
  emit onEntryComplete();
}

// --------------------------------------------------------------------------
void ctkWorkflowStepPrivate::onExitCompleteInternal()const
{
  emit onExitComplete();
}

// --------------------------------------------------------------------------
void ctkWorkflowStepPrivate::invokeValidateCommandInternal(const QString& desiredBranchId)const
{
  emit invokeValidateCommand(desiredBranchId);
}

// --------------------------------------------------------------------------
void ctkWorkflowStepPrivate::invokeOnEntryCommandInternal(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const
{
  emit invokeOnEntryCommand(comingFrom, transitionType);
}

// --------------------------------------------------------------------------
void ctkWorkflowStepPrivate::invokeOnExitCommandInternal(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const
{
  emit invokeOnExitCommand(goingTo, transitionType);
}

// --------------------------------------------------------------------------
// ctkWorkflowStep methods

// --------------------------------------------------------------------------
ctkWorkflowStep::ctkWorkflowStep(): d_ptr(new ctkWorkflowStepPrivate(*this))
{
}

// --------------------------------------------------------------------------
ctkWorkflowStep::ctkWorkflowStep(const QString& newId)
  : d_ptr(new ctkWorkflowStepPrivate(*this))
{
  Q_D(ctkWorkflowStep);
  d->Id = newId;
}

// --------------------------------------------------------------------------
ctkWorkflowStep::ctkWorkflowStep(ctkWorkflowStepPrivate * pimpl,
                                 const QString& newId):d_ptr(pimpl)
{
  Q_D(ctkWorkflowStep);
  d->Id = newId;
}

// --------------------------------------------------------------------------
ctkWorkflowStep::~ctkWorkflowStep()
{
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, ctkWorkflow*, workflow, Workflow);
CTK_SET_CPP(ctkWorkflowStep, ctkWorkflow*, setWorkflow, Workflow);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, QString, id, Id);

// --------------------------------------------------------------------------
void ctkWorkflowStep::setId(const QString& newId)
{
  Q_D(ctkWorkflowStep);
  if (d->Workflow && d->Workflow->hasStep(newId) && !this->id().isEmpty())
    {
    qWarning() << QString("ctkWorkflowStep - Failed to change id from '%1' to '%2' - "
                          "Step already added to a workflow !").arg(this->id()).arg(newId);
    return;
    }
  d->Id = newId;
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, QString, name, Name);
CTK_SET_CPP(ctkWorkflowStep, const QString&, setName, Name);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, QString, description, Description);
CTK_SET_CPP(ctkWorkflowStep, const QString&, setDescription, Description);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, QString, statusText, StatusText);
CTK_SET_CPP(ctkWorkflowStep, const QString&, setStatusText, StatusText);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, bool, hasValidateCommand, HasValidateCommand);
CTK_SET_CPP(ctkWorkflowStep, bool, setHasValidateCommand, HasValidateCommand);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, bool, hasOnEntryCommand, HasOnEntryCommand);
CTK_SET_CPP(ctkWorkflowStep, bool, setHasOnEntryCommand, HasOnEntryCommand);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, bool, hasOnExitCommand, HasOnExitCommand);
CTK_SET_CPP(ctkWorkflowStep, bool, setHasOnExitCommand, HasOnExitCommand);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, QState*, processingState, ProcessingState);
CTK_GET_CPP(ctkWorkflowStep, QState*, validationState, ValidationState);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, ctkWorkflowIntrastepTransition*, validationTransition, ValidationTransition);
CTK_GET_CPP(ctkWorkflowStep, ctkWorkflowIntrastepTransition*,
            validationFailedTransition, ValidationFailedTransition);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowStep, bool, isWidgetType, WidgetType);

// --------------------------------------------------------------------------
QObject* ctkWorkflowStep::ctkWorkflowStepQObject()
{
  Q_D(ctkWorkflowStep);
  return d;
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::validationComplete(bool validationResults, const QString& branchId)const
{
  Q_D(const ctkWorkflowStep);
  d->validationCompleteInternal(validationResults, branchId);
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::onEntryComplete()const
{
  Q_D(const ctkWorkflowStep);
  d->onEntryCompleteInternal();
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::onExitComplete()const
{
  Q_D(const ctkWorkflowStep);
  d->onExitCompleteInternal();
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::invokeValidateCommand(const QString& desiredBranchId)const
{
  Q_D(const ctkWorkflowStep);
  d->invokeValidateCommandInternal(desiredBranchId);
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::invokeOnEntryCommand(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const
{
  Q_D(const ctkWorkflowStep);
  d->invokeOnEntryCommandInternal(comingFrom, transitionType);
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::invokeOnExitCommand(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const
{
  Q_D(const ctkWorkflowStep);
  d->invokeOnExitCommandInternal(goingTo, transitionType);
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::validate(const QString& desiredBranchId)
{
  Q_D(ctkWorkflowStep);
  if (this->workflow()->verbose())
    {
    qDebug() << QString("validate - validating the input from %1").arg(d->Name);
    }

  this->validationComplete(true, desiredBranchId);
}


// --------------------------------------------------------------------------
void ctkWorkflowStep::onEntry(const ctkWorkflowStep* comingFrom,
                              const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(comingFrom);
  Q_UNUSED(transitionType);

  // Signals that we are finished
  this->onEntryComplete();
}

// --------------------------------------------------------------------------
void ctkWorkflowStep::onExit(const ctkWorkflowStep* goingTo,
                             const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)
{
  Q_UNUSED(goingTo);
  Q_UNUSED(transitionType);

 // Signals that we are finished
 this->onExitComplete();
}
