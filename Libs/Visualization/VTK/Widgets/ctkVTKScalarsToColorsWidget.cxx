/*=========================================================================

  Library:   CTK

  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKScalarsToColorsView.h"
#include "ctkVTKScalarsToColorsWidget.h"
#include "ui_ctkVTKScalarsToColorsWidget.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkColorTransferFunction.h>
#include <vtkColorTransferFunctionItem.h>
#include <vtkCompositeTransferFunctionItem.h>
#include <vtkLookupTable.h>
#include <vtkLookupTableItem.h>
#include <vtkPiecewiseControlPointsItem.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPiecewiseFunctionItem.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKScalarsToColorsWidget");
//----------------------------------------------------------------------------

class ctkVTKScalarsToColorsWidgetPrivate:
  public Ui_ctkVTKScalarsToColorsWidget,
  public ctkPrivate<ctkVTKScalarsToColorsWidget>
{
public:
  ctkVTKScalarsToColorsWidgetPrivate();
  void setupUi();
};

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsWidgetPrivate methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsWidgetPrivate::ctkVTKScalarsToColorsWidgetPrivate()
{
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidgetPrivate::setupUi()
{
  CTK_P(ctkVTKScalarsToColorsWidget);
  this->Ui_ctkVTKScalarsToColorsWidget->setupUi();
}

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsWidget methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsWidget::ctkVTKScalarsToColorsWidget(QWidget* parentWidget)
  :ctkVTKChartWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkVTKScalarsToColorsWidget);
  CTK_D(ctkVTKScalarsToColorsWidget);
  d->setupUi();
}

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsWidget::~ctkVTKScalarsToColorsWidget()
{
}

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsView* ctkVTKScalarsToColorsWidget::view()const
{
  CTK_D(ctkVTKScalarsToColorsWidget);
  return d->View;
}
