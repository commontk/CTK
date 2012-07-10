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

//QT includes
#include <QDebug>

// CTK includes
#include "ctkFontButton.h"
#include "ctkFontButtonEventPlayer.h"

// ----------------------------------------------------------------------------
ctkFontButtonEventPlayer::ctkFontButtonEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{

}

// ----------------------------------------------------------------------------
bool ctkFontButtonEventPlayer::playEvent(QObject *Object,
                                                const QString &Command,
                                                const QString &Arguments,
                                                bool &Error)
{
  if (Command != "newFont")
    {
    return false;
    }

  if(ctkFontButton* const object =
       qobject_cast<ctkFontButton*>(Object))
    {
    if (Command == "newFont")
      {
      QFont font;
      font.fromString(Arguments);
      object->setCurrentFont(font);
      return true;
      }
    }

  qCritical() << "calling newFont on unhandled type " << Object;
  Error = true;
  return true;
}

