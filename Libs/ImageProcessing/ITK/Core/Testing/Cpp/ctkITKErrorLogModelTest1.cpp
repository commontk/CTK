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

#ifdef __GNUC__
// Disable warnings related to 'itkSmartPointer.h' file
// See http://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
// Note: Ideally the incriminated functions and macros should be fixed upstream ...
# pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

// CTK includes
#include "ctkErrorLogModel.h"
#include "ctkITKErrorLogMessageHandler.h"
#include "ctkModelTester.h"

// ITK includes
#include <itkOutputWindow.h>

// STL includes
#include <cstdlib>
#include <iostream>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

//-----------------------------------------------------------------------------
int ctkITKErrorLogModelTest1(int argc, char * argv [])
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
    // Monitor ITK messages

    model.registerMsgHandler(new ctkITKErrorLogMessageHandler);
    model.setMsgHandlerEnabled(ctkITKErrorLogMessageHandler::HandlerName, true);

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      return EXIT_FAILURE;
      }

    QString itkMessage0("This is a ITK debug message");
    itk::OutputWindowDisplayDebugText(qPrintable(itkMessage0));

    QString itkMessage1("This is a ITK warning message");
    itk::OutputWindowDisplayWarningText(qPrintable(itkMessage1));

    QString itkMessage2("This is a ITK error message");
    itk::OutputWindowDisplayErrorText(qPrintable(itkMessage2));

    // Give enough time to the ErrorLogModel to consider the queued messages.
    processEvents(1000);

    QStringList expectedITKMessages;
    expectedITKMessages << itkMessage0 << itkMessage1 << itkMessage2;

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedITKMessages.count());
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }

    errorMsg = checkTextMessages(__LINE__, model, expectedITKMessages);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }

    // Check if handler can be enabled / disabled multiple times in a row
    for (int idx = 0; idx < 3; ++idx)
      {
      model.setMsgHandlerEnabled(ctkITKErrorLogMessageHandler::HandlerName, false);
      model.setMsgHandlerEnabled(ctkITKErrorLogMessageHandler::HandlerName, true);
      }

    // Clear
    model.clear();

    // Disable ITK messages monitoring
    model.setMsgHandlerEnabled(ctkITKErrorLogMessageHandler::HandlerName, false);

    itk::OutputWindowDisplayDebugText("This ITK debug message should appear in the console");
    itk::OutputWindowDisplayWarningText("This ITK warning message should appear in the console");
    itk::OutputWindowDisplayErrorText("This ITK error message should appear in the console");

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
