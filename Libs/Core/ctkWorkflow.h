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
 
#ifndef __ctkWorkflow_h
#define __ctkWorkflow_h

// Qt includes
#include <QObject>

// CTK includes
#include "ctkPimpl.h"
#include "CTKCoreExport.h"

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
  /// Tthis method will:
  /// <ul>
  ///  <li>Call addStep()</li> to add the origin and destination steps, if they have not been
  /// previously added to the workflow</li>
  ///  <li>If \a directionality is ctkWorkflow::Bidirectional or ctkWorkflow::Forward, creates a
  /// transition from the origin to the destination (more specifically, the transition is from the
  /// \a origin's validation state to the \a destination's processing state, and is of type ctkWorkflowTransition::TransitionToNextStep</li>
  ///  <li>If \a directionality is ctkWorkflow::Bidirectional or ctkWorkflow::Backward, creates a
  /// transition from the destination to the origin (more specifically, the transition is from the
  /// \a destination's processing state to the \a origin's processing state, and is of type
  /// ctkWorkflowTransition::TransitionToPreviousStep</li>
  ///
  /// The default value of directionality is ctkWorkflow::Bidirectional.
  ///
  /// To add a single step, \a destination can be set to 0.
  ///
  /// Returns true/false indicating whether the method was successful.
  virtual bool addTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
                             const QString& branchId = QString(),
                             const ctkWorkflow::TransitionDirectionality directionality
                               = ctkWorkflow::Bidirectional);

  // /// \Determine whether a transition has already been added
  // bool hasTransition(ctkWorkflowStep* origin, ctkWorkflowStep* destination,
  //                    const ctkWorkflow::TransitionDirectionality directionality/*,
  //                    const QString& branchId = QString()*/);

  /// \Determine whether a transition has already been added
  /// If a branch id is not given or is empty: a transition exists if the transition has been
  /// previously added with the same origin, destination and directionality
  /// if a non-empty branch id is given: a transition exists if the transition has been previously
  /// added with the same origin, destination and directionality, OR if a transition has been
  /// previously added wtih the same origin and branch id (for forward transitions) or
  /// with the same destination and branch id (for backward transitions)
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

  /// Returns whether or not we can go forward: i.e. the workflow is running and there exists a step
  /// that directly follows the given step.
  ///
  /// If no step is given, then the workflow's current step will be used.
  bool canGoForward(ctkWorkflowStep* step=0)const;

  /// Returns whether or not we can go backward: i.e. the workflow is running and there exists a
  /// step that directly preceeds the given step.
  ///
  /// If no step is given, then the workflow's current step will be used.
  bool canGoBackward(ctkWorkflowStep* step=0)const;

  /// Returns whether or not we can go to the goal step from the origin step: i.e. there is a path
  /// in the workflow from the current step to the given step.
  ///
  /// If no step is designated as the 'origin', then the workflow's current step will be used
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
  ///
  /// This slot should be connected to each ctkWorkflowStep's validationComplete() signal.
  virtual void evaluateValidationResults(bool validationSucceeded, const QString& branchId);

  /// \brief Workflow processing executed after a step's onEntry function is run.
  ///
  /// This slot should be connected to each ctkWorkflowStep's onEntryComplete() signal.
  virtual void processingAfterOnEntry();

  /// \brief Workflow processing executed after a step's onExit function is run.
  ///
  /// This slot should be connected to each ctkWorkflowStep's onExitComplete() signal.
  virtual void processingAfterOnExit();

protected:

  /// \brief Triggers the start of a ctkWorkflowTransition of type
  /// ctkWorkflowTransitionType::TransitionToNextStep()
  void goToNextStepAfterSuccessfulValidation(const QString& branchId);

  /// \brief Triggers the start of a ctkWorkflowTransition of type
  /// ctkWorkflowTransitionType::ValidationFailedTransition
  void goToProcessingStateAfterValidationFailed();

  /// \brief Processing that occurs after the attempt to go to a 'goTo' step succeeds
  virtual void goToStepSucceeded();

  /// \brief Processing that occurs after the attempt to go to a 'goTo' step fails
  virtual void goToStepFailed();
 
  /// \brief Goes to the step from which the attempt to go to the 'goTo' step was initiated
  void goFromGoToStepToStartingStep();

  /// \brief Performs required connections between the step and this
  /// workflow, if the user is deriving a custom step as a subclasses of ctkWorkflowStep
  virtual void connectStep(ctkWorkflowStep* step);
 
protected slots:

  /// On an attempt to go to the next step, calls the current step's
  /// validate(const QString&) function to validate the processing step.  The
  /// validate(const QString&) function emits a signal that is connected to the
  /// workflow's evaluateValidationResults slot.  If the validation is
  /// successful, then the slot triggers the start of a
  /// ctkWorkflowTransition of type
  /// ctkWorkflowTransitionType::TransitionToNextStep, otherwise it
  /// triggers the start of a ctkWorkflowTransition of type
  /// ctkWorkflowTransitionType::ValidationFailedTransition.
  void attemptToGoToNextStep();

  /// \brief May be called when transitioning to the next step upon successful validation, or
  /// when transitioning to the previous step.
  /// Calls onExit() of the transition's origin step and then onEntry() of
  /// the transition's destination step.
  /// \note Must be sent by a ctkWorkflowTransition.
  void performTransitionBetweenSteps();

signals:
  /// Emitted when the current step has changed, after the step's onEntry() has completed.
  /// \note This signal is not emitted in the process of going to a goToStep
  void currentStepChanged(ctkWorkflowStep* currentStep);

private:
  CTK_DECLARE_PRIVATE(ctkWorkflow);
};

#endif

