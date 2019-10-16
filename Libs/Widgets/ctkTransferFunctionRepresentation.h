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

#ifndef __ctkTransferFunctionRepresentation_h
#define __ctkTransferFunctionRepresentation_h

/// Qt includes
#include <QColor>
#include <QGradient>
#include <QObject>
#include <QPainterPath>

/// CTK includes
#include "ctkPimpl.h"
#include "ctkTransferFunction.h"
#include "ctkWidgetsExport.h"

class ctkTransferFunction;
class ctkTransferFunctionRepresentationPrivate;

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkTransferFunctionRepresentation: public QObject
{
  Q_OBJECT
  Q_PROPERTY(QColor verticalGradientColor READ verticalGradientColor WRITE setVerticalGradientColor)

public:
  /// Construct a representation with no transfer function.
  ctkTransferFunctionRepresentation(QObject* parent = 0);
  /// Construct a representation with transfer function.
  ctkTransferFunctionRepresentation(ctkTransferFunction* transferFunction, QObject* parent = 0);
  virtual ~ctkTransferFunctionRepresentation();

  void setTransferFunction(ctkTransferFunction* transferFunction);
  ctkTransferFunction* transferFunction()const;

  inline qreal posX(const ctkControlPoint* cp)const;
  inline qreal posY(const ctkControlPoint* cp)const;
  inline QColor color(const ctkControlPoint* cp) const;

  inline qreal posX(const ctkPoint& point)const;
  inline qreal posY(const ctkPoint& point)const;
  inline QColor color(const ctkPoint& point) const;

  qreal posX(const qreal& tfX)const;
  qreal posY(const QVariant& tfV)const;
  QColor color(const QVariant& tfV) const;

  QPointF mapPointToScene(const ctkControlPoint* cp)const;
  QPointF mapPointToScene(const ctkPoint& point)const;

  Q_INVOKABLE qreal mapXToScene(qreal posX)const;
  Q_INVOKABLE qreal mapYToScene(qreal posY)const;
  Q_INVOKABLE qreal mapXFromScene(qreal ScenePosX)const;
  Q_INVOKABLE qreal mapYFromScene(qreal ScenePosY)const;
  inline QPointF mapPointFromScene(const QPointF& point)const;

  QList<ctkPoint> bezierParams(ctkControlPoint* start, ctkControlPoint* end) const;
  QList<ctkPoint> nonLinearPoints(ctkControlPoint* start, ctkControlPoint* end) const;

  const QPainterPath& curve()const;
  const QList<QPointF>& points()const;
  const QGradient& gradient()const;

  void computeCurve();
  void computeGradient();

  QColor verticalGradientColor()const;
  void setVerticalGradientColor(QColor verticalGradientColor);

protected Q_SLOTS:
  virtual void onTransferFunctionChanged();

protected:
  qreal computeRangeXDiff(const QRectF& rect, qreal rangeX[2]);
  qreal computeRangeXOffset(qreal rangeX[2]);
  qreal computeRangeYDiff(const QRectF& rect, const QVariant rangeY[2]);
  qreal computeRangeYOffset(const QVariant rangeY[2]);
protected:
  QScopedPointer<ctkTransferFunctionRepresentationPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTransferFunctionRepresentation);
  Q_DISABLE_COPY(ctkTransferFunctionRepresentation);
};

qreal ctkTransferFunctionRepresentation::posX(const ctkControlPoint* cp)const
{
  return this->posX(cp->x());
}
qreal ctkTransferFunctionRepresentation::posY(const ctkControlPoint* cp)const
{
  return this->posY(cp->value());
}
QColor ctkTransferFunctionRepresentation::color(const ctkControlPoint* cp) const
{
  return this->color(cp->value());
}

qreal ctkTransferFunctionRepresentation::posX(const ctkPoint& point)const
{
  return this->posX(point.X);
}
qreal ctkTransferFunctionRepresentation::posY(const ctkPoint& point)const
{
  return this->posY(point.Value);
}
QColor ctkTransferFunctionRepresentation::color(const ctkPoint& point) const
{
  return this->color(point.Value);
}

QPointF ctkTransferFunctionRepresentation::mapPointFromScene(const QPointF& point)const
{
  return QPointF(this->mapXFromScene(point.x()),
                 this->mapYFromScene(point.y()));
}

#endif
