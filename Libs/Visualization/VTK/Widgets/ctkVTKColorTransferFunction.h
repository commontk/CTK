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

#ifndef __ctkVTKColorTransferFunction_h
#define __ctkVTKColorTransferFunction_h

// CTK includes
#include "ctkTransferFunction.h"
#include "ctkPimpl.h"
#include "ctkVisualizationVTKWidgetsExport.h"
#include "ctkVTKObject.h"

class vtkColorTransferFunction;
class ctkVTKColorTransferFunctionPrivate;

/// \ingroup Visualization_VTK_Widgets
///
/// Transfer function for a vtkColorTransferFunction. 
/// The value is an RGB QColor (no alpha supported)
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKColorTransferFunction: public ctkTransferFunction
{
  Q_OBJECT;
  QVTK_OBJECT;
  Q_PROPERTY(int count READ count)
  Q_PROPERTY(bool editable READ isEditable)
  Q_PROPERTY(bool discrete READ isDiscrete)
  Q_PROPERTY(QVariant minValue READ minValue)
  Q_PROPERTY(QVariant maxValue READ maxValue)
public:
  /// Please note that ctkVTKColorTransferFunction methods only work only if
  /// colorTransferFunction is set.
  ctkVTKColorTransferFunction(QObject* parent = 0);
  ctkVTKColorTransferFunction(vtkColorTransferFunction* colorTransferFunction, 
                              QObject* parent = 0);
  virtual ~ctkVTKColorTransferFunction();
  
  /// Please note that controlPoint methods only works if you have at least one
  /// ControlPoint.
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

  Q_INVOKABLE void setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction);
  Q_INVOKABLE vtkColorTransferFunction* colorTransferFunction()const;
protected:
  QScopedPointer<ctkVTKColorTransferFunctionPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKColorTransferFunction);
  Q_DISABLE_COPY(ctkVTKColorTransferFunction);
};

#endif
