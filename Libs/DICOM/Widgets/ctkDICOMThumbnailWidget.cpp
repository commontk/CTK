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

// ctkDICOMWidgets includes
#include "ctkDICOMThumbnailWidget.h"
#include "ui_ctkDICOMThumbnailWidget.h"

// STD includes
#include <iostream>

// Qt includes
#include <QColor>

//----------------------------------------------------------------------------
class ctkDICOMThumbnailWidgetPrivate: public Ui_ctkDICOMThumbnailWidget
{
public:
    ctkDICOMThumbnailWidget* const q_ptr;
    Q_DECLARE_PUBLIC(ctkDICOMThumbnailWidget);

    // Constructor
    ctkDICOMThumbnailWidgetPrivate(ctkDICOMThumbnailWidget* parent);

    bool selectedFlag;

    QColor backgroundColor;
};

//----------------------------------------------------------------------------
// ctkDICOMThumbnailWidgetPrivate methods

ctkDICOMThumbnailWidgetPrivate::ctkDICOMThumbnailWidgetPrivate(ctkDICOMThumbnailWidget* parent): q_ptr(parent){
    Q_Q(ctkDICOMThumbnailWidget);

    this->selectedFlag = false;
    this->backgroundColor = Qt::blue;

    QPalette p(q->palette());
    p.setColor(QPalette::Window, this->backgroundColor);
    q->setPalette(p);
    q->setBackgroundRole(QPalette::Window);
}

//----------------------------------------------------------------------------
// ctkDICOMThumbnailWidget methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailWidget::ctkDICOMThumbnailWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMThumbnailWidgetPrivate(this))
{
  Q_D(ctkDICOMThumbnailWidget);

  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMThumbnailWidget::~ctkDICOMThumbnailWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailWidget::setText(const QString &text)
{
  Q_D(ctkDICOMThumbnailWidget);

  d->textLabel->setText(text);
}

//----------------------------------------------------------------------------
QString ctkDICOMThumbnailWidget::text()const
{
  Q_D(const ctkDICOMThumbnailWidget);
  return d->textLabel->text();
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailWidget::setPixmap(const QPixmap &pixmap)
{
  Q_D(ctkDICOMThumbnailWidget);

  d->pixmapLabel->setPixmap(pixmap);
}

//----------------------------------------------------------------------------
const QPixmap* ctkDICOMThumbnailWidget::pixmap()const
{
  Q_D(const ctkDICOMThumbnailWidget);
  return d->textLabel->pixmap();
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailWidget::setSelected(bool selected){
    Q_D(ctkDICOMThumbnailWidget);

    if(selected == d->selectedFlag)return;

    if(selected){
        QPalette p(this->palette());
        p.setColor(QPalette::Window, d->backgroundColor);
        this->setPalette(p);
        this->setAutoFillBackground(true);
    }else{
        this->setAutoFillBackground(false);
    }

    d->selectedFlag = selected;
}

//----------------------------------------------------------------------------
bool ctkDICOMThumbnailWidget::isSelected(){
    Q_D(ctkDICOMThumbnailWidget);
    return d->selectedFlag;
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailWidget::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    this->setSelected(true);
    emit selected(*this);
}
