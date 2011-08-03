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

// ctkDICOMWidgets includes
#include "ctkThumbnailWidget.h"
#include "ui_ctkThumbnailWidget.h"

// STD includes
#include <iostream>

// Qt includes
#include <QColor>

// logger includes
#include "ctkLogger.h"
static ctkLogger logger("org.commontk.Widgets.ctkDICOMThumbnailListWidget");

//----------------------------------------------------------------------------
class ctkThumbnailWidgetPrivate: public Ui_ctkThumbnailWidget
{
public:
    ctkThumbnailWidget* const q_ptr;
    Q_DECLARE_PUBLIC(ctkThumbnailWidget);

    // Constructor
    ctkThumbnailWidgetPrivate(ctkThumbnailWidget* parent);

    bool SelectedFlag;
    QColor BackgroundColor;
    QModelIndex SourceIndex;
    QPixmap OriginalThumbnail;

    // Redraw thumbnail
    void updateThumbnail();
};

//----------------------------------------------------------------------------
// ctkThumbnailWidgetPrivate methods

//----------------------------------------------------------------------------
ctkThumbnailWidgetPrivate::ctkThumbnailWidgetPrivate(ctkThumbnailWidget* parent): q_ptr(parent){
    Q_Q(ctkThumbnailWidget);

    this->SelectedFlag = false;
    this->BackgroundColor = q->palette().color(QPalette::Highlight);
}

//----------------------------------------------------------------------------
void ctkThumbnailWidgetPrivate::updateThumbnail(){
    this->PixmapLabel->setPixmap(
        this->OriginalThumbnail.isNull() ? QPixmap() :
            this->OriginalThumbnail.scaledToWidth(this->PixmapLabel->width()));
}

//----------------------------------------------------------------------------
// ctkThumbnailWidget methods

//----------------------------------------------------------------------------
ctkThumbnailWidget::ctkThumbnailWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkThumbnailWidgetPrivate(this))
{
  Q_D(ctkThumbnailWidget);

  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkThumbnailWidget::~ctkThumbnailWidget()
{
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::setText(const QString &text)
{
  Q_D(ctkThumbnailWidget);

  d->TextLabel->setText(text);
}

//----------------------------------------------------------------------------
QString ctkThumbnailWidget::text()const
{
  Q_D(const ctkThumbnailWidget);
  return d->TextLabel->text();
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::setPixmap(const QPixmap &pixmap)
{
  Q_D(ctkThumbnailWidget);

  d->OriginalThumbnail = pixmap;
  d->updateThumbnail();
}

//----------------------------------------------------------------------------
const QPixmap* ctkThumbnailWidget::pixmap()const
{
  Q_D(const ctkThumbnailWidget);

  return d->OriginalThumbnail.isNull() ? 0 : &(d->OriginalThumbnail);
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::setSelected(bool flag){
    Q_D(ctkThumbnailWidget);

    if(flag)
    {
        QPalette p(this->palette());
        p.setColor(QPalette::Window, d->BackgroundColor);
        this->setPalette(p);
        this->setAutoFillBackground(true);
    }
    else
    {
        this->setAutoFillBackground(false);
    }

    d->SelectedFlag = flag;
}

//----------------------------------------------------------------------------
bool ctkThumbnailWidget::isSelected(){
    Q_D(ctkThumbnailWidget);
    return d->SelectedFlag;
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    this->setSelected(true);
    emit selected(*this);
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::mouseDoubleClickEvent(QMouseEvent *event){
    Q_UNUSED(event);
    emit doubleClicked(*this);
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::resizeEvent(QResizeEvent *event){
  Q_D(ctkThumbnailWidget);
  Q_UNUSED(event);

  d->updateThumbnail();
}
