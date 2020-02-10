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
#include "ctkErrorLogFDMessageHandler.h"
#include "ctkErrorLogQtMessageHandler.h"
#include "ctkErrorLogStreamMessageHandler.h"

// STL includes
#include <cstdlib>
#include <iostream>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

//-----------------------------------------------------------------------------
int ctkErrorLogModelFileLoggingTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);
  ctkErrorLogModel model;

  // fileLoggingEnabled
  if (!checkBoolean(__LINE__, "FileLoggingEnabled", model.fileLoggingEnabled(), false).isEmpty())
    {
    return EXIT_FAILURE;
    }

  model.setFileLoggingEnabled(true);

  if (!checkBoolean(__LINE__, "FileLoggingEnabled", model.fileLoggingEnabled(), true).isEmpty())
    {
    return EXIT_FAILURE;
    }

  // Create log file
  QTemporaryFile logFile(QDir::tempPath() + "/ctkErrorLogModelFileLoggingTest1.XXXXXX");
  logFile.setAutoRemove(false);
  logFile.open();
  logFile.close();
  QString logFilePath = logFile.fileName();

  // filePath
  if (!checkString(__LINE__, "FilePath", model.filePath(), "").isEmpty())
    {
    return EXIT_FAILURE;
    }

  model.setFilePath(logFilePath);

  if (!checkString(__LINE__, "FilePath", model.filePath(), logFilePath).isEmpty())
    {
    return EXIT_FAILURE;
    }

  // Monitor Qt messages
  model.registerMsgHandler(new ctkErrorLogQtMessageHandler);
  model.setMsgHandlerEnabled(ctkErrorLogQtMessageHandler::HandlerName, true);

  // Monitor Stream messages
  model.registerMsgHandler(new ctkErrorLogStreamMessageHandler);
  model.setMsgHandlerEnabled(ctkErrorLogStreamMessageHandler::HandlerName, true);

  // Monitor FD messages
  model.registerMsgHandler(new ctkErrorLogFDMessageHandler);
  model.setMsgHandlerEnabled(ctkErrorLogFDMessageHandler::HandlerName, true);

  // Qt messages
  QString qtMessage0("This is a qDebug message");
  qDebug().nospace() << qUtf8Printable(qtMessage0);

  QString qtMessage1("This is a qWarning message");
  qWarning().nospace() << qUtf8Printable(qtMessage1);

  QString qtMessage2("This is a qCritical message");
  qCritical().nospace() << qUtf8Printable(qtMessage2);

  // Stream messages
  QString streamMessage0("This is a Cout message");
  std::cout << qPrintable(streamMessage0) << std::endl;

  QString streamMessage1("This is a Cerr message");
  std::cerr << qPrintable(streamMessage1) << std::endl;

  // FD messages
  QString fdMessage0("This is a stdout");
  fprintf(stdout, "%s", qPrintable(fdMessage0));
  QString fdMessage0b(" message");
  fprintf(stdout, "%s\n", qPrintable(fdMessage0b));
  fdMessage0.append(fdMessage0b);
  fflush(stdout);

  // XXX FD messages from stderr and stdout are not always reported in the same order

  //QString fdMessage1("This is a 2nd stdout message");
  //fprintf(stdout, "%s\n", qPrintable(fdMessage1));
  //fflush(stdout);

  QString fdMessage2("This is a stderr");
  fprintf(stderr, "%s", qPrintable(fdMessage2));
  QString fdMessage2b(" message");
  fprintf(stderr, "%s\n", qPrintable(fdMessage2b));
  fdMessage2.append(fdMessage2b);
  fflush(stderr);

  //QString fdMessage3("This is a 2nd stderr message");
  //fprintf(stderr, "%s\n", qPrintable(fdMessage3));
  //fflush(stderr);

  // Give enough time to the ErrorLogModel to consider the queued messages.
  processEvents(1000);

  model.disableAllMsgHandler();

  QList<QStringList> expectedLogEntries;
  expectedLogEntries << (QStringList() << "DEBUG" << "Qt" << qtMessage0);
  expectedLogEntries << (QStringList() << "WARNING" << "Qt" << qtMessage1);
  expectedLogEntries << (QStringList() << "CRITICAL" << "Qt" << qtMessage2);
  expectedLogEntries << (QStringList() << "INFO" << "Stream" << streamMessage0);
  expectedLogEntries << (QStringList() << "CRITICAL" << "Stream" << streamMessage1);
  expectedLogEntries << (QStringList() << "INFO" << "FD" << fdMessage0);
  //expectedLogEntries << (QStringList() << "INFO" << "FD" << fdMessage1);
  expectedLogEntries << (QStringList() << "CRITICAL" << "FD" << fdMessage2);
  //expectedLogEntries << (QStringList() << "CRITICAL" << "FD" << fdMessage3);

  QStringList currentLogEntries = readFile(logFilePath);

  QString expectedLogEntryPatternTemplate("^\\[%1\\]\\[%2\\] [0-9\\.\\s\\:]+ \\[\\] \\(unknown\\:0\\) \\- %3$");
  for(int entryIndex = 0; entryIndex < expectedLogEntries.size(); ++entryIndex)
    {
    QStringList entry = expectedLogEntries.at(entryIndex);
    QRegExp regexp(expectedLogEntryPatternTemplate.arg(entry.at(0)).arg(entry.at(1)).arg(entry.at(2)));
    if (!regexp.exactMatch(currentLogEntries.at(entryIndex)))
      {
      printErrorMessage(
            QString("Line %1 - Log entry %2 does NOT math expected regular expression.\n\tLogEntry: %3\n\tRegExp: %4").
                arg(__LINE__).arg(entryIndex).arg(currentLogEntries.at(entryIndex)).arg(regexp.pattern()));
      return EXIT_FAILURE;
      }
    }

  return EXIT_SUCCESS;
}
