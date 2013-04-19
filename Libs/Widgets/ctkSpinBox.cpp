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
#include "ctkSpinBox.h"
#include "ctkUtils.h"
#include "ctkPimpl.h"

#include <QDebug>

// Qt includes
#include <QDoubleSpinBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QShortcut>
#include <QSizePolicy>
#include <QVariant>

//-----------------------------------------------------------------------------
class ctkSpinBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkSpinBox);
protected:
  ctkSpinBox* const q_ptr;
public:
  ctkSpinBoxPrivate(ctkSpinBox& object);

  QDoubleSpinBox* SpinBox;
  ctkSpinBox::SetMode Mode;

  void init();
  // Compare two double previously rounded according to the number of decimals
  bool compare(double x1, double x2) const;
};

//-----------------------------------------------------------------------------
ctkSpinBoxPrivate::ctkSpinBoxPrivate(ctkSpinBox& object) : q_ptr(&object)
{
  qRegisterMetaType<ctkSpinBox::SetMode>("ctkSpinBox::SetMode");
  this->SpinBox = 0;
  this->Mode = ctkSpinBox::SetIfDifferent;
}

//-----------------------------------------------------------------------------
void ctkSpinBoxPrivate::init()
{
  Q_Q(ctkSpinBox);
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
}

//-----------------------------------------------------------------------------
bool ctkSpinBoxPrivate::compare(double x1, double x2) const
{
  Q_Q(const ctkSpinBox);
  return q->round(x1) == q->round(x2);
}

//-----------------------------------------------------------------------------
ctkSpinBox::ctkSpinBox(QWidget* newParent)
  : QWidget(newParent)
  , d_ptr(new ctkSpinBoxPrivate(*this))
{
  Q_D(ctkSpinBox);
  d->init();
}

//-----------------------------------------------------------------------------
ctkSpinBox::ctkSpinBox(ctkSpinBox::SetMode mode, QWidget* newParent)
  : QWidget(newParent)
  , d_ptr(new ctkSpinBoxPrivate(*this))
{
  Q_D(ctkSpinBox);
  d->init();
  this->setSetMode(mode);
}

//-----------------------------------------------------------------------------
double ctkSpinBox::value() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->value();
}

//-----------------------------------------------------------------------------
double ctkSpinBox::displayedValue() const
{
  Q_D(const ctkSpinBox);
  return this->round(this->value());
}

//-----------------------------------------------------------------------------
QString ctkSpinBox::text() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->text();
}

//-----------------------------------------------------------------------------
QString ctkSpinBox::cleanText() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->cleanText();
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkSpinBox::alignment() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->alignment();
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setAlignment(Qt::Alignment flag)
{
  Q_D(const ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent && flag == d->SpinBox->alignment())
    {
    return;
    }

  d->SpinBox->setAlignment(flag);
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setFrame(bool frame)
{
  Q_D(const ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent && frame == d->SpinBox->hasFrame())
    {
    return;
    }

  d->SpinBox->setFrame(frame);
}

//-----------------------------------------------------------------------------
bool ctkSpinBox::hasFrame() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->hasFrame();
}

//-----------------------------------------------------------------------------
QString ctkSpinBox::prefix() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->prefix();
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setPrefix(const QString &prefix)
{
  Q_D(const ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent && prefix == d->SpinBox->prefix())
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
QString ctkSpinBox::suffix() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->suffix();
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setSuffix(const QString &suffix)
{
  Q_D(const ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent && suffix == d->SpinBox->suffix())
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
double ctkSpinBox::singleStep() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->singleStep();
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setSingleStep(double step)
{
  Q_D(ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent
    && d->compare(step, this->singleStep()))
    {
    return;
    }

  d->SpinBox->setSingleStep(step);
}

//-----------------------------------------------------------------------------
double ctkSpinBox::minimum() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->minimum();
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setMinimum(double min)
{
  Q_D(ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent
    && d->compare(min, this->minimum()))
    {
    return;
    }

  d->SpinBox->setMinimum(min);
}

//-----------------------------------------------------------------------------
double ctkSpinBox::maximum() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->maximum();
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setMaximum(double max)
{
  Q_D(ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent
    && d->compare(max, this->maximum()))
    {
    return;
    }

  d->SpinBox->setMaximum(max);
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setRange(double min, double max)
{
  Q_D(ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent
    && d->compare(max, this->maximum()) && d->compare(min, this->minimum()))
    {
    return;
    }

  d->SpinBox->setRange(min, max);
}

//-----------------------------------------------------------------------------
int ctkSpinBox::decimals() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox->decimals();
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setDecimals(int dec)
{
  Q_D(const ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent && dec == d->SpinBox->decimals())
    {
    return;
    }

  d->SpinBox->setDecimals(dec);
}

//-----------------------------------------------------------------------------
double ctkSpinBox::round(double value) const
{
  Q_D(const ctkSpinBox);
  return QString::number(value, 'f', d->SpinBox->decimals()).toDouble();
}

//-----------------------------------------------------------------------------
QDoubleSpinBox* ctkSpinBox::spinBox() const
{
  Q_D(const ctkSpinBox);
  return d->SpinBox;
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setValue(double value)
{
  Q_D(const ctkSpinBox);
  if (d->Mode == ctkSpinBox::SetIfDifferent)
    {
    this->setValueIfDifferent(value);
    }
  else
    {
    this->setValueAlways(value);
    }
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setValueIfDifferent(double value)
{
  Q_D(ctkSpinBox);
  if (! d->compare(this->value(), value))
    {
    d->SpinBox->setValue(value);
    }
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setValueAlways(double value)
{
  Q_D(const ctkSpinBox);
  d->SpinBox->setValue(value);
}

//-----------------------------------------------------------------------------
void ctkSpinBox::stepUp()
{
  Q_D(const ctkSpinBox);
  d->SpinBox->stepUp();
}

//-----------------------------------------------------------------------------
void ctkSpinBox::stepDown()
{
  Q_D(const ctkSpinBox);
  d->SpinBox->stepDown();
}

//-----------------------------------------------------------------------------
ctkSpinBox::SetMode ctkSpinBox::setMode() const
{
  Q_D(const ctkSpinBox);
  return d->Mode;
}

//-----------------------------------------------------------------------------
void ctkSpinBox::setSetMode(ctkSpinBox::SetMode newMode)
{
  Q_D(ctkSpinBox);
  d->Mode = newMode;
}

