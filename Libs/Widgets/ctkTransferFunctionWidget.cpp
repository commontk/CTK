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
#include <QResizeEvent>
#include <QDebug>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionWidget.h"
#include "ctkTransferFunctionItem.h"
#include "ctkTransferFunctionGradientItem.h"
#include "ctkTransferFunctionControlPointsItem.h"

//-----------------------------------------------------------------------------
class ctkTransferFunctionWidgetPrivate: public ctkPrivate<ctkTransferFunctionWidget>
{
  CTK_DECLARE_PUBLIC(ctkTransferFunctionWidget);
public:
  void init();
  ctkTransferFunction* TransferFunction;
};

//-----------------------------------------------------------------------------
void ctkTransferFunctionWidgetPrivate::init()
{
  CTK_P(ctkTransferFunctionWidget);
  p->setScene(new QGraphicsScene(p));
  p->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  p->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionWidget::ctkTransferFunctionWidget(QWidget* parentWidget)
  :QGraphicsView(parentWidget)
{
  CTK_INIT_PRIVATE(ctkTransferFunctionWidget);
  ctk_d()->init();
}

//-----------------------------------------------------------------------------
ctkTransferFunctionWidget::ctkTransferFunctionWidget(
  ctkTransferFunction* transferFunction, QWidget* parentWidget)
  :QGraphicsView(parentWidget)
{
  CTK_INIT_PRIVATE(ctkTransferFunctionWidget);
  ctk_d()->init();
  this->setTransferFunction(transferFunction);
}
//-----------------------------------------------------------------------------
ctkTransferFunctionWidget::~ctkTransferFunctionWidget()
{
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionWidget::setTransferFunction(ctkTransferFunction* transferFunction)
{
  CTK_D(ctkTransferFunctionWidget);
  d->TransferFunction = transferFunction;
  Q_ASSERT(this->scene());
  this->scene()->clear();
  ctkTransferFunctionGradientItem* gradient = 
    new ctkTransferFunctionGradientItem(transferFunction);
  gradient->setRect(0, 0, this->width(), this->height());
  this->scene()->addItem(gradient);
  ctkTransferFunctionControlPointsItem* controlPoints = 
    new ctkTransferFunctionControlPointsItem(transferFunction);
  controlPoints->setRect(0, 0, this->width(), this->height());
  this->scene()->addItem(controlPoints);
}

//-----------------------------------------------------------------------------
ctkTransferFunction* ctkTransferFunctionWidget::transferFunction()const
{
  return ctk_d()->TransferFunction;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionWidget::resizeEvent(QResizeEvent * event)
{
  QRectF sceneRect(QPointF(0,0),event->size());
  foreach(QGraphicsItem * item, this->scene()->items())
    {
    ctkTransferFunctionItem* rectItem = 
      qgraphicsitem_cast<ctkTransferFunctionItem*>(item);
    if (rectItem)
      {
      rectItem->setRect(sceneRect);
      }
    }
  this->scene()->setSceneRect(sceneRect);
  
  this->QGraphicsView::resizeEvent(event);
  // Control points are resized by the view transform, we want
  // fixed size control points, lines...
  //this->fitInView(this->scene()->sceneRect());
  qDebug() << "resize event caught";
}
/*
//-----------------------------------------------------------------------------
void ctkTransferFunctionWidget::dragEnterEvent ( QDragEnterEvent * event )
{
  qDebug() << "drag event caught";

  this->QGraphicsView::dragEnterEvent(event);

}

//-----------------------------------------------------------------------------
void ctkTransferFunctionWidget::mousePressEvent ( QMouseEvent * event )
{
  qDebug() << "press event caught";
  //One control point is added to the scene
  // 1 - get position of the mouse
  qDebug() << "x position " << event->x();
  qDebug() << "y position " << event->y();

  this->scene()->items()[1]->;

  // 2nd item are the control points

  this->QGraphicsView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionWidget::mouseReleaseEvent ( QMouseEvent * event )
{
  qDebug() << "release event caught";

  this->QGraphicsView::mouseReleaseEvent(event);
}
*/
