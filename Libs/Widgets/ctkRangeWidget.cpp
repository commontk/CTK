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
#include <QMouseEvent>

// CTK includes
#include "ctkRangeWidget.h"
#include "ui_ctkRangeWidget.h"

//-----------------------------------------------------------------------------
class ctkRangeWidgetPrivate: public ctkPrivate<ctkRangeWidget>,
                              public Ui_ctkRangeWidget
{
public:
  ctkRangeWidgetPrivate();
  void updateSpinBoxWidth();
  int synchronizedSpinBoxWidth()const;
  void synchronizeSiblingSpinBox(int newWidth);
  static bool equal(double v1, double v2);
  void relayout();

  bool          Tracking;
  bool          Changing;
  double        MinimumValueBeforeChange;
  double        MaximumValueBeforeChange;
  bool          AutoSpinBoxWidth;
  Qt::Alignment SpinBoxAlignment;
};

// --------------------------------------------------------------------------
bool ctkRangeWidgetPrivate::equal(double v1, double v2)
{
  return qAbs(v1 - v2) < 0.0001;
}

// --------------------------------------------------------------------------
ctkRangeWidgetPrivate::ctkRangeWidgetPrivate()
{
  this->Tracking = true;
  this->Changing = false;
  this->MinimumValueBeforeChange = 0.;
  this->MaximumValueBeforeChange = 0.;
  this->AutoSpinBoxWidth = true;
  this->SpinBoxAlignment = Qt::AlignVCenter;
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
  CTK_P(const ctkRangeWidget);
  //Q_ASSERT(this->MinimumSpinBox->sizeHint() == this->MaximumSpinBox->sizeHint());
  int maxWidth = qMax(this->MinimumSpinBox->sizeHint().width(),
                      this->MaximumSpinBox->sizeHint().width());
  if (!p->parent())
    {
    return maxWidth;
    }
  QList<ctkRangeWidget*> siblings =
    p->parent()->findChildren<ctkRangeWidget*>();
  foreach(ctkRangeWidget* sibling, siblings)
    {
    maxWidth = qMax(maxWidth, qMax(sibling->ctk_d()->MaximumSpinBox->sizeHint().width(),
                                   sibling->ctk_d()->MaximumSpinBox->sizeHint().width()));
    }
  return maxWidth;
}

// --------------------------------------------------------------------------
void ctkRangeWidgetPrivate::synchronizeSiblingSpinBox(int width)
{
  CTK_P(const ctkRangeWidget);
  QList<ctkRangeWidget*> siblings =
    p->parent()->findChildren<ctkRangeWidget*>();
  foreach(ctkRangeWidget* sibling, siblings)
    {
    if (sibling != p && sibling->isAutoSpinBoxWidth())
      {
      sibling->ctk_d()->MinimumSpinBox->setMinimumWidth(width);
      sibling->ctk_d()->MaximumSpinBox->setMinimumWidth(width);
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
  else if (this->SpinBoxAlignment & Qt::AlignVCenter)
    {
    this->GridLayout->addWidget(this->MinimumSpinBox,0,0);
    this->GridLayout->addWidget(this->Slider,0,1);
    this->GridLayout->addWidget(this->MaximumSpinBox,0,2);
    }
  else if (this->SpinBoxAlignment & Qt::AlignBottom)
    {
    this->GridLayout->addWidget(this->MinimumSpinBox,1,0);
    this->GridLayout->addWidget(this->MaximumSpinBox,1,2);
    this->GridLayout->addWidget(this->Slider,0, 0, 1, 3);
    }
}

// --------------------------------------------------------------------------
ctkRangeWidget::ctkRangeWidget(QWidget* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkRangeWidget);
  CTK_D(ctkRangeWidget);
  
  d->setupUi(this);

  d->MinimumSpinBox->setMinimum(d->Slider->minimum());
  d->MinimumSpinBox->setMaximum(d->Slider->maximum());
  d->MaximumSpinBox->setMinimum(d->Slider->minimum());
  d->MaximumSpinBox->setMaximum(d->Slider->maximum());
  d->MinimumSpinBox->setValue(d->Slider->minimumValue());
  d->MaximumSpinBox->setValue(d->Slider->maximumValue());
  
  //this->connect(d->Slider, SIGNAL(minimumValueChanged(double)), d->MinimumSpinBox, SLOT(setValue(double)));
  //this->connect(d->Slider, SIGNAL(maximumValueChanged(double)), d->MaximumSpinBox, SLOT(setValue(double)));
  this->connect(d->Slider, SIGNAL(valuesChanged(double, double)), this, SLOT(changeValues(double,double)));
  this->connect(d->Slider, SIGNAL(minimumValueChanged(double)), this, SLOT(changeMinimumValue(double)));
  this->connect(d->Slider, SIGNAL(maximumValueChanged(double)), this, SLOT(changeMaximumValue(double)));
  
  this->connect(d->MinimumSpinBox, SIGNAL(valueChanged(double)), d->Slider, SLOT(setMinimumValue(double)));
  this->connect(d->MaximumSpinBox, SIGNAL(valueChanged(double)), d->Slider, SLOT(setMaximumValue(double)));
  this->connect(d->MinimumSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMinimumToMaximumSpinBox(double)));
  this->connect(d->MaximumSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMaximumToMinimumSpinBox(double)));

  this->connect(d->Slider, SIGNAL(sliderPressed()), this, SLOT(startChanging()));
  this->connect(d->Slider, SIGNAL(sliderReleased()), this, SLOT(stopChanging()));
  d->MinimumSpinBox->installEventFilter(this);
  d->MaximumSpinBox->installEventFilter(this);
}

// --------------------------------------------------------------------------
double ctkRangeWidget::minimum()const
{
  CTK_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->MinimumSpinBox->minimum(),d->Slider->minimum()));
  return d->Slider->minimum();
}

// --------------------------------------------------------------------------
double ctkRangeWidget::maximum()const
{
  CTK_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->MaximumSpinBox->maximum(), d->Slider->maximum()));
  return d->Slider->maximum();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMinimum(double min)
{
  CTK_D(ctkRangeWidget);
  d->MinimumSpinBox->setMinimum(min);
  // SpinBox can truncate min (depending on decimals).
  // use Spinbox's min to set Slider's min
  d->Slider->setMinimum(d->MinimumSpinBox->minimum());
  Q_ASSERT(d->equal(d->MinimumSpinBox->minimum(),d->Slider->minimum()));
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMaximum(double max)
{
  CTK_D(ctkRangeWidget);
  d->MaximumSpinBox->setMaximum(max);
  // SpinBox can truncate max (depending on decimals).
  // use Spinbox's max to set Slider's max
  d->Slider->setMaximum(d->MaximumSpinBox->maximum());
  Q_ASSERT(d->equal(d->MaximumSpinBox->maximum(), d->Slider->maximum()));
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setRange(double min, double max)
{
  CTK_D(ctkRangeWidget);
  
  d->MinimumSpinBox->setMinimum(qMin(min,max));
  d->MaximumSpinBox->setMaximum(qMax(min,max));
  // SpinBox can truncate the range (depending on decimals).
  // use Spinbox's range to set Slider's range
  d->Slider->setRange(d->MinimumSpinBox->minimum(), d->MaximumSpinBox->maximum());
  Q_ASSERT(d->equal(d->MinimumSpinBox->minimum(), d->Slider->minimum()));
  Q_ASSERT(d->equal(d->MaximumSpinBox->maximum(), d->Slider->maximum()));
  d->updateSpinBoxWidth();
}
/*
// --------------------------------------------------------------------------
double ctkRangeWidget::sliderPosition()const
{
  return ctk_d()->Slider->sliderPosition();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setSliderPosition(double position)
{
  ctk_d()->Slider->setSliderPosition(position);
}
*/
/*
// --------------------------------------------------------------------------
double ctkRangeWidget::previousSliderPosition()
{
  return ctk_d()->Slider->previousSliderPosition();
}
*/

// --------------------------------------------------------------------------
double ctkRangeWidget::minimumValue()const
{
  CTK_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value()));
  return d->Changing ? d->MinimumValueBeforeChange : d->Slider->minimumValue();
}

// --------------------------------------------------------------------------
double ctkRangeWidget::maximumValue()const
{
  CTK_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value()));
  return d->Changing ? d->MaximumValueBeforeChange : d->Slider->maximumValue();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMinimumValue(double _value)
{
  CTK_D(ctkRangeWidget);
  // disable the tracking temporally to emit the
  // signal valueChanged if changeValue() is called
  bool isChanging = d->Changing;
  d->Changing = false;
  d->MinimumSpinBox->setValue(_value);
  // Why do we need to set the value to the slider ?
  //d->Slider->setValue(d->SpinBox->value());
  Q_ASSERT(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value()));
  // restore the prop
  d->Changing = isChanging;
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMaximumValue(double _value)
{
  CTK_D(ctkRangeWidget);
  // disable the tracking temporally to emit the
  // signal valueChanged if changeValue() is called
  bool isChanging = d->Changing;
  d->Changing = false;
  d->MaximumSpinBox->setValue(_value);
  // Why do we need to set the value to the slider ?
  //d->Slider->setValue(d->SpinBox->value());
  Q_ASSERT(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value()));
  // restore the prop
  d->Changing = isChanging;
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setValues(double newMinimumValue, double newMaximumValue)
{
  CTK_D(ctkRangeWidget);
  // disable the tracking temporally to emit the
  // signal valueChanged if changeValue() is called
  bool isChanging = d->Changing;
  d->Changing = false;
  d->MinimumSpinBox->setValue(newMinimumValue);
  d->MaximumSpinBox->setValue(newMaximumValue);
  // Why do we need to set the value to the slider ?
  //d->Slider->setValue(d->SpinBox->value());
  Q_ASSERT(d->equal(d->Slider->minimumValue(), d->MinimumSpinBox->value()));
  Q_ASSERT(d->equal(d->Slider->maximumValue(), d->MaximumSpinBox->value()));
  // restore the prop
  d->Changing = isChanging;
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMinimumToMaximumSpinBox(double minimum)
{
  ctk_d()->MaximumSpinBox->setMinimum(minimum);
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setMaximumToMinimumSpinBox(double maximum)
{
  ctk_d()->MinimumSpinBox->setMaximum(maximum);
}

// --------------------------------------------------------------------------
void ctkRangeWidget::startChanging()
{
  CTK_D(ctkRangeWidget);
  if (d->Tracking)
    {
    return;
    }
  d->Changing = true;
  d->MinimumValueBeforeChange = this->minimumValue();
  d->MaximumValueBeforeChange = this->maximumValue();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::stopChanging()
{
  CTK_D(ctkRangeWidget);
  if (d->Tracking)
    {
    return;
    }
  d->Changing = false;
  bool changed = false;
  if (qAbs(this->minimumValue() - d->MinimumValueBeforeChange) > (this->singleStep() * 0.000000001))
    {
    emit this->minimumValueChanged(this->minimumValue());
    changed = true;
    }
  if (qAbs(this->maximumValue() - d->MaximumValueBeforeChange) > (this->singleStep() * 0.000000001))
    {
    emit this->maximumValueChanged(this->maximumValue());
    changed = true;
    }
  if (changed)
    {
    emit this->valuesChanged(this->minimumValue(), this->maximumValue());
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::changeMinimumValue(double newValue)
{
  CTK_D(ctkRangeWidget);
  //if (d->Tracking)
    {
    emit this->minimumValueIsChanging(newValue);
    }
  if (!d->Changing)
    {
    emit this->minimumValueChanged(newValue);
    emit this->valuesChanged(newValue, this->maximumValue());
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::changeMaximumValue(double newValue)
{
  CTK_D(ctkRangeWidget);
  //if (d->Tracking)
    {
    emit this->maximumValueIsChanging(newValue);
    }
  if (!d->Changing)
    {
    emit this->maximumValueChanged(newValue);
    emit this->valuesChanged(this->minimumValue(), newValue);
    }
}

// --------------------------------------------------------------------------
void ctkRangeWidget::changeValues(double newMinValue, double newMaxValue)
{
  CTK_D(ctkRangeWidget);
  d->MinimumSpinBox->setValue(newMinValue);
  d->MaximumSpinBox->setValue(newMaxValue);
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
  CTK_D(const ctkRangeWidget);
  Q_ASSERT(d->equal(d->Slider->singleStep(), d->MinimumSpinBox->singleStep()) &&
           d->equal(d->Slider->singleStep(), d->MaximumSpinBox->singleStep()));
  return d->Slider->singleStep();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setSingleStep(double step)
{
  CTK_D(ctkRangeWidget);
  d->MinimumSpinBox->setSingleStep(step);
  d->MaximumSpinBox->setSingleStep(step);
  d->Slider->setSingleStep(d->MinimumSpinBox->singleStep());
  Q_ASSERT(d->equal(d->Slider->singleStep(), d->MinimumSpinBox->singleStep()) &&
           d->equal(d->Slider->singleStep(), d->MaximumSpinBox->singleStep()));
}

// --------------------------------------------------------------------------
int ctkRangeWidget::decimals()const
{
  CTK_D(const ctkRangeWidget);
  Q_ASSERT(d->MinimumSpinBox->decimals() == d->MaximumSpinBox->decimals());
  return d->MinimumSpinBox->decimals();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setDecimals(int newDecimals)
{
  CTK_D(ctkRangeWidget);
  d->MinimumSpinBox->setDecimals(newDecimals);
  d->MaximumSpinBox->setDecimals(newDecimals);
  // The number of decimals can change the range values
  // i.e. 50.55 with 2 decimals -> 51 with 0 decimals
  // As the SpinBox range change doesn't fire signals, 
  // we have to do the synchronization manually here
  d->Slider->setMinimum(d->MinimumSpinBox->minimum());
  d->Slider->setMaximum(d->MaximumSpinBox->maximum());
}

// --------------------------------------------------------------------------
QString ctkRangeWidget::prefix()const
{
  CTK_D(const ctkRangeWidget);
  Q_ASSERT(d->MinimumSpinBox->prefix() == d->MaximumSpinBox->prefix());
  return d->MinimumSpinBox->prefix();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setPrefix(const QString& newPrefix)
{
  CTK_D(ctkRangeWidget);
  d->MinimumSpinBox->setPrefix(newPrefix);
  d->MaximumSpinBox->setPrefix(newPrefix);
}

// --------------------------------------------------------------------------
QString ctkRangeWidget::suffix()const
{
  CTK_D(const ctkRangeWidget);
 Q_ASSERT(d->MinimumSpinBox->suffix() == d->MaximumSpinBox->suffix());
  return d->MinimumSpinBox->suffix();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setSuffix(const QString& newSuffix)
{
  CTK_D(ctkRangeWidget);
  d->MinimumSpinBox->setSuffix(newSuffix);
  d->MaximumSpinBox->setSuffix(newSuffix);
}

// --------------------------------------------------------------------------
double ctkRangeWidget::tickInterval()const
{
  CTK_D(const ctkRangeWidget);
  return d->Slider->tickInterval();
}

// --------------------------------------------------------------------------
void ctkRangeWidget::setTickInterval(double ti)
{ 
  CTK_D(ctkRangeWidget);
  d->Slider->setTickInterval(ti);
}

// -------------------------------------------------------------------------
void ctkRangeWidget::reset()
{
  this->setMinimumValue(this->minimum());
  this->setMaximumValue(this->maximum());
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setSpinBoxAlignment(Qt::Alignment alignment)
{
  CTK_D(ctkRangeWidget);
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
  CTK_D(const ctkRangeWidget);
  return d->SpinBoxAlignment;
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setSpinBoxTextAlignment(Qt::Alignment alignment)
{
  CTK_D(ctkRangeWidget);
  d->MinimumSpinBox->setAlignment(alignment);
  d->MaximumSpinBox->setAlignment(alignment);
}

// -------------------------------------------------------------------------
Qt::Alignment ctkRangeWidget::spinBoxTextAlignment()const
{
  CTK_D(const ctkRangeWidget);
  Q_ASSERT(d->MinimumSpinBox->alignment() == d->MaximumSpinBox->alignment());
  return d->MinimumSpinBox->alignment();
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setTracking(bool enable)
{
  CTK_D(ctkRangeWidget);
  d->Tracking = enable;
}

// -------------------------------------------------------------------------
bool ctkRangeWidget::hasTracking()const
{
  CTK_D(const ctkRangeWidget);
  return d->Tracking;
}

// -------------------------------------------------------------------------
bool ctkRangeWidget::isAutoSpinBoxWidth()const
{
  CTK_D(const ctkRangeWidget);
  return d->AutoSpinBoxWidth;
}

// -------------------------------------------------------------------------
void ctkRangeWidget::setAutoSpinBoxWidth(bool autoWidth)
{
  CTK_D(ctkRangeWidget);
  d->AutoSpinBoxWidth = autoWidth;
  d->updateSpinBoxWidth();
}
