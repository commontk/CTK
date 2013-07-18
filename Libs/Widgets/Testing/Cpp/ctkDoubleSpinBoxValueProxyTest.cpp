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
#include <QApplication>
#include <QDebug>
#include <QString>

// CTK includes
#include "ctkDoubleSpinBox.h"
#include "ctkLinearValueProxy.h"
#include "ctkTest.h"
#include "ctkValueProxy.h"

// STD includes
#include <limits>

namespace
{

//-----------------------------------------------------------------------------
void getSpyReport(QSignalSpy& spy, double expectedValue)
{
  QCOMPARE(spy.count(), 1);

  QList<QVariant> arguments = spy.takeFirst(); // take the first signal
  ctkTest::COMPARE(arguments.at(0).toDouble(), expectedValue);
}

//-----------------------------------------------------------------------------
void getSpyReport(QSignalSpy& spy, QString expectedValue)
{
  QCOMPARE(spy.count(), 1);

  QList<QVariant> arguments = spy.takeFirst(); // take the first signal
  QCOMPARE(arguments.at(0).toString(), expectedValue);
}

} // end namespace

// ----------------------------------------------------------------------------
class ctkDoubleSpinBoxValueProxyTester: public QObject
{
  Q_OBJECT
private slots:

  void testSetValue();
  void testSetValue_data();

  void testSetDisplayedValue();
  void testSetDisplayedValue_data();
};

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxValueProxyTester::testSetValue()
{
  // Setup
  ctkDoubleSpinBox spinBox;
  spinBox.setMinimum(-200);
  spinBox.setMaximum(200);
  spinBox.setValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  spinBox.setValueProxy(&proxy);

  // Spies
  QSignalSpy valueSpy(&spinBox, SIGNAL(valueChanged(double)));
  QSignalSpy valueStringSpy(&spinBox, SIGNAL(valueChanged(QString)));

  // Test
  QFETCH(double, value);
  spinBox.setValue(value);

  QFETCH(double, expectedValue);
  QFETCH(QString, expectedStringValue);
  getSpyReport(valueSpy, expectedValue);
  getSpyReport(valueStringSpy, expectedStringValue);
  ctkTest::COMPARE(spinBox.value(), expectedValue);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxValueProxyTester::testSetValue_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("expectedValue");
  QTest::addColumn<QString>("expectedStringValue");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1.0 << 42.19176 << 0.1 << 0.1 << "0.10";

  QTest::newRow("Offset only: less than min")
    << 1.0 << 42.19176 << -510.0 << -242.19 << "-242.19";
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1.0 << 42.19176 << -230.0 << -230.0 << "-230.00";
  QTest::newRow("Offset only: less than min with offset")
    << 1.0 << -42.1976 << -190.0 << -157.8 << "-157.80";

  QTest::newRow("Offset only: more than max with offset")
    << 1.0 << 42.19176 << 160.0 << 157.81 << "157.81";
  QTest::newRow("Offset only: more than max")
    << 1.0 << -42.1976 << 65010.0 << 242.2 << "242.20";
  QTest::newRow("Offset only: less than max but ok with offset")
    << 1.0 << -42.1976 << 229.1 << 229.1 << "229.10";

  QTest::newRow("Offset only: max")
    << 1.0 << 42.19176 << std::numeric_limits<double>::max()
    << 157.81 << "157.81";
  QTest::newRow("Offset only:  min")
    << 1.0 << 42.19176 << -std::numeric_limits<double>::max()
    << -242.19 << "-242.19";
  QTest::newRow("Offset only: infinity")
    << 1.0 << 42.19176 << std::numeric_limits<double>::infinity()
    << 157.81 << "157.81";
  QTest::newRow("Offset only:  - infinity")
    << 1.0 << 42.19176 << -std::numeric_limits<double>::infinity()
    << -242.19 << "-242.19";
  QTest::newRow("Offset only: Nan")
    << 1.0 << 42.19176 << std::numeric_limits<double>::quiet_NaN()
    << 157.81 << "157.81";

  // coeff // offset // value // expectedValue // expectedStringValue
  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << 0.1 << 0.1 << "0.10";

  QTest::newRow("Coeff only: less than min")
    << 5.0 << 0.0 << -510.0 << -40.0 << "-40.00";
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0.0 << -230.0 << -230.0 << "-230.00";
  QTest::newRow("Coeff only: less than min with coeff")
    << 5.0 << 0.0 << -190.0 << -40.0 << "-40.00";

  QTest::newRow("Coeff only: more than max with coeff")
    << 5.0 << 0.0 << 160.0 << 40.0 << "40.00";
  QTest::newRow("Coeff only: more than max")
    << 5.0 << 0.0 << 65010.0 << 40.0 << "40.00";
  QTest::newRow("Offset only: less than max but ok with coeff")
    << 0.5 << 0.0 << 229.2 << 229.2 << "229.20";

  QTest::newRow("Coeff only: max")
    << 5.0 << 0.0 << std::numeric_limits<double>::max() << 40.0 << "40.00";
  QTest::newRow("Coeff only:  min")
    << 5.0 << 0.0 << -std::numeric_limits<double>::max() << -40.0 << "-40.00";
  QTest::newRow("Coeff only: infinity")
    << 5.0 << 0.0 << std::numeric_limits<double>::infinity()
    << 40.0 << "40.00";
  QTest::newRow("Coeff only:  - infinity")
    << 5.0 << 0.0 << -std::numeric_limits<double>::infinity()
    << -40.0 << "-40.00";
  QTest::newRow("Coeff only: Nan")
    << 5.0 << 0.0 << std::numeric_limits<double>::quiet_NaN()
    << 40.0 << "40.00";


  // coeff // offset // value // expectedValue // expectedStringValue
  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 0.0 << 0.1 << 0.1 << "0.10";

  QTest::newRow("Linear: less than min")
    << 5.0 << 12.0 << -510.0 << -42.4 << "-42.40";
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12.0 << -230.0 << -230.0 << "-230.00";
  QTest::newRow("Linear: less than min with function")
    << 5.0 << 12.0 << -61.5 << -42.4 << "-42.40";

  QTest::newRow("Linear: more than max with function")
    << 5.0 << 12.0 << 160.0 << 37.6 << "37.60";
  QTest::newRow("Linear: more than max")
    << 5.0 << 12.0 << 65010.0 << 37.6 << "37.60";
  QTest::newRow("Offset only: less than max but ok with function")
    << 0.5 << 12.0 << 229.2 << 229.2 << "229.20";

  QTest::newRow("Linear: max")
    << 5.0 << 12.0 << std::numeric_limits<double>::max() << 37.6 << "37.60";
  QTest::newRow("Linear:  min")
    << 5.0 << 12.0 << -std::numeric_limits<double>::max() << -42.4 << "-42.40";
  QTest::newRow("Linear: infinity")
    << 5.0 << 12.0 << std::numeric_limits<double>::infinity()
    << 37.6 << "37.60";
  QTest::newRow("Linear:  - infinity")
    << 5.0 << 12.0 << -std::numeric_limits<double>::infinity()
    << -42.4 << "-42.40";
  QTest::newRow("Linear: Nan")
    << 5.0 << 12.0 << std::numeric_limits<double>::quiet_NaN()
    << 37.6 << "37.60";
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxValueProxyTester::testSetDisplayedValue()
{
  // Setup
  ctkDoubleSpinBox spinBox;
  spinBox.setMinimum(-200);
  spinBox.setMaximum(200);
  spinBox.setValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  spinBox.setValueProxy(&proxy);

  // Spies
  QSignalSpy valueSpy(&spinBox, SIGNAL(valueChanged(double)));
  QSignalSpy valueStringSpy(&spinBox, SIGNAL(valueChanged(QString)));

  // Test
  QFETCH(double, displayValue);
  spinBox.setDisplayedValue(displayValue);

  QFETCH(double, expectedValue);
  QFETCH(QString, expectedStringValue);
  QFETCH(double, expectedDisplayValue);
  getSpyReport(valueSpy, expectedValue);
  getSpyReport(valueStringSpy, expectedStringValue);
  ctkTest::COMPARE(spinBox.value(), expectedValue);
  QCOMPARE(spinBox.displayedValue(), expectedDisplayValue);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxValueProxyTester::testSetDisplayedValue_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("displayValue");
  QTest::addColumn<double>("expectedValue");
  QTest::addColumn<QString>("expectedStringValue");
  QTest::addColumn<double>("expectedDisplayValue");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only")
    << 1.0 << 42.19176 << 0.1 << -42.09 << "-42.09" << 0.1;

  QTest::newRow("Offset only: less than min")
    << 1.0 << 42.19176 << -510.0 << -242.19 << "-242.19" << -200.0;
  QTest::newRow("Offset only: more than max")
    << 1.0 << -42.1976 << 65010.0 << 242.2 << "242.20" << 200.0;

  QTest::newRow("Offset only: max")
    << 1.0 << 42.19176 << std::numeric_limits<double>::max()
    << 157.81 << "157.81" << 200.0;
  QTest::newRow("Offset only:  min")
    << 1.0 << 42.19176 << -std::numeric_limits<double>::max()
    << -242.19 << "-242.19" << -200.0;
  QTest::newRow("Offset only: infinity")
    << 1.0 << 42.19176 << std::numeric_limits<double>::infinity()
    << 157.81 << "157.81" << 200.0;
  QTest::newRow("Offset only:  - infinity")
    << 1.0 << 42.19176 << -std::numeric_limits<double>::infinity()
    << -242.19 << "-242.19" << -200.0;
  QTest::newRow("Offset only: Nan")
    << 1.0 << 42.19176 << std::numeric_limits<double>::quiet_NaN()
    << 157.81 << "157.81" << 200.0;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only")
    << 5.0 << 0.0 << 5.0 << 1.0 << "1.00" << 5.0;

  QTest::newRow("Coeff only: less than min")
    << 5.0 << 0.0 << -1010.0 << -40.0 << "-40.00" << -200.0;
  QTest::newRow("Coeff only: more than max")
    << 5.0 << 0.0 << 65010.0 << 40.0 << "40.00" << 200.0;

  QTest::newRow("Coeff only: max")
    << 5.0 << 0.0 << std::numeric_limits<double>::max()
    << 40.0 << "40.00" << 200.0;
  QTest::newRow("Coeff only:  min")
    << 5.0 << 0.0 << -std::numeric_limits<double>::max()
    << -40.0 << "-40.00" << -200.0;
  QTest::newRow("Coeff only: infinity")
    << 5.0 << 0.0 << std::numeric_limits<double>::infinity()
    << 40.0 << "40.00" << 200.0;
  QTest::newRow("Coeff only:  - infinity")
    << 5.0 << 0.0 << -std::numeric_limits<double>::infinity()
    << -40.0 << "-40.00" << -200.0;
  QTest::newRow("Coeff only: Nan")
    << 5.0 << 0.0 << std::numeric_limits<double>::quiet_NaN()
    << 40.0 << "40.00" << 200.0;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 12.0 << 42.0 << 6.0 << "6.00" << 42.0;

  QTest::newRow("Linear: less than min")
    << 5.0 << 12.0 << -5010.0 << -42.4 << "-42.40" << -200.0;

  QTest::newRow("Linear: more than max")
    << 5.0 << 12.0 << 65010.0 << 37.6 << "37.60" << 200.0;

  QTest::newRow("Linear: max")
    << 5.0 << 12.0 << std::numeric_limits<double>::max()
    << 37.6 << "37.60" << 200.0;
  QTest::newRow("Linear:  min")
    << 5.0 << 12.0 << -std::numeric_limits<double>::max()
    << -42.4 << "-42.40" << -200.0;
  QTest::newRow("Linear: infinity")
    << 5.0 << 12.0 << std::numeric_limits<double>::infinity()
    << 37.6 << "37.60" << 200.0;
  QTest::newRow("Linear:  - infinity")
    << 5.0 << 12.0 << -std::numeric_limits<double>::infinity()
    << -42.4 << "-42.40" << -200.0;
  QTest::newRow("Linear: Nan")
    << 5.0 << 12.0 << std::numeric_limits<double>::quiet_NaN()
    << 37.6 << "37.60" << 200.0;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkDoubleSpinBoxValueProxyTest)
#include "moc_ctkDoubleSpinBoxValueProxyTest.cpp"
