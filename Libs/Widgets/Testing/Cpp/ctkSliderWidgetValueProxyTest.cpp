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
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QString>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTimer>

// CTK includes
#include "ctkDoubleSlider.h"
#include "ctkDoubleSpinBox.h"
#include "ctkLinearValueProxy.h"
#include "ctkSliderWidget.h"
#include "ctkTest.h"
#include "ctkValueProxy.h"

namespace
{
//-----------------------------------------------------------------------------
void getSpyReport(QSignalSpy& spy, double expectedValue)
{
  QCOMPARE(spy.count(), 1);

  QList<QVariant> arguments = spy.takeFirst(); // take the first signal
  QCOMPARE(arguments.at(0).toDouble(), expectedValue);
}
} // end namespace

// ----------------------------------------------------------------------------
class ctkSliderWidgetValueProxyTester: public QObject
{
  Q_OBJECT

private slots:
  void testSetValue();
  void testSetValue_data();

  void testSetCoefficient();
  void testSetCoefficient_data();

  void testDecimalsOption();
  void testDecimalsOption_data();
};

//-----------------------------------------------------------------------------
void ctkSliderWidgetValueProxyTester::testSetValue()
{
  // Setup
  ctkSliderWidget slider;
  slider.setMinimum(-200);
  slider.setMaximum(200);
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
  QCOMPARE(slider.value(), expectedValue);
}

//-----------------------------------------------------------------------------
void ctkSliderWidgetValueProxyTester::testSetValue_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("expectedValue");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1.0 << 42.19 << 0.1 << 0.1;
  QTest::newRow("Offset only: keep precision") << 1.0 << 42.19176 << 0.1 << 0.1;

  QTest::newRow("Offset only: less than min")
    << 1.0 << 42.19 << -510.0 << -200.;
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1.0 << 42.19 << -230.0 << -200.;
  QTest::newRow("Offset only: less than min with offset")
    << 1.0 << -42.19 << -190.0 << -190.0;

  QTest::newRow("Offset only: more than max with offset")
    << 1.0 << 42.19 << 160.0 << 160.;
  QTest::newRow("Offset only: more than max")
    << 1.0 << -42.19 << 65010.0 << 200.;
  QTest::newRow("Offset only: less than max but ok with offset")
    << 1.0 << -42.19 << 229.1 << 200.;

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
    << 5. << 0. << 65010.0 << 200.;
  QTest::newRow("Offset only: less than max but ok with coeff")
    << 0.5 << 0. << 229.2 << 200.;

  // coeff // offset // value // expectedValue
  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5. << 0. << 0.1 << 0.1;

  QTest::newRow("Linear: less than min")
    << 5. << 12. << -510.0 << -200.;
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12. << -230. << -200.;
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
void ctkSliderWidgetValueProxyTester::testSetCoefficient()
{
  ctkSliderWidget sliderWidget;
  sliderWidget.setRange(-10000., 10000.);
  sliderWidget.setValue(10.);

  ctkLinearValueProxy proxy;
  proxy.setCoefficient(10.);
  sliderWidget.setValueProxy(&proxy);

  QCOMPARE(sliderWidget.value(), 10.);
  QCOMPARE(sliderWidget.spinBox()->displayedValue(), 100.);

  QFETCH(double, newCoefficient);
  proxy.setCoefficient(newCoefficient);

  QFETCH(double, expectedDisplayedValue);
  QCOMPARE(sliderWidget.value(), 10.);
  QCOMPARE(sliderWidget.spinBox()->displayedValue(),
                   expectedDisplayedValue);
}

//-----------------------------------------------------------------------------
void ctkSliderWidgetValueProxyTester::testSetCoefficient_data()
{
  QTest::addColumn<double>("newCoefficient");
  QTest::addColumn<double>("expectedDisplayedValue");

  QTest::newRow("100") << 100.0 << 1000.;
  QTest::newRow("10") << 10.0 << 100.;
  QTest::newRow("1") << 1.0 << 10.;
  QTest::newRow("0.10") << 0.1 << 1.;
  QTest::newRow("-10") << -10.0 << -100.;
}


//-----------------------------------------------------------------------------
void ctkSliderWidgetValueProxyTester::testDecimalsOption()
{
  ctkSliderWidget sliderWidget;
  sliderWidget.setRange(-10000., 10000.);
  sliderWidget.setDecimals(3);
  sliderWidget.setValue(6.924);

  QFETCH(int, decimalsOption);
  sliderWidget.spinBox()->setDecimalsOption(
    static_cast<ctkDoubleSpinBox::DecimalsOptions>(decimalsOption));

  ctkLinearValueProxy proxy;
  sliderWidget.setValueProxy(&proxy);

  QFETCH(double, coefficient);
  proxy.setCoefficient(coefficient);

  // test if it does not trigger asserts
  sliderWidget.setRange(-10000., 10000.);
  if (decimalsOption & ctkDoubleSpinBox::DecimalsByValue)
    {
    QCOMPARE(sliderWidget.spinBox()->value(), 6.924);
    }

  proxy.setCoefficient(1.);
}

//-----------------------------------------------------------------------------
void ctkSliderWidgetValueProxyTester::testDecimalsOption_data()
{
  QTest::addColumn<int>("decimalsOption");
  QTest::addColumn<double>("coefficient");

  for (double coef = 1.; coef < 10000000.; coef *= 10.)
    {
    QTest::newRow("coef by value") << static_cast<int>(ctkDoubleSpinBox::DecimalsByValue) << coef;
    QTest::newRow("coef by value") << static_cast<int>(ctkDoubleSpinBox::FixedDecimals) << coef;
    QTest::newRow("1./coef by value") << static_cast<int>(ctkDoubleSpinBox::DecimalsByValue) << 1. / coef;
    QTest::newRow("1./coef") << static_cast<int>(ctkDoubleSpinBox::FixedDecimals) << 1./ coef;
    }
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkSliderWidgetValueProxyTest)
#include "moc_ctkSliderWidgetValueProxyTest.cpp"


