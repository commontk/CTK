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
#include "ctkVTKVolumePropertyWidget.h"
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

  vtkVolumeProperty* VolumeProperty;
};

// ----------------------------------------------------------------------------
// ctkVTKVolumePropertyWidgetPrivate methods

// ----------------------------------------------------------------------------
ctkVTKVolumePropertyWidgetPrivate::ctkVTKVolumePropertyWidgetPrivate(
  ctkVTKVolumePropertyWidget& object)
  : q_ptr(&object)
{
  this->VolumeProperty = 0;
}

// ----------------------------------------------------------------------------
void ctkVTKVolumePropertyWidgetPrivate::setupUi(QWidget* widget)
{
  //Q_Q(ctkVTKVolumePropertyWidget);
  this->Ui_ctkVTKVolumePropertyWidget::setupUi(widget);

  this->ScalarOpacityWidget->view()->addCompositeFunction(0, 0, false, true);
  this->ScalarColorWidget->view()->addColorTransferFunction(0);
  this->GradientWidget->view()->addPiecewiseFunction(0);

  this->GradientGroupBox->setCollapsed(true);
  this->AdvancedGroupBox->setCollapsed(true);
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
  this->qvtkReconnect(d->VolumeProperty, newVolumeProperty, vtkCommand::ModifiedEvent,
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
    }

  d->ScalarOpacityWidget->view()->setOpacityFunctionToPlots(opacityFunction);
  d->ScalarOpacityWidget->view()->setColorTransferFunctionToPlots(colorTransferFunction);
  d->ScalarColorWidget->view()->setColorTransferFunctionToPlots(colorTransferFunction);
  d->GradientWidget->view()->setPiecewiseFunctionToPlots(gradientFunction);

  if (d->VolumeProperty)
    {
    d->MaterialPropertyLabel->setAmbient(d->VolumeProperty->GetAmbient());
    d->MaterialPropertyLabel->setDiffuse(d->VolumeProperty->GetDiffuse());
    d->MaterialPropertyLabel->setSpecular(d->VolumeProperty->GetSpecular());
    d->MaterialPropertyLabel->setSpecularPower(d->VolumeProperty->GetSpecularPower());
    d->InterpolationComboBox->setCurrentIndex(
      d->VolumeProperty->GetInterpolationType() == VTK_NEAREST_INTERPOLATION ? 0 : 1);
    }
}

