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
#include <QKeyEvent>

/// CTK includes
#include "ctkTransferFunctionControlPointsItem.h"
#include "ctkTransferFunctionRepresentation.h"
#include "ctkTransferFunctionScene.h"
#include "ctkTransferFunctionView.h"

//-----------------------------------------------------------------------------
class ctkTransferFunctionControlPointsItemPrivate
{
  Q_DECLARE_PUBLIC(ctkTransferFunctionControlPointsItem);

protected:
  ctkTransferFunctionControlPointsItem* q_ptr;
  
public:
  ctkTransferFunctionControlPointsItemPrivate(ctkTransferFunctionControlPointsItem& object);
  void init();
  QList<QPointF> ControlPoints;
  QSize          PointSize;
  int            SelectedPoint;
};

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItemPrivate::ctkTransferFunctionControlPointsItemPrivate(ctkTransferFunctionControlPointsItem& object)
  :q_ptr(&object)
{
  this->PointSize = QSize(12,12);
  this->SelectedPoint = -1;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItemPrivate::init()
{
  Q_Q(ctkTransferFunctionControlPointsItem);
  q->setAcceptedMouseButtons(Qt::LeftButton);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItem::ctkTransferFunctionControlPointsItem(QGraphicsItem* parentGraphicsItem)
  :ctkTransferFunctionItem(parentGraphicsItem)
  , d_ptr(new ctkTransferFunctionControlPointsItemPrivate(*this))
{
  Q_D(ctkTransferFunctionControlPointsItem);
  d->init();
}

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItem::ctkTransferFunctionControlPointsItem(
  ctkTransferFunction* transferFunction, QGraphicsItem* parentItem)
  :ctkTransferFunctionItem(transferFunction, parentItem)
  , d_ptr(new ctkTransferFunctionControlPointsItemPrivate(*this))
{
  Q_D(ctkTransferFunctionControlPointsItem);
  d->init();
}

//-----------------------------------------------------------------------------
ctkTransferFunctionControlPointsItem::~ctkTransferFunctionControlPointsItem()
{  
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::paint(
  QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);
  Q_D(ctkTransferFunctionControlPointsItem);
  int count = this->transferFunction() ? this->transferFunction()->count() : 0;
  if (count <= 0)
    {
    return;
    }

  //ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  //Q_ASSERT(tfScene);
  ctkTransferFunctionRepresentation* tfRep = this->transferFunction()->representation();
  
  const QPainterPath& curve = tfRep->curve();
  QPen pen(QColor(255, 255, 255, 191), 1);
  pen.setCosmetic(true);
  painter->setPen(pen);
  painter->drawPath(curve);

  d->ControlPoints = tfRep->points();
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
  Q_D(ctkTransferFunctionControlPointsItem);
  ctkTransferFunctionView* view = qobject_cast<ctkTransferFunctionView*>(e->widget()->parentWidget());
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
  //ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  //Q_ASSERT(tfScene);
  ctkTransferFunctionRepresentation* tfRep = this->transferFunction()->representation();
  
  // convert coordinates
  QPointF tfPos = tfRep->mapPointFromScene(e->pos());
  // add point to transfer function
  // returns index
  int index = this->transferFunction()->insertControlPoint( tfPos.x());

  //NOT WORKING IN COMPOSITE
  // update value of the point
  if (!QSharedPointer<ctkControlPoint>(this->transferFunction()->controlPoint(index))->value().canConvert<QColor>())
    {
    this->transferFunction()->setControlPointValue( index, tfPos.y());
    }
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
  Q_D(ctkTransferFunctionControlPointsItem);
  if (d->SelectedPoint < 0)
    {
    e->ignore();
    return;
    }

  //ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  //Q_ASSERT(tfScene);
  ctkTransferFunctionRepresentation* tfRep = this->transferFunction()->representation();

  QPointF newPos = tfRep->mapPointFromScene(e->pos());

  // Deal with borders
  if(d->SelectedPoint == 0 || d->SelectedPoint == this->transferFunction()->count() )
    {
    // BEHAVIOR TO BE IMPLEMENTED
    // int borderBehavior = tfScene->borderBehavior();
    // LockBorder
    // Create new point on border at same height
    // Create new point on border on top
    // Create new point on border on bottom
    qDebug() << "border" ;
    return;
    }
  else
    {
    // TO BE IMPLEMENTED
    //int movePointsBehavior = tfScene->movePointsBehavior();
    // initialize to BLOCK_MOVE for now
    int movePointsBehavior = STOP_MOVE;

    switch( movePointsBehavior ){
      case STOP_MOVE:
        stopPoints(newPos);
        break;
      case SWITCH_MOVE:
        switchPoints(newPos);
        break;
      case DRAW_MOVE:
        drawPoints(newPos);
        break;
      case FUSION_MOVE:
        fusionPoints(newPos);
        break;
      default:
        break;
      }
    }
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::stopPoints( QPointF iPointF )
{
  Q_D(ctkTransferFunctionControlPointsItem);

  if( this->transferFunction()->controlPoint(d->SelectedPoint - 1)->x() > iPointF.x() ||
      this->transferFunction()->controlPoint(d->SelectedPoint + 1)->x() < iPointF.x())
    {
    return;
    }

  updatePointPosition(iPointF);
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::switchPoints( QPointF iPointF )
{
  Q_D(ctkTransferFunctionControlPointsItem);

  // Increment or decrement selected point?
  // Don't need to check borders since it is done just before calling this method...
  if( this->transferFunction()->controlPoint(d->SelectedPoint - 1)->x() > iPointF.x() )
    {
    // NOT WORKING IF COMPOSITE TRANSFER FUNCTION
    double value = this->transferFunction()->value(d->SelectedPoint-1 ).toDouble();
    d->SelectedPoint -= 1;
    this->transferFunction()->setControlPointValue(d->SelectedPoint+1, value);
    }
  else if ( this->transferFunction()->controlPoint(d->SelectedPoint + 1)->x() < iPointF.x() )
    {
    double value = this->transferFunction()->value(d->SelectedPoint + 1 ).toDouble();
    d->SelectedPoint += 1;
    this->transferFunction()->setControlPointValue(d->SelectedPoint-1, value);
    }

  updatePointPosition(iPointF);
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::drawPoints( QPointF iPointF )
{
  Q_D(ctkTransferFunctionControlPointsItem);

  // Increment or decrement selected point
  if( this->transferFunction()->controlPoint(d->SelectedPoint - 1)->x() > iPointF.x() )
    {
    //change the selected point
    d->SelectedPoint = d->SelectedPoint -1;
    }
  else if ( this->transferFunction()->controlPoint(d->SelectedPoint + 1)->x() < iPointF.x() )
    {
    d->SelectedPoint = d->SelectedPoint +1;
    }

  updatePointPosition(iPointF);
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::fusionPoints( QPointF iPointF )
{
  Q_UNUSED(iPointF);
  // TO BE IMPLEMENTED
  // if 2 points are to close: delete one..?
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::updatePointPosition( QPointF iPoint )
{
  Q_D(ctkTransferFunctionControlPointsItem);

  this->transferFunction()->setControlPointPos(d->SelectedPoint, iPoint.x());

  // TEST NOT WORKING IN COMPOSITE TRANSFER FUNCTION
  if ( ! QSharedPointer<ctkControlPoint>(this->transferFunction()->controlPoint(d->SelectedPoint))->value().canConvert<QColor>() )
  {
  this->transferFunction()->setControlPointValue(d->SelectedPoint, iPoint.y());
  }
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
  Q_D(ctkTransferFunctionControlPointsItem);
  if (d->SelectedPoint < 0)
    {
    e->ignore();
    return;
    }
  d->SelectedPoint = -1;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionControlPointsItem::keyPressEvent ( QKeyEvent * keyEvent )
{
    Q_D(ctkTransferFunctionControlPointsItem);

    // if a point is selected, "d" and "D" = delete the point
    if (d->SelectedPoint >= 0)
      {
      if ( ! QString::compare(keyEvent->text(), "d") || ! QString::compare(keyEvent->text(), "D") )
        {
        // Get value of selected point
        qreal xValue = this->transferFunction()->controlPoint( d->SelectedPoint )->x();
        // remove point using value ( can't use point ID because it could engender
        // problems when multiple points selection implemented)
        this->transferFunction()->removeControlPoint( xValue );
        }
      }
}
