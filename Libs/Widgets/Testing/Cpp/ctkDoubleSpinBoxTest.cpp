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

  void testDecimalsByValue();
  void testDecimalsByValue_data();
};

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testUI()
{
  ctkDoubleSpinBox spinBox;
  spinBox.setMinimum(-100.);
  spinBox.setMaximum(100.);
  spinBox.setValue(1.);
  spinBox.setDecimalsOption( ctkDoubleSpinBox::DecimalsByKey );
  spinBox.show();

  QDoubleSpinBox doubleSpinBox;
  doubleSpinBox.setMinimum(-100.);
  doubleSpinBox.setMaximum(100.);
  doubleSpinBox.setValue(2.);
  doubleSpinBox.show();

  QTest::qWaitForWindowShown(&spinBox);
  QTest::qWaitForWindowShown(&doubleSpinBox);

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
  spinBox.setMinimum(-100.);
  spinBox.setMaximum(100.);
  spinBox.setValue(1.);

  QFETCH(int, decimalsOptions);
  spinBox.setDecimalsOption( static_cast<ctkDoubleSpinBox::DecimalsOptions>(decimalsOptions) );

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
  QCOMPARE(spy.count(), spinBox.decimals() != 2 ? 1 : 0);
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
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 0:'1' -> 11.00") << options.last() << 0 << int(Qt::Key_1) << "11.00"<< 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 0:'-' -> -1.00") << options.last() << 0 << int(Qt::Key_Minus) << "-1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 0:'del' -> .00") << options.last() << 0 << int(Qt::Key_Delete) << ".00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 0:'backspace' -> 1.00") << options.last() << 0 << int(Qt::Key_Backspace) << "1.00" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 1:'1' -> 11.00") << options.last() << 1 << int(Qt::Key_1) << "11.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 1:'-' -> 1.00") << options.last() << 1 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 1:'del' -> 100.") << options.last() << 1 << int(Qt::Key_Delete) << "100." << 0;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 1:'backspace' -> .00") << options.last() << 1 << int(Qt::Key_Backspace) << ".00" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 2:'1' -> 1.100") << options.last() << 2 << int(Qt::Key_1) << "1.100" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 2:'-' -> 1.00") << options.last() << 2 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 2:'del' -> 1.0") << options.last() << 2 << int(Qt::Key_Delete) << "1.0" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 2:'backspace' -> 100.") << options.last() << 2 << int(Qt::Key_Backspace) << "100." << 0;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 3:'1' -> 1.010") << options.last() << 3 << int(Qt::Key_1) << "1.010" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 3:'-' -> 1.00") << options.last() << 3 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 3:'del' -> 1.0") << options.last() << 3 << int(Qt::Key_Delete) << "1.0" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 3:'backspace' -> 1.0") << options.last() << 3 << int(Qt::Key_Backspace) << "1.0" << 1;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 4:'1' -> 1.001") << options.last() << 4 << int(Qt::Key_1) << "1.001" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 4:'-' -> 1.00") << options.last() << 4 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 4:'del' -> 1.00") << options.last() << 4 << int(Qt::Key_Delete) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey 4:'backspace' -> 1.0") << options.last() << 4 << int(Qt::Key_Backspace) << "1.0" << 1;

  // ctkDoubleSpinBox::ReplaceDecimals
  options << ctkDoubleSpinBox::ReplaceDecimals;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 0:'1' -> 11.00") << options.last() << 0 << int(Qt::Key_1) << "11.00"<< 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 0:'-' -> -1.00") << options.last() << 0 << int(Qt::Key_Minus) << "-1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 0:'del' -> .00") << options.last() << 0 << int(Qt::Key_Delete) << ".00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 0:'backspace' -> 1.00") << options.last() << 0 << int(Qt::Key_Backspace) << "1.00" << 2;

  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 1:'1' -> 11.00") << options.last() << 1 << int(Qt::Key_1) << "11.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 1:'-' -> 1.00") << options.last() << 1 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 1:'del' -> 100") << options.last() << 1 << int(Qt::Key_Delete) << "100" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 1:'backspace' -> .00") << options.last() << 1 << int(Qt::Key_Backspace) << ".00" << 2;

  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 2:'1' -> 1.10") << options.last() << 2 << int(Qt::Key_1) << "1.10" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 2:'-' -> 1.00") << options.last() << 2 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 2:'del' -> 1.00") << options.last() << 2 << int(Qt::Key_Delete) << "1.0" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 2:'backspace' -> 100") << options.last() << 2 << int(Qt::Key_Backspace) << "100" << 2;

  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 3:'1' -> 1.01") << options.last() << 3 << int(Qt::Key_1) << "1.01" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 3:'-' -> 1.00") << options.last() << 3 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 3:'del' -> 1.0") << options.last() << 3 << int(Qt::Key_Delete) << "1.0" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 3:'backspace' -> 1.00") << options.last() << 3 << int(Qt::Key_Backspace) << "1.0" << 2;

  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 4:'1' -> 1.00") << options.last() << 4 << int(Qt::Key_1) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 4:'-' -> 1.00") << options.last() << 4 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 4:'del' -> 1.00") << options.last() << 4 << int(Qt::Key_Delete) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::ReplaceDecimals 4:'backspace' -> 1.00") << options.last() << 4 << int(Qt::Key_Backspace) << "1.0" << 2;

  // ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals
  options << (ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals);
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 0:'1' -> 11.00")
    << options.last() << 0 << int(Qt::Key_1) << "11.00"<< 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 0:'-' -> -1.00")
    << options.last() << 0 << int(Qt::Key_Minus) << "-1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 0:'del' -> .00")
    << options.last() << 0 << int(Qt::Key_Delete) << ".00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 0:'backspace' -> 1.00")
    << options.last() << 0 << int(Qt::Key_Backspace) << "1.00" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 1:'1' -> 11.00")
    << options.last() << 1 << int(Qt::Key_1) << "11.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 1:'-' -> 1.00")
    << options.last() << 1 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 1:'del' -> 100.")
    << options.last() << 1 << int(Qt::Key_Delete) << "100." << 0;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 1:'backspace' -> .00")
    << options.last() << 1 << int(Qt::Key_Backspace) << ".00" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 2:'1' -> 1.10")
    << options.last() << 2 << int(Qt::Key_1) << "1.10" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 2:'-' -> 1.00")
    << options.last() << 2 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 2:'del' -> 1.0")
    << options.last() << 2 << int(Qt::Key_Delete) << "1.0" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 2:'backspace' -> 100.")
    << options.last() << 2 << int(Qt::Key_Backspace) << "100." << 0;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 3:'1' -> 1.01")
    << options.last() << 3 << int(Qt::Key_1) << "1.01" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 3:'-' -> 1.00")
    << options.last() << 3 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 3:'del' -> 1.0")
    << options.last() << 3 << int(Qt::Key_Delete) << "1.0" << 1;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 3:'backspace' -> 1.0")
    << options.last() << 3 << int(Qt::Key_Backspace) << "1.0" << 1;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 4:'1' -> 1.001")
    << options.last() << 4 << int(Qt::Key_1) << "1.001" << 3;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 4:'-' -> 1.00")
    << options.last() << 4 << int(Qt::Key_Minus) << "1.00" << 2;
  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 4:'del' -> 1.00")
    << options.last() << 4 << int(Qt::Key_Delete) << "1.00" << 2;

  QTest::newRow("ctkDoubleSpinBox::DecimalsByKey|ctkDoubleSpinBox::ReplaceDecimals 4:'backspace' -> 1.00")
    << options.last() << 4 << int(Qt::Key_Backspace) << "1.0" << 1;

  foreach(int option, options)
    {
    // bad keys are always rejected
    for (int i = 0; i < 5; ++i)
      {
      QTest::newRow(QString("%1 %2:'a' -> 1.00").arg(option).arg(i).toLatin1())
        << option << i << int(Qt::Key_A) << "1.00" << 2;
      }
    // sign keys are only for the first digit
    QTest::newRow(QString("%1 0:'+' -> 1.00").arg(option).toLatin1())
      << option << 0 << int(Qt::Key_Plus) << "+1.00" << 2;
    QTest::newRow(QString("%1 0:'-' -> -1.00").arg(option).toLatin1())
      << option << 0 << int(Qt::Key_Minus) << "-1.00" << 2;
    for (int i = 1; i < 5; ++i)
      {
      QTest::newRow(QString("%1 %2:'+' -> 1.00").arg(option).arg(i).toLatin1())
        << option << i << int(Qt::Key_Plus) << "1.00" << 2;
      QTest::newRow(QString("%1 %2:'-' -> 1.00").arg(option).arg(i).toLatin1())
        << option << i << int(Qt::Key_Minus) << "1.00" << 2;
      }
    }
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testDecimalsByValue()
{
  ctkDoubleSpinBox spinBox;
  spinBox.setMinimum(-100.);
  spinBox.setMaximum(100.);
  spinBox.setValue(1.);
  spinBox.setDecimalsOption( ctkDoubleSpinBox::DecimalsByValue );
  QSignalSpy spy(&spinBox, SIGNAL(decimalsChanged(int)));

  QFETCH(double, value);
  spinBox.setValue(value);

  QFETCH(QString, expectedText);
  QFETCH(int, expectedDecimals);

  QCOMPARE(spinBox.text(), expectedText);
  QCOMPARE(spinBox.value(), value);
  QCOMPARE(spinBox.decimals(), expectedDecimals);
  QCOMPARE(spy.count(), spinBox.decimals() != 2 ? 1 : 0);
}

// ----------------------------------------------------------------------------
void ctkDoubleSpinBoxTester::testDecimalsByValue_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<QString>("expectedText");
  QTest::addColumn<int>("expectedDecimals");

  QTest::newRow("ctkDoubleSpinBox::DecimalsByValue 0.00") << 0.00 << "0."<< 0;
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
CTK_TEST_MAIN(ctkDoubleSpinBoxTest)
#include "moc_ctkDoubleSpinBoxTest.cpp"


