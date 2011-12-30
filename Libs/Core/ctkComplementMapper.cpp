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
#include <QVariant>

// CTK includes
#include "ctkComplementMapper.h"

//-----------------------------------------------------------------------------
class ctkComplementMapperPrivate
{
public:
  QByteArray PropertyName;
};

// --------------------------------------------------------------------------
// ctkComplementMapper methods

// --------------------------------------------------------------------------
ctkComplementMapper::ctkComplementMapper(
  QObject* targetObject, const QByteArray& property, const char* signal)
  : QObject(targetObject)
  , d_ptr(new ctkComplementMapperPrivate)
{
  Q_ASSERT(property.isEmpty() != true);
  Q_ASSERT(targetObject != 0);
  Q_D(ctkComplementMapper);
  d->PropertyName = property;
  if (signal)
    {
    connect(targetObject, signal, this, SLOT(emitValueChanged()));
    }
}

// --------------------------------------------------------------------------
ctkComplementMapper::~ctkComplementMapper()
{
}

// --------------------------------------------------------------------------
QByteArray ctkComplementMapper::propertyName()const
{
  Q_D(const ctkComplementMapper);
  return d->PropertyName;
}

// --------------------------------------------------------------------------
QObject* ctkComplementMapper::targetObject()const
{
  return this->parent();
}

// --------------------------------------------------------------------------
bool ctkComplementMapper::value()const
{
  return this->targetObject()->property(this->propertyName()).toBool();
}

// --------------------------------------------------------------------------
bool ctkComplementMapper::valueComplement()const
{
  return !this->value();
}

// --------------------------------------------------------------------------
void ctkComplementMapper::setValue(bool value)
{
  this->targetObject()->setProperty(this->propertyName(), QVariant(value));
}

// --------------------------------------------------------------------------
void ctkComplementMapper::setValueComplement(bool value)
{
  this->setValue(!value);
}

// --------------------------------------------------------------------------
void ctkComplementMapper::toggle()
{
  this->setValue(this->valueComplement());
}

// --------------------------------------------------------------------------
void ctkComplementMapper::emitValueChanged()
{
  emit valueChanged(this->value());
  emit valueComplementChanged(this->valueComplement());
}

