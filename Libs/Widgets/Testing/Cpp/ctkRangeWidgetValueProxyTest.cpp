/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

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
#include "ctkRangeWidget.h"
#include "ctkLinearValueProxy.h"
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

  void testSetMinValue();
  void testSetMinValue_data();

  void testSetMaxValue();
  void testSetMaxValue_data();

  void testSetValues();
  void testSetValues_data();

private:
  void testSetValueCommonData();
  void testSetPositionCommonData();
};

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetValues()
{
  // Setup
  ctkRangeWidget ranger;
  ranger.setMinimum(-200);
  ranger.setMaximum(200);
  ranger.setSingleStep(0.01);
  ranger.setMinimumValue(-32.6);
  ranger.setMaximumValue(32.6);

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
  ctkTest::COMPARE(ranger.minimumValue(), expectedMin);
  ctkTest::COMPARE(ranger.maximumValue(), expectedMax);
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
  QTest::newRow("Offset only") << 1.0 << 49.19 << 0.1 << 0.1 << 0.2 << 0.2;

  QTest::newRow("Offset only: max+offset < min+offset < -200") << 1.0 << -42.19
    << -160.0 << -157.81
    << -190.9 << -157.81;
  QTest::newRow("Offset only: max+offset < -200 < min+offset") << 1.0 << -42.19
    << -0.1 << -157.81
    << -160.9 << -0.1;
  QTest::newRow("Offset only: -200 < max+offset < min+offset") << 1.0 << 42.19
    << -0.1 << -130.9
    << -130.9 << -0.1;

  QTest::newRow("Offset only: 200 < max+offset < min+offset") << 1.0 << 42.19
    << 160.0 << 157.81
    << 190.9 << 157.81;
  QTest::newRow("Offset only: max+offset < 200 < min+offset") << 1.0 << 42.19
    << 160.9 << -0.9
    << -0.9 << 157.81;
  QTest::newRow("Offset only: max+offset < min+offset < 200") << 1.0 << 42.19
    << 130.6 << -13.9
    << -13.9 << 130.6;

  QTest::newRow("Offset only: 200 < max = max_double = min = max_double")
    << 1.0 << 42.19
    << std::numeric_limits<double>::max() << 157.81
    << std::numeric_limits<double>::max() << 157.81;
  QTest::newRow("Offset only: max = -max_double < -200 < 200 < min = max_double")
    << 1.0 << 42.19
    << std::numeric_limits<double>::max() << -242.19
    << -std::numeric_limits<double>::max() << 157.81;
  QTest::newRow("Offset only: max = -max_double = min = -max_double < -200")
    << 1.0 << 42.19
    << - std::numeric_limits<double>::max() << -242.19
    << - std::numeric_limits<double>::max() << -242.19;

  QTest::newRow("Offset only: 200 < max = infinity = min = infinity")
    << 1.0 << 42.19
    << std::numeric_limits<double>::infinity() << 157.81
    << std::numeric_limits<double>::infinity() << 157.81;
  QTest::newRow("Offset only: max = -infinity < -200 < 200 < min = infinity")
    << 1.0 << 42.19
    << std::numeric_limits<double>::infinity() << -242.19
    << -std::numeric_limits<double>::infinity() << 157.81;
  QTest::newRow("Offset only: max = -infinity = min = -infinity < -200")
    << 1.0 << 42.19
    << - std::numeric_limits<double>::infinity() << -242.19
    << - std::numeric_limits<double>::infinity() << -242.19;

  QTest::newRow("Offset only: max = min = NaN")
    << 1.0 << 42.19
    << std::numeric_limits<double>::quiet_NaN() << 157.81
    << std::numeric_limits<double>::quiet_NaN() << 157.81;
  QTest::newRow("Offset only: max = NaN && min > 200")
    << 1.0 << 42.19
    << 630.0 << 157.81
    << std::numeric_limits<double>::quiet_NaN() << 157.81;
  QTest::newRow("Offset only: min = NaN && max < -200")
    << 1.0 << 42.19
    << std::numeric_limits<double>::quiet_NaN() << -74.79
    << -794348.12 << -74.79;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << 0.1 << 0.1 << 0.2 << 0.2;

  QTest::newRow("Coeff only: max*coeff < min*coeff < -200") << 5.0 << 0.0
    << -160.0 << -40.0
    << -190.9 << -40.0;
  QTest::newRow("Coeff only: max*coeff < -200 < min*coeff") << 5.0 << 0.0
    << -0.1 << -40.0
    << -160.9 << -0.1;
  QTest::newRow("Coeff only: -200 < max*coeff < min*coeff") << 5.0 << 0.0
    << -0.1 << -20.9
    << -20.9 << -0.1;

  QTest::newRow("Coeff only: 200 < max*coeff < min*coeff") << 5.0 << 0.0
    << 160.0 << 40.0
    << 190.9 << 40.0;
  QTest::newRow("Coeff only: max*coeff < 200 < min*coeff") << 5.0 << 0.0
    << 160.9 << -0.9
    << -0.9 << 40.00;
  QTest::newRow("Coeff only: max*coeff < min*coeff < 200") << 5.0 << 0.0
    << 13.6 << -13.9
    << -13.9 << 13.6;

  QTest::newRow("Coeff only: 200 < max = max_double = min = max_double")
    << 5.0 << 0.0
    << std::numeric_limits<double>::max() << 40.0
    << std::numeric_limits<double>::max() << 40.0;
  QTest::newRow("Coeff only: max = -max_double < -200 < 200 < min = max_double")
    << 5.0 << 0.0
    << std::numeric_limits<double>::max() << -40.0
    << - std::numeric_limits<double>::max() << 40.0;
  QTest::newRow("Coeff only: max = -max_double = min = -max_double < -200")
    << 5.0 << 0.0
    << -std::numeric_limits<double>::max() << -40.0
    << -std::numeric_limits<double>::max() << -40.0;

  QTest::newRow("Coeff only: 200 < max = infinity = min = infinity")
    << 5.0 << 0.0
    << std::numeric_limits<double>::infinity() << 40.0
    << std::numeric_limits<double>::infinity() << 40.0;
  QTest::newRow("Coeff only: max = -infinity < -200 < 200 < min = infinity")
    << 5.0 << 0.0
    << std::numeric_limits<double>::infinity() << -40.0
    << -std::numeric_limits<double>::infinity() << 40.0;
  QTest::newRow("Coeff only: max = -infinity = min = -infinity < -200")
    << 5.0 << 0.0
    << - std::numeric_limits<double>::infinity() << -40.0
    << - std::numeric_limits<double>::infinity() << -40.0;

  QTest::newRow("Coeff only: max = min = NaN")
    << 5.0 << 0.0
    << std::numeric_limits<double>::quiet_NaN() << 40.0
    << std::numeric_limits<double>::quiet_NaN() << 40.0;
  QTest::newRow("Coeff only: max = NaN && min > 200")
    << 5.0 << 0.0
    << 630.0 << 40.0
    << std::numeric_limits<double>::quiet_NaN() << 40.0;
  QTest::newRow("Coeff only: min = NaN && max < -200")
    << 5.0 << 0.0
    << std::numeric_limits<double>::quiet_NaN() << -6.52
    << -794348.12 << -6.52;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 12.0 << 0.1 << 0.1 << 0.2 << 0.2;

  QTest::newRow("Linear:f(max) < f(min) < -200") << 5.0 << 12.0
    << -160.0 << -42.4
    << -190.9 << -42.4;
  QTest::newRow("Linear: f(max) < -200 < f(min)") << 5.0 << 12.0
    << -0.1 << -42.4
    << -160.9 << -0.1;
  QTest::newRow("Linear: -200 < f(max) < f(min)") << 5.0 << 12.0
    << -0.1 << -20.9
    << -20.9 << -0.1;

  QTest::newRow("Linear: 200 < f(max) < f(min)") << 5.0 << 12.0
    << 160.0 << 37.6
    << 190.9 << 37.6;
  QTest::newRow("Linear: f(max) < 200 < f(min)") << 5.0 << 12.0
    << 160.9 << -0.9
    << -0.9 << 37.6;
  QTest::newRow("Linear: f(max) < f(min) < 200") << 5.0 << 12.0
    << 13.6 << -13.9
    << -13.9 << 13.6;

  QTest::newRow("Linear: 200 < max = max_double = min = max_double")
    << 5.0 << 12.0
    << std::numeric_limits<double>::max() << 37.6
    << std::numeric_limits<double>::max() << 37.6;
  QTest::newRow("Linear: max = -max_double < -200 < 200 < min = max_double")
    << 5.0 << 12.0
    << std::numeric_limits<double>::max() << -42.4
    << - std::numeric_limits<double>::max() << 37.6;
  QTest::newRow("Linear: max = -max_double = min = -max_double < -200")
    << 5.0 << 12.0
    << -std::numeric_limits<double>::max() << -42.4
    << -std::numeric_limits<double>::max() << -42.4;

  QTest::newRow("Linear: 200 < max = infinity = min = infinity")
    << 5.0 << 12.0
    << std::numeric_limits<double>::infinity() << 37.6
    << std::numeric_limits<double>::infinity() << 37.6;
  QTest::newRow("Linear: max = -infinity < -200 < 200 < min = infinity")
    << 5.0 << 12.0
    << std::numeric_limits<double>::infinity() << -42.4
    << -std::numeric_limits<double>::infinity() << 37.6;
  QTest::newRow("Linear: max = -infinity = min = -infinity < -200")
    << 5.0 << 12.0
    << - std::numeric_limits<double>::infinity() << -42.4
    << - std::numeric_limits<double>::infinity() << -42.4;

  QTest::newRow("Linear: max = min = NaN")
    << 5.0 << 12.0
    << std::numeric_limits<double>::quiet_NaN() << 37.6
    << std::numeric_limits<double>::quiet_NaN() << 37.6;
  QTest::newRow("Linear: max = NaN && f(min) > 200")
    << 5.0 << 12.0
    << 630.0 << 37.6
    << std::numeric_limits<double>::quiet_NaN() << 37.6;
  QTest::newRow("Linear: min = NaN && f(max) < -200")
    << 5.0 << 12.0
    << std::numeric_limits<double>::quiet_NaN() << -8.92
    << -794348.12 << -8.92;
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetValueProxyTester::testSetMinValue()
{
  // Setup
  ctkRangeWidget ranger;
  ranger.setMinimum(-200);
  ranger.setMaximum(200);
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
  ctkTest::COMPARE(ranger.minimumValue(), expectedValue);
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
  QTest::newRow("Offset only") << 1.0 << 42.19 << 0.1 << 0.1;

  QTest::newRow("Offset only: less than min")
    << 1.0 << 42.19 << -510.0 << -242.19;
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1.0 << 42.19 << -230.0 << -230.0;
  QTest::newRow("Offset only: less than min with offset")
    << 1.0 << -42.19 << -190.0 << -157.81;

  QTest::newRow("Offset only: more than max with offset")
    << 1.0 << 42.19 << 160.0 << 157.81;
  QTest::newRow("Offset only: more than max")
    << 1.0 << -42.19 << 65010.0 << 242.19;
  QTest::newRow("Offset only: less than max but ok with offset")
    << 1.0 << -42.19 << 229.1 << 229.1;

  QTest::newRow("Offset only: max")
    << 1.0 << 42.19 << std::numeric_limits<double>::max() << 157.81;
  QTest::newRow("Offset only:  min")
    << 1.0 << 42.19 << -std::numeric_limits<double>::max() << -242.19;
  QTest::newRow("Offset only: infinity")
    << 1.0 << 42.19 << std::numeric_limits<double>::infinity() << 157.81;
  QTest::newRow("Offset only:  - infinity")
    << 1.0 << 42.19 << -std::numeric_limits<double>::infinity() << -242.19;
  QTest::newRow("Offset only: Nan")
    << 1.0 << 42.19 << std::numeric_limits<double>::quiet_NaN() << 157.81;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << 0.1 << 0.1;

  QTest::newRow("Coeff only: less than min")
    << 5.0 << 0.0 << -510.0 << -40.0;
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0.0 << -230.0 << -230.0;
  QTest::newRow("Coeff only: less than min with coeff")
    << 5.0 << 0.0 << -190.0 << -40.0;

  QTest::newRow("Coeff only: more than max with coeff")
    << 5.0 << 0.0 << 160.0 << 40.0;
  QTest::newRow("Coeff only: more than max")
    << 5.0 << 0.0 << 65010.0 << 40.0;
  QTest::newRow("Offset only: less than max but ok with coeff")
    << 0.5 << 0.0 << 229.2 << 229.2;

  QTest::newRow("Coeff only: max")
    << 5.0 << 0.0 << std::numeric_limits<double>::max() << 40.0;
  QTest::newRow("Coeff only:  min")
    << 5.0 << 0.0 << -std::numeric_limits<double>::max() << -40.0;
  QTest::newRow("Coeff only: infinity")
    << 5.0 << 0.0 << std::numeric_limits<double>::infinity() << 40.0;
  QTest::newRow("Coeff only:  - infinity")
    << 5.0 << 0.0 << -std::numeric_limits<double>::infinity() << -40.0;
  QTest::newRow("Coeff only: Nan")
    << 5.0 << 0.0 << std::numeric_limits<double>::quiet_NaN() << 40.0;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 0.0 << 0.1 << 0.1;

  QTest::newRow("Linear: less than min")
    << 5.0 << 12.0 << -510.0 << -42.4;
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12.0 << -230.0 << -230.0;
  QTest::newRow("Linear: less than min with function")
    << 5.0 << 12.0 << -61.5 << -42.4;

  QTest::newRow("Linear: more than max with function")
    << 5.0 << 12.0 << 160.0 << 37.6;
  QTest::newRow("Linear: more than max")
    << 5.0 << 12.0 << 65010.0 << 37.6;
  QTest::newRow("Offset only: less than max but ok with function")
    << 0.5 << 12.0 << 229.2 << 229.2;

  QTest::newRow("Linear: max")
    << 5.0 << 12.0 << std::numeric_limits<double>::max() << 37.6;
  QTest::newRow("Linear:  min")
    << 5.0 << 12.0 << -std::numeric_limits<double>::max() << -42.4;
  QTest::newRow("Linear: infinity")
    << 5.0 << 12.0 << std::numeric_limits<double>::infinity() << 37.6;
  QTest::newRow("Linear:  - infinity")
    << 5.0 << 12.0 << -std::numeric_limits<double>::infinity() << -42.4;
  QTest::newRow("Linear: Nan")
    << 5.0 << 12.0 << std::numeric_limits<double>::quiet_NaN() << 37.6;
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
    << 1.0 << 42.19 << -510.0 << -32.6 - 42.19;
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1.0 << 42.19 << -230.0 << -32.6 - 42.19;
  QTest::newRow("Offset only: less than min with offset")
    << 1.0 << -42.19 << -190.0 << 9.59;

  QTest::newRow("Offset only: more than max with offset")
    << 1.0 << 42.19 << 160.0 << 157.81;
  QTest::newRow("Offset only: more than max")
    << 1.0 << -42.19 << 65010.0 << 242.19;
  QTest::newRow("Offset only: less than max but ok with offset")
    << 1.0 << -42.19 << 229.1 << 229.1;

  QTest::newRow("Offset only: max")
    << 1.0 << 42.19 << std::numeric_limits<double>::max() << 157.81;
  QTest::newRow("Offset only:  min")
    << 1.0 << 42.19 << -std::numeric_limits<double>::max() << -74.79;
  QTest::newRow("Offset only: infinity")
    << 1.0 << 42.19 << std::numeric_limits<double>::infinity() << 157.81;
  QTest::newRow("Offset only:  - infinity")
    << 1.0 << 42.19 << -std::numeric_limits<double>::infinity() << -74.79;
  QTest::newRow("Offset only: Nan")
    << 1.0 << 42.19 << std::numeric_limits<double>::quiet_NaN() << 157.81;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << 0.1 << 0.1;

  QTest::newRow("Coeff only: less than min")
    << 5.0 << 0.0 << -510.0 << -6.52;
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0.0 << -230.0 << -65.2;
  QTest::newRow("Coeff only: less than min with coeff")
    << 5.0 << 0.0 << -190.0 << -6.52;

  QTest::newRow("Coeff only: more than max with coeff")
    << 5.0 << 0.0 << 160.0 << 40.0;
  QTest::newRow("Coeff only: more than max")
    << 5.0 << 0.0 << 65010.0 << 40.0;
  QTest::newRow("Offset only: less than max but ok with coeff")
    << 0.5 << 0.0 << 229.2 << 229.2;

  QTest::newRow("Coeff only: max")
    << 5.0 << 0.0 << std::numeric_limits<double>::max() << 40.0;
  QTest::newRow("Coeff only:  min")
    << 5.0 << 0.0 << -std::numeric_limits<double>::max() << -6.52;
  QTest::newRow("Coeff only: infinity")
    << 5.0 << 0.0 << std::numeric_limits<double>::infinity() << 40.0;
  QTest::newRow("Coeff only:  - infinity")
    << 5.0 << 0.0 << -std::numeric_limits<double>::infinity() << -6.52;
  QTest::newRow("Coeff only: Nan")
    << 5.0 << 0.0 << std::numeric_limits<double>::quiet_NaN() << 40.0;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 0.0 << 0.1 << 0.1;

  QTest::newRow("Linear: less than min")
    << 5.0 << 12.0 << -510.0 << -8.92;
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12.0 << -230.0 << -89.2;
  QTest::newRow("Linear: less than min with function")
    << 5.0 << 12.0 << -61.5 << -8.92;

  QTest::newRow("Linear: more than max with function")
    << 5.0 << 12.0 << 160.0 << 37.6;
  QTest::newRow("Linear: more than max")
    << 5.0 << 12.0 << 65010.0 << 37.6;
  QTest::newRow("Offset only: less than max but ok with function")
    << 0.5 << 12.0 << 229.2 << 229.2;

  QTest::newRow("Linear: max")
    << 5.0 << 12.0 << std::numeric_limits<double>::max() << 37.6;
  QTest::newRow("Linear:  min")
    << 5.0 << 12.0 << -std::numeric_limits<double>::max() << -8.92;
  QTest::newRow("Linear: infinity")
    << 5.0 << 12.0 << std::numeric_limits<double>::infinity() << 37.6;
  QTest::newRow("Linear:  - infinity")
    << 5.0 << 12.0 << -std::numeric_limits<double>::infinity() << -8.92;
  QTest::newRow("Linear: Nan")
    << 5.0 << 12.0 << std::numeric_limits<double>::quiet_NaN() << 37.6;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkRangeWidgetValueProxyTest)
#include "moc_ctkRangeWidgetValueProxyTest.cpp"
