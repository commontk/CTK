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

#ifndef __ctkBasePopupWidget_p_h
#define __ctkBasePopupWidget_p_h

// Qt includes
#include <QPointer>
class QLabel;
class QPropertyAnimation;

// CTK includes
#include "ctkBasePopupWidget.h"
#define PopupWindowType Qt::Tool

// -------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkBasePopupWidgetPrivate
  : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkBasePopupWidget);
protected:
  ctkBasePopupWidget* const q_ptr;
public:
  ctkBasePopupWidgetPrivate(ctkBasePopupWidget& object);
  ~ctkBasePopupWidgetPrivate();
  virtual void init();

  bool isOpening()const;
  bool isClosing()const;
  /// Return true if the animation was closing (direction == backward).
  /// It doesn't indicate if the action is still running or finished.
  /// Can only be called in a slot as it uses sender().
  bool wasClosing()const;

  bool fitBaseWidgetSize()const;
  Qt::Alignment pixmapAlignment()const;
  void setupPopupPixmapWidget();

  QWidgetList focusWidgets(bool onlyVisible = false)const;

  // Return the widget if the mouse cursor is above any of the focus widgets or their
  // children.
  virtual QWidget* mouseOver();

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
  QRect desiredOpenGeometry(QRect baseGeometry)const;
  QRect baseGeometry()const;
  QPoint mapToGlobal(const QPoint& baseWidgetPoint)const;
  
  QPropertyAnimation* currentAnimation()const;

  //void temporarilyHiddenOn();
  //void temporarilyHiddenOff();

  void hideAll();

protected:
  QPointer<QWidget> BaseWidget;

  double EffectAlpha;

  ctkBasePopupWidget::AnimationEffect Effect;
  int                 EffectDuration;
  QPropertyAnimation* AlphaAnimation;
  bool                ForcedTranslucent;
  QPropertyAnimation* ScrollAnimation;
  QLabel*             PopupPixmapWidget;
  
  // Geometry attributes
  Qt::Alignment    Alignment;
  Qt::Orientations Orientations;

  ctkBasePopupWidget::VerticalDirection VerticalDirection;
  Qt::LayoutDirection HorizontalDirection;
};

#endif
