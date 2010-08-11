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
#include <QHBoxLayout>

// CTK includes
#include "ctkRangeSlider.h"
#include "ctkDoubleRangeSlider.h"


//-----------------------------------------------------------------------------
class ctkDoubleRangeSliderPrivate: public ctkPrivate<ctkDoubleRangeSlider>
{
public:
  ctkDoubleRangeSliderPrivate();
  
  int toInt(double _value)const;
  double minFromInt(int _value)const;
  double maxFromInt(int _value)const;
  void init();
  void connectSlider();
  void updateMinOffset(double value);
  void updateMaxOffset(double value);

  ctkRangeSlider* Slider;
  double Minimum;
  double Maximum;
  bool   SettingRange;

  // we should have a MinValueOffset and MinPositionOffset (and MinimumOffset?)
  double MinOffset;
  // we should have a MaxValueOffset and MaxPositionOffset (and MaximumOffset?)
  double MaxOffset;
  double SingleStep;
  double MinValue;
  double MaxValue;
};

// --------------------------------------------------------------------------
ctkDoubleRangeSliderPrivate::ctkDoubleRangeSliderPrivate()
{
  // the initial values will be overwritten in
  // ctkDoubleRangeSliderPrivate::init()
  this->Slider = 0;
  this->Minimum = 0.;
  this->Maximum = 99.;
  this->SettingRange = false;
  this->MinOffset = 0.;
  this->MaxOffset = 0.;
  this->SingleStep = 1.;
  this->MinValue = 0.;
  this->MaxValue = 99.;
}
 
// --------------------------------------------------------------------------
void ctkDoubleRangeSliderPrivate::init()
{
  CTK_P(ctkDoubleRangeSlider);
  this->Slider = new ctkRangeSlider(p);
  QHBoxLayout* l = new QHBoxLayout(p);
  l->addWidget(this->Slider);
  l->setContentsMargins(0,0,0,0);
  
  this->Minimum = this->Slider->minimum();
  this->Maximum = this->Slider->maximum();
  this->MinValue = this->Slider->minimumValue();
  this->MaxValue = this->Slider->maximumValue();
  this->SingleStep = this->Slider->singleStep();

  this->connectSlider();
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSliderPrivate::connectSlider()
{
  CTK_P(ctkDoubleRangeSlider);
  p->connect(this->Slider, SIGNAL(minimumValueChanged(int)),
             p, SLOT(onMinValueChanged(int)));
  p->connect(this->Slider, SIGNAL(maximumValueChanged(int)),
             p, SLOT(onMaxValueChanged(int)));
  p->connect(this->Slider, SIGNAL(valuesChanged(int,int)),
             p, SLOT(onValuesChanged(int,int)));

  p->connect(this->Slider, SIGNAL(minimumPositionChanged(int)),
             p, SLOT(onMinPosChanged(int)));
  p->connect(this->Slider, SIGNAL(maximumPositionChanged(int)),
             p, SLOT(onMaxPosChanged(int)));
  p->connect(this->Slider, SIGNAL(positionsChanged(int,int)),
             p, SLOT(onPositionsChanged(int,int)));

  p->connect(this->Slider, SIGNAL(sliderPressed()),
             p, SIGNAL(sliderPressed()));
  p->connect(this->Slider, SIGNAL(sliderReleased()),
             p, SIGNAL(sliderReleased()));
  p->connect(this->Slider, SIGNAL(rangeChanged(int, int)),
             p, SLOT(onRangeChanged(int, int)));
}

// --------------------------------------------------------------------------
int ctkDoubleRangeSliderPrivate::toInt(double doubleValue)const
{
  double tmp = doubleValue / this->SingleStep;
  int intValue = qRound(tmp);
  return intValue;
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSliderPrivate::minFromInt(int intValue)const
{
  double doubleValue = this->SingleStep * (this->MinOffset + intValue) ;
  return doubleValue;
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSliderPrivate::maxFromInt(int intValue)const
{
  double doubleValue = this->SingleStep * (this->MaxOffset + intValue) ;
  return doubleValue;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSliderPrivate::updateMinOffset(double value)
{
  this->MinOffset = (value / this->SingleStep) - this->toInt(value);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSliderPrivate::updateMaxOffset(double value)
{
  this->MaxOffset = (value / this->SingleStep) - this->toInt(value);
}

// --------------------------------------------------------------------------
ctkDoubleRangeSlider::ctkDoubleRangeSlider(QWidget* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDoubleRangeSlider);
  ctk_d()->init();
}

// --------------------------------------------------------------------------
ctkDoubleRangeSlider::ctkDoubleRangeSlider(Qt::Orientation _orientation, QWidget* _parent)
  : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDoubleRangeSlider);
  ctk_d()->init();
  this->setOrientation(_orientation);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMinimum(double min)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Minimum = min;
  if (d->Minimum >= d->MinValue)
    {// TBD: use same offset
    d->updateMinOffset(d->Minimum);
    }
  if (d->Minimum >= d->MaxValue)
    {// TBD: use same offset
    d->updateMaxOffset(d->Minimum);
    }
  d->SettingRange = true;
  d->Slider->setMinimum(d->toInt(min));
  d->SettingRange = false;
  emit this->rangeChanged(d->Minimum, d->Maximum);
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::minimum()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->Minimum;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMaximum(double max)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Maximum = max;
  if (d->Maximum <= d->MinValue)
    {// TBD: use same offset
    d->updateMinOffset(d->Maximum);
    }
  if (d->Maximum <= d->MaxValue)
    {// TBD: use same offset ?
    d->updateMaxOffset(d->Maximum);
    }
  d->SettingRange = true;
  d->Slider->setMaximum(d->toInt(max));
  d->SettingRange = false;
  emit this->rangeChanged(d->Minimum, d->Maximum);
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::maximum()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->Maximum;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setRange(double min, double max)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Minimum = min;
  d->Maximum = max;
  if (d->Minimum >= d->MinValue)
    {// TBD: use same offset
    d->updateMinOffset(d->Minimum);
    }
  if (d->Minimum >= d->MaxValue)
    {// TBD: use same offset
    d->updateMaxOffset(d->Minimum);
    }
  if (d->Maximum <= d->MinValue)
    {// TBD: use same offset
    d->updateMinOffset(d->Maximum);
    }
  if (d->Maximum <= d->MaxValue)
    {// TBD: use same offset ?
    d->updateMaxOffset(d->Maximum);
    }
  d->SettingRange = true;
  d->Slider->setRange(d->toInt(min), d->toInt(max));
  d->SettingRange = false;
  emit this->rangeChanged(d->Minimum, d->Maximum);
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::minimumPosition()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->minFromInt(d->Slider->minimumPosition());
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMinimumPosition(double minPos)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Slider->setMinimumPosition(d->toInt(minPos));
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::maximumPosition()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->maxFromInt(d->Slider->maximumPosition());
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMaximumPosition(double maxPos)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Slider->setMaximumPosition(d->toInt(maxPos));
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setPositions(double minPos, double maxPos)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Slider->setPositions(d->toInt(minPos), d->toInt(maxPos));
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::minimumValue()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->MinValue;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMinimumValue(double newMinValue)
{
  CTK_D(ctkDoubleRangeSlider);
  newMinValue = qBound(d->Minimum, newMinValue, d->Maximum);
  d->updateMinOffset(newMinValue);  
  if (newMinValue >= d->MaxValue)
    {
    d->updateMaxOffset(newMinValue);
    }
  int newIntValue = d->toInt(newMinValue);
  if (newIntValue != d->Slider->minimumValue())
    {
    // d->Slider will emit a minimumValueChanged signal that is connected to
    // ctkDoubleSlider::onValueChanged
    d->Slider->setMinimumValue(newIntValue);
    }
  else
    {
    double oldValue = d->MinValue;
    d->MinValue = newMinValue;
    // don't emit a valuechanged signal if the new value is quite 
    // similar to the old value.
    if (qAbs(newMinValue - oldValue) > (d->SingleStep * 0.000000001))
      {
      emit this->minimumValueChanged(newMinValue);
      }
    }
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::maximumValue()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->MaxValue;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMaximumValue(double newMaxValue)
{
  CTK_D(ctkDoubleRangeSlider);
  newMaxValue = qBound(d->Minimum, newMaxValue, d->Maximum);
  d->updateMaxOffset(newMaxValue);
  if (newMaxValue <= d->MinValue)
    {
    d->updateMinOffset(newMaxValue);
    }
  int newIntValue = d->toInt(newMaxValue);
  if (newIntValue != d->Slider->maximumValue())
    {
    // d->Slider will emit a maximumValueChanged signal that is connected to
    // ctkDoubleSlider::onValueChanged
    d->Slider->setMaximumValue(newIntValue);
    }
  else
    {
    double oldValue = d->MaxValue;
    d->MaxValue = newMaxValue;
    // don't emit a valuechanged signal if the new value is quite 
    // similar to the old value.
    if (qAbs(newMaxValue - oldValue) > (d->SingleStep * 0.000000001))
      {
      emit this->maximumValueChanged(newMaxValue);
      }
    }
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setValues(double newMinValue, double newMaxValue)
{
  this->setMinimumValue(qMin(newMinValue, newMaxValue));
  this->setMaximumValue(qMax(newMinValue, newMaxValue));
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::singleStep()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->SingleStep;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setSingleStep(double newStep)
{
  CTK_D(ctkDoubleRangeSlider);
  d->SingleStep = newStep;
  // The following can fire A LOT of signals that shouldn't be 
  // fired.
  bool oldBlockSignals = this->blockSignals(true);
  d->updateMinOffset(d->MinValue);
  d->updateMaxOffset(d->MaxValue);
  // update the new values of the ctkRangeSlider
  double _minvalue = d->MinValue;
  double _maxvalue = d->MaxValue;
  // calling setMinimum or setMaximum can change the values MinimumValue
  // and MaximumValue, this is why we re-set them later.  
  this->setMinimum(d->Minimum);
  this->setMaximum(d->Maximum);
  this->setMinimumValue(_minvalue);
  this->setMinimumPosition(_minvalue);
  this->setMaximumValue(_maxvalue);
  this->setMaximumPosition(_maxvalue);
  this->blockSignals(oldBlockSignals);
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::tickInterval()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->minFromInt(d->Slider->tickInterval());
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setTickInterval(double newTickInterval)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Slider->setTickInterval(d->toInt(newTickInterval));
}

// --------------------------------------------------------------------------
bool ctkDoubleRangeSlider::hasTracking()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->Slider->hasTracking();
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setTracking(bool enable)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Slider->setTracking(enable);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::triggerAction( QAbstractSlider::SliderAction action)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Slider->triggerAction(action);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setOrientation(Qt::Orientation newOrientation)
{
  CTK_D(ctkDoubleRangeSlider);
  d->Slider->setOrientation(newOrientation);
}

// --------------------------------------------------------------------------
Qt::Orientation ctkDoubleRangeSlider::orientation()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->Slider->orientation();
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onMinValueChanged(int newValue)
{
  CTK_D(ctkDoubleRangeSlider);
  double doubleNewValue = d->minFromInt(newValue);
  if (d->MinValue == doubleNewValue)
    {
    return;
    }
  d->MinValue = doubleNewValue;
  emit this->valuesChanged(d->MinValue, d->MaxValue);
  emit this->minimumValueChanged(d->MinValue);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onMaxValueChanged(int newValue)
{
  CTK_D(ctkDoubleRangeSlider);
  double doubleNewValue = d->maxFromInt(newValue);
  if (d->MaxValue == doubleNewValue)
    {
    return;
    }
  d->MaxValue = doubleNewValue;
  emit this->valuesChanged(d->MinValue, d->MaxValue);
  emit this->maximumValueChanged(d->MaxValue);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onValuesChanged(int newMinValue, int newMaxValue)
{
  CTK_D(ctkDoubleRangeSlider);
  double doubleNewMinValue = d->minFromInt(newMinValue);
  double doubleNewMaxValue = d->maxFromInt(newMaxValue);

  bool emitMinValueChanged = (d->MinValue != doubleNewMinValue);
  bool emitMaxValueChanged = (d->MaxValue != doubleNewMaxValue);

  if (!emitMinValueChanged && !emitMaxValueChanged)
    {
    return;
    }
  d->MinValue = doubleNewMinValue;
  d->MaxValue = doubleNewMaxValue;
  emit this->valuesChanged(d->MinValue, d->MaxValue);
  if (emitMinValueChanged)
    {
    emit this->minimumValueChanged(d->MinValue);
    }
  if (emitMaxValueChanged)
    {
    emit this->maximumValueChanged(d->MaxValue);
    }
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onMinPosChanged(int newPosition)
{
  CTK_D(const ctkDoubleRangeSlider);
  emit this->minimumPositionChanged(d->minFromInt(newPosition));
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onMaxPosChanged(int newPosition)
{
  CTK_D(const ctkDoubleRangeSlider);
  emit this->maximumPositionChanged(d->maxFromInt(newPosition));
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onPositionsChanged(int min, int max)
{
  CTK_D(const ctkDoubleRangeSlider);
  emit this->positionsChanged(d->minFromInt(min), d->maxFromInt(max));
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onRangeChanged(int min, int max)
{
  CTK_D(const ctkDoubleRangeSlider);
  if (!d->SettingRange)
    {
    this->setRange(d->minFromInt(min), d->maxFromInt(max));
    }
}

// --------------------------------------------------------------------------
ctkRangeSlider* ctkDoubleRangeSlider::slider()const
{
  CTK_D(const ctkDoubleRangeSlider);
  return d->Slider;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setSlider(ctkRangeSlider* slider)
{
  CTK_D(ctkDoubleRangeSlider);
  slider->setOrientation(d->Slider->orientation());
  slider->setMinimum(d->Slider->minimum());
  slider->setMaximum(d->Slider->maximum());
  slider->setValues(d->Slider->minimumValue(), d->Slider->maximumValue());
  slider->setSingleStep(d->Slider->singleStep());
  slider->setTracking(d->Slider->hasTracking());
  slider->setTickInterval(d->Slider->tickInterval());
  delete d->Slider;
  qobject_cast<QHBoxLayout*>(this->layout())->addWidget(slider);
  d->Slider = slider;
  d->connectSlider();
}
