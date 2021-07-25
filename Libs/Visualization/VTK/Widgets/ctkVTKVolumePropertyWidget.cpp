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
#include <QToolButton>

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKScalarsToColorsView.h"
#include "ctkVTKVolumePropertyWidget.h"
#include "ctkUtils.h"
#include "ui_ctkVTKVolumePropertyWidget.h"

// VTK includes
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
#include <vtkVolumeProperty.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKVolumePropertyWidget");
//----------------------------------------------------------------------------

class ctkVTKVolumePropertyWidgetPrivate:
  public Ui_ctkVTKVolumePropertyWidget
{
   Q_DECLARE_PUBLIC(ctkVTKVolumePropertyWidget);
protected:
  ctkVTKVolumePropertyWidget* const q_ptr;
public:
  ctkVTKVolumePropertyWidgetPrivate(ctkVTKVolumePropertyWidget& object);
  void setupUi(QWidget* widget);
  void computeIntensityRange(double* range);
  void updateThresholdSlider(vtkPiecewiseFunction* opacityFunction);
  void setThreshold(double min, double max, double opacity);

  vtkVolumeProperty* VolumeProperty;
  int                CurrentComponent;
  QToolButton*       ShowOpacityThresholdWidgetButton;
};

// ----------------------------------------------------------------------------
// ctkVTKVolumePropertyWidgetPrivate methods

// ----------------------------------------------------------------------------
ctkVTKVolumePropertyWidgetPrivate::ctkVTKVolumePropertyWidgetPrivate(
  ctkVTKVolumePropertyWidget& object)
  : q_ptr(&object)
{
  this->VolumeProperty = 0;
  this->CurrentComponent = 0;
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidgetPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkVTKVolumePropertyWidget);
  Q_ASSERT(q == widget);
  this->Ui_ctkVTKVolumePropertyWidget::setupUi(widget);

  double validBounds[4] = {VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 0., 1.};
  this->ScalarOpacityWidget->view()->setValidBounds(validBounds);
  this->ScalarColorWidget->view()->setValidBounds(validBounds);

  // Gradient magnitude is always positive
  double validPositiveBounds[4] = { 0, VTK_DOUBLE_MAX, 0., 1. };
  this->GradientWidget->view()->setValidBounds(validPositiveBounds);

  QObject::connect(this->ScalarOpacityWidget->view(), SIGNAL(extentChanged()),
                   q, SIGNAL(chartsExtentChanged()));

  this->ScalarOpacityWidget->view()->addCompositeFunction(0, 0, true, true);
  vtkCompositeControlPointsItem* composite =
  vtkCompositeControlPointsItem::SafeDownCast(
    this->ScalarOpacityWidget->view()->opacityFunctionPlots()[1]);
  composite->SetColorFill(true);
  composite->SetPointsFunction(vtkCompositeControlPointsItem::OpacityPointsFunction);
  this->ScalarColorWidget->view()->addColorTransferFunction(0);
  this->GradientWidget->view()->addPiecewiseFunction(0);

  this->ShowOpacityThresholdWidgetButton = new QToolButton;
  this->ShowOpacityThresholdWidgetButton->setIcon(
    QIcon(":Icons/threshold.png"));
  this->ShowOpacityThresholdWidgetButton->setCheckable(true);
  this->ShowOpacityThresholdWidgetButton->setAutoRaise(true);
  QObject::connect(this->ShowOpacityThresholdWidgetButton,
                   SIGNAL(toggled(bool)),
                   q, SLOT(onThresholdOpacityToggled(bool)));
  QObject::connect(this->ShowOpacityThresholdWidgetButton,
                   SIGNAL(toggled(bool)),
                   q, SIGNAL(thresholdEnabledChanged(bool)));
  this->ScalarOpacityWidget->addExtraWidget(
    this->ShowOpacityThresholdWidgetButton);
  this->ScalarOpacityThresholdWidget->setVisible(false);

  QObject::connect(this->ScalarOpacityWidget, SIGNAL(axesModified()),
                   q, SLOT(onAxesModified()), Qt::QueuedConnection);
  QObject::connect(this->ScalarColorWidget, SIGNAL(axesModified()),
                   q, SLOT(onAxesModified()), Qt::QueuedConnection);
  QObject::connect(this->GradientWidget, SIGNAL(axesModified()),
                   q, SLOT(onAxesModified()), Qt::QueuedConnection);

  this->GradientGroupBox->setCollapsed(true);
  this->AdvancedGroupBox->setCollapsed(true);

  QObject::connect(this->InterpolationComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(setInterpolationMode(int)));
  QObject::connect(this->ShadeCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(setShade(bool)));
  QObject::connect(this->MaterialPropertyWidget, SIGNAL(ambientChanged(double)),
                   q, SLOT(setAmbient(double)));
  QObject::connect(this->MaterialPropertyWidget, SIGNAL(diffuseChanged(double)),
                   q, SLOT(setDiffuse(double)));
  QObject::connect(this->MaterialPropertyWidget, SIGNAL(specularChanged(double)),
                   q, SLOT(setSpecular(double)));
  QObject::connect(this->MaterialPropertyWidget, SIGNAL(specularPowerChanged(double)),
                   q, SLOT(setSpecularPower(double)));
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidgetPrivate::computeIntensityRange(double* range)
{
  if (!this->VolumeProperty)
    {
    range[0] = 0.;
    range[1] = 1.;
    return;
    }
  range[0] = VTK_DOUBLE_MAX;
  range[1] = VTK_DOUBLE_MIN;

  Q_ASSERT(this->VolumeProperty->GetRGBTransferFunction(this->CurrentComponent));
  Q_ASSERT(this->VolumeProperty->GetScalarOpacity(this->CurrentComponent));
  Q_ASSERT(this->VolumeProperty->GetGradientOpacity(this->CurrentComponent));
  
  double colorRange[2] = {0., 1.};
  this->VolumeProperty->GetRGBTransferFunction(this->CurrentComponent)->GetRange(colorRange);
  range[0] = qMin(range[0], colorRange[0]);
  range[1] = qMax(range[1], colorRange[1]);

  double opacityRange[2] = {0., 1.};
  this->VolumeProperty->GetScalarOpacity(this->CurrentComponent)->GetRange(opacityRange);
  range[0] = qMin(range[0], opacityRange[0]);
  range[1] = qMax(range[1], opacityRange[1]);
}

// ----------------------------------------------------------------------------
// ctkVTKVolumePropertyWidget methods

// ----------------------------------------------------------------------------
ctkVTKVolumePropertyWidget::ctkVTKVolumePropertyWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
   , d_ptr(new ctkVTKVolumePropertyWidgetPrivate(*this))
{
  Q_D(ctkVTKVolumePropertyWidget);
  d->setupUi(this);
}

// ----------------------------------------------------------------------------
ctkVTKVolumePropertyWidget::~ctkVTKVolumePropertyWidget()
{
}

// ----------------------------------------------------------------------------
vtkVolumeProperty* ctkVTKVolumePropertyWidget::volumeProperty()const
{
  Q_D(const ctkVTKVolumePropertyWidget);
  return d->VolumeProperty;
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget
::setVolumeProperty(vtkVolumeProperty* newVolumeProperty)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (d->VolumeProperty == newVolumeProperty)
    {
    return;
    }
  this->qvtkReconnect(d->VolumeProperty, newVolumeProperty,
                      vtkCommand::ModifiedEvent,
                      this, SLOT(updateFromVolumeProperty()));
  d->VolumeProperty = newVolumeProperty;

  this->updateFromVolumeProperty();
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::updateFromVolumeProperty()
{
  Q_D(ctkVTKVolumePropertyWidget);
  vtkColorTransferFunction* colorTransferFunction = 0;
  vtkPiecewiseFunction* opacityFunction = 0;
  vtkPiecewiseFunction* gradientFunction = 0;
  bool scalarColorMapping = true;
  if (d->VolumeProperty)
    {
    colorTransferFunction =
      d->VolumeProperty->GetRGBTransferFunction()->GetSize() ?
      d->VolumeProperty->GetRGBTransferFunction() : 0;
    opacityFunction =
      d->VolumeProperty->GetScalarOpacity()->GetSize() ?
      d->VolumeProperty->GetScalarOpacity() : 0;
    gradientFunction =
      d->VolumeProperty->GetGradientOpacity()->GetSize() ?
      d->VolumeProperty->GetGradientOpacity() : 0;
    scalarColorMapping = d->VolumeProperty->GetIndependentComponents();
    }

  d->ScalarOpacityThresholdWidget->setPiecewiseFunction(
    this->isThresholdEnabled() ? opacityFunction : 0);

  this->qvtkDisconnect(0, vtkCommand::EndInteractionEvent,
                       this, SLOT(updateRange()));
  this->qvtkConnect(opacityFunction, vtkCommand::EndInteractionEvent,
                    this, SLOT(updateRange()), 0., Qt::QueuedConnection);
  this->qvtkConnect(opacityFunction, vtkCommand::EndEvent,
                    this, SLOT(updateRange()), 0., Qt::QueuedConnection);
  this->qvtkConnect(colorTransferFunction, vtkCommand::EndInteractionEvent,
                    this, SLOT(updateRange()), 0., Qt::QueuedConnection);
  this->qvtkConnect(colorTransferFunction, vtkCommand::EndEvent,
                    this, SLOT(updateRange()), 0., Qt::QueuedConnection);
  this->qvtkConnect(gradientFunction, vtkCommand::EndInteractionEvent,
                    this, SLOT(updateRange()), 0., Qt::QueuedConnection);
  this->qvtkConnect(gradientFunction, vtkCommand::EndEvent,
                    this, SLOT(updateRange()), 0., Qt::QueuedConnection);

  d->ScalarOpacityWidget->view()->setOpacityFunctionToPlots(opacityFunction);
  d->ScalarOpacityWidget->view()->setColorTransferFunctionToPlots(colorTransferFunction);
  d->ScalarColorWidget->view()->setColorTransferFunctionToPlots(colorTransferFunction);
  d->GradientWidget->view()->setPiecewiseFunctionToPlots(gradientFunction);

  // If color is specified by dependent components (RGBA volume) then do not show scalar color transfer
  // function, because that function has no effect.
  d->ScalarColorGroupBox->setVisible(scalarColorMapping);

  if (d->VolumeProperty)
    {
    d->InterpolationComboBox->setCurrentIndex(
      d->VolumeProperty->GetInterpolationType() == VTK_NEAREST_INTERPOLATION ? 0 : 1);
    d->ShadeCheckBox->setChecked(d->VolumeProperty->GetShade(d->CurrentComponent));
    d->MaterialPropertyWidget->setAmbient(d->VolumeProperty->GetAmbient(d->CurrentComponent));
    d->MaterialPropertyWidget->setDiffuse(d->VolumeProperty->GetDiffuse(d->CurrentComponent));
    d->MaterialPropertyWidget->setSpecular(d->VolumeProperty->GetSpecular(d->CurrentComponent));
    d->MaterialPropertyWidget->setSpecularPower(d->VolumeProperty->GetSpecularPower(d->CurrentComponent));
    }
  this->updateRange();
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::updateRange()
{
  Q_D(ctkVTKVolumePropertyWidget);

  double range[2] = {0.0};
  d->computeIntensityRange(range);
  d->ScalarOpacityThresholdWidget->setRange(range[0], range[1]);

  // Elements: {leftMin, leftMax, bottomMin, bottomMax, rightMin, rightMax, topMin, topMax}
  // (probably according to vtkAxis::Location)
  double chartBounds[8] = {0.0};
  d->ScalarOpacityWidget->view()->chartBounds(chartBounds);
  chartBounds[2] = range[0];
  chartBounds[3] = range[1];
  d->ScalarOpacityWidget->view()->setChartUserBounds(chartBounds);
  d->ScalarOpacityWidget->view()->update();

  d->ScalarColorWidget->view()->chartBounds(chartBounds);
  chartBounds[2] = range[0];
  chartBounds[3] = range[1];
  d->ScalarColorWidget->view()->setChartUserBounds(chartBounds);
  d->ScalarColorWidget->view()->update();
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::chartsBounds(double bounds[4])const
{
  Q_D(const ctkVTKVolumePropertyWidget);

  double chartBounds[8] = {0.0};
  d->ScalarOpacityWidget->view()->chartBounds(chartBounds);
  memcpy(bounds, chartBounds, 4*sizeof(double));

  d->ScalarColorWidget->view()->chartBounds(chartBounds);
  bounds[0] = qMin(bounds[0], chartBounds[0]);
  bounds[1] = qMax(bounds[1], chartBounds[1]);
  bounds[2] = qMin(bounds[2], chartBounds[2]);
  bounds[3] = qMax(bounds[3], chartBounds[3]);
}

void ctkVTKVolumePropertyWidget::chartsGradientBounds(double bounds[4])const
{
  Q_D(const ctkVTKVolumePropertyWidget);

  double chartBounds[8] = { 0.0 };
  d->GradientWidget->view()->chartBounds(chartBounds);
  memcpy(bounds, chartBounds, 4 * sizeof(double));
}

// ----------------------------------------------------------------------------
QList<double> ctkVTKVolumePropertyWidget::chartsBounds()const
{
  double boundsArray[4] = {0.0};
  this->chartsBounds(boundsArray);

  QList<double> bounds;
  bounds << boundsArray[0] << boundsArray[1] << boundsArray[2] << boundsArray[3];
  return bounds;
}

// ----------------------------------------------------------------------------
QList<double> ctkVTKVolumePropertyWidget::chartsGradientBounds()const
{
  double boundsArray[4] = { 0.0 };
  this->chartsGradientBounds(boundsArray);

  QList<double> bounds;
  bounds << boundsArray[0] << boundsArray[1] << boundsArray[2] << boundsArray[3];
  return bounds;
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setChartsExtent(double extent[2])
{
  this->setChartsExtent(extent[0], extent[1]);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setChartsGradientExtent(double extent[2])
{
  this->setChartsGradientExtent(extent[0], extent[1]);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setChartsExtent(double min, double max)
{
  Q_D(ctkVTKVolumePropertyWidget);

  double chartExtent[8] = {0.0};
  d->ScalarOpacityWidget->view()->chartExtent(chartExtent);
  chartExtent[0] = min;
  chartExtent[1] = max;
  d->ScalarOpacityWidget->view()->setChartUserExtent(chartExtent);
  d->ScalarOpacityWidget->view()->update();

  d->ScalarColorWidget->view()->chartExtent(chartExtent);
  chartExtent[0] = min;
  chartExtent[1] = max;
  d->ScalarColorWidget->view()->setChartUserExtent(chartExtent);
  d->ScalarColorWidget->view()->update();
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setChartsGradientExtent(double min, double max)
{
  Q_D(ctkVTKVolumePropertyWidget);

  double chartExtent[8] = { 0.0 };
  d->GradientWidget->view()->chartExtent(chartExtent);
  chartExtent[0] = min;
  chartExtent[1] = max;
  d->GradientWidget->view()->setChartUserExtent(chartExtent);
  d->GradientWidget->view()->update();
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::chartsExtent(double extent[4])const
{
  Q_D(const ctkVTKVolumePropertyWidget);

  double chartExtent[8] = {0.0};
  d->ScalarOpacityWidget->view()->chartExtent(chartExtent);
  memcpy(extent, chartExtent, 4*sizeof(double));

  d->ScalarColorWidget->view()->chartExtent(chartExtent);
  extent[0] = qMin(extent[0], chartExtent[0]);
  extent[1] = qMax(extent[1], chartExtent[1]);
  extent[2] = qMin(extent[2], chartExtent[2]);
  extent[3] = qMax(extent[3], chartExtent[3]);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::chartsGradientExtent(double extent[4])const
{
  Q_D(const ctkVTKVolumePropertyWidget);
  double chartExtent[8] = { 0.0 };
  d->GradientWidget->view()->chartExtent(chartExtent);
  memcpy(extent, chartExtent, 4 * sizeof(double));
}

// ----------------------------------------------------------------------------
QList<double> ctkVTKVolumePropertyWidget::chartsExtent()const
{
  double extentArray[4] = {0.0};
  this->chartsExtent(extentArray);

  QList<double> extent;
  extent << extentArray[0] << extentArray[1] << extentArray[2] << extentArray[3];
  return extent;
}

// ----------------------------------------------------------------------------
QList<double> ctkVTKVolumePropertyWidget::chartsGradientExtent()const
{
  double extentArray[4] = { 0.0 };
  this->chartsGradientExtent(extentArray);

  QList<double> extent;
  extent << extentArray[0] << extentArray[1] << extentArray[2] << extentArray[3];
  return extent;
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setInterpolationMode(int mode)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (!d->VolumeProperty)
    {
    return;
    }
  d->VolumeProperty->SetInterpolationType(mode);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setShade(bool enable)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (!d->VolumeProperty)
    {
    return;
    }
  d->VolumeProperty->SetShade(d->CurrentComponent, enable);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setAmbient(double value)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (!d->VolumeProperty)
    {
    return;
    }
  d->VolumeProperty->SetAmbient(d->CurrentComponent, value);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setDiffuse(double value)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (!d->VolumeProperty)
    {
    return;
    }
  d->VolumeProperty->SetDiffuse(d->CurrentComponent, value);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setSpecular(double value)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (!d->VolumeProperty)
    {
    return;
    }
  d->VolumeProperty->SetSpecular(d->CurrentComponent, value);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setSpecularPower(double value)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (!d->VolumeProperty)
    {
    return;
    }
  d->VolumeProperty->SetSpecularPower(d->CurrentComponent, value);
}

// ----------------------------------------------------------------------------
bool ctkVTKVolumePropertyWidget::isThresholdEnabled()const
{
  Q_D(const ctkVTKVolumePropertyWidget);
  return d->ScalarOpacityThresholdWidget->isVisibleTo(
    const_cast<ctkVTKVolumePropertyWidget*>(this));
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setThresholdEnabled(bool enable)
{
  Q_D(ctkVTKVolumePropertyWidget);
  d->ShowOpacityThresholdWidgetButton->setChecked(enable);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::onThresholdOpacityToggled(bool enable)
{
  Q_D(ctkVTKVolumePropertyWidget);
  d->ScalarOpacityThresholdWidget->setVisible(enable);
  this->updateFromVolumeProperty();
}

// ----------------------------------------------------------------------------
bool ctkVTKVolumePropertyWidget::isThresholdToggleVisible()const
{
  Q_D(const ctkVTKVolumePropertyWidget);
  return d->ShowOpacityThresholdWidgetButton->isVisibleTo(
    const_cast<ctkVTKVolumePropertyWidget*>(this));
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::setThresholdToggleVisible(bool showToggle)
{
  Q_D(ctkVTKVolumePropertyWidget);
  d->ShowOpacityThresholdWidgetButton->setVisible(showToggle);
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::onAxesModified()
{
  Q_D(ctkVTKVolumePropertyWidget);
  //return;
  ctkVTKScalarsToColorsWidget* senderWidget =
    qobject_cast<ctkVTKScalarsToColorsWidget*>(this->sender());
  
  double xRange[2] = {0.,0.};
  senderWidget->xRange(xRange);
  if (senderWidget != d->ScalarOpacityWidget)
    {
    bool wasBlocking = d->ScalarOpacityWidget->blockSignals(true);
    d->ScalarOpacityWidget->setXRange(xRange[0], xRange[1]);
    d->ScalarOpacityWidget->blockSignals(wasBlocking);
    }
  if (senderWidget != d->ScalarColorWidget)
    {
    bool wasBlocking = d->ScalarColorWidget->blockSignals(true);
    d->ScalarColorWidget->setXRange(xRange[0], xRange[1]);
    d->ScalarColorWidget->blockSignals(wasBlocking);
    }
  if (senderWidget != d->GradientWidget)
    {
    bool wasBlocking = d->GradientWidget->blockSignals(true);
    d->GradientWidget->setXRange(xRange[0], xRange[1]);
    d->GradientWidget->blockSignals(wasBlocking);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::moveAllPoints(double xOffset, double yOffset,
                                               bool dontMoveFirstAndLast)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (d->VolumeProperty)
    {
    d->VolumeProperty->InvokeEvent(vtkCommand::StartEvent);
    }
  d->ScalarOpacityWidget->view()
    ->moveAllPoints(xOffset, yOffset, dontMoveFirstAndLast);
  d->ScalarColorWidget->view()
    ->moveAllPoints(xOffset, yOffset, dontMoveFirstAndLast);
  if (d->VolumeProperty)
    {
    d->VolumeProperty->InvokeEvent(vtkCommand::EndEvent);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::moveAllGradientPoints(double xOffset, double yOffset,
                                               bool dontMoveFirstAndLast)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (d->VolumeProperty)
    {
    d->VolumeProperty->InvokeEvent(vtkCommand::StartEvent);
    }
  d->GradientWidget->view()
    ->moveAllPoints(xOffset, yOffset, dontMoveFirstAndLast);
  if (d->VolumeProperty)
    {
    d->VolumeProperty->InvokeEvent(vtkCommand::EndEvent);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::spreadAllPoints(double factor,
                                                 bool dontSpreadFirstAndLast)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (d->VolumeProperty)
    {
    d->VolumeProperty->InvokeEvent(vtkCommand::StartEvent);
    }
  d->ScalarOpacityWidget->view()
    ->spreadAllPoints(factor, dontSpreadFirstAndLast);
  d->ScalarColorWidget->view()
    ->spreadAllPoints(factor, dontSpreadFirstAndLast);
  if (d->VolumeProperty)
    {
    d->VolumeProperty->InvokeEvent(vtkCommand::EndEvent);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidget::spreadAllGradientPoints(double factor,
                                                 bool dontSpreadFirstAndLast)
{
  Q_D(ctkVTKVolumePropertyWidget);
  if (d->VolumeProperty)
    {
    d->VolumeProperty->InvokeEvent(vtkCommand::StartEvent);
    }
  d->GradientWidget->view()
    ->spreadAllPoints(factor, dontSpreadFirstAndLast);
  if (d->VolumeProperty)
    {
    d->VolumeProperty->InvokeEvent(vtkCommand::EndEvent);
    }
}
