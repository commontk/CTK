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

// CTK includes
#include "ctkWorkflow.h"
#include "ctkWorkflowTabWidget.h"
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowGroupBox.h"
#include "ctkExampleWorkflowWidgetStepUsingSignalsAndSlots.h"

// STD includes
#include <cstdlib>
#include <iostream>

///
/// Simple of example of how to setup a workflow using custom steps
/// that were created by implementing the step's functions within a
/// class deriving from QObject, and that communicate with the
/// workflow using signals and slots.
/// See: ctkExampleWorkflowWidgetStepUsingSignalsAndSlots for an
/// example of how to setup the custom steps.

//-----------------------------------------------------------------------------
int ctkExampleUseOfWorkflowWidgetUsingSignalsAndSlots(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  // this boolean is used in setHideWidgetsOfNonCurrentSteps() below
  // false: when a widget does not belong to the current step, it is
  // hidden
  // true: when a widget does not belong to the current step, it is
  // shown, but disabled
  bool hideWidgets = false;

  // create the workflow
  ctkWorkflow* workflow = new ctkWorkflow;

  // create the workflow's UI component
  ctkWorkflowTabWidget* workflowWidget = new ctkWorkflowTabWidget;
  workflowWidget->setWorkflow(workflow);
  ctkWorkflowGroupBox* groupBox = workflowWidget->workflowGroupBox();
  groupBox->setPreText("I am some pre-text");
  groupBox->setPostText("I am some post-text");
  groupBox->setHideWidgetsOfNonCurrentSteps(hideWidgets);

  // create and add the first workflow step
  ctkWorkflowWidgetStep* testStep1 = new ctkWorkflowWidgetStep("Step 1");
  testStep1->setName("Step 1");
  testStep1->setDescription("I am in step 1");
  // can specify the name of the tab
  workflowWidget->associateStepWithLabel(testStep1, "name 1");

  // create and add the second workflow step
  ctkWorkflowWidgetStep* testStep2 = new ctkWorkflowWidgetStep("Step 2");
  testStep2->setName("Step 2");
  testStep2->setDescription("I am in step 2");
  // a new tab is automatically created
  workflowWidget->associateStepWithLabel(testStep2, "name 2");

  // create and add a third workflow step
  ctkWorkflowWidgetStep* testStep3 = new ctkWorkflowWidgetStep("Step 3");
  testStep3->setName("Step 3");
  testStep3->setDescription("I am in step 3");
  // can place a step on a tab that was previously created by
  // specifying its index
  workflowWidget->associateStepWithPage(testStep3, 1, "name 3");

  // add the steps to the workflow
  workflow->addTransition(testStep1, testStep2);
  workflow->addTransition(testStep2, testStep3);

  // create the qObjects that implement the required functions for
  // each step, and communicate with the workflow using signals and slots
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject1 =
      new ctkExampleWorkflowWidgetStepUsingSignalsAndSlots(testStep1);
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject2 =
      new ctkExampleWorkflowWidgetStepUsingSignalsAndSlots(testStep2);
  ctkExampleWorkflowWidgetStepUsingSignalsAndSlots* qObject3 =
      new ctkExampleWorkflowWidgetStepUsingSignalsAndSlots(testStep3);

  // set the widget for each qObject
  qObject1->setWidget(testStep1->stepArea());
  qObject2->setWidget(testStep2->stepArea());
  qObject3->setWidget(testStep3->stepArea());

  // use the qObjects for validation
  QObject::connect(testStep1->ctkWorkflowStepQObject(), SIGNAL(invokeValidateCommand(QString)), qObject1, SLOT(validate(QString)));
  QObject::connect(testStep2->ctkWorkflowStepQObject(), SIGNAL(invokeValidateCommand(QString)), qObject2, SLOT(validate(QString)));
  QObject::connect(testStep3->ctkWorkflowStepQObject(), SIGNAL(invokeValidateCommand(QString)), qObject3, SLOT(validate(QString)));

  // use the qObjects for entry processing
  QObject::connect(testStep1->ctkWorkflowStepQObject(), SIGNAL(invokeOnEntryCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)), qObject1, SLOT(onEntry(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));
  QObject::connect(testStep2->ctkWorkflowStepQObject(), SIGNAL(invokeOnEntryCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)), qObject2, SLOT(onEntry(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));
  QObject::connect(testStep3->ctkWorkflowStepQObject(), SIGNAL(invokeOnEntryCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)), qObject3, SLOT(onEntry(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));

  // use the qObjects for exit processing
  QObject::connect(testStep1->ctkWorkflowStepQObject(), SIGNAL(invokeOnExitCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)), qObject1, SLOT(onExit(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));
  QObject::connect(testStep2->ctkWorkflowStepQObject(), SIGNAL(invokeOnExitCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)), qObject2, SLOT(onExit(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));
  QObject::connect(testStep3->ctkWorkflowStepQObject(), SIGNAL(invokeOnExitCommand(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)), qObject3, SLOT(onExit(const ctkWorkflowStep*,ctkWorkflowInterstepTransition::InterstepTransitionType)));

  // use the qObjects for populating the stepWidgetsList
  QObject::connect(testStep1->ctkWorkflowStepQObject(), SIGNAL(invokeCreateUserInterfaceCommand()), qObject1, SLOT(createUserInterface()));
  QObject::connect(qObject1, SIGNAL(createUserInterfaceComplete()), testStep1->ctkWorkflowStepQObject(), SIGNAL(showUserInterfaceComplete()));
  QObject::connect(testStep2->ctkWorkflowStepQObject(), SIGNAL(invokeCreateUserInterfaceCommand()), qObject2, SLOT(createUserInterface()));
  QObject::connect(qObject2, SIGNAL(createUserInterfaceComplete()), testStep2->ctkWorkflowStepQObject(), SIGNAL(showUserInterfaceComplete()));
  QObject::connect(testStep3->ctkWorkflowStepQObject(), SIGNAL(invokeCreateUserInterfaceCommand()), qObject3, SLOT(createUserInterface()));
  QObject::connect(qObject3, SIGNAL(createUserInterfaceComplete()), testStep3->ctkWorkflowStepQObject(), SIGNAL(showUserInterfaceComplete()));

  testStep1->setHasValidateCommand(1);
  testStep1->setHasOnEntryCommand(1);
  testStep1->setHasOnExitCommand(1);
  testStep1->setHasCreateUserInterfaceCommand(1);

  testStep2->setHasValidateCommand(1);
  testStep2->setHasOnEntryCommand(1);
  testStep2->setHasOnExitCommand(1);
  testStep2->setHasCreateUserInterfaceCommand(1);

  testStep3->setHasValidateCommand(1);
  testStep3->setHasOnEntryCommand(1);
  testStep3->setHasOnExitCommand(1);
  testStep3->setHasCreateUserInterfaceCommand(1);

  // testStep1 is the initial step
  workflow->setInitialStep(testStep1);

  // testStep3 will be a finish step
  // - will perform the processing associated with entering and
  // leaving each step, using the default values supplied
  // - if successful: brings you back to the step where you requested
  // to go to the finish step, so that you can begin customization
  // using user inputs if desired
  // - if unsuccessful: leaves you in the step of failure, so that you
  // can attempt to recify things from there; prints an error message
  // at the bottom of the widget.  To see this behavior:
  // 1) "Next" to step 2
  // 2) change step 2's value to something invalid (ex. 0)
  // 3) "Back" to step 1
  // 4) "finish" - attempts to go to step 3, but leaves you in step 2

  // start the workflow
  workflow->start();
  workflowWidget->show();
  // change this value (500) to increase the time that the widget is
  // shown
  QTimer::singleShot(500, &app, SLOT(quit()));
  app.exec();

  // stop the workflow
  workflow->stop();
  QTimer::singleShot(100, &app, SLOT(quit()));
  app.exec();

  // handles deletion of the workflowWidget, workflow, steps, states
  // and transitions
  delete workflowWidget;

  return EXIT_SUCCESS;
}
