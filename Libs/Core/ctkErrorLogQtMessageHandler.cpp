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

// CTK includes
#include "ctkErrorLogQtMessageHandler.h"

// STD includes
#include <iostream>

//------------------------------------------------------------------------------
QString ctkErrorLogQtMessageHandler::HandlerName = QLatin1String("Qt");

//------------------------------------------------------------------------------
//Q_DECLARE_METATYPE(QPointer<ctkErrorLogQtMessageHandler>)
Q_DECLARE_METATYPE(ctkErrorLogQtMessageHandler*)

// --------------------------------------------------------------------------
ctkErrorLogQtMessageHandler::ctkErrorLogQtMessageHandler() : Superclass()
{
  this->SavedQtMessageHandler = 0;

  QCoreApplication * coreApp = QCoreApplication::instance();

  // Keep track of all instantiated ctkErrorLogModel
  QList<QVariant> handlers = coreApp->property("ctkErrorLogQtMessageHandlers").toList();
  handlers << QVariant::fromValue(this);
  //handlers << QVariant::fromValue(QPointer<ctkErrorLogQtMessageHandler>(this));
  coreApp->setProperty("ctkErrorLogQtMessageHandlers", handlers);
}

namespace
{
//------------------------------------------------------------------------------
void ctkErrorLogModelQtMessageOutput(QtMsgType type, const char *msg)
{
  // Warning: To avoid inifinite loop, do not use Q_ASSERT in this function.
  if (QString(msg).isEmpty())
    {
    return;
    }
  ctkErrorLogModel::LogLevel level = ctkErrorLogModel::Unknown;
  if (type == QtDebugMsg)
    {
    level = ctkErrorLogModel::Debug;
    }
  else if (type == QtWarningMsg)
    {
    level = ctkErrorLogModel::Warning;
    }
  else if (type == QtCriticalMsg)
    {
    level = ctkErrorLogModel::Critical;
    }
  else if (type == QtFatalMsg)
    {
    level = ctkErrorLogModel::Fatal;
    }

  QCoreApplication * coreApp = QCoreApplication::instance();
  QList<QVariant> handlers = coreApp->property("ctkErrorLogQtMessageHandlers").toList();
  foreach(QVariant v, handlers)
    {
    ctkErrorLogQtMessageHandler* handler = v.value<ctkErrorLogQtMessageHandler*>();
    Q_ASSERT(handler);
//    //QPointer<ctkErrorLogQtMessageHandler> handler = v.value<QPointer<ctkErrorLogQtMessageHandler> >();
//    //if(handler.isNull())
//    //  {
//    //  continue;
//    //  }
    if (!handler->errorLogModel())
      {
      std::cout << "ErrorLogModel is Null !" << std::endl;
      return;
      }
    handler->errorLogModel()->addEntry(level, handler->handlerPrettyName(), msg);
    }
}
}

// --------------------------------------------------------------------------
QString ctkErrorLogQtMessageHandler::handlerName()const
{
  return ctkErrorLogQtMessageHandler::HandlerName;
}

// --------------------------------------------------------------------------
void ctkErrorLogQtMessageHandler::setEnabledInternal(bool value)
{
  if (value)
    {
    this->SavedQtMessageHandler = qInstallMsgHandler(ctkErrorLogModelQtMessageOutput);
    }
  else
    {
    qInstallMsgHandler(this->SavedQtMessageHandler);
    }
}
