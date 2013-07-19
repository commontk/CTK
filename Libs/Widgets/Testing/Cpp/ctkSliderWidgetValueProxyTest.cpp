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
  ctkTest::COMPARE(arguments.at(0).toDouble(), expectedValue);
}
} // end namespace

// ----------------------------------------------------------------------------
class ctkSliderWidgetValueProxyTester: public QObject
{
  Q_OBJECT
private slots:

  void testSetValue();
  void testSetValue_data();
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
  ctkTest::COMPARE(slider.value(), expectedValue);
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
  QTest::newRow("Offset only") << 1.0 << 42.19176 << 0.1 << 0.1;

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

  // coeff // offset // value // expectedValue
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

  // coeff // offset // value // expectedValue
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

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkSliderWidgetValueProxyTest)
#include "moc_ctkSliderWidgetValueProxyTest.cpp"


