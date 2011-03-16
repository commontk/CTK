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

//----------------------------------------------------------------------------
class ctkDICOMThumbnailWidgetPrivate: public Ui_ctkDICOMThumbnailWidget
{
public:
};

//----------------------------------------------------------------------------
// ctkDICOMThumbnailWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMThumbnailWidget methods

//----------------------------------------------------------------------------
ctkDICOMThumbnailWidget::ctkDICOMThumbnailWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMThumbnailWidgetPrivate)
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
void ctkDICOMThumbnailWidget::mousePressEvent(QMouseEvent* event)
{
  Q_UNUSED(event);
  emit selected(*this);
}
