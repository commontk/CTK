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
#include "ctkErrorLogQtMessageHandler.h"
#include "ctkModelTester.h"

// STL includes
#include <cstdlib>
#include <iostream>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

//-----------------------------------------------------------------------------
int ctkErrorLogModelEntryGroupingTest1(int argc, char * argv [])
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
    // Monitor Qt messages
    model.registerMsgHandler(new ctkErrorLogQtMessageHandler);
    model.setMsgHandlerEnabled(ctkErrorLogQtMessageHandler::HandlerName, true);

    // --------------------------------------------------------------------------
    // Test LogEntryGrouping

    model.setLogEntryGrouping(true);

    QString qtMessage0("This is a qDebug message - 1");
    qDebug().nospace() << qUtf8Printable(qtMessage0);

    QString qtMessage0b("This is a qDebug message - 2");
    qDebug().nospace() << qUtf8Printable(qtMessage0b);

    QString qtMessage1("This is a qWarning message");
    qWarning().nospace() << qUtf8Printable(qtMessage1);

    QString qtMessage2("This is a qCritical message - 1");
    qCritical().nospace() << qUtf8Printable(qtMessage2);

    QString qtMessage2b("This is a qCritical message - 2");
    qCritical().nospace() << qUtf8Printable(qtMessage2b);

    // Give enough time to the ErrorLogModel to consider the queued messages.
    processEvents(1000);

    QStringList expectedQtMessages;
    expectedQtMessages << qtMessage0.append("\n").append(qtMessage0b)
                       << qtMessage1
                       << qtMessage2.append("\n").append(qtMessage2b);

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
