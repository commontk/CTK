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
#include <vtkIdTypeArray.h>
#include <vtkLookupTable.h>
#include <vtkLookupTableItem.h>
#include <vtkPen.h>
#include <vtkPiecewiseControlPointsItem.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPiecewiseFunctionItem.h>
#include <vtkSmartPointer.h>

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
  void updateBounds();
  void showBorders(bool);

  double ValidBounds[4];
};

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsViewPrivate methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsViewPrivate::ctkVTKScalarsToColorsViewPrivate(ctkVTKScalarsToColorsView& object)
  :q_ptr(&object)
{
  this->ValidBounds[0] = this->ValidBounds[2] = 0.;
  this->ValidBounds[1] = this->ValidBounds[3] = -1.;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsViewPrivate::init()
{
  Q_Q(ctkVTKScalarsToColorsView);
  vtkChartXY* chart = q->chart();
  chart->SetAutoAxes(false);
  chart->SetHiddenAxisBorder(0);
  this->showBorders(true);
  QObject::connect(q, SIGNAL(boundsChanged()), q, SLOT(onBoundsChanged()));
  q->onChartUpdated();
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsViewPrivate::showBorders(bool visible)
{
  Q_Q(ctkVTKScalarsToColorsView);
  vtkChartXY* chart = q->chart();
  for (int i = 0; i < 4; ++i)
    {
    chart->GetAxis(i)->SetVisible(true);
    chart->GetAxis(i)->GetPen()->SetOpacityF(0.3);
    chart->GetAxis(i)->SetTitle("");
    chart->GetAxis(i)->SetNumberOfTicks(0);
    chart->GetAxis(i)->SetLabelsVisible(false);
    chart->GetAxis(i)->SetMargins(7.,7.);
    if (visible)
      {
      chart->GetAxis(i)->SetBehavior(2);
      }
    else
      {
      chart->GetAxis(i)->SetBehavior(1);
      }
    }
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
                      this, SLOT(editPoint(vtkObject*,void*)));
    }
  this->Superclass::addPlot(plot);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView::onBoundsChanged()
{
  this->boundAxesToChartBounds();

  // Set range to bounds only if range is invalid. Otherwise keep it as is, because
  // the user wants to keep the range they set after a transfer function is shifted
  double extent[8];
  this->chartExtent(extent);
  if (extent[0] >= extent[1])
    {
    this->setAxesToChartBounds();
    }

  this->Superclass::onChartUpdated();
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView::addLookupTable(vtkLookupTable* lut)
{
  vtkSmartPointer<vtkLookupTableItem> item =
    vtkSmartPointer<vtkLookupTableItem>::New();
  item->SetLookupTable(lut);
  this->addPlot(item);
  return item;
}
// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addColorTransferFunction(vtkColorTransferFunction* colorTF,
                           bool editable)
{
  vtkSmartPointer<vtkColorTransferFunctionItem> item =
    vtkSmartPointer<vtkColorTransferFunctionItem>::New();
  item->SetColorTransferFunction(colorTF);
  this->addPlot(item);
  if (editable)
    {
    this->addColorTransferFunctionControlPoints(colorTF);
    }
  return item;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addOpacityFunction(vtkPiecewiseFunction* opacityTF,
                     bool editable)
{
  return this->addPiecewiseFunction(opacityTF, editable);
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addPiecewiseFunction(vtkPiecewiseFunction* piecewiseTF,
                       bool editable)
{
  vtkSmartPointer<vtkPiecewiseFunctionItem> item =
    vtkSmartPointer<vtkPiecewiseFunctionItem>::New();
  item->SetPiecewiseFunction(piecewiseTF);
  QColor defaultColor = this->palette().highlight().color();
  item->SetColor(defaultColor.redF(), defaultColor.greenF(), defaultColor.blueF());
  item->SetMaskAboveCurve(true);
  this->addPlot(item);
  if (editable)
    {
    this->addPiecewiseFunctionControlPoints(piecewiseTF);
    }
  return item;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addCompositeFunction(vtkColorTransferFunction* colorTF,
                       vtkPiecewiseFunction* opacityTF,
                       bool colorTFEditable, bool opacityTFEditable)
{
  vtkSmartPointer<vtkCompositeTransferFunctionItem> item =
    vtkSmartPointer<vtkCompositeTransferFunctionItem>::New();
  item->SetColorTransferFunction(colorTF);
  item->SetOpacityFunction(opacityTF);
  item->SetMaskAboveCurve(true);
  this->addPlot(item);
  if (colorTFEditable && opacityTFEditable)
    {
    this->addCompositeFunctionControlPoints(colorTF, opacityTF);
    }
  else if (colorTFEditable)
    {
    this->addColorTransferFunctionControlPoints(colorTF);
    }
  else if (opacityTFEditable)
    {
    this->addOpacityFunctionControlPoints(opacityTF);
    }
  return item;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addColorTransferFunctionControlPoints(vtkColorTransferFunction* colorTF)
{
  Q_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkColorTransferControlPointsItem> controlPointsItem =
    vtkSmartPointer<vtkColorTransferControlPointsItem>::New();
  controlPointsItem->SetColorTransferFunction(colorTF);
  controlPointsItem->SetValidBounds(d->ValidBounds);
  this->addPlot(controlPointsItem);
  return controlPointsItem;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addOpacityFunctionControlPoints(vtkPiecewiseFunction* opacityTF)
{
  return this->addPiecewiseFunctionControlPoints(opacityTF);
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addCompositeFunctionControlPoints(vtkColorTransferFunction* colorTF,
                                    vtkPiecewiseFunction* opacityTF)
{
  Q_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkCompositeControlPointsItem> controlPointsItem =
    vtkSmartPointer<vtkCompositeControlPointsItem>::New();
  controlPointsItem->SetColorTransferFunction(colorTF);
  controlPointsItem->SetOpacityFunction(opacityTF);
  controlPointsItem->SetValidBounds(d->ValidBounds);
  this->addPlot(controlPointsItem);
  return controlPointsItem;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView
::addPiecewiseFunctionControlPoints(vtkPiecewiseFunction* piecewiseTF)
{
  Q_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkPiecewiseControlPointsItem> controlPointsItem =
    vtkSmartPointer<vtkPiecewiseControlPointsItem>::New();
  controlPointsItem->SetPiecewiseFunction(piecewiseTF);
  controlPointsItem->SetValidBounds(d->ValidBounds);
  this->addPlot(controlPointsItem);
  return controlPointsItem;
}

// ----------------------------------------------------------------------------
QList<vtkPlot*> ctkVTKScalarsToColorsView::plots()const
{
  QList<vtkPlot*> res;
  const vtkIdType count = this->chart()->GetNumberOfPlots();
  for(vtkIdType i = 0; i < count; ++i)
    {
    res << this->chart()->GetPlot(i);
    }
  return res;
}

// ----------------------------------------------------------------------------
QList<vtkControlPointsItem*> ctkVTKScalarsToColorsView
::controlPointsItems()const
{
  QList<vtkControlPointsItem*> res;
  foreach(vtkPlot* plot, this->plots())
    {
    vtkControlPointsItem* controlPointsItem =
      vtkControlPointsItem::SafeDownCast(plot);
    if (controlPointsItem)
      {
      res << controlPointsItem;
      }
    }
  return res;
}

// ----------------------------------------------------------------------------
QList<vtkPlot*> ctkVTKScalarsToColorsView::lookupTablePlots()const
{
  QList<vtkPlot*> res;
  foreach(vtkPlot* plot, this->plots())
    {
    if (vtkLookupTableItem::SafeDownCast(plot))
      {
      res << plot;
      }
    }
  return res;
}

// ----------------------------------------------------------------------------
QList<vtkPlot*> ctkVTKScalarsToColorsView::lookupTablePlots(vtkLookupTable* lut)const
{
  QList<vtkPlot*> res;
  foreach(vtkPlot* plot, this->lookupTablePlots())
    {
    vtkLookupTableItem* item = vtkLookupTableItem::SafeDownCast(plot);
    if (item->GetLookupTable() == lut)
      {
      res << plot;
      }
    }
  return res;
}

// ----------------------------------------------------------------------------
QList<vtkPlot*> ctkVTKScalarsToColorsView::colorTransferFunctionPlots()const
{
  QList<vtkPlot*> res;
  foreach(vtkPlot* plot, this->plots())
    {
    if (vtkColorTransferFunctionItem::SafeDownCast(plot) ||
        vtkColorTransferControlPointsItem::SafeDownCast(plot))
      {
      res << plot;
      }
    }
  return res;
}

// ----------------------------------------------------------------------------
QList<vtkPlot*> ctkVTKScalarsToColorsView
::colorTransferFunctionPlots(vtkColorTransferFunction* colorTF)const
{
  QList<vtkPlot*> res;
  foreach(vtkPlot* plot, this->colorTransferFunctionPlots())
    {
    vtkColorTransferFunctionItem* item =
      vtkColorTransferFunctionItem::SafeDownCast(plot);
    if (item
        && item->GetColorTransferFunction() == colorTF)
      {
      res << plot;
      }
    vtkColorTransferControlPointsItem* controlPointsItem =
      vtkColorTransferControlPointsItem::SafeDownCast(plot);
    if (controlPointsItem
        && controlPointsItem->GetColorTransferFunction() == colorTF)
      {
      res << plot;
      }
    }
  return res;
}

// ----------------------------------------------------------------------------
QList<vtkPlot*> ctkVTKScalarsToColorsView::opacityFunctionPlots()const
{
  QList<vtkPlot*> res;
  foreach(vtkPlot* plot, this->plots())
    {
    if (vtkPiecewiseFunctionItem::SafeDownCast(plot) ||
        vtkPiecewiseControlPointsItem::SafeDownCast(plot) ||
        vtkCompositeTransferFunctionItem::SafeDownCast(plot) ||
        vtkCompositeControlPointsItem::SafeDownCast(plot))
      {
      res << plot;
      }
    }
  return res;
}

// ----------------------------------------------------------------------------
QList<vtkPlot*> ctkVTKScalarsToColorsView
::opacityFunctionPlots(vtkPiecewiseFunction* opacityTF)const
{
  QList<vtkPlot*> res;
  foreach(vtkPlot* plot, this->opacityFunctionPlots())
    {
    vtkPiecewiseFunctionItem* item =
      vtkPiecewiseFunctionItem::SafeDownCast(plot);
    if (item
        && item->GetPiecewiseFunction() == opacityTF)
      {
      res << plot;
      }
    vtkPiecewiseControlPointsItem* controlPointsItem =
      vtkPiecewiseControlPointsItem::SafeDownCast(plot);
    if (controlPointsItem
        && controlPointsItem->GetPiecewiseFunction() == opacityTF)
      {
      res << plot;
      }
    vtkCompositeTransferFunctionItem* compositeItem =
      vtkCompositeTransferFunctionItem::SafeDownCast(plot);
    if (compositeItem
        && compositeItem->GetOpacityFunction() == opacityTF)
      {
      res << plot;
      }
    vtkCompositeControlPointsItem* compositeControlPointsItem =
      vtkCompositeControlPointsItem::SafeDownCast(plot);
    if (compositeControlPointsItem
        && compositeControlPointsItem->GetOpacityFunction() == opacityTF)
      {
      res << plot;
      }
    }
  return res;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView::setLookuptTableToPlots(vtkLookupTable* lut)
{
  foreach(vtkLookupTableItem* plot,
          this->plots<vtkLookupTableItem>())
    {
    plot->SetLookupTable(lut);
    }
  this->onChartUpdated();
  emit functionChanged();
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView
::setColorTransferFunctionToPlots(vtkColorTransferFunction* colorTF)
{
  foreach(vtkColorTransferFunctionItem* plot,
          this->plots<vtkColorTransferFunctionItem>())
    {
    plot->SetColorTransferFunction(colorTF);
    }
  foreach(vtkColorTransferControlPointsItem* plot,
          this->plots<vtkColorTransferControlPointsItem>())
    {
    plot->SetColorTransferFunction(colorTF);
    }
  this->onChartUpdated();
  emit functionChanged();
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView
::setOpacityFunctionToPlots(vtkPiecewiseFunction* opacityTF)
{
  this->setPiecewiseFunctionToPlots(opacityTF);
  foreach(vtkCompositeTransferFunctionItem* plot,
          this->plots<vtkCompositeTransferFunctionItem>())
    {
    plot->SetOpacityFunction(opacityTF);
    }
  foreach(vtkCompositeControlPointsItem* plot,
          this->plots<vtkCompositeControlPointsItem>())
    {
    plot->SetOpacityFunction(opacityTF);
    }
  this->onChartUpdated();
  emit functionChanged();
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView
::setPiecewiseFunctionToPlots(vtkPiecewiseFunction* piecewiseTF)
{
  foreach(vtkPiecewiseFunctionItem* plot,
          this->plots<vtkPiecewiseFunctionItem>())
    {
    plot->SetPiecewiseFunction(piecewiseTF);
    }
  foreach(vtkPiecewiseControlPointsItem* plot,
          this->plots<vtkPiecewiseControlPointsItem>())
    {
    plot->SetPiecewiseFunction(piecewiseTF);
    }
  this->onChartUpdated();
  emit functionChanged();
}

// ----------------------------------------------------------------------------
bool ctkVTKScalarsToColorsView
::areBordersVisible()const
{
  return this->chart()->GetAxis(0)->GetVisible();
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView
::setBordersVisible(bool show)
{
  Q_D(ctkVTKScalarsToColorsView);
  d->showBorders(show);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView
::validBounds(double* bounds)const
{
  Q_D(const ctkVTKScalarsToColorsView);
  memcpy(bounds, d->ValidBounds, 4 * sizeof(double));
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView
::setValidBounds(double* bounds)
{
  Q_D(ctkVTKScalarsToColorsView);
  foreach(vtkControlPointsItem* plot, this->controlPointsItems())
    {
    plot->SetValidBounds(bounds);
    }
  memcpy(d->ValidBounds, bounds, 4 * sizeof(double));
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView
::setPlotsUserBounds(double* bounds)
{
  double plotBounds[4];
  this->chartBoundsToPlotBounds(bounds, plotBounds);
  foreach(vtkScalarsToColorsItem* plot,
          this->plots<vtkScalarsToColorsItem>())
    {
    plot->SetUserBounds(plotBounds);
    }
  foreach(vtkControlPointsItem* plot, this->controlPointsItems())
    {
    plot->SetUserBounds(plotBounds);
    }
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
  vtkCompositeControlPointsItem* compositeControlPoints =
    vtkCompositeControlPointsItem::SafeDownCast(controlPoints);
  if (colorTransferFunctionItem &&
      (!compositeControlPoints ||
        compositeControlPoints->GetPointsFunction() == vtkCompositeControlPointsItem::ColorPointsFunction ||
        compositeControlPoints->GetPointsFunction() == vtkCompositeControlPointsItem::ColorAndOpacityPointsFunction))
    {
    double xrgbms[6];
    vtkColorTransferFunction* colorTF = colorTransferFunctionItem->GetColorTransferFunction();
    colorTF->GetNodeValue(pointToEdit, xrgbms);
    QColor oldColor = QColor::fromRgbF(xrgbms[1], xrgbms[2], xrgbms[3]);
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

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView::boundAxesToChartBounds()
{
  this->Superclass::boundAxesToChartBounds();
  /// We only set the plot user bounds if the chart is using User bounds.
  double userBounds[8];
  this->chartUserBounds(userBounds);
  if (userBounds[0] < userBounds[1])
    {
    this->setPlotsUserBounds(userBounds);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView::moveAllPoints(double xOffset, double yOffset,
                                              bool dontMoveFirstAndLast)
{
  vtkVector2f offset(xOffset, yOffset);
  foreach(vtkControlPointsItem* controlPointsItem, this->controlPointsItems())
    {
    controlPointsItem->MovePoints(offset, dontMoveFirstAndLast);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsView::spreadAllPoints(double factor,
                                                bool dontSpreadFirstAndLast)
{
  foreach(vtkControlPointsItem* controlPointsItem, this->controlPointsItems())
    {
    controlPointsItem->SpreadPoints(factor, dontSpreadFirstAndLast);
    }
}
