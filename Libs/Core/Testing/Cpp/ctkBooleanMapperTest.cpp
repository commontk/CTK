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
#include "ctkBooleanMapper.h"
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
class ctkBooleanMapperTester: public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void testValue();
  void testComplement();
  void testValueAsInt();
  void testSignals();
};

// ----------------------------------------------------------------------------
void ctkBooleanMapperTester::testValue()
{
  ctkObjectWithBoolProp object;
  ctkBooleanMapper* complementMapper =
    new ctkBooleanMapper(&object, "checked", SIGNAL(toggled(bool)));

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
void ctkBooleanMapperTester::testComplement()
{
  ctkObjectWithBoolProp object;
  ctkBooleanMapper* complementMapper =
    new ctkBooleanMapper(&object, "checked", SIGNAL(toggled(bool)));

  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->complement(), true);

  object.setChecked(true);
  QCOMPARE(object.checked(), true);
  QCOMPARE(complementMapper->complement(), false);

  complementMapper->setComplement(true);
  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->complement(), true);
}


// ----------------------------------------------------------------------------
void ctkBooleanMapperTester::testValueAsInt()
{
  ctkObjectWithBoolProp object;
  ctkBooleanMapper* complementMapper =
    new ctkBooleanMapper(&object, "checked", SIGNAL(toggled(bool)));
  complementMapper->setTrueValue(QVariant(-1));
  complementMapper->setFalseValue(QVariant(19));

  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->valueAsInt(), 19);

  object.setChecked(true);
  QCOMPARE(object.checked(), true);
  QCOMPARE(complementMapper->valueAsInt(), -1);

  object.setChecked(false);
  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->valueAsInt(), 19);

  complementMapper->setValueAsInt(-1);
  QCOMPARE(object.checked(), true);
  QCOMPARE(complementMapper->valueAsInt(), -1);

  complementMapper->setValueAsInt(19);
  QCOMPARE(object.checked(), false);
  QCOMPARE(complementMapper->valueAsInt(), 19);
}

// ----------------------------------------------------------------------------
void ctkBooleanMapperTester::testSignals()
{
  ctkObjectWithBoolProp object;
  ctkBooleanMapper* complementMapper =
    new ctkBooleanMapper(&object, "checked", SIGNAL(toggled(bool)));

  QSignalSpy spyToggled(&object, SIGNAL(toggled(bool)));
  QSignalSpy spyValueChanged(complementMapper, SIGNAL(valueChanged(bool)));
  QSignalSpy spyComplementChanged(complementMapper,
                                  SIGNAL(complementChanged(bool)));
  QSignalSpy spyValueAsIntChanged(complementMapper,
                                  SIGNAL(valueAsIntChanged(int)));

  object.setChecked(true);
  QCOMPARE(spyToggled.count(), 1);
  QCOMPARE(spyValueChanged.count(), 1);
  QCOMPARE(spyComplementChanged.count(), 1);
  QCOMPARE(spyValueAsIntChanged.count(), 1);

  spyToggled.clear();
  spyValueChanged.clear();
  spyComplementChanged.clear();
  spyValueAsIntChanged.clear();

  // no op
  complementMapper->setValue(complementMapper->value());
  QCOMPARE(spyToggled.count(), 0);
  QCOMPARE(spyValueChanged.count(), 0);
  QCOMPARE(spyComplementChanged.count(), 0);
  QCOMPARE(spyValueAsIntChanged.count(), 0);

  complementMapper->toggle();
  QCOMPARE(spyToggled.count(), 1);
  QCOMPARE(spyValueChanged.count(), 1);
  QCOMPARE(spyComplementChanged.count(), 1);
  QCOMPARE(spyValueAsIntChanged.count(), 1);
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkBooleanMapperTest)
#include "moc_ctkBooleanMapperTest.cpp"
