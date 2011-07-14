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
#include "ctkVTKColorTransferFunction.h"
#include "ctkLogger.h"

/// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.visualization.core.ctkVTKColorTransferFunction");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkVTKColorTransferFunctionPrivate
{
public:
  vtkSmartPointer<vtkColorTransferFunction> ColorTransferFunction;
};

//-----------------------------------------------------------------------------
ctkVTKColorTransferFunction::ctkVTKColorTransferFunction(QObject* parentObject)
  :ctkTransferFunction(parentObject)
  , d_ptr(new ctkVTKColorTransferFunctionPrivate)
{
}

//-----------------------------------------------------------------------------
ctkVTKColorTransferFunction::ctkVTKColorTransferFunction(vtkColorTransferFunction* colorTransferFunction, 
                                                         QObject* parentObject)
  :ctkTransferFunction(parentObject)
  , d_ptr(new ctkVTKColorTransferFunctionPrivate)
{
  this->setColorTransferFunction(colorTransferFunction);
}

//-----------------------------------------------------------------------------
ctkVTKColorTransferFunction::~ctkVTKColorTransferFunction()
{
}

//-----------------------------------------------------------------------------
int ctkVTKColorTransferFunction::count()const
{
  Q_D(const ctkVTKColorTransferFunction);
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    return -1;
    }
  return d->ColorTransferFunction->GetSize();
}

//-----------------------------------------------------------------------------
bool ctkVTKColorTransferFunction::isDiscrete()const
{
  return false;
}

//-----------------------------------------------------------------------------
bool ctkVTKColorTransferFunction::isEditable()const
{
  return true;
}

//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::range(qreal& minRange, qreal& maxRange)const
{
  Q_D(const ctkVTKColorTransferFunction);
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    //Q_ASSERT(d->ColorTransferFunction.GetPointer());
    logger.warn("no ColorTransferFunction");
    minRange = 1.;
    maxRange = 0.;
    return;
    }
  double rangeValues[2];
  d->ColorTransferFunction->GetRange(rangeValues);
  minRange = rangeValues[0];
  maxRange = rangeValues[1];
}

//-----------------------------------------------------------------------------
QVariant ctkVTKColorTransferFunction::minValue()const
{
  Q_D(const ctkVTKColorTransferFunction);
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    //Q_ASSERT(d->ColorTransferFunction.GetPointer());
    logger.warn("no ColorTransferFunction");
    return -1;
    }
  double rgb[3];
  QColor minValue = QColor::fromRgbF(1.,1.,1.);
  for (int i = 0; i < this->count(); ++i)
    {
    d->ColorTransferFunction->GetColor(i, rgb);
    Q_ASSERT(rgb[0] >= 0. && rgb[0] <= 1. &&
             rgb[1] >= 0. && rgb[1] <= 1. &&
             rgb[2] >= 0. && rgb[2] <= 1.);
    QColor color = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
    if ( qGray(color.red(), color.green(), color.blue()) <
         qGray(minValue.red(), minValue.green(), minValue.blue()))
      {
      minValue = color;
      }
    }
  return minValue;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKColorTransferFunction::maxValue()const
{
  Q_D(const ctkVTKColorTransferFunction);
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    //Q_ASSERT(d->ColorTransferFunction.GetPointer());
    logger.warn("no ColorTransferFunction");
    return -1;
    }
  double rgb[3];
  QColor minValue = QColor::fromRgbF(0.,0.,0.);
  for (int i = 0; i < this->count(); ++i)
    {
    d->ColorTransferFunction->GetColor(i, rgb);
    Q_ASSERT(rgb[0] >= 0. && rgb[0] <= 1. &&
             rgb[1] >= 0. && rgb[1] <= 1. &&
             rgb[2] >= 0. && rgb[2] <= 1.);
    QColor color = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
    if ( qGray(color.red(), color.green(), color.blue()) >
         qGray(minValue.red(), minValue.green(), minValue.blue()))
      {
      minValue = color;
      }
    }
  return minValue;
}

//-----------------------------------------------------------------------------
ctkControlPoint* ctkVTKColorTransferFunction::controlPoint(int index)const
{
  Q_D(const ctkVTKColorTransferFunction);
  Q_ASSERT(index >= 0 && index < this->count());
	double values[6];
  d->ColorTransferFunction->GetNodeValue(index, values);
  Q_ASSERT(values[0] >= d->ColorTransferFunction->GetRange()[0] &&
           values[0] <= d->ColorTransferFunction->GetRange()[1] &&
           values[1] >= 0. && values[1] <= 1. &&  // Red
           values[2] >= 0. && values[2] <= 1. &&  // Green
           values[3] >= 0. && values[3] <= 1. &&  // Blue
           values[4] >= 0. && values[4] <= 1. &&  // MidPoint
           values[5] >= 0. && values[5] <= 1.);   // Sharpness
  QColor rgb = QColor::fromRgbF(values[1], values[2], values[3]);
  if (index + 1 >= this->count())
    {
    ctkControlPoint* cp = new ctkControlPoint();
    cp->P.X = values[0];
    cp->P.Value = rgb;
    return cp;
    }
  ctkNonLinearControlPoint* cp = new ctkNonLinearControlPoint();
  cp->P.X = values[0];
  cp->P.Value = rgb;
  double nextValues[6];
  d->ColorTransferFunction->GetNodeValue(index + 1, nextValues);
  Q_ASSERT(nextValues[0] >= d->ColorTransferFunction->GetRange()[0] &&
           nextValues[0] <= d->ColorTransferFunction->GetRange()[1] &&
           nextValues[1] >= 0. && nextValues[1] <= 1. &&  // Red
           nextValues[2] >= 0. && nextValues[2] <= 1. &&  // Green
           nextValues[3] >= 0. && nextValues[3] <= 1. &&  // Blue
           nextValues[4] >= 0. && nextValues[4] <= 1. &&  // MidPoint
           nextValues[5] >= 0. && nextValues[5] <= 1.);   // Sharpness
  // Optimization: don't use SubPoints when the sharpness is 0.
  if (values[5] == 0.)
    {
    cp->SubPoints << ctkPoint(values[0], rgb);
    rgb = QColor::fromRgbF(nextValues[1], nextValues[2], nextValues[3]);
    cp->SubPoints << ctkPoint(nextValues[0], rgb);
    return cp;
    } 
  double subPoints[30];
  d->ColorTransferFunction->GetTable(cp->x(), values[0], 10, subPoints);
  qreal interval = (values[0] - cp->x()) / 9.;
  for(int i = 0; i < 10; ++i)
    {
    rgb = QColor::fromRgbF(subPoints[3*i], 
                           subPoints[3*i+1],
                           subPoints[3*i+2]);
    cp->SubPoints << ctkPoint(cp->x() + interval*i, rgb);
    }
  return cp;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKColorTransferFunction::value(qreal pos)const
{
  Q_D(const ctkVTKColorTransferFunction);
  Q_ASSERT(d->ColorTransferFunction.GetPointer());
  double rgb[3];
  d->ColorTransferFunction->GetColor(pos, rgb);
  QColor color = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
  return color;
}

//-----------------------------------------------------------------------------
int ctkVTKColorTransferFunction::insertControlPoint(const ctkControlPoint& cp)
{
  Q_D(ctkVTKColorTransferFunction);
  int index = -1;
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    return index;
    }
  QColor rgb = cp.value().value<QColor>();
  const ctkNonLinearControlPoint* nonLinearCp = dynamic_cast<const ctkNonLinearControlPoint*>(&cp);
  if (nonLinearCp)
    {
    // TODO retrieve midpoint & sharpness
    index = d->ColorTransferFunction->AddRGBPoint(
      cp.x(), rgb.redF(), rgb.greenF(), rgb.blueF());
    }
  else
    {
    index = d->ColorTransferFunction->AddRGBPoint(
      cp.x(), rgb.redF(), rgb.greenF(), rgb.blueF());
    }
  return index;
}
//-----------------------------------------------------------------------------
int ctkVTKColorTransferFunction::insertControlPoint(qreal pos)
{
  Q_D(ctkVTKColorTransferFunction);
  int index = -1;
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    return index;
    }

  // Get color at the given position
  double* rgb = d->ColorTransferFunction->GetColor( pos );

  // Add new point with the appropriate color
  index = d->ColorTransferFunction->AddRGBPoint(
    pos, rgb[0], rgb[1], rgb[2]);

  return index;
}
//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::setControlPointPos(int index, qreal pos)
{
  Q_D(ctkVTKColorTransferFunction);
  double values[6];
  d->ColorTransferFunction->GetNodeValue(index, values);
  values[0] = pos;
  // warning, a possible new range is not supported
  // SetNodeValue eventually fire the signal changed()
  d->ColorTransferFunction->SetNodeValue(index, values);
}

//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::setControlPointValue(int index, const QVariant& value)
{
  Q_D(ctkVTKColorTransferFunction);
  Q_ASSERT(value.value<QColor>().isValid());
  QColor rgb = value.value<QColor>();
  double values[6];
  d->ColorTransferFunction->GetNodeValue(index, values);
  values[1] = rgb.redF();
  values[2] = rgb.greenF();
  values[3] = rgb.blueF();
  // setNodeValue should eventually fired the signal changed()
  d->ColorTransferFunction->SetNodeValue(index, values);
}

//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction)
{
  Q_D(ctkVTKColorTransferFunction);
  d->ColorTransferFunction = colorTransferFunction;
  this->qvtkReconnect(d->ColorTransferFunction,vtkCommand::ModifiedEvent,
                      this, SIGNAL(changed()));
  emit changed();
}

//-----------------------------------------------------------------------------
vtkColorTransferFunction* ctkVTKColorTransferFunction::colorTransferFunction()const
{
  Q_D(const ctkVTKColorTransferFunction);
  return d->ColorTransferFunction;
}

//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::removeControlPoint( qreal pos )
{
  Q_D(ctkVTKColorTransferFunction);
  d->ColorTransferFunction->RemovePoint( pos );
}
