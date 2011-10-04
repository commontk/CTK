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

#ifndef __ctkWorkflowStep_h
#define __ctkWorkflowStep_h

// Qt includes
class QObject;
class QState;

// CTK includes
#include "ctkPimpl.h"
#include "ctkWorkflow_p.h"
#include "ctkWorkflowTransitions.h"

#include "ctkCoreExport.h"

class ctkWorkflow;

class ctkWorkflowStepPrivate;

/// \ingroup Core
/// \brief ctkWorkflowStep is the basis for a workflow step.
///
/// A workflow step is a placeholder for various states and transitions that are used in a
/// typical workflow. Such steps can be added to instances of the ctkWorkflow class.

class CTK_CORE_EXPORT ctkWorkflowStep
{

public:
  explicit ctkWorkflowStep();
  explicit ctkWorkflowStep(const QString& newId);
  virtual ~ctkWorkflowStep();

  /// Get the \a workflow associated with this step
  ctkWorkflow* workflow()const;

  /// Get id
  QString id()const;

  /// Set step Id
  /// \note Setting the Id after the step had been added to a workflow is a no-op
  void setId(const QString& newStepId);

  /// Set/get \a name
  QString name()const;
  void setName(const QString& newName);

  /// Set/get \a description
  QString description()const;
  void setDescription(const QString& newDescription);

  /// Get \a statusText
  QString statusText()const;

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

  /// Returns true if the object is a widget; otherwise returns false
  bool isWidgetType()const;

  /// Get QObject associated with this step, to connect signals/slots
  QObject* ctkWorkflowStepQObject();

protected:

  explicit ctkWorkflowStep(ctkWorkflowStepPrivate * pimpl, const QString& newId);

  /// Set workflow
  void setWorkflow(ctkWorkflow* newWorkflow);

  void setStatusText(const QString& newText);

  /// \brief Get the step's processing state.
  ///
  /// This state is used to perform the processing associated with this step.
  QState* processingState()const;

  /// \brief Get the step's validation state.
  ///
  /// This state is used to validate the processing associated with this step.
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

  /// \brief  Reimplement this function for step-specific processing when entering a step.
  ///
  /// To define a custom step, developers can either reimplement the onEntry() method in a subclass
  /// of ctkWorkflowStep, or create a ctkWorkflowStep instance and use signals and slots, as
  /// similarly as described for validate().
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
  /// in a subclass of ctkWorkflowStep, or create a ctkWorkflowStep instance and use signals and
  /// slots, similarly as described for validate().
  ///
  /// Each step should be self-contained, \a goingTo and \a transitionType may be used only to
  /// decide on how processing should be done for the current step.
  ///
  /// \param goingTo gives the step that the state machine will go to after
  /// transitioning from this step.
  ///
  /// \param transitionType gives the type of the transition used to get to this step.
  virtual void onExit(const ctkWorkflowStep* goingTo,
                      const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

  /// \brief Validates the computation performed in this step's processing state.
  ///
  /// When creating a custom step, developers can create a validate(const QString&) method is one of two ways:
  /// 1) Reimplement the validate(const QString&) method in a subclass of
  /// ctkWorkflowStep, following these instructions:
  /// <ul>
  ///   <li>invoke the superclass method ctkWorkflowStep::validateComplete(bool, const QString&)
  /// (true on successful validation, false on failure; the QString is the desired branchId to use
  /// with branching workflows)</li>
  /// </ul>
  //
  /// OR:
  ///
  /// 2) Create an instance of a ctkWorkflowStep then:
  /// <ul>
  ///  <li>Call setHasValidateCommand(1) on the step
  ///  <li>Create a slot foo() associated with any QObject*, following these instructions:</li>
  ///  <ul>
  ///     <li>Set the following connection:</li>
  ///     <ul>
  ///       <li>QObject::connect(step, SIGNAL(invokeValidateCommand(const QString&)), object,
  /// SLOT(foo(const QString&)))</li>
  ///      </ul>
  ///    </ul>
  ///  </ul>
  virtual void validate(const QString& desiredBranchId = QString());

  /// \brief Signal (emitted by the private implementation) indicating that validation of this
  /// step's processing should be performed.
  ///
  /// \sa validation()
  void invokeValidateCommand(const QString& desiredBranchId = QString())const;

  /// \brief Signal (emitted by the private implementation) indicating that validation of this
  /// step's processing has completed.
  ///
  /// \sa validation()
  void validationComplete(bool validationSuceeded, const QString& branchId = QString())const;

  /// \brief Signal (emitted by the private implementation) indicating that the step's 'onEntry'
  /// processing should be performed.
  ///
  /// \sa onEntry()
  void invokeOnEntryCommand(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const;

  /// \brief Signal (emitted by the private implementation) indicating that the step's 'onEntry'
  /// processing has completed.
  ///
  /// \sa onEntry()
  void onEntryComplete()const;

  /// \brief Signal (emitted by the private implementation) indicating that the step's 'onExit'
  /// processing should be performed.
  ///
  /// \sa onExit()
  void invokeOnExitCommand(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const;

  /// \brief Signal (emitted by the private implementation) indicating that the step's 'onExit'
  /// processing has completed.
  ///
  /// \sa onExit()
  void onExitComplete()const;

protected:
  QScopedPointer<ctkWorkflowStepPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowStep);
  Q_DISABLE_COPY(ctkWorkflowStep);
  friend class ctkWorkflow;        // For access to processingState, validationState, setWorkflow, validate
  friend class ctkWorkflowPrivate; // For access to invokeOn{Entry,Exit}Command, on{Entry,Exit}
};

#endif
