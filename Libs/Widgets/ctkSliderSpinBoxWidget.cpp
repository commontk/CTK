/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QMouseEvent>

// CTK includes
#include "ctkSliderSpinBoxWidget.h"
#include "ui_ctkSliderSpinBoxWidget.h"

//-----------------------------------------------------------------------------
namespace
{
bool equal(double v1, double v2)
{
  return qAbs(v1 - v2) < 0.0001;
}
}

//-----------------------------------------------------------------------------
class ctkSliderSpinBoxWidgetPrivate: public ctkPrivate<ctkSliderSpinBoxWidget>,
                                     public Ui_ctkSliderSpinBoxWidget
{
public:
  ctkSliderSpinBoxWidgetPrivate();
  void updateSpinBoxWidth();
  int synchronizedSpinBoxWidth()const;
  void synchronizeSiblingSpinBox(int newWidth);

  bool   Tracking;
  bool   Changing;
  double ValueBeforeChange;
  bool   AutoSpinBoxWidth;
};

// --------------------------------------------------------------------------
ctkSliderSpinBoxWidgetPrivate::ctkSliderSpinBoxWidgetPrivate()
{
  this->Tracking = true;
  this->Changing = false;
  this->ValueBeforeChange = 0.;
  this->AutoSpinBoxWidth = true;
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidgetPrivate::updateSpinBoxWidth()
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
int ctkSliderSpinBoxWidgetPrivate::synchronizedSpinBoxWidth()const
{
  CTK_P(const ctkSliderSpinBoxWidget);
  int maxWidth = this->SpinBox->sizeHint().width();
  if (!p->parent())
    {
    return maxWidth;
    }
  QList<ctkSliderSpinBoxWidget*> siblings =
    p->parent()->findChildren<ctkSliderSpinBoxWidget*>();
  foreach(ctkSliderSpinBoxWidget* sibling, siblings)
    {
    maxWidth = qMax(maxWidth, sibling->ctk_d()->SpinBox->sizeHint().width());
    }
  return maxWidth;
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidgetPrivate::synchronizeSiblingSpinBox(int width)
{
  CTK_P(const ctkSliderSpinBoxWidget);
  QList<ctkSliderSpinBoxWidget*> siblings =
    p->parent()->findChildren<ctkSliderSpinBoxWidget*>();
  foreach(ctkSliderSpinBoxWidget* sibling, siblings)
    {
    if (sibling != p && sibling->isAutoSpinBoxWidth())
      {
      sibling->ctk_d()->SpinBox->setMinimumWidth(width);
      }
    }
}

// --------------------------------------------------------------------------
ctkSliderSpinBoxWidget::ctkSliderSpinBoxWidget(QWidget* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkSliderSpinBoxWidget);
  CTK_D(ctkSliderSpinBoxWidget);
  
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
double ctkSliderSpinBoxWidget::minimum()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  Q_ASSERT(equal(d->SpinBox->minimum(),d->Slider->minimum()));
  return d->Slider->minimum();
}

// --------------------------------------------------------------------------
double ctkSliderSpinBoxWidget::maximum()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  Q_ASSERT(equal(d->SpinBox->maximum(),d->Slider->maximum()));
  return d->Slider->maximum();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setMinimum(double min)
{
  CTK_D(ctkSliderSpinBoxWidget);
  d->SpinBox->setMinimum(min);
  // SpinBox can truncate min (depending on decimals).
  // use Spinbox's min to set Slider's min
  d->Slider->setMinimum(d->SpinBox->minimum());
  Q_ASSERT(equal(d->SpinBox->minimum(),d->Slider->minimum()));
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setMaximum(double max)
{
  CTK_D(ctkSliderSpinBoxWidget);
  d->SpinBox->setMaximum(max);
  // SpinBox can truncate max (depending on decimals).
  // use Spinbox's max to set Slider's max
  d->Slider->setMaximum(d->SpinBox->maximum());
  Q_ASSERT(equal(d->SpinBox->maximum(), d->Slider->maximum()));
  d->updateSpinBoxWidth();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setRange(double min, double max)
{
  CTK_D(ctkSliderSpinBoxWidget);
  
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
double ctkSliderSpinBoxWidget::sliderPosition()const
{
  return ctk_d()->Slider->sliderPosition();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setSliderPosition(double position)
{
  ctk_d()->Slider->setSliderPosition(position);
}
*/
/*
// --------------------------------------------------------------------------
double ctkSliderSpinBoxWidget::previousSliderPosition()
{
  return ctk_d()->Slider->previousSliderPosition();
}
*/

// --------------------------------------------------------------------------
double ctkSliderSpinBoxWidget::value()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  Q_ASSERT(equal(d->Slider->value(), d->SpinBox->value()));
  return d->Changing ? d->ValueBeforeChange : d->Slider->value();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setValue(double _value)
{
  CTK_D(ctkSliderSpinBoxWidget);
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
void ctkSliderSpinBoxWidget::startChanging()
{
  CTK_D(ctkSliderSpinBoxWidget);
  if (d->Tracking)
    {
    return;
    }
  d->Changing = true;
  d->ValueBeforeChange = this->value();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::stopChanging()
{
  CTK_D(ctkSliderSpinBoxWidget);
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
void ctkSliderSpinBoxWidget::changeValue(double newValue)
{
  CTK_D(ctkSliderSpinBoxWidget);
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
bool ctkSliderSpinBoxWidget::eventFilter(QObject *obj, QEvent *event)
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
       // here we might prevent ctkSliderSpinBoxWidget::stopChanging
       // from sending a valueChanged() event as the spinbox might
       // send a valueChanged() after eventFilter() is done.
       this->stopChanging();
       }
     } 
   // standard event processing
   return this->Superclass::eventFilter(obj, event);
 }

// --------------------------------------------------------------------------
double ctkSliderSpinBoxWidget::singleStep()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  Q_ASSERT(equal(d->Slider->singleStep(), d->SpinBox->singleStep()));
  return d->Slider->singleStep();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setSingleStep(double step)
{
  CTK_D(ctkSliderSpinBoxWidget);
  d->SpinBox->setSingleStep(step);
  d->Slider->setSingleStep(d->SpinBox->singleStep());
  Q_ASSERT(equal(d->Slider->singleStep(), d->SpinBox->singleStep()));
}

// --------------------------------------------------------------------------
int ctkSliderSpinBoxWidget::decimals()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  return d->SpinBox->decimals();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setDecimals(int newDecimals)
{
  CTK_D(ctkSliderSpinBoxWidget);
  d->SpinBox->setDecimals(newDecimals);
}

// --------------------------------------------------------------------------
QString ctkSliderSpinBoxWidget::prefix()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  return d->SpinBox->prefix();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setPrefix(const QString& newPrefix)
{
  CTK_D(ctkSliderSpinBoxWidget);
  d->SpinBox->setPrefix(newPrefix);
}

// --------------------------------------------------------------------------
QString ctkSliderSpinBoxWidget::suffix()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  return d->SpinBox->suffix();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setSuffix(const QString& newSuffix)
{
  CTK_D(ctkSliderSpinBoxWidget);
  d->SpinBox->setSuffix(newSuffix);
}

// --------------------------------------------------------------------------
double ctkSliderSpinBoxWidget::tickInterval()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  return d->Slider->tickInterval();
}

// --------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setTickInterval(double ti)
{ 
  CTK_D(ctkSliderSpinBoxWidget);
  d->Slider->setTickInterval(ti);
}

// -------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::reset()
{
  this->setValue(0.);
}

// -------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setSpinBoxAlignment(Qt::Alignment alignment)
{
  return ctk_d()->SpinBox->setAlignment(alignment);
}

// -------------------------------------------------------------------------
Qt::Alignment ctkSliderSpinBoxWidget::spinBoxAlignment()const
{
  return ctk_d()->SpinBox->alignment();
}

// -------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setTracking(bool enable)
{
  CTK_D(ctkSliderSpinBoxWidget);
  d->Tracking = enable;
}

// -------------------------------------------------------------------------
bool ctkSliderSpinBoxWidget::hasTracking()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  return d->Tracking;
}

// -------------------------------------------------------------------------
bool ctkSliderSpinBoxWidget::isAutoSpinBoxWidth()const
{
  CTK_D(const ctkSliderSpinBoxWidget);
  return d->AutoSpinBoxWidth;
}

// -------------------------------------------------------------------------
void ctkSliderSpinBoxWidget::setAutoSpinBoxWidth(bool autoWidth)
{
  CTK_D(ctkSliderSpinBoxWidget);
  d->AutoSpinBoxWidth = autoWidth;
  d->updateSpinBoxWidth();
}
