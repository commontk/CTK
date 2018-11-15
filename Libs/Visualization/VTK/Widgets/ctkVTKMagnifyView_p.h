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

#ifndef __ctkVTKMagnifyView_p_h
#define __ctkVTKMagnifyView_p_h

// Qt includes
#include <QPointer>
class QPointF;
class QTimerEvent;

// CTK includes
#include "ctkVTKMagnifyView.h"
#include <ctkVTKObject.h>
#include "ctkVTKOpenGLNativeWidget.h"

/// \ingroup Visualization_VTK_Widgets
class ctkVTKMagnifyViewPrivate : public QObject
{
  Q_OBJECT
  QVTK_OBJECT
  Q_DECLARE_PUBLIC(ctkVTKMagnifyView);
protected:
  ctkVTKMagnifyView* const q_ptr;
public:
  ctkVTKMagnifyViewPrivate(ctkVTKMagnifyView& object);
  virtual ~ctkVTKMagnifyViewPrivate();

  void init();

  void observe(ctkVTKOpenGLNativeWidget * widget);
  void remove(ctkVTKOpenGLNativeWidget * widget);
  void connectRenderWindow(ctkVTKOpenGLNativeWidget * widget);
  void disconnectRenderWindow(ctkVTKOpenGLNativeWidget * widget);

protected:
  void updatePixmap();
  void removePixmap();
  void timerEvent(QTimerEvent * event);
  void restartTimer();
  void resetEventHandler();

  enum PendingEventType {
    NoEvent = 0,
    UpdatePixmapEvent,
    RemovePixmapEvent
    };

  struct EventHandlerStruct
    {
    PendingEventType EventType;
    QPointer<ctkVTKOpenGLNativeWidget> Widget;
    QPointF Position;
    int UpdateInterval;
    int TimerId;
    };

public Q_SLOTS:
  void pushUpdatePixmapEvent();
  void pushUpdatePixmapEvent(QPointF pos);
  void pushRemovePixmapEvent();

public:
  QList<ctkVTKOpenGLNativeWidget *> ObservedQVTKWidgets;
  double Magnification;
  bool ObserveRenderWindowEvents;
  EventHandlerStruct EventHandler;
};

#endif
