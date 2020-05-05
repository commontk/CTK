/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QTest>

// CTK includes
#include "ctkDoubleSpinBox.h"
#include "ctkLinearValueProxy.h"
#include "ctkRangeWidget.h"
#include "ctkTest.h"
#include "ctkValueProxy.h"

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
class CustomSpy : public QObject
{
  Q_OBJECT
public:
  CustomSpy()
    {
    this->AcknowledgedSignals = 0;
    }

public slots:
  void onValuesChanged(double min, double max)
    {
    ++this->AcknowledgedSignals;
    this->MinSpyData.append(min);
    this->MaxSpyData.append(max);
    }

public:
  void getSpyReport(double min, double max)
    {
    QCOMPARE(this->AcknowledgedSignals, 1);

    QCOMPARE(this->MinSpyData.size(), 1);
    ctkTest::COMPARE(this->MinSpyData[0], min);
    QCOMPARE(this->MaxSpyData.size(), 1);
    ctkTest::COMPARE(this->MaxSpyData[0], max);
    }

  QList<double> MinSpyData;
  QList<double> MaxSpyData;
  int AcknowledgedSignals;
};

} // end namespace

//-----------------------------------------------------------------------------
class ctkRangeWidgetValueProxyTester: public QObject
{
  Q_OBJECT
private slots:

  void testSetValues();
  void testSetValues_data();

  void testSetMinValue();
  void testSetMinValue_data();

  void testSetMaxValue();
  void testSetMaxValue_data();

  void testSetCoefficient();
  void testSetCoefficient_data();

private:
  void testSetValueCommonData();
  void testSetPositionCommonData();
};

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetValues()
{
  // Setup
  ctkRangeWidget ranger;
  ranger.setRange(-200., 200.);
  ranger.setSingleStep(0.01);
  ranger.setValues(-32.6, 32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  ranger.setValueProxy(&proxy);

  // \todo when valuesChanged is fixed
  // Spy
  //CustomSpy valuesSpy;
  //QObject::connect(&ranger, SIGNAL(valuesChanged(double, double)),
  //                 &valuesSpy, SLOT(onValuesChanged(double, double)));

  // Test
  QFETCH(double, min);
  QFETCH(double, max);
  ranger.setValues(min, max);

  QFETCH(double, expectedMin);
  QFETCH(double, expectedMax);
  //valuesSpy.getSpyReport(expectedMin, expectedMax);
  QCOMPARE(ranger.minimumValue(), expectedMin);
  QCOMPARE(ranger.maximumValue(), expectedMax);
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetValues_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("min");
  QTest::addColumn<double>("expectedMin");
  QTest::addColumn<double>("max");
  QTest::addColumn<double>("expectedMax");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1. << 49.19 << 0.1 << 0.1 << 0.2 << 0.2;

  QTest::newRow("Offset only: max+offset < min+offset < -200")
    << 1. << -42.19 << -160. << -190.9 << -190.9 << -160.;
  QTest::newRow("Offset only: max+offset < -200 < min+offset")
    << 1. << -42.19 << -0.1 << -160.9 << -160.9 << -0.1;
  QTest::newRow("Offset only: -200 < max+offset < min+offset")
    << 1. << 42.19<< -0.1 << -130.9 << -130.9 << -0.1;

  QTest::newRow("Offset only: 200 < max+offset < min+offset")
    << 1. << 42.19 << 160. << 160. << 190.9 << 190.9;
  QTest::newRow("Offset only: max+offset < 200 < min+offset")
    << 1. << 42.19 << 160.9 << -0.9 << -0.9 << 160.9;
  QTest::newRow("Offset only: max+offset < min+offset < 200")
    << 1. << 42.19 << 130.6 << -13.9 << -13.9 << 130.6;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5. << 0. << 0.1 << 0.1 << 0.2 << 0.2;

  QTest::newRow("Coeff only: max*coeff < min*coeff < -200")
    << 5. << 0. << -160. << -190.9 << -190.9 << -160.;
  QTest::newRow("Coeff only: max*coeff < -200 < min*coeff")
    << 5. << 0. << -0.1 << -160.9 << -160.9 << -0.1;
  QTest::newRow("Coeff only: -200 < max*coeff < min*coeff")
    << 5. << 0. << -0.1 << -20.9 << -20.9 << -0.1;

  QTest::newRow("Coeff only: 200 < max*coeff < min*coeff")
    << 5. << 0. << 160. << 160. << 190.9 << 190.9;
  QTest::newRow("Coeff only: max*coeff < 200 < min*coeff")
    << 5. << 0. << 160.9 << -0.9 << -0.9 << 160.9;
  QTest::newRow("Coeff only: max*coeff < min*coeff < 200")
    << 5. << 0. << 13.6 << -13.9 << -13.9 << 13.6;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 12. << 0.1 << 0.1 << 0.2 << 0.2;

  QTest::newRow("Linear:f(max) < f(min) < -200")
    << 5. << 12. << -160. << -190.9 << -190.9 << -160.;
  QTest::newRow("Linear: f(max) < -200 < f(min)")
    << 5. << 12. << -0.1 << -160.9 << -160.9 << -0.1;
  QTest::newRow("Linear: -200 < f(max) < f(min)")
    << 5. << 12. << -0.1 << -20.9 << -20.9 << -0.1;

  QTest::newRow("Linear: 200 < f(max) < f(min)")
    << 5. << 12. << 160.0 << 160.0 << 190.9 << 190.9;
  QTest::newRow("Linear: f(max) < 200 < f(min)")
    << 5. << 12. << 160.9 << -0.9 << -0.9 << 160.9;
  QTest::newRow("Linear: f(max) < f(min) < 200")
    << 5. << 12. << 13.6 << -13.9 << -13.9 << 13.6;

}

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetMinValue()
{
  // Setup
  ctkRangeWidget ranger;
  ranger.setRange(-200., 200.);
  ranger.setSingleStep(0.01);
  ranger.setMinimumValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  ranger.setValueProxy(&proxy);

  // \todo when valuesChanged is fixed
  // Spy
  //QSignalSpy valueSpy(&ranger, SIGNAL(minimumValueChanged(double)));

  // Test
  QFETCH(double, value);
  ranger.setMinimumValue(value);

  QFETCH(double, expectedValue);
  //getSpyReport(valueSpy, expectedValue);
  QCOMPARE(ranger.minimumValue(), expectedValue);
  QVERIFY(ranger.minimumValue() <= ranger.maximumValue());
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetMinValue_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("expectedValue");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1. << 42.19 << 0.1 << 0.1;

  QTest::newRow("Offset only: less than min")
    << 1. << 42.19 << -510. << -200.;
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1. << 42.19 << -230. << -200.;
  QTest::newRow("Offset only: less than min with offset")
    << 1. << -42.19 << -190. << -190.;

  QTest::newRow("Offset only: more than max with offset")
    << 1. << 42.19 << 160. << 99.;
  QTest::newRow("Offset only: more than max")
    << 1. << -42.19 << 65010.0 << 99.;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5. << 0. << 0.1 << 0.1;

  QTest::newRow("Coeff only: less than min")
    << 5. << 0. << -510. << -200.;
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0. << -230.0 << -200.0;
  QTest::newRow("Coeff only: less than min with coeff")
    << 5. << 0. << -190.0 << -190.0;

  QTest::newRow("Coeff only: more than max with coeff")
    << 5. << 0. << 160. << 99.;
  QTest::newRow("Coeff only: more than max")
    << 5. << 0. << 65010. << 99.;
  QTest::newRow("Coeff only: less than max but ok with coeff")
    << 0.5 << 0. << 229.2 << 99.;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5. << 0. << 0.1 << 0.1;

  QTest::newRow("Linear: less than min")
    << 5. << 12. << -510. << -200.;
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12. << -230. << -200.;
  QTest::newRow("Linear: less than min with function")
    << 5. << 12. << -61.5 << -61.5;

  QTest::newRow("Linear: more than max with function")
    << 5. << 12. << 160.0 << 99.;
  QTest::newRow("Linear: more than max")
    << 5. << 12. << 65010. << 99.;
  QTest::newRow("Linear: less than max but ok with function")
    << 0.5 << 12. << 229.2 << 99.;
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetMaxValue()
{
  // Setup
  ctkRangeWidget ranger;
  ranger.setMinimum(-200);
  ranger.setMaximum(200);
  ranger.setSingleStep(0.01);
  ranger.setMaximumValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  ranger.setValueProxy(&proxy);

  // \todo when valuesChanged is fixed
  // Spy
  //QSignalSpy valueSpy(&ranger, SIGNAL(maximumValueChanged(double)));

  // Test
  QFETCH(double, value);
  ranger.setMaximumValue(value);

  QFETCH(double, expectedValue);
  //getSpyReport(valueSpy, expectedValue);
  ctkTest::COMPARE(ranger.maximumValue(), expectedValue);
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetMaxValue_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("expectedValue");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1.0 << 42.19 << 0.1 << 0.1;

  QTest::newRow("Offset only: less than min")
    << 1. << 42.19 << -510. << -32.6;
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1. << 42.19 << -230. << -32.6;
  QTest::newRow("Offset only: less than min with offset")
    << 1. << -42.19 << -190. << -32.6;

  QTest::newRow("Offset only: more than max with offset")
    << 1. << 42.19 << 160. << 160.;
  QTest::newRow("Offset only: more than max")
    << 1. << -42.19 << 65010.0 << 200.;
  QTest::newRow("Offset only: less than max but ok with offset")
    << 1. << -42.19 << 229.1 << 200.;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0. << 0.1 << 0.1;

  QTest::newRow("Coeff only: less than min")
    << 5. << 0. << -510. << -32.6;
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0. << -230. << -32.6;
  QTest::newRow("Coeff only: less than min with coeff")
    << 5. << 0. << -190. << -32.6;

  QTest::newRow("Coeff only: more than max with coeff")
    << 5. << 0. << 160. << 160.;
  QTest::newRow("Coeff only: more than max")
    << 5. << 0. << 65010. << 200.;
  QTest::newRow("Offset only: less than max but ok with coeff")
    << 0.5 << 0. << 229.2 << 200.;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5. << 0. << 0.1 << 0.1;

  QTest::newRow("Linear: less than min")
    << 5. << 12. << -510. << -32.6;
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12. << -230. << -32.6;
  QTest::newRow("Linear: less than min with function")
    << 5. << 12. << -61.5 << -32.6;

  QTest::newRow("Linear: more than max with function")
    << 5. << 12. << 160. << 160.;
  QTest::newRow("Linear: more than max")
    << 5. << 12. << 65010.0 << 200.;
  QTest::newRow("Linear: less than max but ok with function")
    << 0.5 << 12. << 229.2 << 200.;
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetCoefficient()
{
  ctkRangeWidget rangeWidget;
  rangeWidget.setRange(-10000., 10000.);
  rangeWidget.setValues(10., 50.);

  ctkLinearValueProxy proxy;
  proxy.setCoefficient(10.);
  rangeWidget.setValueProxy(&proxy);

  QCOMPARE(rangeWidget.minimumValue(), 10.);
  QCOMPARE(rangeWidget.maximumValue(), 50.);
  QCOMPARE(rangeWidget.minimumSpinBox()->displayedValue(), 100.);
  QCOMPARE(rangeWidget.maximumSpinBox()->displayedValue(), 500.);

  QFETCH(double, newCoefficient);
  proxy.setCoefficient(newCoefficient);

  QFETCH(double, expectedMinimumDisplayedValue);
  QFETCH(double, expectedMaximumDisplayedValue);
  QCOMPARE(rangeWidget.minimumValue(), 10.);
  QCOMPARE(rangeWidget.maximumValue(), 50.);
  QCOMPARE(rangeWidget.minimumSpinBox()->displayedValue(),
                   expectedMinimumDisplayedValue);
  QCOMPARE(rangeWidget.maximumSpinBox()->displayedValue(),
                   expectedMaximumDisplayedValue);
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetCoefficient_data()
{
  QTest::addColumn<double>("newCoefficient");
  QTest::addColumn<double>("expectedMinimumDisplayedValue");
  QTest::addColumn<double>("expectedMaximumDisplayedValue");

  QTest::newRow("100") << 100.0 << 1000. << 5000.;
  QTest::newRow("10") << 10.0 << 100. << 500.;
  QTest::newRow("1") << 1.0 << 10. << 50.;
  QTest::newRow("0.10") << 0.1 << 1. << 5.;
  QTest::newRow("-10") << -10.0 << -100. << -500.;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkRangeWidgetValueProxyTest)
#include "moc_ctkRangeWidgetValueProxyTest.cpp"
