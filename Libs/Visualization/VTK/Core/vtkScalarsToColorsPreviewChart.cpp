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

#include "vtkScalarsToColorsPreviewChart.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkColorTransferFunctionItem.h>
#include <vtkColorTransferFunction.h>
#include <vtkObjectFactory.h>

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkScalarsToColorsPreviewChart)

// ----------------------------------------------------------------------------
vtkScalarsToColorsPreviewChart::vtkScalarsToColorsPreviewChart()
{
  this->SetAutoAxes(false);
  this->SetRenderEmpty(true);
  this->SetLayoutStrategy(vtkChart::FILL_RECT);
  this->ZoomWithMouseWheelOff();

  for (int i = 0; i < 4; ++i)
  {
    this->GetAxis(i)->SetNumberOfTicks(0);
    this->GetAxis(i)->SetLabelsVisible(false);
    this->GetAxis(i)->SetTitle("");
  }
  this->GetAxis(vtkAxis::LEFT)->SetVisible(true);
  this->GetAxis(vtkAxis::LEFT)->SetMargins(20, 0);
  this->GetAxis(vtkAxis::RIGHT)->SetVisible(true);
  this->GetAxis(vtkAxis::RIGHT)->SetMargins(20, 0);
  this->GetAxis(vtkAxis::BOTTOM)->SetVisible(false);
  this->GetAxis(vtkAxis::BOTTOM)->SetMargins(0, 0);
  this->GetAxis(vtkAxis::TOP)->SetVisible(false);
  this->GetAxis(vtkAxis::TOP)->SetMargins(0, 0);

  //
  this->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);

  this->SetInteractive(false);
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsPreviewChart::SetColorTransferFunction(
  vtkColorTransferFunction* function)
{
  this->ClearPlots();

  vtkSmartPointer<vtkColorTransferFunctionItem> compositeVisibleItem =
    vtkSmartPointer<vtkColorTransferFunctionItem>::New();
  compositeVisibleItem->SetMaskAboveCurve(false);
  compositeVisibleItem->SetInteractive(false);
  compositeVisibleItem->SetOpacity(1);
  compositeVisibleItem->SelectableOff();
  if (function == CTK_NULLPTR)
  {
    vtkSmartPointer<vtkColorTransferFunction> ctf =
      vtkSmartPointer<vtkColorTransferFunction>::New();
    compositeVisibleItem->SetColorTransferFunction(ctf);
  }
  else
  {
    compositeVisibleItem->SetColorTransferFunction(function);
  }
  this->AddPlot(compositeVisibleItem);
}

// ----------------------------------------------------------------------------
vtkScalarsToColorsPreviewChart::~vtkScalarsToColorsPreviewChart()
{
}
