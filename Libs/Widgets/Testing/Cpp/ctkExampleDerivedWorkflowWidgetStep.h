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

#ifndef __ctkExampleDerivedWorkflowWidgetStep_h
#define __ctkExampleDerivedWorkflowWidgetStep_h

// CTK includes
#include "ctkPimpl.h"
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowTransitions.h"
class ctkWorkflow;

class ctkExampleDerivedWorkflowWidgetStepPrivate;
class QLabel;
class QLineEdit;

///
/// ctkExampleDerivedWorkflowWidgetStep is an example custom step with
/// a user interface, created by deriving ctkWorkflowWidgetStep and
/// re-implementing validate(const QString&), onEntry(), onExit()
/// and populateStepWidgetsList().

class ctkExampleDerivedWorkflowWidgetStep : public ctkWorkflowWidgetStep
{
  Q_OBJECT
public:
  typedef ctkWorkflowWidgetStep Superclass;
  explicit ctkExampleDerivedWorkflowWidgetStep(const QString& newId);
  virtual ~ctkExampleDerivedWorkflowWidgetStep();

  ///
  /// Set/get the label on this step's user interface
  QLabel* label()const;
  void setLabel(QLabel* label);

  ///
  /// Set/get the line edit on this step's user interface
  QLineEdit* lineEdit()const;
  void setLineEdit(QLineEdit* lineEdit);

  ///
  /// Get the values for the counters of the number of times we have
  /// run the onEntry() and onExit() functions
  int numberOfTimesRanOnEntry()const;
  int numberOfTimesRanOnExit()const;

  ///
  /// Adds the label and line edit on this step's user interface to
  /// the given list, which will be used by the superclass's
  /// showUserInterface() function
  virtual void createUserInterface();

protected:

  virtual void validate(const QString& desiredBranchId = QString());

  ///
  /// Increments the counter numberOfTimesRanOnEntry
  virtual void onEntry(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

  ///
  /// Increments the counter numberOfTimesRanOnExit
  virtual void onExit(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

protected:
  QScopedPointer<ctkExampleDerivedWorkflowWidgetStepPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkExampleDerivedWorkflowWidgetStep);
  Q_DISABLE_COPY(ctkExampleDerivedWorkflowWidgetStep);

};

#endif
