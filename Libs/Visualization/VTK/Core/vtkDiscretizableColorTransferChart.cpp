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
#include "vtkDiscretizableColorTransferChart.h"

#include "ctkVTKScalarsToColorsUtils.h"
#include "ctkCompilerDetections_p.h" // For CTK_NULLPTR
#include "vtkDiscretizableColorTransferControlPointsItem.h"

#include <vtkAxis.h>
#include <vtkColorTransferFunction.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkCompositeTransferFunctionItem.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkFloatArray.h>
#include <vtkObjectFactory.h>
#include <vtkTable.h>
#include <vtkTransform2D.h>

// ----------------------------------------------------------------------------
class vtkHistogramMarker : public vtkObject
{
public:

  static vtkHistogramMarker* New();

  vtkHistogramMarker();
  virtual ~vtkHistogramMarker();

  void SetPosition(double pos);

  vtkSmartPointer<vtkTable> GetTable();

private:
  vtkSmartPointer<vtkTable> m_table;

};

// ----------------------------------------------------------------------------
// vtkHistogramMarker methods

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkHistogramMarker)

// ----------------------------------------------------------------------------
vtkHistogramMarker::vtkHistogramMarker()
{
  m_table = vtkSmartPointer<vtkTable>::New();

  vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
  arrX->SetName("X Axis");
  m_table->AddColumn(arrX);

  vtkSmartPointer<vtkFloatArray> arrC = vtkSmartPointer<vtkFloatArray>::New();
  arrC->SetName("YAxis");
  m_table->AddColumn(arrC);

  m_table->SetNumberOfRows(2);
  m_table->SetValue(0, 0, 55);
  m_table->SetValue(0, 1, 0);

  m_table->SetValue(1, 0, 55);
  m_table->SetValue(1, 1, 1);
}

// ----------------------------------------------------------------------------
vtkSmartPointer<vtkTable> vtkHistogramMarker::GetTable()
{
  return m_table;
}

// ----------------------------------------------------------------------------
void vtkHistogramMarker::SetPosition(double pos)
{
  m_table->SetValue(0, 0, pos);
  m_table->SetValue(1, 0, pos);
  m_table->Modified();
}

// ----------------------------------------------------------------------------
vtkHistogramMarker::~vtkHistogramMarker()
{
}

// ----------------------------------------------------------------------------
// vtkDiscretizableColorTransferChart methods

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkDiscretizableColorTransferChart)

// ----------------------------------------------------------------------------
vtkDiscretizableColorTransferChart::vtkDiscretizableColorTransferChart()
{
  this->Transform = vtkSmartPointer<vtkTransform2D>::New();

  this->MinMarker = vtkSmartPointer<vtkHistogramMarker>::New();
  this->MaxMarker = vtkSmartPointer<vtkHistogramMarker>::New();

  this->SetAutoAxes(false);
  this->SetRenderEmpty(true);
  this->SetLayoutStrategy(vtkChart::FILL_RECT);
  this->ZoomWithMouseWheelOff();

  for (int i = 0; i < 4; ++i)
  {
    this->GetAxis(i)->SetVisible(true);
    this->GetAxis(i)->SetNumberOfTicks(0);
    this->GetAxis(i)->SetLabelsVisible(false);
    this->GetAxis(i)->SetTitle("");
  }
  this->GetAxis(vtkAxis::LEFT)->SetMargins(20, 0);
  this->GetAxis(vtkAxis::RIGHT)->SetMargins(20, 0);

  this->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);

  ///Disable actions
  this->SetActionToButton(ZOOM, -1);/// We don't want to zoom
  this->SetActionToButton(PAN, -1);/// Axes are not forced to bounds, disable panning

  this->CompositeHiddenItem = CTK_NULLPTR;
  this->ControlPoints = CTK_NULLPTR;

  this->rangeMoving = RangeMoving_NONE;

  this->DataRange[0] = VTK_DOUBLE_MAX;
  this->DataRange[1] = VTK_DOUBLE_MIN;
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferChart::SetColorTransferFunction(
  vtkDiscretizableColorTransferFunction* function)
{
  if (function == CTK_NULLPTR)
  {
    vtkSmartPointer<vtkDiscretizableColorTransferFunction> emptyCtf =
      vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
    this->SetColorTransferFunction(emptyCtf, 0, 255);
    return;
  }

  this->SetColorTransferFunction(function,
    function->GetRange()[0], function->GetRange()[1]);
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferChart::SetColorTransferFunction(
  vtkDiscretizableColorTransferFunction* function,
  double rangeMin, double rangeMax)
{
  this->ColorTransferFunction = function;
  this->ClearPlots();

  ///Build the histogram chart
  this->CompositeHiddenItem =
    vtkSmartPointer<vtkCompositeTransferFunctionItem>::New();
  this->CompositeHiddenItem->SetMaskAboveCurve(true);
  this->CompositeHiddenItem->SetOpacity(0);
  this->CompositeHiddenItem->SetInteractive(false);
  this->CompositeHiddenItem->SetColorTransferFunction(function);
  this->CompositeHiddenItem->SetOpacityFunction(
    function->GetScalarOpacityFunction());

  this->ControlPoints =
    vtkSmartPointer<vtkDiscretizableColorTransferControlPointsItem>::New();
  this->ControlPoints->SetUseOpacityPointHandles(false);
  this->ControlPoints->SetEndPointsRemovable(false);
  this->ControlPoints->SetInteractive(false);
  this->ControlPoints->SetEndPointsXMovable(false);
  this->ControlPoints->SetColorTransferFunction(function);
  this->ControlPoints->SetOpacityFunction(
    function->GetScalarOpacityFunction());
  this->ControlPoints->SetPointsFunction(
    vtkCompositeControlPointsItem::ColorAndOpacityPointsFunction);

  this->OriginalRange[0] = rangeMin;
  this->OriginalRange[1] = rangeMax;

  ///Add the Min/Max Markers
  this->MinPlot = AddPlot(vtkChart::LINE);
  this->MaxPlot = AddPlot(vtkChart::LINE);
  this->MinLinePlot = AddPlot(vtkChart::LINE);
  this->MaxLinePlot = AddPlot(vtkChart::LINE);

  this->MinPlot->SetInputData(this->MinMarker->GetTable(), 0, 1);
  this->MaxPlot->SetInputData(this->MaxMarker->GetTable(), 0, 1);
  this->MinLinePlot->SetInputData(this->MinMarker->GetTable(), 0, 1);
  this->MaxLinePlot->SetInputData(this->MaxMarker->GetTable(), 0, 1);

  this->MinPlot->SetColor(187, 187, 187, 100);
  this->MinPlot->SetWidth(11.0);

  this->MaxPlot->SetColor(187, 187, 187, 100);
  this->MaxPlot->SetWidth(11.0);

  this->MinLinePlot->SetColor(255, 255, 255, 255);
  this->MinLinePlot->SetWidth(1.0);

  this->MaxLinePlot->SetColor(255, 255, 255, 255);
  this->MaxLinePlot->SetWidth(1.0);

  this->CurrentRange[0] = rangeMin;
  this->MinMarker->SetPosition(this->CurrentRange[0]);

  this->CurrentRange[1] = rangeMax;
  this->MaxMarker->SetPosition(this->CurrentRange[1]);

  this->AddPlot(this->CompositeHiddenItem);
  this->AddPlot(this->ControlPoints);
}

// ----------------------------------------------------------------------------
vtkDiscretizableColorTransferChart::~vtkDiscretizableColorTransferChart()
{
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferChart::UpdateMarkerPosition(
  const vtkContextMouseEvent& m)
{
  vtkVector2f pos;
  this->Transform->InverseTransformPoints(m.GetScenePos().GetData(),
    pos.GetData(), 1);

  double limitRange[2];
  limitRange[0] = std::min(this->OriginalRange[0], this->DataRange[0]);
  limitRange[1] = std::max(this->OriginalRange[1], this->DataRange[1]);

  if (rangeMoving == RangeMoving_MIN)
  {
    double newValue = static_cast<double>(pos.GetX());
    if (newValue < limitRange[0])
    {
      this->CurrentRange[0] = limitRange[0];
    }
    else if (newValue < this->CurrentRange[1])
    {
      this->CurrentRange[0] = newValue;
    }
    this->MinMarker->SetPosition(this->CurrentRange[0]);
    if (this->ColorTransferFunction != CTK_NULLPTR)
    {
      this->ControlPoints->StartProcessing();
      ctk::remapColorScale(this->ColorTransferFunction, this->CurrentRange[0],
        this->CurrentRange[1]);
      this->ControlPoints->EndProcessing();
    }
  }
  else if (rangeMoving == RangeMoving_MAX)
  {
    double newValue = static_cast<double>(pos.GetX());
    if (newValue > limitRange[1])
    {
      this->CurrentRange[1] = limitRange[1];
    }
    else if (newValue > this->CurrentRange[0])
    {
      this->CurrentRange[1] = newValue;
    }
    this->MaxMarker->SetPosition(this->CurrentRange[1]);
    if (this->ColorTransferFunction)
    {
      this->ControlPoints->StartProcessing();
      ctk::remapColorScale(this->ColorTransferFunction, this->CurrentRange[0],
        this->CurrentRange[1]);
      this->ControlPoints->EndProcessing();
    }
  }
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferChart::MouseMoveEvent(
  const vtkContextMouseEvent &mouse)
{
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON &&
    rangeMoving != RangeMoving_NONE)
  {
    this->UpdateMarkerPosition(mouse);
  }

  return this->Superclass::MouseMoveEvent(mouse);
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferChart::IsInRange(double min, double max,
  double value)
{
  return value >= min && value < max;
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferChart::MouseButtonPressEvent(
  const vtkContextMouseEvent& mouse)
{
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON)
  {
    if (rangeMoving == RangeMoving_NONE)
    {
      this->CalculateUnscaledPlotTransform(
        this->CompositeHiddenItem->GetXAxis(),
        this->CompositeHiddenItem->GetYAxis(), this->Transform.Get());

      ///Need to compute the position of the cursor toward the marker
      double minCoord[2] = {this->CurrentRange[0], 0};
      double pixelMin[2];
      this->Transform->TransformPoints(minCoord, pixelMin, 1);

      double maxCoord[2] = {this->CurrentRange[1], 0};
      double pixelMax[2];
      this->Transform->TransformPoints(maxCoord, pixelMax, 1);

      double catchWidth = 5;
      ///If min and max get close prefer min over max
      if (IsInRange(pixelMin[0] - catchWidth,
        pixelMin[0] + catchWidth, mouse.GetPos().GetX()))
      {
        rangeMoving = RangeMoving_MIN;
      }
      else if (IsInRange( pixelMax[0] - catchWidth, pixelMax[0] + catchWidth,
        mouse.GetPos().GetX()))
      {
        rangeMoving = RangeMoving_MAX;
      }
    }
  }
  return this->Superclass::MouseButtonPressEvent(mouse);
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferChart::MouseButtonReleaseEvent(
  const vtkContextMouseEvent &mouse)
{
  rangeMoving = RangeMoving_NONE;
  return this->Superclass::MouseButtonReleaseEvent(mouse);
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferChart::GetCurrentControlPointColor(
  double rgb[3])
{
  vtkColorTransferFunction* ctf =
    this->ControlPoints->GetColorTransferFunction();
  if (!ctf)
  {
    return false;
  }

  vtkIdType currentIdx = this->ControlPoints->GetCurrentPoint();
  if (currentIdx < 0)
  {
    return false;
  }

  double xrgbms[6];
  ctf->GetNodeValue(currentIdx, xrgbms);
  rgb[0] = xrgbms[1];
  rgb[1] = xrgbms[2];
  rgb[2] = xrgbms[3];

  return true;
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferChart::SetCurrentControlPointColor(
  const double rgb[3])
{
  vtkColorTransferFunction* ctf =
    this->ControlPoints->GetColorTransferFunction();
  if (!ctf)
  {
    return;
  }

  vtkIdType currentIdx = this->ControlPoints->GetCurrentPoint();
  if (currentIdx < 0)
  {
    return;
  }

  double xrgbms[6];
  ctf->GetNodeValue(currentIdx, xrgbms);
  xrgbms[1] = rgb[0];
  xrgbms[2] = rgb[1];
  xrgbms[3] = rgb[2];
  ctf->SetNodeValue(currentIdx, xrgbms);
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferChart::SetDataRange(double min, double max)
{
  this->DataRange[0] = min;
  this->DataRange[1] = max;
}

// ----------------------------------------------------------------------------
double* vtkDiscretizableColorTransferChart::GetDataRange()
{
  return this->DataRange;
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferChart::SetCurrentRange(
  double min, double max)
{
  double limitRange[2];
  limitRange[0] = std::min(this->OriginalRange[0], this->DataRange[0]);
  limitRange[1] = std::max(this->OriginalRange[1], this->DataRange[1]);

  ///check if min < max;
  min = vtkMath::ClampValue(min, limitRange[0], limitRange[1]);
  max = vtkMath::ClampValue(max, limitRange[0], limitRange[1]);
  if (min < max)
  {
    this->CurrentRange[0] = 
      min < limitRange[0] ? limitRange[0] : min;
    this->CurrentRange[1] =
      max > limitRange[1] ? limitRange[1] : max;
    this->MinMarker->SetPosition(this->CurrentRange[0]);
    this->MaxMarker->SetPosition(this->CurrentRange[1]);
  }

  if (this->ColorTransferFunction != CTK_NULLPTR)
  {
    this->ControlPoints->StartProcessing();
    ctk::remapColorScale(this->ColorTransferFunction,
      this->CurrentRange[0], this->CurrentRange[1]);
    this->ControlPoints->EndProcessing();
  }
}

// ----------------------------------------------------------------------------
double* vtkDiscretizableColorTransferChart::GetCurrentRange()
{
  return this->CurrentRange;
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferChart::CenterRange(double center)
{
  double width = this->CurrentRange[1] - this->CurrentRange[0];
  double limitRange[2];
  limitRange[0] = std::min(this->OriginalRange[0], this->DataRange[0]);
  limitRange[1] = std::max(this->OriginalRange[1], this->DataRange[1]);
  double limitWidth = 2.0 * std::min(center - limitRange[0], limitRange[1] - center);

  width = width < limitWidth ? width : limitWidth;

  double newMin = center - width / 2;
  double newMax = newMin + width;

  this->SetCurrentRange(newMin, newMax);
}

// ----------------------------------------------------------------------------
vtkCompositeControlPointsItem*
vtkDiscretizableColorTransferChart::GetControlPointsItem()
{
  return this->ControlPoints;
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferChart::IsProcessingColorTransferFunction() const
{
  return this->ControlPoints->IsProcessing();
}

