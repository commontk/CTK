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
#include "ctkDoubleRangeSlider.h"
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
class ctkDoubleRangeSliderValueProxyTester: public QObject
{
  Q_OBJECT
private slots:

  void testSetValues();
  void testSetValues_data();

  void testSetMinValue();
  void testSetMinValue_data() { testSetValueCommonData(); };

  void testSetMaxValue();
  void testSetMaxValue_data(){ testSetValueCommonData(); };

  void testSetMinPosition();
  void testSetMinPosition_data() { testSetPositionCommonData(); };

  void testSetMaxPosition();
  void testSetMaxPosition_data() { testSetPositionCommonData(); };

private:
  void testSetValueCommonData();
  void testSetPositionCommonData();
};

//-----------------------------------------------------------------------------
void ctkDoubleRangeSliderValueProxyTester::testSetValues()
{
  // Setup
  ctkDoubleRangeSlider slider;
  slider.setMinimum(-200);
  slider.setMaximum(200);
  slider.setSingleStep(0.01);
  slider.setMinimumValue(-32.6);
  slider.setMaximumValue(32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  slider.setValueProxy(&proxy);

  // Spy
  CustomSpy valuesSpy;
  QObject::connect(&slider, SIGNAL(valuesChanged(double, double)),
                   &valuesSpy, SLOT(onValuesChanged(double, double)));

  // Test
  QFETCH(double, min);
  QFETCH(double, max);
  slider.setValues(min, max);

  QFETCH(double, expectedMin);
  QFETCH(double, expectedMax);
  valuesSpy.getSpyReport(expectedMin, expectedMax);
  QCOMPARE(slider.minimumValue(), expectedMin);
  QCOMPARE(slider.maximumValue(), expectedMax);
}

//-----------------------------------------------------------------------------
void ctkDoubleRangeSliderValueProxyTester::testSetValues_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("min");
  QTest::addColumn<double>("expectedMin");
  QTest::addColumn<double>("max");
  QTest::addColumn<double>("expectedMax");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1.0 << 42.19176 << 0.1 << 0.1 << 0.2 << 0.2;

  QTest::newRow("Offset only: max+offset < min+offset < -200")
    << 1. << -42.19 << -160. << -190.9 << -190.9 << -160.;
  QTest::newRow("Offset only: max+offset < -200 < min+offset")
    << 1. << -42.19 << -0.1 << -160.9 << -160.9 << -0.1;
  QTest::newRow("Offset only: -200 < max+offset < min+offset")
    << 1. << 42.19 << -0.1 << -130.9 << -130.9 << -0.1;

  QTest::newRow("Offset only: 200 < max+offset < min+offset")
    << 1. << 42.19 << 160. << 160. << 190.9 << 190.9;
  QTest::newRow("Offset only: max+offset < 200 < min+offset")
    << 1. << 42.19 << 160.9 << -0.9 << -0.9 << 160.9;
  QTest::newRow("Offset only: max+offset < min+offset < 200")
    << 1. << 42.19 << 130.6 << -13.9 << -13.9 << 130.6;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << 0.1 << 0.1 << 0.2 << 0.2;

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
  QTest::newRow("Linear") << 5.0 << 12.0 << 0.1 << 0.1 << 0.2 << 0.2;

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
void ctkDoubleRangeSliderValueProxyTester::testSetMinValue()
{
  // Setup
  ctkDoubleRangeSlider slider;
  slider.setMinimum(-200);
  slider.setMaximum(200);
  slider.setSingleStep(0.01);
  slider.setMinimumValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  slider.setValueProxy(&proxy);

  // Spy
  QSignalSpy valueSpy(&slider, SIGNAL(minimumValueChanged(double)));

  // Test
  QFETCH(double, value);
  slider.setMinimumValue(value);

  QFETCH(double, expectedValue);
  getSpyReport(valueSpy, expectedValue);
  QCOMPARE(slider.minimumValue(), expectedValue);
}

//-----------------------------------------------------------------------------
void ctkDoubleRangeSliderValueProxyTester::testSetMaxValue()
{
  // Setup
  ctkDoubleRangeSlider slider;
  slider.setMinimum(-200);
  slider.setMaximum(200);
  slider.setSingleStep(0.01);
  slider.setMaximumValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  slider.setValueProxy(&proxy);

  // Spy
  QSignalSpy valueSpy(&slider, SIGNAL(maximumValueChanged(double)));

  // Test
  QFETCH(double, value);
  slider.setMaximumValue(value);

  QFETCH(double, expectedValue);
  getSpyReport(valueSpy, expectedValue);
  ctkTest::COMPARE(slider.maximumValue(), expectedValue);
}

//-----------------------------------------------------------------------------
void ctkDoubleRangeSliderValueProxyTester::testSetValueCommonData()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("expectedValue");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1.0 << 42.19176 << 0.1 << 0.1;

  QTest::newRow("Offset only: less than min")
    << 1. << 42.19 << -510. << -200.;
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1. << 42.19 << -230. << -200.;
  QTest::newRow("Offset only: less than min with offset")
    << 1. << -42.19 << -190. << -190.;

  QTest::newRow("Offset only: more than max with offset")
    << 1. << 42.19 << 160. << 160.;
  QTest::newRow("Offset only: more than max")
    << 1. << -42.19 << 65010.0 << 200.;
  QTest::newRow("Offset only: less than max but ok with offset")
    << 1. << -42.19 << 229.1 << 200.;

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << 0.1 << 0.1;

  QTest::newRow("Coeff only: less than min")
    << 5. << 0. << -510. << -200.;
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0. << -230. << -200.;
  QTest::newRow("Coeff only: less than min with coeff")
    << 5.0 << 0. << -190. << -190.;

  QTest::newRow("Coeff only: more than max with coeff")
    << 5. << 0. << 160. << 160.;
  QTest::newRow("Coeff only: more than max")
    << 5. << 0. << 65010. << 200.;
  QTest::newRow("Offset only: less than max but ok with coeff")
    << 0.5 << 0. << 229.2 << 200.;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 0.0 << 0.1 << 0.1;

  QTest::newRow("Linear: less than min")
    << 5. << 12. << -510.0 << -200.;
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12. << -230.0 << -200.;
  QTest::newRow("Linear: less than min with function")
    << 5. << 12. << -61.5 << -61.5;

  QTest::newRow("Linear: more than max with function")
    << 5. << 12. << 160. << 160.;
  QTest::newRow("Linear: more than max")
    << 5. << 12. << 65010. << 200.;
  QTest::newRow("Linear: less than max but ok with function")
    << 0.5 << 12. << 229.2 << 200.;
}

//-----------------------------------------------------------------------------
void ctkDoubleRangeSliderValueProxyTester::testSetMinPosition()
{
  // Setup
  ctkDoubleRangeSlider slider;
  slider.setMinimum(-200);
  slider.setMaximum(200);
  slider.setSingleStep(0.01);
  slider.setMinimumValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  slider.setValueProxy(&proxy);

  // Spy
  QSignalSpy valueSpy(&slider, SIGNAL(minimumValueChanged(double)));

  // Test
  QFETCH(double, sliderPosition);
  slider.setMinimumPosition(sliderPosition);

  QFETCH(double, expectedSliderPosition);
  QFETCH(double, expectedValue);
  getSpyReport(valueSpy, expectedValue);
  ctkTest::COMPARE(slider.minimumValue(), expectedValue);
  ctkTest::COMPARE(slider.minimumPosition(), expectedSliderPosition);
}

//-----------------------------------------------------------------------------
void ctkDoubleRangeSliderValueProxyTester::testSetMaxPosition()
{
  // Setup
  ctkDoubleRangeSlider slider;
  slider.setRange(-200., 200.);
  slider.setSingleStep(0.01);
  slider.setMaximumValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  slider.setValueProxy(&proxy);

  // Spy
  QSignalSpy valueSpy(&slider, SIGNAL(maximumValueChanged(double)));

  // Test
  QFETCH(double, sliderPosition);
  slider.setMaximumPosition(sliderPosition);

  QFETCH(double, expectedSliderPosition);
  QFETCH(double, expectedValue);
  getSpyReport(valueSpy, expectedValue);
  ctkTest::COMPARE(slider.maximumValue(), expectedValue);
  ctkTest::COMPARE(slider.maximumPosition(), expectedSliderPosition);
}

//-----------------------------------------------------------------------------
void ctkDoubleRangeSliderValueProxyTester::testSetPositionCommonData()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("sliderPosition");
  QTest::addColumn<double>("expectedValue");
  QTest::addColumn<double>("expectedSliderPosition");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1. << 42.19 << 0.1 << 0.1 << 0.1;
  QTest::newRow("Offset only: less than min")
    << 1. << 42.19 << -510. << -200. << -200.;
  QTest::newRow("Offset only: more than max")
    << 1. << -42.19 << 65010. << 200. << 200.;


  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5. << 0. << 5. << 5. << 5.;
  QTest::newRow("Coeff only: less than min")
    << 5. << 0. << -1010. << -200. << -200.;
  QTest::newRow("Coeff only: more than max")
    << 5. << 0. << 65010. << 200. << 200.;

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5. << 12. << 42. << 42. << 42.;

  QTest::newRow("Linear: less than min")
    << 5. << 12. << -5010. << -200. << -200.;

  QTest::newRow("Linear: more than max")
    << 5. << 12. << 65010. << 200. << 200.;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkDoubleRangeSliderValueProxyTest)
#include "moc_ctkDoubleRangeSliderValueProxyTest.cpp"
