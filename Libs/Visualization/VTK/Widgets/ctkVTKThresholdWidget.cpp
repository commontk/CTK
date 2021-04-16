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

// STD includes
#include <cmath> // for pow

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
  bool UseSharpness;

protected:
  void setNodes(double nodeValues[4][4]);
  void setNodes(double nodeValues[][4], const int nodeCount);
  void setNodeValue(int index, double* nodeValue);
  double safeX(double value, double& previous)const;
  static double nextHigher(double value);
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
  this->UseSharpness = false;
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
  // The min index is the point before the opacity is >0
  for (minIndex=0; minIndex < this->PiecewiseFunction->GetSize(); ++minIndex)
    {
    double node[4];
    this->PiecewiseFunction->GetNodeValue(minIndex, node);
    if (node[1] > 0.)
      {
      opacity = node[1];
      max = node[0];
      break;
      }
    min = node[0];
    }
  if (minIndex == this->PiecewiseFunction->GetSize())
    {
    return;
    }
  int maxIndex = minIndex + 1;
  for (;maxIndex < this->PiecewiseFunction->GetSize(); ++maxIndex)
    {
    double node[4];
    this->PiecewiseFunction->GetNodeValue(maxIndex, node);
    // use the max opacity
    opacity = std::max(opacity, node[1]);
    if (node[1] < opacity)
      {
      break;
      }
    max = node[0];
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

//----------------------------------------------------------------------------
double ctkVTKThresholdWidgetPrivate::nextHigher(double value)
{
  // Increment the value by the smallest offset possible
  typedef union {
      long long i64;
      double d64;
    } dbl_64;
  dbl_64 d;
  d.d64 = value;
  d.i64 += (value >= 0) ? 1 : -1;
  return d.d64;
}

// ----------------------------------------------------------------------------
double ctkVTKThresholdWidgetPrivate::safeX(double value, double& previous)const
{
  if (value < previous)
    {
    value = previous;
    }
  if (value == previous && !this->PiecewiseFunction->GetAllowDuplicateScalars())
    {
    value = this->nextHigher(value);
    }
  previous = value;
  return value;
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

  int nodeCount = 0;
  if (this->UseSharpness)
    {
    //        +------2
    //        |      |
    // 0------1      +----3
    nodeCount = 4;
    }
  else 
    {
    //        2------3
    //        |      |
    // 0------1      4----5
    nodeCount = 6;
    }
  double (*nodes)[4] = new double[nodeCount][4];

  double previous = VTK_DOUBLE_MIN;
  // Start of the curve, always y=0
  int index = 0;
  nodes[index][0] = this->safeX(range[0], previous);
  nodes[index][1] = 0.;
  nodes[index][2] = 0.5; // midpoint
  nodes[index][3] = 0.; // sharpness
  ++index;

  // Starting threshold point with a sharp slope that jumps to the opacity
  // which is set by the next point
  nodes[index][0] = this->safeX(min, previous);
  nodes[index][1] = 0.;
  nodes[index][2] = this->UseSharpness ? 0. : 0.5;
  nodes[index][3] = this->UseSharpness ? 1. : 0.;
  ++index;

  if (!this->UseSharpness)
    {
    nodes[index][0] = this->safeX(min, previous);
    nodes[index][1] = opacity;
    nodes[index][2] = 0.5;
    nodes[index][3] = 0.;
    ++index;
    }

  // Ending threshold point with a sharp slope that jumps back to a 0 opacity
  nodes[index][0] = this->safeX(max, previous);;
  nodes[index][1] = opacity;
  nodes[index][2] = this->UseSharpness ? 0. : 0.5;
  nodes[index][3] = this->UseSharpness ? 1. : 0.;
  ++index;

  if (!this->UseSharpness)
    {
    nodes[index][0] = this->safeX(max, previous);
    nodes[index][1] = 0.;
    nodes[index][2] = 0.5;
    nodes[index][3] = 0.;
    ++index;
    }

  // End of the curve, always y = 0
  nodes[index][0] = this->safeX(range[1], previous);
  nodes[index][1] = 0.;
  nodes[index][2] = 0.5;
  nodes[index][3] = 0.;
  ++index;

  q->qvtkBlock(this->PiecewiseFunction, vtkCommand::ModifiedEvent, q);
  this->setNodes(nodes, nodeCount);
  q->qvtkUnblock(this->PiecewiseFunction, vtkCommand::ModifiedEvent, q);
  q->updateFromPiecewiseFunction();
  delete []nodes;
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidgetPrivate
::setNodes(double nodeValues[][4], const int nodeCount)
{
  for(int i = 0; i < nodeCount; ++i)
    {
    int index = i;
    double node[4];
    // Search the index where to write the node
    for (int j = 0; j < i; ++j)
      {
      this->PiecewiseFunction->GetNodeValue(j, node);
      bool different = node[0] != nodeValues[j][0] ||
                       node[1] != nodeValues[j][1] ||
                       node[2] != nodeValues[j][2] ||
                       node[3] != nodeValues[j][3];
      if (different)
        {
        index = j;
        break;
        }
      }
    if (index >= this->PiecewiseFunction->GetSize())
      {
      node[0] = VTK_DOUBLE_MAX;
      }
    else if (index == i)
      {
      this->PiecewiseFunction->GetNodeValue(index, node);
      }
    // else we already have node correctly set
    if (!this->UseSharpness)
      {
      // be smart in order to reduce "jumps"
      // Here is the intermediate steps we try to avoid:
      // Start
      //     2---3
      //     |   |
      // 0---1   4--5
      // After node1 is set:
      //     1   _3
      //     |\ / |
      // 0---| 2  4--5
      // After node1 is set:
      //       2--3
      //       |  |
      // 0-----1  4--5
      if ((i == 1 && node[0] < nodeValues[i][0]) ||
          (i == 3 && node[0] < nodeValues[i][0]))
        {
        this->setNodeValue(index + 1, nodeValues[i+1]);
        }
      }
    this->setNodeValue(index, nodeValues[i]);
    }
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidgetPrivate::setNodeValue(int index, double* nodeValues)
{
  if (index >= this->PiecewiseFunction->GetSize())
    {
    this->PiecewiseFunction->AddPoint(
      nodeValues[0], nodeValues[1], nodeValues[2], nodeValues[3]);
    }
  else
    {
    double values[4];
    this->PiecewiseFunction->GetNodeValue(index, values);
    // Updating the node will fire a modified event which can be costly
    // We change the node values only if there is a change.
    if (values[0] != nodeValues[0] ||
        values[1] != nodeValues[1] ||
        values[2] != nodeValues[2] ||
        values[3] != nodeValues[3])
      {
      this->PiecewiseFunction->SetNodeValue(index, nodeValues);
      }
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
  int minIndex = 1;
  if (d->PiecewiseFunction->GetSize() > minIndex)
    {
    d->PiecewiseFunction->GetNodeValue(minIndex, node);
    minThreshold = node[0];
    }
  int maxIndex = d->UseSharpness ? 2 : 3;
  if (d->PiecewiseFunction->GetSize() > maxIndex)
    {
    d->PiecewiseFunction->GetNodeValue(maxIndex, node);
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

// ----------------------------------------------------------------------------
bool ctkVTKThresholdWidget::useSharpness()const
{
  Q_D(const ctkVTKThresholdWidget);
  return d->UseSharpness;
}

// ----------------------------------------------------------------------------
void ctkVTKThresholdWidget::setUseSharpness(bool use)
{
  Q_D(ctkVTKThresholdWidget);
  if (use == d->UseSharpness)
    {
    return;
    }
  d->UseSharpness = use;

  double min, max, value;
  d->guessThreshold(min, max, value);
  d->setThreshold(min, max, value);
}
