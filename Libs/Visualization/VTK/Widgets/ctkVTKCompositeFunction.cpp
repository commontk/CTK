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
#include "ctkVTKCompositeFunction.h"

/// VTK includes
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

class ctkVTKCompositeFunctionPrivate
{
public:
  vtkSmartPointer<vtkPiecewiseFunction>     PiecewiseFunction;
  vtkSmartPointer<vtkColorTransferFunction> ColorTransferFunction;
};

//-----------------------------------------------------------------------------
ctkVTKCompositeFunction::ctkVTKCompositeFunction(vtkPiecewiseFunction* piecewiseFunction,
                                                 vtkColorTransferFunction* colorTransferFunction,
                                                 QObject* parentObject)
  :ctkTransferFunction(parentObject)
  , d_ptr(new ctkVTKCompositeFunctionPrivate)
{
  this->setPiecewiseFunction(piecewiseFunction);
  this->setColorTransferFunction(colorTransferFunction);
}

//-----------------------------------------------------------------------------
ctkVTKCompositeFunction::~ctkVTKCompositeFunction()
{
}

//-----------------------------------------------------------------------------
int ctkVTKCompositeFunction::count()const
{
  Q_D(const ctkVTKCompositeFunction);
  // count points from piecewise
  // could be from color transfer function
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->PiecewiseFunction.GetPointer());
    return -1;
    }

  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->ColorTransferFunction.GetPointer());
    return -1;
    }
  //qDebug() << "Piecewise: " << d->PiecewiseFunction->GetSize();
  //qDebug() << "Color Transfer: " << d->ColorTransferFunction->GetSize();

  // check if 2 tranfer function have same size
  Q_ASSERT( d->PiecewiseFunction->GetSize() == d->ColorTransferFunction->GetSize());

  return d->PiecewiseFunction->GetSize();
}

//-----------------------------------------------------------------------------
bool ctkVTKCompositeFunction::isDiscrete()const
{
  return false;
}

//-----------------------------------------------------------------------------
bool ctkVTKCompositeFunction::isEditable()const
{
  return true;
}

//-----------------------------------------------------------------------------
void ctkVTKCompositeFunction::range(qreal& minRange, qreal& maxRange)const
{
  Q_D(const ctkVTKCompositeFunction);
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
QVariant ctkVTKCompositeFunction::minValue()const
{
  Q_D(const ctkVTKCompositeFunction);
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->PiecewiseFunction.GetPointer());
    return -1;
    }
  //Initialize to max value
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
QVariant ctkVTKCompositeFunction::maxValue()const
{
  Q_D(const ctkVTKCompositeFunction);
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->PiecewiseFunction.GetPointer());
    return -1;
    }
  //Initialize to min value
  double maxValue = VTK_DOUBLE_MIN;
  for (int i = 0; i < this->count(); ++i)
    {
    double value[4];
    d->PiecewiseFunction->GetNodeValue(i, value);
    maxValue = qMax(maxValue, value[1]);
    }
  return maxValue;
}

//-----------------------------------------------------------------------------
ctkControlPoint* ctkVTKCompositeFunction::controlPoint(int index)const
{
  Q_D(const ctkVTKCompositeFunction);
  Q_ASSERT(index >= 0 && index < this->count());

  double valuesPWF[4];
  double valuesCTF[6];
#ifndef QT_NO_DEBUG
  double* rangePWF = d->PiecewiseFunction->GetRange();
  double* rangeCTF = d->ColorTransferFunction->GetRange();
#endif
  d->PiecewiseFunction->GetNodeValue(index, valuesPWF);
  d->ColorTransferFunction->GetNodeValue(index, valuesCTF);

  QVariant rangeY[2];
  rangeY[0] = this->minValue();
  rangeY[1] = this->maxValue();

  // test piecewise
#ifndef QT_NO_DEBUG
  Q_ASSERT(valuesPWF[0] >= rangePWF[0] && valuesPWF[0] <= rangePWF[1] &&  // X
    valuesPWF[1] >= rangeY[0].toDouble() && valuesPWF[1] <= rangeY[1].toDouble()  &&  // Y
    valuesPWF[2] >= 0. && valuesPWF[2] <= 1. &&                // Midpoint
    valuesPWF[3] >= 0. && valuesPWF[3] <= 1. );                // Sharpness

  // test color transfer
  Q_ASSERT(valuesCTF[0] >= rangeCTF[0] &&
    valuesCTF[0] <= rangeCTF[1] &&
    valuesCTF[1] >= 0. && valuesCTF[1] <= 1. &&  // Red
    valuesCTF[2] >= 0. && valuesCTF[2] <= 1. &&  // Green
    valuesCTF[3] >= 0. && valuesCTF[3] <= 1. &&  // Blue
    valuesCTF[4] >= 0. && valuesCTF[4] <= 1. &&  // MidPoint
    valuesCTF[5] >= 0. && valuesCTF[5] <= 1.);   // Sharpness
#endif
  // if only 2 points -> linear
  if (index + 1 >= this->count())
    {
    ctkControlPoint* cp = new ctkControlPoint();
    cp->P.X = valuesPWF[0];
    // update value of QVariant
    cp->P.Value = QColor::fromRgbF(
      valuesCTF[1], valuesCTF[2], valuesCTF[3], valuesPWF[1]);
    return cp;
    }

  //else

  ctkNonLinearControlPoint* cp = new ctkNonLinearControlPoint();
  cp->P.X = valuesPWF[0];
  // update value of QVariant
  cp->P.Value = QColor::fromRgbF(valuesCTF[1], valuesCTF[2], valuesCTF[3], valuesPWF[1]);

  double nextValuesPWF[4], nextValuesCTF[6];
  d->PiecewiseFunction->GetNodeValue(index + 1, nextValuesPWF);
  d->ColorTransferFunction->GetNodeValue(index + 1, nextValuesCTF);

#ifndef QT_NO_DEBUG
  Q_ASSERT(nextValuesPWF[0] >= rangePWF[0] && nextValuesPWF[0] <= rangePWF[1]  &&  // X
    nextValuesPWF[1] >= rangeY[0].toDouble() && nextValuesPWF[1] <= rangeY[1].toDouble()  &&  // Y
    nextValuesPWF[2] >= 0. && nextValuesPWF[2] <= 1. &&                // Midpoint
    nextValuesPWF[3] >= 0. && nextValuesPWF[3] <= 1. );                // Sharpness

  Q_ASSERT(nextValuesCTF[0] >= rangeCTF[0] &&
    nextValuesCTF[0] <= rangeCTF[1] &&
    nextValuesCTF[1] >= 0. && nextValuesCTF[1] <= 1. &&  // Red
    nextValuesCTF[2] >= 0. && nextValuesCTF[2] <= 1. &&  // Green
    nextValuesCTF[3] >= 0. && nextValuesCTF[3] <= 1. &&  // Blue
    nextValuesCTF[4] >= 0. && nextValuesCTF[4] <= 1. &&  // MidPoint
    nextValuesCTF[5] >= 0. && nextValuesCTF[5] <= 1.);   // Sharpness
#endif
  // Optimization: don't use subPoints if the ramp is linear (sharpness == 0)
  if (valuesPWF[3] == 0. && valuesCTF[5] == 0.)
    {
    cp->SubPoints << ctkPoint(valuesPWF[0], QColor::fromRgbF(
      valuesCTF[1], valuesCTF[2], valuesCTF[3], valuesPWF[1]));
    cp->SubPoints << ctkPoint(nextValuesPWF[0], QColor::fromRgbF(
      nextValuesCTF[1], nextValuesCTF[2], nextValuesCTF[3], nextValuesPWF[1]));
    return cp;
    }

  double subPointsCTF[300];
  double subPointsPWF[100];
  d->ColorTransferFunction->GetTable(cp->x(), nextValuesCTF[0], 100, subPointsCTF);
  d->PiecewiseFunction->GetTable(cp->x(), nextValuesCTF[0], 100, subPointsPWF);
  qreal interval = (nextValuesCTF[0] - cp->x()) / 99.;

  for(int i = 0; i < 100; ++i)
    {
    qreal red =  subPointsCTF[3*i];
    qreal green =  subPointsCTF[3*i+1];
    qreal blue =  subPointsCTF[3*i+2];
    qreal alpha = subPointsPWF[i];
    QColor compositeValue = QColor::fromRgbF(red, green, blue, alpha );
    cp->SubPoints << ctkPoint(cp->x() + interval*i, compositeValue);
    }
  return cp;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKCompositeFunction::value(qreal pos)const
{
  Q_D(const ctkVTKCompositeFunction);
  Q_ASSERT(d->PiecewiseFunction.GetPointer());
  Q_ASSERT(d->ColorTransferFunction.GetPointer());

  // Get color
  double rgb[3];
  d->ColorTransferFunction->GetColor(pos, rgb);

  // Get Alpha
  qreal alpha;
  alpha = d->PiecewiseFunction->GetValue( pos );

  // returns RGBA
  QColor compositeValue(rgb[0], rgb[1], rgb[2], alpha);
  return compositeValue;
}

//-----------------------------------------------------------------------------
int ctkVTKCompositeFunction::insertControlPoint(const ctkControlPoint& cp)
{
  Q_UNUSED(cp);
  Q_D(ctkVTKCompositeFunction);
  int index = -1;
  // check piecewise
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    return index;
    }
  // cp: x
  // value = rgba
  /*
  // check color tf
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
    }*/
  return index;
}
//-----------------------------------------------------------------------------
int ctkVTKCompositeFunction::insertControlPoint(qreal pos)
{
  Q_D(ctkVTKCompositeFunction);
  int index = -1;
  // check piecewise
  if (d->PiecewiseFunction.GetPointer() == 0)
    {
    return index;
    }
  //check color tf
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    return index;
    }

  // Add color to CTF
  double color[3];
  d->ColorTransferFunction->GetColor( pos, color );
  int indexColor =
      d->ColorTransferFunction->AddRGBPoint( pos, color[0], color[1], color[2] );

  // Add point to piecewise
#ifndef QT_NO_DEBUG
  int indexPiecewise =
#endif
      d->PiecewiseFunction->AddPoint( pos, 0);

  // check index
#ifndef QT_NO_DEBUG
  Q_ASSERT(indexColor == indexPiecewise);
#endif

  index = indexColor;

  return index;
}

//-----------------------------------------------------------------------------
void ctkVTKCompositeFunction::setControlPointPos(int index, qreal pos)
{
  Q_D(ctkVTKCompositeFunction);

  // update X pos in the CTF
  double valuesColor[6];
  d->ColorTransferFunction->GetNodeValue(index, valuesColor);
  valuesColor[0] = pos;
  // warning, a possible new range is not supported
  // SetNodeValue eventually fire the signal changed()
  d->ColorTransferFunction->SetNodeValue(index, valuesColor);

  // Update X pos in the PWF
  double valuesPiecewise[4];
  d->PiecewiseFunction->GetNodeValue(index, valuesPiecewise);
  // warning, a possible new range is not supported
  // SetNodeValue eventually fire the signal changed()
  valuesPiecewise[0] = pos;
  d->PiecewiseFunction->SetNodeValue(index, valuesPiecewise);
}

//-----------------------------------------------------------------------------
void ctkVTKCompositeFunction::setControlPointValue(int index, const QVariant& value)
{
  Q_D(ctkVTKCompositeFunction);
  // QVariant = RGBA

  double values[4];
  d->PiecewiseFunction->GetNodeValue(index, values);
  values[1] = value.toDouble();
  // setNodeValue should eventually fired the signal changed()
  d->PiecewiseFunction->SetNodeValue(index, values);
}

//-----------------------------------------------------------------------------
void ctkVTKCompositeFunction::setPiecewiseFunction(vtkPiecewiseFunction* piecewiseFunction)
{
  Q_D(ctkVTKCompositeFunction);
  d->PiecewiseFunction = piecewiseFunction;
  this->qvtkReconnect(d->PiecewiseFunction,vtkCommand::ModifiedEvent,
                      this, SIGNAL(changed()));
  emit changed();
}
//-----------------------------------------------------------------------------
void ctkVTKCompositeFunction::setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction)
{
  Q_D(ctkVTKCompositeFunction);
  d->ColorTransferFunction = colorTransferFunction;
  this->qvtkReconnect(d->ColorTransferFunction,vtkCommand::ModifiedEvent,
                      this, SIGNAL(changed()));
  emit changed();
}

//-----------------------------------------------------------------------------
vtkPiecewiseFunction* ctkVTKCompositeFunction::piecewiseFunction()const
{
  Q_D(const ctkVTKCompositeFunction);
  return d->PiecewiseFunction;
}

//-----------------------------------------------------------------------------
vtkColorTransferFunction* ctkVTKCompositeFunction::colorTransferFunction()const
{
  Q_D(const ctkVTKCompositeFunction);
  return d->ColorTransferFunction;
}

//-----------------------------------------------------------------------------
void ctkVTKCompositeFunction::removeControlPoint( qreal pos )
{
  Q_UNUSED(pos);
  // TO BE IMPLEMENTED
}
