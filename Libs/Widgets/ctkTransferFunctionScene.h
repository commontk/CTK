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

#ifndef __ctkTransferFunctionScene_h
#define __ctkTransferFunctionScene_h

/// Qt includes
#include <QGraphicsScene>

/// CTK includes
#include "CTKWidgetsExport.h"
#include "ctkPimpl.h"

class ctkTransferFunction;
class ctkTransferFunctionScenePrivate;
class ctkControlPoint;
class ctkPoint;

//-----------------------------------------------------------------------------
class CTK_WIDGETS_EXPORT ctkTransferFunctionScene: public QGraphicsScene
{
  Q_OBJECT;
public:
  ctkTransferFunctionScene(QObject* parent = 0);
  ctkTransferFunctionScene(ctkTransferFunction* transferFunction, QObject* parent = 0);
  virtual ~ctkTransferFunctionScene();

  void setTransferFunction(ctkTransferFunction* transferFunction);
  ctkTransferFunction* transferFunction()const;

  qreal y(const QVariant& v) const;
  QColor color(const QVariant& v) const;

  qreal posX(const qreal& x)const;
  qreal posY(const QVariant& value)const;
  
  QPointF mapPointToScreen(const ctkControlPoint* cp)const;
  QPointF mapPointToScreen(const ctkPoint& point)const;
 
  qreal mapXToScreen(qreal posX)const;
  qreal mapYToScreen(qreal posY)const;
  qreal mapXFromScreen(qreal screenPosX)const;
  qreal mapYFromScreen(qreal screenPosY)const;

  QList<ctkPoint> bezierParams(ctkControlPoint* start, ctkControlPoint* end) const;
  QList<ctkPoint> nonLinearPoints(ctkControlPoint* start, ctkControlPoint* end) const;

  const QPainterPath& curve()const;
  const QList<QPointF>& points()const;
  void computeCurve();

protected slots:
  virtual void onTransferFunctionChanged();

protected:
  qreal computeRangeXDiff(const QRectF& rect, qreal rangeX[2]);
  qreal computeRangeXOffset(qreal rangeX[2]);
  qreal computeRangeYDiff(const QRectF& rect, const QVariant rangeY[2]);
  qreal computeRangeYOffset(const QVariant rangeY[2]);
private:
  CTK_DECLARE_PRIVATE(ctkTransferFunctionScene);
};

#endif
