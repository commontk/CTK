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
#include "ctkBranchingWorkflowStep.h"
#include "ctkExampleDerivedWorkflowStep.h"
#include "ctkWorkflow.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
/*       "simple" s3----s4
//               /        \
//  s0----s1----s2        s7----s8
//               \        /
//     "advanced" s5----s6
*/

//-----------------------------------------------------------------------------
int ctkWorkflowTest3(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);
  int defaultTime = 100;

  // create two steps and the workflow
  ctkWorkflow *workflow = new ctkWorkflow();
  ctkExampleDerivedWorkflowStep* s0 = new ctkExampleDerivedWorkflowStep("Step 0");
  ctkExampleDerivedWorkflowStep* s1 = new ctkExampleDerivedWorkflowStep("Step 1");
  ctkBranchingWorkflowStep* s2 = new ctkBranchingWorkflowStep("Step 2");
  ctkExampleDerivedWorkflowStep* s3 = new ctkExampleDerivedWorkflowStep("Step 3");
  ctkExampleDerivedWorkflowStep* s4 = new ctkExampleDerivedWorkflowStep("Step 4");
  ctkExampleDerivedWorkflowStep* s5 = new ctkExampleDerivedWorkflowStep("Step 5");
  ctkExampleDerivedWorkflowStep* s6 = new ctkExampleDerivedWorkflowStep("Step 6");
  ctkExampleDerivedWorkflowStep* s7 = new ctkExampleDerivedWorkflowStep("Step 7");
  ctkExampleDerivedWorkflowStep* s8 = new ctkExampleDerivedWorkflowStep("Step 8");

  workflow->addTransition(s0, s1);
  workflow->addTransition(s1, s2);
  workflow->addTransition(s2, s3, "simple");
  workflow->addTransition(s3, s4);
  workflow->addTransition(s4, s7);
  workflow->addTransition(s7, s8);
  workflow->addTransition(s2, s5, "advanced");
  workflow->addTransition(s5, s6);
  workflow->addTransition(s6, s7);
  workflow->setInitialStep(s0);

  // test error handling for branching workflows:
  if (workflow->addTransition(s6, s7))
    {
    std::cout << "should not be able to add duplicates of the same transition" << std::endl;
    return EXIT_FAILURE;
    }

  if (workflow->addTransition(s6, s6))
    {
    std::cout << "currently do not support transitions from a step to itself" << std::endl;
    return EXIT_FAILURE;
    }

  if (workflow->addTransition(s2, s5, "simple"))
    {
    std::cout << "should not be able to add multiple transitions with same id" << std::endl;
    return EXIT_FAILURE;
    }

  // test that the transitions are occuring properly
  workflow->start();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // transition to s1
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s1)
    {
    std::cerr << "error transitioning s0->s1" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s2
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s2)
    {
    std::cerr << "error transitioning s1->s2" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s3
  s2->setBranchId("simple");
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s3)
    {
    std::cerr << "*** branch *** error transitioning s2->s3" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s4
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s4)
    {
    std::cerr << "error transitioning s3->s4" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s7
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s7)
    {
    std::cerr << "error transitioning s4->s7" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s8
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s8)
    {
    std::cerr << "error transitioning s7->s8" << std::endl;
    return EXIT_FAILURE;
    }

  // transition back to s7
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s7)
    {
    std::cerr << "error transitioning s8->s7" << std::endl;
    return EXIT_FAILURE;
    }

  // transition back to s4
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s4)
    {
    std::cerr << "*** reverse branch *** error transitioning s7->s4" << std::endl;
    return EXIT_FAILURE;
    }

  // transition back to s3
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s3)
    {
    std::cerr << "error transitioning s4->s3" << std::endl;
    return EXIT_FAILURE;
    }

  // transition back to s2
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s2)
    {
    std::cerr << "error transitioning s3->s2" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s5
  s2->setBranchId("advanced");
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s5)
    {
    std::cerr << "*** branch *** error transitioning s2->s5" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s6
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s6)
    {
    std::cerr << "error transitioning s5->s6" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s7
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s7)
    {
    std::cerr << "error transitioning s6->s7" << std::endl;
    return EXIT_FAILURE;
    }

  // transition to s8
  workflow->goForward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s8)
    {
    std::cerr << "error transitioning s7->s8" << std::endl;
    return EXIT_FAILURE;
    }

  // transition back to s7
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s7)
    {
    std::cerr << "error transitioning s8->s7" << std::endl;
    return EXIT_FAILURE;
    }

  // transition back to s6
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s6)
    {
    std::cerr << "error transitioning s7->s6" << std::endl;
    std::cerr << qPrintable(workflow->currentStep()->id());
    return EXIT_FAILURE;
    }

  // transition back to s5
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s5)
    {
    std::cerr << "error transitioning s6->s5" << std::endl;
    return EXIT_FAILURE;
    }

  // transition back to s2
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s2)
    {
    std::cerr << "error transitioning s5->s2" << std::endl;
    return EXIT_FAILURE;
    }

  // transition back to s1
  workflow->goBackward();
  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();
  if (workflow->currentStep() != s1)
    {
    std::cerr << "error transitioning s2->s1" << std::endl;
    return EXIT_FAILURE;
    }

  workflow->stop();

  int d = workflow->backwardDistanceToStep(s7);
  if (d != 5)
    {
    std::cerr << "error distance between s7->s0, got"<< d << std::endl;
    return EXIT_FAILURE;
    }

  QTimer::singleShot(defaultTime, &app, SLOT(quit()));
  app.exec();

  // handles deletions of the workflow, steps, states and transitions
  delete workflow;

  return EXIT_SUCCESS;
}
