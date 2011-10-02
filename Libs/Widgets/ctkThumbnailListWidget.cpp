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
#include "ctkThumbnailLabel.h"
#include "ctkThumbnailListWidget.h"
#include "ctkThumbnailListWidget_p.h"

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

  this->ThumbnailSize = QSize(-1, -1);
  this->CurrentThumbnail = -1;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidgetPrivate::clearAllThumbnails(){
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
      ctkThumbnailLabel* widget = new ctkThumbnailLabel(d->ScrollAreaContentWidget);
      widget->setText("");
      if(d->ThumbnailSize.isValid()){
        widget->setFixedSize(d->ThumbnailSize);
      }
      widget->setPixmap(thumbnails[i]);
      d->ScrollAreaContentWidget->layout()->addWidget(widget);

      this->connect(widget, SIGNAL(selected(ctkThumbnailLabel)), this, SLOT(onThumbnailSelected(ctkThumbnailLabel)));
      this->connect(widget, SIGNAL(selected(ctkThumbnailLabel)), this, SIGNAL(selected(ctkThumbnailLabel)));
      this->connect(widget, SIGNAL(doubleClicked(ctkThumbnailLabel)), this, SIGNAL(doubleClicked(ctkThumbnailLabel)));
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
int ctkThumbnailListWidget::currentThumbnail(){
  Q_D(ctkThumbnailListWidget);

  return d->CurrentThumbnail;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::onThumbnailSelected(const ctkThumbnailLabel &widget){
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
void ctkThumbnailListWidget::setThumbnailSize(QSize size){
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
QSize ctkThumbnailListWidget::thumbnailSize()const{
  Q_D(const ctkThumbnailListWidget);
  return d->ThumbnailSize;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::clearThumbnails(){
  Q_D(ctkThumbnailListWidget);

  d->clearAllThumbnails();
}
