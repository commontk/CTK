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
#include <QDebug>
//#include <QGLWidget>
#include <QGraphicsScene>
#include <QResizeEvent>

/// CTK includes
#include "ctkHistogram.h"
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionWidget.h"
#include "ctkTransferFunctionScene.h"
#include "ctkTransferFunctionGradientItem.h"
#include "ctkTransferFunctionControlPointsItem.h"
#include "ctkTransferFunctionHistogramItem.h"

//-----------------------------------------------------------------------------
class ctkTransferFunctionWidgetPrivate: public ctkPrivate<ctkTransferFunctionWidget>
{
  CTK_DECLARE_PUBLIC(ctkTransferFunctionWidget);
public:
  ctkTransferFunctionWidgetPrivate();
  void init();
  ctkTransferFunction* TransferFunction;
};

//-----------------------------------------------------------------------------
ctkTransferFunctionWidgetPrivate::ctkTransferFunctionWidgetPrivate()
{
  this->TransferFunction = 0;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionWidgetPrivate::init()
{
  CTK_P(ctkTransferFunctionWidget);
  p->setScene(new ctkTransferFunctionScene(p));
  p->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  p->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //p->setViewport(new QGLWidget);
  p->setRenderHint(QPainter::Antialiasing);
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
  ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  Q_ASSERT(tfScene);
  tfScene->clear();
  tfScene->setTransferFunction(transferFunction);

  ctkTransferFunctionGradientItem* gradient = 
    new ctkTransferFunctionGradientItem(transferFunction);
  gradient->setRect(tfScene->sceneRect());
  this->scene()->addItem(gradient);

  if (qobject_cast<ctkHistogram*>(transferFunction) != 0)
    {
    gradient->setMask(false);
    ctkHistogram* histogram = qobject_cast<ctkHistogram*>(transferFunction);
    ctkTransferFunctionHistogramItem* histogramItem = 
      new ctkTransferFunctionHistogramItem(histogram);
    //controlPoints->setRect(tfScene->sceneRect());
    this->scene()->addItem(histogramItem);
    }
  else
    {
    ctkTransferFunctionControlPointsItem* controlPoints = 
      new ctkTransferFunctionControlPointsItem(transferFunction);
    //controlPoints->setRect(tfScene->sceneRect());
    this->scene()->addItem(controlPoints);
    } 
}

//-----------------------------------------------------------------------------
ctkTransferFunction* ctkTransferFunctionWidget::transferFunction()const
{
  return ctk_d()->TransferFunction;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionWidget::resizeEvent(QResizeEvent * event)
{
  /*
  QRectF sceneRect(QPointF(0,0),event->size());
  this->scene()->setSceneRect(sceneRect);
  foreach(QGraphicsItem * item, this->scene()->items())
    {
    ctkTransferFunctionItem* rectItem = 
      qgraphicsitem_cast<ctkTransferFunctionItem*>(item);
    if (rectItem)
      {
      rectItem->setRect(sceneRect);
      }
    }
  */
  QMatrix zoomMatrix;
  zoomMatrix.scale(event->size().width(), event->size().height());
  bool blocked = this->blockSignals(true);
  this->setMatrix(zoomMatrix);
  this->blockSignals(blocked);
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
