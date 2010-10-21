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

// Qt includes
#include <QObject>
#include <QWidget>
#include <QList>
#include <QDebug>
#include <QIcon>

// CTK includes
#include "ctkWorkflowAbstractWidgetStep.h"
#include "ctkWorkflowAbstractWidgetStep_p.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflow.h"
//#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkLogger.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.widgets.ctkWorkflowAbstractWidgetStep");
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ctkWorkflowAbstractWidgetStepPrivate methods

//-----------------------------------------------------------------------------
ctkWorkflowAbstractWidgetStepPrivate::ctkWorkflowAbstractWidgetStepPrivate(ctkWorkflowAbstractWidgetStep& object)
  :q_ptr(&object)
{
//  this->buttonBoxWidget = 0;
//  this->hasButtonBoxWidget = false;

  this->icon = QIcon();
  this->created = false;
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStepPrivate::invokeShowUserInterfaceCommandInternal()const
{
  emit invokeShowUserInterfaceCommand();
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStepPrivate::showUserInterfaceCompleteInternal()const
{
  emit showUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStepPrivate::showUserInterface()
{
  Q_Q(ctkWorkflowAbstractWidgetStep);
  q->showUserInterface();
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStepPrivate::invokeCreateUserInterfaceCommandInternal()const
{
  emit invokeCreateUserInterfaceCommand();
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStepPrivate::createUserInterfaceCompleteInternal()const
{
  emit createUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
// ctkWorkflowAbstractWidgetStep methods

//-----------------------------------------------------------------------------
ctkWorkflowAbstractWidgetStep::ctkWorkflowAbstractWidgetStep(ctkWorkflow* newWorkflow, const QString& newId) : Superclass(newWorkflow, newId)
  , d_ptr(new ctkWorkflowAbstractWidgetStepPrivate(*this))
{
  Q_D(ctkWorkflowAbstractWidgetStep);
  d->hasShowUserInterfaceCommand = 0;
  d->hasCreateUserInterfaceCommand = 0;
  d->ButtonBoxHints = ctkWorkflowAbstractWidgetStep::NoHints;
}

//-----------------------------------------------------------------------------
ctkWorkflowAbstractWidgetStep::~ctkWorkflowAbstractWidgetStep()
{
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowAbstractWidgetStep, int, hasShowUserInterfaceCommand, hasShowUserInterfaceCommand);
CTK_SET_CPP(ctkWorkflowAbstractWidgetStep, int, setHasShowUserInterfaceCommand, hasShowUserInterfaceCommand);
CTK_GET_CPP(ctkWorkflowAbstractWidgetStep, int, hasCreateUserInterfaceCommand, hasCreateUserInterfaceCommand);
CTK_SET_CPP(ctkWorkflowAbstractWidgetStep, int, setHasCreateUserInterfaceCommand, hasCreateUserInterfaceCommand);
CTK_GET_CPP(ctkWorkflowAbstractWidgetStep, QString, backButtonText, backButtonText);
CTK_SET_CPP(ctkWorkflowAbstractWidgetStep, const QString&, setBackButtonText, backButtonText);
CTK_GET_CPP(ctkWorkflowAbstractWidgetStep, QString, nextButtonText, nextButtonText);
CTK_SET_CPP(ctkWorkflowAbstractWidgetStep, const QString&, setNextButtonText, nextButtonText);
// CTK_GET_CPP(ctkWorkflowAbstractWidgetStep, QList<QString>, finishButtonTexts, finishButtonTexts);
// CTK_SET_CPP(ctkWorkflowAbstractWidgetStep, QList<QString>, setFinishButtonTexts, finishButtonTexts);
//CTK_GET_CPP(ctkWorkflowAbstractWidgetStep, bool, hasButtonBoxWidget, hasButtonBoxWidget);
//CTK_SET_CPP(ctkWorkflowAbstractWidgetStep, bool, setHasButtonBoxWidget, hasButtonBoxWidget);
CTK_GET_CPP(ctkWorkflowAbstractWidgetStep, QIcon, icon, icon);
CTK_SET_CPP(ctkWorkflowAbstractWidgetStep, const QIcon&, setIcon, icon);

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowAbstractWidgetStep, ctkWorkflowAbstractWidgetStep::ButtonBoxHints,
            buttonBoxHints, ButtonBoxHints);
CTK_SET_CPP(ctkWorkflowAbstractWidgetStep, ctkWorkflowAbstractWidgetStep::ButtonBoxHints,
            setButtonBoxHints, ButtonBoxHints);

//-----------------------------------------------------------------------------
// void ctkWorkflowAbstractWidgetStep::setFinishButtonText(const QString& name)
// {
//   QList<QString> names;
//   names << name;
//   this->setFinishButtonTexts(names);
// }

// //-----------------------------------------------------------------------------
// ctkWorkflowButtonBoxWidget* ctkWorkflowAbstractWidgetStep::buttonBoxWidget()
// {
//   Q_D(ctkWorkflowAbstractWidgetStep);

//   if (!d->hasButtonBoxWidget)
//     {
//     return 0;
//     }

//   if (!d->buttonBoxWidget)
//     {
//     if (!this->workflow())
//       {
//       logger.error("buttonBoxWidget - Cannot create buttonBoxWidget without a workflow");
//       return 0;
//       }
//     d->buttonBoxWidget = new ctkWorkflowButtonBoxWidget(this->workflow());
//     }
//   return d->buttonBoxWidget;
// }

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStep::showUserInterface()
{
  Q_D(ctkWorkflowAbstractWidgetStep);

  // use the user's showUserInterfaceCommand if given
  if (d->hasShowUserInterfaceCommand)
    {
    this->invokeShowUserInterfaceCommand();
    return;
    }

  // otherwise we provide an implementation here
  logger.debug(QString("showUserInterface - showing %1").arg(this->name()));

  // create the user interface if this is the first time we're showing this step
  if (!d->created)
    {
    if (d->hasCreateUserInterfaceCommand)
      {
      this->invokeCreateUserInterfaceCommand();
      }
    else
      {
      this->createUserInterface();
      }
    d->created = true;
    }

  emit showUserInterfaceComplete();
}

// --------------------------------------------------------------------------
QObject* ctkWorkflowAbstractWidgetStep::ctkWorkflowAbstractWidgetStepQObject()
{
  Q_D(ctkWorkflowAbstractWidgetStep);
  return d;
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStep::invokeShowUserInterfaceCommand()const
{
  Q_D(const ctkWorkflowAbstractWidgetStep);
  d->invokeShowUserInterfaceCommandInternal();
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStep::showUserInterfaceComplete()const
{
  Q_D(const ctkWorkflowAbstractWidgetStep);
  d->showUserInterfaceCompleteInternal();
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStep::invokeCreateUserInterfaceCommand()const
{
  Q_D(const ctkWorkflowAbstractWidgetStep);
  d->invokeCreateUserInterfaceCommandInternal();
}

//-----------------------------------------------------------------------------
void ctkWorkflowAbstractWidgetStep::createUserInterfaceComplete()const
{
  Q_D(const ctkWorkflowAbstractWidgetStep);
  d->createUserInterfaceCompleteInternal();
}
