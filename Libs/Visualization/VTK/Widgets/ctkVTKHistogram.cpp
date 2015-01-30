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
#include "ctkVTKHistogram.h"
#include "ctkLogger.h"

/// VTK includes
#include <vtkDataArray.h>
#include <vtkIntArray.h>
#include <vtkMath.h>
#include <vtkSmartPointer.h>

/// STL include
#include <limits>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.visualization.core.ctkVTKHistogram");
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkVTKHistogramPrivate
{
public:
  ctkVTKHistogramPrivate();
  vtkSmartPointer<vtkDataArray> DataArray;
  vtkSmartPointer<vtkIntArray>  Bins;
  int                           UserNumberOfBins;
  int                           Component;
  mutable double                Range[2];
  int                           MinBin;
  int                           MaxBin;

  int computeNumberOfBins()const;
};

//-----------------------------------------------------------------------------
ctkVTKHistogramPrivate::ctkVTKHistogramPrivate()
{
  this->Bins = vtkSmartPointer<vtkIntArray>::New();
  this->UserNumberOfBins = -1;
  this->Component = 0;
  this->Range[0] = this->Range[1] = 0.;
  this->MinBin = 0;
  this->MaxBin = 0;
}

//-----------------------------------------------------------------------------
int ctkVTKHistogramPrivate::computeNumberOfBins()const
{
  if (this->DataArray.GetPointer() == 0)
    {
    return -1;
    }
  
  if (this->DataArray->GetDataType() == VTK_CHAR ||
      this->DataArray->GetDataType() == VTK_SIGNED_CHAR ||
      this->DataArray->GetDataType() == VTK_UNSIGNED_CHAR)
    {
    this->Range[0] = this->DataArray->GetDataTypeMin();
    this->Range[1] = this->DataArray->GetDataTypeMax();
    }
  else
    {
    this->DataArray->GetRange(this->Range, this->Component);
    if (this->DataArray->GetDataType() == VTK_FLOAT ||
        this->DataArray->GetDataType() == VTK_DOUBLE)
      {
      this->Range[1] += 0.01;
      }
    //else
    //  {
    //  this->Range[1] += 1;
    //  }
    }
  if (this->UserNumberOfBins > 0)
    {
    return this->UserNumberOfBins;
    }

  return static_cast<int>(this->Range[1] - this->Range[0]) + 1;
}

//-----------------------------------------------------------------------------
ctkVTKHistogram::ctkVTKHistogram(QObject* parentObject)
  :ctkHistogram(parentObject)
  , d_ptr(new ctkVTKHistogramPrivate)
{
}

//-----------------------------------------------------------------------------
ctkVTKHistogram::ctkVTKHistogram(vtkDataArray* dataArray, 
                                 QObject* parentObject)
  :ctkHistogram(parentObject)
  , d_ptr(new ctkVTKHistogramPrivate)
{
  this->setDataArray(dataArray);
}

//-----------------------------------------------------------------------------
ctkVTKHistogram::~ctkVTKHistogram()
{
}

//-----------------------------------------------------------------------------
int ctkVTKHistogram::count()const
{
  Q_D(const ctkVTKHistogram);
  return d->Bins->GetNumberOfTuples();
}

//-----------------------------------------------------------------------------
void ctkVTKHistogram::range(qreal& minRange, qreal& maxRange)const
{
  Q_D(const ctkVTKHistogram);
  if (d->DataArray.GetPointer() == 0)
    {
    //Q_ASSERT(d->DataArray.GetPointer());
    logger.warn("no dataArray");
    minRange = 1.; // set incorrect values
    maxRange = 0.;
    return;
    }
  if (d->Range[0] == d->Range[1])
    {
    minRange = d->DataArray->GetDataTypeMin();
    maxRange = d->DataArray->GetDataTypeMax();
    return;
    }
  minRange = d->Range[0];
  maxRange = d->Range[1];
} 

//-----------------------------------------------------------------------------
QVariant ctkVTKHistogram::minValue()const
{
  //Q_D(const ctkVTKHistogram);
  return 0;//d->MinBin;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKHistogram::maxValue()const
{
  Q_D(const ctkVTKHistogram);
  return d->MaxBin;
}

//-----------------------------------------------------------------------------
ctkControlPoint* ctkVTKHistogram::controlPoint(int index)const
{
  Q_D(const ctkVTKHistogram);
  ctkHistogramBar* cp = new ctkHistogramBar();
  cp->P.X = this->indexToPos(index);
  cp->P.Value = d->Bins->GetValue(index);
  return cp;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKHistogram::value(qreal pos)const
{
  QSharedPointer<ctkControlPoint> point(this->controlPoint(this->posToIndex(pos)));
  return point->value();
}

//-----------------------------------------------------------------------------
qreal ctkVTKHistogram::indexToPos(int index)const
{
  qreal posRange[2];
  this->range(posRange[0], posRange[1]);
  return posRange[0] + index * ((posRange[1] - posRange[0]) / (this->count() - 1));
}

//-----------------------------------------------------------------------------
int ctkVTKHistogram::posToIndex(qreal pos)const
{
  qreal posRange[2];
  this->range(posRange[0], posRange[1]);
  return (pos - posRange[0]) / ((posRange[1] - posRange[0]) / (this->count() - 1));
}

//-----------------------------------------------------------------------------
void ctkVTKHistogram::setDataArray(vtkDataArray* newDataArray)
{
  Q_D(ctkVTKHistogram);
  d->DataArray = newDataArray;
  this->qvtkReconnect(d->DataArray,vtkCommand::ModifiedEvent,
                      this, SIGNAL(changed()));
  emit changed();
}

//-----------------------------------------------------------------------------
vtkDataArray* ctkVTKHistogram::dataArray()const
{
  Q_D(const ctkVTKHistogram);
  return d->DataArray;
}

//-----------------------------------------------------------------------------
void ctkVTKHistogram::setComponent(int component)
{
  Q_D(ctkVTKHistogram);
  d->Component = component;
  // need rebuild
}

//-----------------------------------------------------------------------------
int ctkVTKHistogram::component()const
{
  Q_D(const ctkVTKHistogram);
  return d->Component;
}

//-----------------------------------------------------------------------------
int ctkVTKHistogram::numberOfBins()const
{
  Q_D(const ctkVTKHistogram);
  return d->UserNumberOfBins;
}

//-----------------------------------------------------------------------------
void ctkVTKHistogram::setNumberOfBins(int number)
{
  Q_D(ctkVTKHistogram);
  d->UserNumberOfBins = number;
}

//-----------------------------------------------------------------------------
template <class T>
void populateBins(vtkIntArray* bins, const ctkVTKHistogram* histogram)
{
  vtkDataArray* scalars = histogram->dataArray();
  int* binsPtr = bins->WritePointer(0, bins->GetNumberOfTuples());

  // reset bins to 0
  memset(binsPtr, 0, bins->GetNumberOfComponents()*bins->GetNumberOfTuples()*sizeof(int));

  const vtkIdType componentNumber = scalars->GetNumberOfComponents();
  const vtkIdType tupleNumber = scalars->GetNumberOfTuples();
  int component = histogram->component();

  double range[2];
  histogram->range(range[0], range[1]);
  T offset = static_cast<T>(range[0]);

  T* ptr = static_cast<T*>(scalars->WriteVoidPointer(0, tupleNumber));
  T* endPtr = ptr + tupleNumber * componentNumber;
  ptr += component;
  for (; ptr < endPtr; ptr += componentNumber)
    {
    Q_ASSERT( (static_cast<long long>(*ptr) - offset) == 
              (static_cast<int>(*ptr) - offset));
    binsPtr[static_cast<int>(*ptr - offset)]++;
    }
}

//-----------------------------------------------------------------------------
template <class T>
void populateIrregularBins(vtkIntArray* bins, const ctkVTKHistogram* histogram)
{
  vtkDataArray* scalars = histogram->dataArray();

  int* binsPtr = bins->WritePointer(0, bins->GetNumberOfComponents()*bins->GetNumberOfTuples());
  // reset bins to 0
  memset(binsPtr, 0, bins->GetNumberOfTuples() * sizeof(int));

  const vtkIdType componentNumber = scalars->GetNumberOfComponents();
  const vtkIdType tupleNumber = scalars->GetNumberOfTuples();
  int component = histogram->component();

  double range[2];
  histogram->range(range[0], range[1]);
  double offset = range[0];

  double binWidth = 1.;
  if (range[1] != range[0])
    {
    binWidth = static_cast<double>(bins->GetNumberOfTuples()) / (range[1] - range[0]);
    }

  T* ptr = static_cast<T*>(scalars->WriteVoidPointer(0, tupleNumber));
  T* endPtr = ptr + tupleNumber * componentNumber;
  ptr += component;
  for (; ptr < endPtr; ptr += componentNumber)
    {
    if (std::numeric_limits<T>::has_quiet_NaN &&
        vtkMath::IsNan(*ptr))
      {
      continue;
      }
    binsPtr[vtkMath::Floor((static_cast<double>(*ptr) - offset) * binWidth)]++;
    }
}

//-----------------------------------------------------------------------------
void ctkVTKHistogram::build()
{
  Q_D(ctkVTKHistogram);

  if (d->DataArray.GetPointer() == 0)
    {
    d->MinBin = 0;
    d->MaxBin = 0;
    d->Bins->SetNumberOfTuples(0);
    return;
    }

  const int binCount = d->computeNumberOfBins();

  d->Bins->SetNumberOfComponents(1);
  d->Bins->SetNumberOfTuples(binCount);

  if (binCount <= 0)
    {
    d->MinBin = 0;
    d->MaxBin = 0;
    return;
    }

  // What is the type of the array, discrete or reals
  if (static_cast<double>(binCount) != (d->Range[1] - d->Range[0] + 1))
    {
    switch(d->DataArray->GetDataType())
      {
      vtkTemplateMacro(populateIrregularBins<VTK_TT>(d->Bins, this));
      }
    }
  else
    {
    switch(d->DataArray->GetDataType())
      {
      vtkTemplateMacro(populateBins<VTK_TT>(d->Bins, this));
      }
    }
  // update Min/Max values
  int* binPtr = d->Bins->GetPointer(0);
  int* endPtr = d->Bins->GetPointer(binCount-1);
  d->MinBin = *endPtr;
  d->MaxBin = *endPtr;
  for (;binPtr < endPtr; ++binPtr)
    {
    d->MinBin = qMin(*binPtr, d->MinBin);
    d->MaxBin = qMax(*binPtr, d->MaxBin);
    }
  emit changed();
}

//-----------------------------------------------------------------------------
void ctkVTKHistogram::removeControlPoint( qreal pos )
{
  Q_UNUSED(pos);
  // TO BE IMPLEMENTED
}
