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
#include "ctkVTKLookupTable.h"

/// VTK includes
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class ctkVTKLookupTablePrivate
{
public:
  vtkSmartPointer<vtkLookupTable> LookupTable;
};

//-----------------------------------------------------------------------------
ctkVTKLookupTable::ctkVTKLookupTable(QObject* parentObject)
  :ctkTransferFunction(parentObject)
  , d_ptr(new ctkVTKLookupTablePrivate)
{
}

//-----------------------------------------------------------------------------
ctkVTKLookupTable::ctkVTKLookupTable(vtkLookupTable* lookupTable, 
                                     QObject* parentObject)
  :ctkTransferFunction(parentObject)
  , d_ptr(new ctkVTKLookupTablePrivate)
{
  this->setLookupTable(lookupTable);
}

//-----------------------------------------------------------------------------
ctkVTKLookupTable::~ctkVTKLookupTable()
{
}

//-----------------------------------------------------------------------------
int ctkVTKLookupTable::count()const
{
  Q_D(const ctkVTKLookupTable);
  if (d->LookupTable.GetPointer() == 0)
    {
    Q_ASSERT(d->LookupTable.GetPointer());
    return -1;
    }
  return d->LookupTable->GetNumberOfColors();
}

//-----------------------------------------------------------------------------
bool ctkVTKLookupTable::isDiscrete()const
{
  return true;
}

//-----------------------------------------------------------------------------
bool ctkVTKLookupTable::isEditable()const
{
  return true;
}

//-----------------------------------------------------------------------------
void ctkVTKLookupTable::range(qreal& minRange, qreal& maxRange)const
{
  Q_D(const ctkVTKLookupTable);
  if (d->LookupTable.GetPointer() == 0)
    {
    Q_ASSERT(d->LookupTable.GetPointer());
    minRange = 1.; // set incorrect values
    maxRange = 0.;
    return;
    }
  double rangeValues[2];
  d->LookupTable->GetTableRange(rangeValues);
  minRange = rangeValues[0];
  maxRange = rangeValues[1];
}

//-----------------------------------------------------------------------------
QVariant ctkVTKLookupTable::minValue()const
{
  Q_D(const ctkVTKLookupTable);
  if (d->LookupTable.GetPointer() == 0)
    {
    Q_ASSERT(d->LookupTable.GetPointer());
    return QColor();
    }
  QColor minValue = QColor::fromHsvF(
    d->LookupTable->GetHueRange()[0],
    d->LookupTable->GetSaturationRange()[0],
    d->LookupTable->GetValueRange()[0],
    d->LookupTable->GetAlphaRange()[0]);
  return minValue;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKLookupTable::maxValue()const
{
  Q_D(const ctkVTKLookupTable);
  if (d->LookupTable.GetPointer() == 0)
    {
    Q_ASSERT(d->LookupTable.GetPointer());
    return QColor();
    }
  QColor maxValue = QColor::fromHsvF(
    d->LookupTable->GetHueRange()[1],
    d->LookupTable->GetSaturationRange()[1],
    d->LookupTable->GetValueRange()[1],
    d->LookupTable->GetAlphaRange()[1]);
  return maxValue;
}

//-----------------------------------------------------------------------------
qreal ctkVTKLookupTable::indexToPos(int index)const
{
  Q_D(const ctkVTKLookupTable);
  double* range = d->LookupTable->GetRange();
  return range[0] + index * ((range[1] - range[0]) / (d->LookupTable->GetNumberOfColors() - 1));
}

//-----------------------------------------------------------------------------
int ctkVTKLookupTable::posToIndex(qreal pos)const
{
  Q_D(const ctkVTKLookupTable);
  double* range = d->LookupTable->GetRange();
  return (pos - range[0]) / ((range[1] - range[0]) / (d->LookupTable->GetNumberOfColors() - 1));
}

//-----------------------------------------------------------------------------
ctkControlPoint* ctkVTKLookupTable::controlPoint(int index)const
{
  ctkControlPoint* cp = new ctkControlPoint();
  cp->P.X = this->indexToPos(index);
  cp->P.Value = this->value(cp->P.X);
  return cp;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKLookupTable::value(qreal pos)const
{
  Q_D(const ctkVTKLookupTable);
  Q_ASSERT(d->LookupTable.GetPointer());
  double rgb[3];
  d->LookupTable->GetColor(pos, rgb);
  double alpha = d->LookupTable->GetOpacity(pos);
  return QColor::fromRgbF(rgb[0], rgb[1], rgb[2], alpha);
}

//-----------------------------------------------------------------------------
int ctkVTKLookupTable::insertControlPoint(const ctkControlPoint& cp)
{
  Q_UNUSED(cp);
  //Q_D(ctkVTKLookupTable);
  qDebug() << "ctkVTKLookupTable doesn't support insertControlPoint";
  return -1;
}

//-----------------------------------------------------------------------------
// insert point with value = 0
int ctkVTKLookupTable::insertControlPoint(qreal pos)
{
  Q_UNUSED(pos);
  // nothing
  int index = 0;

  return index;
}
//-----------------------------------------------------------------------------
void ctkVTKLookupTable::setControlPointPos(int index, qreal pos)
{
  Q_UNUSED(index);
  Q_UNUSED(pos);
  //Q_D(ctkVTKLookupTable);
  // TODO, inform that nothing is done here.
  qDebug() << "ctkVTKLookupTable doesn't support setControlPointPos";
  return;
}

//-----------------------------------------------------------------------------
void ctkVTKLookupTable::setControlPointValue(int index, const QVariant& value)
{
  Q_D(ctkVTKLookupTable);
  Q_ASSERT(value.value<QColor>().isValid());
  QColor rgba = value.value<QColor>();
  d->LookupTable->SetTableValue(index, rgba.redF(), rgba.greenF(), rgba.blueF(), rgba.alphaF());
}

//-----------------------------------------------------------------------------
void ctkVTKLookupTable::setLookupTable(vtkLookupTable* lookupTable)
{
  Q_D(ctkVTKLookupTable);
  d->LookupTable = lookupTable;
  this->qvtkReconnect(d->LookupTable,vtkCommand::ModifiedEvent,
                      this, SIGNAL(changed()));
  emit changed();
}

//-----------------------------------------------------------------------------
vtkLookupTable* ctkVTKLookupTable::lookupTable()const
{
  Q_D(const ctkVTKLookupTable);
  return d->LookupTable;
}

//-----------------------------------------------------------------------------
void ctkVTKLookupTable::removeControlPoint( qreal pos )
{
  Q_UNUSED(pos);
  // TO BE IMPLEMENTED
}
