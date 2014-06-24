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

// Qt includes
#include <QList>
#include <QDebug>

// CTK includes
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowWidgetStep_p.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflow.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
// ctkWorkflowWidgetStepPrivate methods

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStepPrivate::ctkWorkflowWidgetStepPrivate(ctkWorkflowWidgetStep& object)
  :Superclass(object), q_ptr(&object)
{
  this->WidgetType = true;
//  this->buttonBoxWidget = 0;
//  this->hasButtonBoxWidget = false;

  this->icon = QIcon();
  this->created = false;
}

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStepPrivate::~ctkWorkflowWidgetStepPrivate()
{
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStepPrivate::invokeShowUserInterfaceCommandInternal()const
{
  emit invokeShowUserInterfaceCommand();
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStepPrivate::showUserInterfaceCompleteInternal()const
{
  emit showUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStepPrivate::showUserInterface()
{
  Q_Q(ctkWorkflowWidgetStep);
  q->showUserInterface();
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStepPrivate::invokeCreateUserInterfaceCommandInternal()const
{
  emit invokeCreateUserInterfaceCommand();
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStepPrivate::createUserInterfaceCompleteInternal()const
{
  emit createUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
// ctkWorkflowWidgetStep methods

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStep::ctkWorkflowWidgetStep(QWidget* newParent) :
  QWidget(newParent),
  ctkWorkflowStep(new ctkWorkflowWidgetStepPrivate(*this), QString())
{
  Q_D(ctkWorkflowWidgetStep);
  d->hasShowUserInterfaceCommand = false;
  d->hasCreateUserInterfaceCommand = false;
  d->ButtonBoxHints = ctkWorkflowWidgetStep::NoHints;
}

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStep::ctkWorkflowWidgetStep(const QString& newId, QWidget* newParent) :
  QWidget(newParent),
  ctkWorkflowStep(new ctkWorkflowWidgetStepPrivate(*this), newId)
{
  Q_D(ctkWorkflowWidgetStep);
  d->hasShowUserInterfaceCommand = false;
  d->hasCreateUserInterfaceCommand = false;
  d->ButtonBoxHints = ctkWorkflowWidgetStep::NoHints;
}

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStep::~ctkWorkflowWidgetStep()
{
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowWidgetStep, bool, hasShowUserInterfaceCommand, hasShowUserInterfaceCommand);
CTK_SET_CPP(ctkWorkflowWidgetStep, bool, setHasShowUserInterfaceCommand, hasShowUserInterfaceCommand);
CTK_GET_CPP(ctkWorkflowWidgetStep, bool, hasCreateUserInterfaceCommand, hasCreateUserInterfaceCommand);
CTK_SET_CPP(ctkWorkflowWidgetStep, bool, setHasCreateUserInterfaceCommand, hasCreateUserInterfaceCommand);
CTK_GET_CPP(ctkWorkflowWidgetStep, QString, backButtonText, backButtonText);
CTK_SET_CPP(ctkWorkflowWidgetStep, const QString&, setBackButtonText, backButtonText);
CTK_GET_CPP(ctkWorkflowWidgetStep, QString, nextButtonText, nextButtonText);
CTK_SET_CPP(ctkWorkflowWidgetStep, const QString&, setNextButtonText, nextButtonText);
// CTK_GET_CPP(ctkWorkflowWidgetStep, QList<QString>, finishButtonTexts, finishButtonTexts);
// CTK_SET_CPP(ctkWorkflowWidgetStep, QList<QString>, setFinishButtonTexts, finishButtonTexts);
//CTK_GET_CPP(ctkWorkflowWidgetStep, bool, hasButtonBoxWidget, hasButtonBoxWidget);
//CTK_SET_CPP(ctkWorkflowWidgetStep, bool, setHasButtonBoxWidget, hasButtonBoxWidget);
CTK_GET_CPP(ctkWorkflowWidgetStep, QIcon, icon, icon);
CTK_SET_CPP(ctkWorkflowWidgetStep, const QIcon&, setIcon, icon);

//-----------------------------------------------------------------------------
QWidget* ctkWorkflowWidgetStep::stepArea()
{
  return this;
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowWidgetStep, ctkWorkflowWidgetStep::ButtonBoxHints,
            buttonBoxHints, ButtonBoxHints);
CTK_SET_CPP(ctkWorkflowWidgetStep, ctkWorkflowWidgetStep::ButtonBoxHints,
            setButtonBoxHints, ButtonBoxHints);

//-----------------------------------------------------------------------------
// void ctkWorkflowWidgetStep::setFinishButtonText(const QString& name)
// {
//   QList<QString> names;
//   names << name;
//   this->setFinishButtonTexts(names);
// }

// //-----------------------------------------------------------------------------
// ctkWorkflowButtonBoxWidget* ctkWorkflowWidgetStep::buttonBoxWidget()
// {
//   Q_D(ctkWorkflowWidgetStep);

//   if (!d->hasButtonBoxWidget)
//     {
//     return 0;
//     }

//   if (!d->buttonBoxWidget)
//     {
//     if (!this->workflow())
//       {
//       qWarning() << "buttonBoxWidget - Cannot create buttonBoxWidget without a workflow";
//       return 0;
//       }
//     d->buttonBoxWidget = new ctkWorkflowButtonBoxWidget(this->workflow());
//     }
//   return d->buttonBoxWidget;
// }

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::showUserInterface()
{
  Q_D(ctkWorkflowWidgetStep);

  // use the user's showUserInterfaceCommand if given
  if (d->hasShowUserInterfaceCommand)
    {
    this->invokeShowUserInterfaceCommand();
    return;
    }

  // otherwise we provide an implementation here
  if (this->workflow()->verbose())
    {
    qDebug() << QString("showUserInterface - showing %1").arg(this->name());
    }

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

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::invokeShowUserInterfaceCommand()const
{
  Q_D(const ctkWorkflowWidgetStep);
  d->invokeShowUserInterfaceCommandInternal();
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::showUserInterfaceComplete()const
{
  Q_D(const ctkWorkflowWidgetStep);
  d->showUserInterfaceCompleteInternal();
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::invokeCreateUserInterfaceCommand()const
{
  Q_D(const ctkWorkflowWidgetStep);
  d->invokeCreateUserInterfaceCommandInternal();
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::createUserInterfaceComplete()const
{
  Q_D(const ctkWorkflowWidgetStep);
  d->createUserInterfaceCompleteInternal();
}
