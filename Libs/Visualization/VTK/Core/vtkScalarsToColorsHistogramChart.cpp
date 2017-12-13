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
#include <vtkDescriptiveStatistics.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
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

  this->LeftAxisMode = VTK_AUTO;

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
void vtkScalarsToColorsHistogramChart::SetLeftAxisMode(int mode)
{
  switch (mode)
  {
    case vtkScalarsToColorsHistogramChart::VTK_AUTO:
      this->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::AUTO);
      break;
    case vtkScalarsToColorsHistogramChart::MAXIMUM:
    case vtkScalarsToColorsHistogramChart::MEAN_PLUS_THREE_SIGMA:
      this->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
      break;
    default:
      return;
  }

  this->LeftAxisMode = mode;
}

// ----------------------------------------------------------------------------
int vtkScalarsToColorsHistogramChart::GetLeftAxisMode()
{
  return this->LeftAxisMode;
}

// ----------------------------------------------------------------------------
void vtkScalarsToColorsHistogramChart::SetHistogramInputData(vtkTable* table,
    const char* xAxisColumn, const char* yAxisColumn)
{
  if (this->LeftAxisMode == MAXIMUM
   || this->LeftAxisMode == MEAN_PLUS_THREE_SIGMA)
  {
    // compute histogram mean and standard deviation
    vtkNew<vtkDescriptiveStatistics> statisticsFilter;
    statisticsFilter->SetInputData(table);
    statisticsFilter->AddColumn(yAxisColumn);
    statisticsFilter->Update();
    vtkMultiBlockDataSet* stats = vtkMultiBlockDataSet::SafeDownCast(
      statisticsFilter->GetOutputDataObject(vtkStatisticsAlgorithm::OUTPUT_MODEL));
    vtkTable* statsPrimary = vtkTable::SafeDownCast(stats->GetBlock(0));
    vtkTable* statsDerived = vtkTable::SafeDownCast(stats->GetBlock(1));

    // update axis
    if (this->LeftAxisMode == MAXIMUM)
    {
      double max = statsPrimary->GetValueByName(0, "Maximum").ToDouble();
      this->GetAxis(vtkAxis::LEFT)->SetUnscaledRange(0, max);
    }
    else if (this->LeftAxisMode == MEAN_PLUS_THREE_SIGMA)
    {
      double mean = statsPrimary->GetValueByName(0, "Mean").ToDouble();
      double sigma = statsDerived->GetValueByName(0, "Standard Deviation").ToDouble();
      this->GetAxis(vtkAxis::LEFT)->SetUnscaledRange(0, mean + 3 * sigma);
    }
  }

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
