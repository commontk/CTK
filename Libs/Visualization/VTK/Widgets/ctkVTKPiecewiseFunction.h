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

#ifndef __ctkVTKPiecewiseFunction_h
#define __ctkVTKPiecewiseFunction_h

// CTK includes
#include "ctkTransferFunction.h"
#include "ctkPimpl.h"
#include "ctkVisualizationVTKWidgetsExport.h"
#include "ctkVTKObject.h"

class vtkPiecewiseFunction;
class ctkVTKPiecewiseFunctionPrivate;

/// \ingroup Visualization_VTK_Widgets
///
/// Transfer function for a vtkPiecewiseFunction

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKPiecewiseFunction: public ctkTransferFunction
{
  Q_OBJECT;
  QVTK_OBJECT;
public:
  ctkVTKPiecewiseFunction(vtkPiecewiseFunction* piecewiserFunction,
                              QObject* parent = 0);
  virtual ~ctkVTKPiecewiseFunction();

  virtual ctkControlPoint* controlPoint(int index)const;
  virtual QVariant value(qreal pos)const;
  virtual int count()const;
  virtual bool isDiscrete()const;
  virtual bool isEditable()const;

  virtual void range(qreal& minRange, qreal& maxRange)const;
  virtual QVariant minValue()const;
  virtual QVariant maxValue()const;

  virtual int insertControlPoint(const ctkControlPoint& cp);
  virtual int insertControlPoint(qreal pos);

  virtual void setControlPointPos(int index, qreal pos);
  virtual void setControlPointValue(int index, const QVariant& value);

  virtual void removeControlPoint( qreal pos );

  void setPiecewiseFunction(vtkPiecewiseFunction* piecewiseFunction);
  vtkPiecewiseFunction* piecewiseFunction()const;
protected:
  QScopedPointer<ctkVTKPiecewiseFunctionPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKPiecewiseFunction);
  Q_DISABLE_COPY(ctkVTKPiecewiseFunction);
};

#endif
