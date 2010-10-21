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
#include <QString>
#include <QVBoxLayout>
#include <QDebug>
#include <QList>

// CTK includes
#include "ctkWorkflowGroupBox.h"
#include "ctkWorkflowAbstractWidgetStep.h"
#include "ctkFittedTextBrowser.h"
#include "ui_ctkWorkflowGroupBox.h"
#include "ctkLogger.h"

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.widgets.ctkWorkflowGroupBox");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkWorkflowGroupBoxPrivate: public Ui_ctkWorkflowGroupBox
                                  
{
public:
  ctkWorkflowGroupBoxPrivate();
  ~ctkWorkflowGroupBoxPrivate();

  bool HideWidgetsOfNonCurrentSteps;

  ctkWorkflowStep* StepShownPreviously;
  ctkWorkflowStep* StepShownCurrently;

  void setupUi(ctkWorkflowGroupBox* widget);
};

// --------------------------------------------------------------------------
// ctkWorkflowGroupBoxPrivate methods

//---------------------------------------------------------------------------
ctkWorkflowGroupBoxPrivate::ctkWorkflowGroupBoxPrivate()
{
  this->HideWidgetsOfNonCurrentSteps = false;

  this->StepShownPreviously = 0;
  this->StepShownCurrently = 0;
}

//---------------------------------------------------------------------------
ctkWorkflowGroupBoxPrivate::~ctkWorkflowGroupBoxPrivate()
{
}

//---------------------------------------------------------------------------
void ctkWorkflowGroupBoxPrivate::setupUi(ctkWorkflowGroupBox* widget)
{
  this->Ui_ctkWorkflowGroupBox::setupUi(widget);
}

// --------------------------------------------------------------------------
// ctkWorkflowGroupBoxMethods

// --------------------------------------------------------------------------
ctkWorkflowGroupBox::ctkWorkflowGroupBox(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkWorkflowGroupBoxPrivate)
{
  Q_D(ctkWorkflowGroupBox);
  d->setupUi(this);
}

// --------------------------------------------------------------------------
ctkWorkflowGroupBox::~ctkWorkflowGroupBox()
{
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowGroupBox, bool, hideWidgetsOfNonCurrentSteps, HideWidgetsOfNonCurrentSteps);
CTK_SET_CPP(ctkWorkflowGroupBox, bool, setHideWidgetsOfNonCurrentSteps, HideWidgetsOfNonCurrentSteps);
CTK_GET_CPP(ctkWorkflowGroupBox, QLayout*, clientAreaLayout, ClientAreaLayout);

// --------------------------------------------------------------------------
void ctkWorkflowGroupBox::updateGroupBox(ctkWorkflowStep* currentStep)
{
  Q_D(ctkWorkflowGroupBox);

  d->StepShownPreviously = d->StepShownCurrently;
  d->StepShownCurrently = currentStep;

  if (currentStep)
    { 
    this->setTitle(currentStep->name());
    this->setSubTitle(currentStep->description());
    this->setErrorText(currentStep->statusText());

    // don't show textual elements if they are empty
    d->SubTitleTextBrowser->setVisible(!this->subTitle().isEmpty());
    d->PreTextBrowser->setVisible(!this->preText().isEmpty());
    d->PostTextBrowser->setVisible(!this->postText().isEmpty());
    d->ErrorTextBrowser->setVisible(!this->errorText().isEmpty());
    }

  // disable/hide the previously shown step
  if (ctkWorkflowAbstractWidgetStep* prevStep = dynamic_cast<ctkWorkflowAbstractWidgetStep*>(d->StepShownPreviously))
    {
    logger.debug(QString("updateClientArea - hiding %1").arg(prevStep->name()));
    if (QWidget* stepArea = prevStep->stepArea())
      {
      stepArea->setEnabled(false);
      if (d->HideWidgetsOfNonCurrentSteps)
        {
        stepArea->hide();
        }
      }
    }
  
  ctkWorkflowAbstractWidgetStep* currentWidgetStep = dynamic_cast<ctkWorkflowAbstractWidgetStep*>(currentStep);
  // show/enable the current step
  if (currentWidgetStep)
    {
    currentWidgetStep->showUserInterface();

    if (QWidget* stepArea = currentWidgetStep->stepArea())
      {
      // add the step's client area to the widget if we haven't before
      if (!this->isAncestorOf(stepArea))
        {
        d->ClientAreaLayout->addWidget(stepArea);
        }

      stepArea->setEnabled(true);
      stepArea->show();
      }
      }
    
}

// --------------------------------------------------------------------------
QString ctkWorkflowGroupBox::title()const
{
  Q_D(const ctkWorkflowGroupBox);
  return d->CollapsibleButton->text();
}

// --------------------------------------------------------------------------
void ctkWorkflowGroupBox::setTitle(const QString& newTitleText)
{
  Q_D(ctkWorkflowGroupBox);
  d->CollapsibleButton->setText(newTitleText);
}

// --------------------------------------------------------------------------
QString ctkWorkflowGroupBox::subTitle()const
{
  Q_D(const ctkWorkflowGroupBox);
  return d->SubTitleTextBrowser->toPlainText();
}

// --------------------------------------------------------------------------
void ctkWorkflowGroupBox::setSubTitle(const QString& newSubTitleText)
{
  Q_D(ctkWorkflowGroupBox);
  d->SubTitleTextBrowser->setPlainText(newSubTitleText);
}

// --------------------------------------------------------------------------
QString ctkWorkflowGroupBox::preText()const
{
  Q_D(const ctkWorkflowGroupBox);
  return d->PreTextBrowser->toPlainText();
}

// --------------------------------------------------------------------------
void ctkWorkflowGroupBox::setPreText(const QString& newPreText)
{
  Q_D(ctkWorkflowGroupBox);
  d->PreTextBrowser->setPlainText(newPreText);
}

// --------------------------------------------------------------------------
QString ctkWorkflowGroupBox::postText()const
{
  Q_D(const ctkWorkflowGroupBox);
  return d->PostTextBrowser->toPlainText();
}

// --------------------------------------------------------------------------
void ctkWorkflowGroupBox::setPostText(const QString& newPostText)
{
  Q_D(ctkWorkflowGroupBox);
  d->PostTextBrowser->setPlainText(newPostText);
}

// --------------------------------------------------------------------------
QString ctkWorkflowGroupBox::errorText()const
{
  Q_D(const ctkWorkflowGroupBox);
  return d->ErrorTextBrowser->toPlainText();
}

// --------------------------------------------------------------------------
void ctkWorkflowGroupBox::setErrorText(const QString& newErrorText)
{
  Q_D(ctkWorkflowGroupBox);
  d->ErrorTextBrowser->setPlainText(newErrorText);
}
