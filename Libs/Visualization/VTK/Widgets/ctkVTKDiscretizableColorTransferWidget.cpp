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

#include "ctkVTKDiscretizableColorTransferWidget.h"

// CTK includes
#include "ctkColorPickerButton.h"
#include "ctkDoubleSlider.h"
#include "ctkVTKScalarsToColorsComboBox.h"
#include "ctkVTKScalarsToColorsUtils.h"
#include "ui_ctkVTKDiscretizableColorTransferWidget.h"
#include "vtkScalarsToColorsContextItem.h"

// Qt includes
#include <QColorDialog>
#include <QCheckBox>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

// VTK includes
#if CTK_USE_QVTKOPENGLWIDGET
#include <QVTKOpenGLWidget.h>
#else
#include <QVTKWidget.h>
#endif
#include <vtkCallbackCommand.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkControlPointsItem.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkDoubleArray.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkIntArray.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarsToColors.h>
#include <vtkTable.h>

// ----------------------------------------------------------------------------
class ctkVTKDiscretizableColorTransferWidgetPrivate :
  public Ui_ctkVTKDiscretizableColorTransferWidget
{
  Q_DECLARE_PUBLIC(ctkVTKDiscretizableColorTransferWidget);
protected:
  ctkVTKDiscretizableColorTransferWidget* const q_ptr;

public:
  ctkVTKDiscretizableColorTransferWidgetPrivate(
    ctkVTKDiscretizableColorTransferWidget& object);

  void setupUi(QWidget* widget);

#if CTK_USE_QVTKOPENGLWIDGET
  QVTKOpenGLWidget* ScalarsToColorsView;
#else
  QVTKWidget* ScalarsToColorsView;
#endif

  vtkSmartPointer<vtkScalarsToColorsContextItem> scalarsToColorsContextItem;
  vtkSmartPointer<vtkContextView> scalarsToColorsContextView;
  vtkSmartPointer<vtkEventQtSlotConnect> eventLink;

  ///Option part
  ctkColorPickerButton* nanButton;
  QCheckBox* discretizeCheckBox;
  QSpinBox* nbOfDiscreteValuesSpinBox;

  /// Stores the range of the data.
  /// Extracted from the histogram
  double dataRange[2];
  double dataMean;

  double previousOpacityValue;

  vtkSmartPointer<vtkCallbackCommand> colorTransferFunctionModified;
  static void colorTransferFunctionModifiedCallback(vtkObject *caller,
    unsigned long eid, void *clientdata, void *calldata);
};

// ----------------------------------------------------------------------------
ctkVTKDiscretizableColorTransferWidgetPrivate
::ctkVTKDiscretizableColorTransferWidgetPrivate(
  ctkVTKDiscretizableColorTransferWidget& object)
  : q_ptr(&object)
{
  this->scalarsToColorsSelector = nullptr;

  // Option menu
  this->nanButton = nullptr;
  this->discretizeCheckBox = nullptr;
  this->nbOfDiscreteValuesSpinBox = nullptr;

  this->dataRange[0] = VTK_DOUBLE_MAX;
  this->dataRange[1] = VTK_DOUBLE_MIN;
  this->dataMean = 0.;

  this->previousOpacityValue = 0.;

  this->colorTransferFunctionModified =
    vtkSmartPointer<vtkCallbackCommand>::New();
  this->colorTransferFunctionModified->SetClientData(this);
  this->colorTransferFunctionModified->SetCallback(
    this->colorTransferFunctionModifiedCallback);
}

//-----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidgetPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkVTKDiscretizableColorTransferWidget);

  this->Ui_ctkVTKDiscretizableColorTransferWidget::setupUi(widget);

#if CTK_USE_QVTKOPENGLWIDGET
  this->ScalarsToColorsView = new QVTKOpenGLWidget;
#else
  this->ScalarsToColorsView = new QVTKWidget;
#endif
  this->gridLayout->addWidget(this->ScalarsToColorsView, 3, 2, 7, 1);

  this->scalarsToColorsContextItem =
    vtkSmartPointer<vtkScalarsToColorsContextItem>::New();
  this->scalarsToColorsContextView = vtkSmartPointer<vtkContextView> ::New();

  this->scalarsToColorsContextView->GetScene()->AddItem(
    this->scalarsToColorsContextItem.Get());
  this->scalarsToColorsContextView->SetInteractor(
    this->ScalarsToColorsView->GetInteractor());
  this->ScalarsToColorsView->SetRenderWindow(
    this->scalarsToColorsContextView->GetRenderWindow());

  q->setViewBackgroundColor(QColor(49, 54, 59));

  this->previousOpacityValue = opacitySlider->value();

  this->scalarsToColorsSelector->addScalarsToColors(nullptr, q->tr("Reset"));
  this->scalarsToColorsSelector->setCurrentIndex(-1);

  this->eventLink = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  this->eventLink->Connect(scalarsToColorsContextItem.Get(),
    vtkControlPointsItem::CurrentPointEditEvent,
    q, SLOT(onCurrentPointEdit()));

  this->scalarsToColorsContextItem->AddObserver(vtkCommand::EndEvent,
    this->colorTransferFunctionModified);

  QObject::connect(this->scalarsToColorsSelector,
    SIGNAL(currentScalarsToColorsChanged(vtkScalarsToColors*)),
    q, SLOT(onPaletteIndexChanged(vtkScalarsToColors*)));

  QObject::connect(opacitySlider, SIGNAL(valueChanged(double)),
    q, SLOT(setGlobalOpacity(double)));

  QObject::connect(resetRangeButton, SIGNAL(clicked()),
    q, SLOT(resetColorTransferFunctionRange()));

  QObject::connect(centerRangeButton, SIGNAL(clicked()),
    q, SLOT(centerColorTransferFunctionRange()));

  QObject::connect(invertColorTransferFunctionButton, SIGNAL(clicked()),
    q, SLOT(invertColorTransferFunction()));

  QObject::connect(rangeSlider, SIGNAL(valuesChanged(double, double)),
    q, SLOT(setColorTransferFunctionRange(double, double)));

  /// Option panel menu
  QWidget* nanColorWidget = new QWidget(optionButton);
  QHBoxLayout* nanColorLayout = new QHBoxLayout(nanColorWidget);
  QWidget* discretizeWidget = new QWidget(optionButton);
  QHBoxLayout* discretizeLayout = new QHBoxLayout(discretizeWidget);

  nanColorLayout->setContentsMargins(0, 0, 0, 0);
  discretizeLayout->setContentsMargins(0, 0, 0, 0);

  optionButton->setIcon(q->style()->standardIcon(
    QStyle::SP_FileDialogDetailedView, nullptr, optionButton));

  QLabel* nanLabel = new QLabel(q->tr("NaN values"));
  nanButton = new ctkColorPickerButton;
  nanButton->setToolTip(q->tr("NaN color"));
  nanColorLayout->addWidget(nanButton);
  nanColorLayout->addWidget(nanLabel);

  discretizeCheckBox = new QCheckBox;
  discretizeCheckBox->setText(q->tr("Discretize"));
  discretizeCheckBox->setToolTip(q->tr("Discretize color transfer function"));
  nbOfDiscreteValuesSpinBox = new QSpinBox;
  nbOfDiscreteValuesSpinBox->setMinimum(1);
  nbOfDiscreteValuesSpinBox->setMaximum(255);
  nbOfDiscreteValuesSpinBox->setToolTip(q->tr("Number of discrete values"));
  nbOfDiscreteValuesSpinBox->setEnabled(discretizeCheckBox->isChecked());

  discretizeLayout->addWidget(discretizeCheckBox);
  discretizeLayout->addWidget(nbOfDiscreteValuesSpinBox);

  QMenu* optionMenu = new QMenu(optionButton);
  QWidgetAction* nanColorAction = new QWidgetAction(optionButton);
  nanColorAction->setDefaultWidget(nanColorWidget);
  QWidgetAction* discretizeAction = new QWidgetAction(optionButton);
  discretizeAction->setDefaultWidget(discretizeWidget);
  optionMenu->addAction(nanColorAction);
  optionMenu->addSeparator();
  optionMenu->addAction(discretizeAction);

  optionButton->setMenu(optionMenu);
  optionButton->setPopupMode(QToolButton::InstantPopup);

  QObject::connect(nanButton, SIGNAL(clicked()), q, SLOT(setNaNColor()));

  QObject::connect(discretizeCheckBox, SIGNAL(toggled(bool)),
    q, SLOT(setDiscretize(bool)));

  QObject::connect(nbOfDiscreteValuesSpinBox, SIGNAL(valueChanged(int)),
    q, SLOT(setNumberOfDiscreteValues(int)));

  ///Enable nbOfValuesSpinBox only if we use discretize
  QObject::connect(discretizeCheckBox, SIGNAL(toggled(bool)),
    nbOfDiscreteValuesSpinBox, SLOT(setEnabled(bool)));
}

// ----------------------------------------------------------------------------
void
ctkVTKDiscretizableColorTransferWidgetPrivate::colorTransferFunctionModifiedCallback(
  vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
  ctkVTKDiscretizableColorTransferWidgetPrivate* self =
    reinterpret_cast<ctkVTKDiscretizableColorTransferWidgetPrivate*>(
      clientdata);

  vtkSmartPointer<vtkDiscretizableColorTransferFunction> dctf =
    self->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction();

  if (dctf == nullptr)
  {
    return;
  }

  if (self->scalarsToColorsContextItem->IsProcessingColorTransferFunction())
  {
    return;
  }

  if (dctf->GetDiscretize())
  {
    dctf->Build();
  }

  self->discretizeCheckBox->setChecked(dctf->GetDiscretize());

  if (dctf->GetDiscretize())
  {
    self->nbOfDiscreteValuesSpinBox->setValue(dctf->GetNumberOfValues());
  }

  double* newRange = self->scalarsToColorsContextItem->GetCurrentRange();
  self->rangeSlider->setValues(newRange[0], newRange[1]);

  double r, g, b;
  self->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction()->
    GetNanColor(r, g, b);
  QColor selected = QColor::fromRgbF(r, g, b);
  self->nanButton->setColor(selected);

  self->ScalarsToColorsView->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
ctkVTKDiscretizableColorTransferWidget::ctkVTKDiscretizableColorTransferWidget(
  QWidget* parent)
  : QWidget(parent)
  , d_ptr(new ctkVTKDiscretizableColorTransferWidgetPrivate(*this))
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->setupUi(this);
}

// ----------------------------------------------------------------------------
ctkVTKDiscretizableColorTransferWidget::~ctkVTKDiscretizableColorTransferWidget()
{
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setColorTransferFunction(
  vtkScalarsToColors* ctf)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  vtkScalarsToColors* oldCtf =
    d->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction();
  if (oldCtf != nullptr)
  {
    oldCtf->RemoveObserver(d->colorTransferFunctionModified);
  }

  ///Setting the transfer function to the scalarsToColorsContextItem convert
  /// it to a vtkDiscretizableTransferFunction
  d->scalarsToColorsContextItem->SetColorTransferFunction(ctf);

  ctf = d->scalarsToColorsContextItem->GetColorTransferFunction();
  emit(currentScalarsToColorsChanged(ctf));

  if (ctf == nullptr)
  {
    d->rangeSlider->setRange(0., 255.);
    d->rangeSlider->setValues(0., 1.);
    d->rangeSlider->setEnabled(false);
    d->previousOpacityValue = 0.0;
    d->opacitySlider->setValue(d->previousOpacityValue);
    d->opacitySlider->setEnabled(false);
    d->optionButton->setEnabled(false);
    d->resetRangeButton->setEnabled(false);
    d->centerRangeButton->setEnabled(false);
    d->invertColorTransferFunctionButton->setEnabled(false);
    return;
  }

  // Set sliders values depending on the new color transfer function
  d->rangeSlider->setEnabled(true);
  d->opacitySlider->setEnabled(true);
  d->optionButton->setEnabled(true);
  d->resetRangeButton->setEnabled(true);
  d->centerRangeButton->setEnabled(true);
  d->invertColorTransferFunctionButton->setEnabled(true);

  double* newRange = d->scalarsToColorsContextItem->
    GetDiscretizableColorTransferFunction()->GetRange();

  d->rangeSlider->setRange(newRange[0], newRange[1]);

  d->previousOpacityValue = 1.0;
  d->opacitySlider->setValue(d->previousOpacityValue);

  ctf->AddObserver(
    vtkCommand::ModifiedEvent, d->colorTransferFunctionModified);
  d->colorTransferFunctionModified->Execute(ctf, vtkCommand::ModifiedEvent,
    this);
}

// ----------------------------------------------------------------------------
vtkScalarsToColors*
ctkVTKDiscretizableColorTransferWidget::colorTransferFunction() const
{
  Q_D(const ctkVTKDiscretizableColorTransferWidget);
  return d->scalarsToColorsContextItem->GetColorTransferFunction();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setHistogram(
  vtkImageAccumulate* histogram)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  histogram->Update();
  d->dataRange[0] = histogram->GetMin()[0];
  d->dataRange[1] = histogram->GetMax()[0];
  d->dataMean = histogram->GetMean()[0];

  int* output = static_cast<int*>(histogram->GetOutput()->GetScalarPointer());
  double spacing = histogram->GetComponentSpacing()[0];
  double bin = histogram->GetComponentOrigin()[0];

  vtkSmartPointer<vtkDoubleArray> bins =
    vtkSmartPointer<vtkDoubleArray>::New();
  bins->SetNumberOfComponents(1);
  bins->SetNumberOfTuples(255);
  bins->SetName("image_extents");
  vtkSmartPointer<vtkIntArray> frequencies =
    vtkSmartPointer<vtkIntArray>::New();
  frequencies->SetNumberOfComponents(1);
  frequencies->SetNumberOfTuples(255);
  frequencies->SetName("Frequency");

  for (unsigned int j = 0; j < 255; ++j)
  {
    bins->SetTuple1(j, bin);
    bin += spacing;
    frequencies->SetTuple1(j, *output++);
  }

  vtkNew<vtkTable> table;
  table->AddColumn(bins);
  table->AddColumn(frequencies);

  d->scalarsToColorsContextItem->SetHistogramTable(table.Get(),
    "image_extents", "Frequency");

  d->scalarsToColorsContextItem->SetDataRange(d->dataRange[0], d->dataRange[1]);

  d->ScalarsToColorsView->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onPaletteIndexChanged(
  vtkScalarsToColors* ctf)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (ctf == nullptr)
  {
    this->setColorTransferFunction(ctf);
    return;
  }

  if (ctf->IsA("vtkDiscretizableColorTransferFunction"))
  {
    vtkNew<vtkDiscretizableColorTransferFunction> newCtf;
    vtkNew<vtkPiecewiseFunction> newPf;
    newCtf->DeepCopy(vtkDiscretizableColorTransferFunction::SafeDownCast(ctf));
    newPf->DeepCopy(vtkDiscretizableColorTransferFunction::SafeDownCast(ctf)->GetScalarOpacityFunction());
    newCtf->SetScalarOpacityFunction(newPf.Get());
    newCtf->EnableOpacityMappingOn();
    this->setColorTransferFunction(newCtf.Get());
  }
  else if (ctf->IsA("vtkColorTransferFunction"))
  {
    vtkNew<vtkColorTransferFunction> newCtf;
    newCtf->DeepCopy(vtkColorTransferFunction::SafeDownCast(ctf));
    this->setColorTransferFunction(newCtf.Get());
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setGlobalOpacity(double value)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsContextItem->SetGlobalOpacity(
    value / d->previousOpacityValue);
  d->previousOpacityValue = value;
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setNaNColor()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  QColor selected = d->nanButton->color();
  d->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction()->
    SetNanColor(selected.redF(), selected.greenF(), selected.blueF());
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setDiscretize(bool checked)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction()->
    SetDiscretize(checked);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setNumberOfDiscreteValues(
  int value)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction()
    ->SetNumberOfValues(value);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setColorTransferFunctionRange(
  double minValue, double maxValue)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->scalarsToColorsContextItem->SetCurrentRange(minValue, maxValue);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onCurrentPointEdit()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  double rgb[3];
  if (d->scalarsToColorsContextItem->GetCurrentControlPointColor(rgb))
  {
    QColor color = QColorDialog::getColor(
      QColor::fromRgbF(rgb[0], rgb[1], rgb[2]), this, "Select color at point",
        QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
      rgb[0] = color.redF();
      rgb[1] = color.greenF();
      rgb[2] = color.blueF();
      d->scalarsToColorsContextItem->SetCurrentControlPointColor(rgb);
    }
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::resetColorTransferFunctionRange()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  if (d->dataRange[0] <= d->dataRange[1])
  {
    d->scalarsToColorsContextItem->SetCurrentRange(
      d->dataRange[0], d->dataRange[1]);
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::centerColorTransferFunctionRange()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsContextItem->CenterRange(d->dataMean);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::invertColorTransferFunction()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsContextItem->InvertColorTransferFunction();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setViewBackgroundColor(
  const QColor& i_color)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsContextView->GetRenderer()->SetBackground(
    i_color.redF(), i_color.greenF(), i_color.blueF());
}

// ----------------------------------------------------------------------------
QColor ctkVTKDiscretizableColorTransferWidget::viewBackgroundColor() const
{
  Q_D(const ctkVTKDiscretizableColorTransferWidget);
  double rgb[3];
  d->scalarsToColorsContextView->GetRenderer()->GetBackground(rgb);
  return QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
}

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsComboBox*
ctkVTKDiscretizableColorTransferWidget::scalarsToColorsSelector() const
{
  Q_D(const ctkVTKDiscretizableColorTransferWidget);
  return d->scalarsToColorsSelector;
}
