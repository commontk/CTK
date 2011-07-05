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
#include <QEvent>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkPopupWidget.h"

#define LEAVE_CLOSING_DELAY 66
#define ENTER_OPENING_DELAY 66
#define DEFAULT_FADING_DURATION 333 // fast enough
#define FADING_FPS 33 // 30 fps

// -------------------------------------------------------------------------
class ctkPopupWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkPopupWidget);
protected:
  ctkPopupWidget* const q_ptr;
public:
  ctkPopupWidgetPrivate(ctkPopupWidget& object);
  void init();

  enum OpenStateType{
    Closed =0,
    Closing,
    Opening,
    Open
  };
  
  QWidget* BaseWidget;
  QTimer* Timer;
  int Alpha;
  int CurrentAlpha;
  int Duration;
  OpenStateType OpenState;
  bool AutoHide;
};

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
  this->Timer = 0;
  this->Alpha = 255;
  this->CurrentAlpha = 0;
  this->Duration = DEFAULT_FADING_DURATION;

  this->OpenState = Closed;
  this->AutoHide = true;
}

// -------------------------------------------------------------------------
void ctkPopupWidgetPrivate::init()
{
  Q_Q(ctkPopupWidget);
  q->setAttribute(Qt::WA_NoSystemBackground);
  q->setAttribute(Qt::WA_OpaquePaintEvent, false);
  q->setAttribute(Qt::WA_TranslucentBackground);
  //q->setAttribute(Qt::WA_PaintOnScreen);
  // Already by default
  //q->setAutoFillBackground(false);
  // Obsolete
  //q->setAttribute(Qt::WA_ContentsPropagated);
  this->Alpha = q->style()->styleHint(QStyle::SH_ToolTipLabel_Opacity);
  this->Timer = new QTimer(q);
  QObject::connect(this->Timer, SIGNAL(timeout()), q, SLOT(animatePopup()));
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
int ctkPopupWidget::opacity()const
{
  Q_D(const ctkPopupWidget);
  return d->Alpha;
}

// -------------------------------------------------------------------------
void ctkPopupWidget::setOpacity(int alpha)
{
  Q_D(ctkPopupWidget);
  d->Alpha = alpha;
  d->CurrentAlpha = d->Alpha;
  this->update();
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
  QTimer::singleShot(ENTER_OPENING_DELAY, this, SLOT(updatePopup()));
}

// -------------------------------------------------------------------------
void ctkPopupWidget::animatePopup()
{
  Q_D(ctkPopupWidget);
  this->repaint();
  if (d->OpenState == ctkPopupWidgetPrivate::Opening &&  d->CurrentAlpha < d->Alpha)
    {
    d->CurrentAlpha += d->Alpha * d->Timer->interval() / d->Duration;
    }
  else if (d->OpenState ==  ctkPopupWidgetPrivate::Closing &&  d->CurrentAlpha > 0)
    {
    d->CurrentAlpha -= d->Alpha * d->Timer->interval() / d->Duration;
    }

  if (d->CurrentAlpha >= d->Alpha)
    {
    d->CurrentAlpha = d->Alpha;
    d->OpenState = ctkPopupWidgetPrivate::Open;
    }

  if (d->CurrentAlpha <= 0)
    {
    d->CurrentAlpha = 0;
    d->OpenState = ctkPopupWidgetPrivate::Closed;
    if (d->Alpha > 0)
      {
      this->hide();
      }
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
      stop.second.setAlpha(d->CurrentAlpha);
      stops.push_back(stop);
      }
    newGradient->setStops(stops);
    brush = QBrush(*newGradient);
    delete newGradient;
    }
  else
    {
    QColor color = brush.color();
    color.setAlpha(d->CurrentAlpha);
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
  if (obj == d->BaseWidget &&
      event->type() == QEvent::Enter)
    {
    QTimer::singleShot(ENTER_OPENING_DELAY, this, SLOT(updatePopup()));
    }
  else if (obj == d->BaseWidget &&
      event->type() == QEvent::Leave)
    {
    QTimer::singleShot(LEAVE_CLOSING_DELAY, this, SLOT(updatePopup()));
    }
  return this->QObject::eventFilter(obj, event);
}

// --------------------------------------------------------------------------
void ctkPopupWidget::updatePopup()
{
  Q_D(ctkPopupWidget);
  if (!d->AutoHide)
    {
    return;
    }
  if (this->underMouse() || (d->BaseWidget && d->BaseWidget->underMouse()))
    {
    this->showPopup();
    }
  else
    {
    this->hidePopup();
    }
}

// --------------------------------------------------------------------------
void ctkPopupWidget::showPopup()
{
  Q_D(ctkPopupWidget);
  if (this->isVisible() && d->OpenState == ctkPopupWidgetPrivate::Open)
    {
    return;
    }
  if (d->BaseWidget)
    {
    QPoint bottomLeft = QPoint(d->BaseWidget->geometry().x(), d->BaseWidget->geometry().bottom());
    QPoint pos = d->BaseWidget->parentWidget() ? d->BaseWidget->parentWidget()->mapToGlobal(bottomLeft) : bottomLeft;
    this->move(pos);
    /// TODO: need some refinement
    if ((this->sizePolicy().horizontalPolicy() & QSizePolicy::GrowFlag &&
         this->width() < d->BaseWidget->width()) ||
        (this->sizePolicy().horizontalPolicy() & QSizePolicy::ShrinkFlag &&
             this->width() > d->BaseWidget->width()))
      {
      // Fit to BaseWidget size
      this->resize(d->BaseWidget->width(), this->sizeHint().height());
      }
    }
  d->OpenState = ctkPopupWidgetPrivate::Opening;
  d->Timer->start(FADING_FPS);
  this->show();
}

// --------------------------------------------------------------------------
void ctkPopupWidget::hidePopup()
{
  Q_D(ctkPopupWidget);

  if (!this->isVisible() || d->OpenState == ctkPopupWidgetPrivate::Closed)
    {
    return;
    }

  d->OpenState = ctkPopupWidgetPrivate::Closing;
  d->Timer->start(FADING_FPS);
  this->update();
}
