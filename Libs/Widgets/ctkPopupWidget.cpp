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
#include <QDesktopWidget>
#include <QDialog>
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

// -------------------------------------------------------------------------
ctkPopupWidgetPrivate::ctkPopupWidgetPrivate(ctkPopupWidget& object)
  :Superclass(object)
{
  this->Active = false;
  this->AutoShow = true;
  this->ShowDelay = 20;
  this->AutoHide = true;
  this->HideDelay = 200;
}

// -------------------------------------------------------------------------
ctkPopupWidgetPrivate::~ctkPopupWidgetPrivate()
{
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::init()
{
  Q_Q(ctkPopupWidget);
  this->setParent(q);
  q->setActive(true);
  this->Superclass::init();
}

// -------------------------------------------------------------------------
QWidget* ctkPopupWidgetPrivate::mouseOver()
{
  Q_Q(ctkPopupWidget);
  QWidget* widgetUnderCursor = this->Superclass::mouseOver();
  if (widgetUnderCursor &&
      !this->focusWidgets(true).contains(widgetUnderCursor))
    {
    widgetUnderCursor->installEventFilter(q);
    }
  return widgetUnderCursor;
}

// -------------------------------------------------------------------------
bool ctkPopupWidgetPrivate::eventFilter(QObject* obj, QEvent* event)
{
  Q_Q(ctkPopupWidget);
  QWidget* widget = qobject_cast<QWidget*>(obj);
  if (!widget)
    {
    return this->Superclass::eventFilter(obj, event);
    }
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
  if (this->BaseWidget.isNull())
    {
    return false;
    }
  if (event->type() == QEvent::Move && widget != this->BaseWidget)
    {
    if (widget->isAncestorOf(this->BaseWidget))
      {
      q->setGeometry(this->desiredOpenGeometry());
      }
    else if (this->isHidingCandidate(widget))
      {
      QTimer::singleShot(0, this, SLOT(updateVisibility()));
      }
    }
  else if (event->type() == QEvent::Resize)
    {
    if (widget->isAncestorOf(this->BaseWidget))
      {
      q->setGeometry(this->desiredOpenGeometry());
      }
    else if (this->isHidingCandidate(widget))
      {
      QTimer::singleShot(0, this, SLOT(updateVisibility()));
      }
    }
  else if (event->type() == QEvent::WindowStateChange &&
           this->isHidingCandidate(widget))
    {
    QTimer::singleShot(0, this, SLOT(updateVisibility()));
    }
  else if ((event->type() == QEvent::WindowActivate ||
            event->type() == QEvent::WindowDeactivate) &&
           widget == this->BaseWidget->window())
    {
    QTimer::singleShot(0, this, SLOT(updateVisibility()));
    }
  else if (event->type() == QEvent::RequestSoftwareInputPanel)
    {
    qApp->setActiveWindow(widget->window());
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
bool ctkPopupWidgetPrivate::isHidingCandidate(QWidget* widget)const
{
  // The mac window manager is keeping the Qt:Tool widgets always on top,
  // so if a non modal dialog is moved near the popup widget, the popup will
  // always appear on top of the dialog. For this reason we manually have to
  // hide the popup when a dialog is intersecting with the popup.
  bool canWindowsHidePopup = false;
#if defined Q_OS_MAC
  canWindowsHidePopup = true;
#endif
  bool isWindow = widget->isWindow();
  QDialog* dialog = qobject_cast<QDialog*>(widget);
  bool isModal = dialog ? dialog->isModal() : false;
  bool isBasePopupWidget = qobject_cast<ctkBasePopupWidget*>(widget);
  bool isToolTip = widget->windowType() == Qt::ToolTip;
  bool isPopup = widget->windowType() == Qt::Popup;
  bool isSelf = (widget == (this->BaseWidget ? this->BaseWidget->window() : 0));

  return canWindowsHidePopup && isWindow && !isModal && !isBasePopupWidget &&
    !isToolTip && !isPopup && !isSelf;
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::updateVisibility()
{
  Q_Q(ctkPopupWidget);
  // If the BaseWidget window is active, then there is no reason to cover the
  // popup.
  if (this->BaseWidget.isNull()  ||
      // the popupwidget active window is not active
      (!this->BaseWidget->window()->isActiveWindow() &&
      // and no other active window
       (!qApp->activeWindow() ||
      // or the active window is a popup
        (!qobject_cast<ctkBasePopupWidget*>(qApp->activeWindow()) && //->windowType() != PopupWindowType &&
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
          this->isHidingCandidate(topLevelWidget) &&
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
  if (!this->BaseWidget.isNull() &&
      (!this->BaseWidget->isVisible() ||
        this->BaseWidget->window()->windowState() & Qt::WindowMinimized))
    {
    return;
    }
  // Restore the visibility of the popup if it was hidden
  this->temporarilyHiddenOff();
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
// Qt::FramelessWindowHint is required on Windows for Translucent background
// Qt::Toolip is preferred to Qt::Popup as it would close itself at the first
// click outside the widget (typically a click in the BaseWidget)
ctkPopupWidget::ctkPopupWidget(QWidget* parentWidget)
  : Superclass(new ctkPopupWidgetPrivate(*this), parentWidget)
{
  Q_D(ctkPopupWidget);
  d->init();
}

// -------------------------------------------------------------------------
ctkPopupWidget::~ctkPopupWidget()
{
}

// -------------------------------------------------------------------------
bool ctkPopupWidget::isActive()const
{
  Q_D(const ctkPopupWidget);
  return d->Active;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setActive(bool active)
{
  Q_D(ctkPopupWidget);
  if (active == d->Active)
    {
    return;
    }
  d->Active = active;
  if (d->Active)
    {
    if (!d->BaseWidget.isNull())
      {
      d->BaseWidget->installEventFilter(this);
      }
    if (d->PopupPixmapWidget)
      {
      d->PopupPixmapWidget->installEventFilter(this);
      }
    qApp->installEventFilter(d);
    }
  else // not active
    {
    if (!d->BaseWidget.isNull())
      {
      d->BaseWidget->removeEventFilter(this);
      }
    if (d->PopupPixmapWidget)
      {
      d->PopupPixmapWidget->removeEventFilter(this);
      }
    qApp->removeEventFilter(d);
    }
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setBaseWidget(QWidget* widget)
{
  Q_D(ctkPopupWidget);
  if (!d->BaseWidget.isNull())
    {
    d->BaseWidget->removeEventFilter(this);
    }
  this->Superclass::setBaseWidget(widget);
  if (!d->BaseWidget.isNull() && d->Active)
    {
    d->BaseWidget->installEventFilter(this);
    }
  QTimer::singleShot(d->ShowDelay, this, SLOT(updatePopup()));
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
  QTimer::singleShot(d->ShowDelay, this, SLOT(updatePopup()));
}

// -------------------------------------------------------------------------
int ctkPopupWidget::showDelay()const
{
  Q_D(const ctkPopupWidget);
  return d->ShowDelay;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setShowDelay(int delay)
{
  Q_D(ctkPopupWidget);
  d->ShowDelay = delay;
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
  QTimer::singleShot(d->HideDelay, this, SLOT(updatePopup()));
}

// -------------------------------------------------------------------------
int ctkPopupWidget::hideDelay()const
{
  Q_D(const ctkPopupWidget);
  return d->HideDelay;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setHideDelay(int delay)
{
  Q_D(ctkPopupWidget);
  d->HideDelay = delay;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::onEffectFinished()
{
  Q_D(ctkPopupWidget);
  bool wasClosing = d->wasClosing();
  this->Superclass::onEffectFinished();
  if (wasClosing)
    {
    /// restore the AutoShow if needed.
    if (!this->property("AutoShowOnClose").isNull())
      {
      d->AutoShow = this->property("AutoShowOnClose").toBool();
      this->setProperty("AutoShowOnClose", QVariant());
      }
    }
}

// --------------------------------------------------------------------------
void ctkPopupWidget::leaveEvent(QEvent* event)
{
  Q_D(ctkPopupWidget);
  QTimer::singleShot(d->HideDelay, this, SLOT(updatePopup()));
  this->Superclass::leaveEvent(event);
}

// --------------------------------------------------------------------------
void ctkPopupWidget::enterEvent(QEvent* event)
{
  Q_D(ctkPopupWidget);
  QTimer::singleShot(d->ShowDelay, this, SLOT(updatePopup()));
  this->Superclass::enterEvent(event);
}

// --------------------------------------------------------------------------
bool ctkPopupWidget::eventFilter(QObject* obj, QEvent* event)
{
  Q_D(ctkPopupWidget);
  // Here we listen to PopupPixmapWidget, BaseWidget and ctkPopupWidget
  // children popups that were under the mouse
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
      if (obj != d->BaseWidget)
        {
        if (obj != d->PopupPixmapWidget &&
            qobject_cast<QWidget*>(obj)->windowType() == Qt::Popup)
          {
          obj->removeEventFilter(this);
          QTimer::singleShot(d->HideDelay, this, SLOT(updatePopup()));
          }
        break;
        }
      d->temporarilyHiddenOn();
      break;
    case QEvent::Show:
      if (obj != d->BaseWidget)
        {
        break;
        }
      this->setGeometry(d->desiredOpenGeometry());
      d->temporarilyHiddenOff();
      break;
    case QEvent::Enter:
      if ( d->currentAnimation()->state() == QAbstractAnimation::Stopped )
        {
        // Maybe the user moved the mouse on the widget by mistake, don't open
        // the popup instantly...
        QTimer::singleShot(d->ShowDelay, this, SLOT(updatePopup()));
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
          obj != d->PopupPixmapWidget &&
          qobject_cast<QWidget*>(obj)->windowType() == Qt::Popup)
        {
        break;
        }
      // The mouse might have left the area that keeps the popup open
      QTimer::singleShot(d->HideDelay, this, SLOT(updatePopup()));
      if (obj != d->BaseWidget &&
          obj != d->PopupPixmapWidget)
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
  QWidget* mouseOver = (d->AutoShow || d->AutoHide) ? d->mouseOver() : 0;
  if ((d->AutoShow ||
     // Even if there is no AutoShow, we might still want to reopen the popup
     // when closing it inadvertently, except if we are un-pin-ing the popup
      (d->AutoHide && d->isClosing() && this->property("AutoShowOnClose").toBool())) &&
     // to be automatically open, the mouse has to be over a child widget
      mouseOver &&
     // disable opening the popup when the popup is disabled
      (d->BaseWidget.isNull() || d->BaseWidget->isEnabled()))
    {
    this->showPopup();
    }
  else if (d->AutoHide && !mouseOver)
    {
    this->hidePopup();
    }
}


// --------------------------------------------------------------------------
void ctkPopupWidget::hidePopup()
{
  // just in case it was set.
  this->setProperty("forcedClosed", 0);

  this->Superclass::hidePopup();
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
