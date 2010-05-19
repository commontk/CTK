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
#include <QColor>
#include <QDebug>
#include <QLinearGradient>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QtGlobal>
#include <QVariant>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionControlPointsItem.h"

//-----------------------------------------------------------------------------
class ctkTransferFunctionControlPointsItemPrivate: 
  public ctkPrivate<ctkTransferFunctionControlPointsItem>
{
public:
  ctkTransferFunctionControlPointsItemPrivate();
  void init();
  QList<QPointF> ControlPoints;
  QSizeF         PointSize;
  int            SelectedPoint;
};

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItemPrivate::ctkTransferFunctionControlPointsItemPrivate()
{
  this->PointSize = QSizeF(10.,10.);
  this->SelectedPoint = -1;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItemPrivate::init()
{
  CTK_P(ctkTransferFunctionControlPointsItem);
  p->setAcceptedMouseButtons(Qt::LeftButton);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItem::ctkTransferFunctionControlPointsItem(QGraphicsItem* parentGraphicsItem)
  :ctkTransferFunctionItem(parentGraphicsItem)
{
  CTK_INIT_PRIVATE(ctkTransferFunctionControlPointsItem);
  ctk_d()->init();
}

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItem::ctkTransferFunctionControlPointsItem(
  ctkTransferFunction* transferFunction, QGraphicsItem* parentItem)
  :ctkTransferFunctionItem(transferFunction, parentItem)
{
  CTK_INIT_PRIVATE(ctkTransferFunctionControlPointsItem);
  ctk_d()->init();
}

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItem::~ctkTransferFunctionControlPointsItem()
{  
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::paint(
  QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  CTK_D(ctkTransferFunctionControlPointsItem);
  int count = this->transferFunction() ? this->transferFunction()->count() : 0;
  if (count <= 0)
    {
    return;
    }

  qreal rangeXDiff = this->rangeXDiff();
  qreal rangeXOffSet = this->rangeXOffSet();

  qreal rangeYDiff = this->rangeYDiff();
  qreal rangeYOffSet = this->rangeYDiff();

  ctkControlPoint* startCP = this->transferFunction()->controlPoint(0);
  ctkControlPoint* endCP = 0;
  qreal start = 0;
  qreal end = 0;

  QPainterPath path;
 // this->x(controlpoint)
  QPointF startPos(startCP->x() - rangeXOffSet, this->y(startCP->value()) - rangeYOffSet);
  startPos.rx() *= rangeXDiff;
  startPos.setY(this->rect().height() 
                - startPos.y() * rangeYDiff);
  
  d->ControlPoints.clear();
  d->ControlPoints << startPos;

  path.moveTo(startPos);
  for(int i = 1; i < count; ++i)
    {
    endCP = this->transferFunction()->controlPoint(i);
    if (dynamic_cast<ctkNonLinearControlPoint*>(startCP))
      {
      QList<ctkPoint> points = this->nonLinearPoints(startCP, endCP);
      int j;
      for (j = 1; j < points.count(); ++j)
        {
        path.lineTo(
          QPointF(
            this->transferFunction2ScreenCoordinates(points[j].X,
                                                     this->y(points[j].Value))));
        }
      j = points.count() - 1;
      d->ControlPoints << QPointF( this->transferFunction2ScreenCoordinates(
        points[j].X,
        this->y(points[j].Value)));
      }
    else //dynamic_cast<ctkBezierControlPoint*>(startCP))
      {
      QList<ctkPoint> points = this->bezierParams(startCP, endCP);
      QList<ctkPoint>::iterator it = points.begin();
      QList<QPointF> bezierPoints;
      foreach(const ctkPoint& p, points)
        {
        bezierPoints << 
          QPointF((p.X - rangeXOffSet)* rangeXDiff , 
                  this->rect().height() - (this->y(p.Value) - rangeYOffSet)* rangeYDiff);
        }
      path.cubicTo(bezierPoints[1], bezierPoints[2], bezierPoints[3]);
      d->ControlPoints << bezierPoints[3];
      }
    //qDebug() << i << points[0] << points[1] << points[2] << points[3];
    delete startCP;
    startCP = endCP;
    }
  if (startCP)
    {
    delete startCP;
    }
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(QPen(QColor(255, 255, 255, 191), 1));
  painter->drawPath(path);

  QPainterPath points;
  foreach(const QPointF& point, d->ControlPoints)
    {
    points.addEllipse(point, d->PointSize.width(), d->PointSize.height());
    }
  painter->setBrush(QBrush(QColor(191, 191, 191, 127)));
  painter->drawPath(points);
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  qDebug() << "mouse press caught";
  CTK_D(ctkTransferFunctionControlPointsItem);
  QRectF pointArea(QPointF(0,0), d->PointSize*2.);
  d->SelectedPoint = -1;
  for(int i = 0; i < d->ControlPoints.count(); ++i)
    {
    pointArea.moveCenter(d->ControlPoints[i]);
    if (pointArea.contains(e->pos()))
      {
      d->SelectedPoint = i;
      break;
      }
    }
  if (d->SelectedPoint < 0)
    {
    //QPointF currentPoint( e->pos() );
  //  e->pos()->x();
    //this->transferFunction()->insertControlPoint( e->pos().x() );

  // convert coordinates
  QPointF functionCoordinates = screen2TransferFunctionCoordinates( e->pos().x(), e->pos().y());
  // add point to transfer function
  // returns index
  int index = this->transferFunction()->insertControlPoint( functionCoordinates.x());
  // update value of the point
  this->transferFunction()->setControlPointValue( index, functionCoordinates.y());
    }
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
  qDebug() << "mouse caught";
  CTK_D(ctkTransferFunctionControlPointsItem);
  if (d->SelectedPoint < 0)
    {
    e->ignore();
    return;
    }
  qreal range[2];
  this->transferFunction()->range(range);
  qreal newPos = range[0] + e->pos().x() / (this->rect().width() / (range[1] - range[0]));
  newPos = qBound(range[0], newPos, range[1]);
  this->transferFunction()->setControlPointPos(d->SelectedPoint, newPos);
  //this->transferFunction()->setControlPointValue(d->SelectedPoint, e->y());
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
  qDebug() << "mouse release caught";
  CTK_D(ctkTransferFunctionControlPointsItem);
  if (d->SelectedPoint < 0)
    {
    e->ignore();
    return;
    }
  d->SelectedPoint = -1;
}
