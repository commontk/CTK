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
#include <QCoreApplication>
#include <QDebug>

// CTK includes
#include "ctkVTKErrorLogMessageHandler.h"
#include "ctkModelTester.h"

// VTK includes
#include <vtkOutputWindow.h>

// STL includesQList
#include <cstdlib>
#include <iostream>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

//-----------------------------------------------------------------------------
int ctkVTKErrorLogModelTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);

  ctkErrorLogModel model;
  ctkModelTester modelTester;
  modelTester.setVerbose(false);
  QString errorMsg;

  try
    {
    modelTester.setModel(&model);

    // --------------------------------------------------------------------------
    // Monitor VTK messages

    model.registerMsgHandler(new ctkVTKErrorLogMessageHandler);
    model.setMsgHandlerEnabled(ctkVTKErrorLogMessageHandler::HandlerName, true);

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }

    QString vtkMessage0("This is a VTK debug message");
    vtkOutputWindowDisplayDebugText(qPrintable(vtkMessage0));

    QString vtkMessage1("This is a VTK warning message");
    vtkOutputWindowDisplayWarningText(qPrintable(vtkMessage1));

    QString vtkMessage2("This is a VTK error message");
    vtkOutputWindowDisplayErrorText(qPrintable(vtkMessage2));

    // Give enough time to the ErrorLogModel to consider the queued messages.
    processEvents(1000);

    QStringList expectedVTKMessages;
    expectedVTKMessages << vtkMessage0 << vtkMessage1 << vtkMessage2;

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedVTKMessages.count());
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      return EXIT_FAILURE;
      }

    errorMsg = checkTextMessages(__LINE__, model, expectedVTKMessages);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      return EXIT_FAILURE;
      }

    // Check if handler can be enabled / disabled multiple times in a row
    for (int idx = 0; idx < 3; ++idx)
      {
      model.setMsgHandlerEnabled(ctkVTKErrorLogMessageHandler::HandlerName, false);
      model.setMsgHandlerEnabled(ctkVTKErrorLogMessageHandler::HandlerName, true);
      }

    // Clear
    model.clear();

    // Disable VTK messages monitoring
    model.setMsgHandlerEnabled(ctkVTKErrorLogMessageHandler::HandlerName, false);

    vtkOutputWindowDisplayDebugText("This VTK debug message should appear in the console");
    vtkOutputWindowDisplayWarningText("This VTK warning message should appear in the console");
    vtkOutputWindowDisplayErrorText("This VTK error message should appear in the console");

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }
    }
  catch (const char* error)
    {
    model.disableAllMsgHandler();
    std::cerr << error << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
