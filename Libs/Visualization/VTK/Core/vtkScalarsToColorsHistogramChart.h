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

#ifndef __vtkScalarsToColorsHistogramChart_h
#define __vtkScalarsToColorsHistogramChart_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkChartXY.h>
#include <vtkNew.h>

class vtkPlotBar;
class vtkScalarsToColors;
class vtkTable;

class CTK_VISUALIZATION_VTK_CORE_EXPORT vtkScalarsToColorsHistogramChart
  : public vtkChartXY
{
public:
  vtkTypeMacro(vtkScalarsToColorsHistogramChart, vtkChartXY)
  static vtkScalarsToColorsHistogramChart* New();

  enum
  {
    VTK_AUTO, // VTK auto scaling: scale the axis to view all data that is visible.
    MAXIMUM, // Scale the axis to the maximum value of histogram.
    MEAN_PLUS_THREE_SIGMA // Scale the axis to "mean + 3 * sigma".
  };

  /// Set/Get the left axis mode, which controls the axis range computation.
  ///   VTK_AUTO: VTK auto scaling. Scale the axis to view all data that is visible.
  ///   MAXIMUM: Scale the axis to the maximum value of histogram.
  ///   MEAN_PLUS_THREE_SIGMA: Scale the axis to "mean + 3 * sigma".
  /// Default mode is VTK_AUTO.
  void SetLeftAxisMode(int mode);
  int GetLeftAxisMode();

  /// Set input for histogram
  virtual void SetHistogramInputData(vtkTable* table, const char* xAxisColumn,
    const char* yAxisColumn);

  /// Set scalar visibility in the histogram plot bar
  virtual void SetScalarVisibility(bool visible);

  /// Set lookup table
  virtual void SetLookupTable(vtkScalarsToColors* lut);

  /// Set the color array name
  virtual void SelectColorArray(const char* arrayName);

protected:
  vtkNew<vtkPlotBar> HistogramPlotBar;
  int LeftAxisMode;

private:
  vtkScalarsToColorsHistogramChart();
  virtual ~vtkScalarsToColorsHistogramChart();
};

#endif // __vtkScalarsToColorsHistogramChart_h
