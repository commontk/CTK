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

// CTK includes
#include "ctkVTKPropertyWidget.h"
#include "ui_ctkVTKPropertyWidget.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
class ctkVTKPropertyWidgetPrivate: public Ui_ctkVTKPropertyWidget
{
  Q_DECLARE_PUBLIC(ctkVTKPropertyWidget);
protected:
  ctkVTKPropertyWidget* const q_ptr;
public:
  ctkVTKPropertyWidgetPrivate(ctkVTKPropertyWidget& object);

  void setupUi(QWidget *widget);

  vtkSmartPointer<vtkProperty> Property;
};

//-----------------------------------------------------------------------------
ctkVTKPropertyWidgetPrivate::ctkVTKPropertyWidgetPrivate(ctkVTKPropertyWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidgetPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkVTKPropertyWidget);

  this->Ui_ctkVTKPropertyWidget::setupUi(widget);

  q->connect(this->RepresentationComboBox, SIGNAL(currentIndexChanged(int)),
             q, SLOT(setRepresentation(int)));
  q->connect(this->PointSizeSliderWidget, SIGNAL(valueChanged(double)),
             q, SLOT(setPointSize(double)));
  q->connect(this->LineWidthSliderWidget, SIGNAL(valueChanged(double)),
             q, SLOT(setLineWidth(double)));
  q->connect(this->FrontfaceCullingCheckBox, SIGNAL(toggled(bool)),
             q, SLOT(setFrontfaceCulling(bool)));
  q->connect(this->BackfaceCullingCheckBox, SIGNAL(toggled(bool)),
             q, SLOT(setBackfaceCulling(bool)));

  q->connect(this->ColorPickerButton, SIGNAL(colorChanged(QColor)),
             q, SLOT(setColor(QColor)));
  q->connect(this->OpacitySliderWidget, SIGNAL(valueChanged(double)),
             q, SLOT(setOpacity(double)));
  q->connect(this->EdgeVisibilityCheckBox, SIGNAL(toggled(bool)),
             q, SLOT(setEdgeVisibility(bool)));
  q->connect(this->EdgeColorPickerButton, SIGNAL(colorChanged(QColor)),
             q, SLOT(setEdgeColor(QColor)));

  q->connect(this->LightingCheckBox, SIGNAL(toggled(bool)),
             q, SLOT(setLighting(bool)));
  q->connect(this->InterpolationComboBox, SIGNAL(currentIndexChanged(int)),
             q, SLOT(setInterpolation(int)));
  q->connect(this->ShadingCheckBox, SIGNAL(toggled(bool)),
             q, SLOT(setShading(bool)));
}

//-----------------------------------------------------------------------------
ctkVTKPropertyWidget::~ctkVTKPropertyWidget()
{
}

//-----------------------------------------------------------------------------
ctkVTKPropertyWidget::ctkVTKPropertyWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKPropertyWidgetPrivate(*this))
{
  Q_D(ctkVTKPropertyWidget);
  d->setupUi(this);

  vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
  this->setProperty(property);
}

//-----------------------------------------------------------------------------
ctkVTKPropertyWidget::ctkVTKPropertyWidget(vtkProperty* property, QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKPropertyWidgetPrivate(*this))
{
  Q_D(ctkVTKPropertyWidget);
  d->setupUi(this);

  this->setProperty(property);
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidget::setProperty(vtkProperty* property)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() == property)
    {
    return;
    }
  qvtkReconnect(d->Property, property, vtkCommand::ModifiedEvent,
                this, SLOT(updateWidgetFromProperty()));
  d->Property = property;
  d->MaterialPropertyWidget->setProperty(property);
  this->updateWidgetFromProperty();
}

//-----------------------------------------------------------------------------
vtkProperty* ctkVTKPropertyWidget::property()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->Property.GetPointer();
}

//-----------------------------------------------------------------------------
ctkVTKPropertyWidget::GroupsState ctkVTKPropertyWidget::groupsState()const
{
  Q_D(const ctkVTKPropertyWidget);
  ctkVTKPropertyWidget::GroupsState state = 0;
  ctkVTKPropertyWidget* constThis = const_cast<ctkVTKPropertyWidget*>(this);
  if (d->RepresentationCollapsibleGroupBox->isVisibleTo(constThis) )
    {
    state |= ctkVTKPropertyWidget::RepresentationVisible;
    }
  if (d->RepresentationCollapsibleGroupBox->collapsed())
    {
    state |= ctkVTKPropertyWidget::RepresentationCollapsed;
    }
  if (d->ColorCollapsibleGroupBox->isVisibleTo(constThis) )
    {
    state |= ctkVTKPropertyWidget::ColorVisible;
    }
  if (d->ColorCollapsibleGroupBox->collapsed())
    {
    state |= ctkVTKPropertyWidget::ColorCollapsed;
    }
  if (d->LightingCollapsibleGroupBox->isVisibleTo(constThis) )
    {
    state |= ctkVTKPropertyWidget::LightingVisible;
    }
  if (d->LightingCollapsibleGroupBox->collapsed())
    {
    state |= ctkVTKPropertyWidget::LightingCollapsed;
    }
  if (d->MaterialCollapsibleGroupBox->isVisibleTo(constThis) )
    {
    state |= ctkVTKPropertyWidget::MaterialVisible;
    }
  if (d->MaterialCollapsibleGroupBox->collapsed())
    {
    state |= ctkVTKPropertyWidget::MaterialCollapsed;
    }
  return state;
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidget::setGroupsState(ctkVTKPropertyWidget::GroupsState state)
{
  Q_D(ctkVTKPropertyWidget);
  d->RepresentationCollapsibleGroupBox->setVisible(
    state & ctkVTKPropertyWidget::RepresentationVisible);
  d->RepresentationCollapsibleGroupBox->setCollapsed(
    state & ctkVTKPropertyWidget::RepresentationCollapsed);
  d->ColorCollapsibleGroupBox->setVisible(
    state & ctkVTKPropertyWidget::ColorVisible);
  d->ColorCollapsibleGroupBox->setCollapsed(
    state & ctkVTKPropertyWidget::ColorCollapsed);
  d->LightingCollapsibleGroupBox->setVisible(
    state & ctkVTKPropertyWidget::LightingVisible);
  d->LightingCollapsibleGroupBox->setCollapsed(
    state & ctkVTKPropertyWidget::LightingCollapsed);
  d->MaterialCollapsibleGroupBox->setVisible(
    state & ctkVTKPropertyWidget::MaterialVisible);
  d->MaterialCollapsibleGroupBox->setCollapsed(
    state & ctkVTKPropertyWidget::MaterialCollapsed);
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::ColorDialogOptions ctkVTKPropertyWidget
::colorDialogOptions()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->ColorPickerButton->dialogOptions();
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidget
::setColorDialogOptions(ctkColorPickerButton::ColorDialogOptions options)
{
  Q_D(ctkVTKPropertyWidget);
  d->ColorPickerButton->setDialogOptions(options);
  d->EdgeColorPickerButton->setDialogOptions(options);
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidget::updateWidgetFromProperty()
{
  Q_D(ctkVTKPropertyWidget);
  this->setEnabled(d->Property.GetPointer() != 0);
  if (d->Property.GetPointer() == 0)
    {
    return;
    }
  // Warning: Valid as long as the representation matches the combobox indexes.
  d->RepresentationComboBox->setCurrentIndex( d->Property->GetRepresentation() );
  d->PointSizeSliderWidget->setValue( d->Property->GetPointSize() );
  d->LineWidthSliderWidget->setValue( d->Property->GetLineWidth() );
  d->FrontfaceCullingCheckBox->setChecked( d->Property->GetFrontfaceCulling() );
  d->BackfaceCullingCheckBox->setChecked( d->Property->GetBackfaceCulling() );
  double* c = d->Property->GetColor();
  d->ColorPickerButton->setColor(
    QColor::fromRgbF(qMin(c[0],1.), qMin(c[1], 1.), qMin(c[2],1.)));
  d->OpacitySliderWidget->setValue( d->Property->GetOpacity() );
  d->EdgeVisibilityCheckBox->setChecked( d->Property->GetEdgeVisibility() );
  double* ec = d->Property->GetEdgeColor();
  d->EdgeColorPickerButton->setColor(
    QColor::fromRgbF(qMin(ec[0],1.), qMin(ec[1], 1.), qMin(ec[2],1.)));
  d->LightingCheckBox->setChecked( d->Property->GetLighting() );
  d->InterpolationComboBox->setCurrentIndex( d->Property->GetInterpolation() );
  d->ShadingCheckBox->setChecked( d->Property->GetShading() );
}

// --------------------------------------------------------------------------
int ctkVTKPropertyWidget::representation()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->RepresentationComboBox->currentIndex();
}

// --------------------------------------------------------------------------
double ctkVTKPropertyWidget::pointSize()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->PointSizeSliderWidget->value();
}

// --------------------------------------------------------------------------
double ctkVTKPropertyWidget::lineWidth()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->LineWidthSliderWidget->value();
}

// --------------------------------------------------------------------------
bool ctkVTKPropertyWidget::frontfaceCulling()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->FrontfaceCullingCheckBox->isChecked();
}

// --------------------------------------------------------------------------
bool ctkVTKPropertyWidget::backfaceCulling()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->BackfaceCullingCheckBox->isChecked();
}

// --------------------------------------------------------------------------
QColor ctkVTKPropertyWidget::color()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->ColorPickerButton->color();
}

// --------------------------------------------------------------------------
double ctkVTKPropertyWidget::opacity()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->OpacitySliderWidget->value();
}

// --------------------------------------------------------------------------
bool ctkVTKPropertyWidget::edgeVisibility()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->EdgeVisibilityCheckBox->isChecked();
}

// --------------------------------------------------------------------------
QColor ctkVTKPropertyWidget::edgeColor()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->EdgeColorPickerButton->color();
}

// --------------------------------------------------------------------------
bool ctkVTKPropertyWidget::lighting()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->LightingCheckBox->isChecked();
}

// --------------------------------------------------------------------------
int ctkVTKPropertyWidget::interpolation()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->InterpolationComboBox->currentIndex();
}

// --------------------------------------------------------------------------
bool ctkVTKPropertyWidget::shading()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->ShadingCheckBox->isChecked();
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setRepresentation(int newRepresentation)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetRepresentation(newRepresentation);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setPointSize(double newPointSize)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetPointSize(newPointSize);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setLineWidth(double newLineWidth)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetLineWidth(newLineWidth);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setFrontfaceCulling(bool newFrontfaceCulling)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetFrontfaceCulling(newFrontfaceCulling);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setBackfaceCulling(bool newBackfaceCulling)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetBackfaceCulling(newBackfaceCulling);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setColor(const QColor& newColor)
{
  Q_D(ctkVTKPropertyWidget);
  d->MaterialPropertyWidget->setColor(newColor);
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setOpacity(double newOpacity)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetOpacity(newOpacity);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setEdgeVisibility(bool newEdgeVisibility)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetEdgeVisibility(newEdgeVisibility);
    }
}


// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setEdgeColor(const QColor& newColor)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetEdgeColor(
      newColor.redF(), newColor.greenF(), newColor.blueF());
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setLighting(bool newLighting)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetLighting(newLighting);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setInterpolation(int newInterpolation)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetInterpolation(newInterpolation);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setShading(bool newShading)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetShading(newShading);
    }
}
