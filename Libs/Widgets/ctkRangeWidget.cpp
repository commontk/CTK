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
#include <QMouseEvent>
#include <QPointer>

// CTK includes
#include "ctkRangeWidget.h"
#include "ctkValueProxy.h"
#include "ui_ctkRangeWidget.h"

// STD includes
#include <cmath>
#include <limits>

//-----------------------------------------------------------------------------
class ctkRangeWidgetPrivate: public Ui_ctkRangeWidget
{
  Q_DECLARE_PUBLIC(ctkRangeWidget);
protected:
  ctkRangeWidget* const q_ptr;
public:
  ctkRangeWidgetPrivate(ctkRangeWidget& object);
  void connectSlider();

  void updateSpinBoxWidth();
  int synchronizedSpinBoxWidth()const;
  void synchronizeSiblingSpinBox(int newWidth);
  bool equal(double v1, double v2)const;
  void relayout();
  bool useCustomSpinBoxesLimits()const;

  bool          Tracking;
  bool          Changing;
  bool          SettingSliderRange;
  bool          BlockSliderUpdate;
  double        MinimumValueBeforeChange;
  double        MaximumValueBeforeChange;
  bool          AutoSpinBoxWidth;
  double        CustomSpinBoxesLimitsMin;
  double        CustomSpinBoxesLimitsMax;
  Qt::Alignment SpinBoxAlignment;
  QPointer<ctkValueProxy> Proxy;
};

// --------------------------------------------------------------------------
bool ctkRangeWidgetPrivate::equal(double v1, double v2)const
{
  if (v1 == v2)
    {// don't bother computing difference as it could fail for infinity numbers
    return true;
    }
  if (v1 != v1 && v2 != v2)
    {// NaN check
    return true;
    }
  return qAbs(v1 - v2) < pow(10., -this->MinimumSpinBox->decimals());
}

// --------------------------------------------------------------------------
ctkRangeWidgetPrivate::ctkRangeWidgetPrivate(ctkRangeWidget& object)
  :q_ptr(&object)
{
  this->Tracking = true;
  this->Changing = false;
  this->SettingSliderRange = false;
  this->BlockSliderUpdate = false;
  this->MinimumValueBeforeChange = 0.;
  this->MaximumValueBeforeChange = 0.;
  this->AutoSpinBoxWidth = true;
  this->CustomSpinBoxesLimitsMin = std::numeric_limits<double>::max();
  this->CustomSpinBoxesLimitsMax = std::numeric_limits<double>::min();
  this->SpinBoxAlignment = Qt::AlignVCenter;
}

// --------------------------------------------------------------------------
void ctkRangeWidgetPrivate::connectSlider()
{
  Q_Q(ctkRangeWidget);
  QObject::connect(this->Slider, SIGNAL(valuesChanged(double,double)),
                   q, SLOT(changeValues(double,double)));
  QObject::connect(this->Slider, SIGNAL(minimumValueChanged(double)),
                   q, SLOT(changeMinimumValue(double)));
  QObject::connect(this->Slider, SIGNAL(maximumValueChanged(double)),
                   q, SLOT(changeMaximumValue(double)));

  QObject::connect(this->MinimumSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(setSliderValues()));
  QObject::connect(this->MaximumSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(setSliderValues()));
  QObject::connect(this->MinimumSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(setMinimumToMaximumSpinBox(double)));
  QObject::connect(this->MaximumSpinBox, SIGNAL(valueChanged(double)),
                   q, SLOT(setMaximumToMinimumSpinBox(double)));
  QObject::connect(this->MinimumSpinBox, SIGNAL(decimalsChanged(int)),
                   q, SLOT(setDecimals(int)));
  QObject::connect(this->MaximumSpinBox, SIGNAL(decimalsChanged(int)),
                   q, SLOT(setDecimals(int)));

  QObject::connect(this->Slider, SIGNAL(sliderPressed()),
                   q, SLOT(startChanging()));
  QObject::connect(this->Slider, SIGNAL(sliderReleased()),
                   q, SLOT(stopChanging()));
  QObject::connect(this->Slider, SIGNAL(rangeChanged(double,double)),
                   q, SLOT(onSliderRangeChanged(double,double)));
}

// --------------------------------------------------------------------------
void ctkRangeWidgetPrivate::updateSpinBoxWidth()
{
  int spinBoxWidth = this->synchronizedSpinBoxWidth();
  if (this->AutoSpinBoxWidth)
    {
    this->MinimumSpinBox->setMinimumWidth(spinBoxWidth);
    this->MaximumSpinBox->setMinimumWidth(spinBoxWidth);
    }
  else
    {
    this->MinimumSpinBox->setMinimumWidth(0);
    this->MaximumSpinBox->setMinimumWidth(0);
    }
  this->synchronizeSiblingSpinBox(spinBoxWidth);
}

// --------------------------------------------------------------------------
int ctkRangeWidgetPrivate::synchronizedSpinBoxWidth()const
{
  Q_Q(const ctkRangeWidget);
  //Q_ASSERT(this->MinimumSpinBox->sizeHint() == this->MaximumSpinBox->sizeHint());
  int maxWidth = qMax(this->MinimumSpinBox->sizeHint().width(),
                      this->MaximumSpinBox->sizeHint().width());
  if (!q->parent())
    {
    return maxWidth;
    }
  QList<ctkRangeWidget*> siblings =
    q->parent()->findChildren<ctkRangeWidget*>();
  foreach(ctkRangeWidget* sibling, siblings)
    {
    maxWidth = qMax(maxWidth, qMax(sibling->d_func()->MaximumSpinBox->sizeHint().width(),
                                   sibling->d_func()->MaximumSpinBox->sizeHint().width()));
    }
  return maxWidth;
}

// --------------------------------------------------------------------------
void ctkRangeWidgetPrivate::synchronizeSiblingSpinBox(int width)
{
  Q_Q(const ctkRangeWidget);
  QList<ctkRangeWidget*> siblings =
    q->parent()->findChildren<ctkRangeWidget*>();
  foreach(ctkRangeWidget* sibling, siblings)
    {
    if (sibling != q && sibling->isAutoSpinBoxWidth())
      {
      sibling->d_func()->MinimumSpinBox->setMinimumWidth(width);
      sibling->d_func()->MaximumSpinBox->setMinimumWidth(width);
      }
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidgetPrivate::relayout()
{
  this->GridLayout->removeWidget(this->MinimumSpinBox);
  this->GridLayout->removeWidget(this->MaximumSpinBox);
  this->GridLayout->removeWidget(this->Slider);
  if (this->SpinBoxAlignment & Qt::AlignTop)
    {
    this->GridLayout->addWidget(this->MinimumSpinBox,0,0);
    this->GridLayout->addWidget(this->MaximumSpinBox,0,2);
    this->GridLayout->addWidget(this->Slider,1,0,1,3);
    }
  else if (this->SpinBoxAlignment & Qt::AlignBottom)
    {
    this->GridLayout->addWidget(this->MinimumSpinBox,1,0);
    this->GridLayout->addWidget(this->MaximumSpinBox,1,2);
    this->GridLayout->addWidget(this->Slider,0, 0, 1, 3);
    }
  else if (this->SpinBoxAlignment & Qt::AlignRight)
    {
    this->GridLayout->addWidget(this->Slider, 0, 0);
    this->GridLayout->addWidget(this->MinimumSpinBox,0,1);
    this->GridLayout->addWidget(this->MaximumSpinBox,0,2);
    }
  else if (this->SpinBoxAlignment & Qt::AlignLeft)
    {
    this->GridLayout->addWidget(this->MinimumSpinBox,0,0);
    this->GridLayout->addWidget(this->MaximumSpinBox,0,1);
    this->GridLayout->addWidget(this->Slider, 0, 2);
    }
  else // Qt::AlignVCenter (or any other bad alignment)
    {
    this->GridLayout->addWidget(this->MinimumSpinBox,0,0);
    this->GridLayout->addWidget(this->Slider,0,1);
    this->GridLayout->addWidget(this->MaximumSpinBox,0,2);
    }
}

// --------------------------------------------------------------------------
bool ctkRangeWidgetPrivate::useCustomSpinBoxesLimits()const
{
  return this->CustomSpinBoxesLimitsMin <= this->CustomSpinBoxesLimitsMax;
}

// --------------------------------------------------------------------------
ctkRangeWidget::ctkRangeWidget(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkRangeWidgetPrivate(*this))
{
  Q_D(ctkRangeWidget);
  
  d->setupUi(this);

  if (d->useCustomSpinBoxesLimits())
    {
    d->MinimumSpinBox->setRange(d->CustomSpinBoxesLimitsMin, d->Slider->maximum());
    d->MaximumSpinBox->setRange(d->Slider->minimum(), d->CustomSpinBoxesLimitsMax);
    }
  else
    {
    d->MinimumSpinBox->setRange(d->Slider->minimum(), d->Slider->maximum());
    d->MaximumSpinBox->setRange(d->Slider->minimum(), d->Slider->maximum());
    }
  d->MinimumSpinBox->setValue(d->Slider->minimumValue());
  d->MaximumSpinBox->setValue(d->Slider->maximumValue());
  
  d->connectSlider();

  d->MinimumSpinBox->installEventFilter(this);
  d->MaximumSpinBox->installEventFilter(this);
}

// --------------------------------------------------------------------------
ctkRangeWidget::~ctkRangeWidget()
{
}

// --------------------------------------------------------------------------
double ctkRangeWidget::minimum()const
{
  Q_D(const ctkRangeWidget);
  if (!d->useCustomSpinBoxesLimits())
    {
    Q_ASSERT(d->equal(d->MinimumSpinBox->minimum(),d->Slider->minimum()));
    }
  return d->Slider->minimum();
}

// --------------------------------------------------------------------------
double ctkRangeWidget::maximum()const
{
  Q_D(const ctkRangeWidget);
  if (!d->useCustomSpinBoxesLimits())
    {
    Q_ASSERT(d->equal(d->MaximumSpinBox->maximum(), d->Slider->maximum()));
    }
  return d->Slider->maximum();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMinimum(double min)
{
  Q_D(ctkRangeWidget);
  bool blocked = d->MinimumSpinBox->blockSignals(true);
  blocked = d->MaximumSpinBox->blockSignals(true);
  if (d->useCustomSpinBoxesLimits())
    {
    d->MinimumSpinBox->setMinimum(d->CustomSpinBoxesLimitsMin);
    }
  else
    {
    d->MinimumSpinBox->setMinimum(min);
    }
  d->MaximumSpinBox->setMinimum(min);
  d->MinimumSpinBox->blockSignals(blocked);
  d->MaximumSpinBox->blockSignals(blocked);
  // SpinBox can truncate min (depending on decimals).
  // use Spinbox's min to set Slider's min
  d->SettingSliderRange = true;
  d->Slider->setMinimum(d->MaximumSpinBox->minimum());
  d->SettingSliderRange = false;
  if (!d->useCustomSpinBoxesLimits())
    {
    Q_ASSERT(d->equal(d->MinimumSpinBox->minimum(),d->Slider->minimum()));
    }
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMaximum(double max)
{
  Q_D(ctkRangeWidget);
  bool blocked = d->MinimumSpinBox->blockSignals(true);
  blocked = d->MaximumSpinBox->blockSignals(true);
  if (d->useCustomSpinBoxesLimits())
    {
    d->MaximumSpinBox->setMaximum(d->CustomSpinBoxesLimitsMax);
    }
  else
    {
    d->MaximumSpinBox->setMaximum(max);
    }
  d->MinimumSpinBox->setMaximum(max);
  d->MinimumSpinBox->blockSignals(blocked);
  d->MaximumSpinBox->blockSignals(blocked);
  // SpinBox can truncate max (depending on decimals).
  // use Spinbox's max to set Slider's max
  d->SettingSliderRange = true;
  d->Slider->setMaximum(d->MinimumSpinBox->maximum());
  d->SettingSliderRange = false;
  if (!d->useCustomSpinBoxesLimits())
    {
    Q_ASSERT(d->equal(d->MaximumSpinBox->maximum(), d->Slider->maximum()));
    }
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setRange(double min, double max)
{
  Q_D(ctkRangeWidget);

  if (d->useCustomSpinBoxesLimits())
    {
    min = qBound(d->CustomSpinBoxesLimitsMin, min, d->CustomSpinBoxesLimitsMax);
    max = qBound(d->CustomSpinBoxesLimitsMin, max, d->CustomSpinBoxesLimitsMax);
    }

  double oldMin = d->MinimumSpinBox->minimum();
  double oldMax = d->MaximumSpinBox->maximum();
  bool blocked = d->MinimumSpinBox->blockSignals(true);
  if (d->useCustomSpinBoxesLimits())
    {
    d->MinimumSpinBox->setRange(d->CustomSpinBoxesLimitsMin, qMax(min,max));
    }
  else
    {
    d->MinimumSpinBox->setRange(qMin(min,max), qMax(min,max));
    }
  d->MinimumSpinBox->blockSignals(blocked);
  blocked = d->MaximumSpinBox->blockSignals(true);
  if (d->useCustomSpinBoxesLimits())
    {
    d->MaximumSpinBox->setRange(qMin(min,max), d->CustomSpinBoxesLimitsMax);
    }
  else
    {
    d->MaximumSpinBox->setRange(qMin(min,max), qMax(min,max));
    }
  d->MaximumSpinBox->blockSignals(blocked);
  // SpinBox can truncate the range (depending on decimals).
  // use Spinbox's range to set Slider's range
  d->SettingSliderRange = true;
  d->Slider->setRange(d->MaximumSpinBox->minimum(), d->MinimumSpinBox->maximum());
  d->SettingSliderRange = false;
  if (!d->useCustomSpinBoxesLimits())
    {
    if (!(d->equal(d->MinimumSpinBox->minimum(), d->Slider->minimum())) ||
        !(d->equal(d->MaximumSpinBox->maximum(), d->Slider->maximum())) ||
        !(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value())) ||
        !(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value())))
      {
      qWarning("ctkRangeWidget::setRange : slider and spinbox are not synchronized");
      }
    }
  d->updateSpinBoxWidth();
  if (!d->useCustomSpinBoxesLimits())
    {
    if (oldMin != d->MinimumSpinBox->minimum() ||
        oldMax != d->MaximumSpinBox->maximum())
      {
      emit rangeChanged(d->MinimumSpinBox->minimum(), d->MaximumSpinBox->maximum());
      }
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::range(double range[2])const
{
  Q_D(const ctkRangeWidget);
  if (!d->useCustomSpinBoxesLimits())
    {
    Q_ASSERT(d->equal(d->MinimumSpinBox->minimum(), d->Slider->minimum()));
    Q_ASSERT(d->equal(d->MaximumSpinBox->maximum(), d->Slider->maximum()));
    }
  range[0] = d->Slider->minimum();
  range[1] = d->Slider->maximum();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::onSliderRangeChanged(double min, double max)
{
  Q_D(ctkRangeWidget);
  if (!d->SettingSliderRange)
    {
    this->setRange(min, max);
    }
}

/*
// --------------------------------------------------------------------------
double ctkRangeWidget::sliderPosition()const
{
  return d->Slider->sliderPosition();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setSliderPosition(double position)
{
  d->Slider->setSliderPosition(position);
}
*/
/*
// --------------------------------------------------------------------------
double ctkRangeWidget::previousSliderPosition()
{
  return d->Slider->previousSliderPosition();
}
*/

// --------------------------------------------------------------------------
void ctkRangeWidget::values(double &minValue, double &maxValue)const
{
  Q_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value()));
  Q_ASSERT(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value()));
  minValue = d->Changing ? d->MinimumValueBeforeChange : d->Slider->minimumValue();
  maxValue = d->Changing ? d->MaximumValueBeforeChange : d->Slider->maximumValue();
}

// --------------------------------------------------------------------------
double ctkRangeWidget::minimumValue()const
{
  Q_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value()));
  const double minValue =
    d->Changing ? d->MinimumValueBeforeChange : d->Slider->minimumValue();
  return minValue;
}

// --------------------------------------------------------------------------
double ctkRangeWidget::maximumValue()const
{
  Q_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value()));
  const double maxValue =
    d->Changing ? d->MaximumValueBeforeChange : d->Slider->maximumValue();
  return maxValue;
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMinimumValue(double _value)
{
  Q_D(ctkRangeWidget);
  // disable the tracking temporally to emit the
  // signal valueChanged if changeValue() is called
  bool isChanging = d->Changing;
  d->Changing = false;
  d->MinimumSpinBox->setValue(_value);
  Q_ASSERT(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value()));
  // restore the prop
  d->Changing = isChanging;
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMaximumValue(double _value)
{
  Q_D(ctkRangeWidget);
  // disable the tracking temporally to emit the
  // signal valueChanged if changeValue() is called
  bool isChanging = d->Changing;
  d->Changing = false;
  d->MaximumSpinBox->setValue(_value);
  Q_ASSERT(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value()));
  // restore the prop
  d->Changing = isChanging;
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setValues(double newMinimumValue, double newMaximumValue)
{
  Q_D(ctkRangeWidget);
  if (newMinimumValue > newMaximumValue)
    {
    qSwap(newMinimumValue, newMaximumValue);
    }
  // This test must take into account NaN values
  const bool minimumFirst = !(newMinimumValue > this->maximumValue());

  // disable the tracking temporally to emit the
  // signal valueChanged if changeValue() is called
  bool isChanging = d->Changing;
  d->Changing = false;
  // \todo: setting the spinbox separately is currently firing 2 signals and
  // between the signals, the state of the widget is inconsistent.
  bool wasBlocking = d->BlockSliderUpdate;
  d->BlockSliderUpdate = true;
  if (minimumFirst)
    {
    d->MinimumSpinBox->setValue(newMinimumValue);
    d->MaximumSpinBox->setValue(newMaximumValue);
    }
  else
    {
    d->MaximumSpinBox->setValue(newMaximumValue);
    d->MinimumSpinBox->setValue(newMinimumValue);
    }
  d->BlockSliderUpdate = wasBlocking;
  this->setSliderValues();

  Q_ASSERT(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value()));
  Q_ASSERT(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value()));
  // restore the prop
  d->Changing = isChanging;
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setSliderValues()
{
  Q_D(ctkRangeWidget);
  if (d->BlockSliderUpdate)
    {
    return;
    }
  if (d->useCustomSpinBoxesLimits())
    {
    if (d->MinimumSpinBox->value() < d->Slider->minimum())
      {
      d->Slider->setMinimum(d->MinimumSpinBox->value());
      }
    if (d->MaximumSpinBox->value() > d->Slider->maximum())
      {
      d->Slider->setMaximum(d->MaximumSpinBox->value());
      }
    }
  d->Slider->setValues(d->MinimumSpinBox->value(), d->MaximumSpinBox->value());
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMinimumToMaximumSpinBox(double minimum)
{
  Q_D(ctkRangeWidget);
  if (minimum != minimum) // NaN check
    {
    return;
    }
  if (d->useCustomSpinBoxesLimits())
    {
    d->MaximumSpinBox->setRange(minimum, d->CustomSpinBoxesLimitsMax);
    }
  else
    {
    d->MaximumSpinBox->setRange(minimum, d->Slider->maximum());
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMaximumToMinimumSpinBox(double maximum)
{
  Q_D(ctkRangeWidget);
  if (maximum != maximum) // NaN check
    {
    return;
    }
  if (d->useCustomSpinBoxesLimits())
    {
    d->MinimumSpinBox->setRange(d->CustomSpinBoxesLimitsMin, maximum);
    }
  else
    {
    d->MinimumSpinBox->setRange(d->Slider->minimum(), maximum);
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::startChanging()
{
  Q_D(ctkRangeWidget);
  if (d->Tracking)
    {
    return;
    }
  d->MinimumValueBeforeChange = this->minimumValue();
  d->MaximumValueBeforeChange = this->maximumValue();
  d->Changing = true;
}

// --------------------------------------------------------------------------
void ctkRangeWidget::stopChanging()
{
  Q_D(ctkRangeWidget);
  if (d->Tracking)
    {
    return;
    }
  d->Changing = false;
  bool emitMinValChanged = qAbs(this->minimumValue() - d->MinimumValueBeforeChange) > (this->singleStep() * 0.000000001);
  bool emitMaxValChanged = qAbs(this->maximumValue() - d->MaximumValueBeforeChange) > (this->singleStep() * 0.000000001);
  if (emitMinValChanged || emitMaxValChanged)
    {
    // emit the valuesChanged signal first
    emit this->valuesChanged(this->minimumValue(), this->maximumValue());
    }
  if (emitMinValChanged)
    {
    emit this->minimumValueChanged(this->minimumValue());
    }
  if (emitMaxValChanged)
    {
    emit this->maximumValueChanged(this->maximumValue());
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::changeMinimumValue(double newValue)
{
  Q_D(ctkRangeWidget);
  //if (d->Tracking)
    {
    emit this->minimumValueIsChanging(newValue);
    }
  if (!d->Changing)
    {
    emit this->minimumValueChanged(newValue);
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::changeMaximumValue(double newValue)
{
  Q_D(ctkRangeWidget);
  //if (d->Tracking)
    {
    emit this->maximumValueIsChanging(newValue);
    }
  if (!d->Changing)
    {
    emit this->maximumValueChanged(newValue);
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::changeValues(double newMinValue, double newMaxValue)
{
  Q_D(ctkRangeWidget);
  bool wasBlocking = d->BlockSliderUpdate;
  d->BlockSliderUpdate = true;
  d->MinimumSpinBox->setValue(newMinValue);
  d->MaximumSpinBox->setValue(newMaxValue);
  d->BlockSliderUpdate = wasBlocking;
  if (!d->Changing)
    {
    emit this->valuesChanged(newMinValue, newMaxValue);
    }
}

// --------------------------------------------------------------------------
bool ctkRangeWidget::eventFilter(QObject *obj, QEvent *event)
 {
   if (event->type() == QEvent::MouseButtonPress)
     {
     QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
     if (mouseEvent->button() & Qt::LeftButton)
       {
       this->startChanging();
       }
     }
   else if (event->type() == QEvent::MouseButtonRelease) 
     {
     QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
     if (mouseEvent->button() & Qt::LeftButton)
       {
       // here we might prevent ctkRangeWidget::stopChanging
       // from sending a valueChanged() event as the spinbox might
       // send a valueChanged() after eventFilter() is done.
       this->stopChanging();
       }
     } 
   // standard event processing
   return this->Superclass::eventFilter(obj, event);
 }

// --------------------------------------------------------------------------
double ctkRangeWidget::singleStep()const
{
  Q_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->Slider->singleStep(), d->MinimumSpinBox->singleStep()));
  Q_ASSERT(d->equal(d->Slider->singleStep(), d->MaximumSpinBox->singleStep()));
  return d->Slider->singleStep();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setSingleStep(double step)
{
  Q_D(ctkRangeWidget);
  if (!d->Slider->isValidStep(step))
    {
    qWarning() << "ctkRangeWidget::setSingleStep(" << step << ")"
               << "is outside valid bounds";
    return;
    }
  d->MinimumSpinBox->setSingleStep(step);
  d->MaximumSpinBox->setSingleStep(step);
  d->Slider->setSingleStep(d->MinimumSpinBox->singleStep());
  Q_ASSERT(d->equal(d->Slider->singleStep(), d->MinimumSpinBox->singleStep()));
  Q_ASSERT(d->equal(d->Slider->singleStep(), d->MaximumSpinBox->singleStep()));
  Q_ASSERT(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value()));
  Q_ASSERT(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value()));
}

// --------------------------------------------------------------------------
int ctkRangeWidget::decimals()const
{
  Q_D(const ctkRangeWidget);
  Q_ASSERT(d->MinimumSpinBox->decimals() == d->MaximumSpinBox->decimals());
  return d->MinimumSpinBox->decimals();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setDecimals(int newDecimals)
{
  Q_D(ctkRangeWidget);
  d->MinimumSpinBox->setDecimals(newDecimals);
  d->MaximumSpinBox->setDecimals(newDecimals);
  // The number of decimals can change the range values
  // i.e. 50.55 with 2 decimals -> 51 with 0 decimals
  // As the SpinBox range change doesn't fire signals, 
  // we have to do the synchronization manually here
  if (!d->useCustomSpinBoxesLimits())
    {
    d->Slider->setRange(d->MinimumSpinBox->minimum(), d->MaximumSpinBox->maximum());
    }
}

// --------------------------------------------------------------------------
QString ctkRangeWidget::prefix()const
{
  Q_D(const ctkRangeWidget);
  Q_ASSERT(d->MinimumSpinBox->prefix() == d->MaximumSpinBox->prefix());
  return d->MinimumSpinBox->prefix();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setPrefix(const QString& newPrefix)
{
  Q_D(ctkRangeWidget);
  d->MinimumSpinBox->setPrefix(newPrefix);
  d->MaximumSpinBox->setPrefix(newPrefix);
}

// --------------------------------------------------------------------------
QString ctkRangeWidget::suffix()const
{
  Q_D(const ctkRangeWidget);
 Q_ASSERT(d->MinimumSpinBox->suffix() == d->MaximumSpinBox->suffix());
  return d->MinimumSpinBox->suffix();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setSuffix(const QString& newSuffix)
{
  Q_D(ctkRangeWidget);
  d->MinimumSpinBox->setSuffix(newSuffix);
  d->MaximumSpinBox->setSuffix(newSuffix);
}

// --------------------------------------------------------------------------
double ctkRangeWidget::tickInterval()const
{
  Q_D(const ctkRangeWidget);
  return d->Slider->tickInterval();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setTickInterval(double ti)
{ 
  Q_D(ctkRangeWidget);
  d->Slider->setTickInterval(ti);
}

// -------------------------------------------------------------------------
void ctkRangeWidget::reset()
{
  this->setValues(this->minimum(), this->maximum());
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setSpinBoxAlignment(Qt::Alignment alignment)
{
  Q_D(ctkRangeWidget);
  if (d->SpinBoxAlignment == alignment)
    {
    return;
    }
  d->SpinBoxAlignment = alignment;
  d->relayout();
}

// -------------------------------------------------------------------------
Qt::Alignment ctkRangeWidget::spinBoxAlignment()const
{
  Q_D(const ctkRangeWidget);
  return d->SpinBoxAlignment;
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setSpinBoxTextAlignment(Qt::Alignment alignment)
{
  Q_D(ctkRangeWidget);
  d->MinimumSpinBox->setAlignment(alignment);
  d->MaximumSpinBox->setAlignment(alignment);
}

// -------------------------------------------------------------------------
Qt::Alignment ctkRangeWidget::spinBoxTextAlignment()const
{
  Q_D(const ctkRangeWidget);
  Q_ASSERT(d->MinimumSpinBox->alignment() == d->MaximumSpinBox->alignment());
  return d->MinimumSpinBox->alignment();
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setTracking(bool enable)
{
  Q_D(ctkRangeWidget);
  d->Tracking = enable;
}

// -------------------------------------------------------------------------
bool ctkRangeWidget::hasTracking()const
{
  Q_D(const ctkRangeWidget);
  return d->Tracking;
}

// -------------------------------------------------------------------------
bool ctkRangeWidget::isAutoSpinBoxWidth()const
{
  Q_D(const ctkRangeWidget);
  return d->AutoSpinBoxWidth;
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setAutoSpinBoxWidth(bool autoWidth)
{
  Q_D(ctkRangeWidget);
  d->AutoSpinBoxWidth = autoWidth;
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
bool ctkRangeWidget::symmetricMoves()const
{
  Q_D(const ctkRangeWidget);
  return d->Slider->symmetricMoves();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setSymmetricMoves(bool symmetry)
{
  Q_D(ctkRangeWidget);
  d->Slider->setSymmetricMoves(symmetry);
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setCustomSpinBoxesLimits(double min, double max)
{
  Q_D(ctkRangeWidget);

  if (max < min)
    {
    return;
    }

  d->CustomSpinBoxesLimitsMin = min;
  d->CustomSpinBoxesLimitsMax = max;
  this->setRange(d->Slider->minimum(), d->Slider->maximum());
}

// --------------------------------------------------------------------------
double ctkRangeWidget::customSpinBoxesLimitsMin()const
{
  Q_D(const ctkRangeWidget);
  return d->CustomSpinBoxesLimitsMin;
}

// --------------------------------------------------------------------------
double ctkRangeWidget::customSpinBoxesLimitsMax()const
{
  Q_D(const ctkRangeWidget);
  return d->CustomSpinBoxesLimitsMax;
}

// -------------------------------------------------------------------------
ctkDoubleRangeSlider* ctkRangeWidget::slider()const
{
  Q_D(const ctkRangeWidget);
  return d->Slider;
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setSlider(ctkDoubleRangeSlider* slider)
{
  Q_D(ctkRangeWidget);

  slider->setOrientation(d->Slider->orientation());
  slider->setRange(d->Slider->minimum(), d->Slider->maximum());
  slider->setValues(d->Slider->minimumValue(), d->Slider->maximumValue());
  slider->setSingleStep(d->Slider->singleStep());
  slider->setTracking(d->Slider->hasTracking());
  slider->setTickInterval(d->Slider->tickInterval());

  delete d->Slider;
  d->Slider = slider;

  d->connectSlider();

  d->relayout();
}

// -------------------------------------------------------------------------
ctkDoubleSpinBox* ctkRangeWidget::minimumSpinBox()const
{
  Q_D(const ctkRangeWidget);
  return d->MinimumSpinBox;
}

// -------------------------------------------------------------------------
ctkDoubleSpinBox* ctkRangeWidget::maximumSpinBox()const
{
  Q_D(const ctkRangeWidget);
  return d->MaximumSpinBox;
}

//----------------------------------------------------------------------------
void ctkRangeWidget::setValueProxy(ctkValueProxy* proxy)
{
  Q_D(ctkRangeWidget);
  if (proxy == d->Proxy.data())
    {
    return;
    }

  this->onValueProxyAboutToBeModified();

  if (d->Proxy)
    {
    disconnect(d->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
               this, SLOT(onValueProxyAboutToBeModified()));
    disconnect(d->Proxy.data(), SIGNAL(proxyModified()),
               this, SLOT(onValueProxyModified()));
    }

  d->Proxy = proxy;

  if (d->Proxy)
    {
    connect(d->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
            this, SLOT(onValueProxyAboutToBeModified()));
    }

  this->slider()->setValueProxy(proxy);
  this->minimumSpinBox()->setValueProxy(proxy);
  this->maximumSpinBox()->setValueProxy(proxy);

  if (d->Proxy)
    {
    connect(d->Proxy.data(), SIGNAL(proxyModified()),
            this, SLOT(onValueProxyModified()));
    }

  this->onValueProxyModified();
}

//----------------------------------------------------------------------------
ctkValueProxy* ctkRangeWidget::valueProxy() const
{
  Q_D(const ctkRangeWidget);
  return d->Proxy.data();
}

//-----------------------------------------------------------------------------
void ctkRangeWidget::onValueProxyAboutToBeModified()
{
}

//-----------------------------------------------------------------------------
void ctkRangeWidget::onValueProxyModified()
{
}
