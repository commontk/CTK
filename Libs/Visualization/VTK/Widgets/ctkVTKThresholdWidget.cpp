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
#include "ctkLogger.h"
#include "ctkVTKThresholdWidget.h"
#include "ctkUtils.h"
#include "ui_ctkVTKThresholdWidget.h"

// VTK includes
#include <vtkPiecewiseFunction.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKThresholdWidget");
//----------------------------------------------------------------------------

class ctkVTKThresholdWidgetPrivate:
  public Ui_ctkVTKThresholdWidget
{
   Q_DECLARE_PUBLIC(ctkVTKThresholdWidget);
protected:
  ctkVTKThresholdWidget* const q_ptr;
public:
  ctkVTKThresholdWidgetPrivate(ctkVTKThresholdWidget& object);
  void setupUi(QWidget* widget);

  void setThreshold(double min, double max, double opacity);
  void setRange(double min, double max);
  void guessThreshold(double& min, double& max, double& opacity)const;
  
  vtkPiecewiseFunction* PiecewiseFunction;
  bool UserRange;
protected:
  void setNodes(double nodeValues[4][4]);
  void setNodeValue(int index, double* nodeValue);
};

// ----------------------------------------------------------------------------
// ctkVTKThresholdWidgetPrivate methods

// ----------------------------------------------------------------------------
ctkVTKThresholdWidgetPrivate::ctkVTKThresholdWidgetPrivate(
  ctkVTKThresholdWidget& object)
  : q_ptr(&object)
{
  this->PiecewiseFunction = 0;
  this->UserRange = false;
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidgetPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkVTKThresholdWidget);
  Q_ASSERT(q == widget);
  this->Ui_ctkVTKThresholdWidget::setupUi(widget);

  QObject::connect(this->ThresholdSliderWidget, SIGNAL(valuesChanged(double,double)),
                   q, SLOT(setThresholdValues(double,double)));
  QObject::connect(this->OpacitySliderWidget, SIGNAL(valueChanged(double)),
                   q, SLOT(setOpacity(double)));
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidgetPrivate::guessThreshold(double& min, double& max, double& opacity)const
{
  min = this->ThresholdSliderWidget->minimum();
  max = this->ThresholdSliderWidget->maximum();
  opacity = 1.;
  if (!this->PiecewiseFunction || this->PiecewiseFunction->GetSize() == 0)
    {
    return;
    }
  int minIndex; 
  for (minIndex=0; minIndex < this->PiecewiseFunction->GetSize(); ++minIndex)
    {
    double node[4];
    this->PiecewiseFunction->GetNodeValue(minIndex, node);
    if (node[1] > 0.)
      {
      min = node[0];
      opacity = node[1];
      break;
      }
    }
  if (minIndex == this->PiecewiseFunction->GetSize())
    {
    return;
    }
  int maxIndex;
  for (maxIndex = minIndex + 1;
       maxIndex < this->PiecewiseFunction->GetSize();
       ++maxIndex)
    {
    double node[4];
    this->PiecewiseFunction->GetNodeValue(maxIndex, node);
    // average the opacities
    opacity += node[1];
    if (node[1] == 0.)
      {
      max = node[0];
      opacity /= maxIndex - minIndex + 1;
      break;
      }
    }
  // couldn't find the upper threshold value, use the upper range
  if (maxIndex == this->PiecewiseFunction->GetSize())
    {
    opacity /= maxIndex - minIndex;
    }
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidgetPrivate::setRange(double min, double max)
{
  bool wasBlocking = this->ThresholdSliderWidget->blockSignals(true);
  int decimals = qMax(0, -ctk::orderOfMagnitude(max - min) + 2);
  this->ThresholdSliderWidget->setDecimals(decimals);
  this->ThresholdSliderWidget->setSingleStep(pow(10., -decimals));
  this->ThresholdSliderWidget->setRange(min, max);
  this->ThresholdSliderWidget->blockSignals(wasBlocking);
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidgetPrivate::setThreshold(double min, double max, double opacity)
{
  Q_Q(ctkVTKThresholdWidget);
  if (!this->PiecewiseFunction)
    {
    return;
    }

  double range[2];
  this->ThresholdSliderWidget->range(range);

  // Start of the curve, always y=0
  double nodes[4][4];
  nodes[0][0] = range[0];
  nodes[0][1] = 0.;
  nodes[0][2] = 0.5; // midpoint
  nodes[0][3] = 0.; // sharpness

  // Starting threshold point with a sharp slope that jumps to the opacity
  // which is set by the next point
  nodes[1][0] = min  +
    ((min == nodes[0][0] && !this->PiecewiseFunction->GetAllowDuplicateScalars()) ?
      0.00000000000001 : 0.);
  nodes[1][1] = 0.;
  nodes[1][2] = 0.; // jumps directly
  nodes[1][3] = 1.; // sharp

  // Ending threshold point with a sharp slope that jumps back to a 0 opacity
  nodes[2][0] = max +
    ((max == nodes[1][0] && !this->PiecewiseFunction->GetAllowDuplicateScalars()) ?
      0.00000000000001 : 0.);
  nodes[2][1] = opacity;
  nodes[2][2] = 0.;
  nodes[2][3] = 1.;

  // End of the curve, always y = 0
  nodes[3][0] = range[1] +
   ((range[1] == nodes[2][0] && !this->PiecewiseFunction->GetAllowDuplicateScalars()) ?
      0.00000000000001 : 0.);
  nodes[3][1] = 0.;
  nodes[3][2] = 0.5;
  nodes[3][3] = 0.;

  q->qvtkBlock(this->PiecewiseFunction, vtkCommand::ModifiedEvent, q);
  this->setNodes(nodes);
  q->qvtkUnblock(this->PiecewiseFunction, vtkCommand::ModifiedEvent, q);
  q->updateFromPiecewiseFunction();
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidgetPrivate::setNodes(double nodeValues[4][4])
{
  double lastX = VTK_DOUBLE_MIN;
  double minX = nodeValues[0][0];
  for(int i = 0; i < 4; ++i)
    {
    int index = this->PiecewiseFunction->GetSize();
    bool alreadyEqual = false;
    // search the node index to modify
    for (int j = 0; j < this->PiecewiseFunction->GetSize(); ++j)
      {
      double node[4];
      this->PiecewiseFunction->GetNodeValue(j, node);
      if (node[0] < minX || node[0] > lastX)
        {
        index = j;
        break;
        }
      else if (node[0] == lastX)
        {
        if (alreadyEqual)
          {
          index = j;
          break;
          }
        alreadyEqual = true;
        }
      }
    this->setNodeValue(index, nodeValues[i]);
    lastX = nodeValues[i][0];
    }
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidgetPrivate::setNodeValue(int index, double* nodeValues)
{
  if (this->PiecewiseFunction->GetSize() <= index)
    {
    this->PiecewiseFunction->AddPoint(
      nodeValues[0], nodeValues[1], nodeValues[2], nodeValues[3]);
    }
  else
    {
    this->PiecewiseFunction->SetNodeValue(index, nodeValues);
    }
}

// ----------------------------------------------------------------------------
// ctkVTKThresholdWidget methods

// ----------------------------------------------------------------------------
ctkVTKThresholdWidget::ctkVTKThresholdWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
   , d_ptr(new ctkVTKThresholdWidgetPrivate(*this))
{
  Q_D(ctkVTKThresholdWidget);
  d->setupUi(this);
}

// ----------------------------------------------------------------------------
ctkVTKThresholdWidget::~ctkVTKThresholdWidget()
{
}

// ----------------------------------------------------------------------------
vtkPiecewiseFunction* ctkVTKThresholdWidget::piecewiseFunction()const
{
  Q_D(const ctkVTKThresholdWidget);
  return d->PiecewiseFunction;
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidget
::setPiecewiseFunction(vtkPiecewiseFunction* newFunction)
{
  Q_D(ctkVTKThresholdWidget);
  if (d->PiecewiseFunction == newFunction)
    {
    return;
    }
  this->qvtkReconnect(d->PiecewiseFunction, newFunction, vtkCommand::ModifiedEvent,
                      this, SLOT(updateFromPiecewiseFunction()));
  d->PiecewiseFunction = newFunction;
  
  if (!d->UserRange)
    {
    double range[2] = {0., 1.};
    if (d->PiecewiseFunction)
      {
      d->PiecewiseFunction->GetRange(range);
      }
    d->setRange(range[0], range[1]);
    }
  if (d->PiecewiseFunction)
    {
    double min, max, value;
    d->guessThreshold(min, max, value);
    d->setThreshold(min, max, value);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidget::updateFromPiecewiseFunction()
{
  Q_D(ctkVTKThresholdWidget);
  
  if (!d->PiecewiseFunction)
    {
    return;
    }
  double range[2];
  d->ThresholdSliderWidget->range(range);
  double minThreshold = range[0];
  double maxThreshold = range[1];
  double opacity = d->OpacitySliderWidget->value();
  double node[4];
  if (d->PiecewiseFunction->GetSize() > 1)
    {
    d->PiecewiseFunction->GetNodeValue(1, node);
    minThreshold = node[0];
    }
  if (d->PiecewiseFunction->GetSize() > 2)
    {
    d->PiecewiseFunction->GetNodeValue(2, node);
    maxThreshold = node[0];
    opacity = node[1];
    }
  if (d->PiecewiseFunction->GetSize() > 3)
    {
    d->PiecewiseFunction->GetNodeValue(3, node);
    }
  bool wasBlocking = d->ThresholdSliderWidget->blockSignals(true);
  d->ThresholdSliderWidget->setValues(minThreshold, maxThreshold);
  d->ThresholdSliderWidget->blockSignals(wasBlocking);
  d->OpacitySliderWidget->blockSignals(true);
  d->OpacitySliderWidget->setValue(opacity);
  d->OpacitySliderWidget->blockSignals(wasBlocking);
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidget::setThresholdValues(double min, double max)
{
  Q_D(ctkVTKThresholdWidget);
  d->setThreshold(min, max, d->OpacitySliderWidget->value());
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidget::thresholdValues(double* values)const
{
  Q_D(const ctkVTKThresholdWidget);
  values[0] = d->ThresholdSliderWidget->minimumValue();
  values[1] = d->ThresholdSliderWidget->maximumValue();
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidget::setOpacity(double opacity)
{
  Q_D(ctkVTKThresholdWidget);
  d->setThreshold(d->ThresholdSliderWidget->minimumValue(),
                  d->ThresholdSliderWidget->maximumValue(), opacity);
}

// ----------------------------------------------------------------------------
double ctkVTKThresholdWidget::opacity()const
{
  Q_D(const ctkVTKThresholdWidget);
  return d->OpacitySliderWidget->value();
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidget::range(double* range)const
{
  Q_D(const ctkVTKThresholdWidget);
  d->ThresholdSliderWidget->range(range);
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidget::setRange(double min, double max)
{
  Q_D(ctkVTKThresholdWidget);
  d->UserRange = true;
  d->setRange(min, max);
}
