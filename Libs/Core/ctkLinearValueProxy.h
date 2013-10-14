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

#ifndef __ctkLinearValueProxy_h
#define __ctkLinearValueProxy_h

// CTK includes
#include "ctkCoreExport.h"
#include "ctkValueProxy.h"
#include "ctkPimpl.h"

class ctkLinearValueProxyPrivate;

/// \ingroup Core
/// \brief Implementation of an affine value proxy.
/// The ctkLinearValueProxy takes a coefficient and an offset,
/// effectively implementing a value proxy such as:
/// valueProxy = coefficient * value + offset
/// Note: If the coefficient is null then the property value given
/// by value = (valueProxy - offset) / coefficient can give bad results
/// (+ or - infinity depending on the sign of valueProxy - offset).
/// \sa ctkValueProxy
class CTK_CORE_EXPORT ctkLinearValueProxy : public ctkValueProxy
{
  Q_OBJECT
  Q_PROPERTY(double coefficient READ coefficient WRITE setCoefficient)
  Q_PROPERTY(double offset READ offset WRITE setOffset)

public:
  typedef ctkValueProxy Superclass;
  explicit ctkLinearValueProxy(QObject* parent = 0);
  virtual ~ctkLinearValueProxy();

  virtual double proxyValueFromValue(double value) const;

  virtual double valueFromProxyValue(double proxyValue) const;

  virtual double coefficient() const;
  virtual double offset() const;

public Q_SLOTS:
  virtual void setCoefficient(double newCoeff);
  virtual void setOffset(double newOffset);

protected:
  QScopedPointer<ctkLinearValueProxyPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkLinearValueProxy);
  Q_DISABLE_COPY(ctkLinearValueProxy);
};

#endif
