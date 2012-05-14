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
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QLineEdit>
#include <QAbstractItemView>

// CTK includes
#include "ctkPathLineEdit.h"
#include "ctkPathLineEditEventPlayer.h"


// ----------------------------------------------------------------------------
ctkPathLineEditEventPlayer::ctkPathLineEditEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{
}

// ----------------------------------------------------------------------------
bool ctkPathLineEditEventPlayer::playEvent(QObject *Object,
                                           const QString &Command,
                                           const QString &Arguments,
                                           bool &Error)
{
  if (Command != "set_path_string" &&
      Command != "set_edit_string")
    {
    return false;
    }

  if (ctkPathLineEdit* const object = qobject_cast<ctkPathLineEdit*>(Object))
    {
    if (Command == "set_path_string")
      {
      object->setCurrentPath(Arguments);
      return true;
      }
    if (Command == "set_edit_string")
      {
      object->comboBox()->setEditText(Arguments);
      object->comboBox()->lineEdit()->completer()->popup()->show();
      object->update();
      return true;
      }
    }

  qCritical() << "calling set_edit/path_string on unhandled type " << Object;
  Error = true;
  return true;
}

