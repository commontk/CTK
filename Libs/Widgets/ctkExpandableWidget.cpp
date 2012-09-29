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
#include <QDebug>
#include <QEvent>
#include <QLayout>

// CTK includes
#include "ctkExpandableWidget.h"
#include "ctkSizeGrip.h"

//-----------------------------------------------------------------------------
class ctkExpandableWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkExpandableWidget);

protected:
  ctkExpandableWidget* const q_ptr;

public:
  ctkExpandableWidgetPrivate(ctkExpandableWidget& object);
  ~ctkExpandableWidgetPrivate();

  void init();
  void positionSizeGrip();
  QSize resizeHint(QSize sizeHint)const;

  ctkSizeGrip* SizeGrip;
  QSize SizeGripMargins;
  bool SizeGripInside;
};

//-----------------------------------------------------------------------------
ctkExpandableWidgetPrivate::ctkExpandableWidgetPrivate(ctkExpandableWidget& object)
  : q_ptr(&object)
  , SizeGrip(0)
  , SizeGripInside(true)
{
}

//-----------------------------------------------------------------------------
ctkExpandableWidgetPrivate::~ctkExpandableWidgetPrivate()
{
  this->SizeGrip = 0; // will be deleted automatically
}

//-----------------------------------------------------------------------------
void ctkExpandableWidgetPrivate::init()
{
  Q_Q(ctkExpandableWidget);

  this->SizeGrip = new ctkSizeGrip(q, q);
  this->SizeGrip->setResizeWidget(false);
  q->connect(this->SizeGrip, SIGNAL(widgetSizeHintChanged(QSize)),
             q, SLOT(updateSizeHint()));

  this->positionSizeGrip();
}

//-----------------------------------------------------------------------------
void ctkExpandableWidgetPrivate::positionSizeGrip()
{
  Q_Q(ctkExpandableWidget);
  if (!this->SizeGrip)
    {
    return;
    }
  if (this->SizeGripInside)
    {
    q->setContentsMargins(0, 0, 0, 0);
    }
  else
    {
    int rightMargin = q->orientations() == Qt::Horizontal ?
      this->SizeGrip->sizeHint().width() : 0;
    int bottomMargin = q->orientations() == Qt::Vertical ?
      this->SizeGrip->sizeHint().height() : 0;
    q->setContentsMargins(0, 0, rightMargin, bottomMargin);
    }
  int w = q->orientations() == Qt::Vertical ? q->width() : this->SizeGrip->sizeHint().width();
  int h = q->orientations() == Qt::Horizontal ? q->height() : this->SizeGrip->sizeHint().height();
  int x = q->width() - w - this->SizeGripMargins.width();
  int y = q->height() - h - this->SizeGripMargins.height();
  this->SizeGrip->setGeometry(x, y, w, h);
}

//-----------------------------------------------------------------------------
QSize ctkExpandableWidgetPrivate::resizeHint(QSize sizeHint)const
{
  Q_Q(const ctkExpandableWidget);
  if (this->SizeGrip->widgetSizeHint().width() >= 0)
    {
    sizeHint.setWidth(this->SizeGrip->widgetSizeHint().width());
    }
  if (this->SizeGrip->widgetSizeHint().height() >= 0)
    {
    sizeHint.setHeight(this->SizeGrip->widgetSizeHint().height());
    }
  QSize minimumSize = this->SizeGrip->sizeHint()
    + this->SizeGripMargins
    + QSize(q->contentsMargins().right(), q->contentsMargins().bottom());
  sizeHint = sizeHint.expandedTo( minimumSize );
  return sizeHint;
}

//-----------------------------------------------------------------------------
ctkExpandableWidget::ctkExpandableWidget(QWidget *parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkExpandableWidgetPrivate(*this))
{
  Q_D(ctkExpandableWidget);
  d->init();
}

//-----------------------------------------------------------------------------
ctkExpandableWidget::~ctkExpandableWidget()
{
}

//------------------------------------------------------------------------------
void ctkExpandableWidget::setOrientations(Qt::Orientations orientations)
{
  Q_D(ctkExpandableWidget);
  d->SizeGrip->setOrientations(orientations);
  d->positionSizeGrip();
  this->updateGeometry();
}

//------------------------------------------------------------------------------
Qt::Orientations ctkExpandableWidget::orientations()const
{
  Q_D(const ctkExpandableWidget);
  return d->SizeGrip->orientations();
}

//------------------------------------------------------------------------------
void ctkExpandableWidget::setSizeGripInside(bool inside)
{
  Q_D(ctkExpandableWidget);
  d->SizeGripInside = inside;
  d->positionSizeGrip();
  this->updateGeometry();
}

//------------------------------------------------------------------------------
bool ctkExpandableWidget::isSizeGripInside()const
{
  Q_D(const ctkExpandableWidget);
  return d->SizeGripInside;
}

//------------------------------------------------------------------------------
void ctkExpandableWidget::setSizeGripMargins(QSize margins)
{
  Q_D(ctkExpandableWidget);
  d->SizeGripMargins = margins;
  d->positionSizeGrip();
  this->updateGeometry();
}

//------------------------------------------------------------------------------
QSize ctkExpandableWidget::sizeGripMargins()const
{
  Q_D(const ctkExpandableWidget);
  return d->SizeGripMargins;
}

//------------------------------------------------------------------------------
QSize ctkExpandableWidget::minimumSizeHint()const
{
  Q_D(const ctkExpandableWidget);
  QSize sizeHint = this->Superclass::minimumSizeHint();
  sizeHint = d->resizeHint(sizeHint);
  return sizeHint;
}

//------------------------------------------------------------------------------
QSize ctkExpandableWidget::sizeHint()const
{
  Q_D(const ctkExpandableWidget);
  QSize sizeHint = this->Superclass::sizeHint();
  sizeHint = d->resizeHint(sizeHint);
  return sizeHint;
}

//------------------------------------------------------------------------------
void ctkExpandableWidget::resizeEvent(QResizeEvent* event)
{
  Q_D(ctkExpandableWidget);
  this->Superclass::resizeEvent(event);
  d->positionSizeGrip();
}

//------------------------------------------------------------------------------
bool ctkExpandableWidget::event(QEvent* event)
{
  Q_D(ctkExpandableWidget);
  bool res = this->Superclass::event(event);
  if (event->type() == QEvent::ChildAdded &&
      d->SizeGrip && d->SizeGripInside)
    {
    d->SizeGrip->raise();
    }
  return res;
}

//------------------------------------------------------------------------------
void ctkExpandableWidget::updateSizeHint()
{
  this->updateGeometry();
}
