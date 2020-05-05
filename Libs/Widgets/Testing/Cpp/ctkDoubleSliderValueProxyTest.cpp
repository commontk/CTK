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
#include "ctkDoubleSlider.h"
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

} // end namespace

//-----------------------------------------------------------------------------
class ctkDoubleSliderValueProxyTester: public QObject
{
  Q_OBJECT
private slots:

  void testSetValueProxy();

  void testSetValue();
  void testSetValue_data();

  void testSetSliderPosition();
  void testSetSliderPosition_data();

};

//-----------------------------------------------------------------------------
void ctkDoubleSliderValueProxyTester::testSetValueProxy()
{
  ctkDoubleSlider slider;
  slider.setRange(-200., 200.);
  slider.setValue(-32.6);

  ctkLinearValueProxy proxy;
  proxy.setCoefficient(-1.);
  proxy.setOffset(20.);

  QSignalSpy valueSpy(&slider, SIGNAL(valueChanged(double)));
  QSignalSpy rangeSpy(&slider, SIGNAL(rangeChanged(double,double)));
  slider.setValueProxy(&proxy);

  QCOMPARE(valueSpy.count(), 0);
  QCOMPARE(rangeSpy.count(), 0);
}

//-----------------------------------------------------------------------------
void ctkDoubleSliderValueProxyTester::testSetValue()
{
  ctkDoubleSlider slider;
  slider.setRange(-200., 200.);
  slider.setSingleStep(0.01);
  slider.setValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  slider.setValueProxy(&proxy);

  // Spy
  QSignalSpy valueSpy(&slider, SIGNAL(valueChanged(double)));

  // Test
  QFETCH(double, value);
  slider.setValue(value);

  QFETCH(double, expectedValue);
  getSpyReport(valueSpy, expectedValue);
  ctkTest::COMPARE(slider.value(), expectedValue);
}

//-----------------------------------------------------------------------------
void ctkDoubleSliderValueProxyTester::testSetValue_data()
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


  // coeff // offset // value // expectedValue
  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5. << 0. << 0.1 << 0.1;

  QTest::newRow("Coeff only: less than min")
    << 5. << 0. << -510. << -200.;
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0. << -230. << -200.;
  QTest::newRow("Coeff only: less than min with coeff")
    << 5. << 0. << -190. << -190.;

  QTest::newRow("Coeff only: more than max with coeff")
    << 5. << 0. << 160. << 160.;
  QTest::newRow("Coeff only: more than max")
    << 5. << 0. << 65010. << 200.;
  QTest::newRow("Offset only: less than max but ok with coeff")
    << 0.5 << 0. << 229.2 << 200.;


  // coeff // offset // value // expectedValue
  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5. << 0. << 0.1 << 0.1;

  QTest::newRow("Linear: less than min")
    << 5.0 << 12.0 << -510. << -200.;
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12.0 << -230. << -200.;
  QTest::newRow("Linear: less than min with function")
    << 5.0 << 12.0 << -61.5 << -61.5;

  QTest::newRow("Linear: more than max with function")
    << 5.0 << 12.0 << 160. << 160.;
  QTest::newRow("Linear: more than max")
    << 5.0 << 12.0 << 65010. << 200.;
  QTest::newRow("Offset only: less than max but ok with function")
    << 0.5 << 12.0 << 229.2 << 200.;
}

//-----------------------------------------------------------------------------
void ctkDoubleSliderValueProxyTester::testSetSliderPosition()
{
  // Setup
  ctkDoubleSlider slider;
  slider.setRange(-200., 200.);
  slider.setSingleStep(0.01);
  slider.setValue(-32.6);

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  slider.setValueProxy(&proxy);

  // Spy
  QSignalSpy valueSpy(&slider, SIGNAL(valueChanged(double)));

  // Test
  QFETCH(double, sliderPosition);
  slider.setSliderPosition(sliderPosition);

  QFETCH(double, expectedSliderPosition);
  QFETCH(double, expectedValue);
  getSpyReport(valueSpy, expectedValue);
  ctkTest::COMPARE(slider.value(), expectedValue);
  ctkTest::COMPARE(slider.sliderPosition(), expectedSliderPosition);
}

//-----------------------------------------------------------------------------
void ctkDoubleSliderValueProxyTester::testSetSliderPosition_data()
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
CTK_TEST_MAIN(ctkDoubleSliderValueProxyTest)
#include "moc_ctkDoubleSliderValueProxyTest.cpp"
