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
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QDebug>

// CTK includes
#include "ctkWorkflowWidget.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflow.h"
#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkLogger.h"

// STD includes
#include <iostream>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.widgets.ctkWorkflowWidget");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkWorkflowWidgetPrivate: public ctkPrivate<ctkWorkflowWidget>
{
public:
  CTK_DECLARE_PUBLIC(ctkWorkflowWidget);
  ctkWorkflowWidgetPrivate();
  ~ctkWorkflowWidgetPrivate();

  ctkWorkflow* workflow;

  // These are place-holders, so that widgets appear in the correct
  // place given the uncertain order in which they are created by the
  // user using the set() functions.  The layouts that are assigned to
  // them in the set() functions are arbirary, since each section will
  // hold only one widget.
  QWidget* titleSection;
  QWidget* subTitleSection;
  QWidget* preTextSection;
  QWidget* clientSection;
  QWidget* postTextSection;
  QWidget* errorSection;
  QWidget* buttonSection;

  QLabel*  titleLabel;
  QLabel*  subTitleLabel;
  QLabel*  preTextLabel;
  QWidget* clientArea;
  QLabel*  postTextLabel;
  QLabel*  errorLabel;

  // orientation for layout of this widget, and of the client area
  // (for use with QBoxLayout only)
  QBoxLayout::Direction direction;
  QBoxLayout::Direction clientAreaDirection;

  int hideWidgetsOfNonCurrentSteps;

  ctkWorkflowStep* stepShownPreviously;
  ctkWorkflowStep* stepShownCurrently;

  ctkWorkflowButtonBoxWidget* buttonBoxWidget;
  bool                        hasButtonBoxWidget;
};

// --------------------------------------------------------------------------
// ctkWorkflowWidgetPrivate methods

//---------------------------------------------------------------------------
ctkWorkflowWidgetPrivate::ctkWorkflowWidgetPrivate()
{
  this->workflow = 0;

  this->titleSection = new QWidget;
  this->subTitleSection = new QWidget;
  this->preTextSection = new QWidget;
  this->clientSection = new QWidget;
  this->postTextSection = new QWidget;
  this->errorSection = new QWidget;
  this->buttonSection = new QWidget;

  this->titleLabel = 0;
  this->subTitleLabel = 0;
  this->preTextLabel = 0;
  this->clientArea = 0;
  this->postTextLabel = 0;
  this->errorLabel = 0;

  direction = QBoxLayout::TopToBottom;
  clientAreaDirection = QBoxLayout::TopToBottom;

  this->hideWidgetsOfNonCurrentSteps = 0;

  this->stepShownPreviously = 0;
  this->stepShownCurrently = 0;

  this->hasButtonBoxWidget = true;
  this->buttonBoxWidget = 0;
}

//---------------------------------------------------------------------------
ctkWorkflowWidgetPrivate::~ctkWorkflowWidgetPrivate()
{
}

// --------------------------------------------------------------------------
// ctkWorkflowWidgetMethods

// --------------------------------------------------------------------------
ctkWorkflowWidget::ctkWorkflowWidget(QWidget* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkWorkflowWidget);
  CTK_D(ctkWorkflowWidget);

  // layout components vertically by default
  QBoxLayout* layout = new QBoxLayout(d->direction);
  this->setLayout(layout);
  layout->addWidget(d->titleSection);
  layout->addWidget(d->subTitleSection);
  layout->addWidget(d->preTextSection);
  layout->addWidget(d->clientSection);
  layout->addWidget(d->postTextSection);
  layout->addWidget(d->errorSection);
  this->setErrorText("");
  layout->addWidget(d->buttonSection);
}

// --------------------------------------------------------------------------
ctkWorkflowWidget::~ctkWorkflowWidget()
{
}

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowWidget, ctkWorkflow*, workflow, workflow);
CTK_GET_CXX(ctkWorkflowWidget, QBoxLayout::Direction, direction, direction);
CTK_GET_CXX(ctkWorkflowWidget, QBoxLayout::Direction, clientAreaDirection, clientAreaDirection);
CTK_GET_CXX(ctkWorkflowWidget, int, hideWidgetsOfNonCurrentSteps, hideWidgetsOfNonCurrentSteps);
CTK_SET_CXX(ctkWorkflowWidget, int, setHideWidgetsOfNonCurrentSteps, hideWidgetsOfNonCurrentSteps);
CTK_GET_CXX(ctkWorkflowWidget, bool, hasButtonBoxWidget, hasButtonBoxWidget);
CTK_SET_CXX(ctkWorkflowWidget, bool, setHasButtonBoxWidget, hasButtonBoxWidget);
CTK_GET_CXX(ctkWorkflowWidget, ctkWorkflowButtonBoxWidget*, buttonBoxWidget, buttonBoxWidget);

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setWorkflow(ctkWorkflow* newWorkflow)
{
  CTK_D(ctkWorkflowWidget);

  if (d->workflow)
    {
    QObject::disconnect(d->workflow, SIGNAL(currentStepChanged(ctkWorkflowStep*)), this, SLOT(updateClientArea(ctkWorkflowStep)));
    }

  d->workflow = newWorkflow;

  QObject::connect(newWorkflow, SIGNAL(currentStepChanged(ctkWorkflowStep*)), this, SLOT(updateClientArea(ctkWorkflowStep*)));


  // TODO workflow should be a parameter of ctkWorkflowWidget
  // setup the buttonBoxWidget
  if (d->hasButtonBoxWidget)
    {
    if (!d->buttonSection->layout())
      {
      d->buttonSection->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
      }
    d->buttonBoxWidget = new ctkWorkflowButtonBoxWidget(d->workflow);
    d->buttonSection->layout()->addWidget(d->buttonBoxWidget);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::changeWidgetDirection(QWidget* widget, const QBoxLayout::Direction& direction)
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

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setDirection(const QBoxLayout::Direction& direction)
{
  CTK_D(ctkWorkflowWidget);
  this->changeWidgetDirection(this, direction);
  d->direction = direction;
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setClientAreaDirection(const QBoxLayout::Direction& direction)
{
  this->changeWidgetDirection(this->clientArea(), direction);
  this->setPrivateClientAreaDirection(direction);
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setPrivateClientAreaDirection(const QBoxLayout::Direction& direction)
{
  CTK_D(ctkWorkflowWidget);
  d->clientAreaDirection = direction;
}

// --------------------------------------------------------------------------
// reimplement in derived classes
QWidget* ctkWorkflowWidget::clientArea()
{
  CTK_D(ctkWorkflowWidget);

  if (!d->clientArea)
    {
    d->clientArea = new QWidget;

    if (!d->clientSection->layout())
      {
      d->clientSection->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
      }
    d->clientSection->layout()->addWidget(d->clientArea);
    }

  return d->clientArea;
}

// --------------------------------------------------------------------------
// reimplement in derived classes
void ctkWorkflowWidget::addWidget(QWidget* widget)
{
  if (widget)
    {
    QWidget* clientArea = this->clientArea();

    // set the default client area layout, if there isn't one set.
    CTK_D(ctkWorkflowWidget);
    if (!clientArea->layout())
      {
      clientArea->setLayout(new QBoxLayout(d->clientAreaDirection));
      }

    clientArea->layout()->addWidget(widget);
    }
}

// --------------------------------------------------------------------------
// TODO needs work
void ctkWorkflowWidget::updateClientArea(ctkWorkflowStep* currentStep)
{
  // if we are trying to get to the finish step, then we've completely
  // exited the origin - we can set its changeUserInterface flag back
  // to normal, for the next time
  CTK_D(ctkWorkflowWidget);

  if (currentStep)
    {
    d->stepShownPreviously = d->stepShownCurrently;
    d->stepShownCurrently = currentStep;
    this->setTitle(currentStep->name());
    this->setSubTitle(currentStep->description());
    this->setErrorText(currentStep->statusText());
    // disable/hide the previously shown step
    if (ctkWorkflowWidgetStep* prevStep = qobject_cast<ctkWorkflowWidgetStep*>(d->stepShownPreviously))
      {
      logger.debug(QString("updateClientArea - hiding %1").arg(prevStep->name()));
      if (QWidget* stepArea = prevStep->stepArea())
        {
        stepArea->setEnabled(false);
        if (d->hideWidgetsOfNonCurrentSteps)
          {
          stepArea->hide();
          }
        }
      }
    ctkWorkflowWidgetStep* currentWidgetStep = qobject_cast<ctkWorkflowWidgetStep*>(currentStep);
    // show/enable the current step
    if (currentWidgetStep)
      {
      currentWidgetStep->showUserInterface();
      if (QWidget* stepArea = currentWidgetStep->stepArea())
        {
        // add the step's client area to the widget if we haven't before
        if (!this->isAncestorOf(stepArea))
          {
          this->addWidget(stepArea);
          }

        stepArea->setEnabled(true);
        this->showWidget(stepArea);
        }
      }

    // update the button box widget if we have one
    if (d->buttonBoxWidget)
      {
      // Hide button bar if specified by the steps
      bool hideButtonBar = false;
      if(currentWidgetStep)
        {
        hideButtonBar = currentWidgetStep->buttonBoxHints() & ctkWorkflowWidgetStep::ButtonBoxHidden;
        }
      d->buttonBoxWidget->setHidden(hideButtonBar);
      d->buttonBoxWidget->updateButtons();
      }
    }
}

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkWorkflowWidget, QWidget*, clientSection, clientSection);

// --------------------------------------------------------------------------
// reimplement in derived classes
void ctkWorkflowWidget::showWidget(QWidget* widget)
{
  if (widget)
    {
    widget->show();
    }
}

// --------------------------------------------------------------------------
QString ctkWorkflowWidget::title()const
{
  CTK_D(const ctkWorkflowWidget);

  if (!d->titleLabel)
    {
    return 0;
    }

  return d->titleLabel->text();
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setTitle(const QString& titleText)
{
  if (!titleText.isEmpty())
    {
    CTK_D(ctkWorkflowWidget);
    if (!d->titleLabel)
      {
      d->titleLabel = new QLabel;

      if (!d->titleSection->layout())
        {
        d->titleSection->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        }
      d->titleSection->layout()->addWidget(d->titleLabel);
      }

    d->titleLabel->setText(titleText);
    }
}

// --------------------------------------------------------------------------
QString ctkWorkflowWidget::subTitle()const
{
  CTK_D(const ctkWorkflowWidget);

  if (!d->subTitleLabel)
    {
    return 0;
    }

  return d->subTitleLabel->text();
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setSubTitle(const QString& subTitleText)
{
  if (!subTitleText.isEmpty())
    {
    CTK_D(ctkWorkflowWidget);
    if (!d->subTitleLabel)
      {
      d->subTitleLabel = new QLabel;
 
      if (!d->subTitleSection->layout())
        {
        d->subTitleSection->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        }
      d->subTitleSection->layout()->addWidget(d->subTitleLabel);
      }

    d->subTitleLabel->setText(subTitleText);
    }
}

// --------------------------------------------------------------------------
QString ctkWorkflowWidget::preText()const
{
  CTK_D(const ctkWorkflowWidget);

  if (!d->preTextLabel)
    {
    return 0;
    }

  return d->preTextLabel->text();
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setPreText(const QString& preText)
{
  if (!preText.isEmpty())
    {
    CTK_D(ctkWorkflowWidget);
    if (!d->preTextLabel)
      {
      d->preTextLabel = new QLabel;

      if (!d->preTextSection->layout())
        {
        d->preTextSection->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        }
      d->preTextSection->layout()->addWidget(d->preTextLabel);
      }

    d->preTextLabel->setText(preText);
    }
}

// --------------------------------------------------------------------------
QString ctkWorkflowWidget::postText()const
{
  CTK_D(const ctkWorkflowWidget);

  if (!d->postTextLabel)
    {
    return 0;
    }

  return d->postTextLabel->text();
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setPostText(const QString& postText)
{
  if (!postText.isEmpty())
    {
    CTK_D(ctkWorkflowWidget);
    if (!d->postTextLabel)
      {
      d->postTextLabel = new QLabel;

      if (!d->postTextSection->layout())
        {
        d->postTextSection->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        }
      d->postTextSection->layout()->addWidget(d->postTextLabel);
      }

    d->postTextLabel->setText(postText);
    }
}

// --------------------------------------------------------------------------
QString ctkWorkflowWidget::errorText()const
{
  CTK_D(const ctkWorkflowWidget);

  if (!d->errorLabel)
    {
    return 0;
    }

  return d->errorLabel->text();
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setErrorText(const QString& errorText)
{
  CTK_D(ctkWorkflowWidget);
  if (!d->errorLabel)
    {
    d->errorLabel = new QLabel;

    if (!d->errorSection->layout())
      {
      d->errorSection->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
      }
    d->errorSection->layout()->addWidget(d->errorLabel);
    }

  d->errorLabel->setText("Status: " + errorText);
}
