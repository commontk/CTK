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

#ifndef __ctkPopupWidget_h
#define __ctkPopupWidget_h

// Qt includes
#include <QEasingCurve>
#include <QFrame>
#include <QMetaType>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkPopupWidgetPrivate;

/// Description:
class CTK_WIDGETS_EXPORT ctkPopupWidget : public QFrame
{
  Q_OBJECT
  
  Q_ENUMS(AnimationEffect)
  Q_ENUMS(VerticalDirection)

  /// Control wether the popup automatically opens when the mouse
  /// enter the widget. True by default
  Q_PROPERTY( bool autoShow READ autoShow WRITE setAutoShow)

  /// Control wether the popup automatically closes when the mouse
  /// leaves the widget. True by default
  Q_PROPERTY( bool autoHide READ autoHide WRITE setAutoHide)

  /// ScrollEffect by default
  Q_PROPERTY( AnimationEffect animationEffect READ animationEffect WRITE setAnimationEffect)
  
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
  Q_PROPERTY( Qt::Orientation orientation READ orientation WRITE setOrientation);
  
  /// Control where the popup opens vertically.
  /// TopToBottom by default
  Q_PROPERTY( ctkPopupWidget::VerticalDirection verticalDirection READ verticalDirection WRITE setVerticalDirection);

  /// Control where the popup opens horizontally.
  /// LeftToRight by default
  Q_PROPERTY( Qt::LayoutDirection horizontalDirection READ horizontalDirection WRITE setHorizontalDirection);

public:
  typedef QFrame Superclass;
  explicit ctkPopupWidget(QWidget* parent = 0);
  virtual ~ctkPopupWidget();

  /// Widget the popup is attached to. It opens right under \a baseWidget
  /// and if the ctkPopupWidget sizepolicy contains the growFlag/shrinkFlag,
  /// it tries to resize itself to fit the same width of \a baseWidget.
  QWidget* baseWidget()const;
  void setBaseWidget(QWidget* baseWidget);

  bool autoShow()const;
  /// Calling setAutoShow automatically updates opens the popup if the cursor
  /// is above the popup or the base widget.
  void setAutoShow(bool);

  bool autoHide()const;
  /// Don't automatically close the popup when leaving the widget.
  /// Calling setAutoHide automatically updates the state close the popup
  /// if the mouse is not over the popup nor the base widget.
  void setAutoHide(bool autoHide);

  enum AnimationEffect
  {
    WindowOpacityFadeEffect = 0,
    ScrollEffect,
    FadeEffect
  };
  
  AnimationEffect animationEffect()const;
  void setAnimationEffect(AnimationEffect effect);
  
  QEasingCurve::Type easingCurve()const;
  void setEasingCurve(QEasingCurve::Type easingCurve);
  
  Qt::Alignment alignment()const;
  void setAlignment(Qt::Alignment alignment);
  
  Qt::Orientation orientation()const;
  void setOrientation(Qt::Orientation orientation);
  
  enum VerticalDirection{
    TopToBottom = 1,
    BottomToTop = 2
  };
  
  VerticalDirection verticalDirection()const;
  void setVerticalDirection(VerticalDirection direction);
  
  Qt::LayoutDirection horizontalDirection()const;
  void setHorizontalDirection(Qt::LayoutDirection direction);

public slots:
  /// Hide the popup if open or opening. It takes around 300ms 
  /// for the fading effect to hide the popup.
  void hidePopup();
  /// Open the popup if closed or closing. It takes around 300ms 
  /// for the fading effect to open the popup.
  void showPopup();
  /// Show/hide the popup. It can be conveniently linked to a QPushButton
  /// signal.
  inline void showPopup(bool show);
  
  /// Convenient function that calls setAutoHide(!pin) and opens the popup
  /// if pin is true regardless of the value of \a AutoShow.
  /// It is typically connected with a checkable button to anchor the popup.
  void pinPopup(bool pin);

protected slots:
  void updatePopup();
  void onEffectFinished();
  void setWindowAlpha(double alpha);
  void setWindowGeometry(QRect geometry);

protected:
  QScopedPointer<ctkPopupWidgetPrivate> d_ptr;
  Q_PROPERTY(double windowAlpha READ windowAlpha WRITE setWindowAlpha DESIGNABLE false)
  Q_PROPERTY(QRect windowGeometry READ windowGeometry WRITE setWindowGeometry DESIGNABLE false)

  virtual void paintEvent(QPaintEvent*);
  virtual void leaveEvent(QEvent* event);
  virtual void enterEvent(QEvent* event);
  virtual bool eventFilter(QObject* obj, QEvent* event);

  double windowAlpha()const;
  QRect windowGeometry()const;
private:
  Q_DECLARE_PRIVATE(ctkPopupWidget);
  Q_DISABLE_COPY(ctkPopupWidget);
};

Q_DECLARE_METATYPE(ctkPopupWidget::AnimationEffect)
Q_DECLARE_METATYPE(ctkPopupWidget::VerticalDirection)

// -------------------------------------------------------------------------
void ctkPopupWidget::showPopup(bool show)
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
