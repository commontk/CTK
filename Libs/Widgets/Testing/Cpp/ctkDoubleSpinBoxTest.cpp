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
#include "ctkDoubleSpinBox.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkDoubleSpinBoxTester: public QObject
{
  Q_OBJECT
private slots:
  void testUI();

  void testToLocals();

  void testDecimalsByKey();
  void testDecimalsByKey_data();

  void testPrefix();
  void testPrefix_data();

  void testDecimalsByValue();
  void testDecimalsByValue_data();

  void testDecimalPointAlwaysVisible();
  void testDecimalPointAlwaysVisible_data();
};

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testUI()
{
  ctkDoubleSpinBox spinBox;
  spinBox.setMinimum(-100.);
  spinBox.setMaximum(100.);
  spinBox.setDecimalsOption( ctkDoubleSpinBox::DecimalsByValue |ctkDoubleSpinBox::DecimalsByShortcuts );
  spinBox.setValue(26.2110001);
  spinBox.setPrefix("A: ");
  spinBox.setSetMode(ctkDoubleSpinBox::SetAlways);
  spinBox.show();
  QTest::qWaitForWindowShown(&spinBox);
  QObject::connect(&spinBox, SIGNAL(valueChanged(double)),
                   &spinBox, SLOT(setValue(double)), Qt::QueuedConnection);

  QDoubleSpinBox doubleSpinBox;
  doubleSpinBox.setMinimum(-100.);
  doubleSpinBox.setMaximum(100.);
  doubleSpinBox.setValue(2.);
  //doubleSpinBox.show();
  //QTest::qWaitForWindowShown(&doubleSpinBox);

  //qApp->exec();
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testToLocals()
{
  bool ok;
  QLocale().toDouble("+.0", &ok);
  qDebug() << "+.0" << ok;
  QLocale().toDouble("0.0 1", &ok);
  qDebug() << "0.0 1" << ok;
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testDecimalsByKey()
{
  ctkDoubleSpinBox spinBox;
  spinBox.setMinimum(-200.);
  spinBox.setMaximum(200.);
  spinBox.setValue(1.23);

  QFETCH(int, decimalsOptions);
  spinBox.setDecimalsOption( static_cast<ctkDoubleSpinBox::DecimalsOptions>(decimalsOptions) );
  const int oldDecimals = spinBox.decimals();

  QFETCH(int, cursorPosition);
  QFETCH(int, key);

  spinBox.lineEdit()->setCursorPosition(cursorPosition);
  //spinBox.show();
  //QTest::qWaitForWindowShown(&spinBox);
  //qApp->exec();
  QSignalSpy spy(&spinBox, SIGNAL(decimalsChanged(int)));
  QTest::keyClick(spinBox.lineEdit(), static_cast<Qt::Key>(key));

  QFETCH(QString, expectedText);
  QFETCH(int, expectedDecimals);

  QCOMPARE(spinBox.text(), expectedText);
  QCOMPARE(spinBox.value(), expectedText.toDouble());
  QCOMPARE(spinBox.decimals(), expectedDecimals);
  QCOMPARE(spy.count(), spinBox.decimals() != oldDecimals ? 1 : 0);
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testDecimalsByKey_data()
{
  QTest::addColumn<int>("decimalsOptions");
  QTest::addColumn<int>("cursorPosition");
  QTest::addColumn<int>("key");
  QTest::addColumn<QString>("expectedText");
  QTest::addColumn<int>("expectedDecimals");

  QList<int> options;
  // ctkDoubleSpinBox::DecimalsByKey
  options << ctkDoubleSpinBox::DecimalsByKey;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 0:'1' -> 11.23") << options.last() << 0 << int(Qt::Key_1) << "11.23"<< 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 0:'del' -> .23") << options.last() << 0 << int(Qt::Key_Delete) << ".23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 0:'backspace' -> 1.23") << options.last() << 0 << int(Qt::Key_Backspace) << "1.23" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 1:'1' -> 11.23") << options.last() << 1 << int(Qt::Key_1) << "11.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 1:'del' -> 123") << options.last() << 1 << int(Qt::Key_Delete) << "123" << 0;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 1:'backspace' -> .23") << options.last() << 1 << int(Qt::Key_Backspace) << ".23" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 2:'1' -> 1.12") << options.last() << 2 << int(Qt::Key_1) << "1.12" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 2:'del' -> 1.3") << options.last() << 2 << int(Qt::Key_Delete) << "1.3" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 2:'backspace' -> 123") << options.last() << 2 << int(Qt::Key_Backspace) << "123" << 0;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 3:'1' -> 1.21") << options.last() << 3 << int(Qt::Key_1) << "1.21" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 3:'del' -> 1.2") << options.last() << 3 << int(Qt::Key_Delete) << "1.2" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 3:'backspace' -> 1.3") << options.last() << 3 << int(Qt::Key_Backspace) << "1.3" << 1;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 4:'1' -> 1.231") << options.last() << 4 << int(Qt::Key_1) << "1.231" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 4:'del' -> 1.23") << options.last() << 4 << int(Qt::Key_Delete) << "1.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 4:'backspace' -> 1.2") << options.last() << 4 << int(Qt::Key_Backspace) << "1.2" << 1;

  // ctkDoubleSpinBox::ReplaceDecimals
  options << ctkDoubleSpinBox::ReplaceDecimals;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 0:'1' -> 11.23") << options.last() << 0 << int(Qt::Key_1) << "11.23"<< 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 0:'del' -> .23") << options.last() << 0 << int(Qt::Key_Delete) << ".23" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 0:'backspace' -> 1.23") << options.last() << 0 << int(Qt::Key_Backspace) << "1.23" << 2;

  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 1:'1' -> 11.23") << options.last() << 1 << int(Qt::Key_1) << "11.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 1:'del' -> 123") << options.last() << 1 << int(Qt::Key_Delete) << "123" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 1:'backspace' -> .23") << options.last() << 1 << int(Qt::Key_Backspace) << ".23" << 2;

  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 2:'1' -> 1.13") << options.last() << 2 << int(Qt::Key_1) << "1.13" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 2:'del' -> 1.3") << options.last() << 2 << int(Qt::Key_Delete) << "1.3" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 2:'backspace' -> 123") << options.last() << 2 << int(Qt::Key_Backspace) << "123" << 2;

  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 3:'1' -> 1.21") << options.last() << 3 << int(Qt::Key_1) << "1.21" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 3:'del' -> 1.2") << options.last() << 3 << int(Qt::Key_Delete) << "1.2" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 3:'backspace' -> 1.3") << options.last() << 3 << int(Qt::Key_Backspace) << "1.3" << 2;

  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 4:'1' -> 1.23") << options.last() << 4 << int(Qt::Key_1) << "1.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 4:'del' -> 1.23") << options.last() << 4 << int(Qt::Key_Delete) << "1.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 4:'backspace' -> 1.2") << options.last() << 4 << int(Qt::Key_Backspace) << "1.2" << 2;

  // ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals
  options << (ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals);
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 0:'1' -> 11.23")
    << options.last() << 0 << int(Qt::Key_1) << "11.23"<< 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 0:'del' -> .23")
    << options.last() << 0 << int(Qt::Key_Delete) << ".23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 0:'backspace' -> 1.23")
    << options.last() << 0 << int(Qt::Key_Backspace) << "1.23" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 1:'1' -> 11.23")
    << options.last() << 1 << int(Qt::Key_1) << "11.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 1:'del' -> 123")
    << options.last() << 1 << int(Qt::Key_Delete) << "123" << 0;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 1:'backspace' -> .23")
    << options.last() << 1 << int(Qt::Key_Backspace) << ".23" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 2:'1' -> 1.13")
    << options.last() << 2 << int(Qt::Key_1) << "1.13" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 2:'del' -> 1.3")
    << options.last() << 2 << int(Qt::Key_Delete) << "1.3" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 2:'backspace' -> 123")
    << options.last() << 2 << int(Qt::Key_Backspace) << "123" << 0;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 3:'1' -> 1.21")
    << options.last() << 3 << int(Qt::Key_1) << "1.21" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 3:'del' -> 1.2")
    << options.last() << 3 << int(Qt::Key_Delete) << "1.2" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 3:'backspace' -> 1.3")
    << options.last() << 3 << int(Qt::Key_Backspace) << "1.3" << 1;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 4:'1' -> 1.231")
    << options.last() << 4 << int(Qt::Key_1) << "1.231" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 4:'del' -> 1.23")
    << options.last() << 4 << int(Qt::Key_Delete) << "1.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 4:'backspace' -> 1.23")
    << options.last() << 4 << int(Qt::Key_Backspace) << "1.2" << 1;

  // ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals
  options << (ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals);
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 0:'1' -> 11.23")
    << options.last() << 0 << int(Qt::Key_1) << "11.23"<< 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 0:'del' -> .23")
    << options.last() << 0 << int(Qt::Key_Delete) << ".23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 0:'backspace' -> 1.23")
    << options.last() << 0 << int(Qt::Key_Backspace) << "1.23" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 1:'1' -> 11.23")
    << options.last() << 1 << int(Qt::Key_1) << "11.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 1:'del' -> 123")
    << options.last() << 1 << int(Qt::Key_Delete) << "123" << 0;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 1:'backspace' -> .23")
    << options.last() << 1 << int(Qt::Key_Backspace) << ".23" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 2:'1' -> 1.123")
    << options.last() << 2 << int(Qt::Key_1) << "1.123" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 2:'del' -> 1.3")
    << options.last() << 2 << int(Qt::Key_Delete) << "1.3" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 2:'backspace' -> 123")
    << options.last() << 2 << int(Qt::Key_Backspace) << "123" << 0;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 3:'1' -> 1.213")
    << options.last() << 3 << int(Qt::Key_1) << "1.213" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 3:'del' -> 1.2")
    << options.last() << 3 << int(Qt::Key_Delete) << "1.2" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 3:'backspace' -> 1.3")
    << options.last() << 3 << int(Qt::Key_Backspace) << "1.3" << 1;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 4:'1' -> 1.231")
    << options.last() << 4 << int(Qt::Key_1) << "1.231" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 4:'del' -> 1.23")
    << options.last() << 4 << int(Qt::Key_Delete) << "1.23" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::InsertDecimals 4:'backspace' -> 1.23")
    << options.last() << 4 << int(Qt::Key_Backspace) << "1.2" << 1;

  foreach(int option, options)
    {
    // bad keys are always rejected
    for (int i = 0; i < 5; ++i)
      {
      QTest::newRow(QString("%1 %2:'a' -> 1.23").arg(option).arg(i).toLatin1())
        << option << i << int(Qt::Key_A) << "1.23" << 2;
      }
    // sign keys are only for the first digit
    QTest::newRow(QString("%1 0:'+' -> 1.23").arg(option).toLatin1())
      << option << 0 << int(Qt::Key_Plus) << "+1.23" << 2;
    QTest::newRow(QString("%1 0:'-' -> -1.23").arg(option).toLatin1())
      << option << 0 << int(Qt::Key_Minus) << "-1.23" << 2;
    for (int i = 1; i < 5; ++i)
      {
      QTest::newRow(QString("%1 %2:'+' -> 1.23").arg(option).arg(i).toLatin1())
        << option << i << int(Qt::Key_Plus) << "1.23" << 2;
      QTest::newRow(QString("%1 %2:'-' -> 1.23").arg(option).arg(i).toLatin1())
        << option << i << int(Qt::Key_Minus) << "1.23" << 2;
      }
    }
}
// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testPrefix()
{
  ctkDoubleSpinBox spinBox;
  spinBox.setPrefix("A: ");
  spinBox.setDecimalsOption( ctkDoubleSpinBox::FixedDecimals );

  QFETCH(int, cursorPosition);
  QFETCH(int, key);

  spinBox.lineEdit()->setCursorPosition(cursorPosition);
  QTest::keyClick(spinBox.lineEdit(), static_cast<Qt::Key>(key));

  //spinBox.show();
  //QTest::qWaitForWindowShown(&spinBox);
  //qApp->exec();

  QFETCH(double, expectedValue);
  QFETCH(QString, expectedText);
  QFETCH(int, expectedCursorPosition);

  QCOMPARE(spinBox.text(), expectedText);
  QCOMPARE(spinBox.value(), expectedValue);
  QCOMPARE(spinBox.decimals(), 2);
  QCOMPARE(spinBox.lineEdit()->cursorPosition(), expectedCursorPosition);
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testPrefix_data()
{
  QTest::addColumn<int>("cursorPosition");
  QTest::addColumn<int>("key");
  QTest::addColumn<QString>("expectedText");
  QTest::addColumn<double>("expectedValue");
  QTest::addColumn<int>("expectedCursorPosition");

  QTest::newRow("0:'1' -> 0.00") << 0 << int(Qt::Key_1) << "A: 0.00"<< 0.00 << 0;
  QTest::newRow("1:'1' -> 10.00") << 1 << int(Qt::Key_1) << "A: 10.00"<< 10.00 << 4;
  QTest::newRow("2:'1' -> 10.00") << 2 << int(Qt::Key_1) << "A: 10.00"<< 10.00 << 4;
  QTest::newRow("3:'1' -> 10.00") << 3 << int(Qt::Key_1) << "A: 10.00"<< 10.00 << 4;
  QTest::newRow("4:'1' -> 01.00") << 4 << int(Qt::Key_1) << "A: 01.00"<< 1.00 << 5;
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testDecimalsByValue()
{
  ctkDoubleSpinBox spinBox;
  spinBox.setMinimum(-100.);
  spinBox.setMaximum(100.);
  spinBox.setValue(1.23);
  spinBox.setDecimalsOption( ctkDoubleSpinBox::DecimalsByValue );
  QSignalSpy spy(&spinBox, SIGNAL(decimalsChanged(int)));
  const int oldDecimals = spinBox.decimals();

  QFETCH(double, value);
  spinBox.setValue(value);

  QFETCH(QString, expectedText);
  QFETCH(int, expectedDecimals);

  QCOMPARE(spinBox.text(), expectedText);
  QCOMPARE(spinBox.value(), value);
  QCOMPARE(spinBox.decimals(), expectedDecimals);
  QCOMPARE(spy.count(), spinBox.decimals() != oldDecimals ? 1 : 0);
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testDecimalsByValue_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<QString>("expectedText");
  QTest::addColumn<int>("expectedDecimals");

  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 0") << 0. << "0"<< 0;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 0.1") << 0.1 << "0.1" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 0.02") << 0.02 << "0.02" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 10.003") << 10.003 << "10.003" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue -0.0004") << -0.0004 << "-0.0004" << 4;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 0.000056") << 0.000056 << "0.000056" << 6;
  // internally represented as 123456.001109999997425
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 5.00111") << 5.00111 << "5.00111" << 5;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 0.1234567891013151") << 0.1234567891013151 << "0.1234567891013151" << 16;
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testDecimalPointAlwaysVisible()
{
  ctkDoubleSpinBox spinBox;
  spinBox.setDecimals(0);
  spinBox.setDecimalsOption( ctkDoubleSpinBox::DecimalPointAlwaysVisible );

  QFETCH(double, value);
  spinBox.setValue(value);

  QFETCH(QString, expectedText);
  QCOMPARE(spinBox.text(), expectedText);
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testDecimalPointAlwaysVisible_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<QString>("expectedText");

  QTest::newRow("ctkDoubleSpinBox::DecimalPointAlwaysVisible 0") << 0. << "0.";
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 2") << 2. << "2.";
  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 1.01") << 1.01 << "1.";
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkDoubleSpinBoxTest)
#include "moc_ctkDoubleSpinBoxTest.cpp"


