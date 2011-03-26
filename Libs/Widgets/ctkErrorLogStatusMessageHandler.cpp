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
#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QStatusBar>

// CTK includes
#include "ctkErrorLogStatusMessageHandler.h"

// --------------------------------------------------------------------------
QString ctkErrorLogStatusMessageHandler::HandlerName = QLatin1String("Status");

// --------------------------------------------------------------------------
ctkErrorLogStatusMessageHandler::ctkErrorLogStatusMessageHandler() :
  QObject(), ctkErrorLogAbstractMessageHandler()
{
}

// --------------------------------------------------------------------------
QString ctkErrorLogStatusMessageHandler::handlerName()const
{
  return ctkErrorLogStatusMessageHandler::HandlerName;
}

// --------------------------------------------------------------------------
void ctkErrorLogStatusMessageHandler::setEnabledInternal(bool value)
{
  QMainWindow * mainWindow = 0;
  foreach(QWidget* widget, qApp->topLevelWidgets())
    {
    mainWindow = qobject_cast<QMainWindow*>(widget);
    if (mainWindow)
      {
      break;
      }
    }
  if (!mainWindow)
    {
    qCritical() << "ctkErrorLogStatusMessageHandler - "
                   " QMainWindow object should be instantiated before enabling ctkErrorLogStatusMessageHandler";
    return;
    }
  if (!mainWindow->statusBar())
    {
    qCritical() << "ctkErrorLogStatusMessageHandler - Failed to enable the message handler: "
                   "There is no StatusBar associated with QMainWindow" << mainWindow;
    return;
    }
  if (value)
    {
    connect(mainWindow->statusBar(), SIGNAL(messageChanged(QString)),
            this, SLOT(statusBarMessageChanged(QString)));
    }
  else
    {
    disconnect(mainWindow->statusBar(), SIGNAL(messageChanged(QString)),
               this, SLOT(statusBarMessageChanged(QString)));
    }
}

//------------------------------------------------------------------------------
void ctkErrorLogStatusMessageHandler::statusBarMessageChanged(const QString& text)
{
  if (text.isEmpty())
    {
    return;
    }
  this->errorLogModel()->addEntry(
        ctkErrorLogModel::Status, this->handlerPrettyName(), text.toLatin1());
}
