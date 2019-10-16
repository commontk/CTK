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
#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QtGlobal>
#include <QVariant>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionBarsItem.h"
#include "ctkTransferFunctionRepresentation.h"
#include "ctkTransferFunctionScene.h"

// std includes
#include <cmath>

//-----------------------------------------------------------------------------
class ctkTransferFunctionBarsItemPrivate
{
  Q_DECLARE_PUBLIC(ctkTransferFunctionBarsItem);
protected:
  ctkTransferFunctionBarsItem* const q_ptr;

public:
  ctkTransferFunctionBarsItemPrivate(ctkTransferFunctionBarsItem& object);

  QPainterPath createBarsPath(ctkTransferFunction* tf, const QList<QPointF>& points, qreal barWidth, bool useLog, const QRectF& rect);
  QPainterPath createAreaPath(ctkTransferFunction* tf, const QList<QPointF>& points, qreal barWidth, bool useLog, const QRectF& rect);
  qreal barWidth()const;
  bool useLog()const;

  qreal  BarWidthRatio;
  QColor BarColor;
  ctkTransferFunctionBarsItem::LogMode   LogMode;
};

//-----------------------------------------------------------------------------
ctkTransferFunctionBarsItemPrivate::ctkTransferFunctionBarsItemPrivate(ctkTransferFunctionBarsItem& object)
  :q_ptr(&object)
{
  this->BarWidthRatio = 0.6180; // golden ratio... why not.
  this->BarColor = QColor(191, 191, 191, 127);
  this->BarColor = QApplication::palette().color(QPalette::Normal, QPalette::Highlight);
  this->BarColor.setAlphaF(0.2);
  this->LogMode = ctkTransferFunctionBarsItem::AutoLog;
}

//-----------------------------------------------------------------------------
ctkTransferFunctionBarsItem::ctkTransferFunctionBarsItem(QGraphicsItem* parentGraphicsItem)
  :ctkTransferFunctionItem(parentGraphicsItem)
  , d_ptr(new ctkTransferFunctionBarsItemPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkTransferFunctionBarsItem::ctkTransferFunctionBarsItem(
  ctkTransferFunction* transferFunc, QGraphicsItem* parentItem)
  :ctkTransferFunctionItem(transferFunc, parentItem)
  , d_ptr(new ctkTransferFunctionBarsItemPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkTransferFunctionBarsItem::~ctkTransferFunctionBarsItem()
{
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionBarsItem::setBarWidth(qreal newBarWidthRatio)
{
  Q_D(ctkTransferFunctionBarsItem);
  newBarWidthRatio = qBound(static_cast<qreal>(0.), newBarWidthRatio, static_cast<qreal>(1.));
  if (d->BarWidthRatio == newBarWidthRatio)
    {
    return;
    }
  d->BarWidthRatio = newBarWidthRatio;
  this->update();
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionBarsItem::barWidth()const
{
  Q_D(const ctkTransferFunctionBarsItem);
  return d->BarWidthRatio;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionBarsItem::setBarColor(const QColor& color)
{
  Q_D(ctkTransferFunctionBarsItem);
  d->BarColor = color;
}

//-----------------------------------------------------------------------------
QColor ctkTransferFunctionBarsItem::barColor()const
{
  Q_D(const ctkTransferFunctionBarsItem);
  return d->BarColor;
}

//-----------------------------------------------------------------------------
ctkTransferFunctionBarsItem::LogMode ctkTransferFunctionBarsItem::logMode() const
{
  Q_D(const ctkTransferFunctionBarsItem);
  return d->LogMode;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionBarsItem::setLogMode(const LogMode logMode)
{
  Q_D(ctkTransferFunctionBarsItem);
  d->LogMode = logMode;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionBarsItem::paint(
  QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  Q_D(ctkTransferFunctionBarsItem);
  Q_UNUSED(option);
  Q_UNUSED(widget);

  // setup colors 
  QColor penColor = d->BarColor;
  penColor.setAlphaF(1.);
  QPen pen;
  if (penColor == QApplication::palette().color(QPalette::Normal, QPalette::Highlight))
    {
    pen = QPen(penColor, 1);
    }
  else
    {
    pen = QPen(QColor(255, 255, 255, 191), 1);
    }
  pen.setCosmetic(true);

  // setup drawing
  ctkTransferFunction* tf = this->transferFunction();
  if (tf == 0 || tf->count() < 1)
    {
    return;
    }

  Q_ASSERT(tf->representation());
  const QList<QPointF>& points = tf->representation()->points();

  qreal barWidth = d->barWidth(); 
  bool useLog = d->useLog();

  QPainterPath bars;
  if (qFuzzyCompare(d->BarWidthRatio, 1.))
    {
    bars = d->createAreaPath(tf, points, barWidth, useLog, this->rect());
    pen.setWidth(2);
    }
  else
    {
    bars = d->createBarsPath(tf, points, barWidth, useLog, this->rect());
    }
  
  painter->setPen(pen);
  painter->setBrush(QBrush(d->BarColor));
  painter->drawPath(bars);
}

//-----------------------------------------------------------------------------
qreal ctkTransferFunctionBarsItemPrivate::barWidth()const
{
  Q_Q(const ctkTransferFunctionBarsItem);
  ctkTransferFunction* tf = q->transferFunction();
  Q_ASSERT(tf);
  return this->BarWidthRatio * (q->rect().width() / (tf->representation()->points().size() - 1));
}

//-----------------------------------------------------------------------------
bool ctkTransferFunctionBarsItemPrivate::useLog()const
{
  Q_Q(const ctkTransferFunctionBarsItem);
  ctkTransferFunction* tf = q->transferFunction();
  Q_ASSERT(tf);

  bool useLog = false;
  switch (this->LogMode)
    {
    case ctkTransferFunctionBarsItem::AutoLog:
      useLog = tf->maxValue().toReal() - tf->minValue().toReal() > 1000.;
      break;
    case ctkTransferFunctionBarsItem::UseLog:
      useLog = true;
      break;
    default:
    case ctkTransferFunctionBarsItem::NoLog:
      useLog = false;
    }
  return useLog;
}

//-----------------------------------------------------------------------------
QPainterPath ctkTransferFunctionBarsItemPrivate::createBarsPath(ctkTransferFunction* tf, const QList<QPointF>& points, qreal barWidth, bool useLog, const QRectF& rect)
{
  ctkTransferFunctionRepresentation* tfRep = tf->representation();
  Q_ASSERT(tfRep);

  QPainterPath bars;
  foreach(const QPointF& point, points)
    {
    qreal barHeight = point.y();
    if (useLog && barHeight != 1.)
      {
      barHeight = rect.height() - log( tfRep->mapYFromScene(barHeight) )/log(tf->maxValue().toReal());
      }
    bars.addRect(point.x() - barWidth/2, rect.height(),
                 barWidth, barHeight - rect.height() );
    }
  return bars;
}

//-----------------------------------------------------------------------------
QPainterPath ctkTransferFunctionBarsItemPrivate::createAreaPath(ctkTransferFunction* tf, const QList<QPointF>& points, qreal barWidth, bool useLog, const QRectF& rect)
{
  ctkTransferFunctionRepresentation* tfRep = tf->representation();
  Q_ASSERT(tfRep);

  QPainterPath bars;
  // 0.001 is here to ensure the outer border is not displayed on the screen
  bars.moveTo(-barWidth/2, rect.height() + 0.1);
  foreach(const QPointF& point, points)
    {
    qreal barHeight = point.y();
    if (useLog && barHeight != 1.)
      {
      barHeight = rect.height() - log( tfRep->mapYFromScene(barHeight) )/log(tf->maxValue().toReal());
      }
    bars.lineTo(point.x() - barWidth/2, barHeight);
    bars.lineTo(point.x() + barWidth/2, barHeight);
    }
  // close the path ?
  bars.lineTo(rect.width() + barWidth/2, rect.height() + 0.1);
  bars.lineTo(-barWidth/2, rect.height()  + 0.1);
  return bars;
}
