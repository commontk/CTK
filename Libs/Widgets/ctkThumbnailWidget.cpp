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
#include <QApplication>
#include <QColor>
#include <QPainter>

// ctkCore includes
#include "ctkLogger.h"
static ctkLogger logger("org.commontk.Widgets.ctkThumbnailWidget");

// ctkWidgets includes
#include "ctkThumbnailWidget.h"
#include "ui_ctkThumbnailWidget.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
class ctkThumbnailWidgetPrivate: public Ui_ctkThumbnailWidget
{
  Q_DECLARE_PUBLIC(ctkThumbnailWidget);
protected:
  ctkThumbnailWidget* const q_ptr;
public:
  typedef Ui_ctkThumbnailWidget Superclass;

  // Constructor
  ctkThumbnailWidgetPrivate(ctkThumbnailWidget* parent);

  virtual void setupUi(QWidget* widget);

  Qt::Alignment TextPosition;
  bool SelectedFlag;
  QColor SelectedColor;
  QModelIndex SourceIndex;
  QPixmap OriginalThumbnail;
  Qt::TransformationMode TransformationMode;

  // Redraw thumbnail
  void updateThumbnail();
};

//----------------------------------------------------------------------------
// ctkThumbnailWidgetPrivate methods

//----------------------------------------------------------------------------
ctkThumbnailWidgetPrivate::ctkThumbnailWidgetPrivate(ctkThumbnailWidget* parent)
  : q_ptr(parent)
{
  Q_Q(ctkThumbnailWidget);

  this->SelectedFlag = false;
  this->SelectedColor = q->palette().color(QPalette::Highlight);
  this->TextPosition = Qt::AlignTop | Qt::AlignHCenter;
  this->TransformationMode = Qt::FastTransformation;
}

//----------------------------------------------------------------------------
void ctkThumbnailWidgetPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkThumbnailWidget);
  this->Superclass::setupUi(widget);
  q->layout()->setSizeConstraint(QLayout::SetNoConstraint);
  // no text by default
  q->setText(QString());
}

//----------------------------------------------------------------------------
void ctkThumbnailWidgetPrivate::updateThumbnail()
{
  this->PixmapLabel->setPixmap(
    this->OriginalThumbnail.isNull() ? QPixmap() :
      this->OriginalThumbnail.scaled(this->PixmapLabel->size(),
                                     Qt::KeepAspectRatio,
                                     this->TransformationMode));
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
  d->TextLabel->setVisible(!text.isEmpty() &&
    ! (d->TextPosition & Qt::AlignHCenter &&
       d->TextPosition & Qt::AlignVCenter) );
}

//----------------------------------------------------------------------------
QString ctkThumbnailWidget::text()const
{
  Q_D(const ctkThumbnailWidget);
  return d->TextLabel->text();
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::setTextPosition(const Qt::Alignment& position)
{
  Q_D(ctkThumbnailWidget);
  d->TextPosition = position;
  int textIndex = -1;
  for (textIndex = 0; textIndex < this->layout()->count(); ++textIndex)
    {
    if (this->layout()->itemAt(textIndex)->widget() == d->TextLabel)
      {
      break;
      }
    }
  if (textIndex > -1 && textIndex < this->layout()->count())
    {
    this->layout()->takeAt(textIndex);
    }
  int row = 1;
  int col = 1;
  QGridLayout* gridLayout = qobject_cast<QGridLayout*>(this->layout());
  if (position & Qt::AlignTop)
    {
    row = 0;
    }
  else if (position &Qt::AlignBottom)
    {
    row = 2;
    }
  else
    {
    row = 1;
    }
  if (position & Qt::AlignLeft)
    {
    col = 0;
    }
  else if (position & Qt::AlignRight)
    {
    col = 2;
    }
  else
    {
    col = 1;
    }
  if (row == 1 && col == 1)
    {
    d->TextLabel->setVisible(false);
    }
  else
    {
    gridLayout->addWidget(d->TextLabel,row, col);
    }
}

//----------------------------------------------------------------------------
Qt::Alignment ctkThumbnailWidget::textPosition()const
{
  Q_D(const ctkThumbnailWidget);
  return d->TextPosition;
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
Qt::TransformationMode ctkThumbnailWidget::transformationMode()const
{
  Q_D(const ctkThumbnailWidget);
  return d->TransformationMode;
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::setTransformationMode(Qt::TransformationMode mode)
{
  Q_D(ctkThumbnailWidget);
  d->TransformationMode = mode;
  d->updateThumbnail();
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::paintEvent(QPaintEvent* event)
{
  Q_D(ctkThumbnailWidget);
  this->Superclass::paintEvent(event);
  if (d->SelectedFlag && d->SelectedColor.isValid())
    {
    QPainter p(this);
    QPen pen(d->SelectedColor);
    pen.setWidth(7);
    p.setPen(pen);
    p.drawRect(QRect(0,0, this->width() -1, this->height() -1));
    }
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::setSelected(bool flag)
{
  Q_D(ctkThumbnailWidget);
  d->SelectedFlag = flag;
  this->update();
}

//----------------------------------------------------------------------------
bool ctkThumbnailWidget::isSelected()const
{
  Q_D(const ctkThumbnailWidget);
  return d->SelectedFlag;
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::setSelectedColor(const QColor& color)
{
  Q_D(ctkThumbnailWidget);
  d->SelectedColor = color;
  this->update();
}

//----------------------------------------------------------------------------
QColor ctkThumbnailWidget::selectedColor()const
{
  Q_D(const ctkThumbnailWidget);
  return d->SelectedColor;
}

//----------------------------------------------------------------------------
QSize ctkThumbnailWidget::minimumSizeHint()const
{
  Q_D(const ctkThumbnailWidget);
  if (d->TextLabel->isVisibleTo(const_cast<ctkThumbnailWidget*>(this)) &&
      !d->TextLabel->text().isEmpty())
    {
    return d->TextLabel->minimumSizeHint();
    }
  return QSize();
}

//----------------------------------------------------------------------------
QSize ctkThumbnailWidget::sizeHint()const
{
  Q_D(const ctkThumbnailWidget);
  return d->OriginalThumbnail.isNull() ?
    this->Superclass::sizeHint() :
    d->OriginalThumbnail.size().expandedTo(QApplication::globalStrut());
}

//----------------------------------------------------------------------------
int ctkThumbnailWidget::heightForWidth(int width)const
{
  Q_D(const ctkThumbnailWidget);
  if (d->OriginalThumbnail.isNull() ||
      d->OriginalThumbnail.width() == 0)
    {
    return this->Superclass::heightForWidth(width);
    }
  double ratio = static_cast<double>(d->OriginalThumbnail.height()) /
    static_cast<double>(d->OriginalThumbnail.width());
  return static_cast<int>(ratio * width + 0.5);
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::mousePressEvent(QMouseEvent* event)
{
  this->Superclass::mousePressEvent(event);
  this->setSelected(true);
  emit selected(*this);
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
  this->Superclass::mouseDoubleClickEvent(event);
  emit doubleClicked(*this);
}

//----------------------------------------------------------------------------
void ctkThumbnailWidget::resizeEvent(QResizeEvent *event)
{
  Q_D(ctkThumbnailWidget);
  this->Superclass::resizeEvent(event);
  d->updateThumbnail();
}
