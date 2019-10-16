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

/// CTK includes
#include "ctkVTKPiecewiseFunction.h"

/// VTK includes
#include <vtkPiecewiseFunction.h>
#include <vtkSmartPointer.h>

class ctkVTKPiecewiseFunctionPrivate
{
public:
  vtkSmartPointer<vtkPiecewiseFunction> PiecewiseFunction;
};

ctkVTKPiecewiseFunction::ctkVTKPiecewiseFunction(QObject* parentObject)
  :ctkTransferFunction(parentObject)
  , d_ptr(new ctkVTKPiecewiseFunctionPrivate)
{
}

//-----------------------------------------------------------------------------
ctkVTKPiecewiseFunction::ctkVTKPiecewiseFunction(vtkPiecewiseFunction* piecewiseFunction,
                                                         QObject* parentObject)
  :ctkTransferFunction(parentObject)
  , d_ptr(new ctkVTKPiecewiseFunctionPrivate)
{
  this->setPiecewiseFunction(piecewiseFunction);
}

//-----------------------------------------------------------------------------
ctkVTKPiecewiseFunction::~ctkVTKPiecewiseFunction()
{
}

//-----------------------------------------------------------------------------
int ctkVTKPiecewiseFunction::count()const
{
  Q_D(const ctkVTKPiecewiseFunction);
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->PiecewiseFunction.GetPointer());
    return -1;
    }
  return d->PiecewiseFunction->GetSize();
}

//-----------------------------------------------------------------------------
bool ctkVTKPiecewiseFunction::isDiscrete()const
{
  return false;
}

//-----------------------------------------------------------------------------
bool ctkVTKPiecewiseFunction::isEditable()const
{
  return true;
}

//-----------------------------------------------------------------------------
void ctkVTKPiecewiseFunction::range(qreal& minRange, qreal& maxRange)const
{
  Q_D(const ctkVTKPiecewiseFunction);
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->PiecewiseFunction.GetPointer());
    minRange = 1.;
    maxRange = 0.;
    return;
    }
  double rangeValues[2];
  d->PiecewiseFunction->GetRange(rangeValues);
  minRange = rangeValues[0];
  maxRange = rangeValues[1];
}

//-----------------------------------------------------------------------------
QVariant ctkVTKPiecewiseFunction::minValue()const
{
  Q_D(const ctkVTKPiecewiseFunction);
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->PiecewiseFunction.GetPointer());
    return -1;
    }
  //Initialize to max value
  /// TODO initialize with max value
  double minValue = VTK_DOUBLE_MAX;
  for (int i = 0; i < this->count(); ++i)
    {
    double value[4];
    d->PiecewiseFunction->GetNodeValue(i, value);
    minValue = qMin(value[1], minValue);
    }
  return minValue;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKPiecewiseFunction::maxValue()const
{
  Q_D(const ctkVTKPiecewiseFunction);
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->PiecewiseFunction.GetPointer());
    return -1;
    }
  //Initialize to max value
  /// TODO initialize with max value
  qreal maxValue = VTK_DOUBLE_MIN;
  for (int i = 0; i < this->count(); ++i)
    {
    double value[4];
    d->PiecewiseFunction->GetNodeValue(i, value);
    maxValue = qMax(maxValue, value[1]);
    }
  return maxValue;
}

//-----------------------------------------------------------------------------
ctkControlPoint* ctkVTKPiecewiseFunction::controlPoint(int index)const
{
  Q_D(const ctkVTKPiecewiseFunction);
  Q_ASSERT(index >= 0 && index < this->count());

  double values[4];
#ifndef QT_NO_DEBUG
  double* range = d->PiecewiseFunction->GetRange();
#endif
  d->PiecewiseFunction->GetNodeValue(index, values);

  QVariant rangeY[2];
  rangeY[0] = this->minValue();
  rangeY[1] = this->maxValue();
  //    rangeYDiff /= rangeY[1].toReal() - rangeY[0].toReal();

#ifndef QT_NO_DEBUG
  Q_ASSERT(values[0] >= range[0] && values[0] <= range [1] &&  // X
           values[1] >= rangeY[0].toDouble() && values[1] <= rangeY[1].toDouble()  &&  // Y
           values[2] >= 0. && values[2] <= 1. &&                // Midpoint
           values[3] >= 0. && values[3] <= 1. );                // Sharpness
#endif
  if (index + 1 >= this->count())
    {
    ctkControlPoint* cp = new ctkControlPoint();
    cp->P.X = values[0];
    cp->P.Value = values[1];
    return cp;
    }
  ctkNonLinearControlPoint* cp = new ctkNonLinearControlPoint();
  cp->P.X = values[0];
  cp->P.Value = values[1];
  // Optimization: don't use Subpoints if sharpness == 0
  if (values[3] == 0.)
    {
    cp->SubPoints << ctkPoint(values[0], values[1]);
    } 
  double nextValues[4];
  d->PiecewiseFunction->GetNodeValue(index + 1, nextValues);

#ifndef QT_NO_DEBUG
  Q_ASSERT(nextValues[0] >= range[0] && nextValues[0] <= range[1] &&  // X
           nextValues[1] >= rangeY[0].toDouble() && nextValues[1] <= rangeY[1].toDouble()  &&  // Y
           nextValues[2] >= 0. && nextValues[2] <= 1. &&                // Midpoint
           nextValues[3] >= 0. && nextValues[3] <= 1. );                // Sharpness
#endif
  // Optimization: Don't use Subpoints if sharpness == 0
  if (values[3] == 0.)
    {
    cp->SubPoints << ctkPoint(nextValues[0], nextValues[1]);
    return cp;
    }
  double subPoints[100];
  d->PiecewiseFunction->GetTable(cp->x(), nextValues[0], 100, subPoints);
  qreal interval = (nextValues[0] - cp->x()) / 99.;

  // subPoints[i] since value varies (not like in color transfer function widget)
  for(int i = 0; i < 100; ++i)
    {
    cp->SubPoints << ctkPoint(cp->x() + interval*i, subPoints[i]);
    }
  return cp;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKPiecewiseFunction::value(qreal pos)const
{
  Q_D(const ctkVTKPiecewiseFunction);
  Q_ASSERT(d->PiecewiseFunction.GetPointer());
  qreal value;
  // get value for given x
  value = d->PiecewiseFunction->GetValue( pos );
  return value;
}

//-----------------------------------------------------------------------------
int ctkVTKPiecewiseFunction::insertControlPoint(const ctkControlPoint& cp)
{
  Q_D(ctkVTKPiecewiseFunction);
  int index = -1;
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    return index;
    }
  qreal value = cp.value().value<qreal>();
  const ctkNonLinearControlPoint* nonLinearCp = dynamic_cast<const ctkNonLinearControlPoint*>(&cp);
  if (nonLinearCp)
    {
    // TODO retrieve midpoint & sharpness
    index = d->PiecewiseFunction->AddPoint(
      cp.x(), value);
    }
  else
    {
    index = d->PiecewiseFunction->AddPoint(
      cp.x(), value);
    }
  return index;
}
//-----------------------------------------------------------------------------
// insert point with value = 0
int ctkVTKPiecewiseFunction::insertControlPoint(qreal pos)
{
  Q_D(ctkVTKPiecewiseFunction);
  int index = -1;
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    return index;
    }
  index = d->PiecewiseFunction->AddPoint( pos, 0);
  qDebug() << "index of new point: " << index;
  return index;
}

//-----------------------------------------------------------------------------
void ctkVTKPiecewiseFunction::setControlPointPos(int index, qreal pos)
{
  Q_D(ctkVTKPiecewiseFunction);
  double values[4];
  d->PiecewiseFunction->GetNodeValue(index, values);
  // warning, a possible new range is not supported
  // SetNodeValue eventually fire the signal changed()
  values[0] = pos;
  d->PiecewiseFunction->SetNodeValue(index, values);
}

//-----------------------------------------------------------------------------
void ctkVTKPiecewiseFunction::setControlPointValue(int index, const QVariant& value)
{
  Q_D(ctkVTKPiecewiseFunction);
  double values[4];
  d->PiecewiseFunction->GetNodeValue(index, values);
  qDebug() << "old value: " << values[1];
  values[1] = value.toDouble();
  qDebug() << "new value: " << values[1];
  // setNodeValue should eventually fired the signal changed()
  d->PiecewiseFunction->SetNodeValue(index, values);
}

//-----------------------------------------------------------------------------
void ctkVTKPiecewiseFunction::setPiecewiseFunction(vtkPiecewiseFunction* piecewiseFunction)
{
  Q_D(ctkVTKPiecewiseFunction);
  d->PiecewiseFunction = piecewiseFunction;
  this->qvtkReconnect(d->PiecewiseFunction,vtkCommand::ModifiedEvent,
                      this, SIGNAL(changed()));
  emit changed();
}

//-----------------------------------------------------------------------------
vtkPiecewiseFunction* ctkVTKPiecewiseFunction::piecewiseFunction()const
{
  Q_D(const ctkVTKPiecewiseFunction);
  return d->PiecewiseFunction;
}

//-----------------------------------------------------------------------------
void ctkVTKPiecewiseFunction::removeControlPoint( qreal pos )
{
  Q_D(ctkVTKPiecewiseFunction);
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    return;
    }
  d->PiecewiseFunction->RemovePoint( pos );
}
