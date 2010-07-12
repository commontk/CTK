/*=========================================================================

  Library:   CTK

  Copyright (c) 2010  Kitware Inc.

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
#include "ctkVTKTextPropertyWidget.h"
#include "ui_ctkVTKTextPropertyWidget.h"

// VTK includes
#include <vtkTextProperty.h>

//-----------------------------------------------------------------------------
class ctkVTKTextPropertyWidgetPrivate
  : public ctkPrivate<ctkVTKTextPropertyWidget>
  , public Ui_ctkVTKTextPropertyWidget
{
public:
  void init();
  ctkVTKTextPropertyWidgetPrivate();
  vtkTextProperty* TextProperty;
};

//-----------------------------------------------------------------------------
ctkVTKTextPropertyWidgetPrivate::ctkVTKTextPropertyWidgetPrivate()
{
  this->TextProperty = 0;
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidgetPrivate::init()
{
  CTK_P(ctkVTKTextPropertyWidget);
  this->setupUi(p);
  p->setEnabled(this->TextProperty != 0);
  QObject::connect(this->TextLineEdit, SIGNAL(textChanged(const QString&)),
                   p, SIGNAL(textChanged(const QString&)));
  QObject::connect(this->ColorPickerButton, SIGNAL(colorChanged(QColor)),
                   p, SLOT(setColor(const QColor&)));
  QObject::connect(this->OpacitySlider, SIGNAL(valueChanged(double)),
                   p, SLOT(setOpacity(double)));
  QObject::connect(this->FontComboBox, SIGNAL(currentIndexChanged(const QString&)),
                   p, SLOT(setFont(const QString&)));
  QObject::connect(this->BoldCheckBox, SIGNAL(toggled(bool)),
                   p, SLOT(setBold(bool)));
  QObject::connect(this->ItalicCheckBox, SIGNAL(toggled(bool)),
                   p, SLOT(setItalic(bool)));
  QObject::connect(this->ShadowCheckBox, SIGNAL(toggled(bool)),
                   p, SLOT(setShadow(bool)));
}

//-----------------------------------------------------------------------------
ctkVTKTextPropertyWidget::ctkVTKTextPropertyWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkVTKTextPropertyWidget);
  CTK_D(ctkVTKTextPropertyWidget);
  d->init();
}

//-----------------------------------------------------------------------------
ctkVTKTextPropertyWidget::ctkVTKTextPropertyWidget(vtkTextProperty* textProperty, QWidget* parentWidget)
  :QWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkVTKTextPropertyWidget);
  CTK_D(ctkVTKTextPropertyWidget);
  d->init();
  this->setTextProperty(textProperty);
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setTextProperty(vtkTextProperty* textProperty)
{
  CTK_D(ctkVTKTextPropertyWidget);
  qvtkReconnect(d->TextProperty, textProperty, vtkCommand::ModifiedEvent,
                this, SLOT(updateFromTextProperty()));
  d->TextProperty = textProperty;
  this->updateFromTextProperty();
}

//-----------------------------------------------------------------------------
vtkTextProperty* ctkVTKTextPropertyWidget::textProperty()const
{
  CTK_D(const ctkVTKTextPropertyWidget);
  return d->TextProperty;
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::updateFromTextProperty()
{
  CTK_D(ctkVTKTextPropertyWidget);
  this->setEnabled(d->TextProperty != 0);
  if (d->TextProperty == 0)
    {
    return;
    }

  double* color = d->TextProperty->GetColor();
  d->ColorPickerButton->setColor(
    QColor::fromRgbF(color[0],color[1],color[2]));
  d->OpacitySlider->setValue(d->TextProperty->GetOpacity());
  d->FontComboBox->setCurrentIndex(
    d->FontComboBox->findText(d->TextProperty->GetFontFamilyAsString()));
  d->BoldCheckBox->setChecked(d->TextProperty->GetBold() != 0);
  d->ItalicCheckBox->setChecked(d->TextProperty->GetItalic() != 0);
  d->ShadowCheckBox->setChecked(d->TextProperty->GetShadow() != 0);
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setTextVisible(bool visible)
{
  CTK_D(ctkVTKTextPropertyWidget);
  d->TextLabel->setVisible(visible);
  d->TextLineEdit->setVisible(visible);
}

//-----------------------------------------------------------------------------
bool ctkVTKTextPropertyWidget::isTextVisible()const
{
  CTK_D(const ctkVTKTextPropertyWidget);
  Q_ASSERT(d->TextLabel->isVisibleTo(const_cast<ctkVTKTextPropertyWidget*>(this))
           == d->TextLineEdit->isVisibleTo(const_cast<ctkVTKTextPropertyWidget*>(this)));
  return d->TextLineEdit->isVisibleTo(const_cast<ctkVTKTextPropertyWidget*>(this));
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setText(const QString& textString)
{
  CTK_D(ctkVTKTextPropertyWidget);
  d->TextLineEdit->setText(textString);
}

//-----------------------------------------------------------------------------
QString ctkVTKTextPropertyWidget::text()const
{
  CTK_D(const ctkVTKTextPropertyWidget);
  return d->TextLineEdit->text();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setTextLabel(const QString& label)
{
  CTK_D(ctkVTKTextPropertyWidget);
  d->TextLabel->setText(label);
}

//-----------------------------------------------------------------------------
QString ctkVTKTextPropertyWidget::textLabel()const
{
  CTK_D(const ctkVTKTextPropertyWidget);
  return d->TextLabel->text();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setColor(const QColor& color)
{
  CTK_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty)
    {
    return;
    }
  d->TextProperty->SetColor(color.redF(), color.greenF(), color.blueF());
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setOpacity(double opacity)
{
  CTK_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty)
    {
    return;
    }
  d->TextProperty->SetOpacity(opacity);
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setFont(const QString& font)
{
  CTK_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty)
    {
    return;
    }
  d->TextProperty->SetFontFamilyAsString(font.toStdString().data());
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setBold(bool enable)
{
  CTK_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty)
    {
    return;
    }
  d->TextProperty->SetBold(enable);
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setItalic(bool enable)
{
  CTK_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty)
    {
    return;
    }
  d->TextProperty->SetItalic(enable);
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setShadow(bool enable)
{
  CTK_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty)
    {
    return;
    }
  d->TextProperty->SetShadow(enable);
}
