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
#include "vtkScalarsToColorsContextItem.h"

// CTK includes
#include "ctkVTKScalarsToColorsUtils.h"
#include "vtkDiscretizableColorTransferChart.h"
#include "vtkScalarsToColorsHistogramChart.h"
#include "vtkScalarsToColorsPreviewChart.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkBrush.h>
#include <vtkContextScene.h>
#include <vtkCommand.h>
#include <vtkCompositeControlPointsItem.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkObjectFactory.h>
#include <vtkPiecewiseFunction.h>
#include <vtkTable.h>
#include <vtkVector.h>

//-----------------------------------------------------------------------------
// Internal class EventForwarder
//-----------------------------------------------------------------------------
class vtkScalarsToColorsContextItem::EventForwarder
{
public:
  EventForwarder(vtkScalarsToColorsContextItem* parent)
    :Self(parent)
  {}

  ~EventForwarder()
  {}

  void ForwardEvent(vtkObject* vtkNotUsed(object), unsigned long eventId,
    void* vtkNotUsed(data))
  {
    this->Self->InvokeEvent(eventId);
  }

  // Reference to owner of the EventForwarder
  vtkScalarsToColorsContextItem* Self;
};

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkScalarsToColorsContextItem)

// ----------------------------------------------------------------------------
vtkScalarsToColorsContextItem::vtkScalarsToColorsContextItem()
{
  this->PrivateEventForwarder = new EventForwarder(this);

  this->LastSceneSize = vtkVector2i(0, 0);

  this->LimitRange[0] = VTK_DOUBLE_MIN;
  this->LimitRange[1] = VTK_DOUBLE_MAX;

  vtkSmartPointer<vtkBrush> b = vtkSmartPointer<vtkBrush>::New();
  b->SetOpacityF(0);

  //Histogram
  this->HistogramChart =
    vtkSmartPointer<vtkScalarsToColorsHistogramChart>::New();
  this->HistogramChart->SetScalarVisibility(true);
  AddItem(this->HistogramChart.GetPointer());

  //Editor 
  this->EditorChart =
    vtkSmartPointer<vtkDiscretizableColorTransferChart>::New();
  this->EditorChart->SetBackgroundBrush(b);
  AddItem(this->EditorChart.GetPointer());

  //Preview
  this->PreviewChart =
    vtkSmartPointer<vtkScalarsToColorsPreviewChart>::New();
  AddItem(this->PreviewChart.GetPointer());

  this->SetColorTransferFunction(CTK_NULLPTR);
}

// ----------------------------------------------------------------------------
vtkScalarsToColorsContextItem::~vtkScalarsToColorsContextItem()
{
  this->RemoveAllObservers();

  delete this->PrivateEventForwarder;
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::SetColorTransferFunction(
  vtkScalarsToColors* ctf)
{
  if (ctf == CTK_NULLPTR)
  {
    this->SetDiscretizableColorTransferFunction(CTK_NULLPTR);
    return;
  }

  if (ctf->IsA("vtkDiscretizableColorTransferFunction"))
  {
    this->SetDiscretizableColorTransferFunction(
      vtkDiscretizableColorTransferFunction::SafeDownCast(ctf));
  }
  else if (ctf->IsA("vtkColorTransferFunction"))
  {
    vtkSmartPointer<vtkColorTransferFunction> newCtf =
      vtkColorTransferFunction::SafeDownCast(ctf);

    vtkSmartPointer<vtkDiscretizableColorTransferFunction> dctf =
      vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
    dctf->vtkColorTransferFunction::DeepCopy(newCtf);

    vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
      vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFunction->AddPoint(0.0, 0.0);
    opacityFunction->AddPoint(255.0, 1.0);
    dctf->SetScalarOpacityFunction(opacityFunction);
    dctf->EnableOpacityMappingOn();

    this->SetDiscretizableColorTransferFunction(dctf);
  }
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::SetDiscretizableColorTransferFunction(
  vtkDiscretizableColorTransferFunction* colorTransfer)
{
  vtkSmartPointer<vtkCompositeControlPointsItem> oldControlPoints =
    this->EditorChart->GetControlPointsItem();

  if (oldControlPoints != CTK_NULLPTR)
  {
    oldControlPoints->RemoveObservers(vtkCommand::EndEvent);
    oldControlPoints->RemoveObservers(
      vtkControlPointsItem::CurrentPointEditEvent);
  }

  this->ColorTransferFunction = colorTransfer;

  this->EditorChart->SetColorTransferFunction(this->ColorTransferFunction);

  this->PreviewChart->SetColorTransferFunction(this->ColorTransferFunction);

  this->HistogramChart->SetLookupTable(this->ColorTransferFunction);

  vtkSmartPointer<vtkCompositeControlPointsItem> controlPoints =
    this->EditorChart->GetControlPointsItem();

  controlPoints->AddObserver(vtkCommand::EndEvent,
    this->PrivateEventForwarder, &EventForwarder::ForwardEvent);
  controlPoints->AddObserver(vtkControlPointsItem::CurrentPointEditEvent,
    this->PrivateEventForwarder, &EventForwarder::ForwardEvent);

  /// Set the preview chart range to the color transfer function range
  this->RecalculateChartsRange();
}

// ----------------------------------------------------------------------------
vtkScalarsToColors* vtkScalarsToColorsContextItem::GetColorTransferFunction()
{
  return this->ColorTransferFunction;
}

// ----------------------------------------------------------------------------
vtkDiscretizableColorTransferFunction*
  vtkScalarsToColorsContextItem::GetDiscretizableColorTransferFunction()
{
  return this->ColorTransferFunction;
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::SetHistogramTable(vtkTable* table,
  const char* xAxisColumn, const char* yAxisColumn)
{
  this->HistogramChart->SetHistogramInputData(table, xAxisColumn, yAxisColumn);
  this->HistogramChart->SetLookupTable(this->ColorTransferFunction);
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::SetDataRange(double min, double max)
{
  this->EditorChart->SetDataRange(min, max);
  this->RecalculateChartsRange();
}

// ----------------------------------------------------------------------------
double* vtkScalarsToColorsContextItem::GetDataRange()
{
  return this->EditorChart->GetDataRange();
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::SetCurrentRange(double min, double max)
{
  this->EditorChart->SetCurrentRange(min, max);
}

// ----------------------------------------------------------------------------
double* vtkScalarsToColorsContextItem::GetCurrentRange()
{
  return this->EditorChart->GetCurrentRange();
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::CenterRange(double center)
{
  this->EditorChart->CenterRange(center);
}

// ----------------------------------------------------------------------------
double* vtkScalarsToColorsContextItem::GetLimitRange()
{
  return LimitRange;
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::RecalculateChartsRange()
{
  if (this->GetDiscretizableColorTransferFunction() == CTK_NULLPTR)
  {
    return;
  }

  /// Recalculate limit range
  double* ctfRange = this->GetDiscretizableColorTransferFunction()->GetRange();
  this->LimitRange[0] = std::min(ctfRange[0], this->GetDataRange()[0]);
  this->LimitRange[1] = std::max(ctfRange[1], this->GetDataRange()[1]);

  this->EditorChart->GetAxis(vtkAxis::BOTTOM)->SetUnscaledRange(
    this->LimitRange);
  this->EditorChart->RecalculateBounds();

  this->HistogramChart->GetAxis(vtkAxis::BOTTOM)->SetUnscaledRange(
    this->LimitRange);
  this->HistogramChart->RecalculateBounds();

  this->PreviewChart->GetAxis(vtkAxis::BOTTOM)->SetUnscaledRange(
    this->LimitRange);
  this->PreviewChart->RecalculateBounds();
}

// ----------------------------------------------------------------------------
bool vtkScalarsToColorsContextItem::GetCurrentControlPointColor(double rgb[3])
{
  return this->EditorChart->GetCurrentControlPointColor(rgb);
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::SetCurrentControlPointColor(
    const double rgb[3])
{
  this->EditorChart->SetCurrentControlPointColor(rgb);
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::SetGlobalOpacity(double opacity)
{
  ctk::setTransparency(this->ColorTransferFunction, (opacity));
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsContextItem::InvertColorTransferFunction()
{
  ctk::reverseColorMap(this->ColorTransferFunction);
}

// ----------------------------------------------------------------------------
bool vtkScalarsToColorsContextItem::IsProcessingColorTransferFunction() const
{
  return this->EditorChart->IsProcessingColorTransferFunction();
}

// ----------------------------------------------------------------------------
bool vtkScalarsToColorsContextItem::Paint(vtkContext2D* painter)
{
  vtkContextScene* scene = this->GetScene();

  int sceneWidth = scene->GetSceneWidth();
  int sceneHeight = scene->GetSceneHeight();

  if (sceneWidth != this->LastSceneSize.GetX()
    || sceneHeight != this->LastSceneSize.GetY())
  {
    // Update the geometry size cache
    this->LastSceneSize.Set(sceneWidth, sceneHeight);

    float colorBarThickness = 20;
    vtkRectf colorTransferFunctionChartSize(0.0, 0.0, sceneWidth,
     colorBarThickness);

    this->PreviewChart->SetSize(colorTransferFunctionChartSize);
    this->PreviewChart->SetHiddenAxisBorder(0);
    this->PreviewChart->RecalculateBounds();

    vtkRectf editorChartSize(0.0, 0.0, sceneWidth,
      sceneHeight);
    this->EditorChart->SetSize(editorChartSize);
    this->EditorChart->RecalculateBounds();

    this->HistogramChart->SetSize(editorChartSize);
    this->HistogramChart->RecalculateBounds();
  }

  return this->Superclass::Paint(painter);
}
