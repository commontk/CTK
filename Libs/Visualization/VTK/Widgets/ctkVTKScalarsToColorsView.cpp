/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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
#include <QColorDialog>

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKScalarsToColorsView.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkColorTransferControlPointsItem.h>
#include <vtkColorTransferFunction.h>
#include <vtkColorTransferFunctionItem.h>
#include <vtkCompositeControlPointsItem.h>
#include <vtkCompositeTransferFunctionItem.h>
#include <vtkLookupTable.h>
#include <vtkLookupTableItem.h>
#include <vtkPiecewiseControlPointsItem.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPiecewiseFunctionItem.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKScalarsToColorsView");
//----------------------------------------------------------------------------

class ctkVTKScalarsToColorsViewPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKScalarsToColorsView);
protected:
  ctkVTKScalarsToColorsView* const q_ptr;
public:
  ctkVTKScalarsToColorsViewPrivate(ctkVTKScalarsToColorsView& object);
  void init();
  void updateChart();
};

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsViewPrivate methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsViewPrivate::ctkVTKScalarsToColorsViewPrivate(ctkVTKScalarsToColorsView& object)
  :q_ptr(&object)
{
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsViewPrivate::init()
{
  Q_Q(ctkVTKScalarsToColorsView);
  vtkChartXY* chart = q->chart();
  chart->SetAutoAxes(false);
  chart->SetHiddenAxisBorder(0);
  for (int i = 0; i < 4; ++i)
    {
    chart->GetAxis(i)->SetVisible(false);
    chart->GetAxis(i)->SetMinimumLimit(0.);
    chart->GetAxis(i)->SetMaximumLimit(1.);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsViewPrivate::updateChart()
{

}

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsView methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsView::ctkVTKScalarsToColorsView(QWidget* parentWidget)
  :ctkVTKChartView(parentWidget)
  , d_ptr(new ctkVTKScalarsToColorsViewPrivate(*this))
{
  Q_D(ctkVTKScalarsToColorsView);
  d->init();
}

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsView::~ctkVTKScalarsToColorsView()
{
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView::addPlot(vtkPlot* plot)
{
  if (vtkColorTransferControlPointsItem::SafeDownCast(plot))
    {
    this->qvtkConnect(plot, vtkControlPointsItem::CurrentPointEditEvent,
                      this, SLOT(editPoint(vtkObject*, void*)));
    }
  this->Superclass::addPlot(plot);
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView::addLookupTable(vtkLookupTable* lut)
{
  Q_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkLookupTableItem> item =
    vtkSmartPointer<vtkLookupTableItem>::New();
  item->SetLookupTable(lut);
  this->addPlot(item);
  d->updateChart();
  return item;
}
// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView::addColorTransferFunction(vtkColorTransferFunction* colorTF)
{
  Q_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkColorTransferFunctionItem> item =
    vtkSmartPointer<vtkColorTransferFunctionItem>::New();
  item->SetColorTransferFunction(colorTF);
  this->addPlot(item);
  vtkSmartPointer<vtkColorTransferControlPointsItem> controlPointsItem =
    vtkSmartPointer<vtkColorTransferControlPointsItem>::New();
  controlPointsItem->SetColorTransferFunction(colorTF);
  this->addPlot(controlPointsItem);
  d->updateChart();
  return item;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView::addOpacityFunction(vtkPiecewiseFunction* opacityTF)
{
  Q_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkPiecewiseFunctionItem> item =
    vtkSmartPointer<vtkPiecewiseFunctionItem>::New();
  item->SetPiecewiseFunction(opacityTF);
  QColor defaultColor = this->palette().highlight().color();
  item->SetColor(defaultColor.redF(), defaultColor.greenF(), defaultColor.blueF());
  item->SetMaskAboveCurve(true);
  this->addPlot(item);
  vtkSmartPointer<vtkPiecewiseControlPointsItem> controlPointsItem =
    vtkSmartPointer<vtkPiecewiseControlPointsItem>::New();
  controlPointsItem->SetPiecewiseFunction(opacityTF);
  this->addPlot(controlPointsItem);
  d->updateChart();
  return item;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addCompositeFunction(vtkColorTransferFunction* colorTF,
                       vtkPiecewiseFunction* opacityTF)
{
  Q_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkCompositeTransferFunctionItem> item =
    vtkSmartPointer<vtkCompositeTransferFunctionItem>::New();
  item->SetColorTransferFunction(colorTF);
  item->SetOpacityFunction(opacityTF);
  item->SetMaskAboveCurve(true);
  this->addPlot(item);
  vtkSmartPointer<vtkCompositeControlPointsItem> controlPointsItem =
    vtkSmartPointer<vtkCompositeControlPointsItem>::New();
  controlPointsItem->SetColorTransferFunction(colorTF);
  controlPointsItem->SetOpacityFunction(opacityTF);
  this->addPlot(controlPointsItem);

  d->updateChart();
  return item;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView::editPoint(vtkObject* caller, void* callData)
{
  vtkControlPointsItem* controlPoints = reinterpret_cast<vtkControlPointsItem*>(caller);
  int pointToEdit = reinterpret_cast<unsigned long>(callData);
  if (!controlPoints || pointToEdit < 0)
    {
    return;
    }
  vtkColorTransferControlPointsItem* colorTransferFunctionItem =
    vtkColorTransferControlPointsItem::SafeDownCast(controlPoints);
  if (colorTransferFunctionItem)
    {
    double xrgbms[6];
    vtkColorTransferFunction* colorTF = colorTransferFunctionItem->GetColorTransferFunction();
    colorTF->GetNodeValue(pointToEdit, xrgbms);
    QColor oldColor = QColor::fromRgbF(xrgbms[0], xrgbms[1], xrgbms[2]);
    QColor newColor = QColorDialog::getColor(oldColor, this);
    if (newColor.isValid())
      {
      xrgbms[1] = newColor.redF();
      xrgbms[2] = newColor.greenF();
      xrgbms[3] = newColor.blueF();
      colorTF->SetNodeValue(pointToEdit, xrgbms);
      }
    }
}
