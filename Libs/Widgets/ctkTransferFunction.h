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

#ifndef __ctkTransferFunction_h
#define __ctkTransferFunction_h

/// Qt includes
#include <QObject>
#include <QtGlobal>
#include <QSharedPointer>
#include <QVariant>

/// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"

class ctkTransferFunctionPrivate;
class ctkTransferFunctionRepresentation;

//-----------------------------------------------------------------------------
/// \ingroup Widgets
struct CTK_WIDGETS_EXPORT ctkPoint
{
  ctkPoint()
    :X(0),Value(0)
  {
  }
  ctkPoint(qreal x, const QVariant& v)
    :X(x),Value(v)
  {
  }
  qreal X;
  // QColor or qreal.
  QVariant Value;
};

//-----------------------------------------------------------------------------
/// \ingroup Widgets
/// assumes the mapping is linear by default, if not, then subclasses must be
/// used
struct CTK_WIDGETS_EXPORT ctkControlPoint
{
  virtual ~ctkControlPoint();
  inline const qreal& x()const {return this->P.X;}
  inline const QVariant& value()const {return this->P.Value;}
  ctkPoint P;
};

//-----------------------------------------------------------------------------
/// \ingroup Widgets
struct CTK_WIDGETS_EXPORT ctkBezierControlPoint : public ctkControlPoint
{
  virtual ~ctkBezierControlPoint();
  ctkPoint P1;
  ctkPoint P2;
};

//-----------------------------------------------------------------------------
/// \ingroup Widgets
struct CTK_WIDGETS_EXPORT ctkNonLinearControlPoint : public ctkControlPoint
{
  virtual ~ctkNonLinearControlPoint();
  QList<ctkPoint> SubPoints;
};

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkTransferFunction: public QObject
{
  Q_OBJECT
public:
  ctkTransferFunction(QObject* parent = 0);
  virtual ~ctkTransferFunction();

  virtual ctkControlPoint* controlPoint(int index)const = 0;
  inline QVariant value(int index)const;
  virtual QVariant value(qreal pos)const = 0;

  virtual int count()const = 0;
  virtual bool isDiscrete()const = 0;
  virtual bool isEditable()const = 0;

  inline void range(qreal rangeValues[2])const;
  virtual void range(qreal& minRange, qreal& maxRange)const=0;
  virtual QVariant minValue()const = 0;
  virtual QVariant maxValue()const = 0;
  inline void valueRange(QVariant range[2])const;
  ///
  virtual int insertControlPoint(const ctkControlPoint& cp) = 0;
  virtual int insertControlPoint(qreal pos) = 0;

  virtual void removeControlPoint( qreal pos ) = 0;

  ///
  /// be careful with it, as changing the value might require
  /// more changes to ctkControlPoint.
  virtual void setControlPointPos(int index, qreal pos)=0;
  ///
  /// be careful with it, as changing the value might require
  /// more changes to ctkControlPoint.
  virtual void setControlPointValue(int index, const QVariant& value)=0;

  Q_INVOKABLE ctkTransferFunctionRepresentation* representation()const;
Q_SIGNALS:
  void changed();
protected:
  QScopedPointer<ctkTransferFunctionPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTransferFunction);
  Q_DISABLE_COPY(ctkTransferFunction);
};

//-----------------------------------------------------------------------------
QVariant ctkTransferFunction::value(int index)const
{
  QSharedPointer<ctkControlPoint> cp(this->controlPoint(index));
  return cp->P.Value;
}

//-----------------------------------------------------------------------------
void ctkTransferFunction::range(qreal rangeValues[2])const
{
  this->range(rangeValues[0], rangeValues[1]);
}

//-----------------------------------------------------------------------------
void ctkTransferFunction::valueRange(QVariant rangeValues[2])const
{
  rangeValues[0] = this->minValue();
  rangeValues[1] = this->maxValue();
}

#endif
