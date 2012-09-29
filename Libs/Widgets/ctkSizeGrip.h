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

#ifndef __ctkSizeGrip_h
#define __ctkSizeGrip_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkSizeGripPrivate;

class CTK_WIDGETS_EXPORT ctkSizeGrip
  : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(Qt::Orientations orientations READ orientations WRITE setOrientations)
  Q_PROPERTY(QSize widgetSizeHint READ widgetSizeHint WRITE setWidgetSizeHint)
  Q_PROPERTY(bool resizeWidget READ resizeWidget WRITE setResizeWidget)

public:
  typedef QWidget Superclass;
  explicit ctkSizeGrip(QWidget *parent);
  explicit ctkSizeGrip(QWidget* widgetToResize, QWidget *parent);
  virtual ~ctkSizeGrip();

  void setOrientations(Qt::Orientations orientations);
  Qt::Orientations orientations()const;

  QWidget* widgetToResize()const;
  void setWidgetToResize(QWidget* target);

  QSize widgetSizeHint()const;

  void setResizeWidget(bool resize);
  bool resizeWidget()const;

  virtual QSize sizeHint() const;

public Q_SLOTS:
  void setWidgetSizeHint(QSize sizeHint);

Q_SIGNALS:
  void widgetSizeHintChanged(QSize sizeHint);

protected:
  QScopedPointer<ctkSizeGripPrivate> d_ptr;

  void paintEvent(QPaintEvent* );
  bool event(QEvent* );
  void mousePressEvent(QMouseEvent* );
  void mouseMoveEvent(QMouseEvent* );
  void mouseReleaseEvent(QMouseEvent* mouseEvent);

private:
  Q_DECLARE_PRIVATE(ctkSizeGrip)
  Q_DISABLE_COPY(ctkSizeGrip)
};

#endif // __ctkSizeGrip_h
