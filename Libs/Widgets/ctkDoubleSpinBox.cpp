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
#include "ctkDoubleSpinBox_p.h"
#include "ctkUtils.h"
#include "ctkValueProxy.h"
#include "ctkPimpl.h"

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QShortcut>
#include <QSizePolicy>
#include <QStyle>
#include <QStyleOptionSpinBox>
#include <QVariant>

//-----------------------------------------------------------------------------
// ctkQDoubleSpinBox
//----------------------------------------------------------------------------
ctkQDoubleSpinBox::ctkQDoubleSpinBox(ctkDoubleSpinBoxPrivate* pimpl,
                                     QWidget* spinBoxParent)
  : QDoubleSpinBox(spinBoxParent)
  , d_ptr(pimpl)
{
  this->InvertedControls = false;
}

//----------------------------------------------------------------------------
QLineEdit* ctkQDoubleSpinBox::lineEdit()const
{
  return this->QDoubleSpinBox::lineEdit();
}
//----------------------------------------------------------------------------
void ctkQDoubleSpinBox::initStyleOptionSpinBox(QStyleOptionSpinBox* option)
{
  this->initStyleOption(option);
}

//----------------------------------------------------------------------------
void ctkQDoubleSpinBox::setInvertedControls(bool invertedControls)
{
  this->InvertedControls = invertedControls;
}

//----------------------------------------------------------------------------
bool ctkQDoubleSpinBox::invertedControls() const
{
  return this->InvertedControls;
}

//----------------------------------------------------------------------------
void ctkQDoubleSpinBox::stepBy(int steps)
{
  if (this->InvertedControls)
    {
    steps = -steps;
    }
  this->Superclass::stepBy(steps);
}

//----------------------------------------------------------------------------
QAbstractSpinBox::StepEnabled ctkQDoubleSpinBox::stepEnabled() const
{
  if (!this->InvertedControls)
    {
    return this->Superclass::stepEnabled();
    }

  if (this->isReadOnly())
    {
    return StepNone;
    }

  if (this->wrapping())
    {
    return StepEnabled(StepUpEnabled | StepDownEnabled);
    }

  StepEnabled ret = StepNone;
  double value = this->value();
  if (value < this->maximum())
    {
    ret |= StepDownEnabled;
    }
  if (value > this->minimum())
    {
    ret |= StepUpEnabled;
    }
  return ret;
}

//-----------------------------------------------------------------------------
double ctkQDoubleSpinBox::valueFromText(const QString &text) const
{
  Q_D(const ctkDoubleSpinBox);

  QString copy = text;
  int pos = this->lineEdit()->cursorPosition();
  QValidator::State state = QValidator::Acceptable;
  int decimals = 0;
  double value = d->validateAndInterpret(copy, pos, state, decimals);
  return value;
}

//-----------------------------------------------------------------------------
QString ctkQDoubleSpinBox::textFromValue(double value) const
{
  Q_D(const ctkDoubleSpinBox);
  QString text = this->QDoubleSpinBox::textFromValue(value);
  if (text.isEmpty())
    {
    text = "0";
    }
  // If there is no decimal, it does not mean there won't be any.
  if (d->DOption & ctkDoubleSpinBox::DecimalPointAlwaysVisible &&
      text.indexOf(this->locale().decimalPoint()) == -1)
    {
    text += this->locale().decimalPoint();
    }
  return text;
}

//-----------------------------------------------------------------------------
int ctkQDoubleSpinBox::decimalsFromText(const QString &text) const
{
  Q_D(const ctkDoubleSpinBox);

  QString copy = text;
  int pos = this->lineEdit()->cursorPosition();
  int decimals = 0;
  QValidator::State state = QValidator::Acceptable;
  d->validateAndInterpret(copy, pos, state, decimals);
  return decimals;
}

//-----------------------------------------------------------------------------
QValidator::State ctkQDoubleSpinBox::validate(QString &text, int &pos) const
{
  Q_D(const ctkDoubleSpinBox);

  QValidator::State state = QValidator::Acceptable;
  int decimals = 0;
  d->validateAndInterpret(text, pos, state, decimals);
  return state;
}

//-----------------------------------------------------------------------------
// ctkDoubleSpinBoxPrivate
//-----------------------------------------------------------------------------
ctkDoubleSpinBoxPrivate::ctkDoubleSpinBoxPrivate(ctkDoubleSpinBox& object)
  : q_ptr(&object)
{
  qRegisterMetaType<ctkDoubleSpinBox::SetMode>("ctkDoubleSpinBox::SetMode");
  qRegisterMetaType<ctkDoubleSpinBox::DecimalsOptions>("ctkDoubleSpinBox::DecimalsOption");
  this->SpinBox = 0;
  this->Mode = ctkDoubleSpinBox::SetIfDifferent;
  this->DefaultDecimals = 2;
  // InsertDecimals is not a great default, but it is QDoubleSpinBox's default.
  this->DOption = ctkDoubleSpinBox::DecimalsByShortcuts
    | ctkDoubleSpinBox::InsertDecimals;
  this->InvertedControls = false;
  this->SizeHintPolicy = ctkDoubleSpinBox::SizeHintByMinMax;
  this->InputValue = 0.;
  this->InputRange[0] = 0.;
  this->InputRange[1] = 99.99;
  this->ForceInputValueUpdate = false;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::init()
{
  Q_Q(ctkDoubleSpinBox);
  this->SpinBox = new ctkQDoubleSpinBox(this, q);
  this->SpinBox->setInvertedControls(this->InvertedControls);
  // ctkQDoubleSpinBox needs to be first to receive textChanged() signals.
  QLineEdit* lineEdit = new QLineEdit(q);
  QObject::connect(lineEdit, SIGNAL(textChanged(QString)),
                   this, SLOT(editorTextChanged(QString)));
  this->SpinBox->setLineEdit(lineEdit);
  lineEdit->setObjectName(QLatin1String("qt_spinbox_lineedit"));
  this->InputValue = this->SpinBox->value();
  this->InputRange[0] = this->SpinBox->minimum();
  this->InputRange[1] = this->SpinBox->maximum();

  QObject::connect(this->SpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onValueChanged()));
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
double ctkDoubleSpinBoxPrivate::round(double value, int decimals) const
{
  return QString::number(value, 'f', decimals).toDouble();
}

//-----------------------------------------------------------------------------
QString ctkDoubleSpinBoxPrivate::stripped(const QString& text, int* pos) const
{
  Q_Q(const ctkDoubleSpinBox);
  QString strip(text);
  if (strip.startsWith(q->prefix()))
    {
    strip.remove(0, q->prefix().size());
    }
  if (strip.endsWith(q->suffix()))
    {
    strip.chop(q->suffix().size());
    }
  strip = strip.trimmed();
  if (pos)
    {
    int stripInText = text.indexOf(strip);
    *pos = qBound(0, *pos - stripInText, strip.size());
    }
  return strip;
}

//-----------------------------------------------------------------------------
int ctkDoubleSpinBoxPrivate::boundDecimals(int dec)const
{
  Q_Q(const ctkDoubleSpinBox);
  if (dec == -1)
    {
    return q->decimals();
    }
  int min = (this->DOption & ctkDoubleSpinBox::DecimalsAsMin) ?
    this->DefaultDecimals : 0;
  int max = (this->DOption & ctkDoubleSpinBox::DecimalsAsMax) ?
    this->DefaultDecimals : 323; // see QDoubleSpinBox::decimals doc
  return qBound(min, dec, max);
}

//-----------------------------------------------------------------------------
int ctkDoubleSpinBoxPrivate::decimalsForValue(double value) const
{
  int decimals = this->DefaultDecimals;
  if (this->DOption & ctkDoubleSpinBox::DecimalsByValue)
    {
    decimals = ctk::significantDecimals(value, decimals);
    }
  return this->boundDecimals(decimals);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::setValue(double value, int dec)
{
  Q_Q(ctkDoubleSpinBox);
  dec = this->boundDecimals(dec);
  const bool changeDecimals = dec != q->decimals();
  if (changeDecimals)
    {
    // don't fire valueChanged signal because we will change the value
    // right after anyway.
    const bool blockValueChangedSignal = (this->round(this->SpinBox->value(), dec) != value);
    bool wasBlocked = false;
    if (blockValueChangedSignal)
      {
      wasBlocked = this->SpinBox->blockSignals(true);
      }
    // don't fire decimalsChanged signal yet, wait for the value to be
    // up-to-date.
    this->SpinBox->setDecimals(dec);
    if (blockValueChangedSignal)
      {
      this->SpinBox->blockSignals(wasBlocked);
      }
    }
  this->SpinBox->setValue(value); // re-do the text (calls textFromValue())
  if (changeDecimals)
    {
    emit q->decimalsChanged(dec);
    }
  if (this->SizeHintPolicy == ctkDoubleSpinBox::SizeHintByValue)
    {
    this->CachedSizeHint = QSize();
    this->CachedMinimumSizeHint = QSize();
    q->updateGeometry();
    }
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::setDecimals(int dec)
{
  Q_Q(ctkDoubleSpinBox);
  dec = this->boundDecimals(dec);
  this->SpinBox->setDecimals(dec);
  emit q->decimalsChanged(dec);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::editorTextChanged(const QString& text)
{
  if (this->SpinBox->keyboardTracking())
    {
    QString tmp = text;
    int pos = this->SpinBox->lineEdit()->cursorPosition();
    QValidator::State state = QValidator::Invalid;
    int decimals = 0;
    this->validateAndInterpret(tmp, pos, state, decimals);
    if (state == QValidator::Acceptable)
      {
      double newValue = this->SpinBox->valueFromText(tmp);
      int decimals = this->boundDecimals(this->SpinBox->decimalsFromText(tmp));
      bool changeDecimals = this->DOption & ctkDoubleSpinBox::DecimalsByKey &&
        decimals != this->SpinBox->decimals();
      if (changeDecimals)
        {
        this->ForceInputValueUpdate = true;
        this->setValue(newValue, decimals);
        this->ForceInputValueUpdate = false;
        }
      // else, let QDoubleSpinBox process the validation.
      }
    }
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBoxPrivate
::validateAndInterpret(QString &input, int &pos,
                       QValidator::State &state, int &decimals) const
{
  Q_Q(const ctkDoubleSpinBox);
  if (this->CachedText == input)
    {
    state = this->CachedState;
    decimals = this->CachedDecimals;
    return this->CachedValue;
    }
  const double max = this->SpinBox->maximum();
  const double min = this->SpinBox->minimum();

  int posInValue = pos;
  QString text = this->stripped(input, &posInValue);
  // posInValue can change, track the offset.
  const int oldPosInValue = posInValue;
  state = QValidator::Acceptable;
  decimals = 0;

  double value = min;
  const int dec = text.indexOf(q->locale().decimalPoint());

  bool ok = false;
  value = q->locale().toDouble(text, &ok);

  // could be in an intermediate state
  if (!ok  && state == QValidator::Acceptable)
    {
    if (text.isEmpty() ||
        text == "." ||
        text == "-" ||
        text == "+" ||
        text == "-." ||
        text == "+.")
      {
      state = QValidator::Intermediate;
      }
    }
  // could be because of group separators:
  if (!ok && state == QValidator::Acceptable)
    {
    if (q->locale().groupSeparator().isPrint())
      {
      int start = (dec == -1 ? text.size() : dec)- 1;
      int lastGroupSeparator = start;
      for (int digit = start; digit >= 0; --digit)
        {
        if (text.at(digit) == q->locale().groupSeparator())
          {
          if (digit != lastGroupSeparator - 3)
            {
            state = QValidator::Invalid;
            break;
            }
          text.remove(digit, 1);
          lastGroupSeparator = digit;
          }
        }
      }
    // try again without the group separators
    value = q->locale().toDouble(text, &ok);
    }
  // test the decimalPoint
  if (!ok && state == QValidator::Acceptable)
    {
    // duplicate decimal points probably means the user typed another decimal points,
    // move the cursor pos to the right then
    if (dec + 1 < text.size() &&
        text.at(dec + 1) == q->locale().decimalPoint() &&
        posInValue == dec + 1)
      {
      text.remove(dec + 1, 1);
      value = q->locale().toDouble(text, &ok);
      }
    }
  if (ok && state != QValidator::Invalid)
    {
    if (dec != -1)
      {
      decimals = text.size() - (dec + 1);
      if (decimals > q->decimals())
        {
        // With ReplaceDecimals on, key strokes replace decimal digits
        if (posInValue > dec && posInValue < text.size())
          {
          const int extraDecimals = decimals - q->decimals();
          if (this->DOption & ctkDoubleSpinBox::ReplaceDecimals)
            {
            text.remove(posInValue, extraDecimals);
            decimals = q->decimals();
            value = q->locale().toDouble(text, &ok);
            }
          else if (!(this->DOption & ctkDoubleSpinBox::InsertDecimals))
            {
            text.remove(text.size() - extraDecimals, extraDecimals);
            decimals = q->decimals();
            value = q->locale().toDouble(text, &ok);
            }
          }
        }
      // When DecimalsByKey is set, it is possible to extend the number of decimals
      if (decimals > q->decimals() &&
          !(this->DOption & ctkDoubleSpinBox::DecimalsByKey) )
        {
        state = QValidator::Invalid;
        }
      }
    }
  if (state == QValidator::Acceptable)
    {
    if (!ok)
      {
      state = QValidator::Invalid;
      }
    else if (value >= min && value <= max)
      {
      state = QValidator::Acceptable;
      }
    else if (max == min)
      { // when max and min is the same the only non-Invalid input is max (or min)
      state = QValidator::Invalid;
      }
    else if ((value >= 0 && value > max) || (value < 0 && value < min))
      {
      state = QValidator::Invalid;
      }
    else
      {
      state = QValidator::Intermediate;
      }
    }

  if (state != QValidator::Acceptable)
    {
    value = max > 0 ? min : max;
    }

  pos += posInValue - oldPosInValue;
  input = q->prefix() + text + q->suffix();
  this->CachedText = input;
  this->CachedState = state;
  this->CachedValue = value;
  this->CachedDecimals = decimals;
  return value;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::onValueChanged()
{
  Q_Q(ctkDoubleSpinBox);
  double newValue = this->SpinBox->value();
  double oldValue = q->value();
  if (this->Proxy)
    {
    oldValue = this->Proxy.data()->proxyValueFromValue(oldValue);
    }
  // Don't trigger value changed signal if the difference only happened on the
  // precision.
  if (this->compare(oldValue, newValue) && !this->ForceInputValueUpdate)
    {
    return;
    }
  // Force it only once (when the user typed a new number that could have change
  // the number of decimals which could have make the compare test always pass.
  this->ForceInputValueUpdate = false;

  double minimum = q->minimum();
  double maximum = q->maximum();
  if (this->Proxy)
    {
    minimum = this->Proxy.data()->proxyValueFromValue(minimum);
    maximum = this->Proxy.data()->proxyValueFromValue(maximum);
    }
  // Special case to return max precision
  if (this->compare(minimum, newValue))
    {
    newValue = q->minimum();
    }
  else if (this->compare(maximum, newValue))
    {
    newValue = q->maximum();
    }
  else if (this->Proxy)
    {
    newValue = this->Proxy.data()->valueFromProxyValue(newValue);
    }
  this->InputValue = newValue;
  emit q->valueChanged(newValue);
  // \tbd The string might not make much sense when using proxies.
  emit q->valueChanged(
    QString::number(newValue, 'f', this->SpinBox->decimals()));
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::onValueProxyAboutToBeModified()
{
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::onValueProxyModified()
{
  Q_Q(ctkDoubleSpinBox);
  int oldDecimals = q->decimals();
  double oldValue = this->InputValue;
  ctkDoubleSpinBox::SetMode oldSetMode = this->Mode;

  // Only the display is changed, not the programatic value, no need to trigger
  // signals
  bool wasBlocking = q->blockSignals(true);
  // Enforce a refresh. Signals are blocked so it should not trigger unwanted
  // signals
  this->Mode = ctkDoubleSpinBox::SetAlways;
  q->setRange(this->InputRange[0], this->InputRange[1]);
  q->setValue(oldValue);
  this->Mode = oldSetMode;
  q->blockSignals(wasBlocking);
  // Decimals might change when value proxy is modified.
  if (oldDecimals != q->decimals())
    {
    emit q->decimalsChanged(q->decimals());
    }
}

//-----------------------------------------------------------------------------
// ctkDoubleSpinBox
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
ctkDoubleSpinBox::~ctkDoubleSpinBox()
{
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::value() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->InputValue;
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::displayedValue() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->value();
}

//----------------------------------------------------------------------------
void ctkDoubleSpinBox::setDisplayedValue(double value)
{
  Q_D(ctkDoubleSpinBox);
  d->SpinBox->setValue(value);
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
void ctkDoubleSpinBox::setReadOnly(bool readOnly)
{
  Q_D(const ctkDoubleSpinBox);
  d->SpinBox->setReadOnly(readOnly);
  d->SpinBox->setButtonSymbols(readOnly ? QAbstractSpinBox::NoButtons : QAbstractSpinBox::UpDownArrows);
}

//-----------------------------------------------------------------------------
bool ctkDoubleSpinBox::isReadOnly() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->isReadOnly();
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
  double step = d->SpinBox->singleStep();
  return step;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setSingleStep(double newStep)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
    && d->compare(newStep, this->singleStep()))
    {
    return;
    }

  d->SpinBox->setSingleStep(newStep);
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::minimum() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->InputRange[0];
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setMinimum(double newMin)
{
  this->setRange(newMin, qMax(newMin, this->maximum()));
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::maximum() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->InputRange[1];
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setMaximum(double newMax)
{
  this->setRange(qMin(newMax, this->minimum()), newMax);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setRange(double newMin, double newMax)
{
  Q_D(ctkDoubleSpinBox);
  if (newMin > newMax)
    {
    qSwap(newMin, newMax);
    }
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
      && newMin == d->InputRange[0]
      && newMax == d->InputRange[1])
    {
    return;
    }
  d->InputRange[0] = newMin;
  d->InputRange[1] = newMax;
  if (d->Proxy)
    {
    newMin = d->Proxy.data()->proxyValueFromValue(newMin);
    newMax = d->Proxy.data()->proxyValueFromValue(newMax);
    if (newMin > newMax)
      {
      qSwap(newMin, newMax);
      }
    }

  d->SpinBox->setRange(newMin, newMax);
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
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
      && dec == this->decimals()
      && dec == d->DefaultDecimals)
    {
    return;
    }

  d->DefaultDecimals = dec;
  // The number of decimals may or may not depend on the value. Recompute the
  // new number of decimals.
  double currentValue = this->value();
  if (d->Proxy)
    {
    currentValue = d->Proxy.data()->proxyValueFromValue(currentValue);
    }
  int newDecimals = d->decimalsForValue(currentValue);
  d->setValue(currentValue, newDecimals);
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
QLineEdit* ctkDoubleSpinBox::lineEdit() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SpinBox->lineEdit();
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
void ctkDoubleSpinBox::setValueIfDifferent(double newValue)
{
  Q_D(ctkDoubleSpinBox);
  if (newValue == d->InputValue)
    {
    return;
    }
  this->setValueAlways(newValue);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setValueAlways(double newValue)
{
  Q_D(ctkDoubleSpinBox);
  newValue = qBound(d->InputRange[0], newValue, d->InputRange[1]);
  const bool valueModified = d->InputValue != newValue;
  d->InputValue = newValue;
  double newValueToDisplay = newValue;
  if (d->Proxy)
    {
    newValueToDisplay = d->Proxy.data()->proxyValueFromValue(newValueToDisplay);
    }
  const int decimals = d->decimalsForValue(newValueToDisplay);
  // setValueAlways already fires the valueChanged() signal if needed, same
  // thing for d->setValue() with decimalsChanged(). There is no need to
  // propagate the valueChanged/decimalsChanged signals from the spinbox.
  // Alternatively we could also have set a flag that prevents onValueChanged()
  // to trigger the valueChanged() signal.
  //bool wasBlocking = d->SpinBox->blockSignals(true);
  d->setValue(newValueToDisplay, decimals);
  //d->SpinBox->blockSignals(wasBlocking);
  const bool signalsEmitted = (newValue != d->InputValue);
  // Fire the valueChanged signal only if d->setValue() did not fire it
  // already..
  if (valueModified && !signalsEmitted)
    {
    emit valueChanged(d->InputValue);
    emit valueChanged(QString::number(d->InputValue, 'f', d->SpinBox->decimals()));
    }
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
  this->setValueAlways(this->value());
}

//----------------------------------------------------------------------------
void ctkDoubleSpinBox::setInvertedControls(bool invertedControls)
{
  Q_D(ctkDoubleSpinBox);
  d->InvertedControls = invertedControls;
  d->SpinBox->setInvertedControls(d->InvertedControls);
}

//----------------------------------------------------------------------------
bool ctkDoubleSpinBox::invertedControls() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->InvertedControls;
}

//----------------------------------------------------------------------------
void ctkDoubleSpinBox
::setSizeHintPolicy(ctkDoubleSpinBox::SizeHintPolicy newSizeHintPolicy)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
      && newSizeHintPolicy == d->SizeHintPolicy)
    {
    return;
    }
  d->SizeHintPolicy = newSizeHintPolicy;
  d->CachedSizeHint = QSize();
  d->CachedMinimumSizeHint = QSize();
  this->updateGeometry();
}

//----------------------------------------------------------------------------
ctkDoubleSpinBox::SizeHintPolicy ctkDoubleSpinBox::sizeHintPolicy() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->SizeHintPolicy;
}

//----------------------------------------------------------------------------
void ctkDoubleSpinBox::setValueProxy(ctkValueProxy* proxy)
{
  Q_D(ctkDoubleSpinBox);
  if (proxy == d->Proxy.data())
    {
    return;
    }

  d->onValueProxyAboutToBeModified();

  if (d->Proxy)
    {
    disconnect(d->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
               d, SLOT(onValueProxyAboutToBeModified()));
    disconnect(d->Proxy.data(), SIGNAL(proxyModified()),
               d, SLOT(onValueProxyModified()));
    }

  d->Proxy = proxy;

  if (d->Proxy)
    {
    connect(d->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
            d, SLOT(onValueProxyAboutToBeModified()));
    connect(d->Proxy.data(), SIGNAL(proxyModified()),
            d, SLOT(onValueProxyModified()));
    }

  d->onValueProxyModified();
}

//----------------------------------------------------------------------------
ctkValueProxy* ctkDoubleSpinBox::valueProxy() const
{
  Q_D(const ctkDoubleSpinBox);
  return d->Proxy.data();
}

//----------------------------------------------------------------------------
QSize ctkDoubleSpinBox::sizeHint() const
{
  Q_D(const ctkDoubleSpinBox);
  if (d->SizeHintPolicy == ctkDoubleSpinBox::SizeHintByMinMax)
    {
    return this->Superclass::sizeHint();
    }
  if (!d->CachedSizeHint.isEmpty())
    {
    return d->CachedSizeHint;
    }

  QSize newSizeHint;
  newSizeHint.setHeight(this->lineEdit()->sizeHint().height());

  QString extraString = " "; // give some room
  QString s = this->text() + extraString;
  s.truncate(18);
  int extraWidth = 2; // cursor width

  this->ensurePolished(); // ensure we are using the right font
  const QFontMetrics fm(this->fontMetrics());
  #if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
  int width_in_pixels = fm.horizontalAdvance(s + extraString);
  #else
  int width_in_pixels = fm.width(s + extraString);
  #endif
  newSizeHint.setWidth(width_in_pixels + extraWidth);

  QStyleOptionSpinBox opt;
  d->SpinBox->initStyleOptionSpinBox(&opt);

#if QT_VERSION < QT_VERSION_CHECK(5,1,0)
  QSize extraSize(35, 6);
  opt.rect.setSize(newSizeHint + extraSize);
  extraSize += newSizeHint - this->style()->subControlRect(
    QStyle::CC_SpinBox, &opt,
    QStyle::SC_SpinBoxEditField, this).size();
  // Converging size hint...
  opt.rect.setSize(newSizeHint + extraSize);
  extraSize += newSizeHint - this->style()->subControlRect(
    QStyle::CC_SpinBox, &opt,
    QStyle::SC_SpinBoxEditField, this).size();
  newSizeHint += extraSize;
#endif

  opt.rect = this->rect();
  d->CachedSizeHint = this->style()->sizeFromContents(
    QStyle::CT_SpinBox, &opt, newSizeHint, this)
    .expandedTo(QApplication::globalStrut());
  return d->CachedSizeHint;
}

//----------------------------------------------------------------------------
QSize ctkDoubleSpinBox::minimumSizeHint() const
{
  Q_D(const ctkDoubleSpinBox);
  if (d->SizeHintPolicy == ctkDoubleSpinBox::SizeHintByMinMax)
    {
    // For some reasons, Superclass::minimumSizeHint() returns the spinbox
    // sizeHint()
    return this->spinBox()->minimumSizeHint();
    }
  if (!d->CachedMinimumSizeHint.isEmpty())
    {
    return d->CachedMinimumSizeHint;
    }

  QSize newSizeHint;
  newSizeHint.setHeight(this->lineEdit()->minimumSizeHint().height());

  QString extraString = " "; // give some room
  QString s = this->text() + extraString;
  s.truncate(18);
  int extraWidth = 2; // cursor width

  this->ensurePolished(); // ensure we are using the right font
  const QFontMetrics fm(this->fontMetrics());
  #if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
  int width_in_pixels = fm.horizontalAdvance(s + extraString);
  #else
  int width_in_pixels = fm.width(s + extraString);
  #endif
  newSizeHint.setWidth(width_in_pixels + extraWidth);

  QStyleOptionSpinBox opt;
  d->SpinBox->initStyleOptionSpinBox(&opt);

#if QT_VERSION < QT_VERSION_CHECK(5,1,0)
  QSize extraSize(35, 6);
  opt.rect.setSize(newSizeHint + extraSize);
  extraSize += newSizeHint - this->style()->subControlRect(
    QStyle::CC_SpinBox, &opt,
    QStyle::SC_SpinBoxEditField, this).size();
  // Converging size hint...
  opt.rect.setSize(newSizeHint + extraSize);
  extraSize += newSizeHint - this->style()->subControlRect(
    QStyle::CC_SpinBox, &opt,
    QStyle::SC_SpinBoxEditField, this).size();
  newSizeHint += extraSize;
#endif

  opt.rect = this->rect();
  d->CachedMinimumSizeHint = this->style()->sizeFromContents(
    QStyle::CT_SpinBox, &opt, newSizeHint, this)
    .expandedTo(QApplication::globalStrut());
  return d->CachedMinimumSizeHint;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::keyPressEvent(QKeyEvent* event)
{
  Q_D(ctkDoubleSpinBox);
  const bool accept = this->eventFilter(d->SpinBox, event);
  event->setAccepted(accept);
}

//-----------------------------------------------------------------------------
bool ctkDoubleSpinBox::eventFilter(QObject* obj, QEvent* event)
{
  Q_D(ctkDoubleSpinBox);
  if (d->DOption & ctkDoubleSpinBox::DecimalsByShortcuts &&
    obj == d->SpinBox && event->type() == QEvent::KeyPress)
    {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    Q_ASSERT(keyEvent);
    int newDecimals = -1;
    if (keyEvent->modifiers() & Qt::ControlModifier)
      {
      if (keyEvent->key() == Qt::Key_Plus
        || keyEvent->key() == Qt::Key_Equal)
        {
        newDecimals = this->decimals() + 1;
        }
      else if (keyEvent->key() == Qt::Key_Minus)
        {
        newDecimals = this->decimals() - 1;
        }
      else if (keyEvent->key() == Qt::Key_0)
        {
        newDecimals = d->DefaultDecimals;
        }
      }
    if (newDecimals != -1)
      {
      double currentValue = this->value();
      if (d->Proxy)
        {
        currentValue = d->Proxy.data()->proxyValueFromValue(currentValue);
        }
      // increasing the number of decimals should restore lost precision
      d->setValue(currentValue, newDecimals);
      return true;
      }
    return QWidget::eventFilter(obj, event);
    }
  else
    {
    // pass the event on to the parent class
    return QWidget::eventFilter(obj, event);
    }
}
