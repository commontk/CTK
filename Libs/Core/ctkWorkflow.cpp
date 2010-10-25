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
#include <QStateMachine>
#include <QState>

// CTK includes
#include "ctkWorkflow.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowStep_p.h"
#include "ctkWorkflowTransitions.h"
#include "ctkLogger.h"

// STD includes
#include <iostream>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.core.ctkWorkflow");
//--------------------------------------------------------------------------

namespace
{
//-----------------------------------------------------------------------------
struct forwardAndBackwardSteps
{
  QList<ctkWorkflowStep*> forwardSteps()
  {
    return this->ForwardSteps;
  }

  QList<ctkWorkflowStep*> backwardSteps()
  {
    return this->BackwardSteps;
  }

  QList<QString> forwardBranchIds()
  {
    return this->ForwardBranchIds;
  }

  QList<QString> backwardBranchIds()
  {
    return this->BackwardBranchIds;
  }

  void appendForwardStep(ctkWorkflowStep* step, QString id)
  {
    this->ForwardSteps.append(step);
    this->ForwardBranchIds.append(id);
  }

  void appendBackwardStep(ctkWorkflowStep* step, QString id)
  {
    this->BackwardSteps.append(step);
    this->BackwardBranchIds.append(id);
  }

  QString firstForwardBranchId()
  {
    if (this->ForwardBranchIds.isEmpty())
      {
      return QString();
      }
    else
      {
    return this->ForwardBranchIds.first();
      }
  }

  ctkWorkflowStep* forwardStep(QString branchId)
  {
    int index = this->ForwardBranchIds.indexOf(branchId);
    if (index != -1)
      {
      return ForwardSteps.at(index);
      }
    else
      {
      return 0;
      }
  }

  QString backwardBranchId(ctkWorkflowStep* step)
  {
    int index = this->BackwardSteps.indexOf(step);
    if (index != -1)
      {
      return BackwardBranchIds.at(index);
      }
    else
      {
      return QString();
      }
  }

  QString forwardBranchId(ctkWorkflowStep* step)
  {
    int index = this->ForwardSteps.indexOf(step);
    if (index != -1)
      {
      return ForwardBranchIds.at(index);
      }
    else
      {
      return QString();
      }
  }

private:
  QList<ctkWorkflowStep*> ForwardSteps;
  QList<ctkWorkflowStep*> BackwardSteps;

  QList<QString> ForwardBranchIds;
  QList<QString> BackwardBranchIds;

};
}

//-----------------------------------------------------------------------------
class ctkWorkflowPrivate
{
  Q_DECLARE_PUBLIC(ctkWorkflow);
protected:
  ctkWorkflow* const q_ptr;
public:
  ctkWorkflowPrivate(ctkWorkflow& object);
  ~ctkWorkflowPrivate();

  /// \brief Add a step to the workflow
  ///
  /// \note The step's components will be automatically be added to the state machine
  /// (i.e. the processingState state, validationState state, validationTransition transition
  /// and validationFailedtransition transition.
  ///
  /// \return True or False indicating whether the method was successful.
  void addStep(ctkWorkflowStep* step);

  /// \brief Returns whether a transition has been previously added with the same origin,
  /// destination and directionality
  bool hasDuplicateTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                              const ctkWorkflow::TransitionDirectionality directionality);

  /// \brief Returns whether a transition has been previously added with the same origin and branch
  /// id (for forward transitions) or with the same destination and branch id (for backward transitions
  bool hasTransitionWithSameBranchId(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                                     const QString& branchId,
                                     const ctkWorkflow::TransitionDirectionality directionality);

  /// \brief Creates a transition from the origin to the destinatio.
  ///
  /// More specifically, the transition is from the \a origin's validation state to the \a
  /// destination's processing state, and is of type ctkWorkflowTransition::TransitionToNextStep
  ///
  /// The destination step should semantically be a next step, i.e. from a workflow perspective, the
  /// \a destination step is meant to appear after the \a origin step.
  ///
  /// Returns true/false indicating whether the method was successful.
  void createTransitionToNextStep(ctkWorkflowStep* origin,
                                  ctkWorkflowStep* destination,
                                  const QString& branchId = "");

  /// \brief Creates a transition from the destination to the origin
  ///
  /// More specifically, the transition is from the \a destination's processing state to the \a
  /// origin's processing state, and is of type ctkWorkflowTransition::TransitionToPreviousStep
  ///
  /// The destination step should semantically be a next step, i.e. from a workflow perspective, the
  /// \a destination step is meant to appear after the \a origin step.
  ///
  /// Returns true/false indicating whether the method was successful.
  void createTransitionToPreviousStep(ctkWorkflowStep* origin,
                                      ctkWorkflowStep* destination,
                                      const QString& branchId = "");

  /// \brief Creates a transition from the goTo step to the step from which the attempt to go to the
  /// goTo step was initiated.
  ///
  /// More specifically, the transition is from the \a goTo step's processing state to the \a
  /// starting step's processing state, and is of type ctkWorkflowTransition::TransitionToPreviousStartingStep
  ///
  /// Returns true/false indicating whether the method was successful.
  void createTransitionToPreviousStartingStep(ctkWorkflowStep* startingStep,
                                              ctkWorkflowStep* currentStep);
  ///
  void validateInternal(ctkWorkflowStep* step);

  /// \brief Performs computation required when entering this step.
  ///
  /// Does some sanity checks and then either calls onEntry() or emits the invokeOnEntryCommand(),
  /// depending on whether the user indicates that there is an onEntryCommand.
  void onEntryInternal(ctkWorkflowStep* step, ctkWorkflowStep* comingFrom,
                       const ctkWorkflowInterstepTransition::InterstepTransitionType& transitionType);

  /// \brief Performs computation required when exiting this step.
  ///
  /// Does some sanity checks and then either calls onExit() or emits the invokeOnExitCommand(),
  /// depending on whether the user indicates that there is an onExitCommand.
  void onExitInternal(ctkWorkflowStep* step, ctkWorkflowStep* goingTo,
                      const ctkWorkflowInterstepTransition::InterstepTransitionType& transitionType);

  /// Get the step in the workflow with a given id.
  ctkWorkflowStep* stepFromId(const QString& id)const;

  /// Get the step that a state belongs to (if any)
  ctkWorkflowStep* stepFromState(const QAbstractState* state);

  /// Get the number of forward steps from the given step
  int numberOfForwardSteps(ctkWorkflowStep* step);

  /// Get the number of backward steps from the given step
  int numberOfBackwardSteps(ctkWorkflowStep* step);

  /// Get the ids of the steps that directly follow the given step.
  QList<QString> forwardBranchIds(ctkWorkflowStep* step)const;

  /// Get the ids of the steps that directly preceed the given step.
  QList<QString> backwardBranchIds(ctkWorkflowStep* step)const;

  /// Determines whether there exists a path from the origin step (the current step by default) to
  /// the step with the given goalId
  bool pathExists(const QString& goalId, ctkWorkflowStep* origin = 0)const;

  /// Determines whether there exists a path from the current step's next step (as given by the
  /// branchId) to the step with the given goalId
  bool pathExistsFromNextStep(const QString& goalId, const QString& branchId)const;

  QStateMachine* StateMachine;

  // Maintain a list of pointers to the steps in the workflow,
  // along with their forward and backward transitions
  QMap<ctkWorkflowStep*, forwardAndBackwardSteps*>         StepToForwardAndBackwardStepMap;

  // ... and its associated convenient typedef
  typedef QMap<ctkWorkflowStep*, forwardAndBackwardSteps*> StepToForwardAndBackwardStepMapType;
  typedef QList<ctkWorkflowStep*>                          StepListType;

  // Maintain a map of <state, step> key/value pairs, to find the step
  // that a given state belongs to
  typedef QMap<const QAbstractState*, ctkWorkflowStep*>           StateToStepMapType;
  typedef QMap<const QAbstractState*, ctkWorkflowStep*>::iterator StateToStepMapIterator;
  StateToStepMapType                                              StateToStepMap;

  ctkWorkflowStep*                         InitialStep;
  ctkWorkflowStep*                         CurrentStep;
  QMap<ctkWorkflowStep*, ctkWorkflowStep*> StepToPreviousStepMap;

  // Used when performing a transition
  ctkWorkflowStep*                                        OriginStep;
  ctkWorkflowStep*                                        DestinationStep;
  ctkWorkflowInterstepTransition::InterstepTransitionType TransitionType;

  QString          DesiredBranchId; // Desired branchId specified when invoking goForward

  ctkWorkflowStep* GoToStep;   // Desired step when attempting to go to a finish step

  ctkWorkflowStep* StartingStep; // Current step when we began the attempt to go to the desired finish step

  // Temporary transition after successfully going to finish step, to get us back to the starting step
  ctkWorkflowInterstepTransition* TransitionToPreviousStartingStep;

  QString ARTIFICIAL_BRANCH_ID_PREFIX;

};

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

  this->ARTIFICIAL_BRANCH_ID_PREFIX = "ctkWorkflowArtificialBranchId_";
}

// --------------------------------------------------------------------------
ctkWorkflowPrivate::~ctkWorkflowPrivate()
{ 
}

// --------------------------------------------------------------------------
void ctkWorkflowPrivate::addStep(ctkWorkflowStep* step)
{
  Q_Q(ctkWorkflow);

  Q_ASSERT(step);
  Q_ASSERT(!q->hasStep(step->id()));
  Q_ASSERT(!this->StateMachine->isRunning());

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
  q->connectStep(step);
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

  logger.debug(QString("validateInternal - validating input from %1").arg(step->name()));

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

  logger.debug(QString("onEntryInternal - entering input from %1").arg(step->name()));

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
void ctkWorkflowPrivate::onExitInternal(
    ctkWorkflowStep* step,
    ctkWorkflowStep* goingTo,
    const ctkWorkflowInterstepTransition::InterstepTransitionType& transitionType)
{
  Q_ASSERT(step);

  logger.debug(QString("onExitInternal - exiting %1").arg(step->name()));

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
}

// --------------------------------------------------------------------------
bool ctkWorkflow::addTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                                const QString& branchId,
                                const ctkWorkflow::TransitionDirectionality directionality)
{
  Q_D(ctkWorkflow);

  if (d->StateMachine->isRunning())
    {
    logger.warn("addTransition - Cannot add a transition while the workflow is started !");
    return false;
    }

  // cannot currently create a transition between two steps of the same id, which is equivalent to
  // adding a transition from a step to itself
  if (origin && destination && (QString::compare(origin->id(), destination->id(), Qt::CaseInsensitive) == 0))
    {
    logger.error("addTransition - Workflow does not currently support a transition"
                 " from a step to itself.  Use GoToStep instead !");
    return false;
    }

  // add the origin step if it doesn't exist in the workflow yet
  if (origin && !this->hasStep(origin->id()))
    {
    d->addStep(origin);
    }

  // add the destination step if it doesn't exist in the workflow yet
  if (destination && !this->hasStep(destination->id()))
    {
    d->addStep(destination);
    }

  if (origin && destination)
    {
    // ensure we haven't already added a transition with the same origin, destination and directionality
    if (this->hasTransition(origin, destination, branchId, directionality))
      {
      logger.warn("addTransition - Cannot create a transition that matches a "
                  "previously created transtiion");
      return false;
      }

    // create the forward transition
    if (directionality == ctkWorkflow::Forward
        || directionality == ctkWorkflow::Bidirectional)
      {
      d->createTransitionToNextStep(origin, destination, branchId);
      }

    // create the backward transition
    if (directionality == ctkWorkflow::Backward
        || directionality == ctkWorkflow::Bidirectional)
      {
      d->createTransitionToPreviousStep(origin, destination, branchId);
      }
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
void ctkWorkflow::connectStep(ctkWorkflowStep* step)
{
  Q_ASSERT(step);

  if (!step->hasValidateCommand())
    {
    QObject::connect(step->ctkWorkflowStepQObject(), SIGNAL(validationComplete(bool, const QString&)), this, SLOT(evaluateValidationResults(bool, const QString&)));
    }

  if (!step->hasOnEntryCommand())
    {
    QObject::connect(step->ctkWorkflowStepQObject(), SIGNAL(onEntryComplete()), this, SLOT(processingAfterOnEntry()));
    }

  if (!step->hasOnExitCommand())
    {
    QObject::connect(step->ctkWorkflowStepQObject(), SIGNAL(onExitComplete()), this, SLOT(processingAfterOnExit()));
    }
}

// --------------------------------------------------------------------------
QList<ctkWorkflowStep*> ctkWorkflow::forwardSteps(ctkWorkflowStep* step)const
{
  Q_D(const ctkWorkflow);
  // use the given step if provided, otherwise use the workflow's current step
  if (step)
    {
    return d->StepToForwardAndBackwardStepMap.value(step)->forwardSteps();
    }
  else if (d->CurrentStep)
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
  if (step)
    {
    return d->StepToForwardAndBackwardStepMap.value(step)->backwardSteps();
    }
  else if (d->CurrentStep)
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
CTK_GET_CPP(ctkWorkflow, ctkWorkflowStep*, initialStep, InitialStep);
CTK_SET_CPP(ctkWorkflow, ctkWorkflowStep*, setInitialStep, InitialStep);
CTK_GET_CPP(ctkWorkflow, ctkWorkflowStep*, currentStep, CurrentStep);

// --------------------------------------------------------------------------
void ctkWorkflow::start()
{
  Q_D(ctkWorkflow);
  if (!d->InitialStep)
    {
    logger.warn("start - Cannot start workflow without an initial step");
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
    logger.warn("goForward - The workflow is not running !");
    return;
    }

  // if we're just going to the next step and not to a 'goTo' step, then check to make sure that
  // there exists a step following the current step
  if (!d->GoToStep)
    {
    if (!this->canGoForward())
      {
      logger.warn("goForward - Attempt to goForward from a finish step !");
      return;
      }
    }

  d->DesiredBranchId = desiredBranchId;

  logger.info("goForward - posting ValidationTransition");
  d->StateMachine->postEvent(
      new ctkWorkflowIntrastepTransitionEvent(ctkWorkflowIntrastepTransition::ValidationTransition));
}

// --------------------------------------------------------------------------
void ctkWorkflow::goBackward(const QString& desiredBranchId)
{
  Q_D(ctkWorkflow);

  if (!this->isRunning())
    {
    logger.warn("goBackward - The workflow is not running !");
    return;
    }

  if (!this->canGoBackward())
    {
    logger.warn("goBackward - Attempt to goBackward from first step !");
    return;
    }

  ctkWorkflowStep* previousStep = d->StepToPreviousStepMap[d->CurrentStep];
  Q_ASSERT(previousStep);
  QString branchId = d->StepToForwardAndBackwardStepMap.value(d->CurrentStep)->backwardBranchId(previousStep);
  Q_ASSERT(!branchId.isEmpty());

  d->DesiredBranchId = desiredBranchId;
  
  logger.info("goBackward - posting TransitionToPreviousStep");
  d->StateMachine->postEvent(
                             new ctkWorkflowInterstepTransitionEvent(ctkWorkflowInterstepTransition::TransitionToPreviousStep, branchId));
}

// --------------------------------------------------------------------------
void ctkWorkflow::goToStep(const QString& targetId)
{
  Q_D(ctkWorkflow);

  if (!this->isRunning())
    {
    logger.warn("goToStep - The workflow is not running !");
    return;
    }

  // TODO currently returns true only if the workflow is running - need logic here
  if (!this->canGoToStep(targetId))
    {
    logger.warn(QString("goToStep - Cannot goToStep %1 ").arg(targetId));
    return;
    }

#ifndef QT_NO_DEBUG
  ctkWorkflowStep* step = d->stepFromId(targetId);
  Q_ASSERT(step);
#endif

  logger.info(QString("goToStep - Attempting to go to finish step %1").arg(targetId));

  // if (step == d->CurrentStep)
  //   {
  //   qDebug() << "we are already in the desired finish step";
  //   return;
  //   }

  d->GoToStep = d->stepFromId(targetId);
  d->StartingStep = d->CurrentStep;
  this->goForward();
}

// --------------------------------------------------------------------------
void ctkWorkflow::attemptToGoToNextStep()
{
  logger.info("attemptToGoToNextStep - Attempting to go to the next step ");
 
  Q_D(ctkWorkflow);
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
  logger.debug("goToNextStepAfterSuccessfulValidation - Calidation succeeded");
  logger.info("goToNextStepAfterSuccessfulValidation - Posting TransitionToNextStep");

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
      logger.warn("goToNextStepAfterSuccessfulValidation - ctkWorkflowStep::ValidatComplete() "
                  "did not provide branchId at a branch in the workflow - will follow first "
                  "transition that was created");
      }
    }
  // validationComplete() gives us a branchId
  else
    {
    if (numberOfForwardSteps == 1 && firstForwardBranchId.contains(d->ARTIFICIAL_BRANCH_ID_PREFIX))
      {
      transitionBranchId = firstForwardBranchId;
      logger.warn("goToNextStepAfterSuccessfulValidation -  ctkWorkflowStep::ValidationComplete()"
                  " returns a branchId, but was overridden by the workflow");
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
  logger.debug("goToNextStepAfterSuccessfulValidation - Validation failed");
  Q_D(ctkWorkflow);
  // Validation failed in the process of attempting to go to the finish step
  if (d->GoToStep)
    {
    this->goToStepFailed();
    }

  logger.info("goToNextStepAfterSuccessfulValidation - Posting ValidationFailedTransition");
  d->StateMachine->postEvent(new ctkWorkflowIntrastepTransitionEvent(ctkWorkflowIntrastepTransition::ValidationFailedTransition));
}

// --------------------------------------------------------------------------
void ctkWorkflow::performTransitionBetweenSteps()
{
  Q_D(ctkWorkflow);
  logger.debug("performTransitionBetweenSteps - Performing transition between steps");

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
void ctkWorkflow::processingAfterOnExit()
{
  Q_D(ctkWorkflow);

  // enter the destination step if we have one
  if (d->DestinationStep)
    {
    d->onEntryInternal(d->DestinationStep, d->OriginStep, d->TransitionType);
    }
  // reset the pointers used internally for performing a transition if we're done
  else
    {
    d->OriginStep = 0;
    d->DestinationStep = 0;
    // we've exited the CurrentStep and haven't gone into another step, so we no longer have a
    // currentStep.
    d->CurrentStep = 0;
    }
}

// --------------------------------------------------------------------------
void ctkWorkflow::processingAfterOnEntry()
{
  logger.debug("processingAfterOnEntry");

  Q_D(ctkWorkflow);

  if (!d->DestinationStep)
    {
    logger.error("processingAfterOnEntry - Called processingAfterOnEntry without "
                 "having set a destination step");
    return;
    }

  // Update the currentStep and previous step
  d->CurrentStep = d->DestinationStep;

  // Reset the pointers used internally for performing a transition
  d->OriginStep = 0;
  d->DestinationStep = 0;

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
  if (d->GoToStep)
    {
    if (d->CurrentStep == d->GoToStep)
      {
      this->goToStepSucceeded();
      }
    // if we're not finished, continue transitioning to the next step
    else
      {
      this->goForward();
      }
    }
  else
    {
    emit this->currentStepChanged(d->CurrentStep);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflow::goToStepSucceeded()
{
  Q_D(ctkWorkflow);

  logger.debug("goToStepSucceeded");

  // after success, go back to the step at which we begin looking for
  // the finish step (will exit the current step and enter the starting step)

  d->createTransitionToPreviousStartingStep(d->StartingStep, d->CurrentStep);

  d->GoToStep = 0;
  d->StartingStep->setStatusText("Attempt to go to the finish step succeeded");
  d->StartingStep = 0;

  this->goFromGoToStepToStartingStep();
}

// --------------------------------------------------------------------------
void ctkWorkflow::goFromGoToStepToStartingStep()
{
  Q_D(ctkWorkflow);
  logger.info("goFromGoToStepToStartingStep - Posting TransitionToPreviousStartingStep");
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
