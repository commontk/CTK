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

#include "ctkErrorLogAbstractMessageHandler.h"

// Qt includes
#include <QHash>
#include <QDateTime>

// CTK includes
#include "ctkErrorLogContext.h"

// --------------------------------------------------------------------------
// ctkErrorLogAbstractMessageHandlerPrivate

// --------------------------------------------------------------------------
class ctkErrorLogAbstractMessageHandlerPrivate
{
public:
  ctkErrorLogAbstractMessageHandlerPrivate();
  ~ctkErrorLogAbstractMessageHandlerPrivate();

  bool                        Enabled;
  QString                     HandlerPrettyName;

  // Use "int" instead of "ctkErrorLogModel::TerminalOutput" to avoid compilation warning ...
  // qhash.h:879: warning: passing 'ctkErrorLogModel::TerminalOutput' chooses 'int' over 'uint' [-Wsign-promo]
  QHash<int, ctkErrorLogTerminalOutput*> TerminalOutputs;
};

// --------------------------------------------------------------------------
ctkErrorLogAbstractMessageHandlerPrivate::
ctkErrorLogAbstractMessageHandlerPrivate()
  : Enabled(false)
{
}

// --------------------------------------------------------------------------
ctkErrorLogAbstractMessageHandlerPrivate::~ctkErrorLogAbstractMessageHandlerPrivate()
{
}

// --------------------------------------------------------------------------
// ctkErrorLogAbstractMessageHandlerPrivate methods

// --------------------------------------------------------------------------
ctkErrorLogAbstractMessageHandler::ctkErrorLogAbstractMessageHandler()
  : Superclass(), d_ptr(new ctkErrorLogAbstractMessageHandlerPrivate)
{
}

// --------------------------------------------------------------------------
ctkErrorLogAbstractMessageHandler::~ctkErrorLogAbstractMessageHandler()
{
}

// --------------------------------------------------------------------------
QString ctkErrorLogAbstractMessageHandler::handlerPrettyName()const
{
  Q_D(const ctkErrorLogAbstractMessageHandler);
  if (d->HandlerPrettyName.isEmpty())
    {
    return this->handlerName();
    }
  else
    {
    return d->HandlerPrettyName;
    }
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractMessageHandler::setHandlerPrettyName(const QString& newHandlerPrettyName)
{
  Q_D(ctkErrorLogAbstractMessageHandler);
  d->HandlerPrettyName = newHandlerPrettyName;
}

// --------------------------------------------------------------------------
bool ctkErrorLogAbstractMessageHandler::enabled()const
{
  Q_D(const ctkErrorLogAbstractMessageHandler);
  return d->Enabled;
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractMessageHandler::setEnabled(bool value)
{
  Q_D(ctkErrorLogAbstractMessageHandler);
  if (value == d->Enabled)
    {
    return;
    }
  this->setEnabledInternal(value);
  d->Enabled = value;
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractMessageHandler::handleMessage(const QString& threadId,
                                                      ctkErrorLogLevel::LogLevel logLevel,
                                                      const QString& origin,
                                                      const ctkErrorLogContext& logContext,
                                                      const QString &text)
{
  Q_D(ctkErrorLogAbstractMessageHandler);
  if (logLevel <= ctkErrorLogLevel::Info)
    {
    if(d->TerminalOutputs.contains(ctkErrorLogTerminalOutput::StandardOutput))
      {
      d->TerminalOutputs.value(ctkErrorLogTerminalOutput::StandardOutput)->output(text);
      }
    }
  else
    {
    if(d->TerminalOutputs.contains(ctkErrorLogTerminalOutput::StandardError))
      {
      d->TerminalOutputs.value(ctkErrorLogTerminalOutput::StandardError)->output(text);
      }
    }
  emit this->messageHandled(QDateTime::currentDateTime(), threadId, logLevel, origin, logContext, text);
}

// --------------------------------------------------------------------------
ctkErrorLogTerminalOutput* ctkErrorLogAbstractMessageHandler::terminalOutput(
    ctkErrorLogTerminalOutput::TerminalOutput terminalOutputType)const
{
  Q_D(const ctkErrorLogAbstractMessageHandler);
  if(d->TerminalOutputs.contains(terminalOutputType))
    {
    return d->TerminalOutputs.value(terminalOutputType);
    }
  return 0;
}

// --------------------------------------------------------------------------
void ctkErrorLogAbstractMessageHandler::setTerminalOutput(
    ctkErrorLogTerminalOutput::TerminalOutput terminalOutputType, ctkErrorLogTerminalOutput* terminalOutput)
{
  Q_D(ctkErrorLogAbstractMessageHandler);
  d->TerminalOutputs.insert(terminalOutputType, terminalOutput);
}
