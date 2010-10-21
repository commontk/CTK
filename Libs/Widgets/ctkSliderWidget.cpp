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

// Qt includes
#include <QDebug>
#include <QMouseEvent>

// CTK includes
#include "ctkSliderWidget.h"
#include "ui_ctkSliderWidget.h"

//-----------------------------------------------------------------------------
namespace
{
bool equal(double v1, double v2)
{
  return qAbs(v1 - v2) < 0.0001;
}
}

//-----------------------------------------------------------------------------
class ctkSliderWidgetPrivate: public Ui_ctkSliderWidget
{
  Q_DECLARE_PUBLIC(ctkSliderWidget);
protected:
  ctkSliderWidget* const q_ptr;

public:
  ctkSliderWidgetPrivate(ctkSliderWidget& object);
  void updateSpinBoxWidth();
  int synchronizedSpinBoxWidth()const;
  void synchronizeSiblingSpinBox(int newWidth);

  bool   Tracking;
  bool   Changing;
  double ValueBeforeChange;
  bool   AutoSpinBoxWidth;
};

// --------------------------------------------------------------------------
ctkSliderWidgetPrivate::ctkSliderWidgetPrivate(ctkSliderWidget& object)
  :q_ptr(&object)
{
  this->Tracking = true;
  this->Changing = false;
  this->ValueBeforeChange = 0.;
  this->AutoSpinBoxWidth = true;
}

// --------------------------------------------------------------------------
void ctkSliderWidgetPrivate::updateSpinBoxWidth()
{
  int spinBoxWidth = this->synchronizedSpinBoxWidth();
  if (this->AutoSpinBoxWidth)
    {
    this->SpinBox->setMinimumWidth(spinBoxWidth);
    }
  else
    {
    this->SpinBox->setMinimumWidth(0);
    }
  this->synchronizeSiblingSpinBox(spinBoxWidth);
}

// --------------------------------------------------------------------------
int ctkSliderWidgetPrivate::synchronizedSpinBoxWidth()const
{
  Q_Q(const ctkSliderWidget);
  int maxWidth = this->SpinBox->sizeHint().width();
  if (!q->parent())
    {
    return maxWidth;
    }
  QList<ctkSliderWidget*> siblings =
    q->parent()->findChildren<ctkSliderWidget*>();
  foreach(ctkSliderWidget* sibling, siblings)
    {
    maxWidth = qMax(maxWidth, sibling->d_func()->SpinBox->sizeHint().width());
    }
  return maxWidth;
}

// --------------------------------------------------------------------------
void ctkSliderWidgetPrivate::synchronizeSiblingSpinBox(int width)
{
  Q_Q(const ctkSliderWidget);
  QList<ctkSliderWidget*> siblings =
    q->parent()->findChildren<ctkSliderWidget*>();
  foreach(ctkSliderWidget* sibling, siblings)
    {
    if (sibling != q && sibling->isAutoSpinBoxWidth())
      {
      sibling->d_func()->SpinBox->setMinimumWidth(width);
      }
    }
}

// --------------------------------------------------------------------------
ctkSliderWidget::ctkSliderWidget(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkSliderWidgetPrivate(*this))
{
  Q_D(ctkSliderWidget);
  
  d->setupUi(this);

  d->Slider->setMaximum(d->SpinBox->maximum());
  d->Slider->setMinimum(d->SpinBox->minimum());

  this->connect(d->Slider, SIGNAL(valueChanged(double)), d->SpinBox, SLOT(setValue(double)));
  this->connect(d->SpinBox, SIGNAL(valueChanged(double)), d->Slider, SLOT(setValue(double)));

  //this->connect(d->Slider, SIGNAL(valueChanged(double)), SIGNAL(valueChanged(double)));
  this->connect(d->Slider, SIGNAL(sliderPressed()), this, SLOT(startChanging()));
  this->connect(d->Slider, SIGNAL(sliderReleased()), this, SLOT(stopChanging()));
  this->connect(d->Slider, SIGNAL(valueChanged(double)), this, SLOT(changeValue(double)));
  d->SpinBox->installEventFilter(this);
}

// --------------------------------------------------------------------------
ctkSliderWidget::~ctkSliderWidget()
{
}

// --------------------------------------------------------------------------
double ctkSliderWidget::minimum()const
{
  Q_D(const ctkSliderWidget);
  Q_ASSERT(equal(d->SpinBox->minimum(),d->Slider->minimum()));
  return d->Slider->minimum();
}

// --------------------------------------------------------------------------
double ctkSliderWidget::maximum()const
{
  Q_D(const ctkSliderWidget);
  Q_ASSERT(equal(d->SpinBox->maximum(),d->Slider->maximum()));
  return d->Slider->maximum();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setMinimum(double min)
{
  Q_D(ctkSliderWidget);
  d->SpinBox->setMinimum(min);
  // SpinBox can truncate min (depending on decimals).
  // use Spinbox's min to set Slider's min
  d->Slider->setMinimum(d->SpinBox->minimum());
  Q_ASSERT(equal(d->SpinBox->minimum(),d->Slider->minimum()));
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setMaximum(double max)
{
  Q_D(ctkSliderWidget);
  d->SpinBox->setMaximum(max);
  // SpinBox can truncate max (depending on decimals).
  // use Spinbox's max to set Slider's max
  d->Slider->setMaximum(d->SpinBox->maximum());
  Q_ASSERT(equal(d->SpinBox->maximum(), d->Slider->maximum()));
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setRange(double min, double max)
{
  Q_D(ctkSliderWidget);
  
  d->SpinBox->setRange(min, max);
  // SpinBox can truncate the range (depending on decimals).
  // use Spinbox's range to set Slider's range
  d->Slider->setRange(d->SpinBox->minimum(), d->SpinBox->maximum());
  Q_ASSERT(equal(d->SpinBox->minimum(), d->Slider->minimum()));
  Q_ASSERT(equal(d->SpinBox->maximum(), d->Slider->maximum()));
  d->updateSpinBoxWidth();
}
/*
// --------------------------------------------------------------------------
double ctkSliderWidget::sliderPosition()const
{
  return d->Slider->sliderPosition();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setSliderPosition(double position)
{
  d->Slider->setSliderPosition(position);
}
*/
/*
// --------------------------------------------------------------------------
double ctkSliderWidget::previousSliderPosition()
{
  return d->Slider->previousSliderPosition();
}
*/

// --------------------------------------------------------------------------
double ctkSliderWidget::value()const
{
  Q_D(const ctkSliderWidget);
  Q_ASSERT(equal(d->Slider->value(), d->SpinBox->value()));
  return d->Changing ? d->ValueBeforeChange : d->Slider->value();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setValue(double _value)
{
  Q_D(ctkSliderWidget);
  // disable the tracking temporally to emit the
  // signal valueChanged if changeValue() is called
  bool isChanging = d->Changing;
  d->Changing = false;
  d->SpinBox->setValue(_value);
  // Why do we need to set the value to the slider ?
  //d->Slider->setValue(d->SpinBox->value());
  Q_ASSERT(equal(d->Slider->value(), d->SpinBox->value()));
  // restore the prop
  d->Changing = isChanging;
}

// --------------------------------------------------------------------------
void ctkSliderWidget::startChanging()
{
  Q_D(ctkSliderWidget);
  if (d->Tracking)
    {
    return;
    }
  d->Changing = true;
  d->ValueBeforeChange = this->value();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::stopChanging()
{
  Q_D(ctkSliderWidget);
  if (d->Tracking)
    {
    return;
    }
  d->Changing = false;
  if (qAbs(this->value() - d->ValueBeforeChange) > (this->singleStep() * 0.000000001))
    {
    emit this->valueChanged(this->value());
    }
}

// --------------------------------------------------------------------------
void ctkSliderWidget::changeValue(double newValue)
{
  Q_D(ctkSliderWidget);
  if (!d->Tracking)
    {
    emit this->valueIsChanging(newValue);
    }
  if (!d->Changing)
    {
    emit this->valueChanged(newValue);
    }
}

// --------------------------------------------------------------------------
bool ctkSliderWidget::eventFilter(QObject *obj, QEvent *event)
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
       // here we might prevent ctkSliderWidget::stopChanging
       // from sending a valueChanged() event as the spinbox might
       // send a valueChanged() after eventFilter() is done.
       this->stopChanging();
       }
     } 
   // standard event processing
   return this->Superclass::eventFilter(obj, event);
 }

// --------------------------------------------------------------------------
double ctkSliderWidget::singleStep()const
{
  Q_D(const ctkSliderWidget);
  Q_ASSERT(equal(d->Slider->singleStep(), d->SpinBox->singleStep()));
  return d->Slider->singleStep();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setSingleStep(double step)
{
  Q_D(ctkSliderWidget);
  d->SpinBox->setSingleStep(step);
  d->Slider->setSingleStep(d->SpinBox->singleStep());
  Q_ASSERT(equal(d->Slider->singleStep(), d->SpinBox->singleStep()));
}

// --------------------------------------------------------------------------
int ctkSliderWidget::decimals()const
{
  Q_D(const ctkSliderWidget);
  return d->SpinBox->decimals();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setDecimals(int newDecimals)
{
  Q_D(ctkSliderWidget);
  d->SpinBox->setDecimals(newDecimals);
  // The number of decimals can change the range values
  // i.e. 50.55 with 2 decimals -> 51 with 0 decimals
  // As the SpinBox range change doesn't fire signals, 
  // we have to do the synchronization manually here
  d->Slider->setMinimum(d->SpinBox->minimum());
  d->Slider->setMaximum(d->SpinBox->maximum());
}

// --------------------------------------------------------------------------
QString ctkSliderWidget::prefix()const
{
  Q_D(const ctkSliderWidget);
  return d->SpinBox->prefix();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setPrefix(const QString& newPrefix)
{
  Q_D(ctkSliderWidget);
  d->SpinBox->setPrefix(newPrefix);
#if QT_VERSION < 0x040800
  /// Setting the prefix doesn't recompute the sizehint, do it manually here:
  /// See: http://bugreports.qt.nokia.com/browse/QTBUG-9530
  d->SpinBox->setRange(d->SpinBox->minimum(), d->SpinBox->maximum());
#endif
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
QString ctkSliderWidget::suffix()const
{
  Q_D(const ctkSliderWidget);
  return d->SpinBox->suffix();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setSuffix(const QString& newSuffix)
{
  Q_D(ctkSliderWidget);
  d->SpinBox->setSuffix(newSuffix);
#if QT_VERSION < 0x040800
  /// Setting the suffix doesn't recompute the sizehint, do it manually here:
  /// See: http://bugreports.qt.nokia.com/browse/QTBUG-9530
  d->SpinBox->setRange(d->SpinBox->minimum(), d->SpinBox->maximum());
#endif
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
double ctkSliderWidget::tickInterval()const
{
  Q_D(const ctkSliderWidget);
  return d->Slider->tickInterval();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setTickInterval(double ti)
{ 
  Q_D(ctkSliderWidget);
  d->Slider->setTickInterval(ti);
}

// -------------------------------------------------------------------------
void ctkSliderWidget::reset()
{
  this->setValue(0.);
}

// -------------------------------------------------------------------------
void ctkSliderWidget::setSpinBoxAlignment(Qt::Alignment alignment)
{
  Q_D(ctkSliderWidget);
  return d->SpinBox->setAlignment(alignment);
}

// -------------------------------------------------------------------------
Qt::Alignment ctkSliderWidget::spinBoxAlignment()const
{
  Q_D(const ctkSliderWidget);
  return d->SpinBox->alignment();
}

// -------------------------------------------------------------------------
void ctkSliderWidget::setTracking(bool enable)
{
  Q_D(ctkSliderWidget);
  d->Tracking = enable;
}

// -------------------------------------------------------------------------
bool ctkSliderWidget::hasTracking()const
{
  Q_D(const ctkSliderWidget);
  return d->Tracking;
}

// -------------------------------------------------------------------------
bool ctkSliderWidget::isAutoSpinBoxWidth()const
{
  Q_D(const ctkSliderWidget);
  return d->AutoSpinBoxWidth;
}

// -------------------------------------------------------------------------
void ctkSliderWidget::setAutoSpinBoxWidth(bool autoWidth)
{
  Q_D(ctkSliderWidget);
  d->AutoSpinBoxWidth = autoWidth;
  d->updateSpinBoxWidth();
}
