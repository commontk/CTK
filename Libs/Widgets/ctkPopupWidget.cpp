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

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QEvent>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkPopupWidget.h"

#define LEAVE_CLOSING_DELAY 100 // we don't want to be too fast to close
#define ENTER_OPENING_DELAY 20 // we want to be responsive but allow "errors"
#define DEFAULT_FADING_DURATION 333 // fast enough without being too slow

// -------------------------------------------------------------------------
QGradient* duplicateGradient(const QGradient* gradient)
{
  QGradient* newGradient = 0;
  switch (gradient->type())
    {
    case QGradient::LinearGradient:
      {
      const QLinearGradient* linearGradient = static_cast<const QLinearGradient*>(gradient);
      newGradient = new QLinearGradient(linearGradient->start(), linearGradient->finalStop());
      break;
      }
    case QGradient::RadialGradient:
      {
      const QRadialGradient* radialGradient = static_cast<const QRadialGradient*>(gradient);
      newGradient = new QRadialGradient(radialGradient->center(), radialGradient->radius());
      break;
      }
    case QGradient::ConicalGradient:
      {
      const QConicalGradient* conicalGradient = static_cast<const QConicalGradient*>(gradient);
      newGradient = new QConicalGradient(conicalGradient->center(), conicalGradient->angle());
      break;
      }
    default:
      break;
    }
  if (!newGradient)
    {
    Q_ASSERT(gradient->type() != QGradient::NoGradient);
    return newGradient;
    }
  newGradient->setCoordinateMode(gradient->coordinateMode());
  newGradient->setSpread(gradient->spread());
  newGradient->setStops(gradient->stops());
  return newGradient;
}

// -------------------------------------------------------------------------
class ctkPopupWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkPopupWidget);
protected:
  ctkPopupWidget* const q_ptr;
public:
  ctkPopupWidgetPrivate(ctkPopupWidget& object);
  ~ctkPopupWidgetPrivate();
  void init();
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
  Qt::Alignment   Alignment;
  Qt::Orientation Orientation;
  
  ctkPopupWidget::VerticalDirection VerticalDirection;
  Qt::LayoutDirection HorizontalDirection;
};


// -------------------------------------------------------------------------
ctkPopupWidgetPrivate::ctkPopupWidgetPrivate(ctkPopupWidget& object)
  :q_ptr(&object)
{
  this->BaseWidget = 0;
  this->AutoShow = true;
  this->AutoHide = true;
  this->Effect = ctkPopupWidget::ScrollEffect;
  this->EffectAlpha = 1.;
  this->AlphaAnimation = 0;
  this->ForcedTranslucent = false;
  this->ScrollAnimation = 0;
  this->PopupPixmapWidget = 0;
  // Geometry attributes
  this->Alignment = Qt::AlignJustify | Qt::AlignBottom;
  this->Orientation = Qt::Vertical;
  this->VerticalDirection = ctkPopupWidget::TopToBottom;
  this->HorizontalDirection = Qt::LeftToRight;
}

// -------------------------------------------------------------------------
ctkPopupWidgetPrivate::~ctkPopupWidgetPrivate()
{
  delete this->PopupPixmapWidget;
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::init()
{
  Q_Q(ctkPopupWidget);

  this->AlphaAnimation = new QPropertyAnimation(q, "effectAlpha", q);
  this->AlphaAnimation->setDuration(DEFAULT_FADING_DURATION);
  this->AlphaAnimation->setStartValue(0.);
  this->AlphaAnimation->setEndValue(1.);
  QObject::connect(this->AlphaAnimation, SIGNAL(finished()),
                   q, SLOT(onEffectFinished()));

  this->PopupPixmapWidget = new QLabel(0, Qt::ToolTip | Qt::FramelessWindowHint);
  this->ScrollAnimation = new QPropertyAnimation(q, "effectGeometry", q);
  this->ScrollAnimation->setDuration(DEFAULT_FADING_DURATION);
  QObject::connect(this->ScrollAnimation, SIGNAL(finished()),
                   q, SLOT(onEffectFinished()));
  QObject::connect(this->ScrollAnimation, SIGNAL(finished()),
                   this->PopupPixmapWidget, SLOT(hide()));

  q->setAnimationEffect(this->Effect);
  q->setEasingCurve(QEasingCurve::OutCubic);
}

// -------------------------------------------------------------------------
QPropertyAnimation* ctkPopupWidgetPrivate::currentAnimation()const
{
  return this->Effect == ctkPopupWidget::ScrollEffect ?
    this->ScrollAnimation : this->AlphaAnimation;
}

// -------------------------------------------------------------------------
QList<const QWidget*> ctkPopupWidgetPrivate::focusWidgets(bool onlyVisible)const
{
  Q_Q(const ctkPopupWidget);
  QList<const QWidget*> res;
  if (!onlyVisible || q->isVisible())
    {
    res << q;
    }
  if (this->BaseWidget && (!onlyVisible || this->BaseWidget->isVisible()))
    {
    res << this->BaseWidget;
    }
  if (this->PopupPixmapWidget && (!onlyVisible || this->PopupPixmapWidget->isVisible()))
    {
    res << this->PopupPixmapWidget;
    }
  return res;
}

// -------------------------------------------------------------------------
bool ctkPopupWidgetPrivate::mouseOver()
{
  Q_Q(ctkPopupWidget);
  QList<const QWidget*> widgets = this->focusWidgets(true);
  foreach(const QWidget* widget, widgets)
    {
    if (widget->underMouse())
      {
      return true;
      }
    }
  // Warning QApplication::widgetAt(QCursor::pos()) can be a bit slow...
  QWidget* widgetUnderCursor = qApp->widgetAt(QCursor::pos());
  foreach(const QWidget* focusWidget, widgets)
    {
    if (this->isAncestorOf(focusWidget, widgetUnderCursor))
      {
      widgetUnderCursor->installEventFilter(q);
      return true;
      }
    }
  return false;
}

// -------------------------------------------------------------------------
bool ctkPopupWidgetPrivate::isAncestorOf(const QWidget* ancestor, const QWidget* child)const
{
  while (child)
    {
    if (child == ancestor)
        return true;
    child = child->parentWidget();
    }
  return false;
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::setupPopupPixmapWidget()
{
  Q_Q(ctkPopupWidget);
  this->PopupPixmapWidget->setAlignment(this->pixmapAlignment());  
  QPixmap pixmap;
  if (q->testAttribute(Qt::WA_TranslucentBackground))
    {
    // only QImage handle transparency correctly
    QImage image(q->geometry().size(), QImage::Format_ARGB32);
    image.fill(0);
    q->render(&image);
    pixmap = QPixmap::fromImage(image);
    }
  else
    {
    pixmap = QPixmap::grabWidget(q, QRect(QPoint(0,0), q->geometry().size()));
    }
  this->PopupPixmapWidget->setPixmap(pixmap);
  this->PopupPixmapWidget->setAttribute(
    Qt::WA_TranslucentBackground, q->testAttribute(Qt::WA_TranslucentBackground));
  this->PopupPixmapWidget->setWindowOpacity(q->windowOpacity());
}

// -------------------------------------------------------------------------
Qt::Alignment ctkPopupWidgetPrivate::pixmapAlignment()const
{
  Qt::Alignment alignment;
  if (this->VerticalDirection == ctkPopupWidget::TopToBottom)
    {
    alignment |= Qt::AlignBottom;
    }
  else// if (this->VerticalDirection == ctkPopupWidget::BottomToTop)
    {
    alignment |= Qt::AlignTop;
    }

  if (this->HorizontalDirection == Qt::LeftToRight)
    {
    alignment |= Qt::AlignRight;
    }
  else// if (this->VerticalDirection == ctkPopupWidget::BottomToTop)
    {
    alignment |= Qt::AlignLeft;
    }
  return alignment;
}

// -------------------------------------------------------------------------
QRect ctkPopupWidgetPrivate::closedGeometry()const
{
  Q_Q(const ctkPopupWidget);
  return this->closedGeometry(q->geometry());
}

// -------------------------------------------------------------------------
QRect ctkPopupWidgetPrivate::closedGeometry(QRect openGeom)const
{
  if (this->Orientation & Qt::Vertical)
    {
    if (this->VerticalDirection == ctkPopupWidget::BottomToTop)
      {
      openGeom.moveTop(openGeom.bottom());
      }
    openGeom.setHeight(0);
    }
  if (this->Orientation & Qt::Horizontal) 
    {
    if (this->HorizontalDirection == Qt::LeftToRight)
      {
      openGeom.moveLeft(openGeom.right());
      }
    openGeom.setWidth(0);
    }
  return openGeom;
}

// -------------------------------------------------------------------------
QRect ctkPopupWidgetPrivate::desiredOpenGeometry()const
{
  Q_Q(const ctkPopupWidget);
  QSize size = q->size();
  if (!q->testAttribute(Qt::WA_WState_Created))
    {
    size = q->sizeHint();
    }
  
  if (!this->BaseWidget)
    {
    return QRect(q->pos(), size);
    }

  QRect geometry;
  if (this->Alignment & Qt::AlignJustify)
    {
    if (this->Orientation & Qt::Vertical)
      {
      size.setWidth(this->BaseWidget->width());
      }
    }
  if (this->Alignment & Qt::AlignTop &&
      this->Alignment & Qt::AlignBottom)
    {
    size.setHeight(this->BaseWidget->height());
    }

  geometry.setSize(size);

  QPoint topLeft = QPoint(this->BaseWidget->geometry().left(), this->BaseWidget->geometry().top());
  QPoint bottomRight = QPoint(this->BaseWidget->geometry().right(), this->BaseWidget->geometry().bottom());
  
  topLeft = this->BaseWidget->parentWidget() ? this->BaseWidget->parentWidget()->mapToGlobal(topLeft) : topLeft;
  bottomRight = this->BaseWidget->parentWidget() ? this->BaseWidget->parentWidget()->mapToGlobal(bottomRight) : bottomRight;

  if (this->Alignment & Qt::AlignLeft)
    {
    if (this->HorizontalDirection == Qt::LeftToRight)
      {
      geometry.moveLeft(topLeft.x());
      }
    else
      {
      geometry.moveRight(topLeft.x());
      }
    }
  else if (this->Alignment & Qt::AlignRight)
    {
    if (this->HorizontalDirection == Qt::LeftToRight)
      {
      geometry.moveLeft(bottomRight.x());
      }
    else
      {
      geometry.moveRight(bottomRight.x());
      }
    }
  else if (this->Alignment & Qt::AlignHCenter)
    {
    if (this->HorizontalDirection == Qt::LeftToRight)
      {
      geometry.moveLeft((topLeft.x() + bottomRight.x()) / 2 - size.width() / 2);
      }
    else
      {
      geometry.moveRight((topLeft.x() + bottomRight.x()) / 2 + size.width() / 2);
      }
    }
  else if (this->Alignment & Qt::AlignJustify)
    {
    geometry.moveLeft(topLeft.x());
    }

  if (this->Alignment & Qt::AlignTop)
    {
    if (this->VerticalDirection == ctkPopupWidget::TopToBottom)
      {
      geometry.moveTop(topLeft.y());
      }
    else
      {
      geometry.moveBottom(topLeft.y());
      }
    }
  else if (this->Alignment & Qt::AlignBottom)
    {
    if (this->VerticalDirection == ctkPopupWidget::TopToBottom)
      {
      geometry.moveTop(bottomRight.y());
      }
    else
      {
      geometry.moveBottom(bottomRight.y());
      }
    }
  else if (this->Alignment & Qt::AlignVCenter)
    {
    if (this->VerticalDirection == ctkPopupWidget::TopToBottom)
      {
      geometry.moveTop((topLeft.y() + bottomRight.y()) / 2 + size.height() / 2);
      }
    else
      {
      geometry.moveBottom((topLeft.y() + bottomRight.y()) / 2 - size.height() / 2);
      }
    }
  return geometry;
}

// -------------------------------------------------------------------------
// Qt::FramelessWindowHint is required on Windows for Translucent background
// Qt::Toolip is preferred to Qt::Popup as it would close itself at the first
// click outside the widget (typically a click in the BaseWidget)
ctkPopupWidget::ctkPopupWidget(QWidget* parentWidget)
  : Superclass(parentWidget, Qt::ToolTip | Qt::FramelessWindowHint)
  , d_ptr(new ctkPopupWidgetPrivate(*this))
{
  Q_D(ctkPopupWidget);
  d->init();
}

// -------------------------------------------------------------------------
ctkPopupWidget::~ctkPopupWidget()
{
}

// -------------------------------------------------------------------------
QWidget* ctkPopupWidget::baseWidget()const
{
  Q_D(const ctkPopupWidget);
  return d->BaseWidget;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setBaseWidget(QWidget* widget)
{
  Q_D(ctkPopupWidget);
  if (d->BaseWidget)
    {
    d->BaseWidget->removeEventFilter(this);
    }
  d->BaseWidget = widget;
  if (d->BaseWidget)
    {
    d->BaseWidget->installEventFilter(this);
    }
  QTimer::singleShot(ENTER_OPENING_DELAY, this, SLOT(updatePopup()));
}

// -------------------------------------------------------------------------
bool ctkPopupWidget::autoShow()const
{
  Q_D(const ctkPopupWidget);
  return d->AutoShow;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setAutoShow(bool mode)
{
  Q_D(ctkPopupWidget);
  d->AutoShow = mode;
  QTimer::singleShot(ENTER_OPENING_DELAY, this, SLOT(updatePopup()));
}

// -------------------------------------------------------------------------
bool ctkPopupWidget::autoHide()const
{
  Q_D(const ctkPopupWidget);
  return d->AutoHide;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setAutoHide(bool mode)
{
  Q_D(ctkPopupWidget);
  d->AutoHide = mode;
  QTimer::singleShot(LEAVE_CLOSING_DELAY, this, SLOT(updatePopup()));
}

// -------------------------------------------------------------------------
ctkPopupWidget::AnimationEffect ctkPopupWidget::animationEffect()const
{
  Q_D(const ctkPopupWidget);
  return d->Effect;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setAnimationEffect(ctkPopupWidget::AnimationEffect effect)
{
  Q_D(ctkPopupWidget);
  /// TODO: handle the case where there is an animation running
  d->Effect = effect;
}

// -------------------------------------------------------------------------
QEasingCurve::Type ctkPopupWidget::easingCurve()const
{
  Q_D(const ctkPopupWidget);
  return d->AlphaAnimation->easingCurve().type();
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setEasingCurve(QEasingCurve::Type easingCurve)
{
  Q_D(ctkPopupWidget);
  d->AlphaAnimation->setEasingCurve(easingCurve);
  d->ScrollAnimation->setEasingCurve(easingCurve);
}

// -------------------------------------------------------------------------
Qt::Alignment ctkPopupWidget::alignment()const
{
  Q_D(const ctkPopupWidget);
  return d->Alignment;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setAlignment(Qt::Alignment alignment)
{
  Q_D(ctkPopupWidget);
  d->Alignment = alignment;
}

// -------------------------------------------------------------------------
Qt::Orientation ctkPopupWidget::orientation()const
{
  Q_D(const ctkPopupWidget);
  return d->Orientation;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setOrientation(Qt::Orientation orientation)
{
  Q_D(ctkPopupWidget);
  d->Orientation = orientation;
}

// -------------------------------------------------------------------------
ctkPopupWidget::VerticalDirection ctkPopupWidget::verticalDirection()const
{
  Q_D(const ctkPopupWidget);
  return d->VerticalDirection;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setVerticalDirection(ctkPopupWidget::VerticalDirection verticalDirection)
{
  Q_D(ctkPopupWidget);
  d->VerticalDirection = verticalDirection;
}

// -------------------------------------------------------------------------
Qt::LayoutDirection ctkPopupWidget::horizontalDirection()const
{
  Q_D(const ctkPopupWidget);
  return d->HorizontalDirection;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setHorizontalDirection(Qt::LayoutDirection horizontalDirection)
{
  Q_D(ctkPopupWidget);
  d->HorizontalDirection = horizontalDirection;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::onEffectFinished()
{
  Q_D(ctkPopupWidget);
  if (d->ForcedTranslucent)
    {
    d->ForcedTranslucent = false;
    this->setAttribute(Qt::WA_TranslucentBackground, false);
    }
  if (qobject_cast<QAbstractAnimation*>(this->sender())->direction() == QAbstractAnimation::Backward)
    {
    this->hide();
    }
  else
    {
    this->show();
    }
}

// -------------------------------------------------------------------------
void ctkPopupWidget::paintEvent(QPaintEvent* event)
{
  Q_D(ctkPopupWidget);
  Q_UNUSED(event);

  QPainter painter(this);
  QBrush brush = this->palette().window();
  if (brush.style() == Qt::LinearGradientPattern ||
      brush.style() == Qt::ConicalGradientPattern ||
      brush.style() == Qt::RadialGradientPattern)
    {
    QGradient* newGradient = duplicateGradient(brush.gradient());
    QGradientStops stops;
    foreach(QGradientStop stop, newGradient->stops())
      {
      stop.second.setAlpha(stop.second.alpha() * d->EffectAlpha);
      stops.push_back(stop);
      }
    newGradient->setStops(stops);
    brush = QBrush(*newGradient);
    delete newGradient;
    }
  else
    {
    QColor color = brush.color();
    color.setAlpha(color.alpha() * d->EffectAlpha);
    brush.setColor(color);
    }
  //QColor semiTransparentColor = this->palette().window().color();
  //semiTransparentColor.setAlpha(d->CurrentAlpha);
  painter.fillRect(this->rect(), brush);
  // Let the QFrame draw itself if needed
  this->Superclass::paintEvent(event);
}

// --------------------------------------------------------------------------
void ctkPopupWidget::leaveEvent(QEvent* event)
{
  QTimer::singleShot(LEAVE_CLOSING_DELAY, this, SLOT(updatePopup()));
  this->Superclass::leaveEvent(event);
}

// --------------------------------------------------------------------------
void ctkPopupWidget::enterEvent(QEvent* event)
{
  QTimer::singleShot(ENTER_OPENING_DELAY, this, SLOT(updatePopup()));
  this->Superclass::enterEvent(event);
}

// --------------------------------------------------------------------------
bool ctkPopupWidget::eventFilter(QObject* obj, QEvent* event)
{
  Q_D(ctkPopupWidget);
  if (event->type() == QEvent::Enter)
    {
    if ( d->currentAnimation()->state() == QAbstractAnimation::Stopped )
      {
      // Maybe the user moved the mouse on the widget by mistake, don't open
      // the popup instantly...
      QTimer::singleShot(ENTER_OPENING_DELAY, this, SLOT(updatePopup()));
      }
    else 
      {
      // ... except if the popup is closing, we want to reopen it as sooon as
      // possible.
      this->updatePopup();
      }
    }
  else if (event->type() == QEvent::Leave)
    {
    QTimer::singleShot(LEAVE_CLOSING_DELAY, this, SLOT(updatePopup()));
    if (obj != d->BaseWidget)
      {
      obj->removeEventFilter(this);
      }
    }
  return this->QObject::eventFilter(obj, event);
}

// --------------------------------------------------------------------------
void ctkPopupWidget::updatePopup()
{
  Q_D(ctkPopupWidget);

  // Querying mouseOver can be slow, don't do it if not needed.
  bool mouseOver = (d->AutoShow || d->AutoHide) && d->mouseOver();
    
  if (d->AutoShow && mouseOver)
    {
    this->showPopup();
    }
  else if (d->AutoHide && !mouseOver)
    {
    this->hidePopup();
    }
}

// --------------------------------------------------------------------------
void ctkPopupWidget::showPopup()
{
  Q_D(ctkPopupWidget);
  if ((this->isVisible() &&
       d->currentAnimation()->state() == QAbstractAnimation::Stopped) ||
      (d->BaseWidget && !d->BaseWidget->isVisible()))
    {
    return;
    }

  // If the layout has never been activated, the widget doesn't know its
  // minSize/maxSize and we then wouldn't know what's its true geometry.
  if (this->layout() && !this->testAttribute(Qt::WA_WState_Created))
    {
    this->layout()->activate();
    }
  this->setGeometry(d->desiredOpenGeometry());
  /// Maybe the popup doesn't allow the desiredOpenGeometry if the widget
  /// minimum size is larger than the desired size.
  QRect openGeometry = this->geometry();
  QRect closedGeometry = d->closedGeometry();

  d->currentAnimation()->setDirection(QAbstractAnimation::Forward);
  
  switch(d->Effect)
    {
    case WindowOpacityFadeEffect:
      if (!this->testAttribute(Qt::WA_TranslucentBackground))
        {
        d->ForcedTranslucent = true;
        this->setAttribute(Qt::WA_TranslucentBackground, true);
        }
      this->show();
      break;
    case ScrollEffect:
      {
      d->PopupPixmapWidget->setGeometry(closedGeometry);
      d->ScrollAnimation->setStartValue(closedGeometry);
      d->ScrollAnimation->setEndValue(openGeometry);
      d->setupPopupPixmapWidget();
      d->PopupPixmapWidget->show();
      break;
      }
    default:
      break;
    }
  switch(d->currentAnimation()->state())
    {
    case QAbstractAnimation::Stopped:
      d->currentAnimation()->start();
      break;
    case QAbstractAnimation::Paused:
      d->currentAnimation()->resume();
      break;
    default:
    case QAbstractAnimation::Running:
      break;
    }
}

// --------------------------------------------------------------------------
void ctkPopupWidget::hidePopup()
{
  Q_D(ctkPopupWidget);

  if (!this->isVisible() &&
      d->currentAnimation()->state() == QAbstractAnimation::Stopped)
    {
    return;
    }
  d->currentAnimation()->setDirection(QAbstractAnimation::Backward);

  QRect openGeometry = this->geometry();
  QRect closedGeometry = d->closedGeometry();

  switch(d->Effect)
    {
    case WindowOpacityFadeEffect:
      if (!this->testAttribute(Qt::WA_TranslucentBackground))
        {
        d->ForcedTranslucent = true;
        this->setAttribute(Qt::WA_TranslucentBackground, true);
        }
      break;
    case ScrollEffect:
      {
      d->setupPopupPixmapWidget();
      d->PopupPixmapWidget->show();
      this->hide();
      break;
      }
    default:
      break;
    }
  switch(d->currentAnimation()->state())
    {
    case QAbstractAnimation::Stopped:
      d->currentAnimation()->start();
      break;
    case QAbstractAnimation::Paused:
      d->currentAnimation()->resume();
      break;
    default:
    case QAbstractAnimation::Running:
      break;
    }
}

// --------------------------------------------------------------------------
void ctkPopupWidget::pinPopup(bool pin)
{
  this->setAutoHide(!pin);
  if (pin)
    {
    this->showPopup();
    }
}

// --------------------------------------------------------------------------
double ctkPopupWidget::effectAlpha()const
{
  Q_D(const ctkPopupWidget);
  return d->EffectAlpha;
}

// --------------------------------------------------------------------------
void ctkPopupWidget::setEffectAlpha(double alpha)
{
  Q_D(ctkPopupWidget);
  d->EffectAlpha = alpha;
  this->repaint();
}

// --------------------------------------------------------------------------
QRect ctkPopupWidget::effectGeometry()const
{
  Q_D(const ctkPopupWidget);
  return d->PopupPixmapWidget->geometry();
}

// --------------------------------------------------------------------------
void ctkPopupWidget::setEffectGeometry(QRect newGeometry)
{
  Q_D(ctkPopupWidget);
  d->PopupPixmapWidget->setGeometry(newGeometry);
  d->PopupPixmapWidget->repaint();
}
