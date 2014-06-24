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

#ifndef __ctkWorkflow_p_h
#define __ctkWorkflow_p_h

// Qt includes
#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

// CTK includes
#include "ctkWorkflow.h"
#include "ctkWorkflowTransitions.h"

class QStateMachine;
class ctkWorkflowStep;
//class ctkWorkflow;
//enum ctkWorkflow::TransitionDirectionality;

//-----------------------------------------------------------------------------
/**
 * \ingroup Core
 */
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

// --------------------------------------------------------------------------
/**
 * \ingroup Core
 */
class ctkWorkflowPrivate : public QObject
{
  Q_OBJECT
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
  bool addStep(ctkWorkflowStep* step);

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

public Q_SLOTS:

  /// \brief Workflow processing executed after a step's onEntry function is run.
  void processingAfterOnEntry();

  /// \brief Workflow processing executed after a step's onExit function is run.
  void processingAfterOnExit();

public:

  QStateMachine* StateMachine;

  // Maintain a list of pointers to the steps in the workflow,
  // along with their forward and backward transitions
  QMap<ctkWorkflowStep*, forwardAndBackwardSteps*>         StepToForwardAndBackwardStepMap;

  // ... and its associated convenient typedef
  typedef QMap<ctkWorkflowStep*, forwardAndBackwardSteps*> StepToForwardAndBackwardStepMapType;
  typedef QList<ctkWorkflowStep*>                          StepListType;

  // Register a list of pointers to the steps in the worflow for cleaning purpose
  StepListType RegisteredSteps;

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

  bool GoBackToOriginStepUponSuccess;

  QString ARTIFICIAL_BRANCH_ID_PREFIX;

  bool Verbose;
};

#endif
