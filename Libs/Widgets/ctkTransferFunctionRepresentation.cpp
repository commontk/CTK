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
#include <QGraphicsScene>
#include <QLinearGradient>
#include <QResizeEvent>
#include <QDebug>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionRepresentation.h"

/// STL includes
#include <limits>

//-----------------------------------------------------------------------------
class ctkTransferFunctionRepresentationPrivate
{
public:
  ctkTransferFunctionRepresentationPrivate();

  ctkTransferFunction* TransferFunction;
  QColor               VerticalGradientColor;

  QPainterPath         Path;
  QLinearGradient      Gradient;
  QList<QPointF>       Points;

  QRectF       rect()const;
  qreal        width()const;
  qreal        height()const;

  qreal        WorldRangeX[2];
  QVariant     WorldRangeY[2];
  qreal        RangeXDiff;
  qreal        RangeXOffSet;
  qreal        RangeYDiff;
  qreal        RangeYOffSet;
};

//-----------------------------------------------------------------------------
ctkTransferFunctionRepresentationPrivate::ctkTransferFunctionRepresentationPrivate()
{
  this->TransferFunction = 0;
  this->VerticalGradientColor = QColor::fromRgbF(1., 0., 0., 1. );
  this->RangeXDiff = 0.;
  this->RangeXOffSet = 0.;
  this->RangeYDiff = 0.;
  this->RangeYOffSet = 0.;
}

//-----------------------------------------------------------------------------
QRectF ctkTransferFunctionRepresentationPrivate::rect()const
{
  return QRectF(0.,0.,1.,1.);
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentationPrivate::width()const
{
  return 1.;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentationPrivate::height()const
{
  return 1.;
}

//-----------------------------------------------------------------------------
ctkTransferFunctionRepresentation::ctkTransferFunctionRepresentation(QObject* parentObject)
  :QObject(parentObject)
  , d_ptr(new ctkTransferFunctionRepresentationPrivate)
{
}

//-----------------------------------------------------------------------------
ctkTransferFunctionRepresentation::ctkTransferFunctionRepresentation(
  ctkTransferFunction* transferFunction, QObject* parentObject)
  :QObject(parentObject)
  , d_ptr(new ctkTransferFunctionRepresentationPrivate)
{
  this->setTransferFunction(transferFunction);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionRepresentation::~ctkTransferFunctionRepresentation()
{
}

//-----------------------------------------------------------------------------
QColor ctkTransferFunctionRepresentation::verticalGradientColor() const
{
  Q_D( const ctkTransferFunctionRepresentation );
  return d->VerticalGradientColor;
}
//-----------------------------------------------------------------------------
void ctkTransferFunctionRepresentation::setVerticalGradientColor( QColor verticalGradientColor )
{
  Q_D( ctkTransferFunctionRepresentation );
  d->VerticalGradientColor = verticalGradientColor;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionRepresentation::setTransferFunction(ctkTransferFunction* transferFunction)
{
  Q_D(ctkTransferFunctionRepresentation);
  if (d->TransferFunction == transferFunction)
    {
    return;
    }
  d->TransferFunction = transferFunction;
  connect( d->TransferFunction, SIGNAL(changed()),
           this, SLOT(onTransferFunctionChanged()),
           Qt::UniqueConnection);
  this->onTransferFunctionChanged();
}

//-----------------------------------------------------------------------------
ctkTransferFunction* ctkTransferFunctionRepresentation::transferFunction()const
{
  Q_D(const ctkTransferFunctionRepresentation);
  return d->TransferFunction;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionRepresentation::onTransferFunctionChanged()
{
  Q_D(ctkTransferFunctionRepresentation);
  // delete cache here
  d->Path = QPainterPath();
  d->Points.clear();
}

//-----------------------------------------------------------------------------
const QPainterPath& ctkTransferFunctionRepresentation::curve()const
{
  Q_D(const ctkTransferFunctionRepresentation);
  if (d->Path.isEmpty())
    {
    const_cast<ctkTransferFunctionRepresentation*>(this)->computeCurve();
    const_cast<ctkTransferFunctionRepresentation*>(this)->computeGradient();
    }
  return d->Path;
}

//-----------------------------------------------------------------------------
const QList<QPointF>& ctkTransferFunctionRepresentation::points()const
{
  Q_D(const ctkTransferFunctionRepresentation);
  if (d->Path.isEmpty())
    {
    const_cast<ctkTransferFunctionRepresentation*>(this)->computeCurve();
    const_cast<ctkTransferFunctionRepresentation*>(this)->computeGradient();
    }
  return d->Points;
}

//-----------------------------------------------------------------------------
const QGradient& ctkTransferFunctionRepresentation::gradient()const
{
  Q_D(const ctkTransferFunctionRepresentation);
  if (d->Path.isEmpty())
    {
    const_cast<ctkTransferFunctionRepresentation*>(this)->computeCurve();
    const_cast<ctkTransferFunctionRepresentation*>(this)->computeGradient();
    }
  return d->Gradient;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionRepresentation::computeCurve()
{
  Q_D(ctkTransferFunctionRepresentation);

  int count = d->TransferFunction ? d->TransferFunction->count() : 0;
  if (count <= 0)
    {
    return;
    }

  d->TransferFunction->range(d->WorldRangeX[0], d->WorldRangeX[1]);
  d->WorldRangeY[0] = this->posY(d->TransferFunction->minValue());
  d->WorldRangeY[1] = this->posY(d->TransferFunction->maxValue());

  d->RangeXDiff   = this->computeRangeXDiff(d->rect(), d->WorldRangeX);
  d->RangeXOffSet = this->computeRangeXOffset(d->WorldRangeX);

  d->RangeYDiff   = this->computeRangeYDiff(d->rect(), d->WorldRangeY);
  d->RangeYOffSet = this->computeRangeYOffset(d->WorldRangeY);

  ctkControlPoint* startCP = d->TransferFunction->controlPoint(0);
  ctkControlPoint* nextCP = 0;

  QPointF startPos = this->mapPointToScene(startCP);

  d->Points.clear();
  d->Points << startPos;

  d->Path = QPainterPath();
  d->Path.moveTo(startPos);
  for(int i = 1; i < count; ++i)
    {
    nextCP = d->TransferFunction->controlPoint(i);
    if (this->transferFunction()->isDiscrete())
      {
      QPointF nextPos = this->mapPointToScene(nextCP);
      qreal midPosX = (startPos.x() + nextPos.x()) / 2.;

      d->Path.lineTo(QPointF(midPosX, startPos.y()));
      d->Path.lineTo(QPointF(midPosX, nextPos.y()));

      d->Points << nextPos;
      startPos = nextPos;
      if (i == count -1)
        {
        d->Path.lineTo(nextPos);
        }
      }
    else if (dynamic_cast<ctkNonLinearControlPoint*>(startCP))
      {
      QList<ctkPoint> points = this->nonLinearPoints(startCP, nextCP);
      int j;
      for (j = 1; j < points.count(); ++j)
        {
        d->Path.lineTo(this->mapPointToScene(points[j]));
        }
      j = points.count() - 1;
      d->Points << this->mapPointToScene(points[j]);
      }
    else //dynamic_cast<ctkBezierControlPoint*>(startCP))
      {
      QList<ctkPoint> points = this->bezierParams(startCP, nextCP);
      QList<QPointF> bezierPoints;
      foreach(const ctkPoint& p, points)
        {
        bezierPoints << this->mapPointToScene(p);
        }
      d->Path.cubicTo(bezierPoints[1], bezierPoints[2], bezierPoints[3]);
      d->Points << bezierPoints[3];
      }
    //qDebug() << i << points[0] << points[1] << points[2] << points[3];
    delete startCP;
    startCP = nextCP;
    }
  if (startCP)
    {
    delete startCP;
    }
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionRepresentation::computeGradient()
{
  Q_D(ctkTransferFunctionRepresentation);

  int count = d->TransferFunction ? d->TransferFunction->count() : 0;
  if (count <= 0)
    {
    return;
    }
  d->TransferFunction->range(d->WorldRangeX[0], d->WorldRangeX[1]);
  d->WorldRangeY[0] = this->posY(d->TransferFunction->minValue());
  d->WorldRangeY[1] = this->posY(d->TransferFunction->maxValue());

  d->RangeXDiff   = this->computeRangeXDiff(QRectF(0.,0.,1.,1.), d->WorldRangeX);
  d->RangeXOffSet = this->computeRangeXOffset(d->WorldRangeX);

  d->RangeYDiff   = this->computeRangeYDiff(QRectF(0.,0.,1.,1.), d->WorldRangeY);
  d->RangeYOffSet = this->computeRangeYOffset(d->WorldRangeY);

  ctkControlPoint* startCP = d->TransferFunction->controlPoint(0);
  ctkControlPoint* nextCP = 0;

  qreal startPos = this->mapXToScene(this->posX(startCP->x()));
  qreal nextPos;

  //
  //if we have no colors in value (i.e. can't convert value to color)
  if (! d->TransferFunction->value(0).canConvert<QColor>())
    {
    // create vertical gradient
    d->Gradient = QLinearGradient(0., 0., 0., 1.);
    // red
    d->Gradient.setColorAt(0, d->VerticalGradientColor );
    // to black
    d->Gradient.setColorAt(1, QColor::fromRgbF(0., 0., 0., 1. ));
    return;
    }

  // classic gradient if we have colors in value
  d->Gradient = QLinearGradient(0., 0., 1., 0.);
  d->Gradient.setColorAt(startPos, this->color(startCP));
  for(int i = 1; i < count; ++i)
    {
    nextCP = d->TransferFunction->controlPoint(i);
    nextPos = this->mapXToScene(this->posX(nextCP));
    if (this->transferFunction()->isDiscrete())
      {
      qreal midPoint = (startPos + nextPos)  / 2;
      d->Gradient.setColorAt(midPoint, this->color(startCP));
      d->Gradient.setColorAt(midPoint + std::numeric_limits<qreal>::epsilon(), this->color(nextCP));
      }
    else if (dynamic_cast<ctkNonLinearControlPoint*>(startCP))
      {
      QList<ctkPoint> points = this->nonLinearPoints(startCP, nextCP);
      foreach(const ctkPoint& p, points)
        {
        d->Gradient.setColorAt(this->mapXToScene(this->posX(p)), this->color(p));
        }
      //no need, d->Gradient.setColorAt(nextPos, this->color(nextCP));
      }
    else //dynamic_cast<ctkBezierControlPoint*>(startCP))
      { // TODO handle bezier points with color
      QList<ctkPoint> points = this->bezierParams(startCP, nextCP);
      QList<QPointF> bezierPoints;
      foreach(const ctkPoint& p, points)
        {
        d->Gradient.setColorAt(this->mapXToScene(this->posX(p)), this->color(p));
        }
      nextPos = this->mapXToScene(this->posX(points[points.size() - 1]));
      }
    //qDebug() << i << points[0] << points[1] << points[2] << points[3];
    delete startCP;
    startCP = nextCP;
    startPos = nextPos;
    }
  d->Gradient.setColorAt(startPos, this->color(startCP));
  if (startCP)
    {
    delete startCP;
    }
}

//-----------------------------------------------------------------------------
QList<ctkPoint> ctkTransferFunctionRepresentation::bezierParams(
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
QList<ctkPoint> ctkTransferFunctionRepresentation::nonLinearPoints(
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
QColor ctkTransferFunctionRepresentation::color(const QVariant& v) const
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
qreal ctkTransferFunctionRepresentation::computeRangeXDiff(const QRectF& rect, qreal rangeX[2])
{
  return rect.width() / (rangeX[1] - rangeX[0]);
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::computeRangeXOffset(qreal rangeX[2])
{
  return rangeX[0];
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::computeRangeYDiff(const QRectF& rect, const QVariant rangeY[2])
{
  qreal rangeYDiff = rect.height();
  qreal rangePosY[2];
  rangePosY[0] = this->posY(rangeY[0]);
  rangePosY[1] = this->posY(rangeY[1]);
  if (rangePosY[1] == rangePosY[0])
    {
    rangeYDiff /= rangePosY[0];
    return rangeYDiff;
    }
  rangeYDiff /= rangePosY[1] - rangePosY[0];
  return rangeYDiff;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::computeRangeYOffset(const QVariant rangeY[2])
{
  qreal rangePosY[2];
  rangePosY[0] = this->posY(rangeY[0]);
  rangePosY[1] = this->posY(rangeY[1]);

  if (rangePosY[1] == rangePosY[0])
    {
    return 0.;
    }
  return rangePosY[0];
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::posX(const qreal& x)const
{
  return x;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::posY(const QVariant& value)const
{
  Q_ASSERT(value.canConvert<qreal>() || value.canConvert<QColor>());
  if (value.canConvert<QColor>())
    {
    return value.value<QColor>().alphaF();
    }
  return value.toReal();
}

//-----------------------------------------------------------------------------
QPointF ctkTransferFunctionRepresentation::mapPointToScene(const ctkControlPoint* cp)const
{
  return QPointF(this->mapXToScene(this->posX(cp->x())),
                 this->mapYToScene(this->posY(cp->value())));
}

//-----------------------------------------------------------------------------
QPointF ctkTransferFunctionRepresentation::mapPointToScene(const ctkPoint& point)const
{
  return QPointF( this->mapXToScene(this->posX(point.X)),
                  this->mapYToScene(this->posY(point.Value)));
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::mapXToScene(qreal xPos)const
{
  Q_D(const ctkTransferFunctionRepresentation);
  return (xPos - d->RangeXOffSet) * d->RangeXDiff;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::mapYToScene(qreal yPos)const
{
  Q_D(const ctkTransferFunctionRepresentation);
  return d->height() - (yPos - d->RangeYOffSet) * d->RangeYDiff;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::mapXFromScene(qreal scenePosX)const
{
  Q_D(const ctkTransferFunctionRepresentation);
  return (scenePosX / d->RangeXDiff) + d->RangeXOffSet;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionRepresentation::mapYFromScene(qreal scenePosY)const
{
  Q_D(const ctkTransferFunctionRepresentation);
  return ((d->height() - scenePosY) / d->RangeYDiff) + d->RangeYOffSet ;
}
