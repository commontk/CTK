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

#ifndef __ctkWorkflowAbstractWidgetStep_p_h
#define __ctkWorkflowAbstractWidgetStep_p_h

// Qt includes
#include <QObject>
#include <QList>
#include <QIcon>
class QWidget;

// CTK includes
#include "ctkWorkflowAbstractWidgetStep.h"
#include "ctkWorkflowStep_p.h"
//class ctkWorkflowButtonBoxWidget;

//-----------------------------------------------------------------------------
class ctkWorkflowAbstractWidgetStepPrivate : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkWorkflowAbstractWidgetStep);
protected:
  ctkWorkflowAbstractWidgetStep* q_ptr;
  
public:
  ctkWorkflowAbstractWidgetStepPrivate(ctkWorkflowAbstractWidgetStep& object);
  ~ctkWorkflowAbstractWidgetStepPrivate(){}

protected:

  QList<ctkWorkflowStep*> finishStepsToHaveButtonsInStepArea;

  // int hasPopulateStepWidgetsListCommand;
  int hasShowUserInterfaceCommand;
  int hasCreateUserInterfaceCommand;

  // names for the buttons
  QString backButtonText;
  QString nextButtonText;

  ctkWorkflowAbstractWidgetStep::ButtonBoxHints  ButtonBoxHints;

  QIcon icon;

  bool created;

  void invokeShowUserInterfaceCommandInternal()const;

  void showUserInterfaceCompleteInternal()const;

  void invokeCreateUserInterfaceCommandInternal()const;

  void createUserInterfaceCompleteInternal()const;

protected slots:

  ///
  /// Calls corresponding function in public implementation

  virtual void showUserInterface();

signals:

  void invokeShowUserInterfaceCommand()const;

  void showUserInterfaceComplete()const;

  void invokeCreateUserInterfaceCommand()const;

  void createUserInterfaceComplete()const;

};

#endif
