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

#ifndef __ctkHistogram_h
#define __ctkHistogram_h

/// Qt includes
#include <QColor>
#include <QObject>
#include <QtGlobal>
#include <QSharedPointer>
#include <QVariant>

/// CTK includes
#include "ctkTransferFunction.h"
#include "ctkWidgetsExport.h"

//-----------------------------------------------------------------------------
/// \ingroup Widgets
struct CTK_WIDGETS_EXPORT ctkHistogramBar : public ctkControlPoint
{
  QColor Color;
};

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkHistogram: public ctkTransferFunction
{
  Q_OBJECT
public:
  ctkHistogram(QObject* parent = 0);
  virtual ~ctkHistogram();
  
  virtual bool isDiscrete()const;
  virtual bool isEditable()const;

  ///
  virtual int insertControlPoint(const ctkControlPoint& cp);
  virtual int insertControlPoint(qreal pos);

  /// 
  /// be careful with it, as changing the value might require
  /// more changes to ctkControlPoint.
  virtual void setControlPointPos(int index, qreal pos);
  /// 
  /// be careful with it, as changing the value might require
  /// more changes to ctkControlPoint.
  virtual void setControlPointValue(int index, const QVariant& value);
  virtual void build()=0;
protected:
  
};

#endif
