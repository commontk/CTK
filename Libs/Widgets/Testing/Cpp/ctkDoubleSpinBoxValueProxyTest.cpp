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
  QCOMPARE(arguments.at(0).toDouble(), expectedValue);
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

  void testSetCoefficient();
  void testSetCoefficient_data();
};

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxValueProxyTester::testSetValue()
{
  // Setup
  ctkDoubleSpinBox spinBox;
  spinBox.setRange(-200., 200.);
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
  QTest::newRow("Offset only") << 1. << 42.19 << 0.1 << 0.1 << "0.10";
  // \tbd could be improved ?
  QTest::newRow("Offset only: keep precision 3")
    << 1. << 42.197 << 0.1 << 0.1 << "0.10";
  QTest::newRow("Offset only: keep precision 4")
    << 1. << 42.1971 << 0.1 << 0.1 << "0.10";

  QTest::newRow("Offset only: less than min")
    << 1. << -42.19 << -220.0 << -200. << "-200.00";
  QTest::newRow("Offset only: less than min with offset")
    << 1. << -42.19 << -190.0 << -190. << "-190.00";
  QTest::newRow("Offset only: more than min")
    << 1. << 42.19 << -190.0 << -190. << "-190.00";
  QTest::newRow("Offset only: more than min with offset")
    << 1. << 42.19 << -220.0 << -200. << "-200.00";

  QTest::newRow("Offset only: more than max")
    << 1. << 42.19 << 220.0 << 200. << "200.00";
  QTest::newRow("Offset only: more than max with offset")
    << 1. << 42.19 << 190.0 << 190. << "190.00";
  QTest::newRow("Offset only: less than max")
    << 1. << -42.19 << 190.0 << 190. << "190.00";
  QTest::newRow("Offset only: less than max with offset")
    << 1. << -42.19 << 220.0 << 200. << "200.00";

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << 0.1 << 0.1 << "0.10";

  QTest::newRow("Coeff only: less than min")
    << 5. << 0. << -220. << -200. << "-200.00";
  QTest::newRow("Coeff only: less than min with offset")
    << 5. << 0. << -190. << -190. << "-190.00";
  QTest::newRow("Coeff only: more than min")
    << 0.5 << 0. << -190. << -190. << "-190.00";
  QTest::newRow("Coeff only: more than min with offset")
    << 0.5 << 0. << -220. << -200. << "-200.00";

  QTest::newRow("Coeff only: more than max")
    << 5. << 0. << 220. << 200. << "200.00";
  QTest::newRow("Coeff only: more than max with offset")
    << 5. << 0. << 190. << 190. << "190.00";
  QTest::newRow("Coeff only: less than max")
    << 0.5 << 0. << 190. << 190. << "190.00";
  QTest::newRow("Coeff only: less than max with offset")
    << 0.5 << 0. << 220. << 200. << "200.00";
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
  QCOMPARE(spinBox.value(), expectedValue);
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
    << 1.0 << 42.19 << 0.1 << -42.09 << "-42.09" << 0.1;

  QTest::newRow("Offset only: less than min")
    << 1.0 << 42.19 << -510.0 << -200. << "-200.00" << -157.81;
  QTest::newRow("Offset only: more than max")
    << 1.0 << -42.19 << 65010.0 << 200. << "200.00" << 157.81;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only")
    << 5.0 << 0.0 << 5.0 << 1.0 << "1.00" << 5.0;

  QTest::newRow("Coeff only: less than min")
    << 5.0 << 0.0 << -1010.0 << -200. << "-200.00" << -1000.;
  QTest::newRow("Coeff only: more than max")
    << 5.0 << 0.0 << 65010.0 << 200.0 << "200.00" << 1000.0;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 12.0 << 42.0 << 6.0 << "6.00" << 42.0;

  QTest::newRow("Linear: less than min")
    << 5.0 << 12.0 << -5010.0 << -200. << "-200.00" << -988.;
  QTest::newRow("Linear: more than max")
    << 5.0 << 12.0 << 65010.0 << 200.00 << "200.00" << 1012.;
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxValueProxyTester::testSetCoefficient()
{
  ctkDoubleSpinBox spinBox;
  spinBox.setRange(-10000., 10000.);
  spinBox.setValue(10.12);

  ctkLinearValueProxy proxy;
  proxy.setCoefficient(10.);
  spinBox.setValueProxy(&proxy);

  QCOMPARE(spinBox.value(), 10.12);
  QCOMPARE(spinBox.displayedValue(), 101.2);

  QFETCH(double, newCoefficient);
  proxy.setCoefficient(newCoefficient);

  QFETCH(double, expectedValue);
  QFETCH(double, expectedDisplayedValue);
  QCOMPARE(spinBox.value(), expectedValue);
  QCOMPARE(spinBox.displayedValue(), expectedDisplayedValue);
}

//-----------------------------------------------------------------------------
void ctkDoubleSpinBoxValueProxyTester::testSetCoefficient_data()
{
  QTest::addColumn<double>("newCoefficient");
  QTest::addColumn<double>("expectedValue");
  QTest::addColumn<double>("expectedDisplayedValue");

  QTest::newRow("100") << 100.0 << 10.12 << 1012.;
  QTest::newRow("10") << 10.0 << 10.12 << 101.2;
  QTest::newRow("1") << 1.0 << 10.12 << 10.12;
  QTest::newRow("0.10") << 0.1 << 10.12 << 1.01;
  QTest::newRow("-10") << -10.0 << 10.12 << -101.2;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkDoubleSpinBoxValueProxyTest)
#include "moc_ctkDoubleSpinBoxValueProxyTest.cpp"
