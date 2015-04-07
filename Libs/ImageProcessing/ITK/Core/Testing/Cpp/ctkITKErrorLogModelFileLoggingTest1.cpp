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
#include "ctkITKErrorLogMessageHandler.h"

// VTK includes
#include <itkObject.h>
#include <itkObjectFactory.h>
#include <itkOutputWindow.h>

// STL includes
#include <cstdlib>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

// --------------------------------------------------------------------------
namespace itk
{

class ctkITKFileLoggingTestObject : public Object
{
public:
  /** Standard class typedefs. */
  typedef ctkITKFileLoggingTestObject   Self;
  typedef Object                        Superclass;
  typedef SmartPointer<Self>            Pointer;
  typedef SmartPointer<const Self>      ConstPointer;

  /** Standard New method. */
  itkNewMacro(ctkITKFileLoggingTestObject);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ctkITKFileLoggingTestObject, Object);

  ctkITKFileLoggingTestObject(){}
  ~ctkITKFileLoggingTestObject(){}

  void exerciseITKDebugMacro(const QString& text)
  {
    itkDebugMacro( << qPrintable(text));
  }

  void exerciseITKWarningMacro(const QString& text)
  {
    itkWarningMacro( << qPrintable(text));
  }
};

} // End of itk namespace

//-----------------------------------------------------------------------------
int ctkITKErrorLogModelFileLoggingTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);
  ctkErrorLogModel model;
  model.setFileLoggingEnabled(true);

  // Create log file
  QTemporaryFile logFile(QDir::tempPath() + "/ctkITKErrorLogModelFileLoggingTest1.XXXXXX");
  logFile.setAutoRemove(false);
  logFile.open();
  logFile.close();
  QString logFilePath = logFile.fileName();

  model.setFilePath(logFilePath);

  // Monitor VTK messages
  model.registerMsgHandler(new ctkITKErrorLogMessageHandler);
  model.setMsgHandlerEnabled(ctkITKErrorLogMessageHandler::HandlerName, true);

  itk::ctkITKFileLoggingTestObject::Pointer object = itk::ctkITKFileLoggingTestObject::New();

  QString itkMessage0("This is a ITK debug message");
  object->exerciseITKDebugMacro(itkMessage0);

  QString itkMessage1("This is a ITK warning message");
  object->exerciseITKWarningMacro(itkMessage1);


  // Give enough time to the ErrorLogModel to consider the queued messages.
  processEvents(1000);

  model.disableAllMsgHandler();

  QStringList logLines = readFile(logFilePath);

  QString expectedLogEntryPatternTemplate(
        "^\\[%1\\]\\[ITK\\] [0-9\\.\\s\\:]+ \\[ctkITKFileLoggingTestObject \\(0x[a-zA-B0-9]+\\)\\] "
        "\\(.+ctkITKErrorLogModelFileLoggingTest1\\.cpp\\:%2\\) \\- %3$");

  int entryIndex = 0;
  QRegExp regexp(expectedLogEntryPatternTemplate.arg("WARNING").arg(70).arg("This is a ITK warning message"));
  if (!regexp.exactMatch(logLines.at(entryIndex)))
    {
    printErrorMessage(
          QString("Line %1 - Log entry %2 does NOT math expected regular expression.\n\tLogEntry: %3\n\tRegExp: %4").
              arg(__LINE__).arg(entryIndex).arg(logLines.at(entryIndex)).arg(regexp.pattern()));
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
