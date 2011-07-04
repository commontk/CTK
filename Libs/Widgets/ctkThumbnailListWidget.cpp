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

// Qt include
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>

// ctk includes
#include "ctkLogger.h"

// ctkWidgets includes
#include "ctkFlowLayout.h"

// ctkDICOMWidgets includes
#include "ctkThumbnailListWidget.h"
#include "ctkThumbnailListWidget_p.h"
#include "ctkThumbnailWidget.h"

// STD includes
#include <iostream>

static ctkLogger logger("org.commontk.Widgets.ctkThumbnailListWidget");

//----------------------------------------------------------------------------
// ctkThumbnailListWidgetPrivate methods

//----------------------------------------------------------------------------
ctkThumbnailListWidgetPrivate::ctkThumbnailListWidgetPrivate(ctkThumbnailListWidget* parent): q_ptr(parent){

}

//----------------------------------------------------------------------------
void ctkThumbnailListWidgetPrivate::init(){
  Q_Q(ctkThumbnailListWidget);

  this->setupUi(q);
  this->ScrollAreaContentWidget->setLayout(new ctkFlowLayout);
  qobject_cast<ctkFlowLayout*>(this->ScrollAreaContentWidget->layout())->setHorizontalSpacing(4);

  this->ThumbnailWidth = -1;
  this->CurrentThumbnail = -1;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidgetPrivate::clearAllThumbnails(){
    Q_Q(ctkThumbnailListWidget);

    // Remove previous displayed thumbnails
    QLayoutItem* item;
    while((item = this->ScrollAreaContentWidget->layout()->takeAt(0)))
    {
        ctkThumbnailWidget* thumbnailWidget = qobject_cast<ctkThumbnailWidget*>(item->widget());
        if(thumbnailWidget)
        {
            q->disconnect(thumbnailWidget, SIGNAL(selected(ctkThumbnailWidget)), q, SLOT(onThumbnailSelected(ctkThumbnailWidget)));
            q->disconnect(thumbnailWidget, SIGNAL(selected(ctkThumbnailWidget)), q, SIGNAL(selected(ctkThumbnailWidget)));
            q->disconnect(thumbnailWidget, SIGNAL(doubleClicked(ctkThumbnailWidget)), q, SIGNAL(doubleClicked(ctkThumbnailWidget)));
        }
        item->widget()->deleteLater();
    }
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

ctkThumbnailListWidget::ctkThumbnailListWidget(ctkThumbnailListWidgetPrivate *_ptr, QWidget *_parent):
  Superclass(_parent),
  d_ptr(_ptr)
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
void ctkThumbnailListWidget::addThumbnails(QList<QPixmap> thumbnails)
{
  Q_D(ctkThumbnailListWidget);
  for(int i=0; i<thumbnails.count(); i++)
    {
      ctkThumbnailWidget* widget = new ctkThumbnailWidget(d->ScrollAreaContentWidget);
      widget->setText("");
      if(d->ThumbnailWidth > 0){
        widget->setFixedWidth(d->ThumbnailWidth);
      }
      widget->setPixmap(thumbnails[i]);
      d->ScrollAreaContentWidget->layout()->addWidget(widget);

      this->connect(widget, SIGNAL(selected(ctkThumbnailWidget)), this, SLOT(onThumbnailSelected(ctkThumbnailWidget)));
      this->connect(widget, SIGNAL(selected(ctkThumbnailWidget)), this, SIGNAL(selected(ctkThumbnailWidget)));
      this->connect(widget, SIGNAL(doubleClicked(ctkThumbnailWidget)), this, SIGNAL(doubleClicked(ctkThumbnailWidget)));
    }
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::setCurrentThumbnail(int index){
  Q_D(ctkThumbnailListWidget);

  int count = d->ScrollAreaContentWidget->layout()->count();

  logger.debug("Select thumbnail " + QVariant(index).toString() + " of " + QVariant(count).toString());

  if(index >= count)return;

  for(int i=0; i<count; i++)
    {
    ctkThumbnailWidget* thumbnailWidget = qobject_cast<ctkThumbnailWidget*>(d->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
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
int ctkThumbnailListWidget::currentThumbnail(){
  Q_D(ctkThumbnailListWidget);

  return d->CurrentThumbnail;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::onThumbnailSelected(const ctkThumbnailWidget &widget){
  Q_D(ctkThumbnailListWidget);
  for(int i=0; i<d->ScrollAreaContentWidget->layout()->count(); i++)
    {
    ctkThumbnailWidget* thumbnailWidget = qobject_cast<ctkThumbnailWidget*>(d->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
    if(thumbnailWidget && (&widget != thumbnailWidget))
      {
      thumbnailWidget->setSelected(false);
      }
    }
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::setThumbnailWidth(int width){
  Q_D(ctkThumbnailListWidget);
  for(int i=0; i<d->ScrollAreaContentWidget->layout()->count(); i++)
    {
    ctkThumbnailWidget* thumbnailWidget = qobject_cast<ctkThumbnailWidget*>(d->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
    if(thumbnailWidget)
      {
        if(width > 0){
          thumbnailWidget->setFixedWidth(width);
        }
      }
    }

  d->ThumbnailWidth = width;
}

//----------------------------------------------------------------------------
int ctkThumbnailListWidget::thumbnailWidth(){
  Q_D(ctkThumbnailListWidget);
  return d->ThumbnailWidth;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::clearThumbnails(){
  Q_D(ctkThumbnailListWidget);

  d->clearAllThumbnails();
}
