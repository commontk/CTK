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

// QT includes
#include <QApplication>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QStyle>
#include <QDebug>

// CTK includes
#include "ctkPushButton.h"
#include "ctkWorkflow.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflowStackedWidget.h"
#include "ctkWorkflowTabWidget.h"
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowGroupBox.h"
#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
bool buttonClickTestSignalSlot(QApplication& app, int defaultTime, ctkWorkflowWidgetStep* currentStep, QWidget* shownStepArea, QLineEdit* shownLineEdit, QLabel* shownLabel, QWidget* hiddenStepArea, QLineEdit* hiddenLineEdit, QLabel* hiddenLabel, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, QPushButton* backButton, QPushButton* nextButton, QPushButton* finishButton1=0, QPushButton* finishButton2=0)
{
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // // ensure we are in the correct step
  if (workflow->currentStep() != currentStep)
    {
    std::cerr << "In incorrect step" << std::endl;
    return false;
    }

  // ensure that the widgets of the current step are visible
  if ((currentStep && (!shownStepArea || !shownStepArea->isVisible()))
      || (shownLabel && !shownLabel->isVisible())
      || (shownLineEdit && !shownLineEdit->isVisible())
      || !backButton->isVisible()
      || !nextButton->isVisible()
      || (finishButton1 && !finishButton1->isVisible())
      || (finishButton2 && !finishButton2->isVisible()))
    {
    std::cerr << "Incorrect widget visibility - the current step's widgets are invisible" << std::endl;
    return false;
    }

  // ensure that buttons are appropriately enabled
  // TODO finish buttons
  if ((workflow->canGoBackward() != backButton->isEnabled()) || (workflow->canGoForward() != nextButton->isEnabled()) || (shownLineEdit && !shownLineEdit->isEnabled()))
    {
    std::cerr << "Incorrect widget visibility - the buttons are incorrectly enabled" << std::endl;
    return false;
    }

  // ensure that the currentStep step's name and description are shown in
  // the widget
  ctkWorkflowGroupBox* groupBox;
  if (ctkWorkflowAbstractPagedWidget* pagedWidget = qobject_cast<ctkWorkflowAbstractPagedWidget*>(workflowWidget))
    {
    groupBox = pagedWidget->workflowGroupBox(currentStep);
    }
  else
    {
    groupBox = workflowWidget->workflowGroupBox();
    }
  Q_ASSERT(groupBox);

  if (currentStep->name() != groupBox->title() || currentStep->description() != groupBox->subTitle())
    {
    std::cerr << "Incorrect widget title/subtitle" << std::endl;
    return false;
    }

  // ensure that the finish button has an icon
  if ((finishButton1 && finishButton1->icon().isNull()) || (finishButton2 && finishButton2->icon().isNull()))
    {
    std::cerr << "Incorrect finish icon visibility" << std::endl;
    return false;
    }

  // ensure that widgets of the other step are either invisible, or
  // visible but disabled
  if (hiddenStepArea)
    {
    if (hiddenStepArea->isVisible() && groupBox->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's stepArea is showing" << std::endl;
      return false;
      }
    else if (hiddenStepArea->isVisible() && hiddenStepArea->isEnabled())
      {
      std::cerr << "Incorrect widget visibility - the other step's stepArea is enabled" << std::endl;
      return false;
      }
    }
  if (hiddenLabel)
    {
    if (hiddenLabel->isVisible() && groupBox->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's label is showing" << std::endl;
      return false;
      }
    }

  if (hiddenLineEdit)
    {
    if (hiddenLineEdit->isVisible() && groupBox->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's lineEdit is showing" << std::endl;
      return false;
      }
    else if (hiddenLineEdit->isVisible() && hiddenLineEdit->isEnabled())
      {
      std::cerr << "Incorrect widget visibility - the other step's lineEdit is enabled" << std::endl;
      return false;
      }
    }

  return true;
}

//-----------------------------------------------------------------------------
struct signalSlotTestData
{
  QPushButton* buttonToClick;
  ctkWorkflowWidgetStep* currentStep;
  ctkWorkflowWidgetStep* hiddenStep;
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* currentQObject;
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* hiddenQObject;
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObjectToChangeLineEdit;
  QString lineEditText;

  signalSlotTestData(QPushButton* newButtonToClick, ctkWorkflowWidgetStep* newCurrentStep, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* newCurrentQObject, ctkWorkflowWidgetStep* newHiddenStep, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* newHiddenQObject, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* newQObjectToChangeLineEdit=0, QString newLineEditText="")
  {
    this->buttonToClick = newButtonToClick;
    this->currentStep = newCurrentStep;
    this->currentQObject = newCurrentQObject;
    this->hiddenStep = newHiddenStep;
    this->hiddenQObject = newHiddenQObject;
    this->qObjectToChangeLineEdit = newQObjectToChangeLineEdit;
    this->lineEditText = newLineEditText;
  }

  bool runTest(QApplication& app, int defaultTime, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, QPushButton* backButton, QPushButton* nextButton, QPushButton* finishButton1=0, QPushButton* finishButton2=0)
  {
    if (this->qObjectToChangeLineEdit)
      {
      this->qObjectToChangeLineEdit->lineEdit()->setText(lineEditText);
      }
    if (this->buttonToClick)
      {
      this->buttonToClick->click();
      }

    if (this->currentStep)
      {
      return buttonClickTestSignalSlot(app, defaultTime, this->currentStep, this->currentStep->stepArea(), this->currentQObject->lineEdit(), this->currentQObject->label(), this->hiddenStep->stepArea(), this->hiddenQObject->lineEdit(), this->hiddenQObject->label(), workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2);
      }
    else
      {
      return buttonClickTestSignalSlot(app, defaultTime, this->currentStep, 0, 0, 0, this->hiddenStep->stepArea(), this->hiddenQObject->lineEdit(), this->hiddenQObject->label(), workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2);
      }
  }
};

//-----------------------------------------------------------------------------
// simulates valid and invalid user interaction for a workflow with
// two steps
int userInteractionSimulator1(QApplication& app, ctkWorkflowWidgetStep* step1, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject1, ctkWorkflowWidgetStep* step2, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject2, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
{

  QPushButton* backButton = workflowWidget->buttonBoxWidget()->backButton();
  QPushButton* nextButton = workflowWidget->buttonBoxWidget()->nextButton();
  Q_ASSERT(backButton);
  Q_ASSERT(nextButton);

  QList<signalSlotTestData*> tests;

  // we should be in the first step at the start of the workflow
  tests << new signalSlotTestData(0, step1, qObject1, step2, qObject2);

  // tests with good input (lineEdit value = 100, which passes the
  // condition that it be >= 10)

  // CurrentStep  ButtonPress  ExpectedStep  Shouldn'tDoAnything
  // step1        next         step2
  // step2        back         step1
  // step1        next         step2
  // step2        back         step1

  tests << new signalSlotTestData(nextButton, step2, qObject2, step1, qObject1)
        << new signalSlotTestData(backButton, step1, qObject1, step2, qObject2)
        << new signalSlotTestData(nextButton, step2, qObject2, step1, qObject1)
        << new signalSlotTestData(backButton, step1, qObject1, step2, qObject2);

  // tests with both good and bad input (lineEdit value may be
  // invalid)

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step1        next         *             step1
  // step1        next                       step2
  // step2        back         *             step1
  // step1        next         * (empty)     step1
  // step1        next                       step2
  tests << new signalSlotTestData(nextButton, step1, qObject1, step2, qObject2, qObject1, "1")
        << new signalSlotTestData(nextButton, step2, qObject2, step1, qObject1, qObject1, "100")
        << new signalSlotTestData(backButton, step1, qObject1, step2, qObject2)
        << new signalSlotTestData(nextButton, step1, qObject1, step2, qObject2, qObject1, "")
        << new signalSlotTestData(nextButton, step2, qObject2, step1, qObject1, qObject1, "100");

  foreach (signalSlotTestData* test, tests)
    {
    if (!test->runTest(app, defaultTime, workflow, workflowWidget, backButton, nextButton))
      {
      return EXIT_FAILURE;
      }
    }

  // TODO
  // // after adding the steps, then the widget's client area should have
  // // them as a child
  // if (!workflowWidget->clientArea()->isAncestorOf(step1->stepArea()))
  //   {
  //   std::cerr << "step1 was incorrectly added to the workflowWidget" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  // if (!workflowWidget->clientArea()->isAncestorOf(step2->stepArea()))
  //   {
  //   std::cerr << "step2 was incorrectly added to the workflowWidget" << std::endl;
  //   return EXIT_FAILURE;
  //   }

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
// simulates valid and invalid user interaction for a workflow with
// three steps
int userInteractionSimulator2(QApplication& app, ctkWorkflowWidgetStep* step1, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject1, ctkWorkflowWidgetStep* step2, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject2, ctkWorkflowWidgetStep* step3, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject3, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
{
  // ensure we can get the pages/layouts we may want
  if (ctkWorkflowAbstractPagedWidget* pagedWidget = qobject_cast<ctkWorkflowAbstractPagedWidget*>(workflowWidget))
    {
    if (!pagedWidget->workflowGroupBox(step1))
      {
      std::cerr << "could not access widget for page 1" << std::endl;
      return EXIT_FAILURE;
      }
    if (!pagedWidget->workflowGroupBox(step1)->clientAreaLayout())
      {
      std::cerr << "could not access client area layout for page 1" << std::endl;
      return EXIT_FAILURE;
      }
    }
  else
    {
    if (!workflowWidget->workflowGroupBox()->clientAreaLayout())
      {
      std::cerr << "could not access client area layout" << std::endl;
      return EXIT_FAILURE;
      }
    }

  QPushButton* backButton = workflowWidget->buttonBoxWidget()->backButton();
  QPushButton* nextButton = workflowWidget->buttonBoxWidget()->nextButton();
  QPushButton* finishButton = workflowWidget->buttonBoxWidget()->goToButtons().first();
  Q_ASSERT(backButton);
  Q_ASSERT(nextButton);
  Q_ASSERT(finishButton);

  QList<signalSlotTestData*> tests;

  // we should be in the first step at the start of the workflow
  tests << new signalSlotTestData(0, step1, qObject1, step2, qObject2);

  // tests with good input (lineEdit value = 100, which passes the
  // condition that it be >= 10)

  // CurrentStep  ButtonPress  ExpectedStep  Shouldn'tDoAnything
  // step1        next         step2
  // step2        next         step3
  // step3        back         step2
  // step2        back         step1
  // step1        next         step2
  // step2        next         step3
  // step3        back         step2

  tests << new signalSlotTestData(nextButton, step2, qObject2, step1, qObject1)
        << new signalSlotTestData(nextButton, step3, qObject3, step2, qObject2, qObject2, "100")
        << new signalSlotTestData(backButton, step2, qObject2, step3, qObject3)
        << new signalSlotTestData(backButton, step1, qObject1, step2, qObject2)
        << new signalSlotTestData(nextButton, step2, qObject2, step1, qObject1)
        << new signalSlotTestData(nextButton, step3, qObject3, step2, qObject2)
        << new signalSlotTestData(backButton, step2, qObject2, step3, qObject3);

  // tests with both good and bad input (lineEdit value may be
  // invalid)

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step2        next         *             step2
  // step2        next                       step3
  // step3        back         *             step2
  // step2        next         * (empty)     step2
  // step2        next                       step3

  tests << new signalSlotTestData(nextButton, step2, qObject2, step3, qObject3, qObject2, "1")
        << new signalSlotTestData(nextButton, step3, qObject3, step2, qObject2, qObject2, "100")
        << new signalSlotTestData(backButton, step2, qObject2, step3, qObject3)
        << new signalSlotTestData(nextButton, step2, qObject2, step3, qObject3, qObject2, "")
        << new signalSlotTestData(nextButton, step3, qObject3, step2, qObject2, qObject2, "100");

  // first go back to the beginning
  tests << new signalSlotTestData(backButton, step2, qObject2, step3, qObject3, qObject2, "100")
        << new signalSlotTestData(backButton, step1, qObject1, step2, qObject2);

  // tests going to the finish step

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step1        finish                     step1
  // step1        finish                     step1
  // step1        next                       step2
  // step2        finish                     step2
  // step2        next                       step3
  // step3        finish                     step3
  // step3        back                       step2
  // step2        finish                     step2
  // step2        back                       step1
  // step1        finish                     step1

  tests << new signalSlotTestData(finishButton, step1, qObject1, step3, qObject3)
        << new signalSlotTestData(finishButton, step1, qObject1, step3, qObject3)
        << new signalSlotTestData(nextButton, step2, qObject2, step1, qObject1)
        << new signalSlotTestData(finishButton, step2, qObject2, step3, qObject3)
        << new signalSlotTestData(nextButton, step3, qObject3, step2, qObject2)
        << new signalSlotTestData(finishButton, step3, qObject3, step2, qObject2)
        << new signalSlotTestData(backButton, step2, qObject2, step3, qObject3)
        << new signalSlotTestData(finishButton, step2, qObject2, step3, qObject3)
        << new signalSlotTestData(backButton, step1, qObject1, step2, qObject2)
        << new signalSlotTestData(finishButton, step1, qObject1, step3, qObject3);

  // tests going to the finish step with invalid input

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step1        finish       * (step2, qObject2)     step2
  // step2        next                       step3
  // step3        back                       step2
  // step2        finish       * (step2)     step2
  // step2        back                       step1
  // step1        finish       * (step3)     step1

  tests << new signalSlotTestData(finishButton, step2, qObject2, step1, qObject1, qObject2, "0")
        << new signalSlotTestData(nextButton, step3, qObject3, step2, qObject2, qObject2, "100")
        << new signalSlotTestData(backButton, step2, qObject2, step3, qObject3)
        << new signalSlotTestData(finishButton, step2, qObject2, step3, qObject3, qObject2, "0")
        << new signalSlotTestData(backButton, step1, qObject1, step2, qObject2, qObject2, "100") // reset text for step2, qObject2
        << new signalSlotTestData(finishButton, step1, qObject1, step3, qObject3, qObject3, "0");

  foreach (signalSlotTestData* test, tests)
    {
    if (!test->runTest(app, defaultTime, workflow, workflowWidget, backButton, nextButton, finishButton))
      {
      return EXIT_FAILURE;
      }
    }


  // TODO
  // // after adding the steps, then the widget's client area should have
  // // them as a child
  // if (!workflowWidget->clientArea()->isAncestorOf(step3->stepArea()))
  //   {
  //   std::cerr << "step3 was incorrectly added to the workflowWidget" << std::endl;
  //   return EXIT_FAILURE;
  //   }

  return EXIT_SUCCESS;
}

// //-----------------------------------------------------------------------------
// // simulates valid and invalid user interaction for a workflow with
// // three steps and two finish steps
// int userInteractionSimulator3(QApplication& app, ctkWorkflowWidgetStep* step1, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject1, ctkWorkflowWidgetStep* step2, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject2, ctkWorkflowWidgetStep* step3, ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject3, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
// {

//   int BACK_BUTTON_INDEX = 0;
//   int NEXT_BUTTON_INDEX = 1;
//   int FINISH_BUTTON_INDEX_1 = 2;
//   int FINISH_BUTTON_INDEX_2 = 3;

//   // get the buttons
//   QPushButton* step1BackButton = 0;
//   QPushButton* step1NextButton = 0;
//   QPushButton* step1FinishButton1 = 0;
//   QPushButton* step1FinishButton2 = 0;
//   QPushButton* step2BackButton = 0;
//   QPushButton* step2NextButton = 0;
//   QPushButton* step2FinishButton1 = 0;
//   QPushButton* step2FinishButton2 = 0;
//   QPushButton* step3BackButton = 0;
//   QPushButton* step3NextButton = 0;
//   QPushButton* step3FinishButton1 = 0;
//   QPushButton* step3FinishButton2 = 0;

//   // get the back/next buttons for the first step
//   QList<QPushButton*> buttons = step1->stepArea()->findChildren<QPushButton*>();
//   // if (buttons.length() != 3)
//   //   {
//   //   std::cerr << "incorrect number of buttons for step1" << std::endl;
//   //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 3" << std::endl;
//   //   return EXIT_FAILURE;
//   //   }
//   step1NextButton = buttons.at(NEXT_BUTTON_INDEX);
//   step1FinishButton1 = buttons.at(FINISH_BUTTON_INDEX_1);
//   step1FinishButton2 = buttons.at(FINISH_BUTTON_INDEX_2);
//   if (!step1NextButton || !step1FinishButton1 || !step1FinishButton2 || step1NextButton->text() != "Next" || step1FinishButton1->text() != "finish" || step1FinishButton2->text() != "finish")
//     {
//     std::cerr << "incorrect button assignment for step1" << std::endl;
//     return EXIT_FAILURE;
//     }

//   // we should be in the first step
//   if (!buttonClickTestSignalSlot(app, defaultTime, step1, qObject1, step2, qObject2, workflow, workflowWidget, step1BackButton, step1NextButton, step2BackButton, step2NextButton, step1FinishButton1, step2FinishButton1, step1FinishButton2, step2FinishButton2)) {return EXIT_FAILURE;}

//   // tests with good input, so that we can get all of the buttons
//   // (lineEdit value = 100, which passes the condition that it be >= 10)

//   // CurrentStep  ButtonPress  ExpectedStep  Shouldn'tDoAnything
//   // step1        next         step2
//   // step2        next         step3

//   // normal forwards/backwards transitions
//   step1NextButton->click();
//   QTimer::singleShot(defaultTime, &app, SLOT(quit()));
//   app.exec();

//   // get the back/next buttons for the second step
//   buttons = step2->stepArea()->findChildren<QPushButton*>();
//   // if (buttons.length() != 4)
//   //   {
//   //   std::cerr << "incorrect number of buttons for step2" << std::endl;
//   //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 4" << std::endl;
//   //   return EXIT_FAILURE;
//   //   }
//   step2BackButton = buttons.at(BACK_BUTTON_INDEX);
//   step2NextButton = buttons.at(NEXT_BUTTON_INDEX);
//   step2FinishButton1 = buttons.at(FINISH_BUTTON_INDEX_1);
//   step2FinishButton2 = buttons.at(FINISH_BUTTON_INDEX_2);
//   if (!step2BackButton || !step2NextButton || !step2FinishButton1 || !step2FinishButton2 || step2BackButton->text() != "Back" || step2NextButton->text() != "Next" || step2FinishButton1->text() != "finish" || step2FinishButton2->text() != "finish")
//     {
//     std::cerr << "incorrect button assignment for step2" << std::endl;
//     return EXIT_FAILURE;
//     }
//   if (!buttonClickTestSignalSlot(app, defaultTime, step2, qObject2, step1, qObject1, workflow, workflowWidget, step2BackButton, step2NextButton, step1BackButton, step1NextButton, step2FinishButton1, step1FinishButton1, step2FinishButton2, step1FinishButton2)) {return EXIT_FAILURE;}

//   step2NextButton->click();
//   QTimer::singleShot(defaultTime, &app, SLOT(quit()));
//   app.exec();

//   // get the back/next buttons for the third step
//   buttons = step3->stepArea()->findChildren<QPushButton*>();
//   // if (buttons.length() != 3)
//   //   {
//   //   std::cerr << "incorrect number of buttons for step3" << std::endl;
//   //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 3" << std::endl;
//   //   return EXIT_FAILURE;
//   //   }
//   step3BackButton = buttons.at(BACK_BUTTON_INDEX);
//   step3FinishButton1 = buttons.at(FINISH_BUTTON_INDEX_1);
//   step3FinishButton2 = buttons.at(FINISH_BUTTON_INDEX_2);
//   if (!step3BackButton || !step3FinishButton1 || !step3FinishButton2 || step3BackButton->text() != "Back" || step3FinishButton1->text() != "finish" || step3FinishButton2->text() != "finish")
//     {
//     std::cerr << "incorrect button assignment for step3" << std::endl;
//     return EXIT_FAILURE;
//     }
//   if (!buttonClickTestSignalSlot(app, defaultTime, step3, qObject3, step2, qObject2, workflow, workflowWidget, step3BackButton, step3NextButton, step2BackButton, step2NextButton, step3FinishButton1, step2FinishButton1, step3FinishButton2, step2FinishButton2)) {return EXIT_FAILURE;}

//   // tests going to the finish step

//   // CurrentStep  ButtonPress  Invalid input ExpectedStep
//   // step3        back                       step2
//   // step2        back                       step1
//   // step1        finish1                    step1
//   // step1        next                       step2
//   // step2        finish1                    step2
//   // step2        finish2                    step2
//   // step2        back                       step1
//   // step1        finish2                    step1
//   // step1        next                       step2

//   step3BackButton->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step2, qObject2, step3, qObject3, workflow, workflowWidget, step2BackButton, step2NextButton, step3BackButton, step3NextButton, step2FinishButton1, step3FinishButton1, step2FinishButton2, step3FinishButton2)) {return EXIT_FAILURE;}

//   step2BackButton->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step1, qObject1, step2, qObject2, workflow, workflowWidget, step1BackButton, step1NextButton, step2BackButton, step2NextButton, step1FinishButton1, step2FinishButton1, step1FinishButton2, step2FinishButton2)) {return EXIT_FAILURE;}

//   step1FinishButton1->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step1, qObject1, step3, qObject3, workflow, workflowWidget, step1BackButton, step1NextButton, step3BackButton, step3BackButton, step1FinishButton1, step3FinishButton1, step1FinishButton2, step3FinishButton2)) {return EXIT_FAILURE;}

//   step1NextButton->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step2, qObject2, step1, qObject1, workflow, workflowWidget, step2BackButton, step2NextButton, step1BackButton, step1BackButton, step2FinishButton1, step1FinishButton1, step2FinishButton2, step1FinishButton2)) {return EXIT_FAILURE;}

//   step2FinishButton1->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step2, qObject2, step3, qObject3, workflow, workflowWidget, step2BackButton, step2NextButton, step3BackButton, step3NextButton, step2FinishButton1, step3FinishButton1, step2FinishButton2, step3FinishButton2)) {return EXIT_FAILURE;}

//   step2FinishButton2->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step2, qObject2, step3, qObject3, workflow, workflowWidget, step2BackButton, step2NextButton, step3BackButton, step3NextButton, step2FinishButton1, step3FinishButton1, step2FinishButton2, step3FinishButton2)) {return EXIT_FAILURE;}

//   step2BackButton->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step1, qObject1, step2, qObject2, workflow, workflowWidget, step1BackButton, step1NextButton, step2BackButton, step2NextButton, step1FinishButton1, step2FinishButton1, step1FinishButton2, step2FinishButton2)) {return EXIT_FAILURE;}

//   step1FinishButton2->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step1, qObject1, step2, qObject2, workflow, workflowWidget, step1BackButton, step1NextButton, step2BackButton, step2BackButton, step1FinishButton1, step2FinishButton1, step1FinishButton2, step2FinishButton2)) {return EXIT_FAILURE;}

//   step1NextButton->click();
//   if (!buttonClickTestSignalSlot(app, defaultTime, step2, qObject2, step1, qObject1, workflow, workflowWidget, step2BackButton, step2NextButton, step1BackButton, step1BackButton, step2FinishButton1, step1FinishButton1, step2FinishButton2, step1FinishButton2)) {return EXIT_FAILURE;}

//   return EXIT_SUCCESS;
// }

//-----------------------------------------------------------------------------
int ctkWorkflowWidgetTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  int defaultTime = 100;
  bool hideWidgets = false;

  ctkWorkflow* workflow = new ctkWorkflow;
  ctkWorkflowWidget* workflowWidget = new ctkWorkflowWidget;
  workflowWidget->setWorkflow(workflow);

  ctkWorkflowGroupBox* groupBox = workflowWidget->workflowGroupBox();
  groupBox->setPreText("I am some pre-text");
  groupBox->setPostText("I am some post-text");
  groupBox->setHideWidgetsOfNonCurrentSteps(hideWidgets);

  // create and add the first workflow step (depends on workflowWidget
  // type)
  ctkWorkflowWidgetStep* step1 = new ctkWorkflowWidgetStep("Step 1");
  step1->setName("Step 1");
  step1->setDescription("I am in step 1");
  if (ctkWorkflowTabWidget* tabWidget = qobject_cast<ctkWorkflowTabWidget*>(workflowWidget))
    {
    tabWidget->associateStepWithLabel(step1, "tab1");
    }

  // step1 is the initial step
  workflow->setInitialStep(step1);

  // create and add the second workflow step (depends on
  // workflowWidget type)
  ctkWorkflowWidgetStep* step2 = new ctkWorkflowWidgetStep("Step 2");
  step2->setName("Step 2");
  step2->setDescription("I am in step 2");
  if (ctkWorkflowTabWidget* tabWidget = qobject_cast<ctkWorkflowTabWidget*>(workflowWidget))
    {
    tabWidget->associateStepWithLabel(step2, "tab2");
    }

  // add the steps to the workflow
  workflow->addTransition(step1, step2);

  // create the qObjects that implement the required functions, and
  // communicate with the workflow using signals and slots
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject1 =
      new ctkExampleWorkflowWidgetStepUsingSignalsAndSlots(step1);
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject2 =
      new ctkExampleWorkflowWidgetStepUsingSignalsAndSlots(step2);

  // set the widget for each qObject
  qObject1->setWidget(step1->stepArea());
  qObject2->setWidget(step2->stepArea());

  // use the qObjects for validation
  QObject::connect(step1->ctkWorkflowStepQObject(),
                   SIGNAL(invokeValidateCommand(QString)),
                   qObject1, SLOT(validate(QString)));
  QObject::connect(step2->ctkWorkflowStepQObject(),
                   SIGNAL(invokeValidateCommand(QString)),
                   qObject2, SLOT(validate(QString)));

  // use the qObjects for entry processing
  QObject::connect(step1->ctkWorkflowStepQObject(),
                   SIGNAL(invokeOnEntryCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)),
                   qObject1, SLOT(onEntry(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));
  QObject::connect(step2->ctkWorkflowStepQObject(),
                   SIGNAL(invokeOnEntryCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)),
                   qObject2, SLOT(onEntry(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));

  // use the qObjects for exit processing
  QObject::connect(step1->ctkWorkflowStepQObject(),
                   SIGNAL(invokeOnExitCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)),
                   qObject1, SLOT(onExit(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));
  QObject::connect(step2->ctkWorkflowStepQObject(),
                   SIGNAL(invokeOnExitCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)),
                   qObject2, SLOT(onExit(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));

  // use the qObjects for populating the stepWidgetsList
  QObject::connect(step1->ctkWorkflowStepQObject(),
                   SIGNAL(invokeCreateUserInterfaceCommand()),
                   qObject1, SLOT(createUserInterface()));
  QObject::connect(step2->ctkWorkflowStepQObject(),
                   SIGNAL(invokeCreateUserInterfaceCommand()),
                   qObject2, SLOT(createUserInterface()));

  step1->setHasValidateCommand(1);
  step2->setHasValidateCommand(1);

  step1->setHasOnEntryCommand(1);
  step2->setHasOnEntryCommand(1);

  step1->setHasOnExitCommand(1);
  step2->setHasOnExitCommand(1);

  step1->setHasCreateUserInterfaceCommand(1);
  step2->setHasCreateUserInterfaceCommand(1);

  // start the workflow
  workflow->start();
  workflowWidget->show();

  // first user interaction test
  if (userInteractionSimulator1(app, step1, qObject1, step2, qObject2, workflow, workflowWidget, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  // stop the workflow
  workflow->stop();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // create and add a third workflow step (depends on workflowWidget
  // type)
  ctkWorkflowWidgetStep* step3 = new ctkWorkflowWidgetStep("Step 3");
  step3->setName("Step 3");
  step3->setDescription("I am in step 3");
  if (ctkWorkflowStackedWidget* stackedWidget = qobject_cast<ctkWorkflowStackedWidget*>(workflowWidget))
    {
    stackedWidget->associateStepWithPage(step3, 1);
    }
  else if (ctkWorkflowTabWidget* tabWidget = qobject_cast<ctkWorkflowTabWidget*>(workflowWidget))
    {
    tabWidget->associateStepWithPage(step3, 1, "tab2");
    }

  // icon test - add an icon to step3, should show up as a icon on the finish button
  step3->setIcon(step3->stepArea()->style()->standardIcon(QStyle::SP_ArrowUp));

  // add the step to the workflow
  workflow->addTransition(step2, step3);

  // create the qObjects that implement the required functions, and
  // communicate with the workflow using signals and slots
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject3 =
      new ctkExampleWorkflowWidgetStepUsingSignalsAndSlots(step3);

  qObject3->setWidget(step3->stepArea());

  // use the qObjects for validation
  QObject::connect(step3->ctkWorkflowStepQObject(), SIGNAL(invokeValidateCommand(QString)),
                   qObject3, SLOT(validate(QString)));

  // use the qObjects for entry processing
  QObject::connect(step3->ctkWorkflowStepQObject(), SIGNAL(invokeOnEntryCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)),
                   qObject3, SLOT(onEntry(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));

  // use the qObjects for exit processing
  QObject::connect(step3->ctkWorkflowStepQObject(), SIGNAL(invokeOnExitCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)),
                   qObject3, SLOT(onExit(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));

  // use the qObjects for populating the stepWidgetsList
  QObject::connect(step3->ctkWorkflowStepQObject(), SIGNAL(invokeCreateUserInterfaceCommand()), qObject3, SLOT(createUserInterface()));
  QObject::connect(qObject3, SIGNAL(createUserInterfaceComplete()), step3->ctkWorkflowStepQObject(), SIGNAL(createUserInterfaceComplete()));

  step3->setHasValidateCommand(1);
  step3->setHasOnEntryCommand(1);
  step3->setHasOnExitCommand(1);
  step3->setHasCreateUserInterfaceCommand(1);

  // restart the workflow
  workflow->start();

  // second user interaction test
  if (userInteractionSimulator2(app, step1, qObject1, step2, qObject2, step3, qObject3, workflow, workflowWidget, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  // stop the workflow
  workflow->stop();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // make the second workflow step a finish step as well
  // TODO put this back once we can add multiple finish steps
//   finishSteps.push_back(step2);
//   step1->setFinishStepsToHaveButtonsInStepArea(finishSteps);
//   step2->setFinishStepsToHaveButtonsInStepArea(finishSteps);
//   step3->setFinishStepsToHaveButtonsInStepArea(finishSteps);
// //  workflow->addFinishStep(step2);

  // // restart the workflow
  // workflow->start();
  // QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  // app.exec();

  // // third user interfaction test
  // if (userInteractionSimulator3(app, step1, qObject1, step2, qObject2, step3, qObject3, workflow, workflowWidget, defaultTime) == EXIT_FAILURE)
  //   {
  //   return EXIT_FAILURE;
  //   }

  // // stop the workflow
  // workflow->stop();
  // QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  // app.exec();

  // handles deletion of the workflowWidget, workflow, steps, states
  // and transitions
  delete workflowWidget;

  return EXIT_SUCCESS;
}
