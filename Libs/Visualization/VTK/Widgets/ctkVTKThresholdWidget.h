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

#ifndef __ctkVTKThresholdWidget_h
#define __ctkVTKThresholdWidget_h

//Qt includes
#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKThresholdWidgetPrivate;

// VTK includes
class vtkPiecewiseFunction;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKThresholdWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  /// Setting the opacity property has no effect when no piecewise function is
  /// set.
  Q_PROPERTY(double opacity READ opacity WRITE setOpacity DESIGNABLE false)
  /// Use 2 (true) or 4 (false) control points to control the threshold.
  /// 4 points by default (false).
  Q_PROPERTY(bool useSharpness READ useSharpness WRITE setUseSharpness)
public:
  ctkVTKThresholdWidget(QWidget* parent = 0);
  virtual ~ctkVTKThresholdWidget();

  vtkPiecewiseFunction* piecewiseFunction()const;

  void range(double* range)const;
  void thresholdValues(double* values)const;
  double opacity()const;

  bool useSharpness()const;
  void setUseSharpness(bool use);

public Q_SLOTS:
  void setPiecewiseFunction(vtkPiecewiseFunction* function);
  void setRange(double min, double max);
  void setThresholdValues(double min, double max);
  void setOpacity(double opacity);

protected Q_SLOTS:
  void updateFromPiecewiseFunction();

protected:
  QScopedPointer<ctkVTKThresholdWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKThresholdWidget);
  Q_DISABLE_COPY(ctkVTKThresholdWidget);
};

#endif
