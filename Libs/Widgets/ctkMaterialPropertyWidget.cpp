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
  this->PresetsListWidget = 0;
}

// --------------------------------------------------------------------------
ctkMaterialPropertyWidget::ctkMaterialPropertyWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkMaterialPropertyWidgetPrivate(*this))
{
  Q_D(ctkMaterialPropertyWidget);
  
  d->setupUi(this);

  connect(d->ColorPickerButton, SIGNAL(colorChanged(QColor)),
          this, SLOT(onColorChanged(QColor)));
  connect(d->OpacitySliderSpinBox, SIGNAL(valueChanged(double)),
          this, SLOT(onOpacityChanged(double)));
  
  connect(d->AmbientSliderSpinBox, SIGNAL(valueChanged(double)),
          this, SLOT(onAmbientChanged(double)));
  connect(d->DiffuseSliderSpinBox, SIGNAL(valueChanged(double)),
          this, SLOT(onDiffuseChanged(double)));
  connect(d->SpecularSliderSpinBox, SIGNAL(valueChanged(double)),
          this, SLOT(onSpecularChanged(double)));
  connect(d->SpecularPowerSliderSpinBox, SIGNAL(valueChanged(double)),
          this, SLOT(onSpecularPowerChanged(double)));

  connect(d->BackfaceCullingCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(onBackfaceCullingChanged(bool)));

  connect(d->PresetsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
          this, SLOT(selectPreset(QListWidgetItem*)));
  // default presets
  this->addPreset(QColor(),1.,1.,0.,0.,1.,"Full ambient eliminating all directional shading.");
  this->addPreset(QColor(),1.,0.2,1.,0.,1.,"Dull material properties (no specular lighting).");
  this->addPreset(QColor(),1.,0.1,0.9,0.2,10.,"Smooth material properties (moderate specular lighting).");
  this->addPreset(QColor(),1.,0.1,0.6,0.5,40.,"Shiny material properties (high specular lighting).");
  
  d->PresetsListWidget->viewport()->setAutoFillBackground( false);
  d->PresetsListWidget->setAutoFillBackground( false );
  d->PresetsListWidget->setMinimumWidth(
    d->PresetsListWidget->frameWidth() // left frame width
    + d->PresetsListWidget->count() * d->MaterialPropertyPreviewLabel->sizeHint().width()
    + d->PresetsListWidget->frameWidth() ); // right frame width
}

// --------------------------------------------------------------------------
ctkMaterialPropertyWidget::~ctkMaterialPropertyWidget()
{
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setColor(const QColor& newColor)
{
  Q_D(const ctkMaterialPropertyWidget);
  d->ColorPickerButton->setColor(newColor);
}

// --------------------------------------------------------------------------
QColor ctkMaterialPropertyWidget::color()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->ColorPickerButton->color();
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setOpacity(double newOpacity)
{
  Q_D(const ctkMaterialPropertyWidget);
  d->OpacitySliderSpinBox->setValue(newOpacity);
}

// --------------------------------------------------------------------------
double ctkMaterialPropertyWidget::opacity()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->OpacitySliderSpinBox->value();
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setAmbient(double newAmbient)
{
  Q_D(const ctkMaterialPropertyWidget);
  d->AmbientSliderSpinBox->setValue(newAmbient);
}

// --------------------------------------------------------------------------
double ctkMaterialPropertyWidget::ambient()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->AmbientSliderSpinBox->value();
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
void ctkMaterialPropertyWidget::setBackfaceCulling(bool newBackfaceCulling)
{
  Q_D(const ctkMaterialPropertyWidget);
  d->BackfaceCullingCheckBox->setChecked(newBackfaceCulling);
}

// --------------------------------------------------------------------------
bool ctkMaterialPropertyWidget::backfaceCulling()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->BackfaceCullingCheckBox->isChecked();
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::onColorChanged(const QColor& newColor)
{
  emit colorChanged(newColor);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::onOpacityChanged(double newOpacity)
{
  emit opacityChanged(newOpacity);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::onAmbientChanged(double newAmbient)
{
  emit ambientChanged(newAmbient);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::onDiffuseChanged(double newDiffuse)
{
  emit diffuseChanged(newDiffuse);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::onSpecularChanged(double newSpecular)
{
  emit specularChanged(newSpecular);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::onSpecularPowerChanged(double newSpecularPower)
{
  emit specularPowerChanged(newSpecularPower);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::onBackfaceCullingChanged(bool newBackfaceCulling)
{
  emit backfaceCullingChanged(newBackfaceCulling);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::addPreset(
  const QColor& color, double opacity,
  double ambient, double diffuse, double specular, double power,
  const QString& label)
{
  Q_D(ctkMaterialPropertyWidget);
  d->PresetsListWidget->addItem("");
  QListWidgetItem* item = d->PresetsListWidget->item(d->PresetsListWidget->count()-1);
  item->setToolTip(label);
  if (color.isValid())
    {
    item->setData(Qt::UserRole, color);
    }
  item->setData(Qt::UserRole + 1, opacity);
  item->setData(Qt::UserRole + 2, ambient);
  item->setData(Qt::UserRole + 3, diffuse);
  item->setData(Qt::UserRole + 4, specular);
  item->setData(Qt::UserRole + 5, power);
  ctkMaterialPropertyPreviewLabel* preset =
    new ctkMaterialPropertyPreviewLabel(color, opacity, ambient, diffuse, specular, power);
  if (!color.isValid())
    {
    connect(this, SIGNAL(colorChanged(QColor)),
            preset, SLOT(setColor(QColor)));
    preset->setColor(this->color());
    }
  preset->setGridOpacity(d->MaterialPropertyPreviewLabel->gridOpacity());
  item->setSizeHint(preset->sizeHint());
  d->PresetsListWidget->setItemWidget(item, preset);
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::selectPreset(QListWidgetItem* preset)
{
  Q_D(ctkMaterialPropertyWidget);
  if (preset->data(Qt::UserRole).isValid())
    {
    d->ColorPickerButton->setColor(preset->data(Qt::UserRole).value<QColor>());
    }
  d->OpacitySliderSpinBox->setValue(preset->data(Qt::UserRole + 1).toDouble());
  d->AmbientSliderSpinBox->setValue(preset->data(Qt::UserRole + 2).toDouble());
  d->DiffuseSliderSpinBox->setValue(preset->data(Qt::UserRole + 3).toDouble());
  d->SpecularSliderSpinBox->setValue(preset->data(Qt::UserRole + 4).toDouble());
  d->SpecularPowerSliderSpinBox->setValue(preset->data(Qt::UserRole + 5).toDouble());
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::resizeEvent(QResizeEvent* resize)
{
  Q_D(ctkMaterialPropertyWidget);
  this->QWidget::resizeEvent(resize);
  if (!d->PresetsListWidget)
    {
    return;
    }
  d->PresetsListWidget->setMaximumWidth(
    d->PresetsListWidget->frameWidth() // left frame width
    + d->PresetsListWidget->count() * d->MaterialPropertyPreviewLabel->sizeHint().width()
    + d->PresetsListWidget->frameWidth() ); // right frame width
  d->PresetsListWidget->setMaximumHeight(
    d->PresetsListWidget->frameWidth() // top frame height
    + d->MaterialPropertyPreviewLabel->sizeHint().height()
    + (d->PresetsListWidget->horizontalScrollBar()->isVisibleTo(d->PresetsListWidget) ? 
      d->PresetsListWidget->horizontalScrollBar()->sizeHint().height() : 0)
    + d->PresetsListWidget->frameWidth() ); // bottom frame height
}

// --------------------------------------------------------------------------
bool ctkMaterialPropertyWidget::isColorVisible()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->ColorPickerButton->isVisibleTo(
    const_cast<ctkMaterialPropertyWidget*>(this));
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setColorVisible(bool show)
{
  Q_D(ctkMaterialPropertyWidget);
  d->ColorLabel->setVisible(show);
  d->ColorPickerButton->setVisible(show);
}

// --------------------------------------------------------------------------
bool ctkMaterialPropertyWidget::isOpacityVisible()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->OpacitySliderSpinBox->isVisibleTo(
    const_cast<ctkMaterialPropertyWidget*>(this));
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setOpacityVisible(bool show)
{
  Q_D(ctkMaterialPropertyWidget);
  d->OpacityLabel->setVisible(show);
  d->OpacitySliderSpinBox->setVisible(show);
}

// --------------------------------------------------------------------------
bool ctkMaterialPropertyWidget::isBackfaceCullingVisible()const
{
  Q_D(const ctkMaterialPropertyWidget);
  return d->BackfaceCullingCheckBox->isVisibleTo(
    const_cast<ctkMaterialPropertyWidget*>(this));
}

// --------------------------------------------------------------------------
void ctkMaterialPropertyWidget::setBackfaceCullingVisible(bool show)
{
  Q_D(ctkMaterialPropertyWidget);
  d->BackfaceCullingLabel->setVisible(show);
  d->BackfaceCullingCheckBox->setVisible(show);
}
