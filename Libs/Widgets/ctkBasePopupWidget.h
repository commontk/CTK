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

#ifndef __ctkBasePopupWidget_h
#define __ctkBasePopupWidget_h

// Qt includes
#include <QEasingCurve>
#include <QFrame>
#include <QMetaType>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkBasePopupWidgetPrivate;

/// \ingroup Widgets
/// Description:
class CTK_WIDGETS_EXPORT ctkBasePopupWidget : public QFrame
{
  Q_OBJECT

  Q_ENUMS(AnimationEffect)
  Q_ENUMS(VerticalDirection)

  /// ScrollEffect by default
  Q_PROPERTY( AnimationEffect animationEffect READ animationEffect WRITE setAnimationEffect)

  /// Effect duration in ms
  /// Default to 333ms
  Q_PROPERTY( int effectDuration READ effectDuration WRITE setEffectDuration);

  /// Opening/Closing curve
  /// QEasingCurve::InOutQuad by default
  Q_PROPERTY( QEasingCurve::Type easingCurve READ easingCurve WRITE setEasingCurve);

  /// Where is the popup in relation to the BaseWidget
  /// To vertically justify, use Qt::AlignTop | Qt::AlignBottom
  /// Qt::AlignJustify | Qt::AlignBottom by default
  Q_PROPERTY( Qt::Alignment alignment READ alignment WRITE setAlignment);
  
  /// Direction of the scrolling effect, can be Qt::Vertical, Qt::Horizontal or
  /// both Qt::Vertical|Qt::Horizontal.
  /// Vertical by default
  Q_PROPERTY( Qt::Orientations orientation READ orientation WRITE setOrientation);
  
  /// Control where the popup opens vertically.
  /// TopToBottom by default
  Q_PROPERTY( ctkBasePopupWidget::VerticalDirection verticalDirection READ verticalDirection WRITE setVerticalDirection);

  /// Control where the popup opens horizontally.
  /// LeftToRight by default
  Q_PROPERTY( Qt::LayoutDirection horizontalDirection READ horizontalDirection WRITE setHorizontalDirection);

public:
  typedef QFrame Superclass;
  /// Although a popup widget is a top-level widget, if a parent is
  /// passed the popup widget will be deleted when that parent is
  /// destroyed (as with any other QObject).
  /// ctkBasePopupWidget is a top-level widget (Qt::ToolTip), so
  /// even if a parent is passed, the popup will display outside the possible
  /// parent layout.
  /// \sa baseWidget().
  explicit ctkBasePopupWidget(QWidget* parent = 0);
  virtual ~ctkBasePopupWidget();

  /// Widget the popup is attached to. It opens right under \a baseWidget
  /// and if the ctkBasePopupWidget sizepolicy contains the growFlag/shrinkFlag,
  /// it tries to resize itself to fit the same width of \a baseWidget.
  /// By default, baseWidget is the parent widget.
  QWidget* baseWidget()const;

  enum AnimationEffect
  {
    WindowOpacityFadeEffect = 0,
    ScrollEffect,
    FadeEffect
  };

  AnimationEffect animationEffect()const;
  void setAnimationEffect(AnimationEffect effect);

  int effectDuration()const;
  void setEffectDuration(int duration);

  QEasingCurve::Type easingCurve()const;
  void setEasingCurve(QEasingCurve::Type easingCurve);

  Qt::Alignment alignment()const;
  void setAlignment(Qt::Alignment alignment);
  
  Qt::Orientations orientation()const;
  void setOrientation(Qt::Orientations orientation);
  
  enum VerticalDirection{
    TopToBottom = 1,
    BottomToTop = 2
  };
  
  VerticalDirection verticalDirection()const;
  void setVerticalDirection(VerticalDirection direction);
  
  Qt::LayoutDirection horizontalDirection()const;
  void setHorizontalDirection(Qt::LayoutDirection direction);

public Q_SLOTS:
  /// Hide the popup if open or opening. It takes around 300ms 
  /// for the fading effect to hide the popup.
  virtual void hidePopup();
  /// Open the popup if closed or closing. It takes around 300ms 
  /// for the fading effect to open the popup.
  virtual void showPopup();
  /// Show/hide the popup. It can be conveniently linked to a QPushButton
  /// signal.
  inline void showPopup(bool show);

Q_SIGNALS:
  void popupOpened(bool open);

protected:
  explicit ctkBasePopupWidget(ctkBasePopupWidgetPrivate* pimpl, QWidget* parent = 0);
  QScopedPointer<ctkBasePopupWidgetPrivate> d_ptr;
  Q_PROPERTY(double effectAlpha READ effectAlpha WRITE setEffectAlpha DESIGNABLE false)
  Q_PROPERTY(QRect effectGeometry READ effectGeometry WRITE setEffectGeometry DESIGNABLE false)

  double effectAlpha()const;
  QRect effectGeometry()const;

  virtual void setBaseWidget(QWidget* baseWidget);

  virtual bool event(QEvent* event);
  virtual void paintEvent(QPaintEvent*);

protected Q_SLOTS:
  virtual void onEffectFinished();
  void setEffectAlpha(double alpha);
  void setEffectGeometry(QRect geometry);
  void onBaseWidgetDestroyed();

private:
  Q_DECLARE_PRIVATE(ctkBasePopupWidget);
  Q_DISABLE_COPY(ctkBasePopupWidget);
};

Q_DECLARE_METATYPE(ctkBasePopupWidget::AnimationEffect)
Q_DECLARE_METATYPE(ctkBasePopupWidget::VerticalDirection)

// -------------------------------------------------------------------------
void ctkBasePopupWidget::showPopup(bool show)
{
  if (show)
    {
    this->showPopup();
    }
  else
    {
    this->hidePopup();
    }
}

#endif
