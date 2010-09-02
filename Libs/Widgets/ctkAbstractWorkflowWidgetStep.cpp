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

// Qt includes
#include <QObject>
#include <QWidget>
#include <QList>
#include <QDebug>
#include <QIcon>

// CTK includes
#include "ctkAbstractWorkflowWidgetStep.h"
#include "ctkAbstractWorkflowWidgetStep_p.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflow.h"
//#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkLogger.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.widgets.ctkAbstractWorkflowWidgetStep");
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ctkAbstractWorkflowWidgetStepPrivate methods

//-----------------------------------------------------------------------------
ctkAbstractWorkflowWidgetStepPrivate::ctkAbstractWorkflowWidgetStepPrivate()
{
//  this->buttonBoxWidget = 0;
//  this->hasButtonBoxWidget = false;

  this->icon = QIcon();
  this->created = false;
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStepPrivate::invokeShowUserInterfaceCommandInternal()const
{
  emit invokeShowUserInterfaceCommand();
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStepPrivate::showUserInterfaceCompleteInternal()const
{
  emit showUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStepPrivate::showUserInterface()
{
  CTK_P(ctkAbstractWorkflowWidgetStep);
  p->showUserInterface();
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStepPrivate::invokeCreateUserInterfaceCommandInternal()const
{
  emit invokeCreateUserInterfaceCommand();
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStepPrivate::createUserInterfaceCompleteInternal()const
{
  emit createUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
// ctkAbstractWorkflowWidgetStep methods

//-----------------------------------------------------------------------------
ctkAbstractWorkflowWidgetStep::ctkAbstractWorkflowWidgetStep(ctkWorkflow* newWorkflow, const QString& newId) : Superclass(newWorkflow, newId)
{
  CTK_INIT_PRIVATE(ctkAbstractWorkflowWidgetStep);
  CTK_D(ctkAbstractWorkflowWidgetStep);
  d->hasShowUserInterfaceCommand = 0;
  d->hasCreateUserInterfaceCommand = 0;
  d->ButtonBoxHints = Self::NoHints;
}

//-----------------------------------------------------------------------------
CTK_GET_CXX(ctkAbstractWorkflowWidgetStep, int, hasShowUserInterfaceCommand, hasShowUserInterfaceCommand);
CTK_SET_CXX(ctkAbstractWorkflowWidgetStep, int, setHasShowUserInterfaceCommand, hasShowUserInterfaceCommand);
CTK_GET_CXX(ctkAbstractWorkflowWidgetStep, int, hasCreateUserInterfaceCommand, hasCreateUserInterfaceCommand);
CTK_SET_CXX(ctkAbstractWorkflowWidgetStep, int, setHasCreateUserInterfaceCommand, hasCreateUserInterfaceCommand);
CTK_GET_CXX(ctkAbstractWorkflowWidgetStep, QString, backButtonText, backButtonText);
CTK_SET_CXX(ctkAbstractWorkflowWidgetStep, const QString&, setBackButtonText, backButtonText);
CTK_GET_CXX(ctkAbstractWorkflowWidgetStep, QString, nextButtonText, nextButtonText);
CTK_SET_CXX(ctkAbstractWorkflowWidgetStep, const QString&, setNextButtonText, nextButtonText);
// CTK_GET_CXX(ctkAbstractWorkflowWidgetStep, QList<QString>, finishButtonTexts, finishButtonTexts);
// CTK_SET_CXX(ctkAbstractWorkflowWidgetStep, QList<QString>, setFinishButtonTexts, finishButtonTexts);
//CTK_GET_CXX(ctkAbstractWorkflowWidgetStep, bool, hasButtonBoxWidget, hasButtonBoxWidget);
//CTK_SET_CXX(ctkAbstractWorkflowWidgetStep, bool, setHasButtonBoxWidget, hasButtonBoxWidget);
CTK_GET_CXX(ctkAbstractWorkflowWidgetStep, QIcon, icon, icon);
CTK_SET_CXX(ctkAbstractWorkflowWidgetStep, const QIcon&, setIcon, icon);

//-----------------------------------------------------------------------------
CTK_GET_CXX(ctkAbstractWorkflowWidgetStep, ctkAbstractWorkflowWidgetStep::ButtonBoxHints,
            buttonBoxHints, ButtonBoxHints);
CTK_SET_CXX(ctkAbstractWorkflowWidgetStep, ctkAbstractWorkflowWidgetStep::ButtonBoxHints,
            setButtonBoxHints, ButtonBoxHints);

//-----------------------------------------------------------------------------
// void ctkAbstractWorkflowWidgetStep::setFinishButtonText(const QString& name)
// {
//   QList<QString> names;
//   names << name;
//   this->setFinishButtonTexts(names);
// }

// //-----------------------------------------------------------------------------
// ctkWorkflowButtonBoxWidget* ctkAbstractWorkflowWidgetStep::buttonBoxWidget()
// {
//   CTK_D(ctkAbstractWorkflowWidgetStep);

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
void ctkAbstractWorkflowWidgetStep::showUserInterface()
{
  CTK_D(ctkAbstractWorkflowWidgetStep);

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
QObject* ctkAbstractWorkflowWidgetStep::ctkAbstractWorkflowWidgetStepQObject()
{
  CTK_D(ctkAbstractWorkflowWidgetStep);
  return d;
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStep::invokeShowUserInterfaceCommand()const
{
  CTK_D(const ctkAbstractWorkflowWidgetStep);
  d->invokeShowUserInterfaceCommandInternal();
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStep::showUserInterfaceComplete()const
{
  CTK_D(const ctkAbstractWorkflowWidgetStep);
  d->showUserInterfaceCompleteInternal();
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStep::invokeCreateUserInterfaceCommand()const
{
  CTK_D(const ctkAbstractWorkflowWidgetStep);
  d->invokeCreateUserInterfaceCommandInternal();
}

//-----------------------------------------------------------------------------
void ctkAbstractWorkflowWidgetStep::createUserInterfaceComplete()const
{
  CTK_D(const ctkAbstractWorkflowWidgetStep);
  d->createUserInterfaceCompleteInternal();
}
