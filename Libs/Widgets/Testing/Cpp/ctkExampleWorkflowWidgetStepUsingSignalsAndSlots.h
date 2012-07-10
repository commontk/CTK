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

#ifndef __ctkExampleWorkflowWidgetStepUsingSignalsAndSlots_h
#define __ctkExampleWorkflowWidgetStepUsingSignalsAndSlots_h

// CTK includes
#include "ctkPimpl.h"
#include "ctkWorkflowTransitions.h"

class ctkExampleWorkflowWidgetStepUsingSignalsAndSlotsPrivate;
class ctkWorkflowWidget;
class ctkWorkflowStep;
class QLabel;
class QLineEdit;
class QWidget;

///
/// ctkExampleWorkflowWidgetStepUsingSignalsAndSlots represents an example
/// custom step with a user interface, created by deriving QObject
/// (not ctkWorkflowWidgetStep) and implementing functions for
/// validate(const QString&), onEntry() and onExit() that work using
/// signals and slots.

///
/// Need two connections to use this class's validate(const QString&) function, and
/// must also set the step's hasValidateCommand flag:
/// QObject::connect(step, SIGNAL(invokeValidateCommand(const QString&)), qObject,
/// SLOT(validate(const QString&)))
/// QObject::connect(qObject, SIGNAL(validationComplete(int)),
/// workflow, SLOT(evaluateValidationResults(int)));
/// step->setHasValidateCommand(1);

///
/// Need two connections to use this class's onEntry()
/// function, and must also set the step's hasOnEntryCommand
/// flag:
/// QObject::connect(step, SIGNAL(invokeOnEntryCommand(const
/// ctkWorkflowWidgetStep*, const
/// ctkWorkflowTransition::WorkflowTransitionType)), qObject,
/// SLOT(onEntry(const ctkWorkflowWidgetStep*, const
/// ctkWorkflowTransition::WorkflowTransitionType)));
/// QObject::connect(qObject, SIGNAL(onEntryComplete()), step,
/// SLOT(evaluateOnEntryResults()));
/// step->setHasOnEntryCommand(1);

///
/// Need two connectins to use this class's onExit() function,
/// and must also set the step's hasOnExitCommand() flag:
/// QObject::connect(step, SIGNAL(invokeOnExitCommand(const
/// ctkWorkflowWidgetStep*, const
/// ctkWorkflowTransition::WorkflowTransitionType)), qObject,
/// SLOT(onExit(const ctkWorkflowWidgetStep*, const
/// ctkWorkflowTransition::WorkflowTransitionType)));
/// QObject::connect(qObject, SIGNAL(onExitComplete()), step,
/// SLOT(evaluateOnExitResults()));
/// step->setHasOnExitCommand(1);

class ctkExampleWorkflowWidgetStepUsingSignalsAndSlots : public QObject
{
  Q_OBJECT

public:

  typedef QObject Superclass;
  explicit ctkExampleWorkflowWidgetStepUsingSignalsAndSlots(ctkWorkflowStep* newStep,
                                                            QObject* newParent = 0);
  virtual ~ctkExampleWorkflowWidgetStepUsingSignalsAndSlots();

  // Set/get the widget onto which this step's user interface will be placed
  QWidget* widget()const;
  void setWidget(QWidget* widget);

  /// Set/get the label on this step's user interface
  QLabel* label()const;
  void setLabel(QLabel* label);

  /// Set/get the line edit on this step's user interface
  QLineEdit* lineEdit()const;
  void setLineEdit(QLineEdit* lineEdit);

  /// 
  /// Get the values for the counters of the number of times we have
  /// run the onEntry() and onExit() functions
  int numberOfTimesRanOnEntry()const;
  int numberOfTimesRanOnExit()const;

public Q_SLOTS:

  /// Returns 1 (validation successful) if the step's lineEdit
  /// contains an integer whose value is greater than or equal to 10,
  /// returns 0 (validation failed) otherwise
  virtual void validate(const QString& desiredBranchId = QString());

  /// Increments the counter numberOfTimesRanOnEntry
  virtual void onEntry(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType tracnsitionType);

  /// Increments the counter numberOfTimesRanOnExit
  virtual void onExit(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

  /// Adds the label and line edit on this step's user interface to
  /// the given list, which will be used by the superclass's
  /// showUserInterface() function 
  virtual void createUserInterface();

Q_SIGNALS:

  /// Signals indicating to the workflow that these processes have
  /// completed
  void validationComplete(bool validationSucceeded, const QString& branchId="")const;
  void onEntryComplete()const;
  void onExitComplete()const;
  void createUserInterfaceComplete()const;

protected:
  QScopedPointer<ctkExampleWorkflowWidgetStepUsingSignalsAndSlotsPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots);
  Q_DISABLE_COPY(ctkExampleWorkflowWidgetStepUsingSignalsAndSlots);

};

#endif
