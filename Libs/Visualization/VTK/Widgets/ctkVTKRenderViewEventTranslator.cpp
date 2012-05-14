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
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QWheelEvent>

// CTK includes
#include "ctkVTKRenderView.h"
#include "ctkVTKRenderViewEventTranslator.h"
#include "ctkCheckableModelHelper.h"

// ----------------------------------------------------------------------------
ctkVTKRenderViewEventTranslator::ctkVTKRenderViewEventTranslator(const QByteArray& Classname, QObject* Parent)
  : pqWidgetEventTranslator(Parent),
    mClassType(Classname),
    lastMoveEvent(QEvent::MouseButtonPress, QPoint(), Qt::MouseButton(),
                                             Qt::MouseButtons(), Qt::KeyboardModifiers()),
    oldMoveEvent(QEvent::MouseMove, QPoint(), Qt::MouseButton(),
                                             Qt::MouseButtons(), Qt::KeyboardModifiers()),
    lastMouseEvent(QEvent::MouseButtonRelease, QPoint(), Qt::MouseButton(),
                                             Qt::MouseButtons(), Qt::KeyboardModifiers())
{
}

// ----------------------------------------------------------------------------
ctkVTKRenderViewEventTranslator::~ctkVTKRenderViewEventTranslator()
{
}

// ----------------------------------------------------------------------------
bool ctkVTKRenderViewEventTranslator::translateEvent(QObject *Object,
                                                     QEvent *Event,
                                                     bool &Error)
{

  Q_UNUSED(Error);
  QWidget* widget = qobject_cast<QWidget*>(Object);

    if(!widget || !Object->inherits(mClassType.data()))
    {
    return false;
    }

  bool handled = false;
  switch(Event->type())
    {
    case QEvent::ContextMenu:
      handled=true;
      break;

    case QEvent::Wheel:
      {
      QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(Event);
      if(wheelEvent)
        {
        QSize size = widget->size();
        double normalized_x = wheelEvent->x()/static_cast<double>(size.width()/2.0);
        double normalized_y = wheelEvent->y()/static_cast<double>(size.height()/2.0);
        int numStep = (wheelEvent->delta() > 0 ) ? 1 : 0;
        int buttons = wheelEvent->buttons();
        int modifiers = wheelEvent->modifiers();
        emit emit recordEvent(Object, "mouseWheel", QString("(%1,%2,%3,%4,%5)")
                              .arg(normalized_x)
                              .arg(normalized_y)
                              .arg(numStep)
                              .arg(buttons)
                              .arg(modifiers));
        }
      }
      handled = true;
      break;

    case QEvent::MouseButtonPress:
        {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(Event);
        if (mouseEvent)
          {
          QSize size = widget->size();
          int x = mouseEvent->x();
          int y = mouseEvent->y();
          double x_center = static_cast<double>(size.width())/2.0;
          double y_center = static_cast<double>(size.height())/2.0;
          double x_norm = (x_center - x)/static_cast<double>(size.width()/2.0);
          double y_norm = (y_center - y)/static_cast<double>(size.height()/2.0);

          int button = mouseEvent->button();
          int buttons = mouseEvent->buttons();
          int modifiers = mouseEvent->modifiers();
          emit recordEvent(Object, "mouseMove", QString("(%1,%2,%3,%4,%5)")
            .arg(x_norm)
            .arg(y_norm)
            .arg(button)
            .arg(buttons)
            .arg(modifiers));
          emit recordEvent(Object, "mousePress", QString("(%1,%2,%3,%4,%5)")
            .arg(x_norm)
            .arg(y_norm)
            .arg(button)
            .arg(buttons)
            .arg(modifiers));
          }

        // reset lastMoveEvent
        QMouseEvent e(QEvent::MouseButtonPress, QPoint(), Qt::MouseButton(),
                      Qt::MouseButtons(), Qt::KeyboardModifiers());

        lastMoveEvent = e;
        lastMouseEvent = e;
        }
      handled = true;
      break;

      case QEvent::MouseMove:
      {
          QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(Event);
          if (mouseEvent)
            {
            QMouseEvent e(QEvent::MouseMove, QPoint(mouseEvent->x(), mouseEvent->y()),
                          mouseEvent->button(), mouseEvent->buttons(),
                          mouseEvent->modifiers());

            lastMoveEvent = e;

            QSize size = widget->size();

            if(lastMouseEvent.type() == QEvent::MouseButtonPress )
              {
              int x = mouseEvent->x();
              int y = mouseEvent->y();
              double x_center = static_cast<double>(size.width())/2.0;
              double y_center = static_cast<double>(size.height())/2.0;
              double x_norm = (x_center - x)/static_cast<double>(size.width()/2.0);
              double y_norm = (y_center - y)/static_cast<double>(size.height()/2.0);

              int button = mouseEvent->button();
              int buttons = mouseEvent->buttons();
              int modifiers = mouseEvent->modifiers();

              emit recordEvent(Object, "mouseMove", QString("(%1,%2,%3,%4,%5)")
                .arg(x_norm)
                .arg(y_norm)
                .arg(button)
                .arg(buttons)
                .arg(modifiers));
              }
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

          // record last move event if it is valid
          if(lastMoveEvent.type() == QEvent::MouseMove)
            {
            int x = mouseEvent->x();
            int y = mouseEvent->y();
            double x_center = static_cast<double>(size.width())/2.0;
            double y_center = static_cast<double>(size.height())/2.0;

            double x_norm = (x_center - x)/static_cast<double>(size.width()/2.0);
            double y_norm = (y_center - y)/static_cast<double>(size.height()/2.0);

            int button = lastMoveEvent.button();
            int buttons = lastMoveEvent.buttons();
            int modifiers = lastMoveEvent.modifiers();

            emit recordEvent(Object, "mouseMove", QString("(%1,%2,%3,%4,%5)")
              .arg(x_norm)
              .arg(y_norm)
              .arg(button)
              .arg(buttons)
              .arg(modifiers));
            }

          int x = mouseEvent->x();
          int y = mouseEvent->y();
          double x_center = static_cast<double>(size.width())/2.0;
          double y_center = static_cast<double>(size.height())/2.0;

          double x_norm = (x_center - x)/static_cast<double>(size.width ()/2.0);
          double y_norm = (y_center - y)/static_cast<double>(size.height()/2.0);

          int button = mouseEvent->button();
          int buttons = mouseEvent->buttons();
          int modifiers = mouseEvent->modifiers();

          emit recordEvent(Object, "mouseRelease", QString("(%1,%2,%3,%4,%5)")
            .arg(x_norm)
            .arg(y_norm)
            .arg(button)
            .arg(buttons)
            .arg(modifiers));
          }
        // reset lastMoveEvent
        QMouseEvent e(QEvent::MouseButtonRelease, QPoint(), Qt::MouseButton(),
                      Qt::MouseButtons(), Qt::KeyboardModifiers());

        lastMouseEvent = e;
        }

      handled = true;
      break;

    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      {
      QKeyEvent* ke = static_cast<QKeyEvent*>(Event);
      QString data =QString("%1:%2:%3:%4:%5:%6")
        .arg(static_cast<int>(ke->type()))
        .arg(ke->key())
        .arg(static_cast<int>(ke->modifiers()))
        .arg(ke->text())
        .arg(static_cast<int>(ke->isAutoRepeat()))
        .arg(ke->count());
      emit recordEvent(Object, "keyEvent", data);
      }
      break;

    default:
      break;
    }

  return handled;
}

