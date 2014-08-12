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
#include <QDir>
#include <QTemporaryFile>

// CTK includes
#include "ctkVTKErrorLogMessageHandler.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObject.h>
#include <vtkOutputWindow.h>

// STL includes
#include <cstdlib>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

//-----------------------------------------------------------------------------
int ctkVTKErrorLogModelFileLoggingTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);
  ctkErrorLogModel model;
  model.setFileLoggingEnabled(true);

  // Create log file
  QTemporaryFile logFile(QDir::tempPath() + "/ctkVTKErrorLogModelFileLoggingTest1.XXXXXX");
  logFile.setAutoRemove(false);
  logFile.open();
  logFile.close();
  QString logFilePath = logFile.fileName();

  model.setFilePath(logFilePath);

  // Monitor VTK messages
  model.registerMsgHandler(new ctkVTKErrorLogMessageHandler);
  model.setMsgHandlerEnabled(ctkVTKErrorLogMessageHandler::HandlerName, true);

  vtkNew<vtkObject> object;

  // VTK messages
  //QString vtkMessage0("This is a VTK debug message");
  vtkDebugWithObjectMacro(object.GetPointer(), "This is a VTK debug message");

  //QString vtkMessage1("This is a VTK warning message");
  vtkWarningWithObjectMacro(object.GetPointer(), "This is a VTK warning message");

  //QString vtkMessage2("This is a VTK error message");
  vtkErrorWithObjectMacro(object.GetPointer(), "This is a VTK error message");

  // Give enough time to the ErrorLogModel to consider the queued messages.
  processEvents(1000);

  return EXIT_SUCCESS;
}
