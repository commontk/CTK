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

#ifndef __ctkTransferFunctionItem_h
#define __ctkTransferFunctionItem_h

/// Qt includes
#include <QGraphicsObject>

/// CTK includes
#include "ctkWidgetsExport.h"
#include "ctkPimpl.h"
#include "ctkTransferFunction.h"

struct ctkControlPoint;
class ctkTransferFunctionItemPrivate;

/// \ingroup Widgets
/// 
/// TODO: should probably derive from QGraphicsItem or QAbstractGraphicsShapeItem
class CTK_WIDGETS_EXPORT ctkTransferFunctionItem: public QGraphicsObject
{
  Q_OBJECT
  Q_PROPERTY(QRectF rect READ rect WRITE setRect)
public:
  ctkTransferFunctionItem(QGraphicsItem* parent = 0);
  ctkTransferFunctionItem(ctkTransferFunction* transferFunction, 
                                  QGraphicsItem* parent = 0);
  virtual ~ctkTransferFunctionItem();

  Q_INVOKABLE void setTransferFunction(ctkTransferFunction* transferFunction);
  ctkTransferFunction* transferFunction()const;

  inline void setRect(qreal x, qreal y, qreal width, qreal height);

  void setRect(const QRectF& rectangle);
  QRectF rect()const;
/*
  qreal rangeXDiff();
  qreal rangeXOffSet();

  qreal rangeYDiff();
  qreal rangeYOffSet();

  QPointF transferFunction2ScreenCoordinates( qreal x, qreal y);
  QPointF screen2TransferFunctionCoordinates( qreal x, qreal y);
*/
  virtual QRectF boundingRect()const;
protected:
  //qreal y(const QVariant& value)const;
  //inline qreal y(const ctkPoint& point)const;

  QColor color(const QVariant& value)const;
  inline QColor color(const ctkPoint& point)const;

  //QList<ctkPoint> bezierParams(ctkControlPoint* start, ctkControlPoint* end)const;
  //QList<ctkPoint> nonLinearPoints(ctkControlPoint* start, ctkControlPoint* end)const;
  virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value);
protected:
  QScopedPointer<ctkTransferFunctionItemPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTransferFunctionItem);
  Q_DISABLE_COPY(ctkTransferFunctionItem);
};

//-----------------------------------------------------------------------------
void ctkTransferFunctionItem::setRect(qreal x, qreal y, qreal width, qreal height)
{
  this->setRect(QRectF(x,y,width,height));
}
/*
//-----------------------------------------------------------------------------
qreal ctkTransferFunctionItem::y(const ctkPoint& p)const
{
  return this->y(p.Value);
}

//-----------------------------------------------------------------------------
QColor ctkTransferFunctionItem::color(const ctkPoint& p)const
{
  return this->color(p.Value);
}
*/
#endif
