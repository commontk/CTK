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

#ifndef __ctkWorkflowStep_h
#define __ctkWorkflowStep_h

// Qt includes
#include <QObject>

// CTK includes
#include "ctkPimpl.h"
#include "CTKCoreExport.h"
#include "ctkWorkflowTransitions.h"
class ctkWorkflow;

class ctkWorkflowStepPrivate;
class QState;

/// \brief ctkWorkflowStep is the basis for a workflow step.
///
/// A workflow step is a placeholder for various states and transitions that are used in a
/// typical workflow. Such steps can be added to instances of the ctkWorkflow class.

class CTK_CORE_EXPORT ctkWorkflowStep : public QObject
{
  Q_OBJECT

public:
  typedef QObject Superclass;
  explicit ctkWorkflowStep(ctkWorkflow* newWorkflow, const QString& newId = QString());
  virtual ~ctkWorkflowStep(){}

  /// Get the \a workflow associated with this step
  ctkWorkflow* workflow()const;

  /// Get id
  QString id()const;

  /// Set/get \a name
  QString name()const;
  void setName(const QString& newName);

  /// Set/get \a description
  QString description()const;
  void setDescription(const QString& newDescription);

  ///
  /// Set/get \a statusText
  QString statusText()const;
  void setStatusText(const QString& newText);

  ///
  /// Set/get whether a validationCommand has been provided in a separate QObject
  /// \note see method 2 described for validation()
  /// \sa validation()
  bool hasValidateCommand()const;
  void setHasValidateCommand(bool newHasValidateCommand);

  /// Set/get whether an onEntryCommand has been provided in a separate QObject
  /// \note See method2 in onEntry()
  /// \sa onEntry()
  bool hasOnEntryCommand()const;
  void setHasOnEntryCommand(bool newHasOnEntryCommand);

  ///
  /// Set/get whether an onExitCommand has been provided in a separate QObject
  /// \note  See method2 in onExit()
  /// \sa onExit()
  bool hasOnExitCommand()const;
  void setHasOnExitCommand(bool newHasOnExitCommand);

protected:

  /// Set step Id
  void setId(const QString& newStepId);

  /// \brief Get the step's processing state.
  ///
  /// This state is used to perform the processing associated with this step.
  QState* processingState()const;

  /// \brief Get the step's validation state.
  ///
  /// The validate(const QString&) method is the key component to define for this state to work as expected.
  /// This state is used to validate the processing pertaining to this
  /// step, then branch to the next step's processingState state on
  /// success, or back to the current step's processingState state on
  /// error.
  ///
  /// When the validation state emits its entered() signal, the corresponding workflow's
  /// attemptToGoToNextStep() slot is called.
  ///
  /// This function calls the step's validate(const QString&) method to evaluate
  /// whether one can transition to the next step.
  QState* validationState()const;

  /// \brief Get the step's validation transition.
  ///
  /// The validation transition is used to bring the state machine
  /// from the step's processingState to its validationState.
  /// More specifically:
  /// <ul>
  ///  <li>its origin state is the processingState state</li>
  ///  <li>its destination state is the validationState state</li>
  /// </ul>
  ///
  /// The transition is of type ctkWorkflowTransition with the value
  /// ctkWorkflowTransitionType::ValidationTransition.
  ctkWorkflowIntrastepTransition* validationTransition()const;

  /// \brief Get the step's validation failed transition.
  ///
  /// The validationFailed transition is used to bring the state
  /// machine from the step's validationState state back to its
  /// processingState, when validation of the processing step fails
  /// (i.e. validate(const QString&) returns false).
  ///
  /// More specifically:
  /// <ul>
  ///  <li>its origin state is the validatationState state</li>
  ///  <li>its destination state is the processingState state</li>
  /// </ul>
  ///
  /// The transition is of type ctkWorkflowTransition with the value
  /// ctkWorkflowTransitionType::ValidationFailedTransition.
  ctkWorkflowIntrastepTransition* validationFailedTransition()const;

  ///\brief  Reimplement this function for step-specific processing when entering a step.
  ///
  /// To define a custom step, developers can either reimplement the onEntry() method in a subclass
  /// of ctkWorkflowStep, or create a ctkWorkflowStep instance and set the onEntryCommand to point
  /// to a callback of their choice.
  ///
  /// Each step should be self-contained, \a comingFrom and \a transitionType may
  /// be used only to decide on how processing should be done for the current step.
  ///
  /// \param comingFrom gives the step that the state machine was in before
  /// transitioning to this step.
  ///
  /// \param transitionType gives the type of the transition used to get to this step.
  virtual void onEntry(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

  /// \brief Reimplement this function for step-specific processing when exiting a step.
  ///
  /// To define a custom step, developers can either reimplement the onExit() method
  /// in a subclass of ctkWorkflowStep, or create a ctkWorkflowStep instance and set the
  /// onExitCommand to point to a callback of their choice.
  ///
  /// Each step should be self-contained, \a goingTo and \a transitionType may be used only to
  /// decide on how processing should be done for the current step.
  ///
  /// \param goingTo gives the step that the state machine will go to after
  /// transitioning from this step.
  /// \param transitionType gives the type of the transition used to get to this step.
  virtual void onExit(const ctkWorkflowStep* goingTo,
                      const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

  /// \brief Validates the computation performed in this step's processing state.
  ///
  /// The validate(const QString&) function is called from the workflow's attemptToGoToNextStep() slot,
  /// which is invoked by the validatationState state's entered() signal.  It must emit a
  /// signal with a single integer parameter.  This signal is connected to the workflow's
  /// evaluateValidationResults(int) slot, which then performs conditional transition to
  /// the next state.
  ///
  /// When creating a custom step, developers can create a validate(const QString&) method is one of two ways:
  /// 1) Reimplement the validate(const QString&) method in a subclass of
  /// ctkWorkflowStep, following these instructions:
  /// <ul>
  ///   <li>*do* call the Superclass's validate(const QString&) function, and fail if it returns 0</li>
  ///   <li>emit the signal ctkWorkflowStep::validateComplete(int) (1 on successful validation,
  /// 0 on failure)</li>
  ///   <li>return an int (1 on successful validation, 0 otherwise)</li>
  /// </ul>
  //
  /// OR:
  ///
  /// 2) Create an instance of a ctkWorkflowStep then:
  /// <ul>
  ///  <li>Call setHasValidateCommand(1) on the step *before* adding the step to the workflow</li>
  ///  <li>Create a slot foo() associated with any QObject*, following these instructions:</li>
  ///  <ul>
  ///    <li>There is *NO* need to call the Superclass's validate(const QString&) function
  /// (error checking is performed before the invokeValidateCommand(const QString&)
  /// signal is emitted)</li>
  ///     <li>Emit a signal bar(int) (1 on successful validation, 0 on
  /// failure)</li>
  ///     <li>Return: 1 on successful validation, 0 otherwise</li>
  ///     <li>Set the following two connections:</li>
  ///     <ul>
  ///       <li>QObject::connect(step, SIGNAL(invokeValidateCommand(const QString&)), object,
  /// SLOT(foo()))</li>
  ///       <li>QObject::connect(object, SIGNAl(bar(int)), workflow,
  /// SLOT(evaluateValidationResults(int)))</li>
  ///      </ul>
  ///    </ul>
  ///  </ul>
  ///
  /// \note ctkWorkflowStep does not function within a workflow as implemented here,
  /// one of the above two methods must be followed
  virtual void validate(const QString& desiredBranchId = QString());

signals:
  /// \brief Signal indicating that validation of this step's processing should be performed.
  ///
  /// \note Should be used if a validationCommand has been provided
  /// See method 2 described in validation()
  ///
  /// \sa validation()
  void invokeValidateCommand(const QString& desiredBranchId = QString())const;

  /// \brief Signal indicating that validation of this step's processing has completed.
  ///
  /// \note Should be used if a validationCommand has not been provided
  /// See method 1 described in validation()
  ///
  /// \param validationSuceeded 1 on successful validation, 0 on failure
  void validationComplete(bool validationSuceeded, const QString& branchId = QString())const;

  void invokeOnEntryCommand(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const;

  void onEntryComplete()const;

  void invokeOnExitCommand(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const;

  void onExitComplete()const;

private:

  CTK_DECLARE_PRIVATE(ctkWorkflowStep);
  friend class ctkWorkflow;        // For access to processingState, validationState, setWorkflow, validate
  friend class ctkWorkflowPrivate; // For access to invokeOn{Entry,Exit}Command, on{Entry,Exit}
};

#endif
