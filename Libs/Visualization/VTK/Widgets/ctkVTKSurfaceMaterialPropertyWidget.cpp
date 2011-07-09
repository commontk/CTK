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
#include "ctkVTKSurfaceMaterialPropertyWidget.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
class ctkVTKSurfaceMaterialPropertyWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKSurfaceMaterialPropertyWidget);
protected:
  ctkVTKSurfaceMaterialPropertyWidget* const q_ptr;
public:
  ctkVTKSurfaceMaterialPropertyWidgetPrivate(ctkVTKSurfaceMaterialPropertyWidget& object);
  vtkSmartPointer<vtkProperty> Property;
  double                       SettingColor;
};

//-----------------------------------------------------------------------------
ctkVTKSurfaceMaterialPropertyWidgetPrivate::ctkVTKSurfaceMaterialPropertyWidgetPrivate(ctkVTKSurfaceMaterialPropertyWidget& object)
  :q_ptr(&object)
{
  this->SettingColor = false;
}

//-----------------------------------------------------------------------------
ctkVTKSurfaceMaterialPropertyWidget::~ctkVTKSurfaceMaterialPropertyWidget()
{
}

//-----------------------------------------------------------------------------
ctkVTKSurfaceMaterialPropertyWidget::ctkVTKSurfaceMaterialPropertyWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKSurfaceMaterialPropertyWidgetPrivate(*this))
{
  this->updateFromProperty();
}

//-----------------------------------------------------------------------------
ctkVTKSurfaceMaterialPropertyWidget::ctkVTKSurfaceMaterialPropertyWidget(vtkProperty* property, QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKSurfaceMaterialPropertyWidgetPrivate(*this))
{
  this->setProperty(property);
}

//-----------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::setProperty(vtkProperty* property)
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  if (d->Property.GetPointer() == property)
    {
    return;
    }
  qvtkReconnect(d->Property, property, vtkCommand::ModifiedEvent,
                this, SLOT(updateFromProperty()));
  d->Property = property;
  this->updateFromProperty();
}

//-----------------------------------------------------------------------------
vtkProperty* ctkVTKSurfaceMaterialPropertyWidget::property()const
{
  Q_D(const ctkVTKSurfaceMaterialPropertyWidget);
  return d->Property.GetPointer();
}

//-----------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::updateFromProperty()
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  this->setEnabled(d->Property.GetPointer() != 0);
  if (d->Property.GetPointer() == 0 || d->SettingColor)
    {
    return;
    }
  double* c = d->Property->GetColor();
  this->setColor(QColor::fromRgbF(qMin(c[0],1.), qMin(c[1], 1.), qMin(c[2],1.)));
  this->setOpacity(d->Property->GetOpacity());
  this->setAmbient(d->Property->GetAmbient());
  this->setDiffuse(d->Property->GetDiffuse());
  this->setSpecular(d->Property->GetSpecular());
  this->setSpecularPower(d->Property->GetSpecularPower());
}

// --------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::onColorChanged(const QColor& newColor)
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  this->Superclass::onColorChanged(newColor);
  if (d->Property.GetPointer() != 0)
    {
    // the value might have changed since we fired the signal, use the current
    // up-to-date value then.
    const QColor c = this->color();
    // Need to work around a VTK bug of SetColor() that fires event
    // in an unstable state:
    // d->Property->SetColor(c.redF(), c.greenF(), c.blueF());
    d->SettingColor = true;
    d->Property->SetAmbientColor(c.redF(), c.greenF(), c.blueF());
    d->Property->SetDiffuseColor(c.redF(), c.greenF(), c.blueF());
    d->Property->SetSpecularColor(c.redF(), c.greenF(), c.blueF());
    d->SettingColor = false;
    // update just in case something connected to the modified event of the
    // vtkProperty modified any attribute
    this->updateFromProperty();
    }
}

// --------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::onOpacityChanged(double newOpacity)
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  this->Superclass::onOpacityChanged(newOpacity);
  if (d->Property.GetPointer() != 0)
    {
    // the value might have changed since we fired the signal, use the current
    // up-to-date value then.
    d->Property->SetOpacity(this->opacity());
    }
}

// --------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::onAmbientChanged(double newAmbient)
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  this->Superclass::onAmbientChanged(newAmbient);
  if (d->Property.GetPointer() != 0)
    {
    // the value might have changed since we fired the signal, use the current
    // up-to-date value then.
    d->Property->SetAmbient(this->ambient());
    }
}

// --------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::onDiffuseChanged(double newDiffuse)
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  this->Superclass::onDiffuseChanged(newDiffuse);
  if (d->Property.GetPointer() != 0)
    {
    // the value might have changed since we fired the signal, use the current
    // up-to-date value then.
    d->Property->SetDiffuse(this->diffuse());
    }
}

// --------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::onSpecularChanged(double newSpecular)
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  this->Superclass::onSpecularChanged(newSpecular);
  if (d->Property.GetPointer() != 0)
    {
    // the value might have changed since we fired the signal, use the current
    // up-to-date value then.
    d->Property->SetSpecular(this->specular());
    }
}

// --------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::onSpecularPowerChanged(double newSpecularPower)
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  this->Superclass::onSpecularPowerChanged(newSpecularPower);
  if (d->Property.GetPointer() != 0)
    {
    // the value might have changed since we fired the signal, use the current
    // up-to-date value then.
    d->Property->SetSpecularPower(this->specularPower());
    }
}

// --------------------------------------------------------------------------
void ctkVTKSurfaceMaterialPropertyWidget::onBackfaceCullingChanged(bool newBackfaceCulling)
{
  Q_D(ctkVTKSurfaceMaterialPropertyWidget);
  this->Superclass::onBackfaceCullingChanged(newBackfaceCulling);
  if (d->Property.GetPointer() != 0)
    {
    // the value might have changed since we fired the signal, use the current
    // up-to-date value then.
    d->Property->SetBackfaceCulling(this->backfaceCulling());
    }
}
