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
#include <QDebug>

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKScalarsToColorsView.h"
#include "ctkVTKScalarsToColorsWidget.h"
#include "ui_ctkVTKScalarsToColorsWidget.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkColorTransferControlPointsItem.h>
#include <vtkColorTransferFunction.h>
#include <vtkColorTransferFunctionItem.h>
#include <vtkCompositeControlPointsItem.h>
#include <vtkCompositeTransferFunctionItem.h>
#include <vtkContextScene.h>
#include <vtkLookupTable.h>
#include <vtkLookupTableItem.h>
#include <vtkPiecewiseControlPointsItem.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPiecewiseFunctionItem.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKScalarsToColorsWidget");
//----------------------------------------------------------------------------

class ctkVTKScalarsToColorsWidgetPrivate:
  public Ui_ctkVTKScalarsToColorsWidget
{
   Q_DECLARE_PUBLIC(ctkVTKScalarsToColorsWidget);
protected:
  ctkVTKScalarsToColorsWidget* const q_ptr;
public:
  ctkVTKScalarsToColorsWidgetPrivate(ctkVTKScalarsToColorsWidget& object);
  void setupUi(QWidget* widget);
  vtkControlPointsItem* CurrentControlPointsItem;
};

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsWidgetPrivate methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsWidgetPrivate::ctkVTKScalarsToColorsWidgetPrivate(
  ctkVTKScalarsToColorsWidget& object)
  : q_ptr(&object)
{
  this->CurrentControlPointsItem = 0;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidgetPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkVTKScalarsToColorsWidget);
  this->Ui_ctkVTKScalarsToColorsWidget::setupUi(widget);
  QObject::connect(this->View, SIGNAL(plotAdded(vtkPlot*)),
                   q, SLOT(onPlotAdded(vtkPlot*)));
  QObject::connect(this->View, SIGNAL(boundsChanged()),
                   q, SLOT(onBoundsChanged()));

  this->PointIdSpinBox->setSpecialValueText("None");
  QObject::connect(this->PointIdSpinBox, SIGNAL(valueChanged(int)),
                   q, SLOT(onCurrentPointChanged(int)));
  this->PointIdSpinBox->setValue(-1);
  QObject::connect(this->ColorPickerButton, SIGNAL(colorChanged(const QColor&)),
                   q, SLOT(onColorChanged(const QColor&)));
  QObject::connect(this->OpacitySpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(onOpacityChanged(double)));
  QObject::connect(this->MidPointSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(onMidPointChanged(double)));
  QObject::connect(this->SharpnessSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(onSharpnessChanged(double)));
  this->ColorPickerButton->setVisible(false);
  this->OpacityLabel->setVisible(false);
  this->OpacitySpinBox->setVisible(false);
  QObject::connect(this->XRangeSlider, SIGNAL(valuesChanged(double, double)),
                   q, SLOT(onXRangeChanged(double, double)));
  QObject::connect(this->YRangeSlider, SIGNAL(valuesChanged(double, double)),
                   q, SLOT(onYRangeChanged(double, double)));
  q->qvtkConnect(this->View->chart()->GetAxis(0),vtkCommand::ModifiedEvent,
                    q, SLOT(onAxesModified()));
  q->qvtkConnect(this->View->chart()->GetAxis(1),vtkCommand::ModifiedEvent,
                    q, SLOT(onAxesModified()));

}

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsWidget methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsWidget::ctkVTKScalarsToColorsWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
   , d_ptr(new ctkVTKScalarsToColorsWidgetPrivate(*this))
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->setupUi(this);
}

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsWidget::~ctkVTKScalarsToColorsWidget()
{
}

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsView* ctkVTKScalarsToColorsWidget::view()const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  return d->View;
}

// ----------------------------------------------------------------------------
bool ctkVTKScalarsToColorsWidget::isHorizontalSliderVisible()const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  return d->XRangeSlider->isVisibleTo(
    const_cast<ctkVTKScalarsToColorsWidget*>(this));
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setHorizontalSliderVisible(bool visible)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->XRangeSlider->setVisible(visible);
}

// ----------------------------------------------------------------------------
bool ctkVTKScalarsToColorsWidget::isVerticalSliderVisible()const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  return d->YRangeSlider->isVisibleTo(
    const_cast<ctkVTKScalarsToColorsWidget*>(this));
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setVerticalSliderVisible(bool visible)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->YRangeSlider->setVisible(visible);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onPlotAdded(vtkPlot* plot)
{
  if (vtkControlPointsItem::SafeDownCast(plot))
    {
    this->qvtkConnect(plot, vtkControlPointsItem::CurrentPointChangedEvent,
                      this, SLOT(setCurrentPoint(vtkObject*, void*)));
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onBoundsChanged()
{
  Q_D(ctkVTKScalarsToColorsWidget);
  double bounds[8];
  d->View->chartBounds(bounds);
  d->XRangeSlider->setMinimum(bounds[vtkAxis::BOTTOM * 2]);
  d->XRangeSlider->setMaximum(bounds[vtkAxis::BOTTOM * 2 + 1]);
  d->YRangeSlider->setMinimum(bounds[vtkAxis::LEFT * 2]);
  d->YRangeSlider->setMaximum(bounds[vtkAxis::LEFT * 2 + 1]);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setCurrentPoint(vtkObject* caller, void* callData)
{
  vtkControlPointsItem* controlPoints = reinterpret_cast<vtkControlPointsItem*>(caller);
  long newPoint = reinterpret_cast<long>(callData);
  if (!controlPoints || newPoint < -1)
    {
    return;
    }
  this->setCurrentControlPointsItem(controlPoints);
  this->setCurrentPoint(newPoint);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setCurrentPoint(int newPoint)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->PointIdSpinBox->setValue(newPoint);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setCurrentControlPointsItem(vtkControlPointsItem* item)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  if (d->CurrentControlPointsItem == item)
    {
    return;
    }
  this->qvtkReconnect(d->CurrentControlPointsItem, item, vtkCommand::ModifiedEvent,
                      this, SLOT(updateCurrentPoint()));
  this->qvtkReconnect(d->CurrentControlPointsItem ?
                      d->CurrentControlPointsItem->GetXAxis() : d->View->chart()->GetAxis(0),
                      item ? item->GetXAxis() : d->View->chart()->GetAxis(0),
                      vtkCommand::ModifiedEvent,
                      this, SLOT(onAxesModified()));
  this->qvtkReconnect(d->CurrentControlPointsItem ?
                      d->CurrentControlPointsItem->GetYAxis() : d->View->chart()->GetAxis(1),
                      item ? item->GetYAxis() : d->View->chart()->GetAxis(1),
                      vtkCommand::ModifiedEvent,
                      this, SLOT(onAxesModified()));
  d->CurrentControlPointsItem = item;
  if (item)
    {
    d->ColorPickerButton->setVisible(
      vtkColorTransferControlPointsItem::SafeDownCast(item) != 0 ||
      vtkCompositeControlPointsItem::SafeDownCast(item) != 0);
    d->OpacityLabel->setVisible(vtkPiecewiseControlPointsItem::SafeDownCast(item) != 0 ||
                                vtkCompositeControlPointsItem::SafeDownCast(item) != 0);
    d->OpacitySpinBox->setVisible(vtkPiecewiseControlPointsItem::SafeDownCast(item) != 0 ||
                                  vtkCompositeControlPointsItem::SafeDownCast(item) != 0);
    this->onAxesModified();
    }
  d->PointIdSpinBox->setEnabled(item != 0);
  d->PointIdSpinBox->setMaximum((item ? item->GetNumberOfPoints() : 0) - 1);
  d->PointIdSpinBox->setValue(item ? item->GetCurrentPoint() : -1);
  this->updateCurrentPoint();
}

// ----------------------------------------------------------------------------
vtkControlPointsItem* ctkVTKScalarsToColorsWidget::currentControlPointsItem()const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  return d->CurrentControlPointsItem;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onCurrentPointChanged(int currentPoint)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  if (d->CurrentControlPointsItem)
    {
    d->CurrentControlPointsItem->SetCurrentPoint(currentPoint);
    }

  d->ColorPickerButton->setEnabled(currentPoint != -1);
  d->OpacitySpinBox->setEnabled(currentPoint != -1);
  d->MidPointSpinBox->setEnabled(currentPoint != -1);
  d->SharpnessSpinBox->setEnabled(currentPoint != -1);

  if (d->CurrentControlPointsItem)
    {
    this->updateCurrentPoint();
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::updateCurrentPoint()
{
  Q_D(ctkVTKScalarsToColorsWidget);
  Q_ASSERT(d->CurrentControlPointsItem);
  Q_ASSERT(d->PointIdSpinBox->value() == d->CurrentControlPointsItem->GetCurrentPoint());
  d->PointIdSpinBox->setMaximum((d->CurrentControlPointsItem ?
                                 d->CurrentControlPointsItem->GetNumberOfPoints() : 0) - 1);

  int pointId = d->PointIdSpinBox->value();
  if (pointId == -1)
    {
    return;
    }

  double point[4];
  d->CurrentControlPointsItem->GetControlPoint(pointId, point);
  d->OpacitySpinBox->setValue(point[1]);
  d->MidPointSpinBox->setValue(point[2]);
  d->SharpnessSpinBox->setValue(point[3]);

  vtkColorTransferControlPointsItem* colorControlPoints =
    vtkColorTransferControlPointsItem::SafeDownCast(d->CurrentControlPointsItem);
  if (colorControlPoints)
    {
    vtkColorTransferFunction* colorTF =
      colorControlPoints->GetColorTransferFunction();
    double xrgbms[6];
    colorTF->GetNodeValue(d->PointIdSpinBox->value(), xrgbms);
    QColor color = QColor::fromRgbF(xrgbms[1], xrgbms[2], xrgbms[3]);
    d->ColorPickerButton->setColor(color);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onColorChanged(const QColor& color)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  if (!color.isValid())
    {
    return;
    }
  Q_ASSERT(d->CurrentControlPointsItem);
  Q_ASSERT(d->PointIdSpinBox->value() != -1);
  Q_ASSERT(d->PointIdSpinBox->value() == d->CurrentControlPointsItem->GetCurrentPoint());

  vtkColorTransferControlPointsItem* colorControlPoints =
    vtkColorTransferControlPointsItem::SafeDownCast(d->CurrentControlPointsItem);
  if (colorControlPoints)
    {
    vtkColorTransferFunction* colorTF =
      colorControlPoints->GetColorTransferFunction();
    double point[6];
    colorTF->GetNodeValue(d->PointIdSpinBox->value(), point);
    point[1] = color.redF();
    point[2] = color.greenF();
    point[3] = color.blueF();
    colorTF->SetNodeValue(d->PointIdSpinBox->value(), point);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onOpacityChanged(double opacity)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  Q_ASSERT(d->CurrentControlPointsItem);

  double point[4];
  d->CurrentControlPointsItem->GetControlPoint(d->PointIdSpinBox->value(), point);
  point[1] = opacity;
  d->CurrentControlPointsItem->SetControlPoint(d->PointIdSpinBox->value(), point);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onMidPointChanged(double midPoint)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  Q_ASSERT(d->CurrentControlPointsItem);

  double point[4];
  d->CurrentControlPointsItem->GetControlPoint(d->PointIdSpinBox->value(), point);
  point[2] = midPoint;
  d->CurrentControlPointsItem->SetControlPoint(d->PointIdSpinBox->value(), point);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onSharpnessChanged(double sharpness)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  Q_ASSERT(d->CurrentControlPointsItem);

  double point[4];
  d->CurrentControlPointsItem->GetControlPoint(d->PointIdSpinBox->value(), point);
  point[3] = sharpness;
  d->CurrentControlPointsItem->SetControlPoint(d->PointIdSpinBox->value(), point);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onXRangeChanged(double min, double max)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  vtkAxis* xAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetXAxis() : d->View->chart()->GetAxis(vtkAxis::BOTTOM);
  Q_ASSERT(xAxis);
  xAxis->SetRange(min, max);
  d->View->scene()->SetDirty(true);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onYRangeChanged(double min, double max)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  vtkAxis* yAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetYAxis() : d->View->chart()->GetAxis(vtkAxis::LEFT);
  Q_ASSERT(yAxis);
  yAxis->SetRange(min, max);
  d->View->scene()->SetDirty(true);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onAxesModified()
{
  Q_D(ctkVTKScalarsToColorsWidget);
  vtkAxis* xAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetXAxis() : d->View->chart()->GetAxis(vtkAxis::BOTTOM);
  Q_ASSERT(xAxis);
  d->XRangeSlider->setValues(xAxis->GetMinimum(), xAxis->GetMaximum());
  vtkAxis* yAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetYAxis() : d->View->chart()->GetAxis(vtkAxis::LEFT);
  Q_ASSERT(yAxis);
  d->YRangeSlider->setValues(yAxis->GetMinimum(), yAxis->GetMaximum());
}
