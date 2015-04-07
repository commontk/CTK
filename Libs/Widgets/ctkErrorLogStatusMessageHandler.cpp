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
#include <QDateTime>
#include <QDebug>
#include <QMainWindow>
#include <QStatusBar>
#include <QThread>

// CTK includes
#include "ctkErrorLogContext.h"
#include "ctkErrorLogStatusMessageHandler.h"
#include "ctkUtils.h"

// --------------------------------------------------------------------------
QString ctkErrorLogStatusMessageHandler::HandlerName = QLatin1String("Status");

// --------------------------------------------------------------------------
ctkErrorLogStatusMessageHandler::ctkErrorLogStatusMessageHandler(QMainWindow * mainWindow) :
  ctkErrorLogAbstractMessageHandler()
{
  this->MainWindow = QPointer<QMainWindow>(mainWindow);
}

// --------------------------------------------------------------------------
QString ctkErrorLogStatusMessageHandler::handlerName()const
{
  return ctkErrorLogStatusMessageHandler::HandlerName;
}

// --------------------------------------------------------------------------
void ctkErrorLogStatusMessageHandler::setEnabledInternal(bool value)
{
  if (value)
    {
    if (this->MainWindow.isNull())
      {
      qCritical() << "ctkErrorLogStatusMessageHandler - "
                     " Failed to enable ctkErrorLogStatusMessageHandler - "
                     "QMainWindow passed to the constructor is Null !";
      return;
      }
    if (!this->MainWindow->statusBar())
      {
      qCritical() << "ctkErrorLogStatusMessageHandler - Failed to enable the message handler: "
                     "There is no StatusBar associated with QMainWindow" << this->MainWindow;
      return;
      }
    connect(this->MainWindow->statusBar(), SIGNAL(messageChanged(QString)),
            this, SLOT(statusBarMessageChanged(QString)));
    }
  else
    {
    if (!this->MainWindow.isNull())
      {
      disconnect(this->MainWindow->statusBar(), SIGNAL(messageChanged(QString)),
                 this, SLOT(statusBarMessageChanged(QString)));
      }
    }
}

//------------------------------------------------------------------------------
void ctkErrorLogStatusMessageHandler::statusBarMessageChanged(const QString& text)
{
  if (text.isEmpty())
    {
    return;
    }
  this->handleMessage(
        ctk::qtHandleToString(QThread::currentThreadId()),
        ctkErrorLogLevel::Status, this->handlerPrettyName(), ctkErrorLogContext(text), text);
}
