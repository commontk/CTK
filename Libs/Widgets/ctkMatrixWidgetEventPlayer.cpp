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
#include "ctkMatrixWidget.h"
#include "ctkMatrixWidgetEventPlayer.h"

// ----------------------------------------------------------------------------
ctkMatrixWidgetEventPlayer::ctkMatrixWidgetEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{
}

// ----------------------------------------------------------------------------
bool ctkMatrixWidgetEventPlayer::playEvent(QObject *Object,
                                           const QString &Command,
                                           const QString &Arguments,
                                           bool &Error)
{
  if (Command != "set_values")
    {
    return false;
    }

  if(ctkMatrixWidget* const object =
       qobject_cast<ctkMatrixWidget*>(Object))
    {
    if (Command == "set_values")
      {
      QVector<double> values;
      foreach(QString value, Arguments.split(" "))
        {
        values.push_back(value.toDouble());
        }
      object->setValues(values);
      return true;
      }
    }

  qCritical() << "calling set_values on unhandled type " << Object;
  Error = true;
  return true;
}

