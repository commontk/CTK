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
#include <QDebug>
#include <QTimer>

// CTK includes
#include "ctkLogger.h"
#include "ctkUtils.h"
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

  bool blockSignals(bool);
  bool checkXRange(double x, int pointId);

  vtkControlPointsItem* CurrentControlPointsItem;
  bool EditColors;
  bool TopWidgetsVisible;
};

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsWidgetPrivate methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsWidgetPrivate::ctkVTKScalarsToColorsWidgetPrivate(
  ctkVTKScalarsToColorsWidget& object)
  : q_ptr(&object)
{
  this->CurrentControlPointsItem = 0;
  this->EditColors = true;
  this->TopWidgetsVisible = true;
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
  QObject::connect(this->View, SIGNAL(functionChanged()),
                   q, SLOT(resetRange()));

  this->PointIdSpinBox->setSpecialValueText("None");
  QObject::connect(this->PointIdSpinBox, SIGNAL(valueChanged(int)),
                   q, SLOT(onCurrentPointChanged(int)));
  this->PointIdSpinBox->setValue(-1);
  QObject::connect(this->ColorPickerButton, SIGNAL(colorChanged(QColor)),
                   q, SLOT(onColorChanged(QColor)));
  QObject::connect(this->XSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(onXChanged(double)));
  QObject::connect(this->OpacitySpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(onOpacityChanged(double)));
  QObject::connect(this->MidPointSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(onMidPointChanged(double)));
  QObject::connect(this->SharpnessSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(onSharpnessChanged(double)));
  this->ColorPickerButton->setVisible(false);
  this->XLabel->setVisible(false);
  this->XSpinBox->setVisible(false);
  this->OpacityLabel->setVisible(false);
  this->OpacitySpinBox->setVisible(false);
  this->SharpnessLabel->setVisible(false);
  this->SharpnessSpinBox->setVisible(false);
  this->MidPointLabel->setVisible(false);
  this->MidPointSpinBox->setVisible(false);
  QObject::connect(this->XRangeSlider, SIGNAL(valuesChanged(double,double)),
                   q, SLOT(setXRange(double,double)));
  QObject::connect(this->YRangeSlider, SIGNAL(valuesChanged(double,double)),
                   q, SLOT(setYRange(double,double)));
  QObject::connect(this->XRangeSlider, SIGNAL(valuesChanged(double,double)),
                   q, SIGNAL(axesModified()));
  QObject::connect(this->YRangeSlider, SIGNAL(valuesChanged(double,double)),
                   q, SIGNAL(axesModified()));

  QObject::connect(this->View, SIGNAL(extentChanged()),
                   q, SLOT(onAxesModified()));

  this->ExpandButton->setMirrorOnExpand(true);
  QObject::connect(this->ExpandButton, SIGNAL(clicked(bool)),
                   q, SLOT(onExpandButton(bool)));
}

// ----------------------------------------------------------------------------
bool ctkVTKScalarsToColorsWidgetPrivate::blockSignals(bool block)
{
  this->ColorPickerButton->blockSignals(block);
  this->XSpinBox->blockSignals(block);
  this->OpacitySpinBox->blockSignals(block);
  this->MidPointSpinBox->blockSignals(block);
  return this->SharpnessSpinBox->blockSignals(block);
}

// ----------------------------------------------------------------------------
bool ctkVTKScalarsToColorsWidgetPrivate::checkXRange(double x, int pointId)
{
  Q_Q(ctkVTKScalarsToColorsWidget);
  QPalette wrongPalette = q->palette();
  wrongPalette.setColor(QPalette::Highlight, Qt::red);
  if (pointId < 0 ||
      pointId >= this->PointIdSpinBox->maximum())
    {
    QTimer::singleShot(2000, q, SLOT(restorePalette()));
    return false;
    }
  if (pointId > 0)
    {
    double previous[4];
    this->CurrentControlPointsItem->GetControlPoint(pointId - 1, previous);
    if (x < previous[0])
      {
      this->XSpinBox->setPalette(wrongPalette);
      this->XSpinBox->selectAll();
      QTimer::singleShot(2000, q, SLOT(restorePalette()));
      this->XSpinBox->setValue(previous[0]);
      return false;
      }
    }
  if (pointId < this->PointIdSpinBox->maximum() - 1)
    {
    double next[4];
    this->CurrentControlPointsItem->GetControlPoint(pointId + 1, next);
    if (x > next[0])
      {
      this->XSpinBox->setPalette(wrongPalette);
      this->XSpinBox->selectAll();
      QTimer::singleShot(2000, q, SLOT(restorePalette()));
      this->XSpinBox->setValue(next[0]);
      return false;
      }
    }
  return true;
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
bool ctkVTKScalarsToColorsWidget::editColors()const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  return d->EditColors;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setEditColors(bool edit)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->EditColors = edit;
}

// ----------------------------------------------------------------------------
bool ctkVTKScalarsToColorsWidget::areTopWidgetsVisible()const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  return d->TopWidgetsVisible;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setTopWidgetsVisible(bool visible)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->ExpandButton->setVisible(visible);
  d->PointIdLabel->setVisible(visible);
  d->PointIdSpinBox->setVisible(visible);
  foreach(QWidget* widget, this->extraWidgets())
  {
    widget->setVisible(visible);
  }
  d->TopSpacer->changeSize(visible ? 40 : 0, visible ? 20 : 0);
  d->TopSpacer->invalidate();
  d->TopWidgetsVisible = visible;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onPlotAdded(vtkPlot* plot)
{
  vtkControlPointsItem* controlPoints = vtkControlPointsItem::SafeDownCast(plot);
  if (controlPoints)
    {
    this->setCurrentControlPointsItem(controlPoints);
    this->qvtkConnect(plot, vtkControlPointsItem::CurrentPointChangedEvent,
                      this, SLOT(setCurrentPoint(vtkObject*,void*)));
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onBoundsChanged()
{
  this->onAxesModified();
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setCurrentPoint(vtkObject* caller, void* callData)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  vtkControlPointsItem* controlPoints = reinterpret_cast<vtkControlPointsItem*>(caller);
  long newPoint = reinterpret_cast<long>(callData);
  if (!controlPoints || newPoint < -1)
    {
    return;
    }
  if (d->CurrentControlPointsItem != controlPoints)
    {
    this->setCurrentControlPointsItem(controlPoints);
    }
  else
    {
    // When a new point is added, the modified event is fired later.
    // however we need to update the max of the current spin box before
    // setting the new value.
    this->updateNumberOfPoints();
    }
  this->setCurrentPoint(newPoint);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setCurrentPoint(int newPoint)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->PointIdSpinBox->setValue(newPoint);
  Q_ASSERT( newPoint == d->PointIdSpinBox->value());
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
    d->ColorPickerButton->setVisible( d->EditColors && d->TopWidgetsVisible &&
      (vtkColorTransferControlPointsItem::SafeDownCast(item) != 0 ||
       vtkCompositeControlPointsItem::SafeDownCast(item) != 0));
    d->XLabel->setVisible(d->TopWidgetsVisible);
    d->XSpinBox->setVisible(d->TopWidgetsVisible);
    d->OpacityLabel->setVisible(d->TopWidgetsVisible &&
      (vtkPiecewiseControlPointsItem::SafeDownCast(item) != 0 ||
       vtkCompositeControlPointsItem::SafeDownCast(item) != 0));
    d->OpacitySpinBox->setVisible(d->TopWidgetsVisible &&
      (vtkPiecewiseControlPointsItem::SafeDownCast(item) != 0 ||
       vtkCompositeControlPointsItem::SafeDownCast(item) != 0));
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
  d->XSpinBox->setEnabled(currentPoint != -1);
  d->OpacitySpinBox->setEnabled(currentPoint != -1);
  d->MidPointSpinBox->setEnabled(currentPoint != -1);
  d->SharpnessSpinBox->setEnabled(currentPoint != -1);

  if (d->CurrentControlPointsItem)
    {
    this->updateCurrentPoint();
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::updateNumberOfPoints()
{
  Q_D(ctkVTKScalarsToColorsWidget);
  const int numberOfPoints = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetNumberOfPoints() : 0;
  d->PointIdSpinBox->setMaximum( numberOfPoints - 1 );
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::updateCurrentPoint()
{
  Q_D(ctkVTKScalarsToColorsWidget);
  Q_ASSERT(d->CurrentControlPointsItem);
  Q_ASSERT(d->PointIdSpinBox->value() == d->CurrentControlPointsItem->GetCurrentPoint());
  this->updateNumberOfPoints();

  int pointId = d->PointIdSpinBox->value();
  if (pointId == -1)
    {
    return;
    }

  double point[4] = {0.0};
  d->CurrentControlPointsItem->GetControlPoint(pointId, point);

  vtkAxis* xAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetXAxis() : d->View->chart()->GetAxis(vtkAxis::BOTTOM);
  Q_ASSERT(xAxis);
  if (xAxis && (xAxis->GetMinimumLimit() > point[0] || xAxis->GetMaximumLimit() < point[0]))
    {
    xAxis->SetMinimumLimit(qMin(xAxis->GetMinimumLimit(), point[0]));
    xAxis->SetMaximumLimit(qMax(xAxis->GetMaximumLimit(), point[0]));
    d->View->boundAxesToChartBounds();
    this->onAxesModified();
    }

  bool oldBlock = d->blockSignals(true);
  d->XSpinBox->setValue(point[0]);
  d->OpacitySpinBox->setValue(point[1]);
  d->MidPointSpinBox->setValue(point[2]);
  d->SharpnessSpinBox->setValue(point[3]);

  vtkColorTransferControlPointsItem* colorControlPoints =
    vtkColorTransferControlPointsItem::SafeDownCast(d->CurrentControlPointsItem);
  vtkCompositeControlPointsItem* compositeControlPoints =
    vtkCompositeControlPointsItem::SafeDownCast(d->CurrentControlPointsItem);
  if (colorControlPoints &&
      (!compositeControlPoints ||
        compositeControlPoints->GetPointsFunction() == vtkCompositeControlPointsItem::ColorPointsFunction ||
        compositeControlPoints->GetPointsFunction() == vtkCompositeControlPointsItem::ColorAndOpacityPointsFunction))
    {
    vtkColorTransferFunction* colorTF =
      colorControlPoints->GetColorTransferFunction();
    double xrgbms[6];
    colorTF->GetNodeValue(d->PointIdSpinBox->value(), xrgbms);
    QColor color = QColor::fromRgbF(xrgbms[1], xrgbms[2], xrgbms[3]);
    d->ColorPickerButton->setColor(color);
    }
  d->blockSignals(oldBlock);
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
void ctkVTKScalarsToColorsWidget::onXChanged(double x)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  if (!d->CurrentControlPointsItem)
    {
    return;
    }

  bool validRange = d->checkXRange(x, d->PointIdSpinBox->value());
  if (!validRange)
    {
    return;
    }
  double point[4];
  d->CurrentControlPointsItem->GetControlPoint(d->PointIdSpinBox->value(), point);
  point[0] = x;
  d->CurrentControlPointsItem->SetControlPoint(d->PointIdSpinBox->value(), point);
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
void ctkVTKScalarsToColorsWidget::xRange(double* range)const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  vtkAxis* xAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetXAxis() : d->View->chart()->GetAxis(vtkAxis::BOTTOM);
  Q_ASSERT(xAxis);
  range[0] = xAxis->GetMinimum();
  range[1] = xAxis->GetMaximum();
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setXRange(double min, double max)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  vtkAxis* xAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetXAxis() : d->View->chart()->GetAxis(vtkAxis::BOTTOM);
  Q_ASSERT(xAxis);
  if (xAxis->GetMinimum() != min || xAxis->GetMaximum() != max)
    {
    xAxis->SetRange(min, max);
    // Repaint the scene
    d->View->scene()->SetDirty(true);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::yRange(double* range)const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  vtkAxis* yAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetYAxis() : d->View->chart()->GetAxis(vtkAxis::LEFT);
  Q_ASSERT(yAxis);
  range[0] = yAxis->GetMinimum();
  range[1] = yAxis->GetMaximum();
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::setYRange(double min, double max)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  vtkAxis* yAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetYAxis() : d->View->chart()->GetAxis(vtkAxis::LEFT);
  Q_ASSERT(yAxis);
  if (yAxis->GetMinimum() != min || yAxis->GetMaximum() != max)
    {
    yAxis->SetRange(min, max);
    // Repaint the scene
    d->View->scene()->SetDirty(true);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::resetRange()
{
  Q_D(ctkVTKScalarsToColorsWidget);
  vtkAxis* xAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetXAxis() : d->View->chart()->GetAxis(vtkAxis::BOTTOM);
  if (xAxis)
    {
    this->setXRange(xAxis->GetMinimumLimit(), xAxis->GetMaximumLimit());
    }

  vtkAxis* yAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetYAxis() : d->View->chart()->GetAxis(vtkAxis::LEFT);
  if (yAxis)
    {
    this->setYRange(yAxis->GetMinimumLimit(), yAxis->GetMaximumLimit());
    }
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onAxesModified()
{
  Q_D(ctkVTKScalarsToColorsWidget);
  vtkAxis* xAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetXAxis() : d->View->chart()->GetAxis(vtkAxis::BOTTOM);
  Q_ASSERT(xAxis);

  bool wasBlocking = d->XRangeSlider->blockSignals(true);
  d->XRangeSlider->setRange(xAxis->GetMinimumLimit(), xAxis->GetMaximumLimit());
  d->XRangeSlider->setValues(xAxis->GetMinimum(), xAxis->GetMaximum());
  d->XSpinBox->setRange(xAxis->GetMinimumLimit(), xAxis->GetMaximumLimit());
  d->XSpinBox->setSingleStep(
    ctk::closestPowerOfTen(xAxis->GetMaximumLimit() - xAxis->GetMinimumLimit()) / 100);
  d->XRangeSlider->blockSignals(wasBlocking);

  vtkAxis* yAxis = d->CurrentControlPointsItem ?
    d->CurrentControlPointsItem->GetYAxis() : d->View->chart()->GetAxis(vtkAxis::LEFT);
  Q_ASSERT(yAxis);

  wasBlocking = d->YRangeSlider->blockSignals(true);
  d->YRangeSlider->setRange(yAxis->GetMinimumLimit(), yAxis->GetMaximumLimit());
  d->YRangeSlider->setValues(yAxis->GetMinimum(), yAxis->GetMaximum());
  d->YRangeSlider->blockSignals(wasBlocking);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::restorePalette()
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->XSpinBox->setPalette(this->palette());
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::onExpandButton(bool state)
{
  Q_D(ctkVTKScalarsToColorsWidget);
  d->MidPointLabel->setVisible(state);;
  d->MidPointSpinBox->setVisible(state);
  d->SharpnessLabel->setVisible(state);
  d->SharpnessSpinBox->setVisible(state);
}

// ----------------------------------------------------------------------------
QWidgetList ctkVTKScalarsToColorsWidget::extraWidgets()const
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  QWidgetList widgets;
  for (int i = 0; i < d->TopLayout->count(); ++i)
    {
    QLayoutItem* topLeftItem = d->TopLayout->itemAt(i);
    if (topLeftItem->spacerItem())
      {
      break;
      }
    widgets << topLeftItem->widget();
    }
  return widgets;
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsWidget::addExtraWidget(QWidget* extraWidget)
{
  Q_D(const ctkVTKScalarsToColorsWidget);
  d->TopLayout->insertWidget(this->extraWidgets().count(), extraWidget);
  if (!d->TopWidgetsVisible)
    {
    extraWidget->setVisible(d->TopWidgetsVisible);
    }
}
