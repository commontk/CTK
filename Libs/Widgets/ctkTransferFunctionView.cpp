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
#include <QDebug>
//#include <QGLWidget>
#include <QGraphicsScene>
#include <QResizeEvent>

/// CTK includes
#include "ctkHistogram.h"
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionBarsItem.h"
#include "ctkTransferFunctionControlPointsItem.h"
#include "ctkTransferFunctionGradientItem.h"
#include "ctkTransferFunctionScene.h"
#include "ctkTransferFunctionView.h"

//-----------------------------------------------------------------------------
class ctkTransferFunctionViewPrivate
{
  Q_DECLARE_PUBLIC(ctkTransferFunctionView);
protected:
  ctkTransferFunctionView* const q_ptr;
public:
  ctkTransferFunctionViewPrivate(ctkTransferFunctionView& object);
  void init();
};

//-----------------------------------------------------------------------------
ctkTransferFunctionViewPrivate::ctkTransferFunctionViewPrivate(ctkTransferFunctionView& object)
  :q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionViewPrivate::init()
{
  Q_Q(ctkTransferFunctionView);
  q->setScene(new ctkTransferFunctionScene(q));
  q->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  q->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //q->setViewport(new QGLWidget);
  q->setRenderHint(QPainter::Antialiasing);
}

//-----------------------------------------------------------------------------
ctkTransferFunctionView::ctkTransferFunctionView(QWidget* parentWidget)
  :QGraphicsView(parentWidget)
  , d_ptr(new ctkTransferFunctionViewPrivate(*this))
{
  Q_D(ctkTransferFunctionView);
  d->init();
}

//-----------------------------------------------------------------------------
ctkTransferFunctionView::~ctkTransferFunctionView()
{
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionView::resizeEvent(QResizeEvent * event)
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
}
