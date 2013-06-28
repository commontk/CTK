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

// CTK includes
#include "ctkDoubleSpinBox.h"
#include "ctkUtils.h"
#include "ctkPimpl.h"

#include <QDebug>

// Qt includes
#include <QDoubleSpinBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QShortcut>
#include <QSizePolicy>
#include <QVariant>

//-----------------------------------------------------------------------------
class ctkDoubleSpinBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkDoubleSpinBox);
protected:
  ctkDoubleSpinBox* const q_ptr;
public:
  ctkDoubleSpinBoxPrivate(ctkDoubleSpinBox& object);

  QDoubleSpinBox* SpinBox;
  ctkDoubleSpinBox::SetMode Mode;
  int DefaultDecimals;
  ctkDoubleSpinBox::DecimalsOptions DOption;

  void init();
  // Compare two double previously rounded according to the number of decimals
  bool compare(double x1, double x2) const;

  // Set the number of decimals of the spinbox and emit the signal
  // No check if they are the same.
  void setDecimals(int dec);
};

//-----------------------------------------------------------------------------
ctkDoubleSpinBoxPrivate::ctkDoubleSpinBoxPrivate(ctkDoubleSpinBox& object) : q_ptr(&object)
{
  qRegisterMetaType<ctkDoubleSpinBox::SetMode>("ctkDoubleSpinBox::SetMode");
  qRegisterMetaType<ctkDoubleSpinBox::DecimalsOptions>("ctkDoubleSpinBox::DecimalsOption");
  this->SpinBox = 0;
  this->Mode = ctkDoubleSpinBox::SetIfDifferent;
  this->DefaultDecimals = 2;
  this->DOption = ctkDoubleSpinBox::UseShortcuts;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::init()
{
  Q_Q(ctkDoubleSpinBox);
  this->SpinBox = new QDoubleSpinBox(q);
  QObject::connect(this->SpinBox, SIGNAL(valueChanged(double)),
    q, SIGNAL(valueChanged(double)));
  QObject::connect(this->SpinBox, SIGNAL(valueChanged(const QString&)),
    q, SIGNAL(valueChanged(const QString &)));
  QObject::connect(this->SpinBox, SIGNAL(editingFinished()),
    q, SIGNAL(editingFinished()));

  QHBoxLayout* l = new QHBoxLayout(q);
  l->addWidget(this->SpinBox);
  l->setContentsMargins(0,0,0,0);
  q->setLayout(l);
  q->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
    QSizePolicy::Fixed, QSizePolicy::ButtonBox));

  this->SpinBox->installEventFilter(q);
}

//-----------------------------------------------------------------------------
bool ctkDoubleSpinBoxPrivate::compare(double x1, double x2) const
{
  Q_Q(const ctkDoubleSpinBox);
  return q->round(x1) == q->round(x2);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::setDecimals(int dec)
{
  Q_Q(ctkDoubleSpinBox);
  this->SpinBox->setDecimals(dec);
  emit q->decimalsChanged(dec);
}

//-----------------------------------------------------------------------------
ctkDoubleSpinBox::ctkDoubleSpinBox(QWidget* newParent)
  : QWidget(newParent)
  , d_ptr(new ctkDoubleSpinBoxPrivate(*this))
{
  Q_D(ctkDoubleSpinBox);
  d->init();
}

//-----------------------------------------------------------------------------
ctkDoubleSpinBox::ctkDoubleSpinBox(ctkDoubleSpinBox::SetMode mode, QWidget* newParent)
  : QWidget(newParent)
  , d_ptr(new ctkDoubleSpinBoxPrivate(*this))
{
  Q_D(ctkDoubleSpinBox);
  d->init();
  this->setSetMode(mode);
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::value() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->value();
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::displayedValue() const
{
  return this->round(this->value());
}

//-----------------------------------------------------------------------------
QString ctkDoubleSpinBox::text() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->text();
}

//-----------------------------------------------------------------------------
QString ctkDoubleSpinBox::cleanText() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->cleanText();
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkDoubleSpinBox::alignment() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->alignment();
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setAlignment(Qt::Alignment flag)
{
  Q_D(const ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent && flag == d->SpinBox->alignment())
    {
    return;
    }

  d->SpinBox->setAlignment(flag);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setFrame(bool frame)
{
  Q_D(const ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent && frame == d->SpinBox->hasFrame())
    {
    return;
    }

  d->SpinBox->setFrame(frame);
}

//-----------------------------------------------------------------------------
bool ctkDoubleSpinBox::hasFrame() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->hasFrame();
}

//-----------------------------------------------------------------------------
QString ctkDoubleSpinBox::prefix() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->prefix();
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setPrefix(const QString &prefix)
{
  Q_D(const ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent && prefix == d->SpinBox->prefix())
    {
    return;
    }

#if QT_VERSION < 0x040800
  /// Setting the prefix doesn't recompute the sizehint, do it manually here:
  /// See: http://bugreports.qt.nokia.com/browse/QTBUG-9530
  d->SpinBox->setRange(d->SpinBox->minimum(), d->SpinBox->maximum());
#endif

  d->SpinBox->setPrefix(prefix);
}

//-----------------------------------------------------------------------------
QString ctkDoubleSpinBox::suffix() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->suffix();
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setSuffix(const QString &suffix)
{
  Q_D(const ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent && suffix == d->SpinBox->suffix())
    {
    return;
    }

#if QT_VERSION < 0x040800
  /// Setting the suffix doesn't recompute the sizehint, do it manually here:
  /// See: http://bugreports.qt.nokia.com/browse/QTBUG-9530
  d->SpinBox->setRange(d->SpinBox->minimum(), d->SpinBox->maximum());
#endif

  d->SpinBox->setSuffix(suffix);
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::singleStep() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->singleStep();
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setSingleStep(double step)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
    && d->compare(step, this->singleStep()))
    {
    return;
    }

  d->SpinBox->setSingleStep(step);
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::minimum() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->minimum();
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setMinimum(double min)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
    && d->compare(min, this->minimum()))
    {
    return;
    }

  d->SpinBox->setMinimum(min);
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::maximum() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->maximum();
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setMaximum(double max)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
    && d->compare(max, this->maximum()))
    {
    return;
    }

  d->SpinBox->setMaximum(max);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setRange(double min, double max)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
    && d->compare(max, this->maximum()) && d->compare(min, this->minimum()))
    {
    return;
    }

  d->SpinBox->setRange(min, max);
}

//-----------------------------------------------------------------------------
int ctkDoubleSpinBox::decimals() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->decimals();
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setDecimals(int dec)
{
  Q_D(ctkDoubleSpinBox);
  dec = qMax(0, dec);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent && dec == this->decimals())
    {
    return;
    }

  d->DefaultDecimals = dec;
  d->setDecimals(d->DefaultDecimals);
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::round(double value) const
{
  Q_D(const ctkDoubleSpinBox);
  return QString::number(value, 'f', d->SpinBox->decimals()).toDouble();
}

//-----------------------------------------------------------------------------
QDoubleSpinBox* ctkDoubleSpinBox::spinBox() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setValue(double value)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent)
    {
    this->setValueIfDifferent(value);
    }
  else
    {
    this->setValueAlways(value);
    }
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setValueIfDifferent(double value)
{
  Q_D(ctkDoubleSpinBox);
  if (! d->compare(this->value(), value))
    {
    d->SpinBox->setValue(value);
    }
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setValueAlways(double value)
{
  Q_D(const ctkDoubleSpinBox);
  d->SpinBox->setValue(value);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::stepUp()
{
  Q_D(const ctkDoubleSpinBox);
  d->SpinBox->stepUp();
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::stepDown()
{
  Q_D(const ctkDoubleSpinBox);
  d->SpinBox->stepDown();
}

//-----------------------------------------------------------------------------
ctkDoubleSpinBox::SetMode ctkDoubleSpinBox::setMode() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->Mode;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setSetMode(ctkDoubleSpinBox::SetMode newMode)
{
  Q_D(ctkDoubleSpinBox);
  d->Mode = newMode;
}

//-----------------------------------------------------------------------------
ctkDoubleSpinBox::DecimalsOptions ctkDoubleSpinBox::decimalsOption()
{
  Q_D(const ctkDoubleSpinBox);
  return d->DOption;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setDecimalsOption(ctkDoubleSpinBox::DecimalsOptions option)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent && option == d->DOption)
    {
    return;
    }

  d->DOption = option;
}

//-----------------------------------------------------------------------------
bool ctkDoubleSpinBox::eventFilter(QObject* obj, QEvent* event)
{
  Q_D(ctkDoubleSpinBox);
  if (d->DOption & ctkDoubleSpinBox::UseShortcuts &&
    obj == d->SpinBox && event->type() == QEvent::KeyPress)
    {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    Q_ASSERT(keyEvent);
    if (keyEvent->modifiers() & Qt::ControlModifier)
      {
      if (keyEvent->key() == Qt::Key_Plus
        || keyEvent->key() == Qt::Key_Equal)
        {
        d->setDecimals(this->decimals() + 1);
        return true;
        }
      else if (keyEvent->key() == Qt::Key_Minus)
        {
        d->setDecimals(this->decimals() - 1);
        return true;
        }
      else if (keyEvent->key() == Qt::Key_0)
        {
        d->setDecimals(d->DefaultDecimals);
        return true;
        }
      }

    return QWidget::eventFilter(obj, event);
    }
  else
    {
    // pass the event on to the parent class
    return QWidget::eventFilter(obj, event);
    }
}
