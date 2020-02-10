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
#include <QProcess>

// CTK includes
#include "ctkCommandLineParser.h"
#include "ctkErrorLogFDMessageHandler.h"
#include "ctkErrorLogQtMessageHandler.h"
#include "ctkErrorLogStreamMessageHandler.h"
#include "ctkModelTester.h"

// STL includes
#include <cstdio>
#include <cstdlib>
#include <iostream>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

namespace
{
//-----------------------------------------------------------------------------
bool checkTerminalOutput(const QStringList& expectedMessages)
{
  ctkCommandLineParser parser;
  parser.setArgumentPrefix("--", "-");
  parser.addArgument("test-launcher", "", QVariant::String, "Path to test launcher");
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(QCoreApplication::arguments(), &ok);
  if(!ok)
    {
    std::cerr << "Line " << __LINE__ << " - Failed to parse arguments !" << std::endl;
    return false;
    }
  if (parsedArgs.contains("test-launcher"))
    {
    QString testLauncher = parsedArgs.value("test-launcher").toString();
    if (!QFile::exists(testLauncher))
      {
      std::cerr << "Line " << __LINE__ << " - Couldn't find test launcher !\n"
                << "\ttest-launcher:" << qPrintable(testLauncher) << std::endl;
      return false;
      }
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(testLauncher, QStringList() << QCoreApplication::arguments().at(0));
    process.waitForFinished(1000);
    QString output = process.readAll();
    QString errorMsg = checkTextMessages(__LINE__, output.split("\n"), expectedMessages);
    if (!errorMsg.isEmpty())
      {
      printErrorMessage(errorMsg);
      return false;
      }
    process.waitForFinished(1000);
    }
  return true;
}
}

//-----------------------------------------------------------------------------
int ctkErrorLogModelTerminalOutputTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);

  QString fdMessage0("This is stdout message");
  QString fdMessage1("This is stderr message");
  QString qtMessage0("This is a qDebug message");
  QString qtMessage1("This is a qWarning message");
  QString qtMessage2("This is a qCritical message");
  QString stdMessage0("This is a std::cerr message");
  QString stdMessage1("This is a std::cout message");

  QStringList expectedMessages;
  expectedMessages << fdMessage0 << fdMessage1
                   << qtMessage0 << qtMessage1 << qtMessage2
                   << stdMessage0 << stdMessage1;

  // Since the order of the messages outputed on the terminal is not deterministic,
  // let's just make sure that all messages have been displayed on the terminal
  // independently of their order.
  if (!checkTerminalOutput(expectedMessages))
    {
    return EXIT_FAILURE;
    }

  QString errorMsg;

  ctkErrorLogModel model;
  ctkErrorLogTerminalOutput::TerminalOutputs currentTerminalOutputEnabled = model.terminalOutputs();
  errorMsg = checkBoolean(__LINE__, "TerminalOutputEnabled",
                          currentTerminalOutputEnabled, ctkErrorLogTerminalOutput::None);
  if (!errorMsg.isEmpty())
    {
    model.disableAllMsgHandler();
    printErrorMessage(errorMsg);
    printTextMessages(model);
    return EXIT_FAILURE;
    }

  model.setTerminalOutputs(ctkErrorLogTerminalOutput::All);

  currentTerminalOutputEnabled = model.terminalOutputs();
  errorMsg = checkBoolean(__LINE__, "TerminalOutputEnabled",
                          currentTerminalOutputEnabled, ctkErrorLogTerminalOutput::All);
  if (!errorMsg.isEmpty())
    {
    model.disableAllMsgHandler();
    printErrorMessage(errorMsg);
    printTextMessages(model);
    return EXIT_FAILURE;
    }

  ctkModelTester modelTester;
  modelTester.setVerbose(false);

  try
    {
    modelTester.setModel(&model);

    // Monitor Qt messages
    model.registerMsgHandler(new ctkErrorLogQtMessageHandler);
    model.setMsgHandlerEnabled(ctkErrorLogQtMessageHandler::HandlerName, true);

    // Monitor Stream messages
    model.registerMsgHandler(new ctkErrorLogStreamMessageHandler);
    model.setMsgHandlerEnabled(ctkErrorLogStreamMessageHandler::HandlerName, true);

    // Monitor FD messages
    model.registerMsgHandler(new ctkErrorLogFDMessageHandler);
    model.setMsgHandlerEnabled(ctkErrorLogFDMessageHandler::HandlerName, true);

    fprintf(stdout, "%s\n", qPrintable(fdMessage0));
    fflush(stdout);

    qDebug().nospace() << qUtf8Printable(qtMessage0);

    std::cerr << qPrintable(stdMessage0) << std::endl;

    qWarning().nospace() << qUtf8Printable(qtMessage1);

    fprintf(stderr, "%s\n", qPrintable(fdMessage1));
    fflush(stderr);

    std::cout << qPrintable(stdMessage1) << std::endl;

    qCritical().nospace() << qUtf8Printable(qtMessage2);

    // Give enough time to the ErrorLogModel to consider the queued messages.
    processEvents(1000);

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedMessages.count());
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }

    errorMsg = checkTextMessages(__LINE__, model, expectedMessages);
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
