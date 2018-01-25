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
#if (QT_VERSION < 0x50000)
#include <QCleanlooksStyle>
#endif
#include <QString>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTimer>

// CTK includes
#include "ctkRangeSlider.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkRangeSliderTester: public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();
  void testGUIEvents();
  void testTooltips();

  void testSimpleMouseEvents();

  void testHandleMouseEvents();
  void testHandleMouseEvents_data();

  void testGrooveMouseEvents();
  void testGrooveMouseEvents_data();
};

// ----------------------------------------------------------------------------
void ctkRangeSliderTester::initTestCase()
{
  // Mouse position on handles does not with with gtk style.
#if (QT_VERSION < 0x50000)
  QApplication::setStyle(new QCleanlooksStyle());
#endif
}

// ----------------------------------------------------------------------------
void ctkRangeSliderTester::testGUIEvents()
{
  ctkRangeSlider rangeSlider;
  rangeSlider.show();
  qApp->processEvents();
  rangeSlider.resize(100, 100);
  qApp->processEvents();
  rangeSlider.resize(1, 100);
  qApp->processEvents();
  rangeSlider.resize(100, 1);
  qApp->processEvents();
  rangeSlider.hide();
  qApp->processEvents();
  rangeSlider.show();
}

// ----------------------------------------------------------------------------
void ctkRangeSliderTester::testTooltips()
{
  ctkRangeSlider rangeSlider;

  QVERIFY(rangeSlider.handleToolTip().isEmpty()); // default value

  rangeSlider.setHandleToolTip("custom tooltip");
  QCOMPARE(rangeSlider.handleToolTip(), QString("custom tooltip"));
}

// ----------------------------------------------------------------------------
void ctkRangeSliderTester::testSimpleMouseEvents()
{
  ctkRangeSlider rangeSlider;

  QPoint center = rangeSlider.rect().center();
  QTest::mouseMove(&rangeSlider, center);

  QCOMPARE(rangeSlider.isSliderDown(), false);
  QTest::mousePress(&rangeSlider, Qt::LeftButton, 0, center);
  QCOMPARE(rangeSlider.isSliderDown(), true);
  QTest::mouseRelease(&rangeSlider, Qt::LeftButton, 0, center);
  QCOMPARE(rangeSlider.isSliderDown(), false);
}

// ----------------------------------------------------------------------------
void ctkRangeSliderTester::testGrooveMouseEvents()
{
  ctkRangeSlider rangeSlider(Qt::Horizontal);
  rangeSlider.setMaximum(100);
  rangeSlider.setValues(10, 90);

  QStyleOptionSlider option;
  // Resize to enforce a 1 to 1 mapping between pixel position and slider value.
  // Size of the handle is added because of the following line in ctkRangeSlider:
  //  sliderMax = gr.right() - sliderLength + 1;
  QRect sliderHandleSize = rangeSlider.style()->subControlRect(
    QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, &rangeSlider );
  rangeSlider.resize(100 + sliderHandleSize.width(), 20);

  QFETCH(int, moveInPx);
  QFETCH(int, expectedMinValue);
  QFETCH(int, expectedMaxValue);

  QPoint currentCursorPos = rangeSlider.rect().center();
  QTest::mouseMove(&rangeSlider, currentCursorPos);
  QTest::mousePress(&rangeSlider, Qt::LeftButton, 0, currentCursorPos);
  currentCursorPos += QPoint(moveInPx, 0);
  ctkTest::mouseMove(&rangeSlider, Qt::LeftButton, 0, currentCursorPos);
  QTest::mouseRelease(&rangeSlider, Qt::LeftButton, 0, currentCursorPos);

  QCOMPARE(rangeSlider.minimumValue(), expectedMinValue);
  QCOMPARE(rangeSlider.maximumValue(), expectedMaxValue);
}

// ----------------------------------------------------------------------------
void ctkRangeSliderTester::testGrooveMouseEvents_data()
{
  QTest::addColumn<int>("moveInPx");
  QTest::addColumn<int>("expectedMinValue");
  QTest::addColumn<int>("expectedMaxValue");

  QTest::newRow("MinMax +5") << 5 << 15 << 95;
  QTest::newRow("MinMax -5") << -5 << 5 << 85;
  QTest::newRow("MinMax +10") << 10 << 20 << 100;
  QTest::newRow("MinMax -10") << -10 << 0 << 80;
  QTest::newRow("MinMax +20 -> bound to max") << 20 << 30 << 100;
  QTest::newRow("MinMax -20 -> bound to min") << -20 << 0 << 70;
}

// ----------------------------------------------------------------------------
void ctkRangeSliderTester::testHandleMouseEvents()
{
  ctkRangeSlider rangeSlider(Qt::Horizontal);
  rangeSlider.setMaximum(100);
  rangeSlider.setValues(10, 80);

  QStyleOptionSlider option;
  // Resize to enforce a 1 to 1 mapping between pixel position and slider value.
  // Size of the handle is added because of the following line in ctkRangeSlider:
  //  sliderMax = gr.right() - sliderLength + 1;
  QRect sliderHandleSize = rangeSlider.style()->subControlRect(
    QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, &rangeSlider );
  rangeSlider.resize(100 + sliderHandleSize.width(), 20);

  rangeSlider.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&rangeSlider);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&rangeSlider);
#endif

  QFETCH(bool, minHandle);
  QFETCH(bool, symmetricMoves);
  QFETCH(int, moveInPx);
  QFETCH(int, expectedMinValue);
  QFETCH(int, expectedMaxValue);

  QPoint currentCursorPos = rangeSlider.rect().center();
  if (minHandle)
    {
    currentCursorPos = QPoint(10, 10);
    }
  else
    {
    currentCursorPos = QPoint(80, 10);
    }
  rangeSlider.setSymmetricMoves(symmetricMoves);

  QTest::mouseMove(&rangeSlider, currentCursorPos);
  QTest::mousePress(&rangeSlider, Qt::LeftButton, 0, currentCursorPos);
  const bool isHandleDown = minHandle ? rangeSlider.isMinimumSliderDown() :
    rangeSlider.isMaximumSliderDown();
  QVERIFY(isHandleDown);
  currentCursorPos += QPoint(moveInPx, 0);
  ctkTest::mouseMove(&rangeSlider, Qt::LeftButton, 0, currentCursorPos);
  QTest::mouseRelease(&rangeSlider, Qt::LeftButton, 0, currentCursorPos);

  QCOMPARE(rangeSlider.minimumValue(), expectedMinValue);
  QCOMPARE(rangeSlider.maximumValue(), expectedMaxValue);
}

// ----------------------------------------------------------------------------
void ctkRangeSliderTester::testHandleMouseEvents_data()
{
  QTest::addColumn<bool>("minHandle");
  QTest::addColumn<bool>("symmetricMoves");
  QTest::addColumn<int>("moveInPx");
  QTest::addColumn<int>("expectedMinValue");
  QTest::addColumn<int>("expectedMaxValue");

  QTest::newRow("Min +5") << true << false << 5 << 15 << 80;
  QTest::newRow("Min -5") << true << false << -5 << 5 << 80;
  QTest::newRow("Min +10") << true << false << 10 << 20 << 80;
  QTest::newRow("Min -10") << true << false << -10 << 0 << 80;
  QTest::newRow("Min +20") << true << false << 20 << 30 << 80;
  QTest::newRow("Min -20 -> bound to min") << true << false << -20 << 0 << 80;

  QTest::newRow("Max +5") << false << false << 5 << 10 << 85;
  QTest::newRow("Max -5") << false << false << -5 << 10 << 75;
  QTest::newRow("Max +10") << false << false << 10 << 10 << 90;
  QTest::newRow("Max -10") << false << false << -10 << 10 << 70;
  QTest::newRow("Max +20 -> bound to max") << false << false << 20 << 10 << 100;
  QTest::newRow("Max -20") << false << false << -20 << 10 << 60;

  QTest::newRow("Sym Min +5") << true << true << 5 << 15 << 75;
  QTest::newRow("Sym Min -5") << true << true << -5 << 5 << 85;
  QTest::newRow("Sym Min +10") << true << true << 10 << 20 << 70;
  QTest::newRow("Sym Min -10") << true << true << -10 << 0 << 90;
  QTest::newRow("Sym Min +20") << true << true << 20 << 30 << 60;
  QTest::newRow("Sym Min -20 -> bound to min") << true << true << -20 << 0 << 90;
  QTest::newRow("Sym Max +20") << false << true << 20 << 0 << 100;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkRangeSliderTest)
#include "moc_ctkRangeSliderTest.cpp"
