/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

// QT includes
#include <QDebug>
#include <QHBoxLayout>

// CTK includes
#include "ctkDoubleSlider.h"

// STD includes
#include <limits>

//-----------------------------------------------------------------------------
class ctkDoubleSliderPrivate
{
  Q_DECLARE_PUBLIC(ctkDoubleSlider);
protected:
  ctkDoubleSlider* const q_ptr;
public:
  ctkDoubleSliderPrivate(ctkDoubleSlider& object);
  int toInt(double _value)const;
  double fromInt(int _value)const;
  void init();
  void updateOffset(double value);

  QSlider*    Slider;
  double      Minimum;
  double      Maximum;
  bool        SettingRange;
  // we should have a Offset and SliderPositionOffset (and MinimumOffset?)
  double      Offset;
  double      SingleStep;
  double      Value;
};

// --------------------------------------------------------------------------
ctkDoubleSliderPrivate::ctkDoubleSliderPrivate(ctkDoubleSlider& object)
  :q_ptr(&object)
{
  this->Slider = 0;
  this->Minimum = 0.;
  this->Maximum = 100.;
  this->SettingRange = false;
  this->Offset = 0.;
  this->SingleStep = 1.;
  this->Value = 0.;
}

// --------------------------------------------------------------------------
void ctkDoubleSliderPrivate::init()
{
  Q_Q(ctkDoubleSlider);
  this->Slider = new QSlider(q);
  QHBoxLayout* l = new QHBoxLayout(q);
  l->addWidget(this->Slider);
  l->setContentsMargins(0,0,0,0);
  
  this->Minimum = this->Slider->minimum();
  this->Maximum = this->Slider->maximum();
  this->SingleStep = this->Slider->singleStep();
  this->Value = this->Slider->value();

  q->connect(this->Slider, SIGNAL(valueChanged(int)), q, SLOT(onValueChanged(int)));
  q->connect(this->Slider, SIGNAL(sliderMoved(int)), q, SLOT(onSliderMoved(int)));
  q->connect(this->Slider, SIGNAL(sliderPressed()), q, SIGNAL(sliderPressed()));
  q->connect(this->Slider, SIGNAL(sliderReleased()), q, SIGNAL(sliderReleased()));
  q->connect(this->Slider, SIGNAL(rangeChanged(int, int)),
             q, SLOT(onRangeChanged(int, int)));

  q->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed,
                               QSizePolicy::Slider));
}
  
// --------------------------------------------------------------------------
int ctkDoubleSliderPrivate::toInt(double doubleValue)const
{
  double tmp = doubleValue / this->SingleStep;
  static const double minInt = std::numeric_limits<int>::min();
  static const double maxInt = std::numeric_limits<int>::max();
#ifndef QT_NO_DEBUG
  if (tmp < minInt || tmp > maxInt)
    {
    qWarning("ctkDoubleSliderPrivate::toInt value out of bounds !");
    }
#endif
  tmp = qBound(minInt, tmp, maxInt);
  int intValue = qRound(tmp);
  //qDebug() << __FUNCTION__ << doubleValue << tmp << intValue;
  return intValue;
}

// --------------------------------------------------------------------------
double ctkDoubleSliderPrivate::fromInt(int intValue)const
{
  double doubleValue = this->SingleStep * (this->Offset + intValue) ;
  //qDebug() << __FUNCTION__ << intValue << doubleValue;
  return doubleValue;
}

// --------------------------------------------------------------------------
void ctkDoubleSliderPrivate::updateOffset(double value)
{
  this->Offset = (value / this->SingleStep) - this->toInt(value);
}

// --------------------------------------------------------------------------
ctkDoubleSlider::ctkDoubleSlider(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkDoubleSliderPrivate(*this))
{
  Q_D(ctkDoubleSlider);
  d->init();
}

// --------------------------------------------------------------------------
ctkDoubleSlider::ctkDoubleSlider(Qt::Orientation _orientation, QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkDoubleSliderPrivate(*this))
{
  Q_D(ctkDoubleSlider);
  d->init();
  this->setOrientation(_orientation);
}

// --------------------------------------------------------------------------
ctkDoubleSlider::~ctkDoubleSlider()
{
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setMinimum(double min)
{
  Q_D(ctkDoubleSlider);
  d->Minimum = min;
  if (d->Minimum >= d->Value)
    {
    d->updateOffset(d->Minimum);
    }
  d->SettingRange = true;
  d->Slider->setMinimum(d->toInt(min));
  d->SettingRange = false;
  emit this->rangeChanged(d->Minimum, d->Maximum);
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setMaximum(double max)
{
  Q_D(ctkDoubleSlider);
  d->Maximum = max;
  if (d->Maximum <= d->Value)
    {
    d->updateOffset(d->Maximum);
    }
  d->SettingRange = true;
  d->Slider->setMaximum(d->toInt(max));
  d->SettingRange = false;
  emit this->rangeChanged(d->Minimum, d->Maximum);
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setRange(double min, double max)
{
  Q_D(ctkDoubleSlider);
  d->Minimum = min;
  d->Maximum = max;
  
  if (d->Minimum >= d->Value)
    {
    d->updateOffset(d->Minimum);
    }
  if (d->Maximum <= d->Value)
    {
    d->updateOffset(d->Maximum);
    }
  d->SettingRange = true;
  d->Slider->setRange(d->toInt(min), d->toInt(max));
  d->SettingRange = false;
  emit this->rangeChanged(d->Minimum, d->Maximum);
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::minimum()const
{
  Q_D(const ctkDoubleSlider);
  return d->Minimum;
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::maximum()const
{
  Q_D(const ctkDoubleSlider);
  return d->Maximum;
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::sliderPosition()const
{
  Q_D(const ctkDoubleSlider);
  return d->fromInt(d->Slider->sliderPosition());
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setSliderPosition(double newSliderPosition)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setSliderPosition(d->toInt(newSliderPosition));
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::value()const
{
  Q_D(const ctkDoubleSlider);
  return d->Value;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setValue(double newValue)
{
  Q_D(ctkDoubleSlider);
  newValue = qBound(d->Minimum, newValue, d->Maximum);
  d->updateOffset(newValue);
  int newIntValue = d->toInt(newValue);
  if (newIntValue != d->Slider->value())
    {
    // d->Slider will emit a valueChanged signal that is connected to
    // ctkDoubleSlider::onValueChanged
    d->Slider->setValue(newIntValue);
    }
  else
    {
    double oldValue = d->Value;
    d->Value = newValue;
    // don't emit a valuechanged signal if the new value is quite 
    // similar to the old value.
    if (qAbs(newValue - oldValue) > (d->SingleStep * 0.000000001))
      {
      emit this->valueChanged(newValue);
      }
    }
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::singleStep()const
{
  Q_D(const ctkDoubleSlider);
  return d->SingleStep;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setSingleStep(double newStep)
{
  Q_D(ctkDoubleSlider);
  d->SingleStep = newStep;
  // update the new values of the QSlider
  double _value = d->Value;
  d->updateOffset(_value);
  bool oldBlockSignals = this->blockSignals(true);
  this->setRange(d->Minimum, d->Maximum);
  d->Slider->setValue(d->toInt(_value));
  d->Value = _value;
  this->blockSignals(oldBlockSignals);
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::tickInterval()const
{
  Q_D(const ctkDoubleSlider);
  return d->fromInt(d->Slider->tickInterval());
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setTickInterval(double newTickInterval)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setTickInterval(d->toInt(newTickInterval));
}

// --------------------------------------------------------------------------
bool ctkDoubleSlider::hasTracking()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->hasTracking();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setTracking(bool enable)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setTracking(enable);
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::triggerAction( QAbstractSlider::SliderAction action)
{
  Q_D(ctkDoubleSlider);
  d->Slider->triggerAction(action);
}

// --------------------------------------------------------------------------
Qt::Orientation ctkDoubleSlider::orientation()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->orientation();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setOrientation(Qt::Orientation newOrientation)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setOrientation(newOrientation);
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onValueChanged(int newValue)
{
  Q_D(ctkDoubleSlider);
  double doubleNewValue = d->fromInt(newValue);
/*
  qDebug() << "onValueChanged: " << newValue << "->"<< d->fromInt(newValue+d->Offset) 
           << " old: " << d->Value << "->" << d->toInt(d->Value) 
           << "offset:" << d->Offset << doubleNewValue;
*/
  if (d->Value == doubleNewValue)
    {
    return;
    }
  d->Value = doubleNewValue;
  emit this->valueChanged(d->Value);
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onSliderMoved(int newPosition)
{
  Q_D(const ctkDoubleSlider);
  emit this->sliderMoved(d->fromInt(newPosition));
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onRangeChanged(int min, int max)
{
  Q_D(const ctkDoubleSlider);
  if (!d->SettingRange)
    {
    this->setRange(d->fromInt(min), d->fromInt(max));
    }
}


