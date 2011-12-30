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
#include <QSignalSpy>

// CTK includes
#include "ctkComplementMapper.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
// We can't use a QCheckBox because we are in QtCore, simulate one.
class ctkObjectWithBoolProp: public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY toggled);
public:
  ctkObjectWithBoolProp(){this->Checked = false;}
  bool checked()const {return this->Checked;}

public Q_SLOTS:
  void setChecked(bool check){
    if (check == this->Checked)
      return;
    this->Checked = check;
    emit toggled(this->Checked);
  }
Q_SIGNALS:
  void toggled(bool);
private:
  bool Checked;
};

// ----------------------------------------------------------------------------
class ctkComplementMapperTester: public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void testValue();
  void testValueComplement();
  void testSignals();
};

// ----------------------------------------------------------------------------
void ctkComplementMapperTester::testValue()
{
  ctkObjectWithBoolProp object;
  ctkComplementMapper* complementMapper =
    new ctkComplementMapper(&object, "checked", SIGNAL(toggled(bool)));

  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->value(), false);

  object.setChecked(true);
  QCOMPARE(object.checked(), true);
  QCOMPARE(complementMapper->value(), true);

  object.setChecked(false);
  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->value(), false);

  complementMapper->setValue(true);
  QCOMPARE(object.checked(), true);
  QCOMPARE(complementMapper->value(), true);

  complementMapper->setValue(false);
  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->value(), false);
}

// ----------------------------------------------------------------------------
void ctkComplementMapperTester::testValueComplement()
{
  ctkObjectWithBoolProp object;
  ctkComplementMapper* complementMapper =
    new ctkComplementMapper(&object, "checked", SIGNAL(toggled(bool)));

  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->valueComplement(), true);

  object.setChecked(true);
  QCOMPARE(object.checked(), true);
  QCOMPARE(complementMapper->valueComplement(), false);

  complementMapper->setValueComplement(true);
  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->valueComplement(), true);
}

// ----------------------------------------------------------------------------
void ctkComplementMapperTester::testSignals()
{
  ctkObjectWithBoolProp object;
  ctkComplementMapper* complementMapper =
    new ctkComplementMapper(&object, "checked", SIGNAL(toggled(bool)));

  QSignalSpy spyToggled(&object, SIGNAL(toggled(bool)));
  QSignalSpy spyValueChanged(complementMapper, SIGNAL(valueChanged(bool)));
  QSignalSpy spyValueComplementChanged(complementMapper,
                                       SIGNAL(valueComplementChanged(bool)));

  object.setChecked(true);
  QCOMPARE(spyToggled.count(), 1);
  QCOMPARE(spyValueChanged.count(), 1);
  QCOMPARE(spyValueComplementChanged.count(), 1);

  spyToggled.clear();
  spyValueChanged.clear();
  spyValueComplementChanged.clear();

  // no op
  complementMapper->setValue(complementMapper->value());
  QCOMPARE(spyToggled.count(), 0);
  QCOMPARE(spyValueChanged.count(), 0);
  QCOMPARE(spyValueComplementChanged.count(), 0);

  complementMapper->toggle();
  QCOMPARE(spyToggled.count(), 1);
  QCOMPARE(spyValueChanged.count(), 1);
  QCOMPARE(spyValueComplementChanged.count(), 1);
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkComplementMapperTest)
#include "moc_ctkComplementMapperTest.cpp"
