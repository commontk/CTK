/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkPopupWidget_p_h
#define __ctkPopupWidget_p_h

// Qt includes
class QPropertyAnimation;
class QLabel;

// CTK includes
#include "ctkPopupWidget.h"

// -------------------------------------------------------------------------
class CTK_WIDGETS_EXPORT ctkPopupWidgetPrivate: public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkPopupWidget);
protected:
  ctkPopupWidget* const q_ptr;
public:
  ctkPopupWidgetPrivate(ctkPopupWidget& object);
  ~ctkPopupWidgetPrivate();
  void init();
  
  bool isOpening()const;
  bool isClosing()const;

  bool fitBaseWidgetSize()const;
  Qt::Alignment pixmapAlignment()const;
  void setupPopupPixmapWidget();

  QList<const QWidget*> focusWidgets(bool onlyVisible = false)const;

  // Return true if the mouse cursor is above any of the focus widgets or their
  // children.
  // If the cursor is above a child widget, install the event filter to listen
  // when the cursor leaves the widget.
  bool mouseOver();

  // Same as QWidget::isAncestorOf() but don't restrain to the same window
  // and apply it to all the focusWidgets
  bool isAncestorOf(const QWidget* ancestor, const QWidget* child)const;


  /// Return the closed geometry for the popup based on the current geometry
  QRect closedGeometry()const;
  /// Return the closed geometry for a given open geometry 
  QRect closedGeometry(QRect openGeom)const;
  
  /// Return the desired geometry, maybe it won't happen if the size is too
  /// small for the popup.
  QRect desiredOpenGeometry()const;
  
  QPropertyAnimation* currentAnimation()const;
  
  virtual bool eventFilter(QObject* obj, QEvent* event);

  void temporarilyHiddenOn();
  void temporarilyHiddenOff();

public slots:
  void updateVisibility();
  void onBaseWidgetDestroyed();
  void onApplicationDeactivate();

protected:
  QWidget* BaseWidget;
  bool AutoShow;
  bool AutoHide;

  double EffectAlpha;

  ctkPopupWidget::AnimationEffect Effect;
  QPropertyAnimation* AlphaAnimation;
  bool                ForcedTranslucent;
  QPropertyAnimation* ScrollAnimation;
  QLabel*             PopupPixmapWidget;
  
  // Geometry attributes
  Qt::Alignment    Alignment;
  Qt::Orientations Orientations;
  
  ctkPopupWidget::VerticalDirection VerticalDirection;
  Qt::LayoutDirection HorizontalDirection;
};

#endif
