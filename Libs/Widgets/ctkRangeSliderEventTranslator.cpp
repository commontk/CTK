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
#include <QEvent>

// CTK includes
#include "ctkDoubleRangeSlider.h"
#include "ctkRangeSlider.h"
#include "ctkRangeSliderEventTranslator.h"

// ----------------------------------------------------------------------------
ctkRangeSliderEventTranslator::ctkRangeSliderEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
}

// ----------------------------------------------------------------------------
bool ctkRangeSliderEventTranslator::translateEvent(QObject *Object,
                                                   QEvent *Event,
                                                   bool &Error)
{
  Q_UNUSED(Error);
  ctkRangeSlider* const object = qobject_cast<ctkRangeSlider*>(Object);
  if(!object)
    {
    return false;
    }
  // Since ctkDoubleRangeSlider internally aggregates a ctkRangeSlider
  if(qobject_cast<ctkDoubleRangeSlider*>(object->parent()))
    {
    return false;
    }

  switch(Event->type())
    {
    case QEvent::Enter:
      this->CurrentObject = object;
      connect(object, SIGNAL(maximumPositionChanged(int)),
              this,SLOT(onMaximumPositionChanged(int)));
      connect(object, SIGNAL(minimumPositionChanged(int)),
              this,SLOT(onMinimumPositionChanged(int)));
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

// ----------------------------------------------------------------------------
void ctkRangeSliderEventTranslator::onMaximumPositionChanged(const int& maxValue)
{
  emit recordEvent(this->CurrentObject, "set_max_int" , QString().setNum(maxValue));
}

// ----------------------------------------------------------------------------
void ctkRangeSliderEventTranslator::onMinimumPositionChanged(const int& minValue)
{
  emit recordEvent(this->CurrentObject, "set_min_int" , QString().setNum(minValue));
}
