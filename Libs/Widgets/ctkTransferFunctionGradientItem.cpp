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
#include "ctkTransferFunctionRepresentation.h"
#include "ctkTransferFunctionScene.h"

//-----------------------------------------------------------------------------
class ctkTransferFunctionGradientItemPrivate
{
public:
  ctkTransferFunctionGradientItemPrivate();
  bool Mask;
};

//-----------------------------------------------------------------------------
ctkTransferFunctionGradientItemPrivate::ctkTransferFunctionGradientItemPrivate()
{
  this->Mask = true;
}

//-----------------------------------------------------------------------------
ctkTransferFunctionGradientItem::ctkTransferFunctionGradientItem(QGraphicsItem* parentGraphicsItem)
  :ctkTransferFunctionItem(parentGraphicsItem)
  , d_ptr(new ctkTransferFunctionGradientItemPrivate)
{
}

//-----------------------------------------------------------------------------
ctkTransferFunctionGradientItem::ctkTransferFunctionGradientItem(
  ctkTransferFunction* transferFunction, QGraphicsItem* parentItem)
  :ctkTransferFunctionItem(transferFunction, parentItem)
  , d_ptr(new ctkTransferFunctionGradientItemPrivate)
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
  Q_UNUSED(option);
  Q_UNUSED(widget);

  if (this->transferFunction()->count() <= 0)
    {
    return;
    }
  //ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  //Q_ASSERT(tfScene);
  ctkTransferFunctionRepresentation* tfRep = this->transferFunction()->representation();

  const QGradient& gradient = tfRep->gradient();

  if ( this->mask() )
    {
    QPainterPath closedPath = tfRep->curve();
    QRectF position = this->rect();
    // link to last point
    closedPath.lineTo(position.x() + position.width(), position.y() + position.height());
    // link to first point
    closedPath.lineTo(position.x(), position.y() + position.height());
    //Don,t need to close because automatic
    QPen pen(QColor(255, 255, 255, 191), 1);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(gradient);
    painter->drawPath(closedPath);
    }
  else
    {
    painter->fillRect(this->rect(), gradient);
    }
}

//-----------------------------------------------------------------------------
bool ctkTransferFunctionGradientItem::mask() const
{
  Q_D( const ctkTransferFunctionGradientItem );
  return d->Mask;
}

//-----------------------------------------------------------------------------
void ctkTransferFunctionGradientItem::setMask( bool mask )
{
  Q_D( ctkTransferFunctionGradientItem );
  d->Mask = mask;
}
