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

#ifndef __vtkDiscretizableColorTransferChart_h
#define __vtkDiscretizableColorTransferChart_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkChartXY.h>
#include <vtkSmartPointer.h>

class vtkCompositeControlPointsItem;
class vtkCompositeTransferFunctionItem;
class vtkDiscretizableColorTransferControlPointsItem;
class vtkDiscretizableColorTransferFunction;
class vtkHistogramMarker;
class vtkTransform2D;

class CTK_VISUALIZATION_VTK_CORE_EXPORT vtkDiscretizableColorTransferChart
  : public vtkChartXY
{
public:
  vtkTypeMacro(vtkDiscretizableColorTransferChart, vtkChartXY)
  static vtkDiscretizableColorTransferChart* New();

  void SetColorTransferFunction(
    vtkDiscretizableColorTransferFunction* function);
  void SetColorTransferFunction(
    vtkDiscretizableColorTransferFunction* function,
    double dataRangeMin, double dataRangeMax);

  /// is a currently selected control point, false otherwise.
  bool GetCurrentControlPointColor(double rgb[3]);

  /// Set the color of the current color control point.
  void SetCurrentControlPointColor(const double rgb[3]);

  ///Set/Get the data range
  void SetDataRange(double min, double max);
  double* GetDataRange();

  /// Set/Get the current range
  ///
  /// Set will clamp values into the current dataRange
  void SetCurrentRange(double min, double max);
  double* GetCurrentRange();
  void RemapColorTransferFunction();

  ///Set/Get the original range
  void SetOriginalRange(double min, double max);
  double* GetOriginalRange();

  /// Center the current position to the given point
  void CenterRange(double center);

  vtkCompositeControlPointsItem* GetControlPointsItem();

  bool IsProcessingColorTransferFunction() const;

  bool MouseMoveEvent(const vtkContextMouseEvent &mouse) VTK_OVERRIDE;
  bool MouseButtonPressEvent(const vtkContextMouseEvent& mouse) VTK_OVERRIDE;
  bool MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse) VTK_OVERRIDE;

protected:

  static bool IsInRange(double min, double max, double value);
  void UpdateMarkerPosition(const vtkContextMouseEvent& m);

  vtkSmartPointer<vtkCompositeTransferFunctionItem> CompositeHiddenItem;
  vtkSmartPointer<vtkDiscretizableColorTransferControlPointsItem> ControlPoints;

  vtkSmartPointer<vtkTransform2D> Transform;
  vtkSmartPointer<vtkDiscretizableColorTransferFunction> ColorTransferFunction;

  vtkPlot *MinPlot;
  vtkPlot *MaxPlot;
  vtkPlot *MinLinePlot;
  vtkPlot *MaxLinePlot;
  vtkSmartPointer<vtkHistogramMarker> MinMarker;
  vtkSmartPointer<vtkHistogramMarker> MaxMarker;

  enum RangeMoving {
    RangeMoving_NONE, RangeMoving_MIN, RangeMoving_MAX
  };
  RangeMoving rangeMoving;

  double DataRange[2];
  double CurrentRange[2];
  double OriginalRange[2];

private:
  vtkDiscretizableColorTransferChart();
  virtual ~vtkDiscretizableColorTransferChart();
};

#endif
