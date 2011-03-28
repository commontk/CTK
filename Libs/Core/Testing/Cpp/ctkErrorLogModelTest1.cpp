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
#include <QTimer>

// CTK includes
#include "ctkErrorLogModel.h"
#include "ctkErrorLogFDMessageHandler.h"
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
QString checkInteger(int line, const char* valueName, int current, int expected)
{
  if (current != expected)
    {
    QString errorMsg("Line %1 - Expected %2: %3 - Current %4: %5\n");
    return errorMsg.arg(line).arg(valueName).
        arg(expected).arg(valueName).arg(current);
    }
  return QString();
}

} // end namespace

//-----------------------------------------------------------------------------
int ctkErrorLogModelTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);
  ctkErrorLogModel model;
  ctkModelTester modelTester;
  modelTester.setVerbose(false);
  QString errorMsg;

  QStringList enabledMessageHandlers = model.msgHandlerEnabled();
  int currentEnabledMessageHandlersCount = enabledMessageHandlers.count();
  errorMsg = checkInteger(__LINE__, "EnabledMessageHandlersCount", currentEnabledMessageHandlersCount, 0);
  if (!errorMsg.isEmpty())
    {
    model.disableAllMsgHandler();
    printErrorMessage(errorMsg);
    return EXIT_FAILURE;
    }

  try
    {
    modelTester.setModel(&model);

    // --------------------------------------------------------------------------
    // Monitor Qt messages
      {
      model.registerMsgHandler(new ctkErrorLogQtMessageHandler);
      model.setMsgHandlerEnabled(ctkErrorLogQtMessageHandler::HandlerName, true);

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        return EXIT_FAILURE;
        }

      QString qtMessage0("This is a qDebug message");
      qDebug().nospace() << qPrintable(qtMessage0);

      QString qtMessage1("This is a qWarning message");
      qWarning().nospace() << qPrintable(qtMessage1);

      QString qtMessage2("This is a qCritical message");
      qCritical().nospace() << qPrintable(qtMessage2);

      QStringList expectedQtMessages;
      expectedQtMessages << qtMessage0 << qtMessage1 << qtMessage2;

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedQtMessages.count());
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        printTextMessages(model);
        return EXIT_FAILURE;
        }

      errorMsg = checkTextMessages(__LINE__, model, expectedQtMessages);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        return EXIT_FAILURE;
        }

      // Check if msgHandlerEnabled() works as expected
      enabledMessageHandlers = model.msgHandlerEnabled();
      currentEnabledMessageHandlersCount = enabledMessageHandlers.count();
      errorMsg = checkInteger(__LINE__, "EnabledMessageHandlersCount", currentEnabledMessageHandlersCount, 1);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        return EXIT_FAILURE;
        }

      // Clear
      model.clear();

      // Disable Qt messages monitoring
      model.setMsgHandlerEnabled(ctkErrorLogQtMessageHandler::HandlerName, false);

      qDebug() << "This qDebug message should appear in the console";
      qWarning() << "This qWarning message should appear in the console";
      qCritical() << "This qCritical message should appear in the console";

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        printTextMessages(model);
        return EXIT_FAILURE;
        }
      }

    // --------------------------------------------------------------------------
    // Monitor Stream messages
      {
      model.registerMsgHandler(new ctkErrorLogStreamMessageHandler);
      model.setMsgHandlerEnabled(ctkErrorLogStreamMessageHandler::HandlerName, true);

      // Make sure Qt message handler is still disabled
      if (model.msgHandlerEnabled(ctkErrorLogQtMessageHandler::HandlerName))
        {
        model.disableAllMsgHandler();
        errorMsg = QLatin1String("Line %1 - Qt message handler should be disabled");
        printErrorMessage(errorMsg.arg(__LINE__));
        return EXIT_FAILURE;
        }

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        printTextMessages(model);
        return EXIT_FAILURE;
        }

      QString streamMessage0("This is a Cout message");
      std::cout << qPrintable(streamMessage0) << std::endl;

      QString streamMessage1("This is a Cerr message");
      std::cerr << qPrintable(streamMessage1) << std::endl;

      QStringList expectedStreamMessages;
      expectedStreamMessages << streamMessage0 << streamMessage1;

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedStreamMessages.count());
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        printTextMessages(model);
        return EXIT_FAILURE;
        }

      errorMsg = checkTextMessages(__LINE__, model, expectedStreamMessages);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        return EXIT_FAILURE;
        }

      // Clear
      model.clear();

      // Disable Stream messages monitoring
      model.setMsgHandlerEnabled(ctkErrorLogStreamMessageHandler::HandlerName, false);

      std::cout << "This std::cout message should appear in the console" << std::endl;
      std::cerr << "This std::cerr message should appear in the console" << std::endl;

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        printTextMessages(model);
        return EXIT_FAILURE;
        }
      }

    // --------------------------------------------------------------------------
    // Monitor FD messages
      {
      model.registerMsgHandler(new ctkErrorLogFDMessageHandler);
      model.setMsgHandlerEnabled(ctkErrorLogFDMessageHandler::HandlerName, true);

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        printTextMessages(model);
        return EXIT_FAILURE;
        }

      QString fdMessage0("This is a stdout");
      fprintf(stdout, "%s", qPrintable(fdMessage0));
      QString fdMessage0b(" message");
      fprintf(stdout, "%s\n", qPrintable(fdMessage0b));
      fdMessage0.append(fdMessage0b);
      fflush(stdout);

//      QString fdMessage1("This is a 2nd stdout message");
//      fprintf(stdout, "%s\n", qPrintable(fdMessage1));
//      fflush(stdout);

      QString fdMessage2("This is a stderr");
      fprintf(stderr, "%s", qPrintable(fdMessage2));
      QString fdMessage2b(" message");
      fprintf(stderr, "%s\n", qPrintable(fdMessage2b));
      fdMessage2.append(fdMessage2b);
      fflush(stderr);

//      QString fdMessage3("This is a 2nd stderr message");
//      fprintf(stderr, "%s\n", qPrintable(fdMessage3));
//      fflush(stderr);

      QStringList expectedFDMessages;
      expectedFDMessages << fdMessage0 /*<< fdMessage1*/ << fdMessage2 /*<< fdMessage3*/;

      // Give enough time to the QFileSystemWatcher used internally by ctkErrorLogFDMessageHandler
      // to consider the updated files.
      QTimer timer;
      timer.setSingleShot(true);
      timer.start(1000);
      while(timer.isActive())
        {
        QCoreApplication::processEvents();
        }

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedFDMessages.count());
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        printTextMessages(model);
        return EXIT_FAILURE;
        }

      errorMsg = checkTextMessages(__LINE__, model, expectedFDMessages);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        return EXIT_FAILURE;
        }

      // Clear
      model.clear();

      // Disable FD messages monitoring
      model.setMsgHandlerEnabled(ctkErrorLogFDMessageHandler::HandlerName, false);

      fprintf(stdout, "%s", "This stdout message should appear in the console\n");
      fprintf(stderr, "%s", "This stderr message should appear in the console\n");
      fflush(stderr);

      errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
      if (!errorMsg.isEmpty())
        {
        model.disableAllMsgHandler();
        printErrorMessage(errorMsg);
        printTextMessages(model);
        return EXIT_FAILURE;
        }
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
