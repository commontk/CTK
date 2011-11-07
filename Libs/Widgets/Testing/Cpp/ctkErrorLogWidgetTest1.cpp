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
#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTime>
#include <QTimer>

// CTK includes
#include <ctkErrorLogQtMessageHandler.h>
#include <ctkErrorLogStatusMessageHandler.h>
#include <ctkErrorLogStreamMessageHandler.h>
#include <ctkErrorLogWidget.h>

// STD includes
#include <cstdlib>
#include <iostream>

// Helper functions
#include "Testing/Cpp/ctkErrorLogModelTestHelper.cpp"

//-----------------------------------------------------------------------------
int ctkErrorLogWidgetTest1(int argc, char * argv [])
{
  QApplication app(argc, argv);

  QMainWindow mainWindow;
  mainWindow.show();

  QString errorMsg;

  ctkErrorLogModel model;

  ctkErrorLogWidget widget;

  // --------------------------------------------------------------------------
  // Monitor application StatusBar messages
    {
    model.registerMsgHandler(new ctkErrorLogStatusMessageHandler(&mainWindow));
    model.setMsgHandlerEnabled(ctkErrorLogStatusMessageHandler::HandlerName, true);

    QString expectedStatusText1 = QLatin1String("This is a status message");
    mainWindow.statusBar()->showMessage(expectedStatusText1);

    // Give enough time to the ErrorLogModel to consider the queued messages.
    processEvents(1000);

    QStringList expectedStatusMessages;
    expectedStatusMessages << expectedStatusText1;

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedStatusMessages.count());
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }

    errorMsg = checkTextMessages(__LINE__, model, expectedStatusMessages);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }

    // Clear
    model.clear();

    // Disable Qt messages monitoring
    model.setMsgHandlerEnabled(ctkErrorLogStatusMessageHandler::HandlerName, false);

    mainWindow.statusBar()->showMessage(
          QLatin1String("This is status message shouldn't appear in ErrorLogModel"));

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

    std::cout << "This is a Cout message" << std::endl;
    std::cerr << "This is a Cerr message" << std::endl;
    }

  // --------------------------------------------------------------------------
  // Monitor Qt messages
    {
    model.registerMsgHandler(new ctkErrorLogQtMessageHandler);
    model.setMsgHandlerEnabled(ctkErrorLogQtMessageHandler::HandlerName, true);

    qDebug() << "This is a Qt Debug message";
    qWarning() << "This is a Qt Warning message";
    qCritical() << "This is a Qt Critical message";
    }

  QTime start = QTime::currentTime();

  for (int i = 0; i < 500; ++i)
    {
    qDebug() << "This is a Qt Debug message - id:0 - group:" << i;
    std::cout << "This is a Cout message - id:1 - group:" << i << std::endl;
    qWarning() << "This is a Qt Warning message - id:2 - group:" << i;
    std::cerr << "This is a Cerr message - id:3 - group:" << i << std::endl;
    qCritical() << "This is a Qt Critical message - id:4 - group:" << i;
    qDebug() << "This is a Qt Debug message - id:5 - group:" << i;
    std::cout << "This is a Cout message - id:6 - group:" << i << std::endl;
    qWarning() << "This is a Qt Warning message - id:7 - group:" << i;
    std::cerr << "This is a Cerr message - id:8 - group:" << i << std::endl;
    qCritical() << "This is a Qt Critical message - id:9 - group:" << i;
    }

  fprintf(stdout, "Msg handling time: %d\n", start.msecsTo(QTime::currentTime()));
  fflush(stdout);

  model.setLogEntryGrouping(true);

  start = QTime::currentTime();

  for (int i = 0; i < 1000; ++i)
    {
    std::cout << "This is a Cout message - id:1 - group:" << i << std::endl;
    std::cout << "This is a Cout message - id:2 - group:" << i << std::endl;
    std::cout << "This is a Cout message - id:3 - group:" << i << std::endl;
    std::cout << "This is a Cout message - id:4 - group:" << i << std::endl;
    }

  fprintf(stdout, "Msg handling time: %d\n", start.msecsTo(QTime::currentTime()));
  fflush(stdout);

  widget.setErrorLogModel(&model);
  widget.setErrorLogModel(0);
  widget.setErrorLogModel(&model);
  widget.setErrorLogModel(&model);
  widget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(1000, &app, SLOT(quit()));
    }

  return app.exec();
}
