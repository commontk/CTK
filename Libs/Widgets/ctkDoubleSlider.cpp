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

// QT includes
#include <QDebug>
#include <QHBoxLayout>
#include <QHelpEvent>
#include <QPointer>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QToolTip>

// CTK includes
#include "ctkDoubleSlider.h"
#include "ctkValueProxy.h"

// STD includes
#include <limits>

//-----------------------------------------------------------------------------
// ctkSlider

//-----------------------------------------------------------------------------
class ctkSlider: public QSlider
{
public:
  ctkSlider(QWidget* parent);
  using QSlider::initStyleOption;
};

//-----------------------------------------------------------------------------
ctkSlider::ctkSlider(QWidget* parent): QSlider(parent)
{
}

//-----------------------------------------------------------------------------
// ctkDoubleSliderPrivate

//-----------------------------------------------------------------------------
class ctkDoubleSliderPrivate
{
  Q_DECLARE_PUBLIC(ctkDoubleSlider);
protected:
  ctkDoubleSlider* const q_ptr;
public:
  ctkDoubleSliderPrivate(ctkDoubleSlider& object);
  int toInt(double value)const;
  double fromInt(int value)const;
  double safeFromInt(int value)const;
  void init();
  void updateOffset(double value);

  ctkSlider*    Slider;
  QString       HandleToolTip;
  double      Minimum;
  double      Maximum;
  bool        SettingRange;
  // we should have a Offset and SliderPositionOffset (and MinimumOffset?)
  double      Offset;
  double      SingleStep;
  double      PageStep;
  double      Value;
  /// Converts input value with displayed value
  QPointer<ctkValueProxy> Proxy;
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
  this->PageStep = 10.;
  this->Value = 0.;
}

// --------------------------------------------------------------------------
void ctkDoubleSliderPrivate::init()
{
  Q_Q(ctkDoubleSlider);
  this->Slider = new ctkSlider(q);
  this->Slider->installEventFilter(q);
  QHBoxLayout* l = new QHBoxLayout(q);
  l->addWidget(this->Slider);
  l->setContentsMargins(0,0,0,0);
  
  this->Minimum = this->Slider->minimum();
  this->Maximum = this->Slider->maximum();
  // this->Slider->singleStep is always 1
  this->SingleStep = this->Slider->singleStep();
  this->PageStep = this->Slider->pageStep();
  this->Value = this->Slider->value();

  q->connect(this->Slider, SIGNAL(valueChanged(int)), q, SLOT(onValueChanged(int)));
  q->connect(this->Slider, SIGNAL(sliderMoved(int)), q, SLOT(onSliderMoved(int)));
  q->connect(this->Slider, SIGNAL(sliderPressed()), q, SIGNAL(sliderPressed()));
  q->connect(this->Slider, SIGNAL(sliderReleased()), q, SIGNAL(sliderReleased()));
  q->connect(this->Slider, SIGNAL(rangeChanged(int,int)),
             q, SLOT(onRangeChanged(int,int)));

  q->setSizePolicy(this->Slider->sizePolicy());
  q->setAttribute(Qt::WA_WState_OwnSizePolicy, false);
}
  
// --------------------------------------------------------------------------
int ctkDoubleSliderPrivate::toInt(double doubleValue)const
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
double ctkDoubleSliderPrivate::safeFromInt(int intValue)const
{
  return qBound(this->Minimum, this->fromInt(intValue), this->Maximum);
}

// --------------------------------------------------------------------------
void ctkDoubleSliderPrivate::updateOffset(double value)
{
  this->Offset = (value / this->SingleStep) - this->toInt(value);
}

//-----------------------------------------------------------------------------
// ctkDoubleSlider

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
  this->setRange(min, qMax(min, this->maximum()));
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setMaximum(double max)
{
  this->setRange(qMin(this->minimum(), max), max);
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setRange(double newMin, double newMax)
{
  Q_D(ctkDoubleSlider);
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

  if (d->Minimum >= d->Value)
    {
    d->updateOffset(d->Minimum);
    }
  if (d->Maximum <= d->Value)
    {
    d->updateOffset(d->Maximum);
    }
  bool wasSettingRange = d->SettingRange;
  d->SettingRange = true;
  d->Slider->setRange(d->toInt(newMin), d->toInt(newMax));
  d->SettingRange = wasSettingRange;
  if (!wasSettingRange && (d->Minimum != oldMin || d->Maximum != oldMax))
    {
    emit this->rangeChanged(this->minimum(), this->maximum());
    }
  /// In case QSlider::setRange(...) didn't notify the value
  /// has changed.
  this->setValue(this->value());
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::minimum()const
{
  Q_D(const ctkDoubleSlider);
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
double ctkDoubleSlider::maximum()const
{
  Q_D(const ctkDoubleSlider);
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
double ctkDoubleSlider::sliderPosition()const
{
  Q_D(const ctkDoubleSlider);
  int intPosition = d->Slider->sliderPosition();
  double position = d->safeFromInt(intPosition);
  if (d->Proxy)
    {
    position = d->Proxy.data()->valueFromProxyValue(position);
    }
  return position;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setSliderPosition(double newPosition)
{
  Q_D(ctkDoubleSlider);
  if (d->Proxy)
    {
    newPosition = d->Proxy.data()->proxyValueFromValue(newPosition);
    }
  int newIntPosition = d->toInt(newPosition);
  d->Slider->setSliderPosition(newIntPosition);
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::value()const
{
  Q_D(const ctkDoubleSlider);
  double val = d->Value;
  if (d->Proxy)
    {
    val = d->Proxy.data()->valueFromProxyValue(val);
    }
  return val;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setValue(double newValue)
{
  Q_D(ctkDoubleSlider);
  if (d->Proxy)
    {
    newValue = d->Proxy.data()->proxyValueFromValue(newValue);
    }

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
      emit this->valueChanged(this->value());
      }
    }
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::singleStep()const
{
  Q_D(const ctkDoubleSlider);
  double step = d->SingleStep;
  return step;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setSingleStep(double newStep)
{
  Q_D(ctkDoubleSlider);
  if (!this->isValidStep(newStep))
    {
    qWarning() << "ctkDoubleSlider::setSingleStep("<< newStep <<")"
               << "is outside of valid bounds.";
    return;
    }
  d->SingleStep = newStep;
  d->updateOffset(d->Value);
  // update the new values of the QSlider
  bool oldBlockSignals = d->Slider->blockSignals(true);
  d->Slider->setRange(d->toInt(d->Minimum), d->toInt(d->Maximum));
  d->Slider->setValue(d->toInt(d->Value));
  d->Slider->setPageStep(d->toInt(d->PageStep));
  d->Slider->blockSignals(oldBlockSignals);
  Q_ASSERT(qFuzzyCompare(d->Value,d->safeFromInt(d->Slider->value())));
}

// --------------------------------------------------------------------------
bool ctkDoubleSlider::isValidStep(double step)const
{
  Q_D(const ctkDoubleSlider);
  if (d->Minimum == d->Maximum)
    {
    return true;
    }
  const double minStep = qMax(d->Maximum / std::numeric_limits<double>::max(),
                              std::numeric_limits<double>::epsilon());
  const double maxStep = qMin(d->Maximum - d->Minimum,
                              static_cast<double>(std::numeric_limits<int>::max()));
  return step >= minStep && step <= maxStep;
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::pageStep()const
{
  Q_D(const ctkDoubleSlider);
  return d->PageStep;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setPageStep(double newStep)
{
  Q_D(ctkDoubleSlider);
  d->PageStep = newStep;
  int intPageStep = d->toInt(d->PageStep);
  d->Slider->setPageStep(intPageStep);
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::tickInterval()const
{
  Q_D(const ctkDoubleSlider);
  // No need to apply Offset
  double interval = d->SingleStep * d->Slider->tickInterval();
  return interval;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setTickInterval(double newInterval)
{
  Q_D(ctkDoubleSlider);
  int newIntInterval = d->toInt(newInterval);
  d->Slider->setTickInterval(newIntInterval);
}

// --------------------------------------------------------------------------
QSlider::TickPosition ctkDoubleSlider::tickPosition()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->tickPosition();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setTickPosition(QSlider::TickPosition newTickPosition)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setTickPosition(newTickPosition);
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
bool ctkDoubleSlider::invertedAppearance()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->invertedAppearance();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setInvertedAppearance(bool invertedAppearance)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setInvertedAppearance(invertedAppearance);
}

// --------------------------------------------------------------------------
bool ctkDoubleSlider::invertedControls()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->invertedControls();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setInvertedControls(bool invertedControls)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setInvertedControls(invertedControls);
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
QString ctkDoubleSlider::handleToolTip()const
{
  Q_D(const ctkDoubleSlider);
  return d->HandleToolTip;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setHandleToolTip(const QString& toolTip)
{
  Q_D(ctkDoubleSlider);
  d->HandleToolTip = toolTip;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onValueChanged(int newValue)
{
  Q_D(ctkDoubleSlider);
  double doubleNewValue = d->safeFromInt(newValue);
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
  emit this->valueChanged(this->value());
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onSliderMoved(int newPosition)
{
  Q_D(const ctkDoubleSlider);
  emit this->sliderMoved(d->safeFromInt(newPosition));
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onRangeChanged(int newIntMin, int newIntMax)
{
  Q_D(const ctkDoubleSlider);
  if (d->SettingRange)
    {
    return;
    }
  double newMin = d->fromInt(newIntMin);
  double newMax = d->fromInt(newIntMax);
  if (d->Proxy)
    {
    newMin = d->Proxy.data()->valueFromProxyValue(newMin);
    newMax = d->Proxy.data()->valueFromProxyValue(newMax);
    }
  this->setRange(newMin, newMax);
}

// --------------------------------------------------------------------------
bool ctkDoubleSlider::eventFilter(QObject* watched, QEvent* event)
{
  Q_D(ctkDoubleSlider);
  if (watched == d->Slider)
    {
    switch(event->type())
      {
      case QEvent::ToolTip:
        {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        QStyleOptionSlider opt;
        d->Slider->initStyleOption(&opt);
        QStyle::SubControl hoveredControl =
          d->Slider->style()->hitTestComplexControl(
            QStyle::CC_Slider, &opt, helpEvent->pos(), this);
        if (!d->HandleToolTip.isEmpty() &&
            hoveredControl == QStyle::SC_SliderHandle)
          {
          QToolTip::showText(helpEvent->globalPos(), d->HandleToolTip.arg(this->value()));
          event->accept();
          return true;
          }
        }
      default:
        break;
      }
    }
  return this->Superclass::eventFilter(watched, event);
}

// --------------------------------------------------------------------------
QSlider* ctkDoubleSlider::slider()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider;
}

//----------------------------------------------------------------------------
void ctkDoubleSlider::setValueProxy(ctkValueProxy* proxy)
{
  Q_D(ctkDoubleSlider);
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
ctkValueProxy* ctkDoubleSlider::valueProxy() const
{
  Q_D(const ctkDoubleSlider);
  return d->Proxy.data();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onValueProxyAboutToBeModified()
{
  Q_D(ctkDoubleSlider);
  d->Slider->setProperty("inputValue", this->value());
  d->Slider->setProperty("inputMinimum", this->minimum());
  d->Slider->setProperty("inputMaximum", this->maximum());
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onValueProxyModified()
{
  Q_D(ctkDoubleSlider);
  bool wasBlockingSignals = this->blockSignals(true);
  bool wasSettingRange = d->SettingRange;
  d->SettingRange = true;
  this->setRange(d->Slider->property("inputMinimum").toDouble(),
                 d->Slider->property("inputMaximum").toDouble());
  d->SettingRange = wasSettingRange;
  this->setValue(d->Slider->property("inputValue").toDouble());
  this->blockSignals(wasBlockingSignals);
}

