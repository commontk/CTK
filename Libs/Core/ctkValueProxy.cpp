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
#include "ctkValueProxy.h"

// --------------------------------------------------------------------------
// ctkValueProxyPrivate

class CTK_CORE_EXPORT ctkValueProxyPrivate
{
  Q_DECLARE_PUBLIC(ctkValueProxy);

public:
  ctkValueProxy* q_ptr;
  ctkValueProxyPrivate(ctkValueProxy& object);
  ~ctkValueProxyPrivate();

  double Value;
  double ProxyValue;
};

// --------------------------------------------------------------------------
// ctkValueProxyPrivate methods

// --------------------------------------------------------------------------
ctkValueProxyPrivate::ctkValueProxyPrivate(ctkValueProxy& object)
  : q_ptr(&object)
{
  this->Value = 0.0;
  this->ProxyValue = 0.0;
}

// --------------------------------------------------------------------------
ctkValueProxyPrivate::~ctkValueProxyPrivate()
{
}

// --------------------------------------------------------------------------
// ctkValueProxy methods

// --------------------------------------------------------------------------
ctkValueProxy::ctkValueProxy(QObject* _parent) : Superclass(_parent)
  , d_ptr(new ctkValueProxyPrivate(*this))
{
}

// --------------------------------------------------------------------------
ctkValueProxy::~ctkValueProxy()
{
}

// --------------------------------------------------------------------------
double ctkValueProxy::value() const
{
  Q_D(const ctkValueProxy);
  return d->Value;
}

// --------------------------------------------------------------------------
void ctkValueProxy::setValue(double newValue)
{
  Q_D(ctkValueProxy);
  if (d->Value == newValue)
    {
    return;
    }

  d->Value = newValue;
  emit this->valueChanged(d->Value);
  this->updateProxyValue();
}

// --------------------------------------------------------------------------
double ctkValueProxy::proxyValue() const
{
  Q_D(const ctkValueProxy);
  return d->ProxyValue;
}

// --------------------------------------------------------------------------
void ctkValueProxy::setProxyValue(double newProxyValue)
{
  Q_D(ctkValueProxy);
  if (d->ProxyValue == newProxyValue)
    {
    return;
    }

  d->ProxyValue = newProxyValue;
  emit this->proxyValueChanged(d->ProxyValue);
  this->updateValue();
}

// --------------------------------------------------------------------------
void ctkValueProxy::updateProxyValue()
{
  Q_D(ctkValueProxy);
  double newProxyValue = this->proxyValueFromValue(d->Value);
  if (newProxyValue == d->ProxyValue)
    {
    return;
    }

  d->ProxyValue = newProxyValue;
  emit this->proxyValueChanged(d->ProxyValue);
}

// --------------------------------------------------------------------------
void ctkValueProxy::updateValue()
{
  Q_D(ctkValueProxy);
  double newValue = this->valueFromProxyValue(d->ProxyValue);
  if (newValue == d->Value)
    {
    return;
    }

  d->Value = newValue;
  emit this->valueChanged(d->Value);
}
