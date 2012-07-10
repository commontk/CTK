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

#ifndef __ctkTransferFunctionNativeItem_h
#define __ctkTransferFunctionNativeItem_h

/// Qt includes
#include <QGraphicsObject>
#include <QColor>

/// CTK includes
#include "ctkWidgetsExport.h"
#include "ctkTransferFunctionItem.h"

class ctkTransferFunctionNativeItemPrivate;

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkTransferFunctionNativeItem: public ctkTransferFunctionItem
{
  Q_OBJECT

public:
  ctkTransferFunctionNativeItem(QGraphicsItem* parent = 0);
  ctkTransferFunctionNativeItem(ctkTransferFunction* transferFunction, 
                                QGraphicsItem* parent = 0);
  virtual ~ctkTransferFunctionNativeItem();

  virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

protected:
  QScopedPointer<ctkTransferFunctionNativeItemPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTransferFunctionNativeItem);
  Q_DISABLE_COPY(ctkTransferFunctionNativeItem);
};

#endif
