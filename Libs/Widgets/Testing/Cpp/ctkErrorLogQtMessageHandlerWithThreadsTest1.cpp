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

// STL includesQList
#include <cstdlib>
#include <iostream>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

namespace
{
//-----------------------------------------------------------------------------
class LogQtMessageThread : public LogMessageThread
{
public:
  LogQtMessageThread(int id, int maxIteration) : LogMessageThread(id, maxIteration){}

  virtual void logMessage(const QDateTime& dateTime, int threadId, int counterIdx)
  {
    QString msg = QString("counterIdx:%1 - %2 - Message from thread: %3\n")
        .arg(counterIdx).arg(dateTime.toString()).arg(threadId);

    qDebug().nospace() << qUtf8Printable(msg);
    qWarning().nospace() << qUtf8Printable(msg);
    qCritical().nospace() << qUtf8Printable(msg);
  }
};

}

//-----------------------------------------------------------------------------
int ctkErrorLogQtMessageHandlerWithThreadsTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);

  ctkErrorLogModel model;
  ctkModelTester modelTester;
  modelTester.setVerbose(false);

  try
  {
    modelTester.setModel(&model);

    // --------------------------------------------------------------------------
    // Monitor Qt messages

    model.registerMsgHandler(new ctkErrorLogQtMessageHandler);
    model.setMsgHandlerEnabled(ctkErrorLogQtMessageHandler::HandlerName, true);

    int threadCount = 10;
    int maxIteration = 5;
    int messagesPerIteration = 3;
    startLogMessageThreads<LogQtMessageThread>(threadCount, maxIteration);

    // Give enough time for the threads to send their messages
    QTimer::singleShot(1500, qApp, SLOT(quit()));
    app.exec();

    int expectedMessageCount = threadCount * maxIteration * messagesPerIteration;
    QString errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedMessageCount);
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
