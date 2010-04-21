/*=========================================================================

  Library:   ctk

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/
/// Qt includes
#include <QGraphicsScene>
#include <QResizeEvent>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionWidget.h"
#include "ctkTransferFunctionItems.h"

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
}
