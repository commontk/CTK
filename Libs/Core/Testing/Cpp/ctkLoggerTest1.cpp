/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QPointer>
#include <QStack>

// CTK includes
#include <ctkErrorLogLevel.h>
#include <ctkLogger.h>
#include <ctkCoreTestingMacros.h>
#include <ctkUtils.h>

// STL includes
#include <cstdlib>
#include <iostream>

namespace
{

// ----------------------------------------------------------------------------
class ctkQtMessageHandler : public QObject
{
public:
  typedef ctkQtMessageHandler Self ;
  explicit ctkQtMessageHandler()
  {
    qInstallMessageHandler(ctkQtMessageHandler::messageHandler);
    Self::Handlers.push(this);
  }
  ~ctkQtMessageHandler()
  {
    // Restore previous handler
    qInstallMessageHandler(this->Previous);
    Self::Handlers.pop();
  }

  static ctkQtMessageHandler* current()
  {
    return Self::Handlers.empty() ? nullptr : Self::Handlers.last();
  }

  static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
  {
    Q_UNUSED(context);
    if (!Self::current())
    {
      return;
    }
    Self::current()->Messages.append(QPair<QtMsgType, QString>(type, msg));
  }

  static QStack<QPointer<ctkQtMessageHandler>> Handlers;

  QtMessageHandler Previous{nullptr};
  QList<QPair<QtMsgType, QString>> Messages;
};

QStack<QPointer<ctkQtMessageHandler>> ctkQtMessageHandler::Handlers;

} // end of anonymous namespace

//-----------------------------------------------------------------------------
int TestDefaults();
int TestSetLogLevel();
int TestLogging();
int TestSoftAssert();

// ----------------------------------------------------------------------------
int ctkLoggerTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  CHECK_EXIT_SUCCESS(TestDefaults());
  CHECK_EXIT_SUCCESS(TestSetLogLevel());
  CHECK_EXIT_SUCCESS(TestLogging());
  CHECK_EXIT_SUCCESS(TestSoftAssert());

  return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
int TestDefaults()
{
  ctkLogger logger("Logger.TestDefaults");

#ifndef QT_NO_DEBUG
  CHECK_INT(logger.logLevel(), ctkErrorLogLevel::LogLevel::Debug);
#else
  CHECK_INT(logger.logLevel(), ctkErrorLogLevel::LogLevel::Warning);
#endif
  return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
int TestSetLogLevel()
{
  ctkLogger logger("Logger.TestSetLogLevel");

  // Test setting and getting log level
  logger.setLogLevel(ctkErrorLogLevel::LogLevel::Info);
  CHECK_INT(logger.logLevel(), ctkErrorLogLevel::LogLevel::Info);

  return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
int TestLogging()
{
  ctkLogger logger("Logger.TestLogging");

  logger.setLogLevel(ctkErrorLogLevel::LogLevel::Debug);

  QList<QPair<QtMsgType, QString>> messages;
  {
    ctkQtMessageHandler handler;
    logger.debug("logger.debug");
    logger.info("logger.info");
    logger.trace("logger.trace"); // Not logged because "trace" < "debug" level
    logger.warn("logger.warn");
    logger.error("logger.error");
    logger.fatal("logger.fatal");

    CHECK_NOT_NULL(handler.current());
    messages = handler.current()->Messages;
  }
  CHECK_INT(messages.count(), 5);

  QPair<QtMsgType, QString> message;

  message = messages.takeFirst();
  CHECK_INT(message.first, QtMsgType::QtDebugMsg);
  CHECK_QSTRING(message.second, QString("logger.debug"));

  message = messages.takeFirst();
  CHECK_INT(message.first, QtMsgType::QtInfoMsg);
  CHECK_QSTRING(message.second, QString("logger.info"));

  message = messages.takeFirst();
  CHECK_INT(message.first, QtMsgType::QtWarningMsg);
  CHECK_QSTRING(message.second, QString("logger.warn"));

  message = messages.takeFirst();
  CHECK_INT(message.first, QtMsgType::QtCriticalMsg);
  CHECK_QSTRING(message.second, QString("logger.error"));

  message = messages.takeFirst();
  CHECK_INT(message.first, QtMsgType::QtCriticalMsg);
  CHECK_QSTRING(message.second, QString("logger.fatal"));

  return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
int TestSoftAssert()
{
  ctkLogger logger("Logger.TestSoftAssert");

  QList<QPair<QtMsgType, QString>> messages;
  {
    ctkQtMessageHandler handler;

    // This should log a warning "Assertion `5 == 6` failed ..."
    CTK_SOFT_ASSERT(5 == 6);

    // This should not log anything
    CTK_SOFT_ASSERT(8 == 8);

    CHECK_NOT_NULL(handler.current());
    messages = handler.current()->Messages;
  }
  CHECK_INT(messages.count(), 1);

  QPair<QtMsgType, QString> message;

  message = messages.takeFirst();
  CHECK_INT(message.first, QtMsgType::QtWarningMsg);
  CHECK_BOOL(message.second.startsWith("Assertion `5 == 6` failed"), true);

  return EXIT_SUCCESS;
}
