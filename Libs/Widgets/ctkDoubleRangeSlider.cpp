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
#include <QHBoxLayout>
#include <QPointer>
#include <QVariant>

// CTK includes
#include "ctkRangeSlider.h"
#include "ctkDoubleRangeSlider.h"
#include "ctkValueProxy.h"

// STD includes
#include <limits>

//-----------------------------------------------------------------------------
class ctkDoubleRangeSliderPrivate
{
  Q_DECLARE_PUBLIC(ctkDoubleRangeSlider);
protected:
  ctkDoubleRangeSlider* const q_ptr;
public:
  ctkDoubleRangeSliderPrivate(ctkDoubleRangeSlider& object);
  
  int toInt(double _value)const;
  double minFromInt(int _value)const;
  double maxFromInt(int _value)const;
  double safeMinFromInt(int _value)const;
  double safeMaxFromInt(int _value)const;
  
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

  QPointer<ctkValueProxy> Proxy;

private:
  Q_DISABLE_COPY(ctkDoubleRangeSliderPrivate);
};

// --------------------------------------------------------------------------
ctkDoubleRangeSliderPrivate::ctkDoubleRangeSliderPrivate(ctkDoubleRangeSlider& object)
  :q_ptr(&object)
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
  Q_Q(ctkDoubleRangeSlider);
  this->Slider = new ctkRangeSlider(q);
  QHBoxLayout* l = new QHBoxLayout(q);
  l->addWidget(this->Slider);
  l->setContentsMargins(0,0,0,0);
  
  this->Minimum = this->Slider->minimum();
  this->Maximum = this->Slider->maximum();
  this->MinValue = this->Slider->minimumValue();
  this->MaxValue = this->Slider->maximumValue();
  this->SingleStep = this->Slider->singleStep();

  q->setSizePolicy(this->Slider->sizePolicy());
  q->setAttribute(Qt::WA_WState_OwnSizePolicy, false);

  this->connectSlider();
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSliderPrivate::connectSlider()
{
  Q_Q(ctkDoubleRangeSlider);
  q->connect(this->Slider, SIGNAL(valuesChanged(int,int)),
             q, SLOT(onValuesChanged(int,int)));

  q->connect(this->Slider, SIGNAL(minimumPositionChanged(int)),
             q, SLOT(onMinPosChanged(int)));
  q->connect(this->Slider, SIGNAL(maximumPositionChanged(int)),
             q, SLOT(onMaxPosChanged(int)));
  q->connect(this->Slider, SIGNAL(positionsChanged(int,int)),
             q, SLOT(onPositionsChanged(int,int)));

  q->connect(this->Slider, SIGNAL(sliderPressed()),
             q, SIGNAL(sliderPressed()));
  q->connect(this->Slider, SIGNAL(sliderReleased()),
             q, SIGNAL(sliderReleased()));
  q->connect(this->Slider, SIGNAL(rangeChanged(int,int)),
             q, SLOT(onRangeChanged(int,int)));
}

// --------------------------------------------------------------------------
int ctkDoubleRangeSliderPrivate::toInt(double doubleValue)const
{
  double tmp = doubleValue / this->SingleStep;
  static const double minInt = std::numeric_limits<int>::min();
  static const double maxInt = std::numeric_limits<int>::max();
#ifndef QT_NO_DEBUG
  static const double maxDouble = std::numeric_limits<double>::max();
  if ( (tmp < minInt || tmp > maxInt) &&
       // If the value is the min or max double, there is no need
       // to warn. It is expected that the number is outside of bounds.
       (doubleValue != -maxDouble && doubleValue != maxDouble) )
    {
    qWarning() << __FUNCTION__ << ": value " << doubleValue
               << " for singleStep " << this->SingleStep
               << " is out of integer bounds !";
    }
#endif
  tmp = qBound(minInt, tmp, maxInt);
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
double ctkDoubleRangeSliderPrivate::safeMinFromInt(int intValue)const
{
  return qBound(this->Minimum, this->minFromInt(intValue), this->Maximum);
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSliderPrivate::safeMaxFromInt(int intValue)const
{
  return qBound(this->Minimum, this->maxFromInt(intValue), this->Maximum);
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
  , d_ptr(new ctkDoubleRangeSliderPrivate(*this))
{
  Q_D(ctkDoubleRangeSlider);
  d->init();
}

// --------------------------------------------------------------------------
ctkDoubleRangeSlider::ctkDoubleRangeSlider(Qt::Orientation _orientation, QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkDoubleRangeSliderPrivate(*this))
{
  Q_D(ctkDoubleRangeSlider);
  d->init();
  this->setOrientation(_orientation);
}

// --------------------------------------------------------------------------
ctkDoubleRangeSlider::~ctkDoubleRangeSlider()
{
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMinimum(double newMin)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newMin = d->Proxy.data()->proxyValueFromValue(newMin);
    }
  double oldMin = d->Minimum;
  d->Minimum = newMin;
  if (d->Minimum >= d->MinValue)
    {// TBD: use same offset
    d->updateMinOffset(d->Minimum);
    }
  if (d->Minimum >= d->MaxValue)
    {// TBD: use same offset
    d->updateMaxOffset(d->Minimum);
    }
  bool wasSettingRange = d->SettingRange;
  d->SettingRange = true;
  d->Slider->setMinimum(d->toInt(newMin));
  d->SettingRange = wasSettingRange;
  if (!wasSettingRange && d->Minimum != oldMin)
    {
    emit this->rangeChanged(this->minimum(), this->maximum());
    }
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::minimum()const
{
  Q_D(const ctkDoubleRangeSlider);
  double min = d->Minimum;
  double max = d->Maximum;
  if (d->Proxy)
    {
    min = d->Proxy.data()->valueFromProxyValue(min);
    max = d->Proxy.data()->valueFromProxyValue(max);
    }
  return qMin(min, max);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMaximum(double newMax)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newMax = d->Proxy.data()->proxyValueFromValue(newMax);
    }
  double oldMax = d->Maximum;
  d->Maximum = newMax;
  if (d->Maximum <= d->MinValue)
    {// TBD: use same offset
    d->updateMinOffset(d->Maximum);
    }
  if (d->Maximum <= d->MaxValue)
    {// TBD: use same offset ?
    d->updateMaxOffset(d->Maximum);
    }
  bool wasSettingRange = d->SettingRange;
  d->SettingRange = true;
  d->Slider->setMaximum(d->toInt(newMax));
  d->SettingRange = wasSettingRange;
  if (!wasSettingRange && d->Maximum != oldMax)
    {
    emit this->rangeChanged(this->minimum(), this->maximum());
    }
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::maximum()const
{
  Q_D(const ctkDoubleRangeSlider);
  double min = d->Minimum;
  double max = d->Maximum;
  if (d->Proxy)
    {
    min = d->Proxy.data()->valueFromProxyValue(min);
    max = d->Proxy.data()->valueFromProxyValue(max);
    }
  return qMax(min, max);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setRange(double newMin, double newMax)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newMin = d->Proxy.data()->proxyValueFromValue(newMin);
    newMax = d->Proxy.data()->proxyValueFromValue(newMax);
    }

  if (newMin > newMax)
    {
    qSwap(newMin, newMax);
    }

  double oldMin = d->Minimum;
  double oldMax = d->Maximum;
  d->Minimum = newMin;
  d->Maximum = newMax;
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
  bool wasSettingRange = d->SettingRange;
  d->SettingRange = true;
  d->Slider->setRange(d->toInt(newMin), d->toInt(newMax));
  d->SettingRange = wasSettingRange;
  if (!wasSettingRange && (d->Minimum != oldMin || d->Maximum != oldMax))
    {
    emit this->rangeChanged(this->minimum(), this->maximum());
    }
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::minimumPosition()const
{
  Q_D(const ctkDoubleRangeSlider);
  int intMinPos = d->Slider->minimumPosition();
  double minPos = d->safeMinFromInt(intMinPos);
  if (d->Proxy)
    {
    minPos = d->Proxy.data()->valueFromProxyValue(minPos);
    }
  return minPos;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMinimumPosition(double newMinPos)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newMinPos = d->Proxy.data()->proxyValueFromValue(newMinPos);
    }
  int newIntMinPos = d->toInt(newMinPos);
  d->Slider->setMinimumPosition(newIntMinPos);
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::maximumPosition()const
{
  Q_D(const ctkDoubleRangeSlider);
  int intMaxPos = d->Slider->maximumPosition();
  double maxPos = d->safeMaxFromInt(intMaxPos);
  if (d->Proxy)
    {
    maxPos = d->Proxy.data()->valueFromProxyValue(maxPos);
    }
  return maxPos;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMaximumPosition(double newMaxPos)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newMaxPos = d->Proxy.data()->proxyValueFromValue(newMaxPos);
    }
  int newIntMaxPos = d->toInt(newMaxPos);
  d->Slider->setMaximumPosition(newIntMaxPos);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setPositions(double newMinPos, double newMaxPos)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newMinPos = d->Proxy.data()->proxyValueFromValue(newMinPos);
    newMaxPos = d->Proxy.data()->proxyValueFromValue(newMaxPos);
    }
  int newIntMinPos = d->toInt(newMinPos);
  int newIntMaxPos = d->toInt(newMaxPos);
  d->Slider->setPositions(newIntMinPos, newIntMaxPos);
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::minimumValue()const
{
  Q_D(const ctkDoubleRangeSlider);
  double minValue = d->MinValue;
  double maxValue = d->MaxValue;
  if (d->Proxy)
    {
    minValue = d->Proxy.data()->valueFromProxyValue(minValue);
    maxValue = d->Proxy.data()->valueFromProxyValue(maxValue);
    }
  return qMin(minValue, maxValue);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMinimumValue(double newMinValue)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newMinValue = d->Proxy.data()->proxyValueFromValue(newMinValue);
    }
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
      emit this->valuesChanged(this->minimumValue(), this->maximumValue());
      emit this->minimumValueChanged(this->minimumValue());
      }
    }
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::maximumValue()const
{
  Q_D(const ctkDoubleRangeSlider);
  double minValue = d->MinValue;
  double maxValue = d->MaxValue;
  if (d->Proxy)
    {
    minValue = d->Proxy.data()->valueFromProxyValue(minValue);
    maxValue = d->Proxy.data()->valueFromProxyValue(maxValue);
    }
  return qMax(minValue, maxValue);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setMaximumValue(double newMaxValue)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newMaxValue = d->Proxy.data()->proxyValueFromValue(newMaxValue);
    }
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
      emit this->valuesChanged(this->minimumValue(), this->maximumValue());
      emit this->maximumValueChanged(this->maximumValue());
      }
    }
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setValues(double newMinVal, double newMaxVal)
{
  Q_D(ctkDoubleRangeSlider);
  // We can't call setMinimumValue() and setMaximumValue() as they would
  // generate an inconsistent state. when minimumValueChanged() is fired the
  // new max value wouldn't be updated yet.
  if (d->Proxy)
    {
    newMaxVal = d->Proxy.data()->proxyValueFromValue(newMaxVal);
    newMinVal = d->Proxy.data()->proxyValueFromValue(newMinVal);
    }
  double newMinValue = qBound(d->Minimum, qMin(newMinVal, newMaxVal), d->Maximum);
  double newMaxValue = qBound(d->Minimum, qMax(newMinVal, newMaxVal), d->Maximum);
  d->updateMinOffset(newMinValue);
  d->updateMaxOffset(newMaxValue);
  int newMinIntValue = d->toInt(newMinValue);
  int newMaxIntValue = d->toInt(newMaxValue);
  if (newMinIntValue != d->Slider->minimumValue() ||
      newMaxIntValue != d->Slider->maximumValue())
    {
    // d->Slider will emit a maximumValueChanged signal that is connected to
    // ctkDoubleSlider::onValueChanged
    d->Slider->setValues(newMinIntValue, newMaxIntValue);
    }
  else
    {
    double oldMinValue = d->MinValue;
    double oldMaxValue = d->MaxValue;
    d->MinValue = newMinValue;
    d->MaxValue = newMaxValue;
    // don't emit a valuechanged signal if the new value is quite
    // similar to the old value.
    bool minChanged = qAbs(newMinValue - oldMinValue) > (d->SingleStep * 0.000000001);
    bool maxChanged = qAbs(newMaxValue - oldMaxValue) > (d->SingleStep * 0.000000001);

    if (minChanged || maxChanged)
      {
      emit this->valuesChanged(this->minimumValue(), this->maximumValue());
      if (minChanged)
        {
        emit this->minimumValueChanged(this->minimumValue());
        }
      if (maxChanged)
        {
        emit this->maximumValueChanged(this->maximumValue());
        }
      }
    }
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::singleStep()const
{
  Q_D(const ctkDoubleRangeSlider);
  double step = d->SingleStep;
  return step;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setSingleStep(double newStep)
{
  Q_D(ctkDoubleRangeSlider);
  if (!this->isValidStep(newStep))
    {
    qWarning() << "ctkDoubleRangeSlider::setSingleStep("<< newStep <<")"
               << "is outside of valid bounds.";
    return;
    }
  d->SingleStep = newStep;
  // The following can fire A LOT of signals that shouldn't be fired.
  bool oldBlockSignals = this->blockSignals(true);
  d->updateMinOffset(d->MinValue);
  d->updateMaxOffset(d->MaxValue);
  double minimum = d->Minimum;
  double maximum = d->Maximum;
  // update the new int values
  double minValue = d->MinValue;
  double maxValue = d->MaxValue;
  if (d->Proxy)
    {
    minimum = d->Proxy.data()->valueFromProxyValue(minimum);
    maximum = d->Proxy.data()->valueFromProxyValue(maximum);
    minValue = d->Proxy.data()->valueFromProxyValue(minValue);
    maxValue = d->Proxy.data()->valueFromProxyValue(maxValue);
   }
  // calling setRange can change the MinimumValue and MaximumValue values,
  // this is why we re-set them after.
  this->setRange(minimum, maximum);
  this->setValues(minValue, maxValue);
  this->setPositions(minValue, maxValue);
  this->blockSignals(oldBlockSignals);
}

// --------------------------------------------------------------------------
bool ctkDoubleRangeSlider::isValidStep(double step)const
{
  Q_D(const ctkDoubleRangeSlider);
  if (d->Minimum == d->Maximum)
    {
    return true;
    }
  const double minStep = qMax(d->Maximum / std::numeric_limits<double>::max(),
                              std::numeric_limits<double>::epsilon());
  const double maxStep = qMin(d->Maximum - d->Minimum,
                              static_cast<double>(std::numeric_limits<int>::max()));
  return (step >= minStep) && (step <= maxStep);
}

// --------------------------------------------------------------------------
double ctkDoubleRangeSlider::tickInterval()const
{
  Q_D(const ctkDoubleRangeSlider);
  double interval = d->SingleStep * d->Slider->tickInterval();
  if (d->Proxy)
    {
    interval = d->Proxy.data()->valueFromProxyValue(interval);
    }
  return interval;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setTickInterval(double newInterval)
{
  Q_D(ctkDoubleRangeSlider);
  if (d->Proxy)
    {
    newInterval = d->Proxy.data()->proxyValueFromValue(newInterval);
    }
  int newIntInterval = d->toInt(newInterval);
  d->Slider->setTickInterval(newIntInterval);
}

// --------------------------------------------------------------------------
QSlider::TickPosition ctkDoubleRangeSlider::tickPosition()const
{
  Q_D(const ctkDoubleRangeSlider);
  return d->Slider->tickPosition();
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setTickPosition(QSlider::TickPosition newTickPosition)
{
  Q_D(ctkDoubleRangeSlider);
  d->Slider->setTickPosition(newTickPosition);
}

// --------------------------------------------------------------------------
bool ctkDoubleRangeSlider::hasTracking()const
{
  Q_D(const ctkDoubleRangeSlider);
  return d->Slider->hasTracking();
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setTracking(bool enable)
{
  Q_D(ctkDoubleRangeSlider);
  d->Slider->setTracking(enable);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::triggerAction( QAbstractSlider::SliderAction action)
{
  Q_D(ctkDoubleRangeSlider);
  d->Slider->triggerAction(action);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setOrientation(Qt::Orientation newOrientation)
{
  Q_D(ctkDoubleRangeSlider);
  if (this->orientation() == newOrientation)
    {
    return;
    }
  if (!testAttribute(Qt::WA_WState_OwnSizePolicy))
    {
    QSizePolicy sp = this->sizePolicy();
    sp.transpose();
    this->setSizePolicy(sp);
    this->setAttribute(Qt::WA_WState_OwnSizePolicy, false);
    }
  // d->Slider will take care of calling updateGeometry
  d->Slider->setOrientation(newOrientation);
}

// --------------------------------------------------------------------------
Qt::Orientation ctkDoubleRangeSlider::orientation()const
{
  Q_D(const ctkDoubleRangeSlider);
  return d->Slider->orientation();
}

// --------------------------------------------------------------------------
bool ctkDoubleRangeSlider::symmetricMoves()const
{
  Q_D(const ctkDoubleRangeSlider);
  return d->Slider->symmetricMoves();
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setSymmetricMoves(bool symmetry)
{
  Q_D(ctkDoubleRangeSlider);
  d->Slider->setSymmetricMoves(symmetry);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onValuesChanged(int newMinValue, int newMaxValue)
{
  Q_D(ctkDoubleRangeSlider);
  double doubleNewMinValue = d->safeMinFromInt(newMinValue);
  double doubleNewMaxValue = d->safeMaxFromInt(newMaxValue);

  bool emitMinValueChanged = (d->MinValue != doubleNewMinValue);
  bool emitMaxValueChanged = (d->MaxValue != doubleNewMaxValue);

  if (!emitMinValueChanged && !emitMaxValueChanged)
    {
    return;
    }
  d->MinValue = doubleNewMinValue;
  d->MaxValue = doubleNewMaxValue;
  emit this->valuesChanged(this->minimumValue(), this->maximumValue());
  if (emitMinValueChanged)
    {
    emit this->minimumValueChanged(this->minimumValue());
    }
  if (emitMaxValueChanged)
    {
    emit this->maximumValueChanged(this->maximumValue());
    }
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onMinPosChanged(int newIntMinPos)
{
  Q_D(const ctkDoubleRangeSlider);
  double newMinPos = d->safeMinFromInt(newIntMinPos);
  if (d->Proxy)
    {
    newMinPos = d->Proxy.data()->valueFromProxyValue(newMinPos);
    }
  emit this->minimumPositionChanged(newMinPos);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onMaxPosChanged(int newIntMaxPos)
{
  Q_D(const ctkDoubleRangeSlider);
  double newMaxPos = d->safeMaxFromInt(newIntMaxPos);
  if (d->Proxy)
    {
    //newMaxPos = d->Proxy.data()->valueFromProxyValue(newMaxPos);
    }
  emit this->maximumPositionChanged(newMaxPos);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onPositionsChanged(int newIntMinPos, int newIntMaxPos)
{
  Q_D(const ctkDoubleRangeSlider);
  double newMinPos = d->safeMinFromInt(newIntMinPos);
  double newMaxPos = d->safeMaxFromInt(newIntMaxPos);
  if (d->Proxy)
    {
    newMinPos = d->Proxy.data()->valueFromProxyValue(newMinPos);
    newMaxPos = d->Proxy.data()->valueFromProxyValue(newMaxPos);
    }
  emit this->positionsChanged(newMinPos, newMaxPos);
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::onRangeChanged(int newIntMin, int newIntMax)
{
  Q_D(const ctkDoubleRangeSlider);
  if (d->SettingRange)
    {
    return;
    }
  double newMin = d->minFromInt(newIntMin);
  double newMax = d->maxFromInt(newIntMax);
  if (d->Proxy)
    {
    newMin = d->Proxy.data()->valueFromProxyValue(newMin);
    newMax = d->Proxy.data()->valueFromProxyValue(newMax);
    }
  this->setRange(newMin, newMax);
}

// --------------------------------------------------------------------------
ctkRangeSlider* ctkDoubleRangeSlider::slider()const
{
  Q_D(const ctkDoubleRangeSlider);
  return d->Slider;
}

// --------------------------------------------------------------------------
void ctkDoubleRangeSlider::setSlider(ctkRangeSlider* newslider)
{
  Q_D(ctkDoubleRangeSlider);
  newslider->setOrientation(d->Slider->orientation());
  newslider->setMinimum(d->Slider->minimum());
  newslider->setMaximum(d->Slider->maximum());
  newslider->setValues(d->Slider->minimumValue(), d->Slider->maximumValue());
  newslider->setSingleStep(d->Slider->singleStep());
  newslider->setTracking(d->Slider->hasTracking());
  newslider->setTickInterval(d->Slider->tickInterval());
  newslider->setTickPosition(d->Slider->tickPosition());
  delete d->Slider;
  qobject_cast<QHBoxLayout*>(this->layout())->addWidget(newslider);
  d->Slider = newslider;
  d->connectSlider();
}

//----------------------------------------------------------------------------
void ctkDoubleRangeSlider::setValueProxy(ctkValueProxy* proxy)
{
  Q_D(ctkDoubleRangeSlider);
  if (proxy == d->Proxy.data())
    {
    return;
    }

  this->onValueProxyAboutToBeModified();

  if (d->Proxy.data())
    {
    disconnect(d->Proxy.data(), 0, this, 0);
    }

  d->Proxy = proxy;

  if (d->Proxy)
    {
    connect(d->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
            this, SLOT(onValueProxyAboutToBeModified()));
    connect(d->Proxy.data(), SIGNAL(proxyModified()),
            this, SLOT(onValueProxyModified()));
    }

  this->onValueProxyModified();
}

//----------------------------------------------------------------------------
ctkValueProxy* ctkDoubleRangeSlider::valueProxy() const
{
  Q_D(const ctkDoubleRangeSlider);
  return d->Proxy.data();
}

//-----------------------------------------------------------------------------
void ctkDoubleRangeSlider::onValueProxyAboutToBeModified()
{
  Q_D(ctkDoubleRangeSlider);
  d->Slider->setProperty("inputMinimumValue", this->minimumValue());
  d->Slider->setProperty("inputMaximumValue", this->maximumValue());
  d->Slider->setProperty("inputMinimum", this->minimum());
  d->Slider->setProperty("inputMaximum", this->maximum());
}

//-----------------------------------------------------------------------------
void ctkDoubleRangeSlider::onValueProxyModified()
{
  Q_D(ctkDoubleRangeSlider);
  bool wasBlockingSignals = this->blockSignals(true);
  bool wasSettingRange = d->SettingRange;
  d->SettingRange = true;
  this->setRange(d->Slider->property("inputMinimum").toDouble(),
                 d->Slider->property("inputMaximum").toDouble());
  d->SettingRange = wasSettingRange;
  this->setValues(d->Slider->property("inputMinimumValue").toDouble(),
                  d->Slider->property("inputMaximumValue").toDouble());
  this->blockSignals(wasBlockingSignals);
}
