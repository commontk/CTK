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
  Q_PROPERTY(int count READ count)
  Q_PROPERTY(bool editable READ isEditable)
  Q_PROPERTY(bool discrete READ isDiscrete)
  Q_PROPERTY(QVariant minValue READ minValue)
  Q_PROPERTY(QVariant maxValue READ maxValue)

public:
  ctkVTKPiecewiseFunction(QObject* parent = 0);
  ctkVTKPiecewiseFunction(vtkPiecewiseFunction* piecewiserFunction,
                              QObject* parent = 0);
  virtual ~ctkVTKPiecewiseFunction();

  Q_INVOKABLE virtual ctkControlPoint* controlPoint(int index)const;
  Q_INVOKABLE virtual QVariant value(qreal pos)const;
  virtual int count()const;
  virtual bool isDiscrete()const;
  virtual bool isEditable()const;

  Q_INVOKABLE virtual void range(qreal& minRange, qreal& maxRange)const;
  virtual QVariant minValue()const;
  virtual QVariant maxValue()const;

  Q_INVOKABLE virtual int insertControlPoint(const ctkControlPoint& cp);
  Q_INVOKABLE virtual int insertControlPoint(qreal pos);

  Q_INVOKABLE virtual void setControlPointPos(int index, qreal pos);
  Q_INVOKABLE virtual void setControlPointValue(int index, const QVariant& value);

  Q_INVOKABLE virtual void removeControlPoint( qreal pos );

  Q_INVOKABLE void setPiecewiseFunction(vtkPiecewiseFunction* piecewiseFunction);
  Q_INVOKABLE vtkPiecewiseFunction* piecewiseFunction()const;
protected:
  QScopedPointer<ctkVTKPiecewiseFunctionPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKPiecewiseFunction);
  Q_DISABLE_COPY(ctkVTKPiecewiseFunction);
};

#endif
