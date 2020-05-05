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
#include "ctkTest.h"

//-----------------------------------------------------------------------------
class ctkDoubleSliderTester: public QObject
{
  Q_OBJECT
private slots:
  void testUI();

  void testRange();
  void testRange_data();

  void testSingleStep();
  void testSingleStep_data();
};

// ----------------------------------------------------------------------------
void ctkDoubleSliderTester::testUI()
{
  ctkDoubleSlider slider;
  slider.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&slider);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&slider);
#endif
  // qApp->exec();
}

// ----------------------------------------------------------------------------
void ctkDoubleSliderTester::testRange()
{
  ctkDoubleSlider slider;

  QFETCH(double, minimum);
  QFETCH(double, maximum);
  slider.setRange(minimum, maximum);

  QFETCH(double, expectedMinimum);
  QFETCH(double, expectedMaximum);
  QFETCH(double, expectedValue);

  QCOMPARE(slider.minimum(), expectedMinimum);
  QCOMPARE(slider.maximum(), expectedMaximum);
  QCOMPARE(slider.value(), expectedValue);
}

// ----------------------------------------------------------------------------
void ctkDoubleSliderTester::testRange_data()
{
  QTest::addColumn<double>("minimum");
  QTest::addColumn<double>("maximum");
  QTest::addColumn<double>("expectedMinimum");
  QTest::addColumn<double>("expectedMaximum");
  QTest::addColumn<double>("expectedValue");

  QTest::newRow("[20.123,20.1234]") << 20.123 << 20.1234 << 20.123 << 20.1234 << 20.123;
}

// ----------------------------------------------------------------------------
void ctkDoubleSliderTester::testSingleStep()
{
  ctkDoubleSlider slider;
  slider.setValue(50.);

  QFETCH(double, minimum);
  QFETCH(double, maximum);
  slider.setRange(minimum, maximum);

  QFETCH(double, singleStep);
  slider.setSingleStep(singleStep);

  QFETCH(double, expectedValue);
  QCOMPARE(slider.value(), expectedValue);
}

// ----------------------------------------------------------------------------
void ctkDoubleSliderTester::testSingleStep_data()
{
  QTest::addColumn<double>("minimum");
  QTest::addColumn<double>("maximum");
  QTest::addColumn<double>("singleStep");
  QTest::addColumn<double>("expectedValue");

  QTest::newRow("1.") << 0. << 100. << 1. << 50.;
  QTest::newRow("100.") << 0. << 100. << 100. << 50.;
  QTest::newRow("0.1") << 0. << 100. << 0.1 << 50.;
  QTest::newRow("min") << 0. << 100. << std::numeric_limits<double>::min() << 50.;
  QTest::newRow("max") << 0. << 100. << std::numeric_limits<double>::max() << 50.;
  QTest::newRow("-max") << 0. << 100. << -std::numeric_limits<double>::max() << 50.;
  QTest::newRow("-inf") << 0. << 100. << -std::numeric_limits<double>::infinity() << 50.;
  QTest::newRow("inf") << 0. << 100. << std::numeric_limits<double>::infinity() << 50.;
  QTest::newRow("NaN") << 0. << 100. << std::numeric_limits<double>::quiet_NaN() << 50.;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkDoubleSliderTest)
#include "moc_ctkDoubleSliderTest.cpp"
