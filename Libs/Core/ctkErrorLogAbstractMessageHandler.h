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

#ifndef CTKERRORLOGABSTRACTMESSAGEHANDLER_H
#define CTKERRORLOGABSTRACTMESSAGEHANDLER_H

// Qt includes
#include <QObject>
#include <QDateTime>

// CTK includes
#include "ctkCoreExport.h"
#include "ctkErrorLogLevel.h"
#include "ctkErrorLogTerminalOutput.h"

//------------------------------------------------------------------------------
class ctkErrorLogAbstractMessageHandlerPrivate;
struct ctkErrorLogContext;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkErrorLogAbstractMessageHandler : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  /// Disabled by default.
  ctkErrorLogAbstractMessageHandler();
  virtual ~ctkErrorLogAbstractMessageHandler();

  virtual QString handlerName()const = 0;

  QString handlerPrettyName()const;

  bool enabled()const;
  void setEnabled(bool value);

  void handleMessage(const QString& threadId, ctkErrorLogLevel::LogLevel logLevel,
                     const QString& origin, const ctkErrorLogContext& logContext,
                     const QString &text);

  ctkErrorLogTerminalOutput* terminalOutput(ctkErrorLogTerminalOutput::TerminalOutput terminalOutputType)const;
  void setTerminalOutput(ctkErrorLogTerminalOutput::TerminalOutput terminalOutputType,
                         ctkErrorLogTerminalOutput * terminalOutput);

Q_SIGNALS:
  void messageHandled(const QDateTime& currentDateTime, const QString& threadId,
                      ctkErrorLogLevel::LogLevel logLevel, const QString& origin,
                      const ctkErrorLogContext& logContext, const QString& text);

protected:
  void setHandlerPrettyName(const QString& newHandlerPrettyName);

  virtual void setEnabledInternal(bool value) = 0;

protected:
  QScopedPointer<ctkErrorLogAbstractMessageHandlerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogAbstractMessageHandler)
  Q_DISABLE_COPY(ctkErrorLogAbstractMessageHandler)
};


#endif // CTKERRORLOGABSTRACTMESSAGEHANDLER_H
