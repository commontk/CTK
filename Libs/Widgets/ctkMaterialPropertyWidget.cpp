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
#include <QListWidgetItem>
#include <QScrollBar>

// CTK includes
#include "ctkMaterialPropertyWidget.h"
#include "ctkMaterialPropertyPreviewLabel.h"
#include "ui_ctkMaterialPropertyWidget.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.libs.widgets.ctkMaterialPropertyWidget");

//-----------------------------------------------------------------------------
class ctkMaterialPropertyWidgetPrivate: public Ui_ctkMaterialPropertyWidget
{
  Q_DECLARE_PUBLIC(ctkMaterialPropertyWidget);
protected:
  ctkMaterialPropertyWidget* const q_ptr;
public:
  ctkMaterialPropertyWidgetPrivate(ctkMaterialPropertyWidget& object);
};

// --------------------------------------------------------------------------
ctkMaterialPropertyWidgetPrivate::ctkMaterialPropertyWidgetPrivate(ctkMaterialPropertyWidget& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
ctkMaterialPropertyWidget::ctkMaterialPropertyWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkMaterialPropertyWidgetPrivate(*this))
{
  Q_D(ctkMaterialPropertyWidget);
  
  d->setupUi(this);
  
  connect(d->PresetsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
          this, SLOT(selectPreset(QListWidgetItem*)));

  this->addPreset(1.,0.,0.,1.,"Full ambient eliminating all directional shading.");
  this->addPreset(0.2,1.,0.,1.,"Dull material properties (no specular lighting).");
  this->addPreset(0.1,0.9,0.2,10.,"Smooth material properties (moderate specular lighting).");
  this->addPreset(0.1,0.6,0.5,40.,"Shiny material properties (high specular lighting).");
  
  d->PresetsListWidget->viewport()->setAutoFillBackground( false);
  d->PresetsListWidget->setAutoFillBackground( false );
  d->PresetsListWidget->setMaximumHeight(
    d->MaterialPropertyPreviewLabel->sizeHint().height()
    + d->PresetsListWidget->horizontalScrollBar()->sizeHint().height()
    + 2. * d->PresetsListWidget->frameWidth());
}

// --------------------------------------------------------------------------
ctkMaterialPropertyWidget::~ctkMaterialPropertyWidget()
{
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setAmbient(double newAmbient)
{
  Q_D(const ctkMaterialPropertyWidget);
  d->DiffuseSliderSpinBox->setValue(newAmbient);
}

// --------------------------------------------------------------------------
double ctkMaterialPropertyWidget::ambient()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->DiffuseSliderSpinBox->value();
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setDiffuse(double newDiffuse)
{
  Q_D(const ctkMaterialPropertyWidget);
  d->DiffuseSliderSpinBox->setValue(newDiffuse);
}

// --------------------------------------------------------------------------
double ctkMaterialPropertyWidget::diffuse()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->DiffuseSliderSpinBox->value();
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setSpecular(double newSpecular)
{
  Q_D(const ctkMaterialPropertyWidget);
  d->SpecularSliderSpinBox->setValue(newSpecular);
}

// --------------------------------------------------------------------------
double ctkMaterialPropertyWidget::specular()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->SpecularSliderSpinBox->value();
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setSpecularPower(double newSpecularPower)
{
  Q_D(const ctkMaterialPropertyWidget);
  d->SpecularPowerSliderSpinBox->setValue(newSpecularPower);
}

// --------------------------------------------------------------------------
double ctkMaterialPropertyWidget::specularPower()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->SpecularPowerSliderSpinBox->value();
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::addPreset(
  double ambient, double diffuse, double specular, double power, const QString& label)
{
  Q_D(ctkMaterialPropertyWidget);
  d->PresetsListWidget->addItem("");
  QListWidgetItem* item = d->PresetsListWidget->item(d->PresetsListWidget->count()-1);
  item->setToolTip(label);
  item->setData(Qt::UserRole, ambient); 
  item->setData(Qt::UserRole + 1, diffuse);
  item->setData(Qt::UserRole + 2, specular);
  item->setData(Qt::UserRole + 3, power);
  ctkMaterialPropertyPreviewLabel* preset =
    new ctkMaterialPropertyPreviewLabel(ambient, diffuse, specular, power);
  preset->setColor(d->MaterialPropertyPreviewLabel->color());
  preset->setGridOpacity(d->MaterialPropertyPreviewLabel->gridOpacity());
  item->setSizeHint(preset->sizeHint());
  d->PresetsListWidget->setItemWidget(item, preset);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::selectPreset(QListWidgetItem* preset)
{
  Q_D(ctkMaterialPropertyWidget);
  d->AmbientSliderSpinBox->setValue(preset->data(Qt::UserRole).toDouble());
  d->DiffuseSliderSpinBox->setValue(preset->data(Qt::UserRole + 1).toDouble());
  d->SpecularSliderSpinBox->setValue(preset->data(Qt::UserRole + 2).toDouble());
  d->SpecularPowerSliderSpinBox->setValue(preset->data(Qt::UserRole + 3).toDouble());
}
