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
#include <QCoreApplication>
#include <QTimer>

// CTK includes
#include "ctkExampleDerivedWorkflowStep.h"
#include "ctkWorkflow.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int numberOfTimesEntryExitTest(ctkExampleDerivedWorkflowStep* step1=0, int step1Entry=0, int step1Exit=0, ctkExampleDerivedWorkflowStep* step2=0, int step2Entry=0, int step2Exit=0, ctkExampleDerivedWorkflowStep* step3=0, int step3Entry=0, int step3Exit=0, ctkExampleDerivedWorkflowStep* step4=0, int step4Entry=0, int step4Exit=0)
{
  if (step1)
    {
    if (step1->numberOfTimesRanOnEntry() != step1Entry || step1->numberOfTimesRanOnExit() != step1Exit)
      {
      return 0;
      }
    }

  if (step2)
    {
    if (step2->numberOfTimesRanOnEntry() != step2Entry || step2->numberOfTimesRanOnExit() != step2Exit)
      {
      return 0;
      }
    }

  if (step3)
    {
    if (step3->numberOfTimesRanOnEntry() != step3Entry || step3->numberOfTimesRanOnExit() != step3Exit)
      {
      return 0;
      }
    }

  if (step4)
    {
    if (step4->numberOfTimesRanOnEntry() != step4Entry || step4->numberOfTimesRanOnExit() != step4Exit)
      {
      return 0;
      }
    }

  return 1;
}

//-----------------------------------------------------------------------------
int currentStepAndNumberOfTimesEntryExitTest(ctkWorkflow* workflow, ctkExampleDerivedWorkflowStep* expectedStep, ctkExampleDerivedWorkflowStep* step1, int step1Entry, int step1Exit, ctkExampleDerivedWorkflowStep* step2, int step2Entry, int step2Exit, ctkExampleDerivedWorkflowStep* step3=0, int step3Entry=0, int step3Exit=0, ctkExampleDerivedWorkflowStep* step4=0, int step4Entry=0, int step4Exit=0)
{
  if (expectedStep)
    {
    if (workflow->currentStep() != expectedStep)
      {
      return 0;
      }
    }
  return numberOfTimesEntryExitTest(step1, step1Entry, step1Exit, step2, step2Entry, step2Exit, step3, step3Entry, step3Exit, step4, step4Entry, step4Exit);
}

//-----------------------------------------------------------------------------
int transitionTest(ctkWorkflow* workflow, int defaultTime, ctkExampleDerivedWorkflowStep* expectedStep, ctkExampleDerivedWorkflowStep* step1, int step1Entry, int step1Exit, ctkExampleDerivedWorkflowStep* step2, int step2Entry, int step2Exit, ctkExampleDerivedWorkflowStep* step3=0, int step3Entry=0, int step3Exit=0, ctkExampleDerivedWorkflowStep* step4=0, int step4Entry=0, int step4Exit=0)
{
  QTimer::singleShot(defaultTime, qApp, SLOT(quit()));
  qApp->exec();
  return currentStepAndNumberOfTimesEntryExitTest(workflow, expectedStep, step1, step1Entry, step1Exit, step2, step2Entry, step2Exit, step3, step3Entry, step3Exit, step4, step4Entry, step4Exit);
}

//-----------------------------------------------------------------------------
int testStartWorkflow(ctkWorkflow* workflow, int defaultTime, bool shouldRun, ctkExampleDerivedWorkflowStep* expectedStep=0, ctkExampleDerivedWorkflowStep* step1=0, int step1Entry=0, int step1Exit=0, ctkExampleDerivedWorkflowStep* step2=0, int step2Entry=0, int step2Exit=0, ctkExampleDerivedWorkflowStep* step3=0, int step3Entry=0, int step3Exit=0, ctkExampleDerivedWorkflowStep* step4=0, int step4Entry=0, int step4Exit=0)
{
  workflow->start();
  QTimer::singleShot(defaultTime, qApp, SLOT(quit()));
  qApp->exec();
  if (workflow->isRunning() != shouldRun)
    {
    return 0;
    }
  return currentStepAndNumberOfTimesEntryExitTest(workflow, expectedStep, step1, step1Entry, step1Exit, step2, step2Entry, step2Exit, step3, step3Entry, step3Exit, step4, step4Entry, step4Exit);
}

//-----------------------------------------------------------------------------
int testStopWorkflow(ctkWorkflow* workflow, int defaultTime, ctkExampleDerivedWorkflowStep* step1, int step1Entry, int step1Exit, ctkExampleDerivedWorkflowStep* step2, int step2Entry, int step2Exit, ctkExampleDerivedWorkflowStep* step3=0, int step3Entry=0, int step3Exit=0, ctkExampleDerivedWorkflowStep* step4=0, int step4Entry=0, int step4Exit=0)
{
  workflow->stop();
  QTimer::singleShot(defaultTime, qApp, SLOT(quit()));
  qApp->exec();
  if (workflow->isRunning())
    {
    return 0;
    }
  return numberOfTimesEntryExitTest(step1, step1Entry, step1Exit, step2, step2Entry, step2Exit, step3, step3Entry, step3Exit, step4, step4Entry, step4Exit);
}

//-----------------------------------------------------------------------------
int ctkWorkflowTest1(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);
  int defaultTime = 100;

  // create two steps and the workflow
  ctkWorkflow *workflow = new ctkWorkflow();
  ctkExampleDerivedWorkflowStep *step1 = new ctkExampleDerivedWorkflowStep("Step 1");
  step1->setName("Step 1");
  step1->setDescription("Description for step 1");
  ctkExampleDerivedWorkflowStep *step2 = new ctkExampleDerivedWorkflowStep("Step 2");
  step2->setName("Step 2");
  step2->setDescription("Description for step 2");

  // --------------------------------------------------------------------------
  // try to add a transition for a step with the same id
  ctkExampleDerivedWorkflowStep *step1Duplicated = new ctkExampleDerivedWorkflowStep("Step 1");
  if (workflow->addTransition(step1, step1Duplicated))
    {
    std::cerr << "workflow connected two steps with the same id";
    return EXIT_FAILURE;
    }

  // try to add a transition from a step to itself
  if (workflow->addTransition(step1, step1))
    {
    std::cerr << "workflow connected two steps with the same id";
    return EXIT_FAILURE;
    }

  // --------------------------------------------------------------------------
  // workflow with no steps
  // try erroneously starting with no steps

  if (!testStartWorkflow(workflow, defaultTime, false))
    {
    std::cerr << "empty workflow is running after start()";
    return EXIT_FAILURE;
    }

  // add the first step
  if (!workflow->addTransition(step1, 0))
    {
    std::cerr << "could not add first step";
    return EXIT_FAILURE;
    }

  // try erroneously starting with no initial step
  if (!testStartWorkflow(workflow, defaultTime, false))
    {
    std::cerr << "workflow is running after start() with no initial step";
    return EXIT_FAILURE;
    }

  // --------------------------------------------------------------------------
  // workflow with one step

  // set the initial step (which sets the initial state)
  workflow->setInitialStep(step1);

  // try starting with one step
  if (!testStartWorkflow(workflow, defaultTime, true, step1, step1, 1, 0, step2, 0, 0))
    {
    std::cerr << "workflow is not running after start() with a single step";
    return EXIT_FAILURE;
    }

  // triggering ValidationTransition and TransitionToPreviousStep
  // should keep us in the same step, when there is only one step
  workflow->goForward();
  if (!transitionTest(workflow, defaultTime, step1, step1, 1, 0, step2, 0, 0))
    {
    std::cerr << "error in validation transition in a workflow with a single step";
    return EXIT_FAILURE;
    }

  // transition to the previous step
  workflow->goBackward();
  if (!transitionTest(workflow, defaultTime, step1, step1, 1, 0, step2, 0, 0))
    {
    std::cerr << "error after transition to previous step in a workflow with a single step";
    return EXIT_FAILURE;
    }

  // stop the workflow
  if (!testStopWorkflow(workflow, defaultTime, step1, 1, 1, step2, 0, 0))
    {
    std::cerr << "workflow with one step still running after stop";
    return EXIT_FAILURE;
    }

  // --------------------------------------------------------------------------
  // workflow with two steps

  // add the second step
  if (!workflow->addTransition(step1, step2))
    {
    std::cerr << "could not add second step";
    return EXIT_FAILURE;
    }

  // start the workflow
  if (!testStartWorkflow(workflow, defaultTime, true, step1, step1, 2, 1, step2, 0, 0))
    {
    std::cerr << "workflow is not running after start() with two steps";
    return EXIT_FAILURE;
    }

  // make sure the workflow has the steps
  if (!workflow->hasStep(step1->id()))
    {
    std::cerr << "Step1 not added to workflow";
    return EXIT_FAILURE;
    }

  if (!workflow->hasStep(step2->id()))
    {
    std::cerr << "Step2 not added to workflow";
    return EXIT_FAILURE;
    }

  // if (workflow->numberOfSteps() != 2)
  //   {
  //   std::cerr << "workflow has " << workflow->numberOfSteps() << " steps, not 2";
  //   return EXIT_FAILURE;
  //   }

  // Test that the workflow transitions from processing to validation state
  workflow->goForward();
  if (!transitionTest(workflow, defaultTime, step2, step1, 2, 2, step2, 1, 0))
    {
    std::cerr << "error transitioning to next step in workflow with two steps";
    return EXIT_FAILURE;
    }

  // Test that the workflow transitions back to the previous step
  workflow->goBackward();
  if (!transitionTest(workflow, defaultTime, step1, step1, 3, 2, step2, 1, 1))
    {
    std::cerr << "error transitioning to previous step in workflow with step steps";
    return EXIT_FAILURE;
    }

  // make sure the workflow stops properly
  if (!testStopWorkflow(workflow, defaultTime, step1, 3, 3, step2, 1, 1))
    {
    std::cerr << "workflow with two steps is running after stop()";
    return EXIT_FAILURE;
    }

  // --------------------------------------------------------------------------
  // Step3

  ctkExampleDerivedWorkflowStep *step3 = new ctkExampleDerivedWorkflowStep("Step 3");
  step3->setName("Step 3");
  step3->setDescription("Description for step 3");

  // --------------------------------------------------------------------------
  // Attempt to retrieve the forward or backward steps of step not yet added to the workflow

  if (workflow->forwardSteps(step3).count() != 0)
    {
    std::cerr << "No forward steps should be associated with step3";
    return EXIT_FAILURE;
    }

  if (workflow->backwardSteps(step3).count() != 0)
    {
    std::cerr << "No backward steps should be associated with step3";
    return EXIT_FAILURE;
    }

  // --------------------------------------------------------------------------
  // workflow with three steps

  // add a third step manually
  if (!workflow->addTransition(step2, step3, "", ctkWorkflow::Forward))
    {
    std::cerr << "could not add step 3 with forward transition";
    return EXIT_FAILURE;
    }

  if (!workflow->addTransition(step2, step3, "",  ctkWorkflow::Backward))
    {
    std::cerr << "could not add next transition between step2 and step 3";
    return EXIT_FAILURE;
    }

  if (workflow->forwardSteps(step1).length() != 1
      || workflow->forwardSteps(step1).first() != step2
      || workflow->forwardSteps(step2).length() != 1
      || workflow->forwardSteps(step2).first() != step3
      || workflow->forwardSteps(step3).length() != 0)
    {
    std::cerr << "error in list of forward steps" << std::endl;
    return EXIT_FAILURE;
    }

  if (workflow->backwardSteps(step1).length() != 0
      || workflow->backwardSteps(step2).length() != 1
      || workflow->backwardSteps(step2).first() != step1
      || workflow->backwardSteps(step3).length() != 1
      || workflow->backwardSteps(step3).first() != step2)
    {
    std::cerr << "error in list of backward steps" << std::endl;
    return EXIT_FAILURE;
    }

  if (!workflow->hasStep(step3->id()))
    {
    std::cerr << "Step3 not added to workflow";
    return EXIT_FAILURE;
    }
  // if (workflow->numberOfSteps() != 3)
  //   {
  //   std::cerr << "workflow has " << workflow->numberOfSteps() << " steps, not 2";
  //   return EXIT_FAILURE;
  //   }

  // now that we've stopped and restarted the state machine, we should
  // be back in the initial step (step 1)
  if (!testStartWorkflow(workflow, defaultTime, true, step1, step1, 4, 3, step2, 1, 1, step3, 0, 0))
    {
    std::cerr << "workflow is not running after start() with three steps";
    return EXIT_FAILURE;
    }

  // test to make sure our lists of forward and backwards steps is correct
  if (!workflow->canGoForward(step1)
      || workflow->canGoBackward(step1)
      || !workflow->canGoForward(step2)
      || !workflow->canGoBackward(step2)
      || workflow->canGoForward(step3)
      || !workflow->canGoBackward(step3))
    {
    std::cerr << "error in can go forward/backward" << std::endl;
    return EXIT_FAILURE;
    }

  // Test that the workflow transitions from step1 to step 2 to step 3
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  workflow->goForward();
  if (!transitionTest(workflow, defaultTime, step3, step1, 4, 4, step2, 2, 2, step3, 1, 0))
    {
    std::cerr << "error transitioning to step3 in workflow with three steps";
    return EXIT_FAILURE;
    }

  // Test that the workflow transitions back to the previous step
  workflow->goBackward();
  if (!transitionTest(workflow, defaultTime, step2, step1, 4, 4, step2, 3, 2, step3, 1, 1))
    {
    std::cerr << "error transitioning to previous step in workflow with three steps";
    return EXIT_FAILURE;
    }

  // make sure the workflow stops properly
  if (!testStopWorkflow(workflow, defaultTime, step1, 4, 4, step2, 3, 3, step3, 1, 1))
    {
    std::cerr << "error stopping workflow with three steps";
    return EXIT_FAILURE;
    }

  // --------------------------------------------------------------------------
  // workflow with a finish step (step 3)

  // restart the workflow
  if (!testStartWorkflow(workflow, defaultTime, true, step1, step1, 5, 4, step2, 3, 3, step3, 1, 1))
    {
    std::cerr << "workflow with finish step is not running after start()";
    return EXIT_FAILURE;
    }

  // try to go automatically to step 3
  workflow->goToStep("Step 3");
  if (!transitionTest(workflow, defaultTime, step1, step1, 6, 5, step2, 4, 4, step3, 2, 2))
    {
    std::cerr << "error after going to finish step";
    return EXIT_FAILURE;
    }

  // try to go automatically to step 3 again
  workflow->goToStep("Step 3");
  if (!transitionTest(workflow, defaultTime, step1, step1, 7, 6, step2, 5, 5, step3, 3, 3))
    {
    std::cerr << "error after going to finish step the second time";
    return EXIT_FAILURE;
    }

  // stop workflow
  if (!testStopWorkflow(workflow, defaultTime, step1, 7, 7, step2, 5, 5, step3, 3, 3))
    {
    std::cerr << "error stopping workflow with finish step";
    return EXIT_FAILURE;
    }

  // --------------------------------------------------------------------------
  // workflow with two finishing steps (step3 and step4)
  ctkExampleDerivedWorkflowStep *step4 = new ctkExampleDerivedWorkflowStep("Step 4");
  step4->setName("Step 4");
  step4->setDescription("Description for step 4");
  workflow->addTransition(step3, step4);

  // restart the workflow
  if (!testStartWorkflow(workflow, defaultTime, true, step1, step1, 8, 7, step2, 5, 5, step3, 3, 3, step4, 0, 0))
    {
    std::cerr << "workflow with two finish steps is not running after start()";
    return EXIT_FAILURE;
    }

  // try to go automatically to step 3
  workflow->goToStep("Step 3");
  if (!transitionTest(workflow, defaultTime, step1, step1, 9, 8, step2, 6, 6, step3, 4, 4, step4, 0, 0))
    {
    std::cerr << "error going to the first finish step of two";
    return EXIT_FAILURE;
    }

  // try to go automatically to step 4
  workflow->goToStep("Step 4");
  if (!transitionTest(workflow, defaultTime, step1, step1, 10, 9, step2, 7, 7, step3, 5, 5, step4, 1, 1))
    {
    std::cerr << "error going to the second finish step of two";
    return EXIT_FAILURE;
    }

  // go to step 3 (a finish step)
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  workflow->goForward();
  if (!transitionTest(workflow, defaultTime, step3, step1, 10, 10, step2, 8, 8, step3, 6, 5, step4, 1, 1))
    {
    std::cerr << "error going from step1 to step3";
    return EXIT_FAILURE;
    }

  // try to go automatically to step 4 (another goTo step)
  workflow->goToStep("Step 4");
  if (!transitionTest(workflow, defaultTime, step3, step1, 10, 10, step2, 8, 8, step3, 7, 6, step4, 2, 2))
    {
    std::cerr << "error going from the first finish step to the second finish step";
    return EXIT_FAILURE;
    }

  // go to step 4, and then go forward (should not let you go past last step)
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  workflow->goForward();
  if (!transitionTest(workflow, defaultTime, step4, step1, 10, 10, step2, 8, 8, step3, 7, 7, step4, 3, 2))
    {
    std::cerr << "error going forward past last step - shouldn't let you";
    return EXIT_FAILURE;
    }

  // now try to go from step 4 to step 4 (should loop)
  workflow->goToStep("Step 4");
  if (!transitionTest(workflow, defaultTime, step4, step1, 10, 10, step2, 8, 8, step3, 7, 7, step4, 4, 3))
    {
    std::cerr << "error looping from step 4 to step 4";
    return EXIT_FAILURE;
    }

  // go back to step 3, and then go from step 3 to step 3 (should loop without hitting step4)
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  workflow->goToStep("Step 3");
  if (!transitionTest(workflow, defaultTime, step3, step1, 10, 10, step2, 8, 8, step3, 9, 8, step4, 4, 4))
    {
    std::cerr << "error looping from step 3 to step 3";
    return EXIT_FAILURE;
    }

  // try to go automatically to step 4 and stay there by setting the property goBackToOriginStepUponSuccess to false
  workflow->setGoBackToOriginStepUponSuccess(false);
  workflow->goToStep("Step 4");
  if (!transitionTest(workflow, defaultTime, step4, step1, 10, 10, step2, 8, 8, step3, 9, 9, step4, 5, 4))
    {
    std::cerr << "error staying at step 4 if property goBackToOriginStepUponSuccess is false";
    return EXIT_FAILURE;
    }

  // after, going backwards to step 3,
  // try to go automatically to step 4 with the property goBackToOriginStepUponSuccess set to true
  workflow->setGoBackToOriginStepUponSuccess(true);
  workflow->goBackward(); // now at step3
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  workflow->goToStep("Step 4");
  if (!transitionTest(workflow, defaultTime, step3, step1, 10, 10, step2, 8, 8, step3, 11, 10, step4, 6, 6))
    {
    std::cerr << "error while coming back to step 3 if property goBackToOriginStepUponSuccess is true";
    return EXIT_FAILURE;
    }

  // handles deletions of the workflow, steps, states and transitions
  delete workflow;

  return EXIT_SUCCESS;
}
