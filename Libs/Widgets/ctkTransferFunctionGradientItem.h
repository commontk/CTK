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

#ifndef __ctkTransferFunctionGradientItem_h
#define __ctkTransferFunctionGradientItem_h

/// Qt includes
#include <QGraphicsObject>

/// CTK includes
#include "CTKWidgetsExport.h"
#include "ctkPimpl.h"
#include "ctkTransferFunctionItem.h"

//class ctkTransferFunctionGradientItemPrivate;

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

#endif
