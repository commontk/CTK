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
 
#ifndef __ctkWorkflow_h
#define __ctkWorkflow_h

// Qt includes
#include <QObject>

// CTK includes
#include "ctkPimpl.h"
#include "ctkCoreExport.h"

class ctkWorkflowStep;
class ctkWorkflowPrivate;
class QAbstractState;

/// \brief ctkWorkflow is the basis for a workflow engine, i.e. a state
/// machine with enhancements to support ctkWorkflowStep.

class CTK_CORE_EXPORT ctkWorkflow : public QObject
{
  Q_OBJECT

public:

  typedef QObject Superclass;
  explicit ctkWorkflow(QObject* parent = 0);
  virtual ~ctkWorkflow();

  /// \brief Start the workflow.
  /// The workflow will always start in the initial step, even if it is stopped and restarted).
  /// \note Calls onEntry() for the initial step.
  virtual void start();

  /// \brief Returns whether the workflow is currently running
  bool isRunning()const;

  /// \brief Stops the workflow.
  /// \note Calls onExit() for the current step.
  virtual void stop();

  /// \brief Transition directionalities.
  ///
  /// The direction of transitions between an origin step and a destination step can be either:
  /// <ul>
  ///  <li>\a Bidirectional: A transition from the origin to the destination, and a transition from
  /// the destination to the origin</li>
  ///  <li>\a Forward: A transition from the origin to the destination only</li>
  ///  <li>\a Backward: A transition from the destination to the origin only</li>
  /// </ul>
  enum TransitionDirectionality
  {
    Bidirectional = 0,
    Forward,
    Backward
  };

  /// \brief Creates a transition between two steps, and adds the two steps to the workflow if they
  /// have not been previously added. (Cannot add two steps with the same id).
  ///
  /// The destination step should semantically be a next step, i.e. from a workflow perspective, the
  /// \a destination step is meant to appear after the \a origin step.
  ///
  /// To add a single step, \a destination can be set to 0.
  ///
  /// Returns true/false indicating whether the method was successful.
  virtual bool addTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                             const QString& branchId = QString(),
                             const ctkWorkflow::TransitionDirectionality directionality
                               = ctkWorkflow::Bidirectional);

  /// \brief Determine whether a transition has already been added
  /// <ul>
  ///  <li>If a branch id is not given or is empty: a transition exists if a transition has been
  /// previously added with the same origin, destination and directionality</li>
  /// <li>If a non-empty branch id is given: a transition exists if the transition has been previously
  /// added with the same origin, destination and directionality, OR if a transition has been
  /// previously added with the same origin and branch id (for forward transitions) or
  /// with the same destination and branch id (for backward transitions)</li>
  /// </ul>
  bool hasTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                     const QString& branchId = QString(),
                     const ctkWorkflow::TransitionDirectionality directionality = ctkWorkflow::Bidirectional);

  /// \brief Set/get the initial step.
  ///
  /// Convenience method to set the QStateMachine's initialState to a specific step's
  /// processing state.
  ///
  /// \note The initialStep() function *must* be called to set the state machine's initial state
  /// correctly
  ctkWorkflowStep* initialStep()const;
  virtual void setInitialStep(ctkWorkflowStep* step);

  /// Get the current step of the state machine
  ctkWorkflowStep* currentStep()const;

  /// Check to see if there is a step with a given id in the workflow.
  bool hasStep(const QString& id)const;

  /// Returns whether or not we can go forward: i.e. there exists a step that directly follows the
  /// given step.
  ///
  /// If no step is given, then the workflow's current step will be used.
  bool canGoForward(ctkWorkflowStep* step=0)const;

  /// Returns whether or not we can go backward: i.e. there exists a step that directly preceeds the
  /// given step.
  ///
  /// If no step is given, then the workflow's current step will be used.
  bool canGoBackward(ctkWorkflowStep* step=0)const;

  /// Returns whether or not we can go to the goal step from the origin step: i.e. there is a path
  /// in the workflow from the current step to the given step.
  ///
  /// If no step is designated as the 'origin', then the workflow's current step will be used
  /// Note: does not currently work in branching workflows if the origin and target steps are not on
  /// the same branch
  bool canGoToStep(const QString& targetId, ctkWorkflowStep* step=0)const;

  /// Get the steps that directly follow the given step.
  ///
  /// More specifically, the returned list of steps will be the destination steps for which
  /// addTransition() has been called with the given step as the origin step and directionality set
  /// to ctkWorkflow::Bidirectional or ctkWorkflow::Forward.
  ///
  /// If no step is given, then the workflow's current step will be used.
  QList<ctkWorkflowStep*> forwardSteps(ctkWorkflowStep* step=0)const;

  /// Get the steps that directly preceed the given step.
  ///
  /// More specifically, the returned list of steps will be the origin steps for which
  /// addTransition() has been called with the given step as the destination step and directionality
  /// set to ctkWorkflow::Bidirectional or ctkWorkflow::Backward.
  ///
  /// If no step is given, then the workflow's current step will be used.
  QList<ctkWorkflowStep*> backwardSteps(ctkWorkflowStep* step=0)const;

  /// Get the steps that are 'finish' steps (i.e. have no step following them)
  QList<ctkWorkflowStep*> finishSteps()const;

public slots:

  /// Use this to trigger evaluation of the processing state of the current step, and subsequent
  /// conditional transition to the next step.
  virtual void goForward(const QString& desiredBranchId = QString());

  /// Use this to trigger transition to the previous step (does not require validation)
  virtual void goBackward(const QString& desiredBranchId = QString());

  /// Go to the given step by iteratively calling goForward() until we reach it.
  virtual void goToStep(const QString& targetId);

  /// \brief Receives the result of a step's validate(const QString&) function.
  ///
  /// If the validation is successful, then this slot begins the transition to the next step.
  virtual void evaluateValidationResults(bool validationSucceeded, const QString& branchId);

  /// \brief Workflow processing executed after a step's onEntry function is run.
  virtual void processingAfterOnEntry();

  /// \brief Workflow processing executed after a step's onExit function is run.
  virtual void processingAfterOnExit();

protected:

  void goToNextStepAfterSuccessfulValidation(const QString& branchId);

  void goToProcessingStateAfterValidationFailed();

  /// \brief Processing that occurs after the attempt to go to a 'goTo' step succeeds
  virtual void goToStepSucceeded();

  /// \brief Processing that occurs after the attempt to go to a 'goTo' step fails
  virtual void goToStepFailed();
 
  /// \brief Goes to the step from which the attempt to go to the 'goTo' step was initiated
  void goFromGoToStepToStartingStep();

  /// \brief Performs required connections between the step and this workflow
  virtual void connectStep(ctkWorkflowStep* step);
 
protected slots:

  /// On an attempt to go to the next step, calls the current step's
  /// validate(const QString&) function to validate the processing step.
  void attemptToGoToNextStep();

  /// \brief Called when transitioning to the next step upon successful validation, or
  /// when transitioning to the previous step.
  /// Calls onExit() of the transition's origin step and then onEntry() of
  /// the transition's destination step.
  /// \note Must be sent by a ctkWorkflowTransition.
  void performTransitionBetweenSteps();

signals:
  /// Emitted when the current step has changed, after the step's onEntry() has completed.
  /// \note This signal is not emitted in the process of going to a goToStep
  void currentStepChanged(ctkWorkflowStep* currentStep);

protected:
  QScopedPointer<ctkWorkflowPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflow);
  Q_DISABLE_COPY(ctkWorkflow);
};

#endif

