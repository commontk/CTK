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

// CTK includes
#include "ctkRangeWidget.h"
#include "ctkTest.h"

// STD includes
#include <limits>

// ----------------------------------------------------------------------------
class ctkRangeWidgetTester: public QObject
{
  Q_OBJECT
private slots:
  void testUI();

  void testSetMinimumValue();
  void testSetMinimumValue_data();

  void testSetMaximumValue();
  void testSetMaximumValue_data();

  void testSetValues();
  void testSetValues_data();

  void testSetRange();
  void testSetRange_data();

  void testSetRangeSetValues();
  void testSetRangeSetValues_data();

};

// ----------------------------------------------------------------------------
void ctkRangeWidgetTester::testUI()
{
  ctkRangeWidget rangeWidget;
  rangeWidget.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&rangeWidget);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&rangeWidget);
#endif
  //qApp->exec();
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetMinimumValue()
{
  ctkRangeWidget rangeWidget;
  rangeWidget.setValues(25., 75.);

  QSignalSpy lowerValueChangedSpy(&rangeWidget,
                                  SIGNAL(minimumValueChanged(double)));
  QSignalSpy valuesChangedSpy(&rangeWidget,
                              SIGNAL(valuesChanged(double,double)));
  QFETCH(double, lowerValue);
  rangeWidget.setMinimumValue(lowerValue);

  QFETCH(double, expectedLowerValue);
  QCOMPARE(rangeWidget.minimumValue(), expectedLowerValue);

  const bool lowerValueChanged = expectedLowerValue != 25.;
  QCOMPARE(lowerValueChangedSpy.count(), lowerValueChanged ? 1 : 0);
  QCOMPARE(valuesChangedSpy.count(), lowerValueChanged ? 1 : 0);
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetMinimumValue_data()
{
  QTest::addColumn<double>("lowerValue");
  QTest::addColumn<double>("expectedLowerValue");

  QTest::newRow("1. -> 1.]") << 1. << 1.;
  QTest::newRow("100. -> 75.]") << 100. << 75.;
  QTest::newRow("-1. -> 0.]") << -1. << 0.;

  QTest::newRow("min -> min") << std::numeric_limits<double>::min()
                              << std::numeric_limits<double>::min();
  QTest::newRow("max -> 75.") << std::numeric_limits<double>::max() << 75.;
  QTest::newRow("-inf -> 0.") << -std::numeric_limits<double>::infinity() << 0.;
  QTest::newRow("inf -> 75.") << std::numeric_limits<double>::infinity() << 75.;
  QTest::newRow("NaN -> 75.") << std::numeric_limits<double>::quiet_NaN() << 0.;
}


//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetMaximumValue()
{
  ctkRangeWidget rangeWidget;
  rangeWidget.setValues(25., 75.);

  QSignalSpy upperValueChangedSpy(&rangeWidget,
                                  SIGNAL(maximumValueChanged(double)));
  QSignalSpy valuesChangedSpy(&rangeWidget,
                              SIGNAL(valuesChanged(double,double)));

  QFETCH(double, upperValue);
  rangeWidget.setMaximumValue(upperValue);

  QFETCH(double, expectedUpperValue);
  QCOMPARE(rangeWidget.maximumValue(), expectedUpperValue);

  const bool upperValueChanged = expectedUpperValue != 75.;
  QCOMPARE(upperValueChangedSpy.count(), upperValueChanged ? 1 : 0);
  QCOMPARE(valuesChangedSpy.count(), upperValueChanged ? 1 : 0);
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetMaximumValue_data()
{
  QTest::addColumn<double>("upperValue");
  QTest::addColumn<double>("expectedUpperValue");

  QTest::newRow("99. -> 99.]") << 99. << 99.;
  QTest::newRow("0. -> 25.]") << 0. << 25.;
  QTest::newRow("100. -> 99.]") << 100. << 99.;

  QTest::newRow("min -> 25.") << std::numeric_limits<double>::min() << 25.;
  QTest::newRow("max -> 99.") << std::numeric_limits<double>::max() << 99.;
  QTest::newRow("-inf -> 25.") << -std::numeric_limits<double>::infinity() << 25.;
  QTest::newRow("inf -> 99.") << std::numeric_limits<double>::infinity() << 99.;
  QTest::newRow("NaN -> 99.") << std::numeric_limits<double>::quiet_NaN() << 25.;
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetValues()
{
  ctkRangeWidget rangeWidget;
  rangeWidget.setValues(25., 75.);

  QSignalSpy lowerValueChangedSpy(&rangeWidget,
                                  SIGNAL(minimumValueChanged(double)));
  QSignalSpy upperValueChangedSpy(&rangeWidget,
                                  SIGNAL(maximumValueChanged(double)));
  QSignalSpy valuesChangedSpy(&rangeWidget,
                              SIGNAL(valuesChanged(double,double)));

  QFETCH(double, lowerValue);
  QFETCH(double, upperValue);
  rangeWidget.setValues(lowerValue, upperValue);

  QFETCH(double, expectedLowerValue);
  QFETCH(double, expectedUpperValue);
  QCOMPARE(rangeWidget.minimumValue(), expectedLowerValue);
  QCOMPARE(rangeWidget.maximumValue(), expectedUpperValue);

  const bool lowerValueChanged = expectedLowerValue != 25.;
  const bool upperValueChanged = expectedUpperValue != 75.;
  QCOMPARE(lowerValueChangedSpy.count(), lowerValueChanged ? 1 : 0);
  QCOMPARE(upperValueChangedSpy.count(), upperValueChanged ? 1 : 0);
  // \todo fix the valuesChanged signal count.
  //QCOMPARE(valuesChangedSpy.count(),
  //         (lowerValueChanged || upperValueChanged) ? 1 : 0);
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetValues_data()
{
  QTest::addColumn<double>("lowerValue");
  QTest::addColumn<double>("upperValue");
  QTest::addColumn<double>("expectedLowerValue");
  QTest::addColumn<double>("expectedUpperValue");

  QTest::newRow("[1.,10.] -> [1., 10.]") << 1. << 10. << 1. << 10.;
  QTest::newRow("[98.,99.] -> [98., 99.]") << 98. << 99. << 98. << 99.;
  QTest::newRow("[1.,1.] -> [1., 1.]") << 1. << 1. << 1. << 1.;
  QTest::newRow("[10.,1.] -> [1., 10.]") << 10. << 1. << 1. << 10.;
  QTest::newRow("[-1.,100.] -> [0., 99.]") << -1. << 100. << 0. << 99.;

  QTest::newRow("[min,max] -> [min, 99.]")
    << std::numeric_limits<double>::min()
    << std::numeric_limits<double>::max()
    << std::numeric_limits<double>::min() << 99.;
  QTest::newRow("[-inf,inf] -> [0., 99.]")
    << -std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity() << 0. << 99.;
  QTest::newRow("[NaN,NaN] -> [99., 99.]")
    << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN() << 0. << 0.;
}


//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetRange()
{
  ctkRangeWidget rangeWidget;
  rangeWidget.setValues(25., 75.);

  QSignalSpy lowerValueChangedSpy(&rangeWidget,
                                  SIGNAL(minimumValueChanged(double)));
  QSignalSpy upperValueChangedSpy(&rangeWidget,
                                  SIGNAL(maximumValueChanged(double)));
  QSignalSpy rangeChangedSpy(&rangeWidget,
                              SIGNAL(rangeChanged(double,double)));

  QFETCH(double, minimum);
  QFETCH(double, maximum);
  rangeWidget.setRange(minimum, maximum);

  QFETCH(double, expectedMinimum);
  QFETCH(double, expectedMaximum);
  ctkTest::COMPARE(rangeWidget.minimum(), expectedMinimum);
  ctkTest::COMPARE(rangeWidget.maximum(), expectedMaximum);

  const bool minimumChanged = expectedMinimum != 0.;
  const bool maximumChanged = expectedMaximum != 99.;
  QCOMPARE(rangeChangedSpy.count(),(minimumChanged || maximumChanged) ? 1 : 0);

  QFETCH(double, expectedLowerValue);
  QFETCH(double, expectedUpperValue);
  ctkTest::COMPARE(rangeWidget.minimumValue(), expectedLowerValue);
  ctkTest::COMPARE(rangeWidget.maximumValue(), expectedUpperValue);

  const bool lowerValueChanged = expectedLowerValue != 25.;
  const bool upperValueChanged = expectedUpperValue != 75.;
  QCOMPARE(lowerValueChangedSpy.count(), lowerValueChanged ? 1 : 0);
  QCOMPARE(upperValueChangedSpy.count(), upperValueChanged ? 1 : 0);
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetRange_data()
{
  QTest::addColumn<double>("minimum");
  QTest::addColumn<double>("maximum");
  QTest::addColumn<double>("expectedMinimum");
  QTest::addColumn<double>("expectedMaximum");
  QTest::addColumn<double>("expectedLowerValue");
  QTest::addColumn<double>("expectedUpperValue");

  QTest::newRow("[1.,98.]") << 1. << 98. << 1. << 98. << 25. << 75.;
  QTest::newRow("[10.0123,99.99]") << 10.0123 << 99.99 << 10.0123 << 99.99 << 25. << 75.;
  QTest::newRow("[0.,2050.9876.]") << 0. << 2050.9876 << 0. << 2050.9876 << 25. << 75.;
  QTest::newRow("[-1.,101.]") << -1. << 101. << -1. << 101. << 25. << 75.;
  QTest::newRow("[1.,50.]") << 1. << 50. << 1. << 50. << 25. << 50.;
  QTest::newRow("[1., 50.5678]") << 1. << 50.5678 << 1. << 50.5678 << 25. << 50.5678 ;
  QTest::newRow("[50.,99.]") << 50. << 99. << 50. << 99. << 50. << 75. ;
  QTest::newRow("[50.5678,99.]") << 50.5678 << 99. << 50.5678 << 99. << 50.5678 << 75. ;
  QTest::newRow("[1.,10.]") << 1. << 10. << 1. << 10. << 10. << 10.;
  QTest::newRow("[90.,99.]") << 90. << 99. << 90. << 99. << 90. << 90.;
  QTest::newRow("[min,max]")
    << std::numeric_limits<double>::min()
    << std::numeric_limits<double>::max()
    << std::numeric_limits<double>::min()
    << std::numeric_limits<double>::max()
    << 25. << 75.;
  QTest::newRow("[-inf,inf]")
    << -std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity()
    << -std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity()
    << 25. << 75.;
  QTest::newRow("[NaN,NaN]")
    << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN()
    << std::numeric_limits<double>::quiet_NaN();
}

//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetRangeSetValues()
{
  ctkRangeWidget rangeWidget;
  QFETCH(double, minimum);
  QFETCH(double, maximum);
  rangeWidget.setRange(minimum, maximum);

  rangeWidget.setDecimals(0);
  rangeWidget.setSingleStep(std::numeric_limits<double>::infinity());
  QFETCH(double, lowerValue);
  QFETCH(double, upperValue);
  rangeWidget.setValues(lowerValue, upperValue);
}


//-----------------------------------------------------------------------------
void ctkRangeWidgetTester::testSetRangeSetValues_data()
{
  QTest::addColumn<double>("minimum");
  QTest::addColumn<double>("maximum");
  QTest::addColumn<double>("lowerValue");
  QTest::addColumn<double>("upperValue");

  QTest::newRow("[-inf,inf,-inf,inf]")
    << -std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity()
    << -std::numeric_limits<double>::infinity()
    << std::numeric_limits<double>::infinity();
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkRangeWidgetTest)
#include "moc_ctkRangeWidgetTest.cpp"
