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
#include "vtkScalarsToColorsHistogramChart.h"

#include <vtkAxis.h>
#include <vtkObjectFactory.h>
#include <vtkPen.h>
#include <vtkPlotBar.h>
#include <vtkScalarsToColors.h>
#include <vtkTable.h>

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkScalarsToColorsHistogramChart)

// ----------------------------------------------------------------------------
vtkScalarsToColorsHistogramChart::vtkScalarsToColorsHistogramChart()
{
  this->ForceAxesToBoundsOn();
  this->SetLayoutStrategy(vtkChart::FILL_SCENE);

  for (int i = 0; i < 4; ++i)
  {
    this->GetAxis(i)->SetVisible(true);
    this->GetAxis(i)->SetNumberOfTicks(0);
    this->GetAxis(i)->SetLabelsVisible(false);
    this->GetAxis(i)->SetMargins(0, 0);
    this->GetAxis(i)->SetTitle("");
  }

  this->SetBarWidthFraction(1.0);

  // Set up the plot bar
  this->AddPlot(this->HistogramPlotBar.Get());

  this->HistogramPlotBar->GetPen()->SetLineType(vtkPen::NO_PEN);
  this->HistogramPlotBar->SetSelectable(false);
  this->HistogramPlotBar->SetInteractive(false);
}

// ----------------------------------------------------------------------------
vtkScalarsToColorsHistogramChart::~vtkScalarsToColorsHistogramChart()
{
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsHistogramChart::SetHistogramInputData(vtkTable* table,
    const char* xAxisColumn, const char* yAxisColumn)
{
  this->HistogramPlotBar->SetInputData(table, xAxisColumn, yAxisColumn);
  this->SelectColorArray(xAxisColumn);
  this->RecalculateBounds();
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsHistogramChart::SetScalarVisibility(bool visible)
{
  this->HistogramPlotBar->SetScalarVisibility(visible);
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsHistogramChart::SetLookupTable(vtkScalarsToColors* lut)
{
  this->HistogramPlotBar->SetLookupTable(lut);
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsHistogramChart::SelectColorArray(const char* arrayName)
{
  this->HistogramPlotBar->SelectColorArray(arrayName);
}
