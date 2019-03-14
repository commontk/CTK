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
public:
  /// Please note that ctkVTKColorTransferFunction methods only work only if
  /// colorTransferFunction is set.
  ctkVTKColorTransferFunction(QObject* parent = nullptr);
  ctkVTKColorTransferFunction(vtkColorTransferFunction* colorTransferFunction, 
                              QObject* parent = nullptr);
  ~ctkVTKColorTransferFunction() override;
  
  /// Please note that controlPoint methods only works if you have at least one
  /// ControlPoint.
  ctkControlPoint* controlPoint(int index)const override;
  QVariant value(qreal pos)const override;
  int count()const override;
  bool isDiscrete()const override;
  bool isEditable()const override;

  void range(qreal& minRange, qreal& maxRange)const override;
  QVariant minValue()const override;
  QVariant maxValue()const override;

  int insertControlPoint(const ctkControlPoint& cp) override;
  int insertControlPoint(qreal pos) override;

  void setControlPointPos(int index, qreal pos) override;
  void setControlPointValue(int index, const QVariant& value) override;

  void removeControlPoint( qreal pos ) override;

  void setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction);
  vtkColorTransferFunction* colorTransferFunction()const;
protected:
  QScopedPointer<ctkVTKColorTransferFunctionPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKColorTransferFunction);
  Q_DISABLE_COPY(ctkVTKColorTransferFunction);
};

#endif
