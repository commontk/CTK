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
class ctkSliderWidgetPrivate: public ctkPrivate<ctkSliderWidget>,
                                     public Ui_ctkSliderWidget
{
public:
  ctkSliderWidgetPrivate();
  void updateSpinBoxWidth();
  int synchronizedSpinBoxWidth()const;
  void synchronizeSiblingSpinBox(int newWidth);

  bool   Tracking;
  bool   Changing;
  double ValueBeforeChange;
  bool   AutoSpinBoxWidth;
};

// --------------------------------------------------------------------------
ctkSliderWidgetPrivate::ctkSliderWidgetPrivate()
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
  CTK_P(const ctkSliderWidget);
  int maxWidth = this->SpinBox->sizeHint().width();
  if (!p->parent())
    {
    return maxWidth;
    }
  QList<ctkSliderWidget*> siblings =
    p->parent()->findChildren<ctkSliderWidget*>();
  foreach(ctkSliderWidget* sibling, siblings)
    {
    maxWidth = qMax(maxWidth, sibling->ctk_d()->SpinBox->sizeHint().width());
    }
  return maxWidth;
}

// --------------------------------------------------------------------------
void ctkSliderWidgetPrivate::synchronizeSiblingSpinBox(int width)
{
  CTK_P(const ctkSliderWidget);
  QList<ctkSliderWidget*> siblings =
    p->parent()->findChildren<ctkSliderWidget*>();
  foreach(ctkSliderWidget* sibling, siblings)
    {
    if (sibling != p && sibling->isAutoSpinBoxWidth())
      {
      sibling->ctk_d()->SpinBox->setMinimumWidth(width);
      }
    }
}

// --------------------------------------------------------------------------
ctkSliderWidget::ctkSliderWidget(QWidget* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkSliderWidget);
  CTK_D(ctkSliderWidget);
  
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
double ctkSliderWidget::minimum()const
{
  CTK_D(const ctkSliderWidget);
  Q_ASSERT(equal(d->SpinBox->minimum(),d->Slider->minimum()));
  return d->Slider->minimum();
}

// --------------------------------------------------------------------------
double ctkSliderWidget::maximum()const
{
  CTK_D(const ctkSliderWidget);
  Q_ASSERT(equal(d->SpinBox->maximum(),d->Slider->maximum()));
  return d->Slider->maximum();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setMinimum(double min)
{
  CTK_D(ctkSliderWidget);
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
  CTK_D(ctkSliderWidget);
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
  CTK_D(ctkSliderWidget);
  
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
  return ctk_d()->Slider->sliderPosition();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setSliderPosition(double position)
{
  ctk_d()->Slider->setSliderPosition(position);
}
*/
/*
// --------------------------------------------------------------------------
double ctkSliderWidget::previousSliderPosition()
{
  return ctk_d()->Slider->previousSliderPosition();
}
*/

// --------------------------------------------------------------------------
double ctkSliderWidget::value()const
{
  CTK_D(const ctkSliderWidget);
  Q_ASSERT(equal(d->Slider->value(), d->SpinBox->value()));
  return d->Changing ? d->ValueBeforeChange : d->Slider->value();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setValue(double _value)
{
  CTK_D(ctkSliderWidget);
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
  CTK_D(ctkSliderWidget);
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
  CTK_D(ctkSliderWidget);
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
  CTK_D(ctkSliderWidget);
  //if (d->Tracking)
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
  CTK_D(const ctkSliderWidget);
  Q_ASSERT(equal(d->Slider->singleStep(), d->SpinBox->singleStep()));
  return d->Slider->singleStep();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setSingleStep(double step)
{
  CTK_D(ctkSliderWidget);
  d->SpinBox->setSingleStep(step);
  d->Slider->setSingleStep(d->SpinBox->singleStep());
  Q_ASSERT(equal(d->Slider->singleStep(), d->SpinBox->singleStep()));
}

// --------------------------------------------------------------------------
int ctkSliderWidget::decimals()const
{
  CTK_D(const ctkSliderWidget);
  return d->SpinBox->decimals();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setDecimals(int newDecimals)
{
  CTK_D(ctkSliderWidget);
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
  CTK_D(const ctkSliderWidget);
  return d->SpinBox->prefix();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setPrefix(const QString& newPrefix)
{
  CTK_D(ctkSliderWidget);
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
  CTK_D(const ctkSliderWidget);
  return d->SpinBox->suffix();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setSuffix(const QString& newSuffix)
{
  CTK_D(ctkSliderWidget);
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
  CTK_D(const ctkSliderWidget);
  return d->Slider->tickInterval();
}

// --------------------------------------------------------------------------
void ctkSliderWidget::setTickInterval(double ti)
{ 
  CTK_D(ctkSliderWidget);
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
  return ctk_d()->SpinBox->setAlignment(alignment);
}

// -------------------------------------------------------------------------
Qt::Alignment ctkSliderWidget::spinBoxAlignment()const
{
  return ctk_d()->SpinBox->alignment();
}

// -------------------------------------------------------------------------
void ctkSliderWidget::setTracking(bool enable)
{
  CTK_D(ctkSliderWidget);
  d->Tracking = enable;
}

// -------------------------------------------------------------------------
bool ctkSliderWidget::hasTracking()const
{
  CTK_D(const ctkSliderWidget);
  return d->Tracking;
}

// -------------------------------------------------------------------------
bool ctkSliderWidget::isAutoSpinBoxWidth()const
{
  CTK_D(const ctkSliderWidget);
  return d->AutoSpinBoxWidth;
}

// -------------------------------------------------------------------------
void ctkSliderWidget::setAutoSpinBoxWidth(bool autoWidth)
{
  CTK_D(ctkSliderWidget);
  d->AutoSpinBoxWidth = autoWidth;
  d->updateSpinBoxWidth();
}
