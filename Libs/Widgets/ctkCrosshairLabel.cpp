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
#include <QColor>
#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QSize>

// CTK includes
#include "ctkCrosshairLabel.h"
#include "ctkLogger.h"

// STD includes
#include <math.h>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkCrosshairLabel");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkCrosshairLabelPrivate
{
  Q_DECLARE_PUBLIC(ctkCrosshairLabel)
protected:
  ctkCrosshairLabel* const q_ptr;
public:
  ctkCrosshairLabelPrivate(ctkCrosshairLabel& object);

  void init();
  void drawCrosshair();
  void drawSimpleCrosshair(QPainter& painter);
  void drawBullsEyeCrosshair(QPainter& painter);

  static int round(double f)
    { return static_cast<int>( f + ( f >= 0 ? 0.5 : -0.5 ) ); }

  bool      ShowCrosshair;
  QPen      CrosshairPen;
  ctkCrosshairLabel::CrosshairTypes CrosshairType;
  int       BullsEyeWidth;

  static const double BULLS_EYE_BLANK_FRACTION;
};

const double ctkCrosshairLabelPrivate::BULLS_EYE_BLANK_FRACTION = 0.1;

// --------------------------------------------------------------------------
// ctkCrosshairLabelPrivate methods

// --------------------------------------------------------------------------
ctkCrosshairLabelPrivate::ctkCrosshairLabelPrivate(ctkCrosshairLabel& object)
  :q_ptr(&object)
{
  this->ShowCrosshair = true;
  this->CrosshairType = ctkCrosshairLabel::SimpleCrosshair;
  this->BullsEyeWidth = 15;
}

//---------------------------------------------------------------------------
void ctkCrosshairLabelPrivate::init()
{
  Q_Q(ctkCrosshairLabel);
  q->setAutoFillBackground(true);
  q->setAlignment(Qt::AlignCenter);
  this->CrosshairPen.setColor(q->palette().color(QPalette::Highlight));
  this->CrosshairPen.setWidth(0);
  this->CrosshairPen.setJoinStyle(Qt::MiterJoin);
}

//---------------------------------------------------------------------------
void ctkCrosshairLabelPrivate::drawCrosshair()
{
  // Abort if we are not to draw the crosshair
  if (!this->ShowCrosshair)
    {
    return;
    }

  // Setup the painter object to paint on the label
  Q_Q(ctkCrosshairLabel);
  QPainter painter(q);
  painter.setPen(this->CrosshairPen);

  // Draw crosshair (based on current parameters) onto the label
  switch (this->CrosshairType)
    {
    case ctkCrosshairLabel::SimpleCrosshair:
      this->drawSimpleCrosshair(painter);
      break;
    case ctkCrosshairLabel::BullsEyeCrosshair:
      this->drawBullsEyeCrosshair(painter);
      break;
    default:
      qCritical() << "Unsupported crosshair type" << static_cast<int>(this->CrosshairType);
      break;
    }
}

//---------------------------------------------------------------------------
void ctkCrosshairLabelPrivate::drawSimpleCrosshair(QPainter& painter)
{
  Q_Q(ctkCrosshairLabel);
  QSize size = q->size();
  double halfWidth = (size.width()-1.0) / 2.0;
  double halfHeight = (size.height()-1.0) / 2.0;
  painter.drawLine(QPointF(0, halfHeight), QPointF(size.width(), halfHeight));
  painter.drawLine(QPointF(halfWidth, 0), QPointF(halfWidth, size.height()));
}

// --------------------------------------------------------------------------
void ctkCrosshairLabelPrivate::drawBullsEyeCrosshair(QPainter& painter)
{
  Q_Q(ctkCrosshairLabel);
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
  double blank = ctkCrosshairLabelPrivate::round(
        std::min(halfWidth, halfHeight) * this->BULLS_EYE_BLANK_FRACTION);

  painter.drawLine(QPointF(0, halfHeight), QPointF(x-blank-1.0, halfHeight));
  painter.drawLine(QPointF(x+bullsEye+blank, halfHeight),
                   QPointF(size.width(), halfHeight));
  painter.drawLine(QPointF(halfWidth, 0), QPointF(halfWidth, y-blank-1.0));
  painter.drawLine(QPointF(halfWidth, y+bullsEye+blank),
                   QPointF(halfWidth, size.height()));
}

//---------------------------------------------------------------------------
// ctkCrosshairLabel methods

// --------------------------------------------------------------------------
ctkCrosshairLabel::ctkCrosshairLabel(QWidget* parent)
  : Superclass(parent)
  , d_ptr(new ctkCrosshairLabelPrivate(*this))
{
  Q_D(ctkCrosshairLabel);
  d->init();
}

// --------------------------------------------------------------------------
ctkCrosshairLabel::~ctkCrosshairLabel()
{
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkCrosshairLabel, bool, showCrosshair, ShowCrosshair)

// --------------------------------------------------------------------------
void ctkCrosshairLabel::setShowCrosshair(bool newShow)
{
  Q_D(ctkCrosshairLabel);
  if (newShow == d->ShowCrosshair)
    {
    return;
    }

  d->ShowCrosshair = newShow;
  this->update();
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkCrosshairLabel, QPen, crosshairPen, CrosshairPen)

// --------------------------------------------------------------------------
void ctkCrosshairLabel::setCrosshairPen(const QPen& newPen)
{
  Q_D(ctkCrosshairLabel);
  if (newPen == d->CrosshairPen)
    {
    return;
    }

  d->CrosshairPen = newPen;
  this->update();
}

// --------------------------------------------------------------------------
QColor ctkCrosshairLabel::crosshairColor() const
{
  Q_D(const ctkCrosshairLabel);
  return d->CrosshairPen.color();
}

// --------------------------------------------------------------------------
void ctkCrosshairLabel::setCrosshairColor(const QColor& newColor)
{
  Q_D(ctkCrosshairLabel);
  if (d->CrosshairPen.color() == newColor)
    {
    return;
    }

  d->CrosshairPen.setColor(newColor);
  this->update();
}

// --------------------------------------------------------------------------
int ctkCrosshairLabel::lineWidth() const
{
  Q_D(const ctkCrosshairLabel);
  return d->CrosshairPen.width();
}

// --------------------------------------------------------------------------
void ctkCrosshairLabel::setLineWidth(int newWidth)
{
  Q_D(ctkCrosshairLabel);
  if (d->CrosshairPen.width() == newWidth || newWidth < 0)
    {
    return;
    }

  d->CrosshairPen.setWidth(newWidth);
  this->update();
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkCrosshairLabel, ctkCrosshairLabel::CrosshairTypes,
            crosshairType, CrosshairType)

// --------------------------------------------------------------------------
void ctkCrosshairLabel::setCrosshairType(const CrosshairTypes& newType)
{
  Q_D(ctkCrosshairLabel);
  if (newType == d->CrosshairType)
    {
    return;
    }

  d->CrosshairType = newType;
  this->update();
}

// --------------------------------------------------------------------------
QColor ctkCrosshairLabel::marginColor() const
  {
  return this->palette().color(QPalette::Window);
  }

// --------------------------------------------------------------------------
void ctkCrosshairLabel::setMarginColor(const QColor& newColor)
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
CTK_GET_CPP(ctkCrosshairLabel, int, bullsEyeWidth, BullsEyeWidth)

// --------------------------------------------------------------------------
void ctkCrosshairLabel::setBullsEyeWidth(int newWidth)
{
  Q_D(ctkCrosshairLabel);
  if (newWidth == d->BullsEyeWidth || newWidth < 0)
    {
    return;
    }

  d->BullsEyeWidth = newWidth;
  this->update();
}

// --------------------------------------------------------------------------
void ctkCrosshairLabel::paintEvent(QPaintEvent * event)
{
  Superclass::paintEvent(event);
  Q_D(ctkCrosshairLabel);
  d->drawCrosshair();
}

// --------------------------------------------------------------------------
QSize ctkCrosshairLabel::minimumSizeHint()const
{
  // Pretty arbitrary size (matches ctkVTKAbstractView)
  return QSize(50, 50);
}

// --------------------------------------------------------------------------
QSize ctkCrosshairLabel::sizeHint()const
{
  // Pretty arbitrary size (matches ctkVTKAbstractView)
  return QSize(300, 300);
}

//----------------------------------------------------------------------------
bool ctkCrosshairLabel::hasHeightForWidth()const
{
  return true;
}

//----------------------------------------------------------------------------
int ctkCrosshairLabel::heightForWidth(int width)const
{
  // Tends to be square
  return width;
}
