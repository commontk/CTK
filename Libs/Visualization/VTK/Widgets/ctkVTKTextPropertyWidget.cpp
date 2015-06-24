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
#include "ctkVTKTextPropertyWidget.h"
#include "ui_ctkVTKTextPropertyWidget.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>

//-----------------------------------------------------------------------------
class ctkVTKTextPropertyWidgetPrivate: public Ui_ctkVTKTextPropertyWidget
{
  Q_DECLARE_PUBLIC(ctkVTKTextPropertyWidget);
protected:
  ctkVTKTextPropertyWidget* const q_ptr;
public:
  void init();
  ctkVTKTextPropertyWidgetPrivate(ctkVTKTextPropertyWidget& object);
  vtkSmartPointer<vtkTextProperty> TextProperty;
};

//-----------------------------------------------------------------------------
ctkVTKTextPropertyWidgetPrivate::ctkVTKTextPropertyWidgetPrivate(ctkVTKTextPropertyWidget& object)
  :q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
ctkVTKTextPropertyWidget::~ctkVTKTextPropertyWidget()
{
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidgetPrivate::init()
{
  Q_Q(ctkVTKTextPropertyWidget);
  this->setupUi(q);
  q->updateFromTextProperty();
  QObject::connect(this->TextLineEdit, SIGNAL(textChanged(QString)),
                   q, SIGNAL(textChanged(QString)));
  QObject::connect(this->ColorPickerButton, SIGNAL(colorChanged(QColor)),
                   q, SLOT(setColor(QColor)));
  QObject::connect(this->OpacitySlider, SIGNAL(valueChanged(double)),
                   q, SLOT(setOpacity(double)));
  QObject::connect(this->FontComboBox, SIGNAL(currentIndexChanged(QString)),
                   q, SLOT(setFont(QString)));
  QObject::connect(this->BoldCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(setBold(bool)));
  QObject::connect(this->ItalicCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(setItalic(bool)));
  QObject::connect(this->ShadowCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(setShadow(bool)));
  QObject::connect(this->SizeSlider, SIGNAL(valueChanged(double)),
                   q, SLOT(setSize(double)));
}

//-----------------------------------------------------------------------------
ctkVTKTextPropertyWidget::ctkVTKTextPropertyWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
  , d_ptr(new ctkVTKTextPropertyWidgetPrivate(*this))
{
  Q_D(ctkVTKTextPropertyWidget);
  d->init();
}

//-----------------------------------------------------------------------------
ctkVTKTextPropertyWidget::ctkVTKTextPropertyWidget(vtkTextProperty* textProperty, QWidget* parentWidget)
  :QWidget(parentWidget)
  , d_ptr(new ctkVTKTextPropertyWidgetPrivate(*this))
{
  Q_D(ctkVTKTextPropertyWidget);
  d->init();
  this->setTextProperty(textProperty);
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setTextProperty(vtkTextProperty* textProperty)
{
  Q_D(ctkVTKTextPropertyWidget);
  qvtkReconnect(d->TextProperty, textProperty, vtkCommand::ModifiedEvent,
                this, SLOT(updateFromTextProperty()));
  d->TextProperty = textProperty;
  this->updateFromTextProperty();
}

//-----------------------------------------------------------------------------
vtkTextProperty* ctkVTKTextPropertyWidget::textProperty()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->TextProperty.GetPointer();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::updateFromTextProperty()
{
  Q_D(ctkVTKTextPropertyWidget);
  this->setEnabled(d->TextProperty.GetPointer() != 0);
  if (d->TextProperty.GetPointer() == 0)
    {
    d->ColorPickerButton->setColor(QColor());
    d->OpacitySlider->setValue(1.);
    d->FontComboBox->setCurrentIndex(-1);
    d->BoldCheckBox->setChecked(false);
    d->ItalicCheckBox->setChecked(false);
    d->ShadowCheckBox->setChecked(false);
    // Default vtkTextProperty font size is 12
    d->SizeSlider->setValue(12);
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
  d->SizeSlider->setValue(d->TextProperty->GetFontSize());
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setTextVisible(bool visible)
{
  Q_D(ctkVTKTextPropertyWidget);
  d->TextLabel->setVisible(visible);
  d->TextLineEdit->setVisible(visible);
}

//-----------------------------------------------------------------------------
bool ctkVTKTextPropertyWidget::isTextVisible()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  Q_ASSERT(d->TextLabel->isVisibleTo(const_cast<ctkVTKTextPropertyWidget*>(this))
           == d->TextLineEdit->isVisibleTo(const_cast<ctkVTKTextPropertyWidget*>(this)));
  return d->TextLineEdit->isVisibleTo(const_cast<ctkVTKTextPropertyWidget*>(this));
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setText(const QString& textString)
{
  Q_D(ctkVTKTextPropertyWidget);
  d->TextLineEdit->setText(textString);
}

//-----------------------------------------------------------------------------
QString ctkVTKTextPropertyWidget::text()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->TextLineEdit->text();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setTextLabel(const QString& label)
{
  Q_D(ctkVTKTextPropertyWidget);
  d->TextLabel->setText(label);
}

//-----------------------------------------------------------------------------
QString ctkVTKTextPropertyWidget::textLabel()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->TextLabel->text();
}

//-----------------------------------------------------------------------------
QColor ctkVTKTextPropertyWidget::color()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->ColorPickerButton->color();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setColor(const QColor& color)
{
  Q_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty.GetPointer() == 0)
    {
    return;
    }
  d->TextProperty->SetColor(color.redF(), color.greenF(), color.blueF());

  emit colorChanged(color);
}

//-----------------------------------------------------------------------------
double ctkVTKTextPropertyWidget::opacity()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->OpacitySlider->value();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setOpacity(double opacity)
{
  Q_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty.GetPointer() == 0)
    {
    return;
    }
  d->TextProperty->SetOpacity(opacity);

  emit opacityChanged(opacity);
}

//-----------------------------------------------------------------------------
QString ctkVTKTextPropertyWidget::font()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->FontComboBox->currentText();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setFont(const QString& font)
{
  Q_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty.GetPointer() == 0)
    {
    return;
    }
  d->TextProperty->SetFontFamilyAsString(font.toStdString().data());
  
  emit fontFamilyChanged(font);
}

//-----------------------------------------------------------------------------
bool ctkVTKTextPropertyWidget::isBold()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->BoldCheckBox->isChecked();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setBold(bool enable)
{
  Q_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty.GetPointer() == 0)
    {
    return;
    }
  d->TextProperty->SetBold(enable);
  
  emit boldChanged(enable);
}

//-----------------------------------------------------------------------------
bool ctkVTKTextPropertyWidget::isItalic()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->ItalicCheckBox->isChecked();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setItalic(bool enable)
{
  Q_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty.GetPointer() == 0)
    {
    return;
    }
  d->TextProperty->SetItalic(enable);
  
  emit italicChanged(enable);
}

//-----------------------------------------------------------------------------
bool ctkVTKTextPropertyWidget::hasShadow()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->ShadowCheckBox->isChecked();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setShadow(bool enable)
{
  Q_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty.GetPointer() == 0)
    {
    return;
    }
  d->TextProperty->SetShadow(enable);
  
  emit shadowChanged(enable);
}

//-----------------------------------------------------------------------------
double ctkVTKTextPropertyWidget::size()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->SizeSlider->value();
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setSize(double size)
{
  Q_D(const ctkVTKTextPropertyWidget);
  if (d->TextProperty.GetPointer() == 0)
    {
    return;
    }
  d->TextProperty->SetFontSize(size);

  emit sizeChanged(size);
}

//-----------------------------------------------------------------------------
void ctkVTKTextPropertyWidget::setSizeVisible(bool visible)
{
  Q_D(ctkVTKTextPropertyWidget);
  d->SizeSlider->setVisible(visible);
}

//-----------------------------------------------------------------------------
bool ctkVTKTextPropertyWidget::isSizeVisible()const
{
  Q_D(const ctkVTKTextPropertyWidget);
  return d->SizeSlider->isVisibleTo(const_cast<ctkVTKTextPropertyWidget*>(this));
}
