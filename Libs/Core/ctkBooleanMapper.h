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

#ifndef __ctkBooleanMapper_h
#define __ctkBooleanMapper_h

// Qt includes
#include <QObject>
#include <QVariant>

// CTK includes
#include "ctkCoreExport.h"
class ctkBooleanMapperPrivate;

//---------------------------------------------------------------------------
/// \ingroup Core
///
/// Example:
///   QCheckBox* checkBox = new QCheckBox;
///   ctkBooleanMapper* inverter =
///     new ctkBooleanMapper("checked", SIGNAL("toggled(bool)"), checkBox);
///   inverter->setComplementValue(true);
///   // -> checkBox->checked() == false
///   inverter->setValue(false);
///   // -> checkBox->checked() == false
///
/// Python example:
///   boolMapper = ctk.ctkBooleanMapper(checkBox, "checked", "toggled(bool)")
///   boolMapper.trueValue = qt.QMessageBox.Yes
///   boolMapper.falseValue = qt.QMessageBox.InvalidRole
///   parent.registerProperty(
///     "settingsPropertyName", boolMapper, "valueAsInt", qt.SIGNAL("valueAsIntChanged(int)"))
///
class CTK_CORE_EXPORT ctkBooleanMapper : public QObject
{
  Q_OBJECT
  /// This property contains the name of the object mapped property.
  Q_PROPERTY(QByteArray propertyName READ propertyName)

  /// This property holds the mapped property.
  /// It is the value of the mapped object property
  Q_PROPERTY(bool value READ value WRITE setValue NOTIFY complementChanged STORED false);

  /// This property is the complement of the mapped property.
  /// false if \a value is true and true if \a value is false
  Q_PROPERTY(bool complement READ complement WRITE setComplement NOTIFY complementChanged STORED false)

  Q_PROPERTY(int valueAsInt READ valueAsInt WRITE setValueAsInt NOTIFY valueAsIntChanged STORED false )
  Q_PROPERTY(QString valueAsString READ valueAsString WRITE setValueAsString NOTIFY valueAsStringChanged STORED false )

  /// 1 by default
  Q_PROPERTY(QVariant trueValue READ trueValue WRITE setTrueValue )

  /// 0 by default
  Q_PROPERTY(QVariant falseValue READ falseValue WRITE setFalseValue )
public:
  /// Map the property \a property of the object.
  /// The mapper becomes a child of \a object and will be destructed when
  /// \a object is destructed.
  /// property and object must be valid and non empty. If signal is 0,
  /// \a valueChanged(bool) and \a complementChanged(bool) won't be fired.
  ctkBooleanMapper(QObject* targetObject, const QByteArray& propertyName, const QByteArray& signal);
  virtual ~ctkBooleanMapper();

  QByteArray propertyName()const;

  /// The mapped object (the mapper parent)
  QObject* targetObject()const;

  bool value()const;
  bool complement()const;
  int valueAsInt()const;
  QString valueAsString()const;

  QVariant trueValue()const;
  QVariant falseValue()const;

  void setTrueValue(const QVariant& value);
  void setFalseValue(const QVariant& value);

public Q_SLOTS:
  void setValue(bool value);
  void setComplement(bool complement);
  void setValueAsInt(int value);
  void setValueAsString(const QString& value);

  void toggle();

Q_SIGNALS:
  void valueChanged(bool value);
  void complementChanged(bool complement);
  void valueAsIntChanged(int value);
  void valueAsStringChanged(const QString& value);

protected Q_SLOTS:
  void emitValueChanged();
  void emitValueAsChanged();

protected:
  QScopedPointer<ctkBooleanMapperPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkBooleanMapper);
  Q_DISABLE_COPY(ctkBooleanMapper);
};

#endif

