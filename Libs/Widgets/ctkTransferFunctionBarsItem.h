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

#ifndef __ctkTransferFunctionBarsItem_h
#define __ctkTransferFunctionBarsItem_h

/// Qt includes
#include <QGraphicsObject>

/// CTK includes
#include "ctkPimpl.h"
#include "ctkTransferFunctionItem.h"
#include "ctkWidgetsExport.h"

class ctkTransferFunction;
class ctkTransferFunctionBarsItemPrivate;

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkTransferFunctionBarsItem: public ctkTransferFunctionItem
{
  Q_OBJECT
  Q_ENUMS(LogMode)
  Q_PROPERTY(qreal barWidth READ barWidth WRITE setBarWidth)
  Q_PROPERTY(QColor barColor READ barColor WRITE setBarColor)
  Q_PROPERTY(LogMode logMode READ logMode WRITE setLogMode)
public:
  ctkTransferFunctionBarsItem(QGraphicsItem* parent = 0);
  ctkTransferFunctionBarsItem(ctkTransferFunction* transferFunc,
                              QGraphicsItem* parent = 0);
  virtual ~ctkTransferFunctionBarsItem();

  void setBarWidth(qreal newBarWidth);
  qreal barWidth()const;

  void setBarColor(const QColor& newBarColor);
  QColor barColor()const;
  
  enum LogMode
  {
    NoLog = 0,
    UseLog = 1,
    AutoLog =2
  };
  LogMode logMode() const;
  void setLogMode(const LogMode logMode);

  virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
protected:
  QScopedPointer<ctkTransferFunctionBarsItemPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTransferFunctionBarsItem);
  Q_DISABLE_COPY(ctkTransferFunctionBarsItem);
};

#endif
