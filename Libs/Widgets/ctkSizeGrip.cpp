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

// CTK includes
#include "ctkSizeGrip.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QLayout>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

//------------------------------------------------------------------------------
static inline bool hasHeightForWidth(QWidget *widget)
{
  if (!widget)
    {
    return false;
    }
  if (QLayout *layout = widget->layout())
    {
    return layout->hasHeightForWidth();
    }
  return widget->sizePolicy().hasHeightForWidth();
}

//------------------------------------------------------------------------------
class ctkSizeGripPrivate
{
  Q_DECLARE_PUBLIC(ctkSizeGrip)

protected:
  ctkSizeGrip* const q_ptr;

public:
  ctkSizeGripPrivate(ctkSizeGrip& object);
  void init();

  Qt::Orientations Orientations;

  QWidget* WidgetToResize;

  QRect WidgetGeom;
  QSize WidgetSizeHint;
  QSize WidgetMaxSize;

  QPoint StartPos;
  bool Hover;
  bool Pressed;
  bool Resize;
};

//------------------------------------------------------------------------------
ctkSizeGripPrivate::ctkSizeGripPrivate(ctkSizeGrip& object)
  : q_ptr(&object)
  , Orientations(Qt::Horizontal | Qt::Vertical)
  , WidgetToResize(0)
  , Hover(false)
  , Pressed(false)
  , Resize(false)
{
}

//------------------------------------------------------------------------------
void ctkSizeGripPrivate::init()
{
  Q_Q(ctkSizeGrip);
  q->setOrientations(Qt::Horizontal | Qt::Vertical);
}

//------------------------------------------------------------------------------
ctkSizeGrip::ctkSizeGrip(QWidget* parent)
  : QWidget(parent)
  , d_ptr(new ctkSizeGripPrivate(*this))
{
  Q_D(ctkSizeGrip);
  d->WidgetToResize = parent;
  d->init();
}

//------------------------------------------------------------------------------
ctkSizeGrip::ctkSizeGrip(QWidget* target, QWidget* parent)
  : QWidget(parent)
  , d_ptr(new ctkSizeGripPrivate(*this))
{
  Q_D(ctkSizeGrip);
  d->WidgetToResize = target;
  d->init();
}

//------------------------------------------------------------------------------
ctkSizeGrip::~ctkSizeGrip()
{
}

//------------------------------------------------------------------------------
void ctkSizeGrip::setOrientations(Qt::Orientations newOrientations)
{
  Q_D(ctkSizeGrip);
  d->Orientations = newOrientations;
  QCursor newCursor;
  QSizePolicy newSizePolicy;
  switch(d->Orientations)
    {
    case Qt::Horizontal:
      newCursor = Qt::SizeHorCursor;
      newSizePolicy = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
      break;
    case Qt::Vertical:
      newCursor = Qt::SizeVerCursor;
      newSizePolicy = QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      break;
    default:
      newCursor = this->isLeftToRight() ?
        Qt::SizeFDiagCursor : Qt::SizeBDiagCursor;
      newSizePolicy = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      break;
    };
  this->setCursor(newCursor);
  this->setSizePolicy(newSizePolicy);
  this->updateGeometry(); // sizeHint might change
}

//------------------------------------------------------------------------------
Qt::Orientations ctkSizeGrip::orientations()const
{
  Q_D(const ctkSizeGrip);
  return d->Orientations;
}

//------------------------------------------------------------------------------
void ctkSizeGrip::setWidgetToResize(QWidget* target)
{
  Q_D(ctkSizeGrip);
  d->WidgetToResize = target;
  d->WidgetSizeHint = QSize();
}

//------------------------------------------------------------------------------
QWidget* ctkSizeGrip::widgetToResize()const
{
  Q_D(const ctkSizeGrip);
  return d->WidgetToResize;
}

//------------------------------------------------------------------------------
void ctkSizeGrip::setResizeWidget(bool resize)
{
  Q_D(ctkSizeGrip);
  d->Resize = resize;
  this->setWidgetSizeHint(d->WidgetSizeHint);
}

//------------------------------------------------------------------------------
bool ctkSizeGrip::resizeWidget()const
{
  Q_D(const ctkSizeGrip);
  return d->Resize;
}

//------------------------------------------------------------------------------
void ctkSizeGrip::setWidgetSizeHint(QSize sizeHint)
{
  Q_D(ctkSizeGrip);
  if (d->Resize && d->WidgetToResize)
    {
    QSize newSize = d->WidgetToResize->size();
    if (sizeHint.width())
      {
      newSize.setWidth(sizeHint.width());
      }
    if (sizeHint.height())
      {
      newSize.setHeight(sizeHint.height());
      }
    d->WidgetToResize->resize(newSize);
    }
  if (d->WidgetSizeHint != sizeHint)
    {
    d->WidgetSizeHint = sizeHint;
    emit widgetSizeHintChanged(d->WidgetSizeHint);
    }
}

//------------------------------------------------------------------------------
QSize ctkSizeGrip::widgetSizeHint()const
{
  Q_D(const ctkSizeGrip);
  return d->WidgetSizeHint;
}

//------------------------------------------------------------------------------
QSize ctkSizeGrip::sizeHint() const
{
  Q_D(const ctkSizeGrip);
  QSize minSize;
  QStyle::ContentsType contents;
  switch (d->Orientations)
    {
    case Qt::Horizontal:
    case Qt::Vertical:
      {
      contents = QStyle::CT_Splitter;
      int splitterWidth = this->style()->pixelMetric(QStyle::PM_SplitterWidth, 0, this);
      minSize = QSize(splitterWidth, splitterWidth);
      break;
      }
    default:
      contents = QStyle::CT_SizeGrip;
      minSize = QSize(13, 13);
      break;
    };
  QStyleOption opt(0);
  opt.init(this);
  return (this->style()->sizeFromContents(contents, &opt, minSize, this).
          expandedTo(QApplication::globalStrut()));
}

//------------------------------------------------------------------------------
void ctkSizeGrip::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  Q_D(ctkSizeGrip);
  QPainter painter(this);
  switch (d->Orientations)
    {
    case Qt::Horizontal:
    case Qt::Vertical:
      {
      QStyleOption opt(0);
      opt.rect = rect();
      opt.palette = palette();
      opt.state = (d->Orientations == Qt::Horizontal) ?
        QStyle::State_Horizontal : QStyle::State_None;
      if (d->Hover)
        {
        opt.state |= QStyle::State_MouseOver;
        }
      if (d->Pressed)
        {
        opt.state |= QStyle::State_Sunken;
        }
      if (isEnabled())
        {
        opt.state |= QStyle::State_Enabled;
        }
      this->style()->drawControl(QStyle::CE_Splitter, &opt, &painter, this);
      break;
      }
    default:
      {
      QStyleOptionSizeGrip opt;
      opt.init(this);
      opt.corner = this->isLeftToRight() ? Qt::BottomRightCorner : Qt::BottomLeftCorner;
      style()->drawControl(QStyle::CE_SizeGrip, &opt, &painter, this);
      break;
      }
    };
}

//------------------------------------------------------------------------------
bool ctkSizeGrip::event(QEvent *event)
{
  Q_D(ctkSizeGrip);
  switch(event->type())
    {
    case QEvent::HoverEnter:
      d->Hover = true;
      update();
      break;
    case QEvent::HoverLeave:
      d->Hover = false;
      update();
      break;
    default:
      break;
    }
  return this->Superclass::event(event);
}

//------------------------------------------------------------------------------
void ctkSizeGrip::mousePressEvent(QMouseEvent * e)
{
  if (e->button() != Qt::LeftButton)
    {
    QWidget::mousePressEvent(e);
    return;
    }

  Q_D(ctkSizeGrip);
  d->StartPos = e->globalPos();
  d->Pressed = true;
  d->WidgetGeom = d->WidgetToResize->geometry();
  d->WidgetSizeHint = QSize();
  d->WidgetMaxSize = d->WidgetToResize->maximumSize();
}


//------------------------------------------------------------------------------
void ctkSizeGrip::mouseMoveEvent(QMouseEvent * e)
{
  if (e->buttons() != Qt::LeftButton)
    {
    this->Superclass::mouseMoveEvent(e);
    return;
    }

  Q_D(ctkSizeGrip);
  if (!d->Pressed || d->WidgetToResize->testAttribute(Qt::WA_WState_ConfigPending))
    {
    return;
    }

  QPoint newPos(e->globalPos());
  QSize offset(newPos.x() - d->StartPos.x(), newPos.y() - d->StartPos.y());

  QSize widgetSizeHint = d->WidgetGeom.size();
  if (d->Orientations & Qt::Vertical)
    {
    widgetSizeHint.rheight() = d->WidgetGeom.height() + offset.height();
    }

  if (d->Orientations & Qt::Horizontal)
    {
    widgetSizeHint.rwidth() = d->WidgetGeom.width() + offset.width() * (this->isLeftToRight() ? 1 : -1);
    }
  widgetSizeHint = widgetSizeHint.boundedTo(d->WidgetMaxSize);

  widgetSizeHint = QLayout::closestAcceptableSize(d->WidgetToResize, widgetSizeHint)
    .expandedTo(QApplication::globalStrut());

  this->setWidgetSizeHint(
    QSize(d->Orientations & Qt::Horizontal ? widgetSizeHint.width() : 0,
          d->Orientations & Qt::Vertical ? widgetSizeHint.height() : 0));
}

//------------------------------------------------------------------------------
void ctkSizeGrip::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
  if (mouseEvent->button() != Qt::LeftButton)
    {
    this->Superclass::mouseReleaseEvent(mouseEvent);
    return;
    }
  Q_D(ctkSizeGrip);
  d->Pressed = false;
  d->StartPos = QPoint();
}
