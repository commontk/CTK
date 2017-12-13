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
#include <QDebug>
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
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkIntArray.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkScalarsToColors.h>
#include <vtkTable.h>

//#define DEBUG_RANGE

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
  vtkSmartPointer<vtkImageAccumulate> histogramFilter;

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
  this->scalarsToColorsSelector = CTK_NULLPTR;

  // Option menu
  this->nanButton = CTK_NULLPTR;
  this->discretizeCheckBox = CTK_NULLPTR;
  this->nbOfDiscreteValuesSpinBox = CTK_NULLPTR;

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
  this->gridLayout->addWidget(this->ScalarsToColorsView, 2, 2, 8, 1);

  this->scalarsToColorsContextItem = vtkSmartPointer<vtkScalarsToColorsContextItem>::New();
  vtkDiscretizableColorTransferFunction* ctf = this->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction();
  ctf->AddObserver(vtkCommand::ModifiedEvent, this->colorTransferFunctionModified);

  this->scalarsToColorsContextView = vtkSmartPointer<vtkContextView> ::New();

#if CTK_USE_QVTKOPENGLWIDGET
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> renwin =
    vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  this->ScalarsToColorsView->SetRenderWindow(renwin);
#endif

  this->scalarsToColorsContextView->SetRenderWindow(
    this->ScalarsToColorsView->GetRenderWindow());
  this->scalarsToColorsContextView->SetInteractor(
    this->ScalarsToColorsView->GetInteractor());
  this->scalarsToColorsContextView->GetScene()->AddItem(
    this->scalarsToColorsContextItem.Get());

  q->setViewBackgroundColor(QColor(49, 54, 59));

  this->previousOpacityValue = opacitySlider->value();

  this->scalarsToColorsSelector->addScalarsToColors(CTK_NULLPTR, q->tr("Reset"));
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

  QObject::connect(zoomOutButton, SIGNAL(clicked()),
    q, SLOT(resetVisibleRangeToData()));

  QObject::connect(zoomInButton, SIGNAL(clicked()),
    q, SLOT(resetVisibleRangeToCTF()));

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
    QStyle::SP_FileDialogDetailedView, CTK_NULLPTR, optionButton));

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
  Q_UNUSED(caller);
  Q_UNUSED(eid);
  Q_UNUSED(calldata);
  ctkVTKDiscretizableColorTransferWidgetPrivate* self =
    reinterpret_cast<ctkVTKDiscretizableColorTransferWidgetPrivate*>(
      clientdata);

  vtkSmartPointer<vtkDiscretizableColorTransferFunction> dctf =
    self->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction();

  if (dctf == CTK_NULLPTR)
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
void ctkVTKDiscretizableColorTransferWidget::copyColorTransferFunction(
  vtkScalarsToColors* ctf, bool useCtfRange)
{
#ifdef DEBUG_RANGE
  if (ctf)
  {
    qDebug() << "DEBUG_RANGE ctf input range = " << ctf->GetRange()[0]
             << " " << ctf->GetRange()[1];
  }
#endif

  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (useCtfRange)
  {
    // set cft, current range and visible range
    d->scalarsToColorsContextItem->CopyColorTransferFunction(ctf);
    emit(currentScalarsToColorsChanged(d->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction()));
  }
  else
  {
    // save old ranges
    double ctfRange[2];
    ctfRange[0] = this->getColorTransferFunctionRange()[0];
    ctfRange[1] = this->getColorTransferFunctionRange()[1];
    double visibleRange[2];
    visibleRange[0] = this->getVisibleRange()[0];
    visibleRange[1] = this->getVisibleRange()[1];

    // set cft, current range and visible range
    d->scalarsToColorsContextItem->CopyColorTransferFunction(ctf);
    emit(currentScalarsToColorsChanged(d->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction()));

    // set old ranges back
    if (visibleRange[0] < visibleRange[1])
    {
      this->setVisibleRange(visibleRange[0], visibleRange[1]);
      this->setColorTransferFunctionRange(ctfRange[0], ctfRange[1]);
    }
  }

  // todo should be replaced by callback when visible range changes
  this->updateCtfWidgets();

  d->colorTransferFunctionModified->Execute(ctf, vtkCommand::ModifiedEvent, this);
}

// ----------------------------------------------------------------------------
vtkDiscretizableColorTransferFunction*
ctkVTKDiscretizableColorTransferWidget::discretizableColorTransferFunction()
const
{
  Q_D(const ctkVTKDiscretizableColorTransferWidget);
  return d->scalarsToColorsContextItem->GetDiscretizableColorTransferFunction();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setLeftAxisMode(int mode)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->scalarsToColorsContextItem->SetLeftAxisMode(mode);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setHistogramConnection(
  vtkAlgorithmOutput* input)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (!input)
  {
    d->histogramFilter = CTK_NULLPTR;
    d->dataMean = 0.;
    this->setDataRange(VTK_DOUBLE_MAX, VTK_DOUBLE_MIN);
    return;
  }

  d->histogramFilter = vtkSmartPointer<vtkImageAccumulate>::New();
  d->histogramFilter->SetInputConnection(input);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::updateHistogram(
  bool updateDataRange)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  this->updateHistogram();

  if (updateDataRange
   && d->histogramFilter
   && d->histogramFilter->GetInputConnection(0, 0))
  {
    // get min max values from histogram
    d->dataMean = d->histogramFilter->GetMean()[0];
    this->setDataRange(d->histogramFilter->GetMin()[0],
                       d->histogramFilter->GetMax()[0]);
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::resetColorTransferFunctionRange(
    ResetCTFRange resetMode)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  double newRange[2];
  switch (resetMode)
  {
    case DATA:
    {
      double* dataRange = this->getDataRange();
      newRange[0] = dataRange[0];
      newRange[1] = dataRange[1];
      break;
    }
    case VISIBLE:
    {
      double* visibleRange = this->getVisibleRange();
      newRange[0] = visibleRange[0];
      newRange[1] = visibleRange[1];
      break;
    }
    default:
      return;
  }

  this->setColorTransferFunctionRange(newRange[0], newRange[1]);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::resetVisibleRange(
    ResetVisibleRange resetMode)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  double newRange[2];
  switch (resetMode)
  {
    case UNION_DATA_AND_CTF:
    {
      double* ctfRange = this->getColorTransferFunctionRange();
      double* dataRange = this->getDataRange();
      newRange[0] = std::min(dataRange[0], ctfRange[0]);
      newRange[1] = std::max(dataRange[1], ctfRange[1]);
      break;
    }
    case UNION_DATA_AND_VISIBLE:
    {
      double* visibleRange = this->getVisibleRange();
      double* dataRange = this->getDataRange();
      newRange[0] = std::min(dataRange[0], visibleRange[0]);
      newRange[1] = std::max(dataRange[1], visibleRange[1]);
      break;
    }
    case ONLY_DATA:
    {
      double* dataRange = this->getDataRange();
      newRange[0] = dataRange[0];
      newRange[1] = dataRange[1];
      break;
    }
    case ONLY_CTF:
    {
      double* ctfRange = this->getColorTransferFunctionRange();
      newRange[0] = ctfRange[0];
      newRange[1] = ctfRange[1];
      break;
    }
    default:
      return;
  }

  this->setVisibleRange(newRange[0], newRange[1]);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::updateCtfWidgets()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (this->discretizableColorTransferFunction() == CTK_NULLPTR)
  {
    this->disableCtfWidgets();
  }
  else
  {
    this->enableCtfWidgets();
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::disableCtfWidgets()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->rangeSlider->setRange(0., 255.);
  d->rangeSlider->setValues(0., 1.);
  d->rangeSlider->setEnabled(false);
  d->previousOpacityValue = 0.0;
  d->opacitySlider->setValue(d->previousOpacityValue);
  d->opacitySlider->setEnabled(false);
  d->optionButton->setEnabled(false);
  d->resetRangeButton->setEnabled(false);
  d->zoomInButton->setEnabled(false);
  d->zoomOutButton->setEnabled(false);
  d->centerRangeButton->setEnabled(false);
  d->invertColorTransferFunctionButton->setEnabled(false);

#ifdef DEBUG_RANGE
  qDebug() << "DEBUG_RANGE slider range = " << 0
           << " " << 255;
  qDebug() << "DEBUG_RANGE slider value = " << 0
           << " " << 1;
#endif
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::enableCtfWidgets()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->rangeSlider->setEnabled(true);
  d->opacitySlider->setEnabled(true);
  d->optionButton->setEnabled(true);
  d->resetRangeButton->setEnabled(true);
  d->zoomInButton->setEnabled(true);
  d->zoomOutButton->setEnabled(true);
  d->centerRangeButton->setEnabled(true);
  d->invertColorTransferFunctionButton->setEnabled(true);

  d->previousOpacityValue = 1.0;
  d->opacitySlider->setValue(d->previousOpacityValue);

  double* visibleRange = this->getVisibleRange();
  double* ctfRange = this->getColorTransferFunctionRange();
  d->rangeSlider->setRange(visibleRange[0], visibleRange[1]);
  d->rangeSlider->setValues(ctfRange[0], ctfRange[1]);

#ifdef DEBUG_RANGE
  qDebug() << "DEBUG_RANGE slider range = " << visibleRange[0]
           << " " << visibleRange[1];
  qDebug() << "DEBUG_RANGE slider value = " << ctfRange[0]
           << " " << ctfRange[1];
#endif
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::updateHistogram()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  // convert histogram data into table

  std::string binsName = "image_extents";
  std::string frequenciesName = "Frequency";

  vtkSmartPointer<vtkDoubleArray> bins =
    vtkSmartPointer<vtkDoubleArray>::New();
  bins->SetNumberOfComponents(1);
  bins->SetName(binsName.c_str());

  vtkSmartPointer<vtkIntArray> frequencies =
    vtkSmartPointer<vtkIntArray>::New();
  frequencies->SetNumberOfComponents(1);
  frequencies->SetName(frequenciesName.c_str());

  vtkNew<vtkTable> table;
  table->AddColumn(bins);
  table->AddColumn(frequencies);

  // fill bins and frequencies

  if (d->histogramFilter == CTK_NULLPTR
   || d->histogramFilter->GetInputConnection(0, 0) == CTK_NULLPTR)
  {
    bins->SetNumberOfTuples(1);
    bins->SetTuple1(0, 0);

    frequencies->SetNumberOfTuples(1);
    frequencies->SetTuple1(0, 0);
  }
  else
  {
    double* visibleRange = d->scalarsToColorsContextItem->GetVisibleRange();

    int extent = d->histogramFilter->GetComponentExtent()[1];
    double origin = visibleRange[0] - std::numeric_limits<double>::epsilon();
    double spacing = (visibleRange[1] - visibleRange[0] + 2 * std::numeric_limits<double>::epsilon())
        / static_cast<double>(extent + 1);

    // recompute histogram in data range
    d->histogramFilter->SetComponentOrigin(origin, 0, 0);
    d->histogramFilter->SetComponentSpacing(spacing, 0, 0);
    d->histogramFilter->Update();

    vtkImageData* histogram = d->histogramFilter->GetOutput();
    vtkIdType* output = static_cast<vtkIdType*>(histogram->GetScalarPointer());

    // set min and max of the slider widget
    vtkDataObject* input = d->histogramFilter->GetInputAlgorithm()->GetOutputDataObject(0);
    vtkImageData* inputImage = vtkImageData::SafeDownCast(input);
    d->rangeSlider->setCustomSpinBoxesLimits(inputImage->GetScalarTypeMin(),
                                             inputImage->GetScalarTypeMax());

#ifdef DEBUG_RANGE
    qDebug() << "DEBUG_RANGE histo input range = " << origin
             << " " << origin + (extent + 1) * spacing;
    qDebug() << "DEBUG_RANGE histo real range = " << *d->histogramFilter->GetMin()
             << " " << *d->histogramFilter->GetMax();
    int dims[3];
    histogram->GetDimensions(dims);
    QDebug deb = qDebug();
    deb << "DEBUG_RANGE histo = ";
    for(vtkIdType i = 0; i < dims[0]; ++i)
    {
        deb << *(static_cast<vtkIdType*>(histogram->GetScalarPointer(i, 0, 0))) << " ";
    }
#endif

    bins->SetNumberOfTuples(extent + 1);
    frequencies->SetNumberOfTuples(extent + 1);

    double bin = origin;
    for (int j = 0; j < extent + 1; ++j)
    {
      bins->SetTuple1(j, bin);
      bin += spacing;
      frequencies->SetTuple1(j, *output++);
    }
  }

  d->scalarsToColorsContextItem->SetHistogramTable(table.Get(),
    binsName.c_str(), frequenciesName.c_str());
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onPaletteIndexChanged(
  vtkScalarsToColors* ctf)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  this->copyColorTransferFunction(ctf);
  d->ScalarsToColorsView->GetInteractor()->Render();
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
double* ctkVTKDiscretizableColorTransferWidget::getColorTransferFunctionRange()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  return d->scalarsToColorsContextItem->GetCurrentRange();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setColorTransferFunctionRange(
  double min, double max)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (min == this->getColorTransferFunctionRange()[0]
   && max == this->getColorTransferFunctionRange()[1])
  {
    return;
  }

  if (max < min)
  {
    return;
  }

  double* visibleRange = this->getVisibleRange();
  if (min < visibleRange[0]
   || max > visibleRange[1])
  {
    double newRange[2] = { visibleRange[0], visibleRange[1] };
    if (min < visibleRange[0])
    {
      newRange[0] = min;
    }
    if (max > visibleRange[1])
    {
      newRange[1] = max;
    }
    this->setVisibleRange(newRange[0], newRange[1]);
  }

  d->scalarsToColorsContextItem->SetCurrentRange(min, max);
}

// ----------------------------------------------------------------------------
double* ctkVTKDiscretizableColorTransferWidget::getVisibleRange()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  return d->scalarsToColorsContextItem->GetVisibleRange();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setVisibleRange(
    double min, double max)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (min == this->getVisibleRange()[0]
   && max == this->getVisibleRange()[1])
  {
    return;
  }

  if (max < min)
  {
    return;
  }

  d->scalarsToColorsContextItem->SetVisibleRange(min, max);

  // todo should be replaced by callback when visible range changes
  this->updateHistogram();
  this->updateCtfWidgets();
}

// ----------------------------------------------------------------------------
double* ctkVTKDiscretizableColorTransferWidget::getDataRange()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  return d->scalarsToColorsContextItem->GetDataRange();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setDataRange(
    double min, double max)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (min == this->getDataRange()[0]
   && max == this->getDataRange()[1])
  {
    return;
  }

  d->scalarsToColorsContextItem->SetDataRange(min, max);
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
void ctkVTKDiscretizableColorTransferWidget::resetVisibleRangeToData()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  this->resetVisibleRange(ctkVTKDiscretizableColorTransferWidget::ONLY_DATA);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::resetVisibleRangeToCTF()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  this->resetVisibleRange(ctkVTKDiscretizableColorTransferWidget::ONLY_CTF);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::resetColorTransferFunctionRange()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  this->resetColorTransferFunctionRange(ctkVTKDiscretizableColorTransferWidget::VISIBLE);
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
