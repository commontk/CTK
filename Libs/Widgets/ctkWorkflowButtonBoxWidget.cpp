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
#include <QBoxLayout>
#include <QDebug>
#include <QList>
#include <QPointer>
#include <QStyle>
#include <QVariant>

// CTK includes
#include "ctkPushButton.h"
#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflow.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class ctkWorkflowButtonBoxWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkWorkflowButtonBoxWidget);
protected:
  ctkWorkflowButtonBoxWidget* const q_ptr;
public:
  ctkWorkflowButtonBoxWidgetPrivate(ctkWorkflowButtonBoxWidget& object);
  ~ctkWorkflowButtonBoxWidgetPrivate(){}

  void setupUi(QWidget * newParent);

  QPointer<ctkWorkflow>    Workflow;

  // Convenient typedefs
  typedef QMap<ctkPushButton*, ctkWorkflowStep*> ButtonToStepMapType;

  // The buttons on the widget (maintain maps associating each forward/goTo button with its step)
  ctkPushButton*      BackButton;
  QString             BackButtonFormat;
  ctkPushButton*      NextButton;
  QString             NextButtonFormat;
  ButtonToStepMapType GoToButtonToStepMap;
  QString             GoToButtonsFormat;

  // Direction for layout (for use with QBoxLayout only)
  QBoxLayout::Direction Direction;
  QSizePolicy ButtonSizePolicy;

  bool HideGoToButtons;
  bool HideInvalidButtons;

  // Functions to add/remove buttons
  void updateBackButton(ctkWorkflowStep* currentStep);
  void updateNextButton(ctkWorkflowStep* currentStep);
  void updateGoToButtons(ctkWorkflowStep* currentStep);

};

//-----------------------------------------------------------------------------
// ctkWorkflowButtonBoxWidgetPrivate methods

//-----------------------------------------------------------------------------
ctkWorkflowButtonBoxWidgetPrivate::ctkWorkflowButtonBoxWidgetPrivate(ctkWorkflowButtonBoxWidget& object)
  :q_ptr(&object)
{
  this->BackButton = 0;
  this->BackButtonFormat = "[<-]{backButtonText|\"Back\"}(back:description)";
  this->NextButton = 0;
  this->NextButtonFormat = "{nextButtonText|\"Next\"}(next:description)[->]";
  this->GoToButtonsFormat = "[icon]{stepid|\"Finish\"}(description)";
  this->Direction = QBoxLayout::LeftToRight;
  this->ButtonSizePolicy = QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->HideGoToButtons = false;
  this->HideInvalidButtons = false;
}

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidgetPrivate::setupUi(QWidget * newParent)
{
  QBoxLayout * boxLayout = new QBoxLayout(this->Direction, newParent);
  boxLayout->setSpacing(0);
  boxLayout->setContentsMargins(0, 0, 0, 0);
  newParent->setLayout(boxLayout);

  // Retrieve standard icons
  QIcon backIcon = newParent->style()->standardIcon(QStyle::SP_ArrowLeft);
  QIcon nextIcon = newParent->style()->standardIcon(QStyle::SP_ArrowRight);

  // Setup the buttons
  this->BackButton = new ctkPushButton(backIcon, this->BackButtonFormat, newParent);
  this->BackButton->setSizePolicy(this->ButtonSizePolicy);
  this->BackButton->setIconAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  newParent->layout()->addWidget(this->BackButton);

  this->NextButton = new ctkPushButton(nextIcon, this->NextButtonFormat, newParent);
  this->NextButton->setSizePolicy(this->ButtonSizePolicy);
  this->NextButton->setIconAlignment(Qt::AlignRight | Qt::AlignVCenter);
  newParent->layout()->addWidget(this->NextButton);
}

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidgetPrivate::updateBackButton(ctkWorkflowStep* currentStep)
{
#ifndef QT_NO_DEBUG
  Q_Q(ctkWorkflowButtonBoxWidget);

  Q_ASSERT(this->Workflow);
  Q_ASSERT(q->layout());
#endif

  ctkWorkflowWidgetStep* step = dynamic_cast<ctkWorkflowWidgetStep*>(currentStep);

  // Set the back button text and icon
  ctkWorkflowWidget::formatButton(this->BackButton, this->BackButtonFormat, step);

  // Disable the back button if we can't go backward
  bool enable = currentStep && this->Workflow->canGoBackward(currentStep);
  bool visible = true;
  // Apply the buttonBox hints if possible
  if (enable && step)
    {
    enable = !(step->buttonBoxHints() & ctkWorkflowWidgetStep::BackButtonDisabled);
    visible = !(step->buttonBoxHints() & (ctkWorkflowWidgetStep::BackButtonHidden |
                                          ctkWorkflowWidgetStep::ButtonBoxHidden));
    }
  this->BackButton->setEnabled(enable);
  if (!enable && this->HideInvalidButtons)
    {
    visible = false;
    }
  this->BackButton->setVisible(visible);
}

//-----------------------------------------------------------------------------
// This will change for branching workflows, to look more like updateGoToButtons()
void ctkWorkflowButtonBoxWidgetPrivate::updateNextButton(ctkWorkflowStep* currentStep)
{
#ifndef QT_NO_DEBUG
  Q_Q(ctkWorkflowButtonBoxWidget);

  Q_ASSERT(this->Workflow);
  Q_ASSERT(q->layout());
#endif

  ctkWorkflowWidgetStep* step = dynamic_cast<ctkWorkflowWidgetStep*>(currentStep);

  // Set the next button text and icon
  ctkWorkflowWidget::formatButton(this->NextButton, this->NextButtonFormat, step);

  // Disable the next button if we can't go backward
  bool enable = currentStep && this->Workflow->canGoForward(currentStep);
  bool visible = true;
  // Apply the buttonBox hints if possible
  if (enable && step)
    {
    enable = !(step->buttonBoxHints() & ctkWorkflowWidgetStep::NextButtonDisabled);
    visible = !(step->buttonBoxHints() & (ctkWorkflowWidgetStep::NextButtonHidden |
                                          ctkWorkflowWidgetStep::ButtonBoxHidden));
    }
  this->NextButton->setEnabled(enable);
  if (!enable && this->HideInvalidButtons)
    {
    visible = false;
    }
  this->NextButton->setVisible(visible);
}

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidgetPrivate::updateGoToButtons(ctkWorkflowStep* currentStep)
{
  Q_Q(ctkWorkflowButtonBoxWidget);

  Q_ASSERT(this->Workflow);
  Q_ASSERT(q->layout());

  // Change the buttons only if the set of steps to have goTo buttons is either empty or has changed
  QSet<ctkWorkflowStep*> goToStepsToHaveButtons =
    QSet<ctkWorkflowStep*>::fromList(this->Workflow->finishSteps());
  QSet<ctkWorkflowStep*> goToStepsThatHaveButtons =
    QSet<ctkWorkflowStep*>::fromList(this->GoToButtonToStepMap.values());

  // Remove the buttons if the set of steps to have goTo buttons has changed
  if (goToStepsThatHaveButtons != goToStepsToHaveButtons)
    {
    foreach (ctkPushButton* goToButton, this->GoToButtonToStepMap.keys())
      {
      q->layout()->removeWidget(goToButton);
      goToButton->deleteLater();
      }
    this->GoToButtonToStepMap.clear();
    }

  // Create the buttons, either for the first time or after removing them above
  if (this->GoToButtonToStepMap.isEmpty())
    {
    // Create a button for each of the goToSteps
    foreach (ctkWorkflowStep* step, goToStepsToHaveButtons)
      {
      // TODO shouldn't have id here
      ctkPushButton* goToButton = new ctkPushButton();
      goToButton->setSizePolicy(this->ButtonSizePolicy);
      ctkWorkflowWidgetStep* wwStep = dynamic_cast<ctkWorkflowWidgetStep*>(step);
      ctkWorkflowWidget::formatButton(goToButton, this->GoToButtonsFormat, wwStep);
      q->layout()->addWidget(goToButton);
      QObject::connect(goToButton, SIGNAL(clicked()), q, SLOT(prepareGoToStep()));
      this->GoToButtonToStepMap[goToButton] = step;
      }
    }

  // Show/hide the goTo buttons depending on whether they are accessible from the current step
  ctkWorkflowWidgetStep* step = dynamic_cast<ctkWorkflowWidgetStep*>(currentStep);
  foreach (ctkPushButton* goToButton, this->GoToButtonToStepMap.keys())
    {
    // TODO enable and show the goTo button if we can go to it
    // ctkWorkflowStep* goToStep = this->GoToButtonToStepMap[goToButton];
    // if (this->Workflow->canGoToStep(currentStep, goToStep))
    // for now we'll assume we can go to the step
    ctkWorkflowStep* goToStep = this->GoToButtonToStepMap[goToButton];
    Q_ASSERT(goToStep);
    bool enable = currentStep && this->Workflow->canGoToStep(goToStep->id(), currentStep);
    bool visible = step ? !(step->buttonBoxHints() & ctkWorkflowWidgetStep::ButtonBoxHidden) : true;
    if ((!enable && this->HideInvalidButtons)
        || this->HideGoToButtons
        )
      {
      visible = false;
      }
    goToButton->setEnabled(enable);
    goToButton->setVisible(visible);
    }
}

//-----------------------------------------------------------------------------
// ctkWorkflowButtonBoxWidget methods

//-----------------------------------------------------------------------------
ctkWorkflowButtonBoxWidget::ctkWorkflowButtonBoxWidget(ctkWorkflow* newWorkflow, QWidget* newParent) :
    Superclass(newParent)
  , d_ptr(new ctkWorkflowButtonBoxWidgetPrivate(*this))
{
  Q_D(ctkWorkflowButtonBoxWidget);

  d->setupUi(this);

  this->setWorkflow(newWorkflow);
}

//-----------------------------------------------------------------------------
ctkWorkflowButtonBoxWidget::ctkWorkflowButtonBoxWidget(QWidget* newParent) :
    Superclass(newParent)
  , d_ptr(new ctkWorkflowButtonBoxWidgetPrivate(*this))
{
  Q_D(ctkWorkflowButtonBoxWidget);

  d->setupUi(this);
}

//-----------------------------------------------------------------------------
ctkWorkflowButtonBoxWidget::~ctkWorkflowButtonBoxWidget()
{
}

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidget::setWorkflow(ctkWorkflow * newWorkflow)
{
  Q_D(ctkWorkflowButtonBoxWidget);

  if (d->Workflow == newWorkflow)
    {
    return;
    }

  // Disconnect
  if (!d->Workflow.isNull())
    {
    QObject::disconnect(d->BackButton, SIGNAL(clicked()), d->Workflow, SLOT(goBackward()));
    QObject::disconnect(d->NextButton, SIGNAL(clicked()), d->Workflow, SLOT(goForward()));
    }

  // Connect
  if (newWorkflow)
    {
    QObject::connect(d->BackButton, SIGNAL(clicked()), newWorkflow, SLOT(goBackward()));
    QObject::connect(d->NextButton, SIGNAL(clicked()), newWorkflow, SLOT(goForward()));
    }

  d->Workflow = newWorkflow;
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowButtonBoxWidget, QString, backButtonFormat,
            BackButtonFormat);

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidget::setBackButtonFormat(const QString& format)
{
  Q_D(ctkWorkflowButtonBoxWidget);
  d->BackButtonFormat = format;
  d->updateBackButton(d->Workflow ? d->Workflow->currentStep() : 0);
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowButtonBoxWidget, QString, nextButtonFormat,
            NextButtonFormat);

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidget::setNextButtonFormat(const QString& format)
{
  Q_D(ctkWorkflowButtonBoxWidget);
  d->NextButtonFormat = format;
  d->updateNextButton(d->Workflow ? d->Workflow->currentStep() : 0);
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowButtonBoxWidget, QString, goToButtonsFormat,
            GoToButtonsFormat);

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidget::setGoToButtonsFormat(const QString& format)
{
  Q_D(ctkWorkflowButtonBoxWidget);
  d->GoToButtonsFormat = format;
  d->updateGoToButtons(d->Workflow ? d->Workflow->currentStep() : 0);
}

//-----------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowButtonBoxWidget, ctkWorkflow*, workflow, Workflow);
CTK_GET_CPP(ctkWorkflowButtonBoxWidget, ctkPushButton*, backButton, BackButton);
CTK_GET_CPP(ctkWorkflowButtonBoxWidget, QBoxLayout::Direction, direction, Direction);
CTK_GET_CPP(ctkWorkflowButtonBoxWidget, bool, hideGoToButtons, HideGoToButtons);
CTK_SET_CPP(ctkWorkflowButtonBoxWidget, bool, setHideGoToButtons, HideGoToButtons);
CTK_GET_CPP(ctkWorkflowButtonBoxWidget, bool, hideInvalidButtons, HideInvalidButtons);
CTK_SET_CPP(ctkWorkflowButtonBoxWidget, bool, setHideInvalidButtons, HideInvalidButtons);

//-----------------------------------------------------------------------------
// TODO will be list of next buttons for branching workflow
ctkPushButton* ctkWorkflowButtonBoxWidget::nextButton()const
{
  Q_D(const ctkWorkflowButtonBoxWidget);
  return d->NextButton;
}

//-----------------------------------------------------------------------------
QList<ctkPushButton*> ctkWorkflowButtonBoxWidget::goToButtons()const
{
  Q_D(const ctkWorkflowButtonBoxWidget);
  return d->GoToButtonToStepMap.keys();
}

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidget::setDirection(const QBoxLayout::Direction& newDirection)
{
  if (QBoxLayout* layout = qobject_cast<QBoxLayout*>(this->layout()))
    {
    layout->setDirection(newDirection);
    }
}

//-----------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidget::updateButtons(ctkWorkflowStep* currentStep)
{
  Q_D(ctkWorkflowButtonBoxWidget);
  d->updateBackButton(currentStep);
  d->updateNextButton(currentStep);
  d->updateGoToButtons(currentStep);
}

//--------------------------------------------------------------------------
void ctkWorkflowButtonBoxWidget::prepareGoToStep()
{
  Q_D(ctkWorkflowButtonBoxWidget);
  if (ctkPushButton* button = qobject_cast<ctkPushButton*>(QObject::sender()))
    {
    if (ctkWorkflowStep* step = d->GoToButtonToStepMap.value(button))
      {
      d->Workflow->goToStep(step->id());
      }
    }
}
