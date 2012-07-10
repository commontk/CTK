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
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRegExp>
#include <QWidget>

// CTK includes
#include "ctkAxesWidget.h"
#include "ctkAxesWidgetEventPlayer.h"

// ----------------------------------------------------------------------------
ctkAxesWidgetEventPlayer::ctkAxesWidgetEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{

}

// ----------------------------------------------------------------------------
bool ctkAxesWidgetEventPlayer::playEvent(QObject *Object,
                                    const QString &Command,
                                    const QString &Arguments,
                                    bool &Error)
{
  Q_UNUSED(Error);
  ctkAxesWidget* widget = qobject_cast<ctkAxesWidget*>(Object);
  if(widget)
    {
    if (Command == "mousePress" || Command == "mouseRelease")
      {
      QRegExp mouseRegExp("\\(([^,]*),([^,]*),([^,]),([^,]),([^,]*)\\)");
      if (mouseRegExp.indexIn(Arguments)!= -1)
        {
        QVariant v = mouseRegExp.cap(1);
        int x = static_cast<int>(v.toDouble() * widget->size().width());
        v = mouseRegExp.cap(2);
        int y = static_cast<int>(v.toDouble() * widget->size().height());
        v = mouseRegExp.cap(4);
        Qt::MouseButtons buttons = static_cast<Qt::MouseButton>(v.toInt());
        v = mouseRegExp.cap(5);
        Qt::KeyboardModifiers keym = static_cast<Qt::KeyboardModifier>(v.toInt());
        v = mouseRegExp.cap(3);

        Qt::MouseButton button = static_cast<Qt::MouseButton>(v.toInt());
        QEvent::Type type = (Command == "mousePress")?
                            QEvent::MouseButtonPress : QEvent::MouseButtonRelease;
        QMouseEvent e(type, QPoint(x,y), button, buttons, keym);
        QCoreApplication::sendEvent(Object, &e);
        }
      return true;
      }
    }
  return false;
}

