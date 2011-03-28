/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QDebug>
#include <QStringList>

// CTK includes
#include "ctkErrorLogModel.h"
#include "ctkErrorLogQtMessageHandler.h"
#include "ctkErrorLogStreamMessageHandler.h"
#include "ctkModelTester.h"

// STL includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
// Utility function

//-----------------------------------------------------------------------------
QString checkRowCount(int line, int currentRowCount, int expectedRowCount)
{
  if (currentRowCount != expectedRowCount)
    {
    QString errorMsg("Line %1 - Expected rowCount: %2 - Current rowCount: %3\n");
    return errorMsg.arg(line).arg(expectedRowCount).arg(currentRowCount);
    }
  return QString();
}

//-----------------------------------------------------------------------------
QString checkTextMessages(int line, const ctkErrorLogModel& model, const QStringList& expectedMessages)
{
  for(int i=0; i < expectedMessages.count(); ++i)
    {
    QModelIndex descriptionIndex = model.index(i, ctkErrorLogModel::DescriptionColumn);
    QString currentMessage = descriptionIndex.data(ctkErrorLogModel::DescriptionTextRole).toString();
    if (currentMessage.compare(expectedMessages.value(i)) != 0)
      {
      QString errorMsg("Line %1 - Problem with row%2 !\n"
                       "\tExpected message [%3]\n"
                       "\tCurrent message [%4]\n");
      return errorMsg.arg(line).arg(i).arg(expectedMessages.value(i)).arg(currentMessage);
      }
    }
  return QString();
}

//-----------------------------------------------------------------------------
void printTextMessages(const ctkErrorLogModel& model)
{
  fprintf(stdout, "%s", "ErrorLogModel rows:\n");
  QString text("\trow %1 => %2\n");
  for (int i=0; i < model.rowCount(); ++i)
    {
    QString description =
        model.index(0, ctkErrorLogModel::DescriptionColumn).data().toString();
    fprintf(stdout, "%s", qPrintable(text.arg(i).arg(description)));
    }
  fflush(stdout);
}

//-----------------------------------------------------------------------------
void printErrorMessage(const QString& errorMessage)
{
  fprintf(stderr, "%s", qPrintable(errorMessage));
  fflush(stderr);
}

//-----------------------------------------------------------------------------
QString checkBoolean(int line, const char* valueName, bool current, bool expected)
{
  if (current != expected)
    {
    QString errorMsg("Line %1 - Expected %2: %3 - Current %4: %5\n");
    return errorMsg.arg(line).arg(valueName).
        arg(static_cast<int>(expected)).arg(valueName).arg(static_cast<int>(current));
    }
  return QString();
}

} // end namespace

//-----------------------------------------------------------------------------
int ctkErrorLogModelTest3(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);
  QString errorMsg;
    
  ctkErrorLogModel model;
  bool currentTerminalOutputEnabled = model.terminalOutputEnabled();
  errorMsg = checkBoolean(__LINE__, "TerminalOutputEnabled", currentTerminalOutputEnabled, false);
  if (!errorMsg.isEmpty())
    {
    model.disableAllMsgHandler();
    printErrorMessage(errorMsg);
    printTextMessages(model);
    return EXIT_FAILURE;
    }

  model.setTerminalOutputEnabled(true);

  currentTerminalOutputEnabled = model.terminalOutputEnabled();
  errorMsg = checkBoolean(__LINE__, "TerminalOutputEnabled", currentTerminalOutputEnabled, true);
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

    QString message0("This is a qDebug message");
    qDebug().nospace() << qPrintable(message0);

    QString message1("This is a std::cerr message");
    std::cerr << qPrintable(message1) << std::endl;

    QString message2("This is a qWarning message");
    qWarning().nospace() << qPrintable(message2);

    QString message3("This is a std::cout message");
    std::cout << qPrintable(message3) << std::endl;

    QString message4("This is a qCritical message");
    qCritical().nospace() << qPrintable(message4);

    QStringList expectedMessages;
    expectedMessages << message0 << message1
                     << message2 << message3
                     << message4;

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
