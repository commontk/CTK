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
#include <QStateMachine>
#include <QState>

#include <QQueue>

// CTK includes
#include "ctkWorkflow.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowStep_p.h"
#include "ctkWorkflowTransitions.h"

// STD includes
#include <iostream>

// --------------------------------------------------------------------------
// ctkWorkflowPrivate methods

// --------------------------------------------------------------------------
ctkWorkflowPrivate::ctkWorkflowPrivate(ctkWorkflow& object)
  :q_ptr(&object)
{
  this->InitialStep = 0;
  this->CurrentStep = 0;

  this->OriginStep = 0;
  this->DestinationStep = 0;

  this->GoToStep = 0;
  this->StartingStep = 0;
  this->TransitionToPreviousStartingStep = 0;

  // By default, go back to the origin step upon success of the goToStep(targetId) attempt.
  this->GoBackToOriginStepUponSuccess = true;

  this->ARTIFICIAL_BRANCH_ID_PREFIX = "ctkWorkflowArtificialBranchId_";

  this->Verbose = false;
}

// --------------------------------------------------------------------------
ctkWorkflowPrivate::~ctkWorkflowPrivate()
{
}

// --------------------------------------------------------------------------
bool ctkWorkflowPrivate::addStep(ctkWorkflowStep* step)
{
  Q_Q(ctkWorkflow);

  Q_ASSERT(step);
  Q_ASSERT(!q->hasStep(step->id()));
  Q_ASSERT(!this->StateMachine->isRunning());

  if (!step->workflow())
    {
    step->setWorkflow(q);
    }
  if (step->workflow() != q)
    {
    // Check if steps are not already associated with a different workflow
    QString msg("addStep - step [%1] already associated with a different workfow !");
    qWarning() << msg.arg(step->id());
    return false;
    }
  if (!this->RegisteredSteps.contains(step))
    {
    this->RegisteredSteps << step;
    emit q->stepRegistered(step);
    }

  // Add the states, creating them if necessary
  this->StateMachine->addState(step->processingState());
  this->StateMachine->addState(step->validationState());

  // Update the map of steps to transitions and the <state,step> map
  this->StepToForwardAndBackwardStepMap.insert(step, new forwardAndBackwardSteps);

  this->StateToStepMap[step->processingState()] = step;
  this->StateToStepMap[step->validationState()] = step;

  // Setup the signal/slot that triggers the attempt to go to the next step
  QObject::connect(step->validationState(), SIGNAL(entered()),
                   q, SLOT(attemptToGoToNextStep()));

  // Setup the signal/slot that triggers the evaluation of the validation results
  // after validate(const QString&) is called
  this->connect(
      step->ctkWorkflowStepQObject(), SIGNAL(validationComplete(bool,QString)),
      q, SLOT(evaluateValidationResults(bool,QString)));

  this->connect(
      step->ctkWorkflowStepQObject(), SIGNAL(onEntryComplete()),
      SLOT(processingAfterOnEntry()));

  this->connect(
      step->ctkWorkflowStepQObject(), SIGNAL(onExitComplete()),
      SLOT(processingAfterOnExit()));

  return true;
}

// --------------------------------------------------------------------------
bool ctkWorkflowPrivate::hasDuplicateTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                                                const ctkWorkflow::TransitionDirectionality directionality)
{
  Q_Q(ctkWorkflow);

  Q_ASSERT(origin);
  Q_ASSERT(destination);
  Q_ASSERT(directionality == ctkWorkflow::Forward || ctkWorkflow::Backward);

  ctkWorkflowPrivate::StepListType stepList;
  ctkWorkflowStep* targetStep = 0;
  if (directionality == ctkWorkflow::Forward)
    {
    stepList = q->forwardSteps(origin);
    targetStep = destination;
    }
  else if (directionality == ctkWorkflow::Backward)
    {
    stepList = q->backwardSteps(destination);
    targetStep = origin;
    }

  foreach(ctkWorkflowStep * step, stepList)
    {
    if (step == targetStep)
      {
      return true;
      }
    }
  return false;
}

// --------------------------------------------------------------------------
bool ctkWorkflowPrivate::hasTransitionWithSameBranchId(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                                                       const QString& branchId,
                                                       const ctkWorkflow::TransitionDirectionality directionality)
{
  Q_ASSERT(origin);
  Q_ASSERT(destination);
  Q_ASSERT(directionality == ctkWorkflow::Forward || ctkWorkflow::Backward);
  Q_ASSERT(!branchId.isEmpty());

  QList<QString> branchIdList;

  if (directionality == ctkWorkflow::Forward)
    {
    branchIdList = this->forwardBranchIds(origin);
    }
  else if (directionality == ctkWorkflow::Backward)
    {
    branchIdList = this->backwardBranchIds(destination);
    }

  foreach(QString id, branchIdList)
    {
    if (QString::compare(id, branchId, Qt::CaseInsensitive) == 0)
      {
      return true;
      }
    }
  return false;
}

// --------------------------------------------------------------------------
void ctkWorkflowPrivate::createTransitionToNextStep(ctkWorkflowStep* origin,
                                                    ctkWorkflowStep* destination,
                                                    const QString& branchId)
{
  Q_Q(ctkWorkflow);

  Q_ASSERT(origin);
  Q_ASSERT(destination);
  Q_ASSERT(!q->hasTransition(origin, destination, branchId, ctkWorkflow::Forward));

  QString id;
  // create an artificial branchId if one is not given
  if (branchId.isEmpty())
    {
    id.setNum(this->numberOfForwardSteps(origin));
    id.prepend(this->ARTIFICIAL_BRANCH_ID_PREFIX);
    }
  else
    {
    id = branchId;
    }

  // Create the transition
  ctkWorkflowInterstepTransition* transition = new ctkWorkflowInterstepTransition(ctkWorkflowInterstepTransition::TransitionToNextStep, id);
  transition->setTargetState(destination->processingState());
  origin->validationState()->addTransition(transition);

  // Update the step to transitions map
  this->StepToForwardAndBackwardStepMap.value(origin)->appendForwardStep(destination, id);

  // Setup the signal/slot that shows and hides the steps' user interfaces
  // on transition to the next step
  QObject::connect(transition, SIGNAL(triggered()), q, SLOT(performTransitionBetweenSteps()));

}

// --------------------------------------------------------------------------
void ctkWorkflowPrivate::createTransitionToPreviousStep(ctkWorkflowStep* origin,
                                                        ctkWorkflowStep* destination,
                                                        const QString& branchId)
{
  Q_Q(ctkWorkflow);

  Q_ASSERT(origin);
  Q_ASSERT(destination);
  Q_ASSERT(!q->hasTransition(origin, destination, branchId, ctkWorkflow::Backward));

  QString id;
  // create an artificial branchId if one is not given
  if (branchId.isEmpty())
    {
    id.setNum(this->numberOfBackwardSteps(destination));
    id.prepend(this->ARTIFICIAL_BRANCH_ID_PREFIX);
    }
  else
    {
    id = branchId;
    }

  ctkWorkflowInterstepTransition* transition = new ctkWorkflowInterstepTransition(ctkWorkflowInterstepTransition::TransitionToPreviousStep, id);
  transition->setTargetState(origin->processingState());
  destination->processingState()->addTransition(transition);

  // Update the step to transitions map
  this->StepToForwardAndBackwardStepMap.value(destination)->appendBackwardStep(origin, id);

  // Setup the signal/slot that shows and hides the steps' user
  // interfaces on transition to the previous step
  QObject::connect(transition, SIGNAL(triggered()), q, SLOT(performTransitionBetweenSteps()));
}

// --------------------------------------------------------------------------
void ctkWorkflowPrivate::createTransitionToPreviousStartingStep(ctkWorkflowStep* startingStep,
                                                                ctkWorkflowStep* currentStep)
{
  Q_Q(ctkWorkflow);

  Q_ASSERT(startingStep);
  Q_ASSERT(currentStep);

  if (!this->TransitionToPreviousStartingStep)
    {
    ctkWorkflowInterstepTransition* transition = new ctkWorkflowInterstepTransition(
        ctkWorkflowInterstepTransition::TransitionToPreviousStartingStepAfterSuccessfulGoToFinishStep);

    // Setup the signal/slot that shows and hides the steps' user interfaces
    // on transition to the previous step
    QObject::connect(transition, SIGNAL(triggered()), q, SLOT(performTransitionBetweenSteps()));

    this->TransitionToPreviousStartingStep = transition;
    }

  QState* currentState;
  // looping on the finish step
  if (startingStep == currentStep)
    {
    currentState = currentStep->validationState();
    }
  else
    {
    currentState = currentStep->processingState();
    }

    this->TransitionToPreviousStartingStep->setTargetState(startingStep->processingState());
    currentState->addTransition(this->TransitionToPreviousStartingStep);
}

// --------------------------------------------------------------------------
ctkWorkflowStep* ctkWorkflowPrivate::stepFromId(const QString& id)const
{
  foreach(ctkWorkflowStep* step, this->StepToForwardAndBackwardStepMap.keys())
    {
    Q_ASSERT(step);
    if (QString::compare(step->id(), id, Qt::CaseInsensitive) == 0)
      {
      return step;
      }
    }
  return 0;
}

// --------------------------------------------------------------------------
QList<QString> ctkWorkflowPrivate::forwardBranchIds(ctkWorkflowStep* step)const
{
  Q_ASSERT(step);
  return this->StepToForwardAndBackwardStepMap.value(step)->forwardBranchIds();
}

// --------------------------------------------------------------------------
QList<QString> ctkWorkflowPrivate::backwardBranchIds(ctkWorkflowStep* step)const
{
  Q_ASSERT(step);
  return this->StepToForwardAndBackwardStepMap.value(step)->backwardBranchIds();
}

//---------------------------------------------------------------------------
void ctkWorkflowPrivate::validateInternal(ctkWorkflowStep* step)
{
  Q_ASSERT(step);

  if(this->Verbose)
    {
    qDebug() << QString("validateInternal - validating input from %1").arg(step->name());
    }

  if (step->hasValidateCommand())
    {
    step->invokeValidateCommand(this->DesiredBranchId);
    }
  else
    {
    step->validate(this->DesiredBranchId);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowPrivate::onEntryInternal(
    ctkWorkflowStep* step,
    ctkWorkflowStep* comingFrom,
    const ctkWorkflowInterstepTransition::InterstepTransitionType& transitionType)
{
  Q_ASSERT(step);

  if(this->Verbose)
    {
    qDebug() << QString("onEntryInternal - entering input from %1").arg(step->name());
    }

  //Ensure we are transitioning between steps or starting the workflow
  Q_ASSERT(transitionType == ctkWorkflowInterstepTransition::TransitionToNextStep
           || transitionType == ctkWorkflowInterstepTransition::TransitionToPreviousStep
           || transitionType == ctkWorkflowInterstepTransition::StartingWorkflow
           || transitionType == ctkWorkflowInterstepTransition::TransitionToPreviousStartingStepAfterSuccessfulGoToFinishStep);

  if (step->hasOnEntryCommand())
    {
    step->invokeOnEntryCommand(comingFrom, transitionType);
    }
  else
    {
    step->onEntry(comingFrom, transitionType);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowPrivate::processingAfterOnEntry()
{
  Q_Q(ctkWorkflow);

  if (this->Verbose)
    {
    qDebug() << "processingAfterOnEntry";
    }

  if (!this->DestinationStep)
    {
    qWarning() << "processingAfterOnEntry - Called processingAfterOnEntry without "
                  "having set a destination step";
    return;
    }

  // Update the currentStep and previous step
  this->CurrentStep = this->DestinationStep;

  // Reset the pointers used internally for performing a transition
  this->OriginStep = 0;
  this->DestinationStep = 0;

  // // Reset the pointers used internally for performing a transition
  // // back to the starting step
  // if (d->TransitionToPreviousStartingStep)
  //   {

  //   std::cout << "TRANSITION TO PREVIOUS STARTING STEP EXISTS" << std::endl;
  //   //d->TransitionToPreviousStartingStep->sourceState()->removeTransition(d->TransitionToPreviousStartingStep);
  //   //std::cout << "removed" << std::endl;
  //   // d->TransitionToPreviousStartingStep = 0;
  //   //destination->processingState()->removeTransition(d->TransitionToPreviousStartingStep);
  //   //delete d->TransitionToPreviousStartingStep;
  //   // d->TransitionToPreviousStartingStep = 0;
  //   std::cout << "here" << std::endl;
  //   }

  // If we are trying to get to the finish step, then check if we are
  // finished.
  if (this->GoToStep)
    {
    if (this->CurrentStep == this->GoToStep)
      {
      q->goToStepSucceeded();
      }
    // if we're not finished, continue transitioning to the next step
    else
      {
      q->goForward();
      }
    }
  else
    {
    emit q->currentStepChanged(this->CurrentStep);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowPrivate::onExitInternal(
    ctkWorkflowStep* step,
    ctkWorkflowStep* goingTo,
    const ctkWorkflowInterstepTransition::InterstepTransitionType& transitionType)
{
  Q_ASSERT(step);

  if (this->Verbose)
    {
    qDebug() << QString("onExitInternal - exiting %1").arg(step->name());
    }

  // Ensure we are transitioning between steps or starting the workflow
  Q_ASSERT (transitionType == ctkWorkflowInterstepTransition::TransitionToNextStep ||
            transitionType == ctkWorkflowInterstepTransition::TransitionToPreviousStep ||
            transitionType == ctkWorkflowInterstepTransition::StoppingWorkflow ||
            transitionType == ctkWorkflowInterstepTransition::TransitionToPreviousStartingStepAfterSuccessfulGoToFinishStep);

  if (step->hasOnExitCommand())
    {
    step->invokeOnExitCommand(goingTo, transitionType);
    }
  else
    {
    step->onExit(goingTo, transitionType);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowPrivate::processingAfterOnExit()
{
  // enter the destination step if we have one
  if (this->DestinationStep)
    {
    this->onEntryInternal(this->DestinationStep, this->OriginStep, this->TransitionType);
    }
  // reset the pointers used internally for performing a transition if we're done
  else
    {
    this->OriginStep = 0;
    this->DestinationStep = 0;
    // we've exited the CurrentStep and haven't gone into another step, so we no longer have a
    // currentStep.
    this->CurrentStep = 0;
    }
}

// --------------------------------------------------------------------------
ctkWorkflowStep* ctkWorkflowPrivate::stepFromState(const QAbstractState* state)
{
  if (state)
    {
    return this->StateToStepMap.value(state);
    }
  return 0;
}

// --------------------------------------------------------------------------
int ctkWorkflowPrivate::numberOfForwardSteps(ctkWorkflowStep* step)
{
  Q_Q(ctkWorkflow);
  return q->forwardSteps(step).length();
}

// --------------------------------------------------------------------------
int ctkWorkflowPrivate::numberOfBackwardSteps(ctkWorkflowStep* step)
{
  Q_Q(ctkWorkflow);
  return q->backwardSteps(step).length();
}

// --------------------------------------------------------------------------
bool ctkWorkflowPrivate::pathExists(const QString& goalId, ctkWorkflowStep* origin)const
{
  Q_Q(const ctkWorkflow);

  Q_ASSERT(!goalId.isEmpty());
  Q_ASSERT(this->CurrentStep);

  QString originId;
  if (origin)
    {
    originId = origin->id();
    }
  else
    {
    originId = this->CurrentStep->id();
    }

  // there exists a path from the origin to the goal if:
  // - there is a goal AND
  // - either:
  //   - the origin is already the goal
  //   - there is a path from at least one of the origin's successors to the goal
  return (q->hasStep(goalId)
          && ((QString::compare(goalId, originId, Qt::CaseInsensitive) == 0)
              || (q->canGoForward(origin)))); // <-- TODO insert logic here for looking at graph
}

// --------------------------------------------------------------------------
bool ctkWorkflowPrivate::pathExistsFromNextStep(const QString& goalId, const QString& branchId)const
{
  Q_ASSERT(!goalId.isEmpty());
  Q_ASSERT(!branchId.isEmpty());
  Q_ASSERT(this->CurrentStep);

  // return whether there exists a path from the the step that will be followed (given the branchId) to the goal
  ctkWorkflowStep* nextStep = this->StepToForwardAndBackwardStepMap.value(this->CurrentStep)->forwardStep(branchId);

  if (!nextStep)
    {
    return false;
    }
  else
    {
    return this->pathExists(goalId, nextStep);
    }
}

// --------------------------------------------------------------------------
// ctkWorkflow methods

// --------------------------------------------------------------------------
ctkWorkflow::ctkWorkflow(QObject* _parent) : Superclass(_parent)
  , d_ptr(new ctkWorkflowPrivate(*this))
{
  Q_D(ctkWorkflow);
  d->StateMachine = new QStateMachine(this);
}

// --------------------------------------------------------------------------
ctkWorkflow::~ctkWorkflow()
{
  Q_D(ctkWorkflow);

  if (d->StateMachine->isRunning())
    {
    d->StateMachine->stop();
    }

  // Clean registered step
  while (!d->RegisteredSteps.isEmpty())
    {
    delete d->RegisteredSteps.takeFirst();
    }
}

// --------------------------------------------------------------------------
bool ctkWorkflow::addTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                                const QString& branchId,
                                const ctkWorkflow::TransitionDirectionality directionality)
{
  Q_D(ctkWorkflow);

  if (d->StateMachine->isRunning())
    {
    qWarning() << "addTransition - Cannot add a transition while the workflow is started !";
    return false;
    }

  // Set origin id if empty
  if (origin && origin->id().isEmpty())
    {
    origin->setId(QString("step%1").arg(d->StepToForwardAndBackwardStepMap.count()));
    }

  // cannot currently create a transition between two steps of the same id, which is equivalent to
  // adding a transition from a step to itself
  if (origin && destination && (QString::compare(origin->id(), destination->id(), Qt::CaseInsensitive) == 0))
    {
    qWarning() << "addTransition - Workflow does not currently support a transition"
                  " from a step to itself.  Use GoToStep instead !";
    return false;
    }

  // add the origin step if it doesn't exist in the workflow yet
  if (origin && !this->hasStep(origin->id()))
    {
    bool ok = d->addStep(origin);
    if (!ok)
      {
      return false;
      }
    }

  // Set destination id if empty
  if (destination && destination->id().isEmpty())
    {
    destination->setId(QString("step%1").arg(d->StepToForwardAndBackwardStepMap.count()));
    }

  // add the destination step if it doesn't exist in the workflow yet
  if (destination && !this->hasStep(destination->id()))
    {
    bool ok = d->addStep(destination);
    if (!ok)
      {
      return false;
      }
    }

  if (origin && destination)
    {
    // ensure we haven't already added a transition with the same origin, destination and directionality
    if (this->hasTransition(origin, destination, branchId, directionality))
      {
      qWarning() << "addTransition - Cannot create a transition that matches a "
                    "previously created transition";
      return false;
      }

    // create the forward transition
    if (directionality == ctkWorkflow::Forward
        || directionality == ctkWorkflow::Bidirectional)
      {
      //qDebug() << "addTransition" << origin->id() << "->" << destination->id();
      d->createTransitionToNextStep(origin, destination, branchId);
      }

    // create the backward transition
    if (directionality == ctkWorkflow::Backward
        || directionality == ctkWorkflow::Bidirectional)
      {
      //qDebug() << "addTransition" << origin->id() << "<-" << destination->id();
      d->createTransitionToPreviousStep(origin, destination, branchId);
      }
    }

  // Set initialStep if needed
  if (origin && d->StepToForwardAndBackwardStepMap.count() == 2 && !this->initialStep())
    {
    this->setInitialStep(origin);
    }

  return true;
}

// --------------------------------------------------------------------------
bool ctkWorkflow::hasTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                                const QString& branchId,
                                const ctkWorkflow::TransitionDirectionality directionality)
{
  Q_D(ctkWorkflow);

  // we have a bidirectional transition if we have both a forward and a backward transition
  if (directionality == ctkWorkflow::Bidirectional)
    {
    return this->hasTransition(origin, destination, branchId, ctkWorkflow::Forward)
      && this->hasTransition(origin, destination, branchId, ctkWorkflow::Backward);
    }
  else
    {
    if (branchId.isEmpty())
      {
      return d->hasDuplicateTransition(origin, destination, directionality);
      }
    else
      {
      return d->hasDuplicateTransition(origin, destination, directionality)
        || d->hasTransitionWithSameBranchId(origin, destination, branchId, directionality);
      }
    }
}

// --------------------------------------------------------------------------
QList<ctkWorkflowStep*> ctkWorkflow::forwardSteps(ctkWorkflowStep* step)const
{
  Q_D(const ctkWorkflow);
  // use the given step if provided, otherwise use the workflow's current step
  if (step && d->StepToForwardAndBackwardStepMap.contains(step))
    {
    return d->StepToForwardAndBackwardStepMap.value(step)->forwardSteps();
    }
  else if (d->CurrentStep && d->StepToForwardAndBackwardStepMap.contains(d->CurrentStep))
    {
    return d->StepToForwardAndBackwardStepMap.value(d->CurrentStep)->forwardSteps();
    }
  else
    {
    return QList<ctkWorkflowStep*>();
    }
}

// --------------------------------------------------------------------------
QList<ctkWorkflowStep*> ctkWorkflow::backwardSteps(ctkWorkflowStep* step)const
{
  Q_D(const ctkWorkflow);
  // use the current step if provided, otherwise use the workflow's current step
  if (step && d->StepToForwardAndBackwardStepMap.contains(step))
    {
    return d->StepToForwardAndBackwardStepMap.value(step)->backwardSteps();
    }
  else if (d->CurrentStep && d->StepToForwardAndBackwardStepMap.contains(d->CurrentStep))
    {
    return d->StepToForwardAndBackwardStepMap.value(d->CurrentStep)->backwardSteps();
    }
  else
    {
    return QList<ctkWorkflowStep*>();
    }
}

// --------------------------------------------------------------------------
QList<ctkWorkflowStep*> ctkWorkflow::finishSteps()const
{
  Q_D(const ctkWorkflow);

  // iterate through our list of steps, and keep the steps that don't have anything following them
  QList<ctkWorkflowStep*> finishSteps;
  foreach (ctkWorkflowStep* step, d->StepToForwardAndBackwardStepMap.keys())
    {
    if (!this->canGoForward(step))
      {
      finishSteps.append(step);
      }
    }
  return finishSteps;
}

// --------------------------------------------------------------------------
QList<ctkWorkflowStep*> ctkWorkflow::steps()const
{
  Q_D(const ctkWorkflow);
  return d->RegisteredSteps;
}

// --------------------------------------------------------------------------
bool ctkWorkflow::canGoForward(ctkWorkflowStep* step)const
{
  return (!this->forwardSteps(step).isEmpty());
}

// --------------------------------------------------------------------------
bool ctkWorkflow::canGoBackward(ctkWorkflowStep* step)const
{
  return (!this->backwardSteps(step).isEmpty());
}

// --------------------------------------------------------------------------
bool ctkWorkflow::canGoToStep(const QString& targetId, ctkWorkflowStep* step)const
{
  Q_D(const ctkWorkflow);
  return d->pathExists(targetId, step);
}

// --------------------------------------------------------------------------
bool ctkWorkflow::hasStep(const QString& id)const
{
  Q_D(const ctkWorkflow);
  return d->stepFromId(id);
}

// --------------------------------------------------------------------------
ctkWorkflowStep* ctkWorkflow::step(const QString& id)const
{
  Q_D(const ctkWorkflow);
  return d->stepFromId(id);
}


// --------------------------------------------------------------------------
// Convenience method to set the QStateMachine's initialState to a
// specific step's processing state.
CTK_GET_CPP(ctkWorkflow, ctkWorkflowStep*, initialStep, InitialStep);
CTK_SET_CPP(ctkWorkflow, ctkWorkflowStep*, setInitialStep, InitialStep);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflow, ctkWorkflowStep*, currentStep, CurrentStep);

// --------------------------------------------------------------------------
void ctkWorkflow::start()
{
  Q_D(ctkWorkflow);
  if (!d->InitialStep)
    {
    qWarning() << "start - Cannot start workflow without an initial step";
    return;
    }

  // Setup to do the entry processing for the initial setp
  d->StateMachine->setInitialState(d->InitialStep->processingState());
  d->OriginStep = 0;
  d->DestinationStep = d->InitialStep;
  d->TransitionType = ctkWorkflowInterstepTransition::StartingWorkflow;
  d->onEntryInternal(d->DestinationStep, d->OriginStep, d->TransitionType);

  d->StateMachine->start();
}

// --------------------------------------------------------------------------
bool ctkWorkflow::isRunning()const
{
  Q_D(const ctkWorkflow);
  return d->StateMachine->isRunning();
}

// --------------------------------------------------------------------------
void ctkWorkflow::stop()
{
  Q_D(ctkWorkflow);
  if (!d->StateMachine->isRunning())
    {
    return;
    }

  // Setup to do the exit processing for the current step
  if (d->CurrentStep)
    {
    d->OriginStep = d->CurrentStep;
    d->DestinationStep = 0;
    d->TransitionType = ctkWorkflowInterstepTransition::StoppingWorkflow;
    d->onExitInternal(d->OriginStep, d->DestinationStep, d->TransitionType);
    }

  d->StateMachine->stop();
}

// --------------------------------------------------------------------------
void ctkWorkflow::goForward(const QString& desiredBranchId)
{
  Q_D(ctkWorkflow);

  if (!this->isRunning())
    {
    qWarning() << "goForward - The workflow is not running !";
    return;
    }

  // if we're just going to the next step and not to a 'goTo' step, then check to make sure that
  // there exists a step following the current step
  if (!d->GoToStep)
    {
    if (!this->canGoForward())
      {
      qWarning() << "goForward - Attempt to goForward from a finish step !";
      return;
      }
    }

  d->DesiredBranchId = desiredBranchId;

  if (d->Verbose)
    {
    qDebug() << "goForward - posting ValidationTransition";
    }
  d->StateMachine->postEvent(
      new ctkWorkflowIntrastepTransitionEvent(ctkWorkflowIntrastepTransition::ValidationTransition));
}

// --------------------------------------------------------------------------
void ctkWorkflow::goBackward(const QString& desiredBranchId)
{
  Q_D(ctkWorkflow);

  if (!this->isRunning())
    {
    qWarning() << "goBackward - The workflow is not running !";
    return;
    }

  if (!this->canGoBackward())
    {
    qWarning() << "goBackward - Attempt to goBackward from first step !";
    return;
    }

  ctkWorkflowStep* previousStep = d->StepToPreviousStepMap[d->CurrentStep];
  Q_ASSERT(previousStep);
  QString branchId = d->StepToForwardAndBackwardStepMap.value(d->CurrentStep)->backwardBranchId(previousStep);
  Q_ASSERT(!branchId.isEmpty());

  d->DesiredBranchId = desiredBranchId;

  if (d->Verbose)
    {
    qDebug() << "goBackward - posting TransitionToPreviousStep";
    }
  d->StateMachine->postEvent(
                             new ctkWorkflowInterstepTransitionEvent(ctkWorkflowInterstepTransition::TransitionToPreviousStep, branchId));
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflow, bool, goBackToOriginStepUponSuccess, GoBackToOriginStepUponSuccess);
CTK_SET_CPP(ctkWorkflow, bool, setGoBackToOriginStepUponSuccess, GoBackToOriginStepUponSuccess);

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflow, bool, verbose, Verbose);
CTK_SET_CPP(ctkWorkflow, bool, setVerbose, Verbose);

// --------------------------------------------------------------------------
void ctkWorkflow::goToStep(const QString& targetId)
{
  Q_D(ctkWorkflow);

  if (!this->isRunning())
    {
    qWarning() << "goToStep - The workflow is not running !";
    return;
    }

  // TODO currently returns true only if the workflow is running - need logic here
  if (!this->canGoToStep(targetId))
    {
    qWarning() << QString("goToStep - Cannot goToStep %1 ").arg(targetId);
    return;
    }

#ifndef QT_NO_DEBUG
  ctkWorkflowStep* step = d->stepFromId(targetId);
  Q_ASSERT(step);
#endif

  if (d->Verbose)
    {
    qDebug() << QString("goToStep - Attempting to go to finish step %1").arg(targetId);
    }

  // if (step == d->CurrentStep)
  //   {
  //   if (d->Verbose)
  //     {
  //     qDebug() << "we are already in the desired finish step";
  //     }
  //   return;
  //   }

  d->GoToStep = d->stepFromId(targetId);
  d->StartingStep = d->CurrentStep;
  this->goForward();
}

// --------------------------------------------------------------------------
void ctkWorkflow::attemptToGoToNextStep()
{
  Q_D(ctkWorkflow);
  if (d->Verbose)
    {
    qDebug() << "attemptToGoToNextStep - Attempting to go to the next step ";
    }

  Q_ASSERT(d->CurrentStep);
  //Q_ASSERT(this->canGoForward(d->CurrentStep));

  d->validateInternal(d->CurrentStep);
}

// --------------------------------------------------------------------------
void ctkWorkflow::evaluateValidationResults(bool validationSucceeded, const QString& branchId)
{
  if (validationSucceeded)
    {
    this->goToNextStepAfterSuccessfulValidation(branchId);
    }
  else
    {
    this->goToProcessingStateAfterValidationFailed();
    }
}

// --------------------------------------------------------------------------
// if ctkWorkflowStep::validationComplete() did not provide a branchId, then:
// - if there is one step following the current step, we will follow that transition
// - if there are multiple steps following the current step, then we will follow the first
// transition that was added
// (either way this corresponds to following the first forwardBranchId we've recorded)
// if ctkWorkflowStep::validationComplete() provided a branchId, then:
// - if there is one transition following the current step that was not created using a branchId,
// then we will follow it
// - otherwise do a conditional branching based on the branchId provided by validationComplete()

void ctkWorkflow::goToNextStepAfterSuccessfulValidation(const QString& branchId)
{
  Q_D(ctkWorkflow);
  if (d->Verbose)
    {
    qDebug() << "goToNextStepAfterSuccessfulValidation - Validation succeeded";
    qDebug() << "goToNextStepAfterSuccessfulValidation - Posting TransitionToNextStep";
    }

  // we may already be in the 'goTo' step - i.e. looping on a finish step
  if (d->GoToStep && d->CurrentStep == d->GoToStep)
    {
    this->goToStepSucceeded();
    return;
    }

  QString transitionBranchId;

  // these values are helpful for the logic below
  QString firstForwardBranchId = d->StepToForwardAndBackwardStepMap.value(d->CurrentStep)->firstForwardBranchId();
  int numberOfForwardSteps = d->numberOfForwardSteps(d->CurrentStep);
  Q_ASSERT(!firstForwardBranchId.isEmpty());
  Q_ASSERT(numberOfForwardSteps);

  // validationComplete() does not give us a branchId
  if (branchId.isEmpty())
    {
    transitionBranchId = firstForwardBranchId;
    if (numberOfForwardSteps > 1)
      {
      qWarning() << "goToNextStepAfterSuccessfulValidation - ctkWorkflowStep::ValidatComplete() "
                    "did not provide branchId at a branch in the workflow - will follow first "
                    "transition that was created";
      }
    }
  // validationComplete() gives us a branchId
  else
    {
    if (numberOfForwardSteps == 1 && firstForwardBranchId.contains(d->ARTIFICIAL_BRANCH_ID_PREFIX))
      {
      transitionBranchId = firstForwardBranchId;
      qWarning() << "goToNextStepAfterSuccessfulValidation -  ctkWorkflowStep::ValidationComplete()"
                    " returns a branchId, but was overridden by the workflow";
      }
    else
      {
      transitionBranchId = branchId;
      }
    }

  // if we are trying to go to a 'goTo' step, check that the selected branch will still take us along a path that leads to the 'goTo' step, and fail if not
  if (d->GoToStep && !d->pathExistsFromNextStep(d->GoToStep->id(), transitionBranchId))
    {
    this->goToProcessingStateAfterValidationFailed();
    return;
    }

  d->StateMachine->postEvent(new ctkWorkflowInterstepTransitionEvent(ctkWorkflowInterstepTransition::TransitionToNextStep, transitionBranchId));
}

// --------------------------------------------------------------------------
void ctkWorkflow::goToProcessingStateAfterValidationFailed()
{
  Q_D(ctkWorkflow);
  if (d->Verbose)
    {
    qDebug() << "goToNextStepAfterSuccessfulValidation - Validation failed";
    }
  // Validation failed in the process of attempting to go to the finish step
  if (d->GoToStep)
    {
    this->goToStepFailed();
    }

  if (d->Verbose)
    {
    qDebug() << "goToNextStepAfterSuccessfulValidation - Posting ValidationFailedTransition";
    }
  d->StateMachine->postEvent(new ctkWorkflowIntrastepTransitionEvent(ctkWorkflowIntrastepTransition::ValidationFailedTransition));
}

// --------------------------------------------------------------------------
void ctkWorkflow::performTransitionBetweenSteps()
{
  Q_D(ctkWorkflow);
  if (d->Verbose)
    {
    qDebug() << "performTransitionBetweenSteps - Performing transition between steps";
    }

  // Alternative: could find the origin and destination step based on
  // d->CurrentStep rather than QObject::sender(), but would require
  // keeping track of an origin step's destination step (and would be
  // tricky in an extension to branching workflows, unless we change
  // this method signature)

  ctkWorkflowInterstepTransition* transition = qobject_cast<ctkWorkflowInterstepTransition*>(QObject::sender());
  Q_ASSERT(transition);

  d->OriginStep = d->stepFromState(transition->sourceState());
  d->DestinationStep = d->stepFromState(transition->targetState());
  d->TransitionType = transition->transitionType();
  Q_ASSERT(d->TransitionType == ctkWorkflowInterstepTransition::TransitionToNextStep
           || d->TransitionType == ctkWorkflowInterstepTransition::TransitionToPreviousStep
           || d->TransitionType == ctkWorkflowInterstepTransition::TransitionToPreviousStartingStepAfterSuccessfulGoToFinishStep);

  // update the map from the step to the previous step if we are going forward
  if (d->TransitionType == ctkWorkflowInterstepTransition::TransitionToNextStep)
    {
    d->StepToPreviousStepMap.insert(d->DestinationStep, d->OriginStep);
    }

  // exit the destination step
  d->onExitInternal(d->OriginStep, d->DestinationStep, d->TransitionType);
}

// --------------------------------------------------------------------------
void ctkWorkflow::goToStepSucceeded()
{
  Q_D(ctkWorkflow);

  if (d->Verbose)
    {
    qDebug() << "goToStepSucceeded";
    }

  // after success, go back to the step at which we begin looking for
  // the finish step (will exit the current step and enter the starting step)
  // only if the property goBackToOriginStepUponSuccess is true.

  if (this->goBackToOriginStepUponSuccess())
    {
    d->createTransitionToPreviousStartingStep(d->StartingStep, d->CurrentStep);
    }

  d->GoToStep = 0;
  d->StartingStep->setStatusText("Attempt to go to the finish step succeeded");
  d->StartingStep = 0;

  if (this->goBackToOriginStepUponSuccess())
    {
    this->goFromGoToStepToStartingStep();
    }
}

// --------------------------------------------------------------------------
void ctkWorkflow::goFromGoToStepToStartingStep()
{
  Q_D(ctkWorkflow);
  if (d->Verbose)
    {
    qDebug() << "goFromGoToStepToStartingStep - Posting TransitionToPreviousStartingStep";
    }
  d->StateMachine->postEvent(new ctkWorkflowInterstepTransitionEvent(ctkWorkflowInterstepTransition::TransitionToPreviousStartingStepAfterSuccessfulGoToFinishStep));
}

// --------------------------------------------------------------------------
void ctkWorkflow::goToStepFailed()
{
  // Abort attempt to get to the finish step
  Q_D(ctkWorkflow);

  d->GoToStep = 0;
  d->StartingStep = 0;

  // We don't need to transition between steps - leave the user at the
  // point of failure, so that they can try to continue manually
  // Emit the signal that we have changed the current step, since it wasn't emitted in the process
  // of going to the 'goTo' step.
  emit this->currentStepChanged(d->CurrentStep);

  // if (failedOnBranch)
  //   {
  //   this->goToProcessingStateAfterValidationFailed();
  //   }

}

// --------------------------------------------------------------------------
int ctkWorkflow::backwardDistanceToStep(ctkWorkflowStep* fromStep,
                                        ctkWorkflowStep* origin) const
{
  if (!fromStep)
    {
    fromStep = this->currentStep();
    }
  if (!origin)
    {
    origin = this->initialStep();
    }

  if (!fromStep || !origin)
    {
    return -1;
    }

  QQueue< std::pair<ctkWorkflowStep*, int> > queue;
  queue.append(std::make_pair(fromStep, 0));
  while (! queue.isEmpty())
    {
    std::pair<ctkWorkflowStep*, int> p = queue.dequeue();
    ctkWorkflowStep* step = p.first;
    if (! step)
      {
      return -1;
      }

    if (step->id() == origin->id())
      {
      return p.second;
      }

    foreach(ctkWorkflowStep* previousStep, this->backwardSteps(step))
      {
      queue.append(std::make_pair(previousStep, p.second + 1));
      }
    }

  return -1;
}
