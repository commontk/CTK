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
#include "ctkTransferFunctionControlPointsItem.h"
#include "ctkTransferFunctionScene.h"
#include "ctkTransferFunctionWidget.h"

//-----------------------------------------------------------------------------
class ctkTransferFunctionControlPointsItemPrivate: 
  public ctkPrivate<ctkTransferFunctionControlPointsItem>
{
public:
  ctkTransferFunctionControlPointsItemPrivate();
  void init();
  QList<QPointF> ControlPoints;
  QSize          PointSize;
  int            SelectedPoint;
};

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItemPrivate::ctkTransferFunctionControlPointsItemPrivate()
{
  this->PointSize = QSize(12,12);
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

  ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  Q_ASSERT(tfScene);
  
  const QPainterPath& curve = tfScene->curve();
  QPen pen(QColor(255, 255, 255, 191), 1);
  pen.setCosmetic(true);
  painter->setPen(pen);
  painter->drawPath(curve);

  d->ControlPoints = tfScene->points();
  painter->setBrush(QBrush(QColor(191, 191, 191, 127)));
  painter->save();
  QTransform transform = painter->transform();
  painter->setTransform(QTransform());
  foreach(const QPointF& point, d->ControlPoints)
    {
    QPointF pos = transform.map(point);
    painter->drawEllipse(pos.x() - d->PointSize.width() / 2, 
                         pos.y() - d->PointSize.height() / 2, 
                         d->PointSize.width(), d->PointSize.width());
    //points.addEllipse(point, d->PointSize.width(), d->PointSize.height());
    }
  painter->restore();
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  CTK_D(ctkTransferFunctionControlPointsItem);
  QWidget* w = e->widget();
  ctkTransferFunctionWidget* view = qobject_cast<ctkTransferFunctionWidget*>(e->widget()->parentWidget());
  Q_ASSERT(view);
  // e->pos() is ok, pointArea should be in the world coordiate
  QRect pointViewArea(QPoint(-d->PointSize.width() / 2, -d->PointSize.height() / 2), d->PointSize);
  QPolygonF pointItemArea = this->mapFromScene(view->mapToScene(pointViewArea));
  d->SelectedPoint = -1;
  for(int i = 0; i < d->ControlPoints.count(); ++i)
    {
    if (pointItemArea.translated(d->ControlPoints[i]).containsPoint(e->pos(), Qt::OddEvenFill))
      {
      d->SelectedPoint = i;
      break;
      }
    }
  if (d->SelectedPoint >= 0)
    {
    return;
    }
  ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  Q_ASSERT(tfScene);
  
  // convert coordinates
  QPointF tfPos = tfScene->mapPointFromScene(e->pos());
  // add point to transfer function
  // returns index
  int index = this->transferFunction()->insertControlPoint( tfPos.x());
  // update value of the point
  if (!QSharedPointer<ctkControlPoint>(this->transferFunction()->controlPoint(index))->value().canConvert<QColor>())
    {
    this->transferFunction()->setControlPointValue( index, tfPos.y());
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

  ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  Q_ASSERT(tfScene);
  QPointF newPos = tfScene->mapPointFromScene(e->pos());

  // Deal with borders
  if(d->SelectedPoint == 0 || d->SelectedPoint == this->transferFunction()->count() )
    {
    qDebug() << "border" ;
    return;
    }
  else if( this->transferFunction()->controlPoint(d->SelectedPoint - 1)->x() > newPos.x() ||
      this->transferFunction()->controlPoint(d->SelectedPoint + 1)->x() < newPos.x())
    {
    return;
    }
  this->transferFunction()->setControlPointPos(d->SelectedPoint, newPos.x());
  if (!QSharedPointer<ctkControlPoint>(this->transferFunction()->controlPoint(d->SelectedPoint))->value().canConvert<QColor>())
    {
    this->transferFunction()->setControlPointValue(d->SelectedPoint, newPos.y());
    }
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
  CTK_D(ctkTransferFunctionControlPointsItem);
  if (d->SelectedPoint < 0)
    {
    e->ignore();
    return;
    }
  d->SelectedPoint = -1;
}
