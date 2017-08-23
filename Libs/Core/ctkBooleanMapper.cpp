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

// CTK includes
#include "ctkBooleanMapper.h"

//-----------------------------------------------------------------------------
class ctkBooleanMapperPrivate
{
public:
  ctkBooleanMapperPrivate();
  QByteArray PropertyName;
  QVariant TrueValue;
  QVariant FalseValue;
};

// --------------------------------------------------------------------------
ctkBooleanMapperPrivate::ctkBooleanMapperPrivate()
{
  this->TrueValue = QVariant(true);
  this->FalseValue = QVariant(false);
}

// --------------------------------------------------------------------------
// ctkBooleanMapper methods

// --------------------------------------------------------------------------
ctkBooleanMapper::ctkBooleanMapper(
  QObject* targetObject, const QByteArray& property, const QByteArray& signal)
  : QObject(targetObject)
  , d_ptr(new ctkBooleanMapperPrivate)
{
  Q_ASSERT(property.isEmpty() != true);
  Q_ASSERT(targetObject != 0);
  Q_D(ctkBooleanMapper);
  d->PropertyName = property;
  if (!signal.isEmpty())
    {
    connect(targetObject, signal, this, SLOT(emitValueChanged()));
    }
}

// --------------------------------------------------------------------------
ctkBooleanMapper::~ctkBooleanMapper()
{
}

// --------------------------------------------------------------------------
QByteArray ctkBooleanMapper::propertyName()const
{
  Q_D(const ctkBooleanMapper);
  return d->PropertyName;
}

// --------------------------------------------------------------------------
QObject* ctkBooleanMapper::targetObject()const
{
  return this->parent();
}

// --------------------------------------------------------------------------
QVariant ctkBooleanMapper::trueValue()const
{
  Q_D(const ctkBooleanMapper);
  return d->TrueValue;
}

// --------------------------------------------------------------------------
QVariant ctkBooleanMapper::falseValue()const
{
  Q_D(const ctkBooleanMapper);
  return d->FalseValue;
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::setTrueValue(const QVariant& trueValue)
{
  Q_D(ctkBooleanMapper);
  if (d->TrueValue == trueValue)
    {
    return;
    }
  d->TrueValue = trueValue;
  this->emitValueAsChanged();
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::setFalseValue(const QVariant& falseValue)
{
  Q_D(ctkBooleanMapper);
  if (d->FalseValue == falseValue)
    {
    return;
    }
  d->FalseValue = falseValue;
  this->emitValueAsChanged();
}

// --------------------------------------------------------------------------
bool ctkBooleanMapper::value()const
{
  return this->targetObject()->property(this->propertyName()).toBool();
}

// --------------------------------------------------------------------------
bool ctkBooleanMapper::complement()const
{
  return !this->value();
}

// --------------------------------------------------------------------------
int ctkBooleanMapper::valueAsInt()const
{
  Q_D(const ctkBooleanMapper);
  return this->value() ? d->TrueValue.toInt() : d->FalseValue.toInt();
}

// --------------------------------------------------------------------------
QString ctkBooleanMapper::valueAsString()const
{
  Q_D(const ctkBooleanMapper);
  return this->value() ? d->TrueValue.toString() : d->FalseValue.toString();
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::setValue(bool value)
{
  this->targetObject()->setProperty(this->propertyName(), QVariant(value));
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::setComplement(bool value)
{
  this->setValue(!value);
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::setValueAsInt(int intValue)
{
  Q_D(ctkBooleanMapper);
  this->setValue( QVariant(intValue) == d->TrueValue);
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::setValueAsString(const QString& stringValue)
{
  Q_D(ctkBooleanMapper);
  this->setValue( QVariant(stringValue) == d->TrueValue );
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::toggle()
{
  this->setValue(this->complement());
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::emitValueChanged()
{
  emit valueChanged(this->value());
  emit complementChanged(this->complement());
  this->emitValueAsChanged();
}

// --------------------------------------------------------------------------
void ctkBooleanMapper::emitValueAsChanged()
{
  emit valueAsIntChanged(this->valueAsInt());
  emit valueAsStringChanged(this->valueAsString());
}
