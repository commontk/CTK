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

#ifndef __ctkComplementMapper_h
#define __ctkComplementMapper_h

// Qt includes
#include <QObject>

// CTK includes
#include "ctkCoreExport.h"
class ctkComplementMapperPrivate;

//---------------------------------------------------------------------------
/// \ingroup Core
/// QCheckBox* checkBox = new QCheckBox;
/// ctkComplementMapper* inverter =
///   new ctkComplementMapper("checked", SIGNAL("toggled(bool)"), checkBox);
/// inverter->setComplementValue(true);
/// // -> checkBox->checked() == false
/// inverter->setValue(false);
/// // -> checkBox->checked() == false
class CTK_CORE_EXPORT ctkComplementMapper : public QObject
{
  Q_OBJECT
  /// This property contains the name of the object mapped property.
  Q_PROPERTY(QByteArray propertyName READ propertyName)

  /// This property holds the mapped property.
  /// It is the value of the mapped object property
  Q_PROPERTY(bool value READ value WRITE setValue NOTIFY valueComplementChanged STORED false);

  /// This property is the complement of the mapped property.
  /// false if \a value is true and true if \a value is false
  Q_PROPERTY(bool valueComplement READ valueComplement WRITE setValueComplement NOTIFY valueComplementChanged STORED false)

public:
  /// Map the property \a property of the object.
  /// The mapper becomes a child of \a object and will be destructed when
  /// \a object is destructed.
  /// property and object must be valid and non empty. If signal is 0,
  /// \a valueChanged(bool) and \a valueComplementChanged(bool) won't be fired.
  ctkComplementMapper(QObject* targetObject, const QByteArray& propertyName, const char* signal);
  virtual ~ctkComplementMapper();

  QByteArray propertyName()const;

  /// The mapped object (the mapper parent)
  QObject* targetObject()const;

  bool value()const;
  bool valueComplement()const;

public Q_SLOTS:
  void setValue(bool value);
  void setValueComplement(bool valueComplement);

  void toggle();

Q_SIGNALS:
  void valueChanged(bool value);
  void valueComplementChanged(bool valueComplement);

protected Q_SLOTS:
  void emitValueChanged();
protected:
  QScopedPointer<ctkComplementMapperPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkComplementMapper);
  Q_DISABLE_COPY(ctkComplementMapper);
};

#endif

