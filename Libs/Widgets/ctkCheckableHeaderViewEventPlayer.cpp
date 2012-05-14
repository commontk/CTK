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
#include <QDebug>

// CTK includes
#include "ctkCheckableHeaderView.h"
#include "ctkCheckableHeaderViewEventPlayer.h"

// ----------------------------------------------------------------------------
ctkCheckableHeaderViewEventPlayer::ctkCheckableHeaderViewEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{

}

// ----------------------------------------------------------------------------
bool ctkCheckableHeaderViewEventPlayer::playEvent(QObject *Object,
                                                  const QString &Command,
                                                  const QString &Arguments,
                                                  bool &Error)
{
  if(Command != "set_section_checked" &&
     Command != "set_section_unchecked")
    {
    return false;
    }

  const int value = Arguments.toInt();

  if(ctkCheckableHeaderView* const object =
     qobject_cast<ctkCheckableHeaderView*>(Object))
    {
    if(Command == "set_section_checked")
      {
      object->setCheckState(value, Qt::Checked);
      }
    if(Command == "set_section_unchecked")
      {
      object->setCheckState(value, Qt::Unchecked);
      }
      return true;
    }

  qCritical() << "calling set_checkable/set_unchecked_all on unhandled type " << Object;
  Error = true;
  return true;
}

