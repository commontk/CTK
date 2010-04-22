/*=========================================================================

  Library:   ctk

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkTransferFunctionItems_h
#define __ctkTransferFunctionItems_h

/// Qt includes
#include <QGraphicsObject>

/// CTK includes
#include "CTKWidgetsExport.h"
#include "ctkPimpl.h"
#include "ctkTransferFunction.h"

class ctkControlPoint;
class ctkTransferFunctionItemPrivate;
//class ctkTransferFunctionGradientItemPrivate;
class ctkTransferFunctionControlPointsItemPrivate;

/// 
/// TODO: should probably derive from QGraphicsItem or QAbstractGraphicsShapeItem
class CTK_WIDGETS_EXPORT ctkTransferFunctionItem: public QGraphicsObject
{
  Q_OBJECT
public:
  ctkTransferFunctionItem(QGraphicsItem* parent = 0);
  ctkTransferFunctionItem(ctkTransferFunction* transferFunction, 
                                  QGraphicsItem* parent = 0);
  virtual ~ctkTransferFunctionItem();

  void setTransferFunction(ctkTransferFunction* transferFunction);
  ctkTransferFunction* transferFunction()const;

  inline void setRect(qreal x, qreal y, qreal width, qreal height);
  void setRect(const QRectF& rectangle);
  QRectF rect()const;
  virtual QRectF boundingRect()const;
protected:
  qreal y(const QVariant& value)const;
  inline qreal y(const ctkPoint& point)const;

  QColor color(const QVariant& value)const;
  inline QColor color(const ctkPoint& point)const;



  QList<ctkPoint> bezierParams(ctkControlPoint* start, ctkControlPoint* end)const;
  QList<ctkPoint> nonLinearPoints(ctkControlPoint* start, ctkControlPoint* end)const;
protected slots:
  virtual void onTransferFunctionChanged();
private:
  CTK_DECLARE_PRIVATE(ctkTransferFunctionItem);
};

void ctkTransferFunctionItem::setRect(qreal x, qreal y, qreal width, qreal height)
{
  this->setRect(QRectF(x,y,width,height));
}

qreal ctkTransferFunctionItem::y(const ctkPoint& p)const
{
  return this->y(p.Value);
}

QColor ctkTransferFunctionItem::color(const ctkPoint& p)const
{
  return this->color(p.Value);
}


//-----------------------------------------------------------------------------
class CTK_WIDGETS_EXPORT ctkTransferFunctionGradientItem: public ctkTransferFunctionItem
{
  Q_OBJECT
public:
  ctkTransferFunctionGradientItem(QGraphicsItem* parent = 0);
  ctkTransferFunctionGradientItem(ctkTransferFunction* transferFunction, 
                                  QGraphicsItem* parent = 0);
  virtual ~ctkTransferFunctionGradientItem();

  virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
};


//-----------------------------------------------------------------------------
class CTK_WIDGETS_EXPORT ctkTransferFunctionControlPointsItem: public ctkTransferFunctionItem
{
  Q_OBJECT
public:
  explicit ctkTransferFunctionControlPointsItem(QGraphicsItem* parent = 0);
  ctkTransferFunctionControlPointsItem(ctkTransferFunction* transferFunction, 
                                       QGraphicsItem* parent = 0);
  virtual ~ctkTransferFunctionControlPointsItem();

  virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* e);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
private:
  CTK_DECLARE_PRIVATE(ctkTransferFunctionControlPointsItem);
};

#endif
