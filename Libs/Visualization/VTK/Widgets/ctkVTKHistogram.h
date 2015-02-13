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

#ifndef __ctkVTKHistogram_h
#define __ctkVTKHistogram_h

// CTK includes
#include "ctkHistogram.h"
#include "ctkPimpl.h"
#include "ctkVisualizationVTKWidgetsExport.h"
#include "ctkVTKObject.h"

class vtkDataArray;
class ctkVTKHistogramPrivate;

/// \ingroup Visualization_VTK_Widgets
///
/// Transfer function for a vtkColorTransferFunction. 
/// The value is an RGB QColor (no alpha supported)
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKHistogram: public ctkHistogram
{
  Q_OBJECT;
  QVTK_OBJECT;
  Q_PROPERTY(int component READ component WRITE setComponent)
  Q_PROPERTY(QVariant maxValue READ maxValue)
  Q_PROPERTY(QVariant minValue READ minValue)
  Q_PROPERTY(int numberOfBins READ numberOfBins WRITE setNumberOfBins)
public:
  ctkVTKHistogram(QObject* parent = 0);
  ctkVTKHistogram(vtkDataArray* dataArray, QObject* parent = 0);
  virtual ~ctkVTKHistogram();
  
  virtual ctkControlPoint* controlPoint(int index)const;
  virtual QVariant value(qreal pos)const;
  /// Returns the number of bins. Returns 0 until build() is called.
  virtual int count()const;

  /// Please note that range only works if you have at least set an array.
  virtual void range(qreal& minRange, qreal& maxRange)const;
  virtual QVariant minValue()const;
  virtual QVariant maxValue()const;

  Q_INVOKABLE void setDataArray(vtkDataArray* dataArray);
  Q_INVOKABLE vtkDataArray* dataArray()const;

  void setComponent(int component);
  int component()const;

  int numberOfBins()const;
  void setNumberOfBins(int number);

  Q_INVOKABLE virtual void removeControlPoint( qreal pos );

  Q_INVOKABLE virtual void build();
protected:
  qreal indexToPos(int index)const;
  int posToIndex(qreal pos)const;

protected:
  QScopedPointer<ctkVTKHistogramPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKHistogram);
  Q_DISABLE_COPY(ctkVTKHistogram);
};

#endif
