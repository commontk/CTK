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

#ifndef __ctkWorkflowWidgetStep_p_h
#define __ctkWorkflowWidgetStep_p_h

// Qt includes
#include <QObject>
#include <QList>
#include <QIcon>

// CTK includes
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowStep_p.h"

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class ctkWorkflowWidgetStepPrivate : public ctkWorkflowStepPrivate
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkWorkflowWidgetStep);
protected:
  ctkWorkflowWidgetStep* q_ptr;

public:
  ctkWorkflowWidgetStepPrivate(ctkWorkflowWidgetStep& object);
  ~ctkWorkflowWidgetStepPrivate();
  typedef ctkWorkflowStepPrivate Superclass;

protected:

  QList<ctkWorkflowStep*> finishStepsToHaveButtonsInStepArea;

  // int hasPopulateStepWidgetsListCommand;
  bool hasShowUserInterfaceCommand;
  bool hasCreateUserInterfaceCommand;

  // names for the buttons
  QString backButtonText;
  QString nextButtonText;

  ctkWorkflowWidgetStep::ButtonBoxHints  ButtonBoxHints;

  QIcon icon;

  bool created;

  void invokeShowUserInterfaceCommandInternal()const;

  void showUserInterfaceCompleteInternal()const;

  void invokeCreateUserInterfaceCommandInternal()const;

  void createUserInterfaceCompleteInternal()const;

protected Q_SLOTS:

  ///
  /// Calls corresponding function in public implementation

  virtual void showUserInterface();

Q_SIGNALS:

  void invokeShowUserInterfaceCommand()const;

  void showUserInterfaceComplete()const;

  void invokeCreateUserInterfaceCommand()const;

  void createUserInterfaceComplete()const;

};

#endif
