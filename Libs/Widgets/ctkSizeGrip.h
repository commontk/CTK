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

/// \ingroup Widgets
/// \brief ctkSizeGrip is a utility widget that allows widget in a layout to be
/// manually resized.
/// It is not meant to be used as is but with a third party that can change the
/// size hint of a widget such as a container widget (e.g. ctkExpandableWidget).
/// To resize a widget, the user must left click on the size grip and drag
/// left/right to control the width and/or up/bottom to control the height.
/// Left button double-click resets the size hint of the target widget to its
/// default.
/// \sa ctkExpandableWidget
class CTK_WIDGETS_EXPORT ctkSizeGrip
  : public QWidget
{
  Q_OBJECT

  /// This property describes the movement of freedom of the sizeGrip.
  /// There are 3 supported orientations: Qt::Vertical, Qt::Horizontal and
  /// Qt::Horizontal|Qt::Vertical.
  /// If the orientation is solely Qt::Horizontal, the size grip only allows
  /// vertical resize. If the orientation is solely Qt::Vertical, only
  /// horizontal resizes are supported.
  /// Note that the orientations change the look of the widget. And it also
  /// probably means that the position of the widget should be changed: bottom
  /// side for Qt::Vertical, right side for Qt::Horizontal and bottom
  /// right corner for Qt::Horizontal|Qt::Vertical.
  /// Qt::Horizontal|Qt::Vertical by default.
  Q_PROPERTY(Qt::Orientations orientations READ orientations WRITE setOrientations)

  /// This property holds the sizeHint set by the user via the size grip or
  /// programatically using \a setWidgetSizeHint().
  /// If width or/and height is 0, it means the sizeHint has not been touched
  /// and the default widget sizeHint should be used instead.
  /// For example, if \a orientations is solely vertical (not also
  /// horizontal), the width component of the widget sizeHint is 0.
  /// If \a orientations is solely horizontal, the widgets izeHint height is 0.
  /// (0,0) by default (meaning the default widget sizehint should be used).
  Q_PROPERTY(QSize widgetSizeHint READ widgetSizeHint WRITE setWidgetSizeHint)

  /// This property holds whether the size grip resizes the widget or not.
  /// If the size grip doesn't resize the widget, it still returns a valid
  /// \a widgetSizeHint that can be used by a third party.
  /// By default, the widget is resized.
  /// \tbd it is experimental and not really working.
  Q_PROPERTY(bool resizeWidget READ resizeWidget WRITE setResizeWidget)

public:
  typedef QWidget Superclass;
  explicit ctkSizeGrip(QWidget *parent);
  explicit ctkSizeGrip(QWidget* widgetToResize, QWidget *parent);
  virtual ~ctkSizeGrip();

  void setOrientations(Qt::Orientations orientations);
  Qt::Orientations orientations()const;

  /// Set the widget to resize.
  /// Note that the widget is not resized by ctkSizeGrip if resizeWidget is false.
  /// It should be a third party that should resize the widget
  /// (e.g. ctkExpandableWidget).
  /// \sa widgetSizeHint()
  QWidget* widgetToResize()const;
  void setWidgetToResize(QWidget* target);

  /// Return the sizeHint set by the user or programatically using
  /// \a setWidgetSizeHint().
  /// If width or/and height is 0, it means the sizeHint has not been touched
  /// and the original widget sizeHint should be used instead.
  /// When the orientation is solely vertical, the returned width component of
  /// the sizeHint is 0. If the orientations is solely horizontal, the sizeHint
  /// height is 0.
  QSize widgetSizeHint()const;

  void setResizeWidget(bool resize);
  bool resizeWidget()const;

  /// SizeHint of the size grip. It depends on the style in use, but it is
  /// typically a small (e.g. 13x13) square sizehint for
  /// Qt::Horizontal|Qt::Vertical \a orientations and a narrow rectangle for
  /// Qt::Horizontal or Qt::Vertical orientations.
  virtual QSize sizeHint() const;

public Q_SLOTS:
  /// Expressively change the sizeHint of the widget to resize.
  /// \sa widgetSizeHint()
  void setWidgetSizeHint(QSize sizeHint);

Q_SIGNALS:
  void widgetSizeHintChanged(QSize sizeHint);

protected:
  QScopedPointer<ctkSizeGripPrivate> d_ptr;

  virtual void paintEvent(QPaintEvent* paintEvent);
  virtual bool event(QEvent* event);
  virtual void mousePressEvent(QMouseEvent* mouseEvent);
  virtual void mouseMoveEvent(QMouseEvent* mouseEvent);
  virtual void mouseReleaseEvent(QMouseEvent* mouseEvent);
  virtual void mouseDoubleClickEvent(QMouseEvent* mouseEvent);

private:
  Q_DECLARE_PRIVATE(ctkSizeGrip)
  Q_DISABLE_COPY(ctkSizeGrip)
};

#endif // __ctkSizeGrip_h
