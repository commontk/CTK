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

#ifndef __vtkScalarsToColorsContextItem_h
#define __vtkScalarsToColorsContextItem_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkAbstractContextItem.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkVector.h>

class vtkScalarsToColorsHistogramChart;
class vtkDiscretizableColorTransferChart;
class vtkDiscretizableColorTransferFunction;
class vtkScalarsToColors;
class vtkScalarsToColorsPreviewChart;
class vtkTable;

/// \ingroup Visualization_VTK_Core
/// This class uses three charts to combine an histogram from a dataset,
/// a color transfer function editor, and a preview of the resulting color
/// transfer function.
class CTK_VISUALIZATION_VTK_CORE_EXPORT vtkScalarsToColorsContextItem
  : public vtkAbstractContextItem
{
  vtkTypeMacro(vtkScalarsToColorsContextItem, vtkAbstractContextItem)
public:
  static vtkScalarsToColorsContextItem* New();

  /// Set/Get the left axis mode.
  /// This controls the axis range computation.
  /// \see vtkScalarsToColorsHistogramChart::SetLeftAxisMode
  void SetLeftAxisMode(int mode);
  int GetLeftAxisMode();

  /// Copy the color transfer function as a vtkDiscretizableColorTransferFunction
  void CopyColorTransferFunction(vtkScalarsToColors* ctf);
  vtkDiscretizableColorTransferFunction* GetDiscretizableColorTransferFunction();
  void ResetColorTransferFunction();
  void BuildColorTransferFunction();

  /// Set the table used by the histogram chart
  void SetHistogramTable(vtkTable* table,
    const char* xAxisColumn, const char* yAxisColumn);

  /// Paint event.
  bool Paint(vtkContext2D* painter) VTK_OVERRIDE;

  /// Get/Set the color of the current control point.
  void SetCurrentControlPointColor(const double rgb[3]);
  bool GetCurrentControlPointColor(double rgb[3]);

  /// Get/Set the range defined by histogram data
  void SetDataRange(double min, double max);
  double* GetDataRange();

  /// Get/Set the range used by the color transfer function
  void SetCurrentRange(double min, double max);
  double* GetCurrentRange();

  /// Get/Set the range displayed in the widget
  void SetVisibleRange(double min, double max);
  double* GetVisibleRange();

  /// Center the color tranfer function around \center
  void CenterRange(double center);

  /// Update charts range to match data and color transfer function ranges
  void RecalculateChartsRange();

  /// Weight opacity control points by \opacity 
  void SetGlobalOpacity(double opacity);

  /// Reverse the color map of the transfer function. Note that opacity values
  /// not modified.
  void InvertColorTransferFunction();

  /// Indicates if the color transfer function is being modified by control
  /// points items.
  bool IsProcessingColorTransferFunction() const;

protected:

  vtkSmartPointer<vtkDiscretizableColorTransferChart> EditorChart;
  vtkSmartPointer<vtkScalarsToColorsPreviewChart> PreviewChart;
  vtkSmartPointer<vtkScalarsToColorsHistogramChart> HistogramChart;

  vtkSmartPointer<vtkDiscretizableColorTransferFunction> ColorTransferFunction;

private:
  vtkScalarsToColorsContextItem();
  ~vtkScalarsToColorsContextItem() VTK_OVERRIDE;

  /// Cached geometry of the scene
  vtkVector2i LastSceneSize;

  /// Internal event forwarder
  class EventForwarder;
  EventForwarder* PrivateEventForwarder;
};
#endif // __vtkScalarsToColorsContextItem_h
