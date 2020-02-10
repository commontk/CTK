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

// CTK includes
#include "ctkVTKScalarBarWidget.h"
#include "ui_ctkVTKScalarBarWidget.h"

// VTK includes
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>

//-----------------------------------------------------------------------------
class ctkVTKScalarBarWidgetPrivate: public Ui_ctkVTKScalarBarWidget
{
  Q_DECLARE_PUBLIC(ctkVTKScalarBarWidget);
protected:
  ctkVTKScalarBarWidget* const q_ptr;
public:
  ctkVTKScalarBarWidgetPrivate(ctkVTKScalarBarWidget& object);
  void init();
  void updateFromScalarBarWidget();

  vtkScalarBarWidget* ScalarBarWidget;
};

//-----------------------------------------------------------------------------
ctkVTKScalarBarWidgetPrivate::ctkVTKScalarBarWidgetPrivate(ctkVTKScalarBarWidget& object)
  :q_ptr(&object)
{
  this->ScalarBarWidget = 0;
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidgetPrivate::init()
{
  Q_Q(ctkVTKScalarBarWidget);
  this->setupUi(q);
  q->setEnabled(this->ScalarBarWidget != 0);
  QObject::connect(this->DisplayScalarBarCheckBox, SIGNAL(toggled(bool)),
                   q, SLOT(setDisplay(bool)));
  QObject::connect(this->MaxNumberOfColorsSpinBox, SIGNAL(valueChanged(int)),
                   q, SLOT(setMaxNumberOfColors(int)));
  QObject::connect(this->NumberOfLabelsSpinBox, SIGNAL(valueChanged(int)),
                   q, SLOT(setNumberOfLabels(int)));

  QObject::connect(this->TitleTextPropertyWidget, SIGNAL(textChanged(QString)),
                   q, SLOT(setTitle(QString)));
  QObject::connect(this->TitleTextPropertyWidget, SIGNAL(colorChanged(QColor)),
                   q, SIGNAL(modified()));
  QObject::connect(this->TitleTextPropertyWidget, SIGNAL(opacityChanged(double)),
                   q, SIGNAL(modified()));
  QObject::connect(this->TitleTextPropertyWidget, SIGNAL(fontFamilyChanged(QString)),
                   q, SIGNAL(modified()));
  QObject::connect(this->TitleTextPropertyWidget, SIGNAL(boldChanged(bool)),
                   q, SIGNAL(modified()));
  QObject::connect(this->TitleTextPropertyWidget, SIGNAL(italicChanged(bool)),
                   q, SIGNAL(modified()));
  QObject::connect(this->TitleTextPropertyWidget, SIGNAL(shadowChanged(bool)),
                   q, SIGNAL(modified()));

  QObject::connect(this->LabelsTextPropertyWidget, SIGNAL(textChanged(QString)),
                   q, SLOT(setLabelsFormat(QString)));
  QObject::connect(this->LabelsTextPropertyWidget, SIGNAL(colorChanged(QColor)),
                   q, SIGNAL(modified()));
  QObject::connect(this->LabelsTextPropertyWidget, SIGNAL(opacityChanged(double)),
                   q, SIGNAL(modified()));
  QObject::connect(this->LabelsTextPropertyWidget, SIGNAL(fontFamilyChanged(QString)),
                   q, SIGNAL(modified()));
  QObject::connect(this->LabelsTextPropertyWidget, SIGNAL(boldChanged(bool)),
                   q, SIGNAL(modified()));
  QObject::connect(this->LabelsTextPropertyWidget, SIGNAL(italicChanged(bool)),
                   q, SIGNAL(modified()));
  QObject::connect(this->LabelsTextPropertyWidget, SIGNAL(shadowChanged(bool)),
                   q, SIGNAL(modified()));
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidgetPrivate::updateFromScalarBarWidget()
{
  Q_Q(ctkVTKScalarBarWidget);

  vtkScalarBarActor* actor =
    this->ScalarBarWidget ? this->ScalarBarWidget->GetScalarBarActor() : 0;
  q->setEnabled(actor != 0);
  if (actor == 0)
    {
    return;
    }
  this->DisplayScalarBarCheckBox->setChecked(this->ScalarBarWidget->GetEnabled() != 0);
  this->MaxNumberOfColorsSpinBox->setValue(actor->GetMaximumNumberOfColors());
  this->NumberOfLabelsSpinBox->setValue(actor->GetNumberOfLabels());

  this->TitleTextPropertyWidget->setTextProperty(
    actor->GetTitleTextProperty());
  this->LabelsTextPropertyWidget->setTextProperty(
    actor->GetLabelTextProperty());
  this->TitleTextPropertyWidget->setText(actor->GetTitle());
  this->LabelsTextPropertyWidget->setText(actor->GetLabelFormat());
}

//-----------------------------------------------------------------------------
ctkVTKScalarBarWidget::ctkVTKScalarBarWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
  , d_ptr(new ctkVTKScalarBarWidgetPrivate(*this))
{
  Q_D(ctkVTKScalarBarWidget);
  d->init();
}

//-----------------------------------------------------------------------------
ctkVTKScalarBarWidget::ctkVTKScalarBarWidget(vtkScalarBarWidget* scalarBarWidget, QWidget* parentWidget)
  :QWidget(parentWidget)
  , d_ptr(new ctkVTKScalarBarWidgetPrivate(*this))
{
  Q_D(ctkVTKScalarBarWidget);
  d->init();
  this->setScalarBarWidget(scalarBarWidget);
}

//-----------------------------------------------------------------------------
ctkVTKScalarBarWidget::~ctkVTKScalarBarWidget()
{
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setScalarBarWidget(vtkScalarBarWidget* scalarBarWidget)
{
  Q_D(ctkVTKScalarBarWidget);
  if (scalarBarWidget == d->ScalarBarWidget)
    {
    return;
    }
  vtkScalarBarActor* oldActor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  vtkScalarBarActor* newActor =
    scalarBarWidget ? scalarBarWidget->GetScalarBarActor() : 0;
  qvtkReconnect(d->ScalarBarWidget, scalarBarWidget, vtkCommand::EnableEvent, 
                this, SLOT(onScalarBarModified()));
  qvtkReconnect(d->ScalarBarWidget, scalarBarWidget, vtkCommand::DisableEvent, 
                this, SLOT(onScalarBarModified()));
  qvtkReconnect(oldActor, newActor, vtkCommand::ModifiedEvent,
                this, SLOT(onScalarBarModified()));
  d->ScalarBarWidget = scalarBarWidget;
  this->onScalarBarModified();
}

//-----------------------------------------------------------------------------
vtkScalarBarWidget* ctkVTKScalarBarWidget::scalarBarWidget()const
{
  Q_D(const ctkVTKScalarBarWidget);
  return d->ScalarBarWidget;
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::onScalarBarModified()
{
  Q_D(ctkVTKScalarBarWidget);
  d->updateFromScalarBarWidget();
  emit modified();
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setDisplay(bool visible)
{
  Q_D(ctkVTKScalarBarWidget);
  if (d->ScalarBarWidget == 0)
    {
    return;
    }
  d->ScalarBarWidget->SetEnabled(visible);
  // calling SetEnabled might fail, make sure the checkbox is up-to-date
  d->DisplayScalarBarCheckBox->setChecked(d->ScalarBarWidget->GetEnabled());
}

//-----------------------------------------------------------------------------
bool ctkVTKScalarBarWidget::display()const
{
  Q_D(const ctkVTKScalarBarWidget);
  return d->DisplayScalarBarCheckBox->isChecked();
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setMaxNumberOfColors(int colorCount)
{
  Q_D(ctkVTKScalarBarWidget);
  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  if (actor == 0)
    {
    return;
    }
  actor->SetMaximumNumberOfColors(colorCount);
}

//-----------------------------------------------------------------------------
int ctkVTKScalarBarWidget::maxNumberOfColors()const
{
  Q_D(const ctkVTKScalarBarWidget);
  return d->MaxNumberOfColorsSpinBox->value();
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setNumberOfLabels(int labelCount)
{
  Q_D(ctkVTKScalarBarWidget);
  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  if (actor == 0)
    {
    return;
    }
  actor->SetNumberOfLabels(labelCount);
}

//-----------------------------------------------------------------------------
int ctkVTKScalarBarWidget::numberOfLabels()const
{
  Q_D(const ctkVTKScalarBarWidget);
  return d->NumberOfLabelsSpinBox->value();
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setTitle(const QString& title)
{
  Q_D(ctkVTKScalarBarWidget);
  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  if (actor == 0)
    {
    return;
    }
  actor->SetTitle(title.toUtf8());
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarBarWidget::title()const
{
  Q_D(const ctkVTKScalarBarWidget);
  return d->TitleTextPropertyWidget->text();
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setLabelsFormat(const QString& format)
{
  Q_D(ctkVTKScalarBarWidget);
  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  if (actor == 0)
    {
    return;
    }
  actor->SetLabelFormat(format.toUtf8());
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarBarWidget::labelsFormat()const
{
  Q_D(const ctkVTKScalarBarWidget);
  return d->LabelsTextPropertyWidget->text();
}
