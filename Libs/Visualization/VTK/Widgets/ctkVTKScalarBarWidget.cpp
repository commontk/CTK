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

// CTK includes
#include "ctkVTKScalarBarWidget.h"
#include "ui_ctkVTKScalarBarWidget.h"

// VTK includes
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>

//-----------------------------------------------------------------------------
class ctkVTKScalarBarWidgetPrivate
  : public ctkPrivate<ctkVTKScalarBarWidget>
  , public Ui_ctkVTKScalarBarWidget
{
public:
  ctkVTKScalarBarWidgetPrivate();
  void init();
  vtkScalarBarWidget* ScalarBarWidget;
};

//-----------------------------------------------------------------------------
ctkVTKScalarBarWidgetPrivate::ctkVTKScalarBarWidgetPrivate()
{
  this->ScalarBarWidget = 0;
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidgetPrivate::init()
{
  CTK_P(ctkVTKScalarBarWidget);
  this->setupUi(p);
  p->setEnabled(this->ScalarBarWidget != 0);
  QObject::connect(this->DisplayScalarBarCheckBox, SIGNAL(toggled(bool)),
                   p, SLOT(setDisplay(bool)));
  QObject::connect(this->MaxNumberOfColorsSpinBox, SIGNAL(valueChanged(int)),
                   p, SLOT(setMaxNumberOfColors(int)));
  QObject::connect(this->NumberOfLabelsSpinBox, SIGNAL(valueChanged(int)),
                   p, SLOT(setNumberOfLabels(int)));
  QObject::connect(this->TitleTextPropertyWidget, SIGNAL(textChanged(const QString&)),
                   p, SLOT(setTitle(const QString&)));
  QObject::connect(this->LabelsTextPropertyWidget, SIGNAL(textChanged(const QString&)),
                   p, SLOT(setLabelsFormat(const QString&)));
}

//-----------------------------------------------------------------------------
ctkVTKScalarBarWidget::ctkVTKScalarBarWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkVTKScalarBarWidget);
  CTK_D(ctkVTKScalarBarWidget);
  d->init();
}

//-----------------------------------------------------------------------------
ctkVTKScalarBarWidget::ctkVTKScalarBarWidget(vtkScalarBarWidget* scalarBarWidget, QWidget* parentWidget)
  :QWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkVTKScalarBarWidget);
  CTK_D(ctkVTKScalarBarWidget);
  d->init();
  this->setScalarBarWidget(scalarBarWidget);
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setScalarBarWidget(vtkScalarBarWidget* scalarBarWidget)
{
  CTK_D(ctkVTKScalarBarWidget);
  if (scalarBarWidget == d->ScalarBarWidget)
    {
    return;
    }
  vtkScalarBarActor* oldActor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  vtkScalarBarActor* newActor =
    scalarBarWidget ? scalarBarWidget->GetScalarBarActor() : 0;
  qvtkReconnect(oldActor, newActor, vtkCommand::ModifiedEvent,
                this, SLOT(updateFromScalarBarWidget()));
  d->ScalarBarWidget = scalarBarWidget;
  this->updateFromScalarBarWidget();
}

//-----------------------------------------------------------------------------
vtkScalarBarWidget* ctkVTKScalarBarWidget::scalarBarWidget()const
{
  CTK_D(const ctkVTKScalarBarWidget);
  return d->ScalarBarWidget;
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::updateFromScalarBarWidget()
{
  CTK_D(const ctkVTKScalarBarWidget);

  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  this->setEnabled(actor != 0);
  if (actor == 0)
    {
    return;
    }
  d->DisplayScalarBarCheckBox->setChecked(d->ScalarBarWidget->GetEnabled() != 0);
  d->MaxNumberOfColorsSpinBox->setValue(actor->GetMaximumNumberOfColors());
  d->NumberOfLabelsSpinBox->setValue(actor->GetNumberOfLabels());

  d->TitleTextPropertyWidget->setTextProperty(
    actor ? actor->GetTitleTextProperty() : 0);
  d->LabelsTextPropertyWidget->setTextProperty(
    actor ? actor->GetLabelTextProperty() : 0);
  d->TitleTextPropertyWidget->setText(actor->GetTitle());
  d->LabelsTextPropertyWidget->setText(actor->GetLabelFormat());
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setDisplay(bool visible)
{
  CTK_D(ctkVTKScalarBarWidget);
  if (d->ScalarBarWidget == 0)
    {
    return;
    }
  d->ScalarBarWidget->SetEnabled(visible);
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setMaxNumberOfColors(int colorCount)
{
  CTK_D(ctkVTKScalarBarWidget);
  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  if (actor == 0)
    {
    return;
    }
  actor->SetMaximumNumberOfColors(colorCount);
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setNumberOfLabels(int labelCount)
{
  CTK_D(ctkVTKScalarBarWidget);
  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  if (actor == 0)
    {
    return;
    }
  actor->SetNumberOfLabels(labelCount);
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setTitle(const QString& title)
{
  CTK_D(ctkVTKScalarBarWidget);
  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  if (actor == 0)
    {
    return;
    }
  actor->SetTitle(title.toStdString().c_str());
}

//-----------------------------------------------------------------------------
void ctkVTKScalarBarWidget::setLabelsFormat(const QString& format)
{
  CTK_D(ctkVTKScalarBarWidget);
  vtkScalarBarActor* actor =
    d->ScalarBarWidget ? d->ScalarBarWidget->GetScalarBarActor() : 0;
  if (actor == 0)
    {
    return;
    }
  actor->SetLabelFormat(format.toStdString().c_str());
}
