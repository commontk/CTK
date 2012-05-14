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
#include <QMessageBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRegExp>
#include <QTimer>
#include <QWidget>

// CTK includes
#include "ctkVTKRenderView.h"
#include "ctkVTKRenderViewEventPlayer.h"

ctkVTKRenderViewEventPlayer::ctkVTKRenderViewEventPlayer(const QByteArray& classname, QObject * p)
      : pqWidgetEventPlayer(p), mClassType(classname)
{
}

bool ctkVTKRenderViewEventPlayer::playEvent(QObject* Object,
                                            const QString& Command,
                                            const QString& Arguments,
                                            bool& Error)
{
  Q_UNUSED(Error);
  QWidget* widget = qobject_cast<QWidget*>(Object);
  if(widget && Object->inherits(mClassType.data()))
    {
    if (Command == "3DViewSize")
      {
      QRegExp mouseRegExp("\\(([^,]*),([^,]*),([^,]),([^,]),([^,]*)\\)");
      if (mouseRegExp.indexIn(Arguments)!= -1)
        {
        QVariant v = mouseRegExp.cap(1);
        int w = v.toInt();
        v = mouseRegExp.cap(2);
        int h = v.toInt();
        v = mouseRegExp.cap(3);
//        int app_w = v.toInt();
        v = mouseRegExp.cap(4);
//        int app_h = v.toInt();
        v = mouseRegExp.cap(5);
        bool rescale = v.toBool();

        if ( (w != widget->width() || h != widget->height()) && rescale == true)
          {
          QMessageBox::StandardButton answer = QMessageBox::warning(0, tr("Size issue ..."),
                                                 tr("The Render view size is: width : %1 \t height : %2 \n"
                                                 "But the size was         width : %3 \t height : %4 \n"
                                                 "during the record \n"
                                                 "Do you want to replace it ?")
                                                 .arg(widget->size().width())
                                                 .arg(widget->size().height())
                                                 .arg(w)
                                                 .arg(h),
                                                 QMessageBox::Yes | QMessageBox::No ,
                                                 QMessageBox::Yes);
          if (answer == QMessageBox::Yes )
            {
            qDebug() << "Foo ****";
//            widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
            widget->setMinimumSize(QSize(w,h));
            widget->setMaximumSize(QSize(w,h));
            widget->updateGeometry();
            widget->topLevelWidget()->topLevelWidget()->updateGeometry();
            }
          }
        }
      return true;
      }
    if (Command == "mousePress" || Command == "mouseRelease" ||
        Command == "mouseMove" || Command == "mouseWheel")
      {
      QRegExp mouseRegExp("\\(([^,]*),([^,]*),([^,]),([^,]),([^,]*)\\)");
      if (mouseRegExp.indexIn(Arguments)!= -1)
        {
        double x_center = widget->size().width() / 2.0;
        double y_center = widget->size().height() / 2.0;

        QVariant v = mouseRegExp.cap(1);
        double x = x_center - (v.toDouble() * x_center);
        x = static_cast<int>(x + 0.5);

        v = mouseRegExp.cap(2);
        double y = y_center - (v.toDouble() * y_center);
        y = static_cast<int>(y + 0.5);

        v = mouseRegExp.cap(4);
        Qt::MouseButtons buttons = static_cast<Qt::MouseButton>(v.toInt());

        v = mouseRegExp.cap(5);
        Qt::KeyboardModifiers keym = static_cast<Qt::KeyboardModifier>(v.toInt());

        v = mouseRegExp.cap(3);
        if (Command == "mouseWheel")
          {
//           QEvent::Type type = QEvent::Wheel;
           int delta = ( v.toInt() == 0 ) ? -1 : 1;
           QWheelEvent we(QPoint(x,y), delta, buttons, keym);
           QCoreApplication::sendEvent(Object, &we);
           return true;
          }
        Qt::MouseButton button = static_cast<Qt::MouseButton>(v.toInt());
        QEvent::Type type = (Command == "mousePress")? QEvent::MouseButtonPress :
          ((Command=="mouseMove")?  QEvent::MouseMove : QEvent::MouseButtonRelease);
        QMouseEvent e(type, QPoint(x,y), button, buttons, keym);
        QCoreApplication::sendEvent(Object, &e);
        }
      return true;
      }
    }
  return false;
}
