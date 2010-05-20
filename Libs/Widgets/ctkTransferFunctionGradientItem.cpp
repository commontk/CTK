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
#include "ctkTransferFunctionScene.h"

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
  ctkTransferFunctionScene* tfScene = dynamic_cast<ctkTransferFunctionScene*>(this->scene());
  Q_ASSERT(tfScene);
  
  const QGradient& gradient = tfScene->gradient();
  painter->fillRect(this->rect(), gradient);
}

