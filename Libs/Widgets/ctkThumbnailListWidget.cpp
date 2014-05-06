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

// Qt include
#include <QDateTime>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>

// ctk includes
#include "ctkLogger.h"

// ctkWidgets includes
#include "ctkFlowLayout.h"

// ctkDICOMWidgets includes
#include "ctkThumbnailLabel.h"
#include "ctkThumbnailListWidget.h"
#include "ctkThumbnailListWidget_p.h"

// STD includes
#include <iostream>

static ctkLogger logger("org.commontk.Widgets.ctkThumbnailListWidget");

//----------------------------------------------------------------------------
// ctkThumbnailListWidgetPrivate methods

//----------------------------------------------------------------------------
ctkThumbnailListWidgetPrivate
::ctkThumbnailListWidgetPrivate(ctkThumbnailListWidget* parent)
  : q_ptr(parent)
  , CurrentThumbnail(-1)
  , ThumbnailSize(-1, -1)
  , RequestRelayout(false)
{
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidgetPrivate::init()
{
  Q_Q(ctkThumbnailListWidget);

  this->setupUi(q);
  ctkFlowLayout* flowLayout = new ctkFlowLayout;
  //flowLayout->setOrientation(Qt::Vertical);
  //flowLayout->setPreferredExpandingDirections(Qt::Horizontal);
  flowLayout->setHorizontalSpacing(4);
  this->ScrollAreaContentWidget->setLayout(flowLayout);
  this->ScrollArea->installEventFilter(q);
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidgetPrivate::clearAllThumbnails()
{
  Q_Q(ctkThumbnailListWidget);

  // Remove previous displayed thumbnails
  QLayoutItem* item;
  while((item = this->ScrollAreaContentWidget->layout()->takeAt(0)))
    {
    ctkThumbnailLabel* thumbnailWidget = qobject_cast<ctkThumbnailLabel*>(item->widget());
    if(thumbnailWidget)
      {
      q->disconnect(thumbnailWidget, SIGNAL(selected(ctkThumbnailLabel)), q, SLOT(onThumbnailSelected(ctkThumbnailLabel)));
      q->disconnect(thumbnailWidget, SIGNAL(selected(ctkThumbnailLabel)), q, SIGNAL(selected(ctkThumbnailLabel)));
      q->disconnect(thumbnailWidget, SIGNAL(doubleClicked(ctkThumbnailLabel)), q, SIGNAL(doubleClicked(ctkThumbnailLabel)));
      }
    item->widget()->deleteLater();
    }
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidgetPrivate::addThumbnail(ctkThumbnailLabel* thumbnail)
{
  Q_Q(ctkThumbnailListWidget);

  this->ScrollAreaContentWidget->layout()->addWidget(thumbnail);
  thumbnail->installEventFilter(q);
  this->RequestRelayout = true;

  q->connect(thumbnail, SIGNAL(selected(ctkThumbnailLabel)),
    q, SLOT(onThumbnailSelected(ctkThumbnailLabel)));
  q->connect(thumbnail, SIGNAL(selected(ctkThumbnailLabel)),
    q, SIGNAL(selected(ctkThumbnailLabel)));
  q->connect(thumbnail, SIGNAL(doubleClicked(ctkThumbnailLabel)),
    q, SIGNAL(doubleClicked(ctkThumbnailLabel)));
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidgetPrivate::updateScrollAreaContentWidgetSize(QSize size)
{
  QSize newViewportSize = size - QSize(2 * this->ScrollArea->lineWidth(),
                                       2 * this->ScrollArea->lineWidth());

  ctkFlowLayout* flowLayout = qobject_cast<ctkFlowLayout*>(
    this->ScrollAreaContentWidget->layout());
  newViewportSize = newViewportSize.expandedTo(flowLayout->minimumSize());
  if (flowLayout->hasHeightForWidth())
    {
    int newViewPortHeight = newViewportSize.height();
    newViewportSize.rheight() = flowLayout->heightForWidth( newViewportSize.width() );
    if (newViewportSize.height() > newViewPortHeight)
      {
      // The new width is too narrow, to fit everything, a vertical scrollbar
      // is needed. Recompute with the scrollbar width.
      newViewportSize.rwidth() -= this->ScrollArea->verticalScrollBar()->sizeHint().width();
      newViewportSize.rheight() = flowLayout->heightForWidth( newViewportSize.width() );
      }
    }
  else if (flowLayout->hasWidthForHeight())
    {
    int newViewPortWidth = newViewportSize.width();
    newViewportSize.rwidth() = flowLayout->widthForHeight( newViewportSize.height() );
    if (newViewportSize.width() > newViewPortWidth)
      {
      // The new height is too narrow, to fit everything, an horizontal scrollbar
      // is needed. Recompute with the scrollbar height.
      newViewportSize.rheight() -= this->ScrollArea->horizontalScrollBar()->sizeHint().height();
      newViewportSize.rwidth() = flowLayout->widthForHeight( newViewportSize.height() );
      }
    }
  this->ScrollAreaContentWidget->resize(newViewportSize);
}

//----------------------------------------------------------------------------
// ctkThumbnailListWidget methods

//----------------------------------------------------------------------------
ctkThumbnailListWidget::ctkThumbnailListWidget(QWidget* _parent):
  Superclass(_parent),
  d_ptr(new ctkThumbnailListWidgetPrivate(this))
{
  Q_D(ctkThumbnailListWidget);

  d->init();
}

//----------------------------------------------------------------------------
ctkThumbnailListWidget
::ctkThumbnailListWidget(ctkThumbnailListWidgetPrivate *_ptr, QWidget *_parent)
  : Superclass(_parent)
  , d_ptr(_ptr)
{
  Q_D(ctkThumbnailListWidget);

  d->init();
}

//----------------------------------------------------------------------------
ctkThumbnailListWidget::~ctkThumbnailListWidget()
{
  Q_D(ctkThumbnailListWidget);

  d->clearAllThumbnails();
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::addThumbnails(const QList<QPixmap>& thumbnails)
{
  for(int i=0; i<thumbnails.count(); i++)
    {
    this->addThumbnail(thumbnails[i]);
    }
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::addThumbnail(const QPixmap& pixmap, const QString& label)
{
  Q_D(ctkThumbnailListWidget);
  ctkThumbnailLabel* widget = new ctkThumbnailLabel(d->ScrollAreaContentWidget);
  widget->setText(label);
  if(d->ThumbnailSize.isValid())
    {
    widget->setFixedSize(d->ThumbnailSize);
    }
  widget->setPixmap(pixmap);
  d->addThumbnail(widget);
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::setCurrentThumbnail(int index)
{
  Q_D(ctkThumbnailListWidget);

  int count = d->ScrollAreaContentWidget->layout()->count();

  logger.debug("Select thumbnail " + QVariant(index).toString() + " of " + QVariant(count).toString());

  if(index >= count)return;

  for(int i=0; i<count; i++)
    {
    ctkThumbnailLabel* thumbnailWidget = qobject_cast<ctkThumbnailLabel*>(d->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
    if(i == index)
      {
      thumbnailWidget->setSelected(true);
      d->ScrollArea->ensureWidgetVisible(thumbnailWidget);
      }
    else
      {
      thumbnailWidget->setSelected(false);
      }
    }

  d->CurrentThumbnail = index;
}

//----------------------------------------------------------------------------
int ctkThumbnailListWidget::currentThumbnail()
{
  Q_D(ctkThumbnailListWidget);

  return d->CurrentThumbnail;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::onThumbnailSelected(const ctkThumbnailLabel &widget)
{
  Q_D(ctkThumbnailListWidget);
  for(int i=0; i<d->ScrollAreaContentWidget->layout()->count(); i++)
    {
    ctkThumbnailLabel* thumbnailWidget = qobject_cast<ctkThumbnailLabel*>(d->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
    if(thumbnailWidget && (&widget != thumbnailWidget))
      {
      thumbnailWidget->setSelected(false);
      }
    }
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::setFlow(Qt::Orientation flow)
{
  Q_D(ctkThumbnailListWidget);
  ctkFlowLayout* flowLayout = qobject_cast<ctkFlowLayout*>(
    d->ScrollAreaContentWidget->layout());
  flowLayout->setOrientation(flow);
}

//----------------------------------------------------------------------------
Qt::Orientation ctkThumbnailListWidget::flow()const
{
  Q_D(const ctkThumbnailListWidget);
  ctkFlowLayout* flowLayout = qobject_cast<ctkFlowLayout*>(
    d->ScrollAreaContentWidget->layout());
  return flowLayout->orientation();
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::setThumbnailSize(QSize size)
{
  Q_D(ctkThumbnailListWidget);
  if (size.isValid())
    {
    foreach( ctkThumbnailLabel* thumbnail,
             this->findChildren<ctkThumbnailLabel*>())
      {
      thumbnail->setFixedSize(size);
      }
    }
  d->ThumbnailSize = size;
}

//----------------------------------------------------------------------------
QSize ctkThumbnailListWidget::thumbnailSize()const
{
  Q_D(const ctkThumbnailListWidget);
  return d->ThumbnailSize;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::clearThumbnails()
{
  Q_D(ctkThumbnailListWidget);
  d->clearAllThumbnails();
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::resizeEvent(QResizeEvent* event)
{
  Q_D(ctkThumbnailListWidget);
  d->updateScrollAreaContentWidgetSize(event->size());
}

//----------------------------------------------------------------------------
bool ctkThumbnailListWidget::eventFilter(QObject* watched, QEvent* event)
{
  Q_D(ctkThumbnailListWidget);
  if (d->RequestRelayout &&
      qobject_cast<ctkThumbnailLabel*>(watched) &&
      event->type() == QEvent::Show)
    {
    d->RequestRelayout = false;
    watched->removeEventFilter(this);
    QTimer::singleShot(0, this, SLOT(updateLayout()));
    }
  return this->Superclass::eventFilter(watched, event);
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::updateLayout()
{
  Q_D(ctkThumbnailListWidget);
  d->updateScrollAreaContentWidgetSize(this->size());
}
