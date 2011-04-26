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

// Qt includes
#include <QColor>
#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QSize>

// CTK includes
#include "ctkCursorPixmapWidget.h"
#include "ctkLogger.h"

// STD includes
#include <math.h>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkCursorPixmapWidget");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkCursorPixmapWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkCursorPixmapWidget);
protected:
  ctkCursorPixmapWidget* const q_ptr;
public:
  ctkCursorPixmapWidgetPrivate(ctkCursorPixmapWidget& object);

  void init();
  void drawCursor();
  void drawCrossHairCursor(QPainter& painter);
  void drawBullsEyeCursor(QPainter& painter);

  bool      ShowCursor;
  QPen      CursorPen;
  ctkCursorPixmapWidget::CursorTypes CursorType;
  int       BullsEyeWidth;

  static const double BULLS_EYE_BLANK_FRACTION = 0.1;
};

// --------------------------------------------------------------------------
// ctkCursorPixmapWidgetPrivate methods

// --------------------------------------------------------------------------
ctkCursorPixmapWidgetPrivate::ctkCursorPixmapWidgetPrivate(ctkCursorPixmapWidget& object)
  :q_ptr(&object)
{
  this->ShowCursor = true;
  this->CursorType = ctkCursorPixmapWidget::CrossHairCursor;
  this->BullsEyeWidth = 15;
}

//---------------------------------------------------------------------------
void ctkCursorPixmapWidgetPrivate::init()
{
  Q_Q(ctkCursorPixmapWidget);
  q->setAutoFillBackground(true);
  q->setAlignment(Qt::AlignCenter);
  this->CursorPen.setColor(q->palette().color(QPalette::Highlight));
  this->CursorPen.setWidth(0);
  this->CursorPen.setJoinStyle(Qt::MiterJoin);
}

//---------------------------------------------------------------------------
void ctkCursorPixmapWidgetPrivate::drawCursor()
{
  // Abort if we are not to draw the cursor
  if (!this->ShowCursor)
    {
    return;
    }

  // Setup the painter object to paint on the label
  Q_Q(ctkCursorPixmapWidget);
  QPainter painter(q);
  painter.setPen(this->CursorPen);

  // Draw cursor (based on current parameters) onto the label
  switch (this->CursorType)
    {
    case ctkCursorPixmapWidget::CrossHairCursor:
      this->drawCrossHairCursor(painter);
      break;
    case ctkCursorPixmapWidget::BullsEyeCursor:
      this->drawBullsEyeCursor(painter);
      break;
    default:
      qDebug() << "Unsupported cursor type" << this->CursorType;
      break;
    }
}

//---------------------------------------------------------------------------
void ctkCursorPixmapWidgetPrivate::drawCrossHairCursor(QPainter& painter)
{
  Q_Q(ctkCursorPixmapWidget);
  QSize size = q->size();
  double halfWidth = (size.width()-1.0) / 2.0;
  double halfHeight = (size.height()-1.0) / 2.0;
  painter.drawLine(QPointF(0, halfHeight), QPointF(size.width(), halfHeight));
  painter.drawLine(QPointF(halfWidth, 0), QPointF(halfWidth, size.height()));
}

// --------------------------------------------------------------------------
void ctkCursorPixmapWidgetPrivate::drawBullsEyeCursor(QPainter& painter)
{
  Q_Q(ctkCursorPixmapWidget);
  QSize size = q->size();

  // Draw rectangle
  double bullsEye = this->BullsEyeWidth;
  double lineWidth = painter.pen().width();
  lineWidth = std::max(lineWidth, 1.0);
  double halfLineWidth = (lineWidth-1.0) / 2.0;
  double x = (size.width()-bullsEye) / 2.0;
  double y = (size.height()-bullsEye) / 2.0;
  double rectWidth = bullsEye;
  if (bullsEye != 1)
    {
    rectWidth = rectWidth - lineWidth;
    }
  rectWidth = std::max(rectWidth, 0.0);
  painter.drawRect(
      QRectF(x+halfLineWidth, y+halfLineWidth, rectWidth, rectWidth));

  // Draw the lines
  double halfWidth = (size.width()-1.0) / 2.0;
  double halfHeight = (size.height()-1.0) / 2.0;
  double blank = round(std::min(halfWidth, halfHeight) * this->BULLS_EYE_BLANK_FRACTION);

  painter.drawLine(QPointF(0, halfHeight), QPointF(x-blank-1.0, halfHeight));
  painter.drawLine(QPointF(x+bullsEye+blank, halfHeight), QPointF(size.width(), halfHeight));
  painter.drawLine(QPointF(halfWidth, 0), QPointF(halfWidth, y-blank-1.0));
  painter.drawLine(QPointF(halfWidth, y+bullsEye+blank), QPointF(halfWidth, size.height()));
}

//---------------------------------------------------------------------------
// ctkCursorPixmapWidget methods

// --------------------------------------------------------------------------
ctkCursorPixmapWidget::ctkCursorPixmapWidget(QWidget* parent)
  : Superclass(parent)
  , d_ptr(new ctkCursorPixmapWidgetPrivate(*this))
{
  Q_D(ctkCursorPixmapWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkCursorPixmapWidget::~ctkCursorPixmapWidget()
{
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkCursorPixmapWidget, bool, showCursor, ShowCursor);

// --------------------------------------------------------------------------
void ctkCursorPixmapWidget::setShowCursor(bool newShow)
{
  Q_D(ctkCursorPixmapWidget);
  if (newShow == d->ShowCursor)
    {
    return;
    }

  d->ShowCursor = newShow;
  this->update();
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkCursorPixmapWidget, QPen, cursorPen, CursorPen);

// --------------------------------------------------------------------------
void ctkCursorPixmapWidget::setCursorPen(const QPen& newPen)
{
  Q_D(ctkCursorPixmapWidget);
  if (newPen == d->CursorPen)
    {
    return;
    }

  d->CursorPen = newPen;
  this->update();
}

// --------------------------------------------------------------------------
QColor ctkCursorPixmapWidget::cursorColor() const
{
  Q_D(const ctkCursorPixmapWidget);
  return d->CursorPen.color();
}

// --------------------------------------------------------------------------
void ctkCursorPixmapWidget::setCursorColor(const QColor& newColor)
{
  Q_D(ctkCursorPixmapWidget);
  if (d->CursorPen.color() == newColor)
    {
    return;
    }

  d->CursorPen.setColor(newColor);
  this->update();
}

// --------------------------------------------------------------------------
int ctkCursorPixmapWidget::lineWidth() const
{
  Q_D(const ctkCursorPixmapWidget);
  return d->CursorPen.width();
}

// --------------------------------------------------------------------------
void ctkCursorPixmapWidget::setLineWidth(int newWidth)
{
  Q_D(ctkCursorPixmapWidget);
  if (d->CursorPen.width() == newWidth || newWidth < 0)
    {
    return;
    }

  d->CursorPen.setWidth(newWidth);
  this->update();
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkCursorPixmapWidget, ctkCursorPixmapWidget::CursorTypes, cursorType, CursorType);

// --------------------------------------------------------------------------
void ctkCursorPixmapWidget::setCursorType(const CursorTypes& newType)
{
  Q_D(ctkCursorPixmapWidget);
  if (newType == d->CursorType)
    {
    return;
    }

  d->CursorType = newType;
  this->update();
}

// --------------------------------------------------------------------------
QColor ctkCursorPixmapWidget::marginColor() const
  {
  return this->palette().color(QPalette::Window);
  }

// --------------------------------------------------------------------------
void ctkCursorPixmapWidget::setMarginColor(const QColor& newColor)
{
  if (!newColor.isValid())
    {
    return;
    }

  QPalette palette(this->palette());
  QColor solidColor(newColor.rgb());
  if (solidColor != palette.color(QPalette::Window))
    {
    // Ignore alpha channel
    palette.setColor(QPalette::Window, solidColor);
    this->setPalette(palette);
    this->update();
    }
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkCursorPixmapWidget, int, bullsEyeWidth, BullsEyeWidth);

// --------------------------------------------------------------------------
void ctkCursorPixmapWidget::setBullsEyeWidth(int newWidth)
{
  Q_D(ctkCursorPixmapWidget);
  if (newWidth == d->BullsEyeWidth || newWidth < 0)
    {
    return;
    }

  d->BullsEyeWidth = newWidth;
  this->update();
}

// --------------------------------------------------------------------------
void ctkCursorPixmapWidget::paintEvent(QPaintEvent * event)
{
  Superclass::paintEvent(event);
  Q_D(ctkCursorPixmapWidget);
  d->drawCursor();
}

// --------------------------------------------------------------------------
QSize ctkCursorPixmapWidget::minimumSizeHint()const
{
  // Pretty arbitrary size (matches ctkVTKAbstractView)
  return QSize(50, 50);
}

// --------------------------------------------------------------------------
QSize ctkCursorPixmapWidget::sizeHint()const
{
  // Pretty arbitrary size (matches ctkVTKAbstractView)
  return QSize(300, 300);
}

//----------------------------------------------------------------------------
bool ctkCursorPixmapWidget::hasHeightForWidth()const
{
  return true;
}

//----------------------------------------------------------------------------
int ctkCursorPixmapWidget::heightForWidth(int width)const
{
  // Tends to be square
  return width;
}
