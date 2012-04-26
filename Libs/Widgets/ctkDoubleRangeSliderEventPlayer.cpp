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
#include "ctkDoubleRangeSlider.h"
#include "ctkDoubleRangeSliderEventPlayer.h"

// ----------------------------------------------------------------------------
ctkDoubleRangeSliderEventPlayer::ctkDoubleRangeSliderEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{

}

// ----------------------------------------------------------------------------
bool ctkDoubleRangeSliderEventPlayer::playEvent(QObject *Object,
                                                const QString &Command,
                                                const QString &Arguments,
                                                bool &Error)
{
  if (Command != "set_max_double" && Command != "set_min_double")
    {
    return false;
    }

  const double value = Arguments.toDouble();

  if(ctkDoubleRangeSlider* const object =
       qobject_cast<ctkDoubleRangeSlider*>(Object))
    {
    if (Command == "set_min_double")
      {
      object->setMinimumValue(value);
      return true;
      }
    if (Command == "set_max_double")
      {
      object->setMaximumValue(value);
      return true;
      }
    }

  qCritical() << "calling set_min/max_double on unhandled type " << Object;
  Error = true;
  return true;
}

