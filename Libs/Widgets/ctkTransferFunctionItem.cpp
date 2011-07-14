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

/// Qt includes
#include <QColor>
#include <QDebug>
#include <QLinearGradient>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QtGlobal>
#include <QVariant>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionItem.h"
#include "ctkTransferFunctionScene.h"

//-----------------------------------------------------------------------------
class ctkTransferFunctionItemPrivate
{
public:
  ctkTransferFunctionItemPrivate();
  void init();
  QRectF               Rect;
  ctkTransferFunction* TransferFunction;
};

//-----------------------------------------------------------------------------
ctkTransferFunctionItemPrivate::ctkTransferFunctionItemPrivate()
{
  this->TransferFunction = 0;
  this->Rect = QRectF(0.,0.,1.,1.);
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionItemPrivate::init()
{
  //Q_Q(ctkTransferFunctionItem);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionItem::ctkTransferFunctionItem(QGraphicsItem* parentGraphicsItem)
  :QGraphicsObject(parentGraphicsItem)
  , d_ptr(new ctkTransferFunctionItemPrivate)
{
  Q_D(ctkTransferFunctionItem);
  d->init();
}

//-----------------------------------------------------------------------------
ctkTransferFunctionItem::ctkTransferFunctionItem(
  ctkTransferFunction* transferFunction, QGraphicsItem* parentItem)
  :QGraphicsObject(parentItem)
  , d_ptr(new ctkTransferFunctionItemPrivate)
{
  Q_D(ctkTransferFunctionItem);
  d->init();
  this->setTransferFunction(transferFunction);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionItem::~ctkTransferFunctionItem()
{
  
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionItem::setTransferFunction(ctkTransferFunction* transferFunction)
{
  Q_D(ctkTransferFunctionItem);
  d->TransferFunction = transferFunction;
  if (this->scene())
    {
    connect(d->TransferFunction,SIGNAL(changed()),
            this->scene(), SLOT(update()),Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
ctkTransferFunction* ctkTransferFunctionItem::transferFunction() const
{
  Q_D(const ctkTransferFunctionItem);
  return d->TransferFunction;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionItem::setRect(const QRectF& newRect)
{
  Q_D(ctkTransferFunctionItem);
  if (d->Rect == newRect)
    {
    return;
    }
  d->Rect = newRect;
  this->update();
}

//-----------------------------------------------------------------------------
QRectF ctkTransferFunctionItem::rect() const
{
  Q_D(const ctkTransferFunctionItem);
  return d->Rect;
}

//-----------------------------------------------------------------------------
QRectF ctkTransferFunctionItem::boundingRect()const
{
  Q_D(const ctkTransferFunctionItem);
  return d->Rect;
}
/*
//-----------------------------------------------------------------------------
QList<ctkPoint> ctkTransferFunctionItem::bezierParams(
  ctkControlPoint* start, ctkControlPoint* end) const
{
  Q_ASSERT(start);
  Q_ASSERT(end);
  QList<ctkPoint> points; 
  
  ctkBezierControlPoint* bezierCP = dynamic_cast<ctkBezierControlPoint*>(start);
  if (!bezierCP)
    {// just duplicate start and end into p1 and p2
    points << start->P;
    points << start->P;
    points << end->P;
    points << end->P;
    return points;
    }
  
  points << start->P;
  points << bezierCP->P1;
  points << bezierCP->P2;
  points << end->P;
  return points;
}

//-----------------------------------------------------------------------------
QList<ctkPoint> ctkTransferFunctionItem::nonLinearPoints(
  ctkControlPoint* start, ctkControlPoint* end) const
{
  Q_ASSERT(start);
    
  ctkNonLinearControlPoint* nonLinearCP = 
    dynamic_cast<ctkNonLinearControlPoint*>(start);
  if (!nonLinearCP)
    {
    QList<ctkPoint> points; 
    points << start->P;
    points << end->P;
    return points;
    }
  return nonLinearCP->SubPoints;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionItem::y(const QVariant& v) const
{ 
  Q_ASSERT(v.canConvert<qreal>() || v.canConvert<QColor>());
  if (v.canConvert<QColor>())
    {
    return v.value<QColor>().alphaF();
    }
  return v.toReal();
}

//-----------------------------------------------------------------------------
QColor ctkTransferFunctionItem::color(const QVariant& v) const
{ 
  //Q_ASSERT(v.canConvert<QColor>());
  if (v.canConvert<QColor>())
    {
    return v.value<QColor>();
    }
  else
    {
    //black background
    QColor defaultColor(0., 0., 0.);
    return defaultColor;
    }
  return QColor();
}
//-----------------------------------------------------------------------------
qreal ctkTransferFunctionItem::rangeXDiff()
{
  // pointer to private class
  Q_D(ctkTransferFunctionItem);

  qreal rangeX[2];
  d->TransferFunction->range(rangeX);
  return this->rect().width() / (rangeX[1] - rangeX[0]);
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionItem::rangeXOffSet()
{
  // pointer to private class
  Q_D(ctkTransferFunctionItem);

  qreal rangeX[2];
  d->TransferFunction->range(rangeX);
  return rangeX[0];
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionItem::rangeYDiff()
{
  // pointer to private class
  Q_D(ctkTransferFunctionItem);

  QVariant rangeY[2];
  rangeY[0] = d->TransferFunction->minValue();
  rangeY[1] = d->TransferFunction->maxValue();
  qreal rangeYDiff = this->rect().height();
  if (rangeY[0].canConvert<qreal>())
    {
    if (rangeY[1].toReal() == rangeY[0].toReal())
      {
      rangeYDiff /= rangeY[0].toReal();
      return rangeYDiff;
      }
    else
      {
      rangeYDiff /= rangeY[1].toReal() - rangeY[0].toReal();
      return rangeYDiff;
      }
    }
    else if (rangeY[0].canConvert<QColor>())
      {
      if ( rangeY[1].value<QColor>().alphaF() == rangeY[0].value<QColor>().alphaF())
        {
        rangeYDiff /= rangeY[0].value<QColor>().alphaF();
        return rangeYDiff;
        }
      else
        {
        rangeYDiff /= rangeY[1].value<QColor>().alphaF() - rangeY[0].value<QColor>().alphaF();
        return rangeYDiff;
        }
      }
    else
      {
      Q_ASSERT(rangeY[0].canConvert<qreal>() ||
      rangeY[0].canConvert<QColor>());
      }
  return rangeYDiff;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionItem::rangeYOffSet()
{
  // pointer to private class
  Q_D(ctkTransferFunctionItem);

  QVariant rangeY[2];
  rangeY[0] = d->TransferFunction->minValue();
  rangeY[1] = d->TransferFunction->maxValue();
  if (rangeY[0].canConvert<qreal>())
    {
    if (rangeY[1].toReal() == rangeY[0].toReal())
      {
      return 0.;
      }
    else
      {
      return rangeY[0].toReal();
      }
    }
  else if (rangeY[0].canConvert<QColor>())
    {
    if ( rangeY[1].value<QColor>().alphaF() == rangeY[0].value<QColor>().alphaF())
      {
      return 0.;
      }
    else
      {
      return rangeY[0].value<QColor>().alphaF();
      }
    }
  else
    {
    Q_ASSERT(rangeY[0].canConvert<qreal>() ||
        rangeY[0].canConvert<QColor>());
    }
  return 0;
}

//-----------------------------------------------------------------------------
QPointF ctkTransferFunctionItem::transferFunction2ScreenCoordinates( qreal x, qreal y)
{
  QPointF screenCoordinates((x - this->rangeXOffSet() ) * this->rangeXDiff(),
      this->rect().height() - (y - this->rangeYOffSet() ) * this->rangeYDiff() );
  return screenCoordinates;
}

//-----------------------------------------------------------------------------
QPointF ctkTransferFunctionItem::screen2TransferFunctionCoordinates( qreal x, qreal y)
{
  QPointF transferFunctionCoordinates((x / this->rangeXDiff() ) + this->rangeXOffSet(),
      ( - y + this->rect().height() )/this->rangeYDiff() + this->rangeYOffSet());

  return transferFunctionCoordinates;
}
*/

//-----------------------------------------------------------------------------
QVariant ctkTransferFunctionItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  QVariant res = this->QGraphicsObject::itemChange(change, value);
  if (change == QGraphicsItem::ItemSceneHasChanged && this->scene())
    {
    connect(this->transferFunction(),SIGNAL(changed()),
            this->scene(), SLOT(update()),Qt::UniqueConnection);
    }
  return res;
}
