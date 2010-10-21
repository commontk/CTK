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

#ifndef __ctkWorkflowStep_p_h
#define __ctkWorkflowStep_p_h

// Qt includes
#include <QObject>
class QString;
class QState;

// CTK includes
#include <ctkPimpl.h>
#include "ctkWorkflow.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowTransitions.h"
class ctkWorkflow;

//-----------------------------------------------------------------------------
class ctkWorkflowStepPrivate: public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkWorkflowStep);

public:
  ctkWorkflowStep* q_ptr;
  ctkWorkflowStepPrivate(ctkWorkflowStep& object);
  ~ctkWorkflowStepPrivate();

protected:
  ctkWorkflow* Workflow;

  QString      Id;
  QString      Name;
  QString      Description;
  QString      StatusText;

  QState* ProcessingState;
  QState* ValidationState;

  ctkWorkflowIntrastepTransition* ValidationTransition;
  ctkWorkflowIntrastepTransition* ValidationFailedTransition;

  bool HasValidateCommand;
  bool HasOnEntryCommand;
  bool HasOnExitCommand;

  void invokeValidateCommandInternal(const QString& desiredBranchId)const;

  void validationCompleteInternal(bool validationSuceeded, const QString& branchId)const;

  void invokeOnEntryCommandInternal(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const;

  void onEntryCompleteInternal()const;

  void invokeOnExitCommandInternal(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const;

  void onExitCompleteInternal()const;


signals:
  void invokeValidateCommand(const QString& desiredBranchId)const;

  void validationComplete(bool validationSuceeded, const QString& branchId)const;

  void invokeOnEntryCommand(const ctkWorkflowStep* comingFrom, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const;

  void onEntryComplete()const;

  void invokeOnExitCommand(const ctkWorkflowStep* goingTo, const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType)const;

  void onExitComplete()const;

private:
  friend class ctkWorkflowPrivate; // for access to invoke...Internal() methods

};

#endif
