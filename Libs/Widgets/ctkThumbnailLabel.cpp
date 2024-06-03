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
static ctkLogger logger("org.commontk.Widgets.ctkThumbnailLabel");

// ctkWidgets includes
#include "ctkThumbnailLabel.h"
#include "ui_ctkThumbnailLabel.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
class ctkThumbnailLabelPrivate: public Ui_ctkThumbnailLabel
{
  Q_DECLARE_PUBLIC(ctkThumbnailLabel);
protected:
  ctkThumbnailLabel* const q_ptr;
public:
  typedef Ui_ctkThumbnailLabel Superclass;

  // Constructor
  ctkThumbnailLabelPrivate(ctkThumbnailLabel* parent);
  virtual ~ctkThumbnailLabelPrivate();

  virtual void setupUi(QWidget* widget);

  Qt::Alignment TextPosition;
  bool SelectedFlag;
  QColor SelectedColor;
  QModelIndex SourceIndex;
  QPixmap OriginalThumbnail;
  Qt::TransformationMode TransformationMode;
  ctkThumbnailLabel::OperationStatus Status;

  // Redraw thumbnail
  void updateThumbnail();
};

//----------------------------------------------------------------------------
// ctkThumbnailLabelPrivate methods

//----------------------------------------------------------------------------
ctkThumbnailLabelPrivate::ctkThumbnailLabelPrivate(ctkThumbnailLabel* parent)
  : q_ptr(parent)
{
  Q_Q(ctkThumbnailLabel);

  this->SelectedFlag = false;
  this->SelectedColor = q->palette().color(QPalette::Highlight);
  this->TextPosition = Qt::AlignTop | Qt::AlignHCenter;
  this->TransformationMode = Qt::FastTransformation;
  this->Status = ctkThumbnailLabel::OperationStatus::NoOperation;
}

//----------------------------------------------------------------------------
ctkThumbnailLabelPrivate::~ctkThumbnailLabelPrivate()
{
}

//----------------------------------------------------------------------------
void ctkThumbnailLabelPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkThumbnailLabel);
  this->Superclass::setupUi(widget);
  q->layout()->setSizeConstraint(QLayout::SetNoConstraint);
  // no text by default
  q->setText(QString());
  this->OperationProgressBar->hide();

  QObject::connect(this->TextPushButton, SIGNAL(clicked(bool)),
                   q, SIGNAL(statusPushButtonClicked(bool)));
}

//----------------------------------------------------------------------------
void ctkThumbnailLabelPrivate::updateThumbnail()
{
  Q_Q(ctkThumbnailLabel);
  QSize size = q->size();

  if (this->TextPushButton->isVisible())
  {
    if (this->TextPosition & Qt::AlignTop)
    {
      size.setHeight(size.height() - this->TextPushButton->height());
    }
    else if (this->TextPosition & Qt::AlignBottom)
    {
      size.setHeight(size.height() - this->TextPushButton->height());
    }
    else if (this->TextPosition & Qt::AlignLeft)
    {
      size.setWidth(size.width() - this->TextPushButton->width());
    }
    else if (this->TextPosition & Qt::AlignRight)
    {
      size.setWidth(size.width() - this->TextPushButton->width());
    }
  }

  if (this->OperationProgressBar->isVisible())
  {
    size.setHeight(size.height() - this->OperationProgressBar->height());
  }

  if (this->OriginalThumbnail.isNull())
  {
    this->PixmapLabel->setPixmap(QPixmap());
  }
  else
  {
    QPixmap scaledThumbnail = this->OriginalThumbnail.scaled(size * q->devicePixelRatioF(),
                                                             Qt::KeepAspectRatio,
                                                             this->TransformationMode);
    this->PixmapLabel->setPixmap(scaledThumbnail);
  }
}

//----------------------------------------------------------------------------
// ctkThumbnailLabel methods

//----------------------------------------------------------------------------
ctkThumbnailLabel::ctkThumbnailLabel(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkThumbnailLabelPrivate(this))
{
  Q_D(ctkThumbnailLabel);

  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkThumbnailLabel::~ctkThumbnailLabel()
{
}

//----------------------------------------------------------------------------
ctkPushButton* ctkThumbnailLabel::textPushButton()
{
  Q_D(ctkThumbnailLabel);
  return d->TextPushButton;
}

//----------------------------------------------------------------------------
QFrame *ctkThumbnailLabel::pixmapFrame()
{
  Q_D(ctkThumbnailLabel);
  return d->PixmapFrame;
}

//----------------------------------------------------------------------------
QLabel* ctkThumbnailLabel::pixmapLabel()
{
  Q_D(ctkThumbnailLabel);
  return d->PixmapLabel;
}

//----------------------------------------------------------------------------
QProgressBar *ctkThumbnailLabel::operationProgressBar()
{
  Q_D(ctkThumbnailLabel);
  return d->OperationProgressBar;
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::setText(const QString &text)
{
  Q_D(ctkThumbnailLabel);
  d->TextPushButton->setText(text);
  d->TextPushButton->setVisible((!d->TextPushButton->text().isEmpty() || !d->TextPushButton->icon().isNull())  &&
    ! (d->TextPosition & Qt::AlignHCenter && d->TextPosition & Qt::AlignVCenter));
}

//----------------------------------------------------------------------------
QString ctkThumbnailLabel::text()const
{
  Q_D(const ctkThumbnailLabel);
  return d->TextPushButton->text();
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::setTextPosition(const Qt::Alignment& position)
{
  Q_D(ctkThumbnailLabel);
  d->TextPosition = position;
  int textIndex = -1;
  for (textIndex = 0; textIndex < this->layout()->count(); ++textIndex)
  {
    if (this->layout()->itemAt(textIndex)->widget() == d->TextPushButton)
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
    d->TextPushButton->setVisible(false);
  }
  else
  {
    gridLayout->addWidget(d->TextPushButton,row, col);
  }
}

//----------------------------------------------------------------------------
Qt::Alignment ctkThumbnailLabel::textPosition()const
{
  Q_D(const ctkThumbnailLabel);
  return d->TextPosition;
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::setOperationStatus(const OperationStatus &status)
{
  Q_D(ctkThumbnailLabel);
  d->Status = status;
}

//----------------------------------------------------------------------------
ctkThumbnailLabel::OperationStatus ctkThumbnailLabel::operationStatus() const
{
  Q_D(const ctkThumbnailLabel);
  return d->Status;
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::setStatusIcon(const QIcon &icon)
{
  Q_D(ctkThumbnailLabel);
  d->TextPushButton->setIcon(icon);
  d->TextPushButton->setVisible((!d->TextPushButton->text().isEmpty() || !d->TextPushButton->icon().isNull())  &&
    ! (d->TextPosition & Qt::AlignHCenter && d->TextPosition & Qt::AlignVCenter));
}

//----------------------------------------------------------------------------
QIcon ctkThumbnailLabel::statusIcon() const
{
  Q_D(const ctkThumbnailLabel);
  return d->TextPushButton->icon();
}

//----------------------------------------------------------------------------
int ctkThumbnailLabel::operationProgress() const
{
  Q_D(const ctkThumbnailLabel);
  return d->OperationProgressBar->value();
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::setOperationProgress(const int &progress)
{
  Q_D(ctkThumbnailLabel);
  d->OperationProgressBar->setValue(progress);
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::setPixmap(const QPixmap &pixmap)
{
  Q_D(ctkThumbnailLabel);

  d->OriginalThumbnail = pixmap;
  d->updateThumbnail();
}

//----------------------------------------------------------------------------
const QPixmap* ctkThumbnailLabel::pixmap()const
{
  Q_D(const ctkThumbnailLabel);

  return d->OriginalThumbnail.isNull() ? 0 : &(d->OriginalThumbnail);
}

//----------------------------------------------------------------------------
Qt::TransformationMode ctkThumbnailLabel::transformationMode()const
{
  Q_D(const ctkThumbnailLabel);
  return d->TransformationMode;
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::setTransformationMode(Qt::TransformationMode mode)
{
  Q_D(ctkThumbnailLabel);
  d->TransformationMode = mode;
  d->updateThumbnail();
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::paintEvent(QPaintEvent* event)
{
  Q_D(ctkThumbnailLabel);
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
void ctkThumbnailLabel::setSelected(bool flag)
{
  Q_D(ctkThumbnailLabel);
  d->SelectedFlag = flag;
  this->update();
}

//----------------------------------------------------------------------------
bool ctkThumbnailLabel::isSelected()const
{
  Q_D(const ctkThumbnailLabel);
  return d->SelectedFlag;
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::setSelectedColor(const QColor& color)
{
  Q_D(ctkThumbnailLabel);
  d->SelectedColor = color;
  this->update();
}

//----------------------------------------------------------------------------
QColor ctkThumbnailLabel::selectedColor()const
{
  Q_D(const ctkThumbnailLabel);
  return d->SelectedColor;
}

//----------------------------------------------------------------------------
QSize ctkThumbnailLabel::minimumSizeHint()const
{
  Q_D(const ctkThumbnailLabel);
  if (d->TextPushButton->isVisibleTo(const_cast<ctkThumbnailLabel*>(this)) &&
      !d->TextPushButton->text().isEmpty())
  {
    return d->TextPushButton->minimumSizeHint();
  }
  return QSize();
}

//----------------------------------------------------------------------------
QSize ctkThumbnailLabel::sizeHint()const
{
  Q_D(const ctkThumbnailLabel);
  return d->OriginalThumbnail.isNull() ?
    this->Superclass::sizeHint() :
    d->OriginalThumbnail.size().expandedTo(QApplication::globalStrut());
}

//----------------------------------------------------------------------------
int ctkThumbnailLabel::heightForWidth(int width)const
{
  Q_D(const ctkThumbnailLabel);
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
void ctkThumbnailLabel::mousePressEvent(QMouseEvent* event)
{
  this->Superclass::mousePressEvent(event);
  this->setSelected(true);
  emit selected(*this);
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
  this->Superclass::mouseDoubleClickEvent(event);
  emit doubleClicked(*this);
}

//----------------------------------------------------------------------------
void ctkThumbnailLabel::resizeEvent(QResizeEvent *event)
{
  Q_D(ctkThumbnailLabel);
  this->Superclass::resizeEvent(event);
  d->updateThumbnail();
}
