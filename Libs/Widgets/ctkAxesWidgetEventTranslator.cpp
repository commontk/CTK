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
#include <QMouseEvent>

// CTK includes
#include "ctkAxesWidget.h"
#include "ctkAxesWidgetEventTranslator.h"
#include "ctkCheckableModelHelper.h"

// ----------------------------------------------------------------------------
ctkAxesWidgetEventTranslator::ctkAxesWidgetEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
}

// ----------------------------------------------------------------------------
bool ctkAxesWidgetEventTranslator::translateEvent(QObject *Object,
                                             QEvent *Event,
                                             bool &Error)
{
  Q_UNUSED(Error);
  ctkAxesWidget* widget = qobject_cast<ctkAxesWidget*>(Object);
  if(!widget)
    {
    return false;
    }

  bool handled = false;
  switch(Event->type())
    {
    case QEvent::MouseButtonPress:
        {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(Event);
        if (mouseEvent)
          {
          QSize size = widget->size();
          double normalized_x = mouseEvent->x()/static_cast<double>(size.width());
          double normalized_y = mouseEvent->y()/static_cast<double>(size.height());
          int button = mouseEvent->button();
          int buttons = mouseEvent->buttons();
          int modifiers = mouseEvent->modifiers();
          emit recordEvent(Object, "mousePress", QString("(%1,%2,%3,%4,%5)")
            .arg(normalized_x)
            .arg(normalized_y)
            .arg(button)
            .arg(buttons)
            .arg(modifiers));
          }
        }
      handled = true;
      break;
    case QEvent::MouseButtonRelease:
        {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(Event);
        if (mouseEvent)
          {
            QSize size = widget->size();

          double normalized_x = mouseEvent->x()/static_cast<double>(size.width());
          double normalized_y = mouseEvent->y()/static_cast<double>(size.height());
          int button = mouseEvent->button();
          int buttons = mouseEvent->buttons();
          int modifiers = mouseEvent->modifiers();

          emit recordEvent(Object, "mouseRelease", QString("(%1,%2,%3,%4,%5)")
            .arg(normalized_x)
            .arg(normalized_y)
            .arg(button)
            .arg(buttons)
            .arg(modifiers));
          }
        }
      handled = true;
      break;
    default:
      break;
    }

  return handled;
}

