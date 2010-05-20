/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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
/// Qt includes
#include <QGraphicsScene>
#include <QLinearGradient>
#include <QResizeEvent>
#include <QDebug>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionScene.h"

/// STL includes
#include <limits>

//-----------------------------------------------------------------------------
class ctkTransferFunctionScenePrivate: public ctkPrivate<ctkTransferFunctionScene>
{
  CTK_DECLARE_PUBLIC(ctkTransferFunctionScene);
public:
  ctkTransferFunctionScenePrivate();
  QRectF               OldRect;
  ctkTransferFunction* TransferFunction;
  QPainterPath   Path;
  QLinearGradient Gradient;
  QList<QPointF> Points;
  qreal        WorldRangeX[2];
  QVariant     WorldRangeY[2];
  qreal        RangeXDiff;
  qreal        RangeXOffSet;
  qreal        RangeYDiff;
  qreal        RangeYOffSet;
};

ctkTransferFunctionScenePrivate::ctkTransferFunctionScenePrivate()
{
  this->TransferFunction = 0;
}

//-----------------------------------------------------------------------------
ctkTransferFunctionScene::ctkTransferFunctionScene(QObject* parentObject)
  :QGraphicsScene(parentObject)
{
  CTK_INIT_PRIVATE(ctkTransferFunctionScene);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionScene::ctkTransferFunctionScene(
  ctkTransferFunction* transferFunction, QObject* parentObject)
  :QGraphicsScene(parentObject)
{
  CTK_INIT_PRIVATE(ctkTransferFunctionScene);
  this->setTransferFunction(transferFunction);
}
//-----------------------------------------------------------------------------
ctkTransferFunctionScene::~ctkTransferFunctionScene()
{
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionScene::setTransferFunction(ctkTransferFunction* transferFunction)
{
  CTK_D(ctkTransferFunctionScene);
  if (d->TransferFunction == transferFunction)
    {
    return;
    }
  d->TransferFunction = transferFunction;
  connect( d->TransferFunction, SIGNAL(changed()),
           this, SLOT(onTransferFunctionChanged()),
           Qt::UniqueConnection);
  this->update();
}

//-----------------------------------------------------------------------------
ctkTransferFunction* ctkTransferFunctionScene::transferFunction()const
{
  return ctk_d()->TransferFunction;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionScene::onTransferFunctionChanged()
{
  CTK_D(ctkTransferFunctionScene);
  // TODO delete cache here
  d->Path = QPainterPath();
  this->update();  
}

//-----------------------------------------------------------------------------
const QPainterPath& ctkTransferFunctionScene::curve()const
{
  CTK_D(const ctkTransferFunctionScene);
  if (d->Path.isEmpty())// || this->sceneRect() != d->OldRect)
    {
    const_cast<ctkTransferFunctionScene*>(this)->computeCurve();
    const_cast<ctkTransferFunctionScene*>(this)->computeGradient();
    }
  return d->Path;
}

//-----------------------------------------------------------------------------
const QList<QPointF>& ctkTransferFunctionScene::points()const
{
  CTK_D(const ctkTransferFunctionScene);
  if (d->Path.isEmpty())// || this->sceneRect() != d->OldRect)
    {
    const_cast<ctkTransferFunctionScene*>(this)->computeCurve();
    const_cast<ctkTransferFunctionScene*>(this)->computeGradient();
    }
  return d->Points;
}

//-----------------------------------------------------------------------------
const QGradient& ctkTransferFunctionScene::gradient()const
{
  CTK_D(const ctkTransferFunctionScene);
  if (d->Path.isEmpty())// || this->sceneRect() != d->OldRect)
    {
    const_cast<ctkTransferFunctionScene*>(this)->computeCurve();
    const_cast<ctkTransferFunctionScene*>(this)->computeGradient();
    }
  return d->Gradient;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionScene::computeCurve()
{
  CTK_D(ctkTransferFunctionScene);

  int count = d->TransferFunction ? d->TransferFunction->count() : 0;
  if (count <= 0)
    {
    return;
    }
  qDebug() << "computeCurve" << this->sceneRect();
  d->TransferFunction->range(d->WorldRangeX[0], d->WorldRangeX[1]);
  d->WorldRangeY[0] = this->posY(d->TransferFunction->minValue());
  d->WorldRangeY[1] = this->posY(d->TransferFunction->maxValue());

  d->RangeXDiff   = this->computeRangeXDiff(this->sceneRect(), d->WorldRangeX);
  d->RangeXOffSet = this->computeRangeXOffset(d->WorldRangeX);

  d->RangeYDiff   = this->computeRangeYDiff(this->sceneRect(), d->WorldRangeY);
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
    if (dynamic_cast<ctkNonLinearControlPoint*>(startCP))
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
      QList<ctkPoint>::iterator it = points.begin();
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
void ctkTransferFunctionScene::computeGradient()
{
  CTK_D(ctkTransferFunctionScene);

  int count = d->TransferFunction ? d->TransferFunction->count() : 0;
  if (count <= 0)
    {
    return;
    }
  qDebug() << "computeCurve" << this->sceneRect();
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
      QList<ctkPoint>::iterator it = points.begin();
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
QList<ctkPoint> ctkTransferFunctionScene::bezierParams(
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
QList<ctkPoint> ctkTransferFunctionScene::nonLinearPoints(
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
QColor ctkTransferFunctionScene::color(const QVariant& v) const
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
qreal ctkTransferFunctionScene::computeRangeXDiff(const QRectF& rect, qreal rangeX[2])
{
  return rect.width() / (rangeX[1] - rangeX[0]);
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionScene::computeRangeXOffset(qreal rangeX[2])
{
  return rangeX[0];
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionScene::computeRangeYDiff(const QRectF& rect, const QVariant rangeY[2])
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
qreal ctkTransferFunctionScene::computeRangeYOffset(const QVariant rangeY[2])
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
qreal ctkTransferFunctionScene::posX(const qreal& x)const
{
  return x;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionScene::posY(const QVariant& value)const
{
  Q_ASSERT(value.canConvert<qreal>() || value.canConvert<QColor>());
  if (value.canConvert<QColor>())
    {
    return value.value<QColor>().alphaF();
    }
  return value.toReal();
}

//-----------------------------------------------------------------------------
QPointF ctkTransferFunctionScene::mapPointToScene(const ctkControlPoint* cp)const
{
  return QPointF(this->mapXToScene(this->posX(cp->x())),
                 this->mapYToScene(this->posY(cp->value())));
}

//-----------------------------------------------------------------------------
QPointF ctkTransferFunctionScene::mapPointToScene(const ctkPoint& point)const
{
  return QPointF( this->mapXToScene(this->posX(point.X)),
                  this->mapYToScene(this->posY(point.Value)));
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionScene::mapXToScene(qreal xPos)const
{
  CTK_D(const ctkTransferFunctionScene);
  return (xPos - d->RangeXOffSet) * d->RangeXDiff;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionScene::mapYToScene(qreal yPos)const
{
  CTK_D(const ctkTransferFunctionScene);
  return this->height() - (yPos - d->RangeYOffSet) * d->RangeYDiff;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionScene::mapXFromScene(qreal scenePosX)const
{
  CTK_D(const ctkTransferFunctionScene);
  return (scenePosX / d->RangeXDiff) + d->RangeXOffSet;
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionScene::mapYFromScene(qreal scenePosY)const
{
  CTK_D(const ctkTransferFunctionScene);
  return ((this->height() - scenePosY) / d->RangeYDiff) + d->RangeYOffSet ;
}
