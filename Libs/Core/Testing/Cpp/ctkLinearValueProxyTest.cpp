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

// QT includes
#include <QSignalSpy>

// CTK includes
#include "ctkLinearValueProxy.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkLinearValueProxyTester: public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void testSimpleSetValue();
  void testSimpleSetValue_data();

  void testCoefficient();
  void testCoefficient_data();

  void testOffset();
  void testOffset_data();

  void testSetValue();
  void testSetValue_data();

  void testSetValueNullCoeff();
  void testSetValueNullCoeff_data();

  void testSetProxyValue();
  void testSetProxyValue_data();

  void testSetProxyValueNullCoeff();
  void testSetProxyValueNullCoeff_data();

  void testProxyModified();
  void testProxyModified_data();
};

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSimpleSetValue()
{
  ctkLinearValueProxy proxy;
  proxy.setValue(28.1358);

  QSignalSpy valueSpy(&proxy, SIGNAL(valueChanged(double)));
  QSignalSpy proxyValueSpy(&proxy, SIGNAL(proxyValueChanged(double)));

  QFETCH(double, value);
  proxy.setValue(value);

  ctkTest::COMPARE(proxy.value(), value);
  ctkTest::COMPARE(proxy.proxyValue(), value);

  ctkTest::COMPARE(valueSpy.count(), 1);
  double valueFromSpy = valueSpy.takeFirst().at(0).toDouble();
  ctkTest::COMPARE(valueFromSpy, value);

  ctkTest::COMPARE(proxyValueSpy.count(), 1);
  double proxyValueFromSpy = proxyValueSpy.takeFirst().at(0).toDouble();
  ctkTest::COMPARE(proxyValueFromSpy, value);
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSimpleSetValue_data()
{
  QTest::addColumn<double>("value");

  QTest::newRow("Null value") << 0.0;
  QTest::newRow("Very very small value") << 1e-26;
  QTest::newRow("Not so small value") << 1e-6;
  QTest::newRow("Max value") <<std::numeric_limits<double>::max();
  QTest::newRow("Min value") <<std::numeric_limits<double>::min();
  QTest::newRow("Infinity") <<std::numeric_limits<double>::infinity();
  QTest::newRow(" - Infinity") << - std::numeric_limits<double>::infinity();
  QTest::newRow("Nan") << - std::numeric_limits<double>::quiet_NaN();
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testCoefficient()
{
  ctkLinearValueProxy proxy;
  proxy.setValue(13.2);

  QSignalSpy valueSpy(&proxy, SIGNAL(valueChanged(double)));
  QSignalSpy proxyValueSpy(&proxy, SIGNAL(proxyValueChanged(double)));

  QFETCH(double, coefficient);
  proxy.setCoefficient(coefficient);

  QFETCH(double, expectedProxyValue);
  ctkTest::COMPARE(proxy.proxyValue(), expectedProxyValue);
  ctkTest::COMPARE(proxy.value(), 13.2);

  ctkTest::COMPARE(valueSpy.count(), 0);
  ctkTest::COMPARE(proxyValueSpy.count(), proxy.proxyValue() != 13.2 ? 1 : 0);
  if (proxyValueSpy.count())
    {
    double proxyValueFromSpy = proxyValueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(proxyValueFromSpy, expectedProxyValue);
    }
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testCoefficient_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("expectedProxyValue");

  QTest::newRow("Null coeff") << 0.0 << 0.0;
  QTest::newRow("Very very small coeff") << 1e-26 << 1.32e-25;
  QTest::newRow("Not so small coeff") << 1e-6 << 1.32e-5;
  QTest::newRow("Normal coeff") << 2.0 << 26.4;
  QTest::newRow("Negative coeff") << -2.0 << -26.4;
  QTest::newRow("Large coeff") << 123456.0 << 1629619.2;
  QTest::newRow("Very very large coeff") << 1e26 << 13.2 * 1e26;
  QTest::newRow("unit coeff") << 1.0 << 13.2;
  QTest::newRow("same coeff") << 1.0 << 13.2;
  QTest::newRow("Max coeff") << std::numeric_limits<double>::max()
    << std::numeric_limits<double>::infinity();
  QTest::newRow("Min coeff") << - std::numeric_limits<double>::max()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Infinity coeff") << std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity();
  QTest::newRow(" - Infinity coeff")
    << - std::numeric_limits<double>::infinity()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Nan coeff") << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN();
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testOffset()
{
  ctkLinearValueProxy proxy;
  proxy.setValue(13.2);

  QSignalSpy valueSpy(&proxy, SIGNAL(valueChanged(double)));
  QSignalSpy proxyValueSpy(&proxy, SIGNAL(proxyValueChanged(double)));

  QFETCH(double, offset);
  proxy.setOffset(offset);

  QFETCH(double, expectedProxyValue);
  ctkTest::COMPARE(proxy.proxyValue(), expectedProxyValue);
  ctkTest::COMPARE(proxy.value(), 13.2);

  ctkTest::COMPARE(valueSpy.count(), 0);
  ctkTest::COMPARE(proxyValueSpy.count(), proxy.proxyValue() != 13.2 ? 1 : 0);
  if (proxyValueSpy.count())
    {
    double proxyValueFromSpy = proxyValueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(proxyValueFromSpy, expectedProxyValue);
    }
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testOffset_data()
{
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("expectedProxyValue");

  QTest::newRow("Very very small offset") << 1 + 1e-26 << 14.2 + 1e-26;
  QTest::newRow("Not so small offset") << 1e-5 << 13.20001;
  QTest::newRow("Normal offset") << -2.0 << 11.2;
  QTest::newRow("Null offset") << 0.0 << 13.2;
  QTest::newRow("Null offset - again") << 0.0 << 13.2;
  QTest::newRow("Max offset") << std::numeric_limits<double>::max()
    << std::numeric_limits<double>::max() + 13.2;
  QTest::newRow("Min offset") << - std::numeric_limits<double>::max()
    << - std::numeric_limits<double>::max() + 13.2;
  QTest::newRow("Infinity offset") << std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity();
  QTest::newRow(" - Infinity offset") << - std::numeric_limits<double>::infinity()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Nan offset") << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN();
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSetValue()
{
  ctkLinearValueProxy proxy;
  QFETCH(double, offset);
  proxy.setCoefficient(3.1);
  proxy.setOffset(offset);
  proxy.setValue(13.2);

  QSignalSpy valueSpy(&proxy, SIGNAL(valueChanged(double)));
  QSignalSpy proxyValueSpy(&proxy, SIGNAL(proxyValueChanged(double)));

  QFETCH(double, value);
  proxy.setValue(value);

  QFETCH(double, expectedProxyValue);
  ctkTest::COMPARE(proxy.proxyValue(), expectedProxyValue);
  ctkTest::COMPARE(proxy.value(), value);

  ctkTest::COMPARE(valueSpy.count(), proxy.value() != 13.2 ? 1 : 0);
  if (valueSpy.count())
    {
    double valueFromSpy = valueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(valueFromSpy, value);
    }

  ctkTest::COMPARE(proxyValueSpy.count(), proxy.proxyValue() != 0.0 ? 1 : 0);
  if (proxyValueSpy.count())
    {
    double proxyValueFromSpy = proxyValueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(proxyValueFromSpy, expectedProxyValue);
    }
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSetValue_data()
{
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("expectedProxyValue");

  QTest::newRow("Linear transform: normal input") << -19.53 << 6.62 << 0.992;
  QTest::newRow("Linear transform: max")
    << -19.53 << std::numeric_limits<double>::max()
    << 3.1*std::numeric_limits<double>::max() - 19.53;
  QTest::newRow("Linear transform: min")
    << -19.53 << - std::numeric_limits<double>::max()
    << -3.1*std::numeric_limits<double>::max() - 19.53;
  QTest::newRow("Linear transform: infinity")
    << -19.53 << std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity();
  QTest::newRow("Linear transform: - infinity")
    << -19.53 << - std::numeric_limits<double>::infinity()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Linear transform: Nan")
    << -19.53 << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN();

  QTest::newRow("Null offset: normal input") << 0.0 << 6.62 << 20.522;
  QTest::newRow("Null offset: max")
    << 0.0 << std::numeric_limits<double>::max()
    << 3.1*std::numeric_limits<double>::max();
  QTest::newRow("Null offset: min")
    << 0.0 << - std::numeric_limits<double>::max()
    << -3.1*std::numeric_limits<double>::max();
  QTest::newRow("Null offset: infinity")
    << 0.0 << std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity();
  QTest::newRow("Null offset: - infinity")
    << 0.0 << - std::numeric_limits<double>::infinity()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Null offset: Nan")
    << 0.0 << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN();
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSetValueNullCoeff()
{
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(0.0);
  proxy.setOffset(-19.53);
  proxy.setValue(13.2);

  QSignalSpy valueSpy(&proxy, SIGNAL(valueChanged(double)));
  QSignalSpy proxyValueSpy(&proxy, SIGNAL(proxyValueChanged(double)));

  QFETCH(double, value);
  proxy.setValue(value);

  QFETCH(double, expectedProxyValue);
  ctkTest::COMPARE(proxy.proxyValue(), expectedProxyValue);
  ctkTest::COMPARE(proxy.value(), value);

  ctkTest::COMPARE(valueSpy.count(), proxy.value() != 13.2 ? 1 : 0);
  if (valueSpy.count())
    {
    double valueFromSpy = valueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(valueFromSpy, value);
    }

  QFETCH(bool, shouldExpectProxyValue);
  ctkTest::COMPARE(proxyValueSpy.count(), shouldExpectProxyValue ? 1 : 0);
  if (proxyValueSpy.count())
    {
    double proxyValueFromSpy = proxyValueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(proxyValueFromSpy, expectedProxyValue);
    }
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSetValueNullCoeff_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<bool>("shouldExpectProxyValue");
  QTest::addColumn<double>("expectedProxyValue");

  QTest::newRow("Null coeff: normal input") << 6.62 << false << -19.53;
  QTest::newRow("Null coeff: max")
    << std::numeric_limits<double>::max() << false << -19.53;;
  QTest::newRow("Null coeff: min")
    << - std::numeric_limits<double>::max() << false << -19.53;
  QTest::newRow("Null coeff: infinity")
    << std::numeric_limits<double>::infinity() << true
    << std::numeric_limits<double>::quiet_NaN();
  QTest::newRow("Null coeff: - infinity")
    << - std::numeric_limits<double>::infinity() << true
    << std::numeric_limits<double>::quiet_NaN();
  QTest::newRow("Null coeff: Nan") << std::numeric_limits<double>::quiet_NaN()
    << true << std::numeric_limits<double>::quiet_NaN();
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSetProxyValue()
{
  ctkLinearValueProxy proxy;
  QFETCH(double, offset);
  proxy.setCoefficient(3.1);
  proxy.setOffset(offset);
  proxy.setProxyValue(13.2);
  double oldValue = proxy.value();

  QSignalSpy valueSpy(&proxy, SIGNAL(valueChanged(double)));
  QSignalSpy proxyValueSpy(&proxy, SIGNAL(proxyValueChanged(double)));

  QFETCH(double, proxyValue);
  proxy.setProxyValue(proxyValue);

  QFETCH(double, expectedValue);
  ctkTest::COMPARE(proxy.proxyValue(), proxyValue);
  ctkTest::COMPARE(proxy.value(), expectedValue);

  ctkTest::COMPARE(valueSpy.count(), proxy.value() != oldValue ? 1 : 0);
  if (valueSpy.count())
    {
    double valueFromSpy = valueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(valueFromSpy, expectedValue);
    }

  ctkTest::COMPARE(proxyValueSpy.count(), proxy.proxyValue() != 13.2 ? 1 : 0);
  if (proxyValueSpy.count())
    {
    double proxyValueFromSpy = proxyValueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(proxyValueFromSpy, proxyValue);
    }
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSetProxyValue_data()
{
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("proxyValue");
  QTest::addColumn<double>("expectedValue");

  QTest::newRow("Linear transform: normal input") << -19.53 << 1.55 << 6.8;
  QTest::newRow("Linear transform: max")
    << -19.53 << std::numeric_limits<double>::max()
    << (std::numeric_limits<double>::max() + 19.53) / 3.1;
  QTest::newRow("Linear transform: min")
    << -19.53 << - std::numeric_limits<double>::max()
    << ( - std::numeric_limits<double>::max() + 19.53) / 3.1;
  QTest::newRow("Linear transform: infinity")
    << -19.53 << std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity();
  QTest::newRow("Linear transform: - infinity")
    << -19.53 << - std::numeric_limits<double>::infinity()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Linear transform: Nan")
    << -19.53 << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN();

  QTest::newRow("Null offset: normal input") << 0.0 << 232.5 << 75.0;
  QTest::newRow("Null offset: max")
    << 0.0 << std::numeric_limits<double>::max()
    << std::numeric_limits<double>::max() / 3.1;
  QTest::newRow("Null offset: min")
    << 0.0 << - std::numeric_limits<double>::max()
    << -std::numeric_limits<double>::max() / 3.1;
  QTest::newRow("Null offset: infinity")
    << 0.0 << std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity();
  QTest::newRow("Null offset: - infinity")
    << 0.0 << - std::numeric_limits<double>::infinity()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Null offset: Nan")
    << 0.0 << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN();
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSetProxyValueNullCoeff()
{
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(0.0);
  proxy.setOffset(-19.53);
  proxy.setValue(13.2);

  QSignalSpy valueSpy(&proxy, SIGNAL(valueChanged(double)));
  QSignalSpy proxyValueSpy(&proxy, SIGNAL(proxyValueChanged(double)));

  QFETCH(double, proxyValue);
  proxy.setProxyValue(proxyValue);

  QFETCH(double, expectedValue);
  ctkTest::COMPARE(proxy.proxyValue(), proxyValue);
  ctkTest::COMPARE(proxy.value(), expectedValue);

  ctkTest::COMPARE(valueSpy.count(), proxy.value() != 13.2 ? 1 : 0);
  if (valueSpy.count())
    {
    double valueFromSpy = valueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(valueFromSpy, expectedValue);
    }

  ctkTest::COMPARE(proxyValueSpy.count(), proxy.proxyValue() != 0.0 ? 1 : 0);
  if (proxyValueSpy.count())
    {
    double proxyValueFromSpy = proxyValueSpy.takeFirst().at(0).toDouble();
    ctkTest::COMPARE(proxyValueFromSpy, proxyValue);
    }
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testSetProxyValueNullCoeff_data()
{
  QTest::addColumn<double>("proxyValue");
  QTest::addColumn<double>("expectedValue");

  QTest::newRow("Null coeff: normal input") << 6.62
    << std::numeric_limits<double>::infinity();
  QTest::newRow("Null coeff: normal negative input") << -398.6
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Null coeff: max")
    << std::numeric_limits<double>::max()
    << std::numeric_limits<double>::infinity();
  QTest::newRow("Null coeff: min")
    << - std::numeric_limits<double>::max()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Null coeff: infinity")
    << std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity();
  QTest::newRow("Null coeff: - infinity")
    << - std::numeric_limits<double>::infinity()
    << - std::numeric_limits<double>::infinity();
  QTest::newRow("Null coeff: Nan") << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN();
}


// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testProxyModified()
{
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(5.0);
  proxy.setOffset(5.0);

  QSignalSpy proxyAboutToBeModifiedSpy(&proxy, SIGNAL(proxyAboutToBeModified()));
  QSignalSpy proxyModifiedSpy(&proxy, SIGNAL(proxyModified()));

  QFETCH(bool, changeCoefficient);
  QFETCH(double, coefficientOrOffset);
  if (changeCoefficient)
    {
    proxy.setCoefficient(coefficientOrOffset);
    }
  else
    {
    proxy.setOffset(coefficientOrOffset);
    }
  QFETCH(int, expectedSignalCount);
  ctkTest::COMPARE(proxyAboutToBeModifiedSpy.count(), expectedSignalCount);
  ctkTest::COMPARE(proxyModifiedSpy.count(), expectedSignalCount);
}

// ----------------------------------------------------------------------------
void ctkLinearValueProxyTester::testProxyModified_data()
{
  QTest::addColumn<double>("coefficientOrOffset");
  QTest::addColumn<bool>("changeCoefficient");
  QTest::addColumn<int>("expectedSignalCount");

  QTest::newRow("change coefficient") << 10.0 << true << 1;
  QTest::newRow("same coefficient") << 5.0 << true << 0;
  QTest::newRow("null coefficient") << 0.0 << true << 1;
  QTest::newRow("change offset") << 10.0 << false << 1;
  QTest::newRow("same offset") << 5.0 << false << 0;
  QTest::newRow("null offset") << 0.0 << false << 1;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkLinearValueProxyTest)
#include "moc_ctkLinearValueProxyTest.cpp"
