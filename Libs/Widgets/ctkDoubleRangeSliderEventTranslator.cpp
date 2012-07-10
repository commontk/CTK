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
#include <QEvent>

// CTK includes
#include "ctkDoubleRangeSlider.h"
#include "ctkDoubleRangeSliderEventTranslator.h"

// ----------------------------------------------------------------------------
ctkDoubleRangeSliderEventTranslator::ctkDoubleRangeSliderEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
bool ctkDoubleRangeSliderEventTranslator::translateEvent(QObject *Object,
                                                         QEvent *Event,
                                                         bool &Error)
{
  Q_UNUSED(Error);
  ctkDoubleRangeSlider* const object = qobject_cast<ctkDoubleRangeSlider*>(Object);

    if(!object)
      {
      return false;
      }

  switch(Event->type())
    {
    case QEvent::Enter:
      this->CurrentObject = object;
      connect(object, SIGNAL(maximumPositionChanged(double)),
              this,SLOT(onMaximumPositionChanged(double)));
      connect(object, SIGNAL(minimumPositionChanged(double)),
              this,SLOT(onMinimumPositionChanged(double)));
      break;
    case QEvent::Leave:
      disconnect(object, 0, this, 0);
      this->CurrentObject = 0;
      break;
    default:
      break;
    }

  return true;
}

void ctkDoubleRangeSliderEventTranslator::onMaximumPositionChanged(const double& maxValue)
{
  emit recordEvent(this->CurrentObject, "set_max_double" , QString().setNum(maxValue));
}

void ctkDoubleRangeSliderEventTranslator::onMinimumPositionChanged(const double& minValue)
{
  emit recordEvent(this->CurrentObject, "set_min_double" , QString().setNum(minValue));
}
