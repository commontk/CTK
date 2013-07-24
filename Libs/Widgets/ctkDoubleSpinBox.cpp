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
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QShortcut>
#include <QSizePolicy>
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
void ctkDoubleSpinBoxPrivate::setValue(double value, int dec)
{
  Q_Q(ctkDoubleSpinBox);
  dec = this->boundDecimals(dec);
  bool changeDecimals = dec != q->decimals();
  if (changeDecimals)
    {
    // don't fire decimalsChanged signal yet, wait for the value to be
    // up-to-date.
    this->SpinBox->setDecimals(dec);
    }
  this->SpinBox->setValue(value); // re-do the text (calls textFromValue())
  if (changeDecimals)
    {
    emit q->decimalsChanged(dec);
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
        this->setValue(newValue, decimals);
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
  //qDebug() << "input: " << input << "pos:" << pos;
  if (this->CachedText == input)
    {
    state = this->CachedState;
    decimals = this->CachedDecimals;
    //qDebug() << "cachedText was '" << this->CachedText << "' state was "
    //         << int(state) << " and value was " << this->CachedValue;
    return this->CachedValue;
    }
  const double max = q->maximum();
  const double min = q->minimum();

  int posInValue = pos;
  QString text = this->stripped(input, &posInValue);
  // posInValue can change, track the offset.
  const int oldPosInValue = posInValue;
  //qDebug() << "text: " << text << pos;
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
    //qDebug() << "Acceptable: " << text << value << ok;
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
  //qDebug() << "end: text is '" << this->CachedText << "' state is "
  //         << int(state) << ", value is " << this->CachedValue
  //         << " decimals is " << decimals
  //         << " and pos is " << pos;
  return value;
}

/*
//-----------------------------------------------------------------------------
double ctkDoubleSpinBoxPrivate
::validateAndInterpret(QString &input, int &pos,
                       QValidator::State &state, int &decimals) const
{
  Q_Q(const ctkDoubleSpinBox);
  qDebug() << "input: " << input << "pos:" << pos;
  if (this->CachedText == input && !input.isEmpty())
    {
    state = this->CachedState;
    decimals = this->CachedDecimals;
    qDebug() << "cachedText was '" << this->CachedText << "' state was "
             << int(state) << " and value was " << this->CachedValue;
    return this->CachedValue;
    }
  const double max = q->maximum();
  const double min = q->minimum();

  QString copy = this->stripped(input, &pos);
  qDebug() << "copy: " << copy << pos;
  int len = copy.size();
  double num = min;
  const bool plus = max >= 0;
  const bool minus = min <= 0;
  decimals = 0;


  switch (len)
    {
    case 0:
      state = max != min ? QValidator::Intermediate : QValidator::Invalid;
      goto end;
      break;
    case 1:
      if (copy.at(0) == q->locale().decimalPoint()
          || (plus && copy.at(0) == QLatin1Char('+'))
          || (minus && copy.at(0) == QLatin1Char('-')))
        {
        state = QValidator::Intermediate;
        goto end;
        }
      break;
    case 2:
      if (copy.at(1) == q->locale().decimalPoint()
          && ((plus && copy.at(0) == QLatin1Char('+')) || (minus && copy.at(0) == QLatin1Char('-'))))
        {
        state = QValidator::Intermediate;
        goto end;
        }
      break;
    default:
      break;
    }

  if (copy.at(0) == q->locale().groupSeparator())
    {
    state = QValidator::Invalid;
    goto end;
    }
  else if (len > 1)
    {
    const int dec = copy.indexOf(q->locale().decimalPoint());
    if (dec != -1)
      {
      if (dec + 1 < copy.size() && copy.at(dec + 1) == q->locale().decimalPoint() && pos == dec + 1)
        {
        copy.remove(dec + 1, 1); // typing a delimiter when you are on the delimiter
        } // should be treated as typing right arrow
      // When DecimalsByKey is set, it is possible to extend the number of decimals
      if (!(this->DOption & ctkDoubleSpinBox::DecimalsByKey) &&
          (copy.size() - dec > q->decimals() + 1))
        {
        state = QValidator::Invalid;
        goto end;
        }
      for (int i=dec + 1; i<copy.size(); ++i)
        {
        if (copy.at(i).isSpace() || copy.at(i) == q->locale().groupSeparator())
          {
          state = QValidator::Invalid;
          goto end;
          }
        }
      decimals = copy.size() - dec - 1;
      }
    else
      {
      /// Don't accept lack of decimal point.
      /// It could change 1.00 into 100 in 1 key stroke (delete or backspace).
      if (this->DOption & ctkDoubleSpinBox::DecimalsByKey)
        {
        state = QValidator::Invalid;
        goto end;
        }
      const QChar &last = copy.at(len - 1);
      const QChar &secondLast = copy.at(len - 2);
      if ((last == q->locale().groupSeparator() || last.isSpace())
          && (secondLast == q->locale().groupSeparator() || secondLast.isSpace()))
        {
        state = QValidator::Invalid;
        goto end;
        }
      else if (last.isSpace() && (!q->locale().groupSeparator().isSpace() || secondLast.isSpace()))
        {
        state = QValidator::Invalid;
        goto end;
        }
      }
    }

    {
    bool ok = false;
    num = q->locale().toDouble(copy, &ok);

    if (!ok) {
    if (q->locale().groupSeparator().isPrint())
      {
      if (max < 1000 && min > -1000 && copy.contains(q->locale().groupSeparator()))
        {
        state = QValidator::Invalid;
        goto end;
        }
      const int len = copy.size();
      for (int i=0; i<len- 1; ++i)
        {
        if (copy.at(i) == q->locale().groupSeparator() && copy.at(i + 1) == q->locale().groupSeparator())
          {
          state = QValidator::Invalid;
          goto end;
          }
        }

      QString copy2 = copy;
      copy2.remove(q->locale().groupSeparator());
      num = q->locale().toDouble(copy2, &ok);

      if (!ok)
        {
        state = QValidator::Invalid;
        goto end;
        }
      }
    }

    if (!ok)
      {
      state = QValidator::Invalid;
      }
    else if (num >= min && num <= max)
      {
      state = QValidator::Acceptable;
      }
    else if (max == min)
      { // when max and min is the same the only non-Invalid input is max (or min)
      state = QValidator::Invalid;
      }
    else
      {
      if ((num >= 0 && num > max) || (num < 0 && num < min))
        {
        state = QValidator::Invalid;
        }
      else
        {
        state = QValidator::Intermediate;
        }
      }
    }
end:
    if (state != QValidator::Acceptable)
      {
      num = max > 0 ? min : max;
      }

    input = q->prefix() + copy + q->suffix();
    this->CachedText = input;
    this->CachedState = state;
    this->CachedValue = num;
    this->CachedDecimals = decimals;
    qDebug() << "end: text is '" << this->CachedText << "' state is "
             << int(state) << ", value is " << this->CachedValue
             << " decimals is " << decimals
             << " and pos is " << pos;

    return num;
}
*/

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::onValueChanged()
{
  Q_Q(ctkDoubleSpinBox);
  double value = q->value();
  emit q->valueChanged(value);
  emit q->valueChanged(QString::number(value, 'f', this->SpinBox->decimals()));
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::onValueProxyAboutToBeModified()
{
  Q_Q(ctkDoubleSpinBox);
  this->SpinBox->setProperty("inputValue", q->value());
  this->SpinBox->setProperty("inputMinimum", q->minimum());
  this->SpinBox->setProperty("inputMaximum", q->maximum());
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxPrivate::onValueProxyModified()
{
  Q_Q(ctkDoubleSpinBox);
  q->setRange(this->SpinBox->property("inputMinimum").toDouble(),
              this->SpinBox->property("inputMaximum").toDouble());
  q->setValue(this->SpinBox->property("inputValue").toDouble());
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
double ctkDoubleSpinBox::value() const
{
  Q_D(const ctkDoubleSpinBox);
  double val = d->SpinBox->value();
  if (d->Proxy)
    {
    val = this->round(d->Proxy.data()->valueFromProxyValue(val));
    }
  return val;
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
  double min = d->SpinBox->minimum();
  if (d->Proxy)
    {
    min = d->Proxy.data()->valueFromProxyValue(min);
    }
  return min;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setMinimum(double newMin)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Proxy)
    {
    newMin = d->Proxy.data()->proxyValueFromValue(newMin);
    }
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
      && d->compare(newMin, d->SpinBox->minimum()))
    {
    return;
    }

  d->SpinBox->setMinimum(newMin);
}

//-----------------------------------------------------------------------------
double ctkDoubleSpinBox::maximum() const
{
  Q_D(const ctkDoubleSpinBox);
  double max = d->SpinBox->maximum();
  if (d->Proxy)
    {
    max = d->Proxy.data()->valueFromProxyValue(max);
    }
  return max;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setMaximum(double newMax)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Proxy)
    {
    newMax = d->Proxy.data()->proxyValueFromValue(newMax);
    }
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
    && d->compare(newMax, d->SpinBox->maximum()))
    {
    return;
    }

  d->SpinBox->setMaximum(newMax);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setRange(double newMin, double newMax)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Proxy)
    {
    newMin = d->Proxy.data()->proxyValueFromValue(newMin);
    newMax = d->Proxy.data()->proxyValueFromValue(newMax);
    }
  if (newMin > newMax)
    {
    qSwap(newMin, newMax);
    }
  if (d->Mode == ctkDoubleSpinBox::SetIfDifferent
      && d->compare(newMax, d->SpinBox->maximum())
      && d->compare(newMin, d->SpinBox->minimum()))
    {
    return;
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
  bool set = false;
  double proxyValue = newValue;
  if (d->Proxy)
    {
    proxyValue = d->Proxy.data()->proxyValueFromValue(proxyValue);
    }

  if (d->DOption & ctkDoubleSpinBox::DecimalsByValue)
    {
    int newValueDecimals =
      ctk::significantDecimals(proxyValue, d->DefaultDecimals);
    set = this->value() != d->round(proxyValue, newValueDecimals)
      || d->SpinBox->decimals() != newValueDecimals;
    }
  else
    {
    set = !d->compare(this->value(), proxyValue);
    }
  if (set)
    {
    // Pass newValue and not proxyValue as setValueAlways also computes the
    // proxyValue from the given value
    this->setValueAlways(newValue);
    }
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBox::setValueAlways(double value)
{
  Q_D(ctkDoubleSpinBox);
  if (d->Proxy)
    {
    value = d->Proxy.data()->proxyValueFromValue(value);
    }

  int decimals = -1;
  if (d->DOption & ctkDoubleSpinBox::DecimalsByValue)
    {
    decimals = ctk::significantDecimals(value, d->DefaultDecimals);
    }

  d->setValue(value, decimals);
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
