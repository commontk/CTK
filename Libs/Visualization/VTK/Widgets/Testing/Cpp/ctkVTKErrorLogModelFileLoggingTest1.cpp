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
  vtkDebugWithObjectMacro(object.GetPointer(), "This is a VTK debug message");
  vtkWarningWithObjectMacro(object.GetPointer(), "This is a VTK warning message");
  vtkErrorWithObjectMacro(object.GetPointer(), "This is a VTK error message");

  // Give enough time to the ErrorLogModel to consider the queued messages.
  processEvents(1000);

  model.disableAllMsgHandler();

  QStringList logLines = readFile(logFilePath);

  QString expectedLogEntryPatternTemplate(
        "^\\[%1\\]\\[VTK\\] [0-9\\.\\s\\:]+ \\[vtkObject \\(0x[a-zA-B0-9]+\\)\\] "
        "\\(.+ctkVTKErrorLogModelFileLoggingTest1\\.cpp\\:%2\\) \\- %3$");

  {
    int entryIndex = 0;
    QRegExp regexp(expectedLogEntryPatternTemplate.arg("WARNING").arg(66).arg("This is a VTK warning message"));
    if (!regexp.exactMatch(logLines.at(entryIndex)))
      {
      printErrorMessage(
            QString("Line %1 - Log entry %2 does NOT math expected regular expression.\n\tLogEntry: %3\n\tRegExp: %4").
                arg(__LINE__).arg(entryIndex).arg(logLines.at(entryIndex)).arg(regexp.pattern()));
      return EXIT_FAILURE;
      }
  }
  {
    int entryIndex = 1;
    QRegExp regexp(expectedLogEntryPatternTemplate.arg("ERROR").arg(67).arg("This is a VTK error message"));
    if (!regexp.exactMatch(logLines.at(entryIndex)))
      {
      printErrorMessage(
            QString("Line %1 - Log entry %2 does NOT math expected regular expression.\n\tLogEntry: %3\n\tRegExp: %4").
                arg(__LINE__).arg(entryIndex).arg(logLines.at(entryIndex)).arg(regexp.pattern()));
      return EXIT_FAILURE;
      }
  }

  return EXIT_SUCCESS;
}
