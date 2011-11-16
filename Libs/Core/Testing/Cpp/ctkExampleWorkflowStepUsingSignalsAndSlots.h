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

#ifndef __ctkExampleWorkflowStepUsingSignalsAndSlots_h
#define __ctkExampleWorkflowStepUsingSignalsAndSlots_h

// CTK includes
#include "ctkPimpl.h"
class ctkWorkflowStep;
#include "ctkWorkflowTransitions.h"

class ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate;

///
/// ctkExampleWorkflowStepUsingSignalsAndSlots represents an example
/// custom step created by deriving QObject (not ctkWorkflowStep) and
/// implementing functions for validate(const QString&), onEntry() and
/// onExit() that work using signals and slots.

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
/// ctkWorkflowStep*, const
/// ctkWorkflowTransition::WorkflowTransitionType)), qObject,
/// SLOT(onEntry(const ctkWorkflowStep*, const
/// ctkWorkflowTransition::WorkflowTransitionType)));
/// QObject::connect(qObject, SIGNAL(onEntryComplete()), step,
/// SLOT(evaluateOnEntryResults()));
/// step->setHasOnEntryCommand(1);

///
/// Need two connectins to use this class's onExit() function,
/// and must also set the step's hasOnExitCommand() flag:
/// QObject::connect(step, SIGNAL(invokeOnExitCommand(const
/// ctkWorkflowStep*, const
/// ctkWorkflowTransition::WorkflowTransitionType)), qObject,
/// SLOT(onExit(const ctkWorkflowStep*, const
/// ctkWorkflowTransition::WorkflowTransitionType)));
/// QObject::connect(qObject, SIGNAL(onExitComplete()), step,
/// SLOT(evaluateOnExitResults()));
/// step->setHasOnExitCommand(1);

class ctkExampleWorkflowStepUsingSignalsAndSlots : public QObject
{
  Q_OBJECT

public:
  typedef QObject Superclass;
  explicit ctkExampleWorkflowStepUsingSignalsAndSlots(ctkWorkflowStep * newStep,
                                                      QObject* newParent = 0);
  virtual ~ctkExampleWorkflowStepUsingSignalsAndSlots();

  /// Get the values for the counters of the number of times we have
  /// run the onEntry() and onExit() functions
  virtual int numberOfTimesRanOnEntry()const;
  virtual int numberOfTimesRanOnExit()const;

protected Q_SLOTS:

  /// Always returns 1 (validation successful)
  virtual void validate(const QString& desiredBranchId = QString())const;

  /// Always returns 0 (validation failed)
  virtual void validateFails()const;

  /// Increments the counter numberOfTimesRanOnEntry
  virtual void onEntry(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

  ///
  /// Increments the counter numberOfTimesRanOnExit
  virtual void onExit(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

Q_SIGNALS:
  ///
  /// Signals indicating to the workflow that these processes have
  /// completed
  void validationComplete(bool validationSucceeded, const QString& branchId = "")const;
  void onEntryComplete()const;
  void onExitComplete()const;

protected:
  QScopedPointer<ctkExampleWorkflowStepUsingSignalsAndSlotsPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkExampleWorkflowStepUsingSignalsAndSlots);
  Q_DISABLE_COPY(ctkExampleWorkflowStepUsingSignalsAndSlots);

};

#endif
