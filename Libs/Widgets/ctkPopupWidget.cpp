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
#include <QDesktopWidget>
#include <QDir>
#include <QEvent>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QPainter>
#include <QPointer>
#include <QPropertyAnimation>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkPopupWidget_p.h"

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
  this->Orientations = Qt::Vertical;
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
  // By default, Tooltips are shown only on active windows. In a popup widget
  // case, we sometimes aren't the active window but we still would like to
  // show the children tooltips.
  q->setAttribute(Qt::WA_AlwaysShowToolTips, true);

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

  qApp->installEventFilter(this);

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
bool ctkPopupWidgetPrivate::isOpening()const
{
  return this->currentAnimation()->state() == QAbstractAnimation::Running &&
    this->currentAnimation()->direction() == QAbstractAnimation::Forward;
}

// -------------------------------------------------------------------------
bool ctkPopupWidgetPrivate::isClosing()const
{
  return this->currentAnimation()->state() == QAbstractAnimation::Running &&
    this->currentAnimation()->direction() == QAbstractAnimation::Backward;
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
  const QPoint pos = QCursor::pos();
  QWidget* widgetUnderCursor = qApp->widgetAt(pos);
  foreach(const QWidget* focusWidget, widgets)
    {
    if (this->isAncestorOf(focusWidget, widgetUnderCursor) &&
        // Ignore when cursor is above a title bar of a focusWidget, underMouse
        // wouldn't have return false, but QApplication::widgetAt would return
        // the widget
        (focusWidget != widgetUnderCursor ||
         QRect(QPoint(0,0), focusWidget->size()).contains(
          focusWidget->mapFromGlobal(pos))))
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
  if (this->Orientations & Qt::Vertical)
    {
    if (this->VerticalDirection == ctkPopupWidget::BottomToTop)
      {
      openGeom.moveTop(openGeom.bottom());
      }
    openGeom.setHeight(0);
    }
  if (this->Orientations & Qt::Horizontal)
    {
    if (this->HorizontalDirection == Qt::RightToLeft)
      {
      openGeom.moveLeft(openGeom.right());
      }
    openGeom.setWidth(0);
    }
  return openGeom;
}

// -------------------------------------------------------------------------
QRect ctkPopupWidgetPrivate::baseGeometry()const
{
  if (!this->BaseWidget)
    {
    return QRect();
    }
  return QRect(this->mapToGlobal(this->BaseWidget->geometry().topLeft()),
               this->BaseWidget->size());
}

// -------------------------------------------------------------------------
QPoint ctkPopupWidgetPrivate::mapToGlobal(const QPoint& baseWidgetPoint)const
{
  QPoint mappedPoint = baseWidgetPoint;
  if (this->BaseWidget && this->BaseWidget->parentWidget())
    {
    mappedPoint = this->BaseWidget->parentWidget()->mapToGlobal(mappedPoint);
    }
  return mappedPoint;
}

// -------------------------------------------------------------------------
QRect ctkPopupWidgetPrivate::desiredOpenGeometry()const
{
  return this->desiredOpenGeometry(this->baseGeometry());
}

// -------------------------------------------------------------------------
QRect ctkPopupWidgetPrivate::desiredOpenGeometry(QRect baseGeometry)const
{
  Q_Q(const ctkPopupWidget);
  QSize size = q->size();
  if (!q->testAttribute(Qt::WA_WState_Created))
    {
    size = q->sizeHint();
    }
  
  if (baseGeometry.isNull())
    {
    return QRect(q->pos(), size);
    }

  QRect geometry;
  if (this->Alignment & Qt::AlignJustify)
    {
    if (this->Orientations & Qt::Vertical)
      {
      size.setWidth(baseGeometry.width());
      }
    }
  if (this->Alignment & Qt::AlignTop &&
      this->Alignment & Qt::AlignBottom)
    {
    size.setHeight(baseGeometry.height());
    }

  geometry.setSize(size);

  QPoint topLeft = baseGeometry.topLeft();
  QPoint bottomRight = baseGeometry.bottomRight();

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
    geometry.moveLeft((topLeft.x() + bottomRight.x()) / 2 - size.width() / 2);
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
    geometry.moveTop((topLeft.y() + bottomRight.y()) / 2 - size.height() / 2);
    }
  return geometry;
}

// -------------------------------------------------------------------------
bool ctkPopupWidgetPrivate::eventFilter(QObject* obj, QEvent* event)
{
  Q_Q(ctkPopupWidget);
  QWidget* widget = qobject_cast<QWidget*>(obj);
  // Here are the application events, it's a lot of events, so we need to be
  // careful to be fast.
  if (event->type() == QEvent::ApplicationDeactivate)
    {
    // We wait to see if there is no other window being active
    QTimer::singleShot(0, this, SLOT(onApplicationDeactivate()));
    }
  else if (event->type() == QEvent::ApplicationActivate)
    {
    QTimer::singleShot(0, this, SLOT(updateVisibility()));
    }
  if (!this->BaseWidget)
    {
    return false;
    }
  if (event->type() == QEvent::Move && widget != this->BaseWidget)
    {
    if (widget->isAncestorOf(this->BaseWidget))
      {
      QMoveEvent* moveEvent = dynamic_cast<QMoveEvent*>(event);
      QPoint topLeft = widget->parentWidget() ? widget->parentWidget()->mapToGlobal(moveEvent->pos()) : moveEvent->pos();
      topLeft += this->BaseWidget->mapTo(widget, QPoint(0,0));
      //q->move(q->pos() + moveEvent->pos() - moveEvent->oldPos());
      QRect newBaseGeometry = this->baseGeometry();
	    newBaseGeometry.moveTopLeft(topLeft);
	    QRect desiredGeometry = this->desiredOpenGeometry(newBaseGeometry);
	    q->move(desiredGeometry.topLeft());
      }
    else if (widget->isWindow() &&
             widget->windowType() != Qt::ToolTip &&
             widget->windowType() != Qt::Popup)
      {
      QTimer::singleShot(0, this, SLOT(updateVisibility()));
      }
    }
  else if (event->type() == QEvent::Resize)
    {
    if (widget->isWindow() &&
        widget != this->BaseWidget->window() &&
        widget->windowType() != Qt::ToolTip &&
        widget->windowType() != Qt::Popup)
      {
      QTimer::singleShot(0, this, SLOT(updateVisibility()));
      }
    }
  else if (event->type() == QEvent::WindowStateChange &&
           widget != this->BaseWidget->window() &&
           widget->windowType() != Qt::ToolTip &&
           widget->windowType() != Qt::Popup)
    {
    QTimer::singleShot(0, this, SLOT(updateVisibility()));
    }
  else if ((event->type() == QEvent::WindowActivate ||
            event->type() == QEvent::WindowDeactivate) &&
           widget == this->BaseWidget->window())
    {
    QTimer::singleShot(0, this, SLOT(updateVisibility()));
    }
  return false;
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::onApplicationDeactivate()
{
  // Still no active window, that means the user now is controlling another
  // application, we have no control over when the other app moves over the
  // popup, so we hide the popup as it would show on top of the other app.
  if (!qApp->activeWindow())
    {
    this->temporarilyHiddenOn();
    }
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::updateVisibility()
{
  Q_Q(ctkPopupWidget);
  // If the BaseWidget window is active, then there is no reason to cover the
  // popup.
  if (!this->BaseWidget  ||
      // the popupwidget active window is not active
      (!this->BaseWidget->window()->isActiveWindow() &&
      // and no other active window
       (!qApp->activeWindow() ||
      // or the active window is a popup/tooltip
        (qApp->activeWindow()->windowType() != Qt::ToolTip &&
         qApp->activeWindow()->windowType() != Qt::Popup))))
    {
    foreach(QWidget* topLevelWidget, qApp->topLevelWidgets())
      {
      // If there is at least 1 window (active or not) that covers the popup,
      // then we ensure the popup is hidden.
      // We have no way of knowing which toplevel is over (z-order) which one,
      // it is an OS specific information.
      // Of course, tooltips and popups don't count as covering windows.
      if (topLevelWidget->isVisible() &&
          !(topLevelWidget->windowState() & Qt::WindowMinimized) &&
          topLevelWidget->windowType() != Qt::ToolTip &&
          topLevelWidget->windowType() != Qt::Popup &&
          topLevelWidget != (this->BaseWidget ? this->BaseWidget->window() : 0) &&
          topLevelWidget->frameGeometry().intersects(q->geometry()))
        {
        //qDebug() << "hide" << q << "because of: " << topLevelWidget
        //         << " with windowType: " << topLevelWidget->windowType()
        //         << topLevelWidget->isVisible()
        //         << (this->BaseWidget ? this->BaseWidget->window() : 0)
        //         << topLevelWidget->frameGeometry();
        this->temporarilyHiddenOn();
        return;
        }
      }
    }
  // If the base widget is hidden or minimized, we don't want to restore the
  // popup.
  if (this->BaseWidget &&
      (!this->BaseWidget->isVisible() ||
        this->BaseWidget->window()->windowState() & Qt::WindowMinimized))
    {
    return;
    }
  // Restore the visibility of the popup if it was hidden
  this->temporarilyHiddenOff();
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::onBaseWidgetDestroyed()
{
  Q_Q(ctkPopupWidget);
  this->hideAll();
  q->setBaseWidget(0);
  // could be a property.
  q->deleteLater();
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::temporarilyHiddenOn()
{
  Q_Q(ctkPopupWidget);
  if (!this->AutoHide &&
      (q->isVisible() || this->isOpening()) &&
      !(q->isHidden() || this->isClosing()))
    {
    this->setProperty("forcedClosed", this->isOpening() ? 2 : 1);
    }
  this->currentAnimation()->stop();
  this->hideAll();
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::temporarilyHiddenOff()
{
  Q_Q(ctkPopupWidget);

  int forcedClosed = this->property("forcedClosed").toInt();
  if (forcedClosed > 0)
    {
    q->show();
    if (forcedClosed == 2)
      {
      emit q->popupOpened(true);
      }
    this->setProperty("forcedClosed", 0);
    }
  else
    {
    q->updatePopup();
    }
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::hideAll()
{
  Q_Q(ctkPopupWidget);
  // Before hiding, transfer the active window flag to its parent, this will
  // prevent the application to send a ApplicationDeactivate signal that
  // doesn't need to be done.
  if (q->isActiveWindow() && this->BaseWidget)
    {
    qApp->setActiveWindow(this->BaseWidget->window());
    }

  q->hide();
  this->PopupPixmapWidget->hide();

  // If there is a popup open in the ctkPopupWidget children, then hide it
  // as well so we don't have a popup open while the ctkPopupWidget is hidden.
  QPointer<QWidget> activePopupWidget = qApp->activePopupWidget();
  if (activePopupWidget && this->isAncestorOf(q, activePopupWidget))
    {
    activePopupWidget->close();
    }
}

// -------------------------------------------------------------------------
// Qt::FramelessWindowHint is required on Windows for Translucent background
// Qt::Toolip is preferred to Qt::Popup as it would close itself at the first
// click outside the widget (typically a click in the BaseWidget)
ctkPopupWidget::ctkPopupWidget(QWidget* parentWidget)
  : Superclass(QApplication::desktop()->screen(QApplication::desktop()->screenNumber(parentWidget)),
               Qt::ToolTip | Qt::FramelessWindowHint)
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
    disconnect(d->BaseWidget, SIGNAL(destroyed(QObject*)),
               d, SLOT(onBaseWidgetDestroyed()));
    }
  d->BaseWidget = widget;
  if (d->BaseWidget)
    {
    d->BaseWidget->installEventFilter(this);
    connect(d->BaseWidget, SIGNAL(destroyed(QObject*)),
            d, SLOT(onBaseWidgetDestroyed()));
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
Qt::Orientations ctkPopupWidget::orientation()const
{
  Q_D(const ctkPopupWidget);
  return d->Orientations;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setOrientation(Qt::Orientations orientations)
{
  Q_D(ctkPopupWidget);
  d->Orientations = orientations;
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
    d->hideAll();
    emit this->popupOpened(false);
    /// restore the AutoShow if needed.
    if (!this->property("AutoShowOnClose").isNull())
      {
      d->AutoShow = this->property("AutoShowOnClose").toBool();
      this->setProperty("AutoShowOnClose", QVariant());
      }
    }
  else
    {
    this->show();
#ifdef Q_WS_X11
    // If the OS applies effects on window appearance, it
    // can take time for the popup to be displayed, we don't want to
    // hide the pixmap too early otherwise to would makes it "flicker"
    // It has the disadvantage of 'opaquing' if the popup is
    // semi transparent because the pixmap and the popup opacities
    // get summed up until the pixmap is hidden.
    // Alternatively, you could remove effects on windows with Compiz.
    QTimer::singleShot(100, d->PopupPixmapWidget, SLOT(hide()));
#else
    d->PopupPixmapWidget->hide();
#endif
    emit this->popupOpened(true);
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
  switch(event->type())
    {
    case QEvent::Move:
      {
	    if (obj != d->BaseWidget)
	      {
	      break;
	      }
	    QMoveEvent* moveEvent = dynamic_cast<QMoveEvent*>(event);
	    QRect newBaseGeometry = d->baseGeometry();
	    newBaseGeometry.moveTopLeft(d->mapToGlobal(moveEvent->pos()));
	    QRect desiredGeometry = d->desiredOpenGeometry(newBaseGeometry);
	    this->move(desiredGeometry.topLeft());
	    //this->move(this->pos() + moveEvent->pos() - moveEvent->oldPos());
	    this->update();
	    break;
	    }	    
    case QEvent::Hide:
    case QEvent::Close:
      // if the mouse was in a base widget child popup, then when we leave
      // the popup we want to check if it needs to be closed.
      if (obj != d->BaseWidget  &&
          qobject_cast<QWidget*>(obj)->windowType() == Qt::Popup)
        {
        QTimer::singleShot(LEAVE_CLOSING_DELAY, this, SLOT(updatePopup()));
        obj->removeEventFilter(this);
        break;
        }
      d->temporarilyHiddenOn();
	    break;
    case QEvent::Show:
      if (obj != d->BaseWidget)
        {
	      break;
	      }
	    d->temporarilyHiddenOff();
	    break;
	  case QEvent::Resize:
	    if (obj != d->BaseWidget ||
	        !(d->Alignment & Qt::AlignJustify ||
	         (d->Alignment & Qt::AlignTop && d->Alignment & Qt::AlignBottom)) ||
	         !(d->isOpening() || this->isVisible()))
	      {
	      break;
	      }
	    // TODO: bug when the effect is WindowOpacityFadeEffect
	    this->setGeometry(d->desiredOpenGeometry());
	    break;
    case QEvent::Enter:
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
      break;
    case QEvent::Leave:
      // Don't listen to base widget children that are popups as what
      // matters here is their close event instead
      if (obj != d->BaseWidget &&
          qobject_cast<QWidget*>(obj)->windowType() == Qt::Popup)
        {
        break;
        }
      // The mouse might have left the area that keeps the popup open
      QTimer::singleShot(LEAVE_CLOSING_DELAY, this, SLOT(updatePopup()));
      if (obj != d->BaseWidget)
        {
        obj->removeEventFilter(this);
        }
      break;
    default:
      break;
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

  // just in case it was set.
  this->setProperty("forcedClosed", 0);

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
      d->ScrollAnimation->setStartValue(closedGeometry);
      d->ScrollAnimation->setEndValue(openGeometry);
      d->setupPopupPixmapWidget();
      d->PopupPixmapWidget->setGeometry(this->geometry());
      d->PopupPixmapWidget->show();
      if (this->isActiveWindow())
        {
        qApp->setActiveWindow(d->BaseWidget ? d->BaseWidget->window() : 0);
        }
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
  Q_D(ctkPopupWidget);
  this->setAutoHide(!pin);
  if (pin)
    {
    this->showPopup();
    }
  else
    {
    // When closing, we don't want to inadvertently re-open the menu.
    this->setProperty("AutoShowOnClose", this->autoShow());
    d->AutoShow = false;
    this->hidePopup();
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
