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
#include "ctkThumbnailWidget.h"
#include "ui_ctkThumbnailListWidget.h"

// STD includes
#include <iostream>

static ctkLogger logger("org.commontk.Widgets.ctkThumbnailListWidget");

//----------------------------------------------------------------------------
class ctkThumbnailListWidgetPrivate
{
public:
  ctkThumbnailListWidgetPrivate(ctkThumbnailListWidget* parent);

  void clearAllThumbnails();
 
  ctkThumbnailListWidget* const q_ptr;
  Q_DECLARE_PUBLIC(ctkThumbnailListWidget);
};

//----------------------------------------------------------------------------
// ctkThumbnailListWidgetPrivate methods

//----------------------------------------------------------------------------
ctkThumbnailListWidgetPrivate::ctkThumbnailListWidgetPrivate(ctkThumbnailListWidget* parent): q_ptr(parent){
  
}

void ctkThumbnailListWidgetPrivate::clearAllThumbnails(){
    Q_Q(ctkThumbnailListWidget);

    // Remove previous displayed thumbnails
    QLayoutItem* item;
    while((item = q->ui->ScrollAreaContentWidget->layout()->takeAt(0)))
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
ctkThumbnailListWidget::ctkThumbnailListWidget(QWidget* _parent):Superclass(_parent), 
								 d_ptr(new ctkThumbnailListWidgetPrivate(this)),
								       ui(new Ui_ctkThumbnailListWidget)
{
    Q_D(ctkThumbnailListWidget);

    ui->setupUi(this);

    ui->ScrollAreaContentWidget->setLayout(new ctkFlowLayout);
    qobject_cast<ctkFlowLayout*>(ui->ScrollAreaContentWidget->layout())->setHorizontalSpacing(4);

    this->ThumbnailWidth = -1;
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
      ctkThumbnailWidget* widget = new ctkThumbnailWidget(ui->ScrollAreaContentWidget);
      widget->setText("");
      if(this->ThumbnailWidth > 0){
	widget->setMaximumWidth(this->ThumbnailWidth);
	widget->setMinimumWidth(this->ThumbnailWidth);
      }
      widget->setPixmap(thumbnails[i]);
      ui->ScrollAreaContentWidget->layout()->addWidget(widget);

      this->connect(widget, SIGNAL(selected(ctkThumbnailWidget)), this, SLOT(onThumbnailSelected(ctkThumbnailWidget)));
      this->connect(widget, SIGNAL(selected(ctkThumbnailWidget)), this, SIGNAL(selected(ctkThumbnailWidget)));
      this->connect(widget, SIGNAL(doubleClicked(ctkThumbnailWidget)), this, SIGNAL(doubleClicked(ctkThumbnailWidget)));
    }
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::selectThumbnail(int index){
    Q_D(ctkThumbnailListWidget);

    int count = ui->ScrollAreaContentWidget->layout()->count();

    logger.debug("Select thumbnail " + QVariant(index).toString() + " of " + QVariant(count).toString());

    if(index >= count)return;

    for(int i=0; i<count; i++)
      {
      ctkThumbnailWidget* thumbnailWidget = qobject_cast<ctkThumbnailWidget*>(ui->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
      if(i == index)
        {
        thumbnailWidget->setSelected(true);
        ui->ScrollArea->ensureWidgetVisible(thumbnailWidget);
        }
      else
        {
        thumbnailWidget->setSelected(false);
        }
      }
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::onThumbnailSelected(const ctkThumbnailWidget &widget){
    Q_D(ctkThumbnailListWidget);
    for(int i=0; i<ui->ScrollAreaContentWidget->layout()->count(); i++)
    {
        ctkThumbnailWidget* thumbnailWidget = qobject_cast<ctkThumbnailWidget*>(ui->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
        if(thumbnailWidget && (&widget != thumbnailWidget))
        {
            thumbnailWidget->setSelected(false);
        }
    }
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::setThumbnailWidth(int width){
  Q_D(ctkThumbnailListWidget);
  for(int i=0; i<ui->ScrollAreaContentWidget->layout()->count(); i++)
    {
    ctkThumbnailWidget* thumbnailWidget = qobject_cast<ctkThumbnailWidget*>(ui->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
    if(thumbnailWidget)
      {
        if(width > 0){
          thumbnailWidget->setMaximumWidth(width);
          thumbnailWidget->setMinimumWidth(width);
        }
      }
    }

  this->ThumbnailWidth = width;
}

//----------------------------------------------------------------------------
int ctkThumbnailListWidget::thumbnailWidth(){
  return this->ThumbnailWidth;
}

//----------------------------------------------------------------------------
void ctkThumbnailListWidget::reset(){
  Q_D(ctkThumbnailListWidget);

  d->clearAllThumbnails();
}
