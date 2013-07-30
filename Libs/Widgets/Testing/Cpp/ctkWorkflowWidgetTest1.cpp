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
#include <QDebug>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QSignalSpy>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkPushButton.h"
#include "ctkWorkflow.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflowStackedWidget.h"
#include "ctkWorkflowTabWidget.h"
#include "ctkWorkflowGroupBox.h"
#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkExampleDerivedWorkflowWidgetStep.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
bool buttonClickTest(QApplication& app, int defaultTime, ctkWorkflowWidgetStep* currentStep, QWidget* shownStepArea, QLineEdit* shownLineEdit, QLabel* shownLabel, QWidget* hiddenStepArea, QLineEdit* hiddenLineEdit, QLabel* hiddenLabel, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, QPushButton* backButton, QPushButton* nextButton, QPushButton* finishButton1=0, QPushButton* finishButton2=0)
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
struct derivedTestData
{
  QPushButton* buttonToClick;
  ctkExampleDerivedWorkflowWidgetStep* currentStep;
  ctkExampleDerivedWorkflowWidgetStep* hiddenStep;
  ctkExampleDerivedWorkflowWidgetStep* stepToChangeLineEdit;
  QString lineEditText;

  derivedTestData(QPushButton* newButtonToClick, ctkExampleDerivedWorkflowWidgetStep* newCurrentStep, ctkExampleDerivedWorkflowWidgetStep* newHiddenStep, ctkExampleDerivedWorkflowWidgetStep* newStepToChangeLineEdit=0, QString newLineEditText="")
  {
    this->buttonToClick = newButtonToClick;
    this->currentStep = newCurrentStep;
    this->hiddenStep = newHiddenStep;
    this->stepToChangeLineEdit = newStepToChangeLineEdit;
    this->lineEditText = newLineEditText;
  }

  bool runTest(QApplication& app, int defaultTime, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, QPushButton* backButton, QPushButton* nextButton, QPushButton* finishButton1=0, QPushButton* finishButton2=0)
  {
    if (this->stepToChangeLineEdit)
      {
      this->stepToChangeLineEdit->lineEdit()->setText(lineEditText);
      }
    if (this->buttonToClick)
      {
      this->buttonToClick->click();
      }

    if (this->currentStep)
      {
      return buttonClickTest(app, defaultTime, this->currentStep, this->currentStep->stepArea(), this->currentStep->lineEdit(), this->currentStep->label(), this->hiddenStep->stepArea(), this->hiddenStep->lineEdit(), this->hiddenStep->label(), workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2);
      }
    else
      {
      return buttonClickTest(app, defaultTime, this->currentStep, 0, 0, 0, this->hiddenStep->stepArea(), this->hiddenStep->lineEdit(), this->hiddenStep->label(), workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2);
      }
  }
};

//-----------------------------------------------------------------------------
// simulates valid and invalid user interaction for a workflow with
// two steps
int userInteractionSimulator1(QApplication& app, ctkExampleDerivedWorkflowWidgetStep* step1, ctkExampleDerivedWorkflowWidgetStep* step2, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
{
  QPushButton* backButton = workflowWidget->buttonBoxWidget()->backButton();
  QPushButton* nextButton = workflowWidget->buttonBoxWidget()->nextButton();
  Q_ASSERT(backButton);
  Q_ASSERT(nextButton);

#ifndef QT_NO_DEBUG
  QPushButton* finishButton = workflowWidget->buttonBoxWidget()->goToButtons().first();
  Q_ASSERT(finishButton);
#endif

  QList<derivedTestData*> tests;

  // we should be in the first step at the start of the workflow
  tests << new derivedTestData(0, step1, step2);

  // tests with good input (lineEdit value = 100, which passes the
  // condition that it be >= 10)

  // CurrentStep  ButtonPress  ExpectedStep  Shouldn'tDoAnything
  // step1        next         step2
  // step2        back         step1
  // step1        next         step2
  // step2        back         step1

  tests << new derivedTestData(nextButton, step2, step1)
        << new derivedTestData(backButton, step1, step2)
        << new derivedTestData(nextButton, step2, step1)
        << new derivedTestData(backButton, step1, step2);

  // tests with both good and bad input (lineEdit value may be
  // invalid)

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step1        next         *             step1
  // step1        next                       step2
  // step2        back         *             step1
  // step1        next         * (empty)     step1
  // step1        next                       step2

  tests << new derivedTestData(nextButton, step1, step2, step1, "1")
        << new derivedTestData(nextButton, step2, step1, step1, "100")
        << new derivedTestData(backButton, step1, step2)
        << new derivedTestData(nextButton, step1, step2, step1, "")
        << new derivedTestData(nextButton, step2, step1, step1, "100");

  foreach (derivedTestData* test, tests)
    {
    if (!test->runTest(app, defaultTime, workflow, workflowWidget, backButton, nextButton))
      {
      return EXIT_FAILURE;
      }
    }

  // TODO
  // // after adding the steps, then the widget's client area should have
  // // them as a child
  // if (!workflowWidget->clientArea()->isAncestorOf(testStep1->stepArea()))
  //   {
  //   std::cerr << "testStep1 was incorrectly added to the workflowWidget" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  // if (!workflowWidget->clientArea()->isAncestorOf(testStep2->stepArea()))
  //   {
  //   std::cerr << "testStep2 was incorrectly added to the workflowWidget" << std::endl;
  //   return EXIT_FAILURE;
  //   }

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
// simulates valid and invalid user interaction for a workflow with
// three steps
int userInteractionSimulator2(QApplication& app, ctkExampleDerivedWorkflowWidgetStep* step1, ctkExampleDerivedWorkflowWidgetStep* step2, ctkExampleDerivedWorkflowWidgetStep* step3, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
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

  QList<derivedTestData*> tests;

  // we should be in the first step at the start of the workflow
  tests << new derivedTestData(0, step1, step2);

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

  tests << new derivedTestData(nextButton, step2, step1)
        << new derivedTestData(nextButton, step3, step2, step2, "100")
        << new derivedTestData(backButton, step2, step3)
        << new derivedTestData(backButton, step1, step2)
        << new derivedTestData(nextButton, step2, step1)
        << new derivedTestData(nextButton, step3, step2)
        << new derivedTestData(backButton, step2, step3);

  // tests with both good and bad input (lineEdit value may be
  // invalid)

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step2        next         *             step2
  // step2        next                       step3
  // step3        back         *             step2
  // step2        next         * (empty)     step2
  // step2        next                       step3

  tests << new derivedTestData(nextButton, step2, step3, step2, "1")
        << new derivedTestData(nextButton, step3, step2, step2, "100")
        << new derivedTestData(backButton, step2, step3)
        << new derivedTestData(nextButton, step2, step3, step2, "")
        << new derivedTestData(nextButton, step3, step2, step2, "100");

  // first go back to the beginning
  tests << new derivedTestData(backButton, step2, step3, step2, "100")
        << new derivedTestData(backButton, step1, step2);

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

  tests << new derivedTestData(finishButton, step1, step3)
        << new derivedTestData(finishButton, step1, step3)
        << new derivedTestData(nextButton, step2, step1)
        << new derivedTestData(finishButton, step2, step3)
        << new derivedTestData(nextButton, step3, step2)
        << new derivedTestData(finishButton, step3, step2)
        << new derivedTestData(backButton, step2, step3)
        << new derivedTestData(finishButton, step2, step3)
        << new derivedTestData(backButton, step1, step2)
        << new derivedTestData(finishButton, step1, step3);

  // tests going to the finish step with invalid input

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step1        finish       * (step2)     step2
  // step2        next                       step3
  // step3        back                       step2
  // step2        finish       * (step2)     step2
  // step2        back                       step1
  // step1        finish       * (step3)     step1

  tests << new derivedTestData(finishButton, step2, step1, step2, "0")
        << new derivedTestData(nextButton, step3, step2, step2, "100")
        << new derivedTestData(backButton, step2, step3)
        << new derivedTestData(finishButton, step2, step3, step2, "0")
        << new derivedTestData(backButton, step1, step2, step2, "100") // reset text for step2
        << new derivedTestData(finishButton, step1, step3, step3, "0");

  foreach (derivedTestData* test, tests)
    {
    if (!test->runTest(app, defaultTime, workflow, workflowWidget, backButton, nextButton, finishButton))
      {
      return EXIT_FAILURE;
      }
    }

  // TODO
  // // after adding the steps, then the widget's client area should have
  // // them as a child
  // if (!workflowWidget->workflowGroupBox()->isAncestorOf(step3->stepArea()))
  //   {
  //   std::cerr << "step3 was incorrectly added to the workflowWidget" << std::endl;
  //   return EXIT_FAILURE;
  //   }

  return EXIT_SUCCESS;
}

// //-----------------------------------------------------------------------------
// // simulates valid and invalid user interaction for a workflow with
// // three steps and two finish steps
// int userInteractionSimulator3(QApplication& app, ctkExampleDerivedWorkflowWidgetStep* step1, ctkExampleDerivedWorkflowWidgetStep* step2, ctkExampleDerivedWorkflowWidgetStep* step3, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
// {
//   QPushButton* backButton = workflowWidget->buttonBoxWidget()->backButton();
//   QPushButton* nextButton = workflowWidget->buttonBoxWidget()->nextButton();
//   QPushButton* finishButton1 = workflowWidget->buttonBoxWidget()->goToButtons().first();
//   QPushButton* finishButton2 = workflowWidget->buttonBoxWidget()->goToButtons().at(1);
//   Q_ASSERT(backButton);
//   Q_ASSERT(nextButton);
//   Q_ASSERT(finishButton1);
//   Q_ASSERT(finishButton2);


//   // if (!nextButton || !finishButton || !finishButton || nextButton->text() != "Next" || finishButton->text() != "finish" || finishButton->text() != "finish")
//   //   {
//   //   std::cerr << "incorrect button assignment for step1" << std::endl;
//   //   return EXIT_FAILURE;
//   //   }

//   // we should be in the first step
//   if (!buttonClickTest(app, defaultTime, step1, step2, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   // tests with good input, so that we can get all of the buttons
//   // (lineEdit value = 100, which passes the condition that it be >= 10)

//   // CurrentStep  ButtonPress  ExpectedStep  Shouldn'tDoAnything
//   // step1        next         step2
//   // step2        next         step3

//   // normal forwards/backwards transitions
//   nextButton->click();
//   QTimer::singleShot(defaultTime, &app, SLOT(quit()));
//   app.exec();

//   // get the back/next buttons for the second step
// //  buttons = step2->stepArea()->findChildren<QPushButton*>();
//   // if (buttons.length() != 4)
//   //   {
//   //   std::cerr << "incorrect number of buttons for step2" << std::endl;
//   //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 4" << std::endl;
//   //   return EXIT_FAILURE;
//   //   }
//   // backButton = buttons.at(BACK_BUTTON_INDEX);
//   // nextButton = buttons.at(NEXT_BUTTON_INDEX);
//   // finishButton = buttons.at(FINISH_BUTTON_INDEX_1);
//   // finishButton = buttons.at(FINISH_BUTTON_INDEX_2);
//   // backButton = workflowWidget->buttonBoxWidget()->backButton();
//   // nextButton = workflowWidget->buttonBoxWidget()->nextButton();
//   // finishButton = workflowWidget->buttonBoxWidget()->goToButtons().first();
//   // finishButton = workflowWidget->buttonBoxWidget()->goToButtons().at(1);
//   // if (!backButton || !nextButton || !finishButton || !finishButton || backButton->text() != "Back" || nextButton->text() != "Next" || finishButton->text() != "finish" || finishButton->text() != "finish")
//   //   {
//   //   std::cerr << "incorrect button assignment for step2" << std::endl;
//   //   return EXIT_FAILURE;
//   //   }
//   if (!buttonClickTest(app, defaultTime, step2, step1, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   nextButton->click();
//   QTimer::singleShot(defaultTime, &app, SLOT(quit()));
//   app.exec();

//   // get the back/next buttons for the third step
// //  buttons = step3->stepArea()->findChildren<QPushButton*>();
//   // if (buttons.length() != 3)
//   //   {
//   //   std::cerr << "incorrect number of buttons for step3" << std::endl;
//   //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 3" << std::endl;
//   //   return EXIT_FAILURE;
//   //   }
//   // backButton = buttons.at(BACK_BUTTON_INDEX);
//   // finishButton1 = buttons.at(FINISH_BUTTON_INDEX_1);
//   // finishButton2 = buttons.at(FINISH_BUTTON_INDEX_2);
//   // backButton = workflowWidget->buttonBoxWidget()->backButton();
//   // finishButton1 = workflowWidget->buttonBoxWidget()->goToButtons().first();
//   // finishButton2 = workflowWidget->buttonBoxWidget()->goToButtons().at(1);
//   // if (!backButton || !finishButton1 || !finishButton2 || backButton->text() != "Back" || finishButton1->text() != "finish" || finishButton2->text() != "finish")
//   //   {
//   //   std::cerr << "incorrect button assignment for step3" << std::endl;
//   //   return EXIT_FAILURE;
//   //   }
//   if (!buttonClickTest(app, defaultTime, step3, step2, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//    // tests going to the finish step

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

//   backButton->click();
//   if (!buttonClickTest(app, defaultTime, step2, step3, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   backButton->click();
//   if (!buttonClickTest(app, defaultTime, step1, step2, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   finishButton1->click();
//   if (!buttonClickTest(app, defaultTime, step1, step3, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   nextButton->click();
//   if (!buttonClickTest(app, defaultTime, step2, step1, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   finishButton1->click();
//   if (!buttonClickTest(app, defaultTime, step2, step3, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   finishButton2->click();
//   if (!buttonClickTest(app, defaultTime, step2, step3, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   backButton->click();
//   if (!buttonClickTest(app, defaultTime, step1, step2, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   finishButton2->click();
//   if (!buttonClickTest(app, defaultTime, step1, step2, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   nextButton->click();
//   if (!buttonClickTest(app, defaultTime, step2, step1, workflow, workflowWidget, backButton, nextButton, finishButton1, finishButton2)) {return EXIT_FAILURE;}

//   return EXIT_SUCCESS;
// }

//-----------------------------------------------------------------------------
int runWorkflowWidgetTest(ctkWorkflowWidget* workflowWidget, QApplication& app, bool hideWidgets, int defaultTime)
{
  ctkWorkflow* workflow = new ctkWorkflow;
  workflowWidget->setWorkflow(workflow);

  ctkWorkflowGroupBox* groupBox = workflowWidget->workflowGroupBox();
  groupBox->setPreText("I am some pre-text");
  groupBox->setPostText("I am some post-text");
  groupBox->setHideWidgetsOfNonCurrentSteps(hideWidgets);

  // create and add the first workflow step (depends on workflowWidget
  // type)
  ctkExampleDerivedWorkflowWidgetStep* step1 = new ctkExampleDerivedWorkflowWidgetStep("Step 1");
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
  ctkExampleDerivedWorkflowWidgetStep* step2 = new ctkExampleDerivedWorkflowWidgetStep("Step 2");
  step2->setName("Step 2");
  step2->setDescription("I am in step 2");
  if (ctkWorkflowTabWidget* tabWidget = qobject_cast<ctkWorkflowTabWidget*>(workflowWidget))
    {
    tabWidget->associateStepWithLabel(step2, "tab2");
    }

  int expectedStepCount = 0;
  int currentStepCount = workflow->steps().count();
  if (currentStepCount != expectedStepCount)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with steps()\n"
              << "\tcurrentStepCount: " << currentStepCount << "\n"
              << "\texpectedStepCount:" << expectedStepCount << std::endl;
    return EXIT_FAILURE;
    }

  QSignalSpy signalSpyStepRegistered(workflow, SIGNAL(stepRegistered(ctkWorkflowStep*)));

  // add the steps to the workflow
  workflow->addTransition(step1, step2);

  expectedStepCount = 2;
  currentStepCount = workflow->steps().count();
  if (currentStepCount != expectedStepCount)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with steps()\n"
              << "\tcurrentStepCount: " << currentStepCount << "\n"
              << "\texpectedStepCount:" << expectedStepCount << std::endl;
    return EXIT_FAILURE;
    }

  int expectedSignalStepRegisteredCount = 2;
  int currentSignalStepRegisteredCount = signalSpyStepRegistered.count();
  if (currentSignalStepRegisteredCount != expectedSignalStepRegisteredCount)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with 'stepRegistered' signal\n"
              << "\tcurrentSignalStepRegisteredCount: " << currentSignalStepRegisteredCount << "\n"
              << "\texpectedSignalStepRegisteredCount:" << expectedSignalStepRegisteredCount << std::endl;
    return EXIT_FAILURE;
    }

  // start the workflow
  workflow->start();
  workflowWidget->show();

  // Attempt to add step to a different workflow
  ctkWorkflow* workflow2 = new ctkWorkflow;
  workflow2->addTransition(step1, step2);

  expectedStepCount = 0;
  currentStepCount = workflow2->steps().count();
  if (currentStepCount != expectedStepCount)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with steps()\n"
              << "\tcurrentStepCount: " << currentStepCount << "\n"
              << "\texpectedStepCount:" << expectedStepCount << std::endl;
    return EXIT_FAILURE;
    }

  // first user interaction test
  if (userInteractionSimulator1(app, step1, step2, workflow, workflowWidget, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  // stop the workflow
  workflow->stop();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // create and add a third workflow step (depends on workflowWidget
  // type)
  ctkExampleDerivedWorkflowWidgetStep* step3 = new ctkExampleDerivedWorkflowWidgetStep("Step 3");
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

  workflow->addTransition(step2, step3);

  // restart the workflow
  workflow->start();


//   second user interaction test
  if (userInteractionSimulator2(app, step1, step2, step3, workflow, workflowWidget, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  // stop the workflow
  workflow->stop();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // TODO put back once we can have more than one finish step
  // make the second workflow step a finish step as well
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
  // if (userInteractionSimulator3(app, step1, step2, step3, workflow,  workflowWidget, defaultTime) == EXIT_FAILURE)
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

//-----------------------------------------------------------------------------
int ctkWorkflowWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  int defaultTime = 100;

  bool hideWidgets = false;
  if (runWorkflowWidgetTest(new ctkWorkflowWidget, app, hideWidgets, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }
  if (runWorkflowWidgetTest(new ctkWorkflowStackedWidget, app, hideWidgets, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }
  if (runWorkflowWidgetTest(new ctkWorkflowTabWidget, app, hideWidgets, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  hideWidgets = true;
  if (runWorkflowWidgetTest(new ctkWorkflowWidget, app, hideWidgets, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }
  if (runWorkflowWidgetTest(new ctkWorkflowStackedWidget, app, hideWidgets, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }
  if (runWorkflowWidgetTest(new ctkWorkflowTabWidget, app, hideWidgets, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}




