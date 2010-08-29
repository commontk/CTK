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

// QT includes
#include <QApplication>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QList>

// CTK includes
#include "ctkWorkflow.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflowStackedWidget.h"
#include "ctkWorkflowTabWidget.h"
#include "ctkExampleDerivedWorkflowWidgetStep.h"

// STD includes
#include <cstdlib>
#include <iostream>

// //-----------------------------------------------------------------------------
bool buttonClickTest(QApplication& app, int defaultTime, ctkExampleDerivedWorkflowWidgetStep* shown, ctkExampleDerivedWorkflowWidgetStep* hidden, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, QPushButton* shownBackButton=0, QPushButton* shownNextButton=0, QPushButton* hiddenBackButton=0, QPushButton* hiddenNextButton=0, QPushButton* shownFinishButton=0, QPushButton* hiddenFinishButton = 0, QPushButton* shownFinishButton2=0, QPushButton* hiddenFinishButton2=0)
{
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // // ensure we are in the correct step
  if (workflow->currentStep() != shown)
    {
    std::cerr << "In incorrect step" << std::endl;
    return false;
    }

  // ensure that widgets of current step are visible and enabled
  if (!shown->stepArea()->isVisible() || (shownBackButton && !shownBackButton->isVisible()) || (shownNextButton && !shownNextButton->isVisible()) || !shown->label()->isVisible() || !shown->lineEdit()->isVisible() || (shownFinishButton && !shownFinishButton->isVisible()) || (shownFinishButton2 && !shownFinishButton2->isVisible()))
    {
    std::cerr << "Incorrect widget visibility - the current step's widgets are invisible" << std::endl;
    return false;
    }
  if ((shownBackButton && !shownBackButton->isEnabled()) || (shownNextButton && !shownNextButton->isEnabled()) || !shown->lineEdit()->isEnabled() || (shownFinishButton && !shownFinishButton->isEnabled()) || (shownFinishButton2 && !shownFinishButton2->isEnabled()))
    {
    std::cerr << "Incorrect widget visibility - the current step's widgets are disabled" << std::endl;    
    return false;
    }

  // ensure that the shown step's name and description are shown in
  // the widget
  if (shown->name() != workflowWidget->title() || shown->description() != workflowWidget->subTitle())
    {
    std::cerr << "Incorrect widget title/subtitle" << std::endl;
    return false;
    }

  // ensure that widgets of the other step are either invisible, or
  // visible but disabled
  if (hidden->stepArea())
    {
    if (hidden->stepArea()->isVisible() && workflowWidget->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's stepArea is showing" << std::endl;
      return false;
      }
    else if (hidden->stepArea()->isVisible() && hidden->stepArea()->isEnabled())
      {
      std::cerr << "Incorrect widget visibility - the other step's stepArea is enabled" << std::endl;
      return false;
      }
    }
  if (hidden->label())
    {
    if (hidden->label()->isVisible() && workflowWidget->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's label is showing" << std::endl;
      return false;
      }
    }
  if (hiddenBackButton)
    {
    if (hiddenBackButton->isVisible() && workflowWidget->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's backButton is showing" << std::endl;
      return false;
      }
    else if (hiddenBackButton->isVisible() && hiddenBackButton->isEnabled())
      {
      std::cerr << "Incorrect widget visibility - the other step's backButton is enabled" << std::endl;
      return false;
      }
    }
  if (hiddenNextButton)
    {
    if (hiddenNextButton->isVisible() && workflowWidget->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's nextButton is showing" << std::endl;
      return false;
      }
    else if (hiddenNextButton->isVisible() && hiddenNextButton->isEnabled())
      {
      std::cerr << "Incorrect widget visibility - the other step's nextButton is enabled" << std::endl;
      return false;
      }
    }
  if (hidden->lineEdit())
    {
    if (hidden->lineEdit()->isVisible() && workflowWidget->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's lineEdit is showing" << std::endl;
      return false;
      }
    else if (hidden->lineEdit()->isVisible() && hidden->lineEdit()->isEnabled())
      {
      std::cerr << "Incorrect widget visibility - the other step's lineEdit is enabled" << std::endl;
      return false;
      }
    }
  if (hiddenFinishButton)
    {
    if (hiddenFinishButton->isVisible() && workflowWidget->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's finishButton is showing" << std::endl;
      return false;
      }
    else if (hiddenFinishButton->isVisible() && hiddenFinishButton->isEnabled())
      {
      std::cerr << "Incorrect widget visibility - the other step's finishButton is enabled" << std::endl;
      return false;
      }      
    }
  if (hiddenFinishButton2)
    {
    if (hiddenFinishButton2->isVisible() && workflowWidget->hideWidgetsOfNonCurrentSteps())
      {
      std::cerr << "Incorrect widget visibility - the other step's finishButton is showing" << std::endl;
      return false;
      }
    else if (hiddenFinishButton2->isVisible() && hiddenFinishButton->isEnabled())
      {
      std::cerr << "Incorrect widget visibility - the other step's finishButton is enabled" << std::endl;
      return false;
      }      
    }

  return true;
}

//-----------------------------------------------------------------------------
// simulates valid and invalid user interaction for a workflow with
// two steps
int userInteractionSimulator1(QApplication& app, ctkExampleDerivedWorkflowWidgetStep* testStep1, ctkExampleDerivedWorkflowWidgetStep* testStep2, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
{
  int BACK_BUTTON_INDEX = 0;
  int NEXT_BUTTON_INDEX = 1;

  // test to ensure we can change the layout of the workflow widget
  workflowWidget->setDirection(QBoxLayout::LeftToRight);

  // get the buttons
  QPushButton* testStep1BackButton = 0;
  QPushButton* testStep1NextButton = 0;
  QPushButton* testStep2BackButton = 0;
  QPushButton* testStep2NextButton = 0;

  // get the back/next buttons for the first step
  QList<QPushButton*> buttons = testStep1->stepArea()->findChildren<QPushButton*>();
  // if (buttons.length() != 1)
  //   {
  //   std::cerr << "incorrect number of buttons for testStep1" << std::endl;
  //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 1" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  testStep1NextButton = buttons.at(NEXT_BUTTON_INDEX);
  if (!testStep1NextButton || testStep1NextButton->text() != "Next")
    {
    std::cerr << "incorrect button assignment for testStep1" << std::endl;
    return EXIT_FAILURE;
    }

  // we should be in the first step at the start of the workflow
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton)) {return EXIT_FAILURE;}

  // tests with good input (lineEdit value = 100, which passes the
  // condition that it be >= 10)

  // CurrentStep  ButtonPress  ExpectedStep  Shouldn'tDoAnything
  // step1        next         step2
  // step2        back         step1
  // step1        next         step2
  // step2        back         step1

  // normal forwards/backwards transitions
  testStep1NextButton->click();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // get the back/next buttons for the second step
  buttons = testStep2->stepArea()->findChildren<QPushButton*>();
  // if (buttons.length() != 1)
  //   {
  //   std::cerr << "incorrect number of buttons for testStep2" << std::endl;
  //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 1" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  testStep2BackButton = buttons.at(BACK_BUTTON_INDEX);
  if (!testStep2BackButton || testStep2BackButton->text() != "Back")
    {
    std::cerr << "incorrect button assignment for testStep2" << std::endl;
    return EXIT_FAILURE;
    }
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton)) {return EXIT_FAILURE;}

  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton)) {return EXIT_FAILURE;}

  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton)) {return EXIT_FAILURE;}

  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton)) {return EXIT_FAILURE;}

  // tests with both good and bad input (lineEdit value may be
  // invalid)

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step1        next         *             step1
  // step1        next                       step2
  // step2        back         *             step1
  // step1        next         * (empty)     step1
  // step1        next                       step2
  
  // ensure transition to the next step fails on bad input
  testStep1->lineEdit()->setText("1");
  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton)) {return EXIT_FAILURE;}

  // now recover, going to the next step using some good input
  testStep1->lineEdit()->setText("100");
  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton)) {return EXIT_FAILURE;}

  // on bad input, going "Back" should be allowed
  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton)) {return EXIT_FAILURE;}

  // ensure transition to the next step fails on empty input
  testStep1->lineEdit()->setText("");
  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton)) {return EXIT_FAILURE;}

  // try again with some good input to ensure we can recover
  testStep1->lineEdit()->setText("100");
  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton)) {return EXIT_FAILURE;}

  // after adding the steps, then the widget's client area should have
  // them as a child
  if (!workflowWidget->clientArea()->isAncestorOf(testStep1->stepArea()))
    {
    std::cerr << "testStep1 was incorrectly added to the workflowWidget" << std::endl;
    return EXIT_FAILURE;
    }
  if (!workflowWidget->clientArea()->isAncestorOf(testStep2->stepArea()))
    {
    std::cerr << "testStep2 was incorrectly added to the workflowWidget" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
// simulates valid and invalid user interaction for a workflow with
// three steps
int userInteractionSimulator2(QApplication& app, ctkExampleDerivedWorkflowWidgetStep* testStep1, ctkExampleDerivedWorkflowWidgetStep* testStep2, ctkExampleDerivedWorkflowWidgetStep* testStep3, bool hideWidgets, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
{
  int BACK_BUTTON_INDEX = 0;
  int NEXT_BUTTON_INDEX = 1;
  int FINISH_BUTTON_INDEX_1 = 2;

  // test that the setDirection layout was applied
  // properly: the widget should be much wider than it is tall
  QSize size = workflowWidget->size();
  int height = size.height();
  int width = size.width();
  if (height >= (width/2))
    {
    std::cerr << "The workflow widget is too tall - was the direction applied?" << std::endl;
    return EXIT_FAILURE;
    }
  // test that we can change the layout of the stepAreas: it should be
  // wider than it was before
  testStep1->setStepAreaDirection(QBoxLayout::LeftToRight);
  testStep2->setStepAreaDirection(QBoxLayout::LeftToRight);
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  size = workflowWidget->size();
  if (size.width() <= width)
    {
    std::cerr << "The workflow widget did not increase in width after setting the step area direction to LeftToRight" << std::endl;
    return EXIT_FAILURE;
    }

  // test to ensure we can change the layout of the workflow widget's
  // client area after having added some steps to it
  if (ctkWorkflowAbstractPagedWidget* pagedWidget = qobject_cast<ctkWorkflowAbstractPagedWidget*>(workflowWidget))
    {
    pagedWidget->setClientAreaDirection(QBoxLayout::LeftToRight);
    }
  else
    {
    workflowWidget->setClientAreaDirection(QBoxLayout::LeftToRight);
    }

  // ensure we can get the pages/layouts we may want
  if (ctkWorkflowAbstractPagedWidget* pagedWidget = qobject_cast<ctkWorkflowAbstractPagedWidget*>(workflowWidget))
    {
    if (!pagedWidget->getWidgetFromIndex(1))
      {
      std::cerr << "could not access widget for page 1" << std::endl;
      return EXIT_FAILURE;
      }
    if (!pagedWidget->getWidgetFromIndex(1)->layout())
      {
      std::cerr << "could not access client area layout for page 1" << std::endl;
      return EXIT_FAILURE;
      }
    }
  else
    {
    if (!workflowWidget->clientArea()->layout())
      {
      std::cerr << "could not access client area layout" << std::endl;
      return EXIT_FAILURE;
      }
    }

  // get the buttons
  QPushButton* testStep1BackButton = 0;
  QPushButton* testStep1NextButton = 0;
  QPushButton* testStep1FinishButton = 0;
  QPushButton* testStep2BackButton = 0;
  QPushButton* testStep2NextButton = 0;
  QPushButton* testStep2FinishButton = 0;
  QPushButton* testStep3BackButton = 0;
  QPushButton* testStep3NextButton = 0;
  QPushButton* testStep3FinishButton = 0;

  // get the back/next buttons for the first step
  QList<QPushButton*> buttons = testStep1->stepArea()->findChildren<QPushButton*>();
  // if (buttons.length() != 2)
  //   {
  //   std::cerr << "incorrect number of buttons for testStep1" << std::endl;
  //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 2" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  testStep1NextButton = buttons.at(NEXT_BUTTON_INDEX);
  testStep1FinishButton = buttons.at(FINISH_BUTTON_INDEX_1);
  if (!testStep1NextButton || !testStep1FinishButton || testStep1NextButton->text() != "Next" || testStep1FinishButton->text() != "Step 3")
    {
    std::cerr << "incorrect button assignment for testStep1" << std::endl;
    return EXIT_FAILURE;
    }

  // we should be in the first step when we start the workflow
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton, testStep1FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

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

  // normal forwards/backwards transitions
  testStep1NextButton->click();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // get the back/next buttons for the second step
  buttons = testStep2->stepArea()->findChildren<QPushButton*>();
  // if (buttons.length() != 3)
  //   {
  //   std::cerr << "incorrect number of buttons for testStep2" << std::endl;
  //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 3" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  testStep2BackButton = buttons.at(BACK_BUTTON_INDEX);
  testStep2NextButton = buttons.at(NEXT_BUTTON_INDEX);
  testStep2FinishButton = buttons.at(FINISH_BUTTON_INDEX_1);
  if (!testStep2BackButton || !testStep2NextButton || !testStep2FinishButton || testStep2BackButton->text() != "Back" || testStep2NextButton->text() != "Next" || testStep2FinishButton->text() != "Step 3")
    {
    std::cerr << "incorrect button assignment for testStep2" << std::endl;
    return EXIT_FAILURE;
    }
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton, testStep2FinishButton, testStep1FinishButton)) {return EXIT_FAILURE;}
  
  testStep2->lineEdit()->setText("100");
  testStep2NextButton->click();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // get the back/next buttons for the third step
  buttons = testStep3->stepArea()->findChildren<QPushButton*>();
  // if (buttons.length() != 2)
  //   {
  //   std::cerr << "incorrect number of buttons for testStep3" << std::endl;
  //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 2" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  testStep3BackButton = buttons.at(BACK_BUTTON_INDEX);
  testStep3FinishButton = buttons.at(FINISH_BUTTON_INDEX_1);
  if (!testStep3BackButton || !testStep3FinishButton || testStep3BackButton->text() != "Back" || testStep3FinishButton->text() != "Step 3")
    {
    std::cerr << "incorrect button assignment for testStep3" << std::endl;
    return EXIT_FAILURE;
    }
  if (!buttonClickTest(app, defaultTime, testStep3, testStep2, workflow, workflowWidget, testStep3BackButton, testStep3NextButton, testStep2BackButton, testStep2NextButton, testStep3FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  // test that the setClientAreaDirection layout was applied properly:
  // the widget should be quite a bit wider than it was before
  if (!hideWidgets)
    {
    QSize size = workflowWidget->size();
    if (size.width() <= width)
      {
      std::cerr << "The workflow widget is not wide enough - was the clientAreaDirection applied?" << std::endl;
      return EXIT_FAILURE;
      }
    }

  testStep3BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton, testStep1FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  // normal forwards/backwards transitions
  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton, testStep2FinishButton, testStep1FinishButton)) {return EXIT_FAILURE;}

  testStep2NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep3, testStep2, workflow, workflowWidget, testStep3BackButton, testStep3NextButton, testStep2BackButton, testStep2NextButton, testStep3FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  testStep3BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  // tests with both good and bad input (lineEdit value may be
  // invalid)

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step2        next         *             step2
  // step2        next                       step3
  // step3        back         *             step2
  // step2        next         * (empty)     step2
  // step2        next                       step3

  // ensure transition to the next step fails on bad input
  testStep2->lineEdit()->setText("1");
  testStep2NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  // now recover, going to the next step with some good input
  testStep2->lineEdit()->setText("100");
  testStep2NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep3, testStep2, workflow, workflowWidget, testStep3BackButton, testStep3NextButton, testStep2BackButton, testStep2NextButton, testStep3FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  // on bad input, going "Back" should be allowed
  testStep3BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  // ensure transition to the next step fails on empty input
  testStep2->lineEdit()->setText("");
  testStep2NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  // try again with some good input to ensure we can recover
  testStep2->lineEdit()->setText("100");
  testStep2NextButton->click(); 
  if (!buttonClickTest(app, defaultTime, testStep3, testStep2, workflow, workflowWidget, testStep3BackButton, testStep3NextButton, testStep2BackButton, testStep2NextButton, testStep3FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

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

  // first go back to the beginning
  testStep3->lineEdit()->setText("100");
  testStep3BackButton->click();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton, testStep1FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  testStep1FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep3, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep3BackButton, testStep3NextButton, testStep1FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}
  
  testStep1FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep3, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep3BackButton, testStep3NextButton, testStep1FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton, testStep2FinishButton, testStep1FinishButton)) {return EXIT_FAILURE;}

  testStep2FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  testStep2NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep3, testStep2, workflow, workflowWidget, testStep3BackButton, testStep3NextButton, testStep2BackButton, testStep2NextButton, testStep3FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  testStep3FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep3, testStep2, workflow, workflowWidget, testStep3BackButton, testStep3NextButton, testStep2BackButton, testStep2NextButton, testStep3FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  testStep3BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  testStep2FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton, testStep1FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  testStep1FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep3, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep3BackButton, testStep3NextButton, testStep1FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  // tests going to the finish step with invalid input

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step1        finish       * (step2)     step2
  // step2        next                       step3
  // step3        back                       step2
  // step2        finish       * (step2)     step2
  // step2        back                       step1
  // step1        finish       * (step3)     step1

  testStep2->lineEdit()->setText("0");
  testStep1FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton, testStep2FinishButton, testStep1FinishButton)) {return EXIT_FAILURE;}

  testStep2->lineEdit()->setText("100");
  testStep2NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep3, testStep2, workflow, workflowWidget, testStep3BackButton, testStep3NextButton, testStep2BackButton, testStep2NextButton, testStep3FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}

  testStep3BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}

  testStep2->lineEdit()->setText("0");
  testStep2FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}  

  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton, testStep1FinishButton, testStep2FinishButton)) {return EXIT_FAILURE;}  

  testStep2->lineEdit()->setText("100");
  testStep3->lineEdit()->setText("0");
  testStep1FinishButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep3, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep3BackButton, testStep3NextButton, testStep1FinishButton, testStep3FinishButton)) {return EXIT_FAILURE;}
    
  // calling showWidget() with a widget that hasn't been previously
  // used with addWidget() would work for ctkWorkflowWidget, but not
  // ctkWorkflowAbstractPagedWidget) - make sure it fails
  if (ctkWorkflowAbstractPagedWidget* pagedWidget = qobject_cast<ctkWorkflowAbstractPagedWidget*>(workflowWidget))
    {
    QWidget* test = new QWidget;
    pagedWidget->showWidget(test);
    if (test->isVisible())
      {
      std::cerr << "calling ctkWorkflowAbstractPagedWidget::showWidget() using a widget that was not previously added seems to work - and it shouldn't" << std::endl;
      return EXIT_FAILURE;
      }
    }

  // after adding the steps, then the widget's client area should have
  // them as a child
  if (!workflowWidget->clientArea()->isAncestorOf(testStep3->stepArea()))
    {
    std::cerr << "testStep3 was incorrectly added to the workflowWidget" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
// simulates valid and invalid user interaction for a workflow with
// three steps and two finish steps
int userInteractionSimulator3(QApplication& app, ctkExampleDerivedWorkflowWidgetStep* testStep1, ctkExampleDerivedWorkflowWidgetStep* testStep2, ctkExampleDerivedWorkflowWidgetStep* testStep3, ctkWorkflow* workflow, ctkWorkflowWidget* workflowWidget, int defaultTime)
{
  int BACK_BUTTON_INDEX = 0;
  int NEXT_BUTTON_INDEX = 1;
  int FINISH_BUTTON_INDEX_1 = 2;
  int FINISH_BUTTON_INDEX_2 = 3;

  // get the buttons
  QPushButton* testStep1BackButton = 0;
  QPushButton* testStep1NextButton = 0;
  QPushButton* testStep1FinishButton1 = 0;
  QPushButton* testStep1FinishButton2 = 0;
  QPushButton* testStep2BackButton = 0;
  QPushButton* testStep2NextButton = 0;
  QPushButton* testStep2FinishButton1 = 0;
  QPushButton* testStep2FinishButton2 = 0;
  QPushButton* testStep3BackButton = 0;
  QPushButton* testStep3NextButton = 0;
  QPushButton* testStep3FinishButton1 = 0;
  QPushButton* testStep3FinishButton2 = 0;

  // get the back/next buttons for the first step
  QList<QPushButton*> buttons = testStep1->stepArea()->findChildren<QPushButton*>();
  // if (buttons.length() != 3)
  //   {
  //   std::cerr << "incorrect number of buttons for testStep1" << std::endl;
  //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 3" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  testStep1NextButton = buttons.at(NEXT_BUTTON_INDEX);
  testStep1FinishButton1 = buttons.at(FINISH_BUTTON_INDEX_1);
  testStep1FinishButton2 = buttons.at(FINISH_BUTTON_INDEX_2);
  if (!testStep1NextButton || !testStep1FinishButton1 || !testStep1FinishButton2 || testStep1NextButton->text() != "Next" || testStep1FinishButton1->text() != "finish" || testStep1FinishButton2->text() != "finish")
    {
    std::cerr << "incorrect button assignment for testStep1" << std::endl;
    return EXIT_FAILURE;
    }

  // we should be in the first step     
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton, testStep1FinishButton1, testStep2FinishButton1, testStep1FinishButton2, testStep2FinishButton2)) {return EXIT_FAILURE;}

  // tests with good input, so that we can get all of the buttons
  // (lineEdit value = 100, which passes the condition that it be >= 10)

  // CurrentStep  ButtonPress  ExpectedStep  Shouldn'tDoAnything
  // step1        next         step2
  // step2        next         step3

  // normal forwards/backwards transitions
  testStep1NextButton->click();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // get the back/next buttons for the second step
  buttons = testStep2->stepArea()->findChildren<QPushButton*>();
  // if (buttons.length() != 4)
  //   {
  //   std::cerr << "incorrect number of buttons for testStep2" << std::endl;
  //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 4" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  testStep2BackButton = buttons.at(BACK_BUTTON_INDEX);
  testStep2NextButton = buttons.at(NEXT_BUTTON_INDEX);
  testStep2FinishButton1 = buttons.at(FINISH_BUTTON_INDEX_1);
  testStep2FinishButton2 = buttons.at(FINISH_BUTTON_INDEX_2);
  if (!testStep2BackButton || !testStep2NextButton || !testStep2FinishButton1 || !testStep2FinishButton2 || testStep2BackButton->text() != "Back" || testStep2NextButton->text() != "Next" || testStep2FinishButton1->text() != "finish" || testStep2FinishButton2->text() != "finish")
    {
    std::cerr << "incorrect button assignment for testStep2" << std::endl;
    return EXIT_FAILURE;
    }
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1NextButton, testStep2FinishButton1, testStep1FinishButton1, testStep2FinishButton2, testStep1FinishButton2)) {return EXIT_FAILURE;}
  
  testStep2NextButton->click();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // get the back/next buttons for the third step
  buttons = testStep3->stepArea()->findChildren<QPushButton*>();
  // if (buttons.length() != 3)
  //   {
  //   std::cerr << "incorrect number of buttons for testStep3" << std::endl;
  //   std::cerr << "number of buttons = " << buttons.length() << ", expecting 3" << std::endl;
  //   return EXIT_FAILURE;
  //   }
  testStep3BackButton = buttons.at(BACK_BUTTON_INDEX);
  testStep3FinishButton1 = buttons.at(FINISH_BUTTON_INDEX_1);
  testStep3FinishButton2 = buttons.at(FINISH_BUTTON_INDEX_2);
  if (!testStep3BackButton || !testStep3FinishButton1 || !testStep3FinishButton2 || testStep3BackButton->text() != "Back" || testStep3FinishButton1->text() != "finish" || testStep3FinishButton2->text() != "finish")
    {
    std::cerr << "incorrect button assignment for testStep3" << std::endl;
    return EXIT_FAILURE;
    }
  if (!buttonClickTest(app, defaultTime, testStep3, testStep2, workflow, workflowWidget, testStep3BackButton, testStep3NextButton, testStep2BackButton, testStep2NextButton, testStep3FinishButton1, testStep2FinishButton1, testStep3FinishButton2, testStep2FinishButton2)) {return EXIT_FAILURE;}

   // tests going to the finish step

  // CurrentStep  ButtonPress  Invalid input ExpectedStep
  // step3        back                       step2
  // step2        back                       step1
  // step1        finish1                    step1
  // step1        next                       step2
  // step2        finish1                    step2
  // step2        finish2                    step2
  // step2        back                       step1
  // step1        finish2                    step1
  // step1        next                       step2

  testStep3BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton1, testStep3FinishButton1, testStep2FinishButton2, testStep3FinishButton2)) {return EXIT_FAILURE;}

  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton, testStep1FinishButton1, testStep2FinishButton1, testStep1FinishButton2, testStep2FinishButton2)) {return EXIT_FAILURE;}

  testStep1FinishButton1->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep3, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep3BackButton, testStep3BackButton, testStep1FinishButton1, testStep3FinishButton1, testStep1FinishButton2, testStep3FinishButton2)) {return EXIT_FAILURE;}

  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1BackButton, testStep2FinishButton1, testStep1FinishButton1, testStep2FinishButton2, testStep1FinishButton2)) {return EXIT_FAILURE;}

  testStep2FinishButton1->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton1, testStep3FinishButton1, testStep2FinishButton2, testStep3FinishButton2)) {return EXIT_FAILURE;}

  testStep2FinishButton2->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep3, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep3BackButton, testStep3NextButton, testStep2FinishButton1, testStep3FinishButton1, testStep2FinishButton2, testStep3FinishButton2)) {return EXIT_FAILURE;}

  testStep2BackButton->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2NextButton, testStep1FinishButton1, testStep2FinishButton1, testStep1FinishButton2, testStep2FinishButton2)) {return EXIT_FAILURE;}

  testStep1FinishButton2->click();
  if (!buttonClickTest(app, defaultTime, testStep1, testStep2, workflow, workflowWidget, testStep1BackButton, testStep1NextButton, testStep2BackButton, testStep2BackButton, testStep1FinishButton1, testStep2FinishButton1, testStep1FinishButton2, testStep2FinishButton2)) {return EXIT_FAILURE;}
  
  testStep1NextButton->click();
  if (!buttonClickTest(app, defaultTime, testStep2, testStep1, workflow, workflowWidget, testStep2BackButton, testStep2NextButton, testStep1BackButton, testStep1BackButton, testStep2FinishButton1, testStep1FinishButton1, testStep2FinishButton2, testStep1FinishButton2)) {return EXIT_FAILURE;}

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
int runWorkflowWidgetTest(ctkWorkflowWidget* workflowWidget, QApplication& app, bool hideWidgets, int defaultTime)
{
  ctkWorkflow* workflow = new ctkWorkflow;
  workflowWidget->setWorkflow(workflow);
  workflowWidget->setPreText("I am some pre-text");
  workflowWidget->setPostText("I am some post-text");
  workflowWidget->setHideWidgetsOfNonCurrentSteps(hideWidgets);
  workflowWidget->setHasButtonBoxWidget(false);

  // create and add the first workflow step (depends on workflowWidget
  // type)
  ctkExampleDerivedWorkflowWidgetStep* testStep1 = new ctkExampleDerivedWorkflowWidgetStep(workflow, "Step 1");
  testStep1->setName("Step 1");
  testStep1->setDescription("I am in step 1");
  if (ctkWorkflowTabWidget* tabWidget = qobject_cast<ctkWorkflowTabWidget*>(workflowWidget))
    {
    tabWidget->addStepArea(testStep1, "tab1");
    }
  testStep1->setHasButtonBoxWidget(true);

  // testStep1 is the initial step
  workflow->setInitialStep(testStep1);

  // create and add the second workflow step (depends on
  // workflowWidget type)
  ctkExampleDerivedWorkflowWidgetStep* testStep2 = new ctkExampleDerivedWorkflowWidgetStep(workflow, "Step 2");
  testStep2->setName("Step 2");
  testStep2->setDescription("I am in step 2");
  if (ctkWorkflowTabWidget* tabWidget = qobject_cast<ctkWorkflowTabWidget*>(workflowWidget))
    {
    tabWidget->addStepArea(testStep2, "tab2");
    }
  testStep2->setHasButtonBoxWidget(true);

  // add the steps to the workflow
  workflow->addTransition(testStep1, testStep2);

  // start the workflow
  workflow->start();
  workflowWidget->show();

  // first user interaction test
  if (userInteractionSimulator1(app, testStep1, testStep2, workflow, workflowWidget, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  // stop the workflow
  workflow->stop();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // create and add a third workflow step (depends on workflowWidget
  // type)
  ctkExampleDerivedWorkflowWidgetStep* testStep3 = new ctkExampleDerivedWorkflowWidgetStep(workflow, "Step 3");
  testStep3->setName("Step 3");
  testStep3->setDescription("I am in step 3");
  if (ctkWorkflowStackedWidget* stackedWidget = qobject_cast<ctkWorkflowStackedWidget*>(workflowWidget))
    {
    stackedWidget->addStepArea(testStep3, 1);
    }
  else if (ctkWorkflowTabWidget* tabWidget = qobject_cast<ctkWorkflowTabWidget*>(workflowWidget))
    {
    tabWidget->addStepArea(testStep3, 1, "tab2");
    }
  testStep3->setHasButtonBoxWidget(true);

  workflow->addTransition(testStep2, testStep3);
  
  // restart the workflow
  workflow->start();

  // second user interaction test
  if (userInteractionSimulator2(app, testStep1, testStep2, testStep3, hideWidgets, workflow, workflowWidget, defaultTime) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  // stop the workflow
  workflow->stop();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // TODO put back once we can have more than one finish step
  // make the second workflow step a finish step as well
//   finishSteps.push_back(testStep2);
//   testStep1->setFinishStepsToHaveButtonsInStepArea(finishSteps);
//   testStep2->setFinishStepsToHaveButtonsInStepArea(finishSteps);
//   testStep3->setFinishStepsToHaveButtonsInStepArea(finishSteps);
// //  workflow->addFinishStep(testStep2);
  
  // // restart the workflow
  // workflow->start();
  // QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  // app.exec();

  // // third user interfaction test
  // if (userInteractionSimulator3(app, testStep1, testStep2, testStep3, workflow,  workflowWidget, defaultTime) == EXIT_FAILURE)
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




