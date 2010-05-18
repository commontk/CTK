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

#ifndef __ctkTransferFunctionControlPointsItem_h
#define __ctkTransferFunctionControlPointsItem_h

/// Qt includes
#include <QGraphicsObject>

/// CTK includes
#include "CTKWidgetsExport.h"
#include "ctkPimpl.h"
#include "ctkTransferFunctionItem.h"

class ctkTransferFunctionControlPointsItemPrivate;

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
