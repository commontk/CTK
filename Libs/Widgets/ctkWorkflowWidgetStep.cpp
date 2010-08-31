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
#include <QWidget>
#include <QList>
#include <QDebug>
#include <QIcon>

// CTK includes
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflow.h"
#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkLogger.h"

// STD includes
#include <iostream>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.widgets.ctkWorkflowWidgetStep");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkWorkflowWidgetStepPrivate : public ctkPrivate<ctkWorkflowWidgetStep>
{
public:
  CTK_DECLARE_PUBLIC(ctkWorkflowWidgetStep);
  ctkWorkflowWidgetStepPrivate();
  ~ctkWorkflowWidgetStepPrivate(){}

  QWidget*                    stepArea;
  QList<QWidget*>             stepWidgetsList;

  QList<ctkWorkflowStep*> finishStepsToHaveButtonsInStepArea;

  // orientation for layout of the step area and button area (for use
  // with QBoxLayouy only)
  QBoxLayout::Direction stepAreaDirection;

  int hasPopulateStepWidgetsListCommand;
  int hasShowUserInterfaceCommand;

  // names for the buttons
  QString backButtonText;
  QString nextButtonText;
//  QList<QString> finishButtonTexts;

  ctkWorkflowButtonBoxWidget* buttonBoxWidget;
  bool                        hasButtonBoxWidget;

  ctkWorkflowWidgetStep::ButtonBoxHints  ButtonBoxHints;

  QIcon icon;
};

//-----------------------------------------------------------------------------
// ctkWorkflowWidgetStepPrivate methods

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStepPrivate::ctkWorkflowWidgetStepPrivate()
{
  this->stepArea = 0;
  this->buttonBoxWidget = 0;
  this->hasButtonBoxWidget = false;

  stepAreaDirection = QBoxLayout::TopToBottom;

  icon = QIcon();
}

//-----------------------------------------------------------------------------
// ctkWorkflowWidgetStep methods

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStep::ctkWorkflowWidgetStep(ctkWorkflow* newWorkflow, const QString& newId) : Superclass(newWorkflow, newId)
{
  CTK_INIT_PRIVATE(ctkWorkflowWidgetStep);
  CTK_D(ctkWorkflowWidgetStep);

  d->hasPopulateStepWidgetsListCommand = 0;
  d->hasShowUserInterfaceCommand = 0;

  QObject::connect(this, SIGNAL(populateStepWidgetsListComplete()), this, SLOT(evaluatePopulateStepWidgetsListResults()));
}

//-----------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowWidgetStep, QBoxLayout::Direction, stepAreaDirection, stepAreaDirection);
CTK_GET_CXX(ctkWorkflowWidgetStep, int, hasPopulateStepWidgetsListCommand, hasPopulateStepWidgetsListCommand);
CTK_SET_CXX(ctkWorkflowWidgetStep, int, setHasPopulateStepWidgetsListCommand, hasPopulateStepWidgetsListCommand);
CTK_GET_CXX(ctkWorkflowWidgetStep, int, hasShowUserInterfaceCommand, hasShowUserInterfaceCommand);
CTK_SET_CXX(ctkWorkflowWidgetStep, int, setHasShowUserInterfaceCommand, hasShowUserInterfaceCommand);
CTK_GET_CXX(ctkWorkflowWidgetStep, QString, backButtonText, backButtonText);
CTK_SET_CXX(ctkWorkflowWidgetStep, const QString&, setBackButtonText, backButtonText);
CTK_GET_CXX(ctkWorkflowWidgetStep, QString, nextButtonText, nextButtonText);
CTK_SET_CXX(ctkWorkflowWidgetStep, const QString&, setNextButtonText, nextButtonText);
// CTK_GET_CXX(ctkWorkflowWidgetStep, QList<QString>, finishButtonTexts, finishButtonTexts);
// CTK_SET_CXX(ctkWorkflowWidgetStep, QList<QString>, setFinishButtonTexts, finishButtonTexts);
CTK_GET_CXX(ctkWorkflowWidgetStep, bool, hasButtonBoxWidget, hasButtonBoxWidget);
CTK_SET_CXX(ctkWorkflowWidgetStep, bool, setHasButtonBoxWidget, hasButtonBoxWidget);
CTK_GET_CXX(ctkWorkflowWidgetStep, QIcon, icon, icon);
CTK_SET_CXX(ctkWorkflowWidgetStep, const QIcon&, setIcon, icon);

//-----------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowWidgetStep, ctkWorkflowWidgetStep::ButtonBoxHints,
            buttonBoxHints, ButtonBoxHints);
CTK_SET_CXX(ctkWorkflowWidgetStep, ctkWorkflowWidgetStep::ButtonBoxHints,
            setButtonBoxHints, ButtonBoxHints);


//-----------------------------------------------------------------------------
// void ctkWorkflowWidgetStep::setFinishButtonText(const QString& name)
// {
//   QList<QString> names;
//   names << name;
//   this->setFinishButtonTexts(names);
// }

//-----------------------------------------------------------------------------
QWidget* ctkWorkflowWidgetStep::stepArea()
{
  CTK_D(ctkWorkflowWidgetStep);
  if (!d->stepArea)
    {
    d->stepArea = new QWidget();
    d->stepArea->hide();
    }

  return d->stepArea;
}

//-----------------------------------------------------------------------------
ctkWorkflowButtonBoxWidget* ctkWorkflowWidgetStep::buttonBoxWidget()
{
  CTK_D(ctkWorkflowWidgetStep);
  if (!d->buttonBoxWidget)
    {
    if  (!this->workflow())
      {
      logger.error("buttonBoxWidget - Cannot create buttonBoxWidget without a workflow");
      return 0;
      }
    d->buttonBoxWidget = new ctkWorkflowButtonBoxWidget(this->workflow());
    d->stepArea->layout()->addWidget(d->buttonBoxWidget);
    }
  return d->buttonBoxWidget;
}

// --------------------------------------------------------------------------
void ctkWorkflowWidgetStep::changeWidgetDirection(QWidget* widget, const QBoxLayout::Direction& direction)
{
  // easy to change layout direction if we don't already have a layout
  if (!widget->layout())
    {
    widget->setLayout(new QBoxLayout(direction));
    }
  // do nothing if the page's layout cannot be cast to a QBoxLayout
  else if (QBoxLayout* widgetLayout = qobject_cast<QBoxLayout*>(widget->layout()))
    {
    widgetLayout->setDirection(direction);
    }
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::setStepAreaDirection(const QBoxLayout::Direction& direction)
{
  this->changeWidgetDirection(this->stepArea(), direction);

  CTK_D(ctkWorkflowWidgetStep);
  d->stepAreaDirection = direction;
}

//-----------------------------------------------------------------------------
// reimplement in derived classes
void ctkWorkflowWidgetStep::showUserInterface()
{
  CTK_D(ctkWorkflowWidgetStep);
  // use the user's showUserInterfaceCommand if given
  if (d->hasShowUserInterfaceCommand)
    {
    emit invokeShowUserInterfaceCommand();
    return;
    }

  // otherwise we provide an implementation here
  logger.debug(QString("showUserInterface - showing %1").arg(this->name()));

  QWidget* stepArea = this->stepArea();

  // create the default step area layout, if there isn't one set
  if (!stepArea->layout())
    {
    d->stepArea->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    }

  // add the step-specific widgets - only runs the first time this
  // method is called
  if (d->stepWidgetsList.isEmpty())
    {
    this->populateStepWidgetsList(d->stepWidgetsList);
    // actually enabling/showing the stepArea is handled by evaluatePopulateStepWidgetsListResults
    }
  else
    {
    this->addButtons();
    }
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::addButtons()
{
  CTK_D(ctkWorkflowWidgetStep);
  if (d->hasButtonBoxWidget)
    {
    ctkWorkflowButtonBoxWidget* buttonBoxWidget = this->buttonBoxWidget();
    buttonBoxWidget->updateButtons();
    }
  
  emit showUserInterfaceComplete();
}

//-----------------------------------------------------------------------------
// reimplement in derived classes
void ctkWorkflowWidgetStep::populateStepWidgetsList(QList<QWidget*>& stepWidgetsList)
{
  CTK_D(ctkWorkflowWidgetStep);
  if (d->hasPopulateStepWidgetsListCommand)
    {
    emit invokePopulateStepWidgetsListCommand(stepWidgetsList);
    }
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::evaluatePopulateStepWidgetsListResults()
{
  // iterate through the list of widgets, add them to the layout
  CTK_D(ctkWorkflowWidgetStep);

  if (!d->stepArea)
    {
    logger.error("evaluatePopulateStepWidgetsListResults - stepArea is Null");
    return;
    }

  foreach(QWidget* widget, d->stepWidgetsList)
    {
    if (widget)
      {
      logger.debug("evaluatePopulateStepWidgetsListResults - Adding widget to layout");
      d->stepArea->layout()->addWidget(widget);
      }
    }

  this->addButtons();

}
