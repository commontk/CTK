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

// QT includes
#include <QDebug>

// CTK includes
#include "ctkCheckableComboBox.h"
#include "ctkCheckableComboBoxEventPlayer.h"

// ----------------------------------------------------------------------------
ctkCheckableComboBoxEventPlayer::ctkCheckableComboBoxEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{

}

// ----------------------------------------------------------------------------
bool ctkCheckableComboBoxEventPlayer::playEvent(QObject *Object,
                                                const QString &Command,
                                                const QString &Arguments,
                                                bool &Error)
{
  if(Command != "check_indexes" &&
     Command != "uncheck_indexes")
    {
    return false;
    }

//  const int value = Arguments.toInt();
  if(ctkCheckableComboBox* const object =
     qobject_cast<ctkCheckableComboBox*>(Object))
    {
    if(Command == "check_indexes")
      {
      QStringList Args = Arguments.split(" ");
      foreach (QString Arg, Args)
        {
        const int value = Arg.toInt();
        QModelIndex index = object->checkableModel()->index(value, 0);
        object->setCheckState(index, Qt::Checked);
        object->update();
        }
      return true;
      }
    if(Command == "uncheck_indexes")
      {
      QStringList Args = Arguments.split(" ");
      foreach (QString Arg, Args)
        {
        const int value = Arg.toInt();
        QModelIndex index = object->checkableModel()->index(value, 0);
        object->setCheckState(index, Qt::Unchecked);
        object->update();
        }
      return true;
      }
    }

  qCritical() << "calling set_checkable/set_unchecked_all on unhandled type " << Object;
  Error = true;
  return true;
}

