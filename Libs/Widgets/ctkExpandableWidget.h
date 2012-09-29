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

#ifndef __ctkExpandableWidget_h
#define __ctkExpandableWidget_h

// Qt includes
#include <QFrame>
class QResizeEvent;

// CTK includes
#include "ctkWidgetsExport.h"
class ctkExpandableWidgetPrivate;

/// \ingroup Widgets
/// \brief Widget that can be resized by the user using a corner size grip.
/// ctkExpandableWidget is a container widget that has a user customizable
/// sizeHint.
/// \note If the widget fails to be resized, consider tweaking the size
/// policy of the parents and children widgets.
/// \sa ctkSizeGrip
class CTK_WIDGETS_EXPORT ctkExpandableWidget: public QFrame
{
  Q_OBJECT
  /// This property controls the movement of freedom allowed to resize the widget.
  /// The location of the size grip widget depends on the orientations:
  ///  - bottom of the widget if Qt::Vertical
  ///  - right of the widget if Qt::Horizontal
  ///  - bottom right corner of the widget if Qt::Horizontal|Qt::Vertical.
  /// Bottom right corner of the widget with a Qt::Horizontal|Qt::Vertical resize
  /// movements of freedom by default.
  /// \sa ctkSizeGrip::orientations, sizeGripInside, sizeGripMargins
  Q_PROPERTY(Qt::Orientations orientations READ orientations WRITE setOrientations)

  /// This property controls whether the size grip widget overlays the children
  /// widgets or it is moved into its own margin.
  /// Please note that QWidget::setContentsMargins is controlled by
  /// ctkExpandableWidget, any value set will be overwritten. You can still set
  /// the layout contents margins though.
  /// true by default.
  /// \sa sizeGripMargins, orientations
  Q_PROPERTY(bool sizeGripInside READ isSizeGripInside WRITE setSizeGripInside)

  /// This property controls the extra padding to give to the size grip widget.
  /// Depending on the contents of ctkExpandableWidget, the location of the size
  /// grip widget could look off and would benefit from being moved from a few
  /// pixels.
  /// When tweaking this property, you may want to make sure it works for all
  /// styles and platforms.
  /// \sa sizeGripInside, orientations
  Q_PROPERTY(QSize sizeGripMargins READ sizeGripMargins WRITE setSizeGripMargins)

public:
  typedef QFrame Superclass;

  ctkExpandableWidget(QWidget *parent=0);
  virtual ~ctkExpandableWidget();

  void setOrientations(Qt::Orientations orientations);
  Qt::Orientations orientations()const;

  void setSizeGripInside(bool);
  bool isSizeGripInside()const;

  void setSizeGripMargins(QSize margins);
  QSize sizeGripMargins()const;

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

public Q_SLOTS:
  /// Recompute the size hint of the widget and resize with regard to the
  /// layout.
  void updateSizeHint();

protected:
  QScopedPointer<ctkExpandableWidgetPrivate> d_ptr;

  virtual void resizeEvent(QResizeEvent* event);
  virtual bool event(QEvent* event);

private:
  Q_DECLARE_PRIVATE(ctkExpandableWidget);
  Q_DISABLE_COPY(ctkExpandableWidget);
};

#endif // __ctkExpandableWidget_h
