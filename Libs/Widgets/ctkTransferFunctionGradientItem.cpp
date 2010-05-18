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
#include "ctkTransferFunctionGradientItem.h"

//-----------------------------------------------------------------------------
ctkTransferFunctionGradientItem::ctkTransferFunctionGradientItem(QGraphicsItem* parentGraphicsItem)
  :ctkTransferFunctionItem(parentGraphicsItem)
{
}

//-----------------------------------------------------------------------------
ctkTransferFunctionGradientItem::ctkTransferFunctionGradientItem(
  ctkTransferFunction* transferFunction, QGraphicsItem* parentItem)
  :ctkTransferFunctionItem(transferFunction, parentItem)
{
}

//-----------------------------------------------------------------------------
ctkTransferFunctionGradientItem::~ctkTransferFunctionGradientItem()
{  
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionGradientItem::paint(
  QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  int count = this->transferFunction() ? this->transferFunction()->count() : 0;
  if (count <= 0)
    {
    painter->fillRect(this->rect(),Qt::black);
    return;
    }
  qreal range[2];
  this->transferFunction()->range(range);
  qreal rangeDiff = this->rect().width() / (range[1] - range[0]);
  qreal rangeOffset = range[0];
  ctkControlPoint* startCP = this->transferFunction()->controlPoint(0);
  ctkControlPoint* endCP = 0;
  
  qreal start = (startCP->x() - rangeOffset) * rangeDiff;
  qreal end = 0;
  for(int i = 1; i < count; ++i)
    {
    endCP = this->transferFunction()->controlPoint(i);
    // TODO, handle Bezier points for a finer gradient
    // TODO, handle nonlinear points
    if (this->transferFunction()->isDiscrete())
      {
      // pos
      end = ((startCP->x() + endCP->x()) /2. -rangeOffset) * rangeDiff;
      QRectF itemRect = QRectF(start, 0, end - start, 
                               this->rect().height());
      if (i==1)
        {
        itemRect.setLeft(0.);
        }
      QColor valueColor = this->color(startCP->value());
      // paint
      painter->fillRect(itemRect, valueColor);
      // draw the last item
      if (i == count -1)
        {
        //pos
        itemRect = QRectF(end, 0, this->rect().width(), 
                          this->rect().height());
        // color
        valueColor = this->color(endCP->value());
        // paint
        painter->fillRect(itemRect, valueColor);
        }
      }
    else if (dynamic_cast<ctkNonLinearControlPoint*>(startCP) != 0)
      {
      QList<ctkPoint> points = this->nonLinearPoints(startCP, endCP);
      for (int j = 1; j < points.count(); ++j)
        {
        // pos
        end = (points[j].X - rangeOffset) * rangeDiff;
        QRectF itemRect = QRectF(start, 0, end - start, 
                                 this->rect().height());
        if (i==1 && j == 1)
          {
          itemRect.setLeft(0.);
          }
        if ((i == count -1) && (j == points.count() -1))
          {
          itemRect.setRight(this->rect().width());
          }
        // color
        QLinearGradient gradient(start, 0, end, 0);
        gradient.setColorAt(0, this->color(points[j-1]));
        gradient.setColorAt(1, this->color(points[j]));
        // paint
        painter->fillRect(itemRect, gradient);
        start = end;
        }
      }
    else
      {
      // pos
      end = (endCP->x() - rangeOffset) * rangeDiff;
      QRectF itemRect = QRectF(start, 0, end - start, 
                               this->rect().height());
      if (i==1)
        {
        itemRect.setLeft(0.);
        }
      if (i == count -1)
        {
        itemRect.setRight(this->rect().width());
        }
      // color
      QLinearGradient gradient(start, 0, end, 0);
      gradient.setColorAt(0, this->color(startCP->value()));
      gradient.setColorAt(1, this->color(endCP->value()));
      // paint
      painter->fillRect(itemRect, gradient);
      }
    delete startCP;
    startCP = endCP;
    start = end;
    }
  if (startCP)
    {
    delete startCP;
    }
}

