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
#include <QSignalSpy>
#include <QString>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTimer>
#include <QVector3D>
#include <QVector4D>

// CTK includes
#include "ctkCoordinatesWidget.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkCoordinatesWidgetTester: public QObject
{
  Q_OBJECT
public:
  explicit ctkCoordinatesWidgetTester(QObject* parent = nullptr) : QObject(parent) {}
private slots:
  void testDefaults();

  void testNormalized();
  void testNormalized_data();

  void testDecimalsByValue();
  void testDecimalsByValue_data();

  void testDecimalsByKey();
  void testDecimalsByKey_data();

  void testDecimalsByShortcuts();
  void testDecimalsByShortcuts_data();

  void testCoordinatesChangedSignal();
  void testCoordinatesChangedSignal_data();

  void testCoordinatesChangedNotFiredIfUnchanged();

private:
  void testDecimals(ctkCoordinatesWidget* coordinatesWidget, int decimals);
  void testDecimals(ctkCoordinatesWidget* coordinatesWidget, QString displayedValues);
};

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testDefaults()
{
  ctkCoordinatesWidget coordinatesWidget;
  QCOMPARE(coordinatesWidget.dimension(), 3);
  QCOMPARE(coordinatesWidget.decimals(), 3);
  QCOMPARE(coordinatesWidget.singleStep(), 1.);
  QCOMPARE(coordinatesWidget.minimum(), -std::numeric_limits<double>::max());
  QCOMPARE(coordinatesWidget.maximum(), std::numeric_limits<double>::max());
  QCOMPARE(coordinatesWidget.isNormalized(), false);
  QCOMPARE(coordinatesWidget.coordinatesAsString(), QString("0,0,0"));
  QCOMPARE(QVector3D(coordinatesWidget.coordinates()[0],
                      coordinatesWidget.coordinates()[1],
                      coordinatesWidget.coordinates()[2]),
            QVector3D(0., 0., 0.));
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testNormalized()
{
  ctkCoordinatesWidget coordinatesWidget;
  QFETCH(QVector3D, coordinates);
  coordinatesWidget.setCoordinates(coordinates.x(), coordinates.y(), coordinates.z());

  coordinatesWidget.setNormalized(true);
  QVector3D after(coordinatesWidget.coordinates()[0],
                  coordinatesWidget.coordinates()[1],
                  coordinatesWidget.coordinates()[2]);
  QVector3D normalized = coordinates.normalized();
  // CoodinatesWidget has only 3 significant decimals (so just test the 2 first
  // decimals because the last one might off of 1).
  QCOMPARE(static_cast<int>(normalized.x() * 100), static_cast<int>(after.x() * 100));
  QCOMPARE(static_cast<int>(normalized.y() * 100), static_cast<int>(after.y() * 100));
  QCOMPARE(static_cast<int>(normalized.z() * 100), static_cast<int>(after.z() * 100));
  QCOMPARE(coordinatesWidget.minimum(), -1.);
  QCOMPARE(coordinatesWidget.maximum(), 1.);
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testNormalized_data()
{
  QTest::addColumn<QVector3D>("coordinates");

  QTest::newRow("0,0,0") << QVector3D();
  QTest::newRow("1,0,0") << QVector3D(1., 0., 0.);
  QTest::newRow("0,1,0") << QVector3D(0., 1., 0.);
  QTest::newRow("0,0,-1") << QVector3D(0., 0., -1.);
  QTest::newRow("2,0,0") << QVector3D(2., 0., 0.);
  QTest::newRow("2,2,0") << QVector3D(2., 2., 0.);
  QTest::newRow("0,-2,2") << QVector3D(0., -2., 2.);
  QTest::newRow("1,2,3") << QVector3D(1., 2., 3.);
  QTest::newRow("-1,-2,-3") << QVector3D(-1., -2., -3.);
}
// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester
::testDecimals(ctkCoordinatesWidget* coordinatesWidget, int decimals)
{
  QList<QDoubleSpinBox*> spinBoxes =
    coordinatesWidget->findChildren<QDoubleSpinBox*>();
  foreach(QDoubleSpinBox* spinBox, spinBoxes)
  {
    QCOMPARE(spinBox->decimals(), decimals);
  }
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester
::testDecimals(ctkCoordinatesWidget* coordinatesWidget,
               QString expectedDisplayedText)
{
  QStringList displayedText;
  QList<QDoubleSpinBox*> spinBoxes =
    coordinatesWidget->findChildren<QDoubleSpinBox*>();
  foreach(QDoubleSpinBox* spinBox, spinBoxes)
  {
    displayedText << spinBox->text();
  }
  QCOMPARE(displayedText.join(", "), expectedDisplayedText);
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testDecimalsByValue()
{
  ctkCoordinatesWidget coordinatesWidget;
  coordinatesWidget.setDecimalsOption(
    ctkDoubleSpinBox::DecimalsByValue);
  coordinatesWidget.setCoordinates(0.,0.,0.);

  QFETCH(QVector3D, coordinates);
  coordinatesWidget.setCoordinates(coordinates.x(), coordinates.y(), coordinates.z());

  QFETCH(int, expectedDecimals);
  testDecimals(&coordinatesWidget, expectedDecimals);
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testDecimalsByValue_data()
{
  QTest::addColumn<QVector3D>("coordinates");
  QTest::addColumn<int>("expectedDecimals");

  QTest::newRow("1, 1, 1.3") << QVector3D(1., 1., 1.3) << 1;
  QTest::newRow("16 digits") << QVector3D(-86.1234567891234567,
                                          133.98765432198765432,
                                          116.01234567891011121) << 3;
  QTest::newRow("same value more digits") << QVector3D(0.001, 0.002, 0.0004) << 4;
}


// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testDecimalsByKey()
{
  ctkCoordinatesWidget coordinatesWidget;
  coordinatesWidget.setDecimalsOption(
    ctkDoubleSpinBox::DecimalsByValue
    | ctkDoubleSpinBox::DecimalsByKey
    | ctkDoubleSpinBox::DecimalsByShortcuts);
  coordinatesWidget.setDecimals(3);
  coordinatesWidget.setCoordinates(1.,1.,1.3);

  QFETCH(QVector3D, coordinates);
  coordinatesWidget.setCoordinates(coordinates.x(), coordinates.y(), coordinates.z());

  QFETCH(int, expectedDecimals);
  testDecimals(&coordinatesWidget, expectedDecimals);

  // Simulate decimals by key
  QFETCH(int, decimalsOffset);
  QList<QDoubleSpinBox*> spinBoxes =
    coordinatesWidget.findChildren<QDoubleSpinBox*>();
  QList<QLineEdit*> lineEdits =
    spinBoxes[0]->findChildren<QLineEdit*>();
  QString text = lineEdits[0]->text();
  switch (decimalsOffset)
  {
    case 1:
      if (!text.contains('.'))
      {
        text += '.';
      }
      text += '7';
      lineEdits[0]->setText(text);
      break;
    case 0:
      text = QString("7") + text;
      lineEdits[0]->setText(text);
      break;
    case -1:
      text.chop(1);
      lineEdits[0]->setText(text);
      break;
    case 100:
      QTest::keyClick(spinBoxes[0], Qt::Key_Plus, Qt::ControlModifier);
      break;
    case -100:
      QTest::keyClick(spinBoxes[0], Qt::Key_Minus, Qt::ControlModifier);
      break;
    default:
      break;
  }

  QFETCH(QVector3D, finalDecimals);
  QCOMPARE(spinBoxes[0]->decimals(), static_cast<int>(finalDecimals.x()));
  QCOMPARE(spinBoxes[1]->decimals(), static_cast<int>(finalDecimals.y()));
  QCOMPARE(spinBoxes[2]->decimals(), static_cast<int>(finalDecimals.z()));
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testDecimalsByKey_data()
{
  QTest::addColumn<QVector3D>("coordinates");
  QTest::addColumn<int>("expectedDecimals");
  QTest::addColumn<int>("decimalsOffset");
  QTest::addColumn<QVector3D>("finalDecimals");

  QTest::newRow("(1, 1, 1) +1")
    << QVector3D(1., 1., 1.) << 0 << 1 << QVector3D(1,1,1);
  QTest::newRow("(1, 1, 1) +0")
    << QVector3D(1., 1., 1.) << 0 << 0 << QVector3D(0,0,0);
  QTest::newRow("(1, 1, 1) -1")
    << QVector3D(1., 1., 1.) << 0 << -1 << QVector3D(0,0,0);
  QTest::newRow("(1, 1, 1) ++")
    << QVector3D(1., 1., 1.) << 0 << 100 << QVector3D(1,1,1);
  QTest::newRow("(1, 1, 1) --")
    << QVector3D(1., 1., 1.) << 0 << -100 << QVector3D(0,0,0);

  QTest::newRow("(1, 1, 1.3) +1")
    << QVector3D(1., 1., 1.3) << 1 << 1 << QVector3D(2,2,2);
  QTest::newRow("(1, 1, 1.3) +0")
    << QVector3D(1., 1., 1.3) << 1 << 0 << QVector3D(1,1,1);
  QTest::newRow("(1, 1, 1.3) -1")
    << QVector3D(1., 1., 1.3) << 1 << -1 << QVector3D(0,1,1);
  QTest::newRow("(1, 1, 1.3) ++")
    << QVector3D(1., 1., 1.3) << 1 << 100 << QVector3D(2,2,2);
  QTest::newRow("(1, 1, 1.3) --")
    << QVector3D(1., 1., 1.3) << 1 << -100 << QVector3D(0,1,1);

  QTest::newRow("(1.3, 1, 1) +1")
    << QVector3D(1.3, 1., 1.) << 1 << 1 << QVector3D(2,2,2);
  QTest::newRow("(1.3, 1, 1) +0")
    << QVector3D(1.3, 1., 1.) << 1 << 0 << QVector3D(1,1,1);
  QTest::newRow("(1.3, 1, 1) -1")
    << QVector3D(1.3, 1., 1.) << 1 << -1 << QVector3D(0,0,0);
  QTest::newRow("(1.3, 1, 1) ++")
    << QVector3D(1.3, 1., 1.) << 1 << 100 << QVector3D(2,2,2);
  QTest::newRow("(1.3, 1, 1) --")
    << QVector3D(1.3, 1., 1.) << 1 << -100 << QVector3D(0,0,0);

  QTest::newRow("(1.3, 1, 1.3) +1")
    << QVector3D(1.3, 1., 1.3) << 1 << 1 << QVector3D(2,2,2);
  QTest::newRow("(1.3, 1, 1.3) +0")
    << QVector3D(1.3, 1., 1.3) << 1 << 0 << QVector3D(1,1,1);
  QTest::newRow("(1.3, 1, 1.3) -1")
    << QVector3D(1.3, 1., 1.3) << 1 << -1 << QVector3D(0,1,1);
  QTest::newRow("(1.3, 1, 1.3) ++")
    << QVector3D(1.3, 1., 1.3) << 1 << 100 << QVector3D(2,2,2);
  QTest::newRow("(1.3, 1, 1.3) --")
    << QVector3D(1.3, 1., 1.3) << 1 << -100 << QVector3D(0,1,1);

  QTest::newRow("(1.*, 1, 1.3) +1")
    << QVector3D(1.12345678910121416, 1., 1.3) << 3 << 1 << QVector3D(4,4,4);
  QTest::newRow("(1.*, 1, 1.3) +0")
    << QVector3D(1.12345678910121416, 1., 1.3) << 3 << 0 << QVector3D(3,3,3);
  QTest::newRow("(1.*, 1, 1.3) -1")
    << QVector3D(1.12345678910121416, 1., 1.3) << 3 << -1 << QVector3D(2,2,2);
  QTest::newRow("(1.*, 1, 1.3) ++")
    << QVector3D(1.12345678910121416, 1., 1.3) << 3 << 100 << QVector3D(4,4,4);
  QTest::newRow("(1.*, 1, 1.3) --")
    << QVector3D(1.12345678910121416, 1., 1.3) << 3 << -100 << QVector3D(2,2,2);

  QTest::newRow("(1, 1.*, 1.3) +1")
    << QVector3D(1., 1.12345678910121416, 1.3) << 3 << 1 << QVector3D(4,4,4);
  QTest::newRow("(1, 1.*, 1.3) +0")
    << QVector3D(1., 1.12345678910121416, 1.3) << 3 << 0 << QVector3D(3,3,3);
  QTest::newRow("(1, 1.*, 1.3) -1")
    << QVector3D(1., 1.12345678910121416, 1.3) << 3 << -1 << QVector3D(2,3,3);
  QTest::newRow("(1, 1.*, 1.3) ++")
    << QVector3D(1., 1.12345678910121416, 1.3) << 3 << 100 << QVector3D(4,4,4);
  QTest::newRow("(1, 1.*, 1.3) --")
    << QVector3D(1., 1.12345678910121416, 1.3) << 3 << -100 << QVector3D(2,3,3);

  QTest::newRow("(1.*, 1.*, 1.3) +1")
    << QVector3D(1.12345678910121416, 1.12345678910121416, 1.3) << 3 << 1 << QVector3D(4,4,4);
  QTest::newRow("(1.*, 1.*, 1.3) +0")
    << QVector3D(1.12345678910121416, 1.12345678910121416, 1.3) << 3 << 0 << QVector3D(3,3,3);
  QTest::newRow("(1.*, 1.*, 1.3) -1")
    << QVector3D(1.12345678910121416, 1.12345678910121416, 1.3) << 3 << -1 << QVector3D(2,3,3);
  QTest::newRow("(1.*, 1.*, 1.3) ++")
    << QVector3D(1.12345678910121416, 1.12345678910121416, 1.3) << 3 << 100 << QVector3D(4,4,4);
  QTest::newRow("(1.*, 1.*, 1.3) --")
    << QVector3D(1.12345678910121416, 1.12345678910121416, 1.3) << 3 << -100 << QVector3D(2,2,2);
}


// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testDecimalsByShortcuts()
{
  ctkCoordinatesWidget coordinatesWidget;
  coordinatesWidget.setDecimalsOption(
    ctkDoubleSpinBox::DecimalsByShortcuts);
  coordinatesWidget.setDecimals(3);
  coordinatesWidget.setCoordinates(1.1234567891011,1.1234567891011,1.1234567891011);

  testDecimals(&coordinatesWidget, 3);
  testDecimals(&coordinatesWidget, "1.123, 1.123, 1.123");

  // Simulate shortcut
  QList<QDoubleSpinBox*> spinBoxes =
    coordinatesWidget.findChildren<QDoubleSpinBox*>();
  QFETCH(int, key);
  QFETCH(int, repeat);
  for (int i = 0; i < repeat; ++i)
  {
    QTest::keyClick(spinBoxes[0], key, Qt::ControlModifier);
  }
  QFETCH(int, expectedDecimals);
  QFETCH(QString, expectedDisplayedText);
  testDecimals(&coordinatesWidget, expectedDecimals);
  testDecimals(&coordinatesWidget, expectedDisplayedText);
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testDecimalsByShortcuts_data()
{
  QTest::addColumn<int>("key");
  QTest::addColumn<int>("repeat");
  QTest::addColumn<int>("expectedDecimals");
  QTest::addColumn<QString>("expectedDisplayedText");

  QTest::newRow("+ -> 1.1235, 1.1235, 1.1235")
    << static_cast<int>(Qt::Key_Plus) << 1 << 4 << "1.1235, 1.1235, 1.1235";
  QTest::newRow("++ -> 1.12346, 1.12346, 1.12346")
    << static_cast<int>(Qt::Key_Plus) << 2 << 5 << "1.12346, 1.12346, 1.12346";
  QTest::newRow("- -> 1.12, 1.12, 1.12")
    << static_cast<int>(Qt::Key_Minus) << 1 << 2 << "1.12, 1.12, 1.12";
  QTest::newRow("-- -> 1.1, 1.1, 1.1")
    << static_cast<int>(Qt::Key_Minus) << 2 << 1 << "1.1, 1.1, 1.1";
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testCoordinatesChangedSignal()
{
  QFETCH(int, dimension);
  QFETCH(QVector4D, input1);
  QFETCH(QVector4D, input2);
  QFETCH(QVector4D, expected2);
  QFETCH(bool, emitExpected);

  ctkCoordinatesWidget coordinatesWidget;
  coordinatesWidget.setDimension(dimension);
  coordinatesWidget.setCoordinates(input1.x(), input1.y(), input1.z(), input1.w());

  QSignalSpy spy(&coordinatesWidget,
    SIGNAL(coordinatesChanged(double,double,double,double)));
  coordinatesWidget.setCoordinates(input2.x(), input2.y(), input2.z(), input2.w());

  if (!emitExpected)
  {
    QCOMPARE(spy.count(), 0);
    return;
  }
  QCOMPARE(spy.count(), 1);
  QList<QVariant> args = spy.takeFirst();
  QCOMPARE(args[0].toDouble(), static_cast<double>(expected2.x()));
  QCOMPARE(args[1].toDouble(), static_cast<double>(expected2.y()));
  QCOMPARE(args[2].toDouble(), static_cast<double>(expected2.z()));
  QCOMPARE(args[3].toDouble(), static_cast<double>(expected2.w()));
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testCoordinatesChangedSignal_data()
{
  QTest::addColumn<int>("dimension");
  QTest::addColumn<QVector4D>("input1");
  QTest::addColumn<QVector4D>("input2");
  QTest::addColumn<QVector4D>("expected2");
  QTest::addColumn<bool>("emitExpected");

  QTest::newRow("3D: w is always 0")
    << 3
    << QVector4D(0.0f, 0.0f, 0.0f, 0.0f)
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f) << QVector4D(1.0f, 2.0f, 3.0f, 0.0f) << true;
  QTest::newRow("3D: no change")
    << 3
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f)
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f) << QVector4D() << false;
  QTest::newRow("4D: all components")
    << 4
    << QVector4D(0.0f, 0.0f, 0.0f, 0.0f)
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f) << QVector4D(1.0f, 2.0f, 3.0f, 4.0f) << true;
  QTest::newRow("4D: no change")
    << 4
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f)
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f) << QVector4D() << false;
  QTest::newRow("2D: z and w are 0")
    << 2
    << QVector4D(0.0f, 0.0f, 0.0f, 0.0f)
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f) << QVector4D(1.0f, 2.0f, 0.0f, 0.0f) << true;
  QTest::newRow("2D: no change")
    << 2
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f)
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f) << QVector4D() << false;
  QTest::newRow("3D: negative values")
    << 3
    << QVector4D(0.0f, 0.0f, 0.0f, 0.0f)
    << QVector4D(-1.0f, -2.0f, -3.0f, -4.0f) << QVector4D(-1.0f, -2.0f, -3.0f, 0.0f) << true;
  QTest::newRow("3D: change to zeros")
    << 3
    << QVector4D(1.0f, 2.0f, 3.0f, 4.0f)
    << QVector4D(0.0f, 0.0f, 0.0f, 0.0f) << QVector4D(0.0f, 0.0f, 0.0f, 0.0f) << true;
}

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testCoordinatesChangedNotFiredIfUnchanged()
{
  ctkCoordinatesWidget coordinatesWidget;
  QSignalSpy spy(&coordinatesWidget,
    SIGNAL(coordinatesChanged(double,double,double,double)));

  // First set: should emit
  coordinatesWidget.setCoordinates(1.0, 2.0, 3.0);
  QCOMPARE(spy.count(), 1);

  // Same values: should not emit again
  coordinatesWidget.setCoordinates(1.0, 2.0, 3.0);
  QCOMPARE(spy.count(), 1);

  // Different values: should emit
  coordinatesWidget.setCoordinates(4.0, 5.0, 6.0);
  QCOMPARE(spy.count(), 2);

  // Same values as last set: should not emit again
  coordinatesWidget.setCoordinates(4.0, 5.0, 6.0);
  QCOMPARE(spy.count(), 2);
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCoordinatesWidgetTest)
#include "ctkCoordinatesWidgetTest.moc"
