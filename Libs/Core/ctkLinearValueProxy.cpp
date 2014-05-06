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
#include <QDebug>

// CTK includes
#include "ctkLinearValueProxy.h"

// STD includes
#include <limits>

// --------------------------------------------------------------------------
// ctkLinearValueProxyPrivate

class CTK_CORE_EXPORT ctkLinearValueProxyPrivate
{
  Q_DECLARE_PUBLIC(ctkLinearValueProxy);

protected:
  ctkLinearValueProxy* q_ptr;
public:
  ctkLinearValueProxyPrivate(ctkLinearValueProxy& object);
  ~ctkLinearValueProxyPrivate();

  bool isCoefficientValid() const;

  double Coefficient;
  double Offset;
};

// --------------------------------------------------------------------------
// ctkLinearValueProxyPrivate methods

// --------------------------------------------------------------------------
ctkLinearValueProxyPrivate::ctkLinearValueProxyPrivate(ctkLinearValueProxy& object)
  :q_ptr(&object)
{
  this->Coefficient = 1.0;
  this->Offset = 0.0;
}

// --------------------------------------------------------------------------
ctkLinearValueProxyPrivate::~ctkLinearValueProxyPrivate()
{
}

// --------------------------------------------------------------------------
bool ctkLinearValueProxyPrivate::isCoefficientValid() const
{
  return qAbs(this->Coefficient) > std::numeric_limits<double>::epsilon();
}

// --------------------------------------------------------------------------
// ctkLinearValueProxy methods

// --------------------------------------------------------------------------
ctkLinearValueProxy::ctkLinearValueProxy(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkLinearValueProxyPrivate(*this))
{
}

// --------------------------------------------------------------------------
ctkLinearValueProxy::~ctkLinearValueProxy()
{
}

// --------------------------------------------------------------------------
double ctkLinearValueProxy::proxyValueFromValue(double value) const
{
  return (this->coefficient() * value) + this->offset();
}

// --------------------------------------------------------------------------
double ctkLinearValueProxy::valueFromProxyValue(double proxyValue) const
{
  return (proxyValue - this->offset()) / this->coefficient();
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkLinearValueProxy, double, coefficient, Coefficient);
CTK_GET_CPP(ctkLinearValueProxy, double, offset, Offset);

// --------------------------------------------------------------------------
void ctkLinearValueProxy::setCoefficient(double newCoeff)
{
  Q_D(ctkLinearValueProxy);
  if (d->Coefficient == newCoeff)
    {
    return;
    }
  emit proxyAboutToBeModified();
  d->Coefficient = newCoeff;
  this->updateProxyValue();
  emit proxyModified();
}

// --------------------------------------------------------------------------
void ctkLinearValueProxy::setOffset(double newOffset)
{
  Q_D(ctkLinearValueProxy);
  if (d->Offset == newOffset)
    {
    return;
    }
  emit proxyAboutToBeModified();
  d->Offset = newOffset;
  this->updateProxyValue();
  emit proxyModified();
}
