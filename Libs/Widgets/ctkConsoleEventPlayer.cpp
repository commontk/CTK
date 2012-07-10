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
#include <QCoreApplication>
#include <QEvent>
#include <QDebug>
#include <QKeyEvent>

// CTK inlcudes
#include "ctkConsole.h"
#include "ctkConsoleEventPlayer.h"

// ----------------------------------------------------------------------------
ctkConsoleEventPlayer::ctkConsoleEventPlayer(QObject* parent)
  : pqWidgetEventPlayer(parent)
{
}

// ----------------------------------------------------------------------------
bool ctkConsoleEventPlayer::playEvent(QObject *Object,
                                      const QString &Command,
                                      const QString &Arguments,
                                      bool &Error)
{
  if (Command != "command")
    {
    return false;
    }

  if (ctkConsole* console = qobject_cast<ctkConsole*>(Object))
    {
    if (Command == "command")
      {
//      console->printMessage(Arguments, console->commandTextColor());
      console->exec(Arguments);
//      QKeyEvent kd(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier,
//                   "Return", false, 0);
//      QKeyEvent ku(QEvent::KeyRelease, Qt::Key_Enter, Qt::NoModifier,
//                   "Return", false, 0);
//      QApplication::sendEvent(Object, &kd);
//      QApplication::sendEvent(Object, &ku);
      return true;
      }
    }

  qCritical() << "calling command on unhandled type " << Object;
  Error = true;
  return true;
}
