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
#include <QString>
#include <QTimer>

// CTK includes
#include "ctkMatrixWidget.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkMatrixWidgetTester: public QObject
{
  Q_OBJECT
private slots:
  void testUI();

  void testDecimals();
};

// ----------------------------------------------------------------------------
void ctkMatrixWidgetTester::testUI()
{
  ctkMatrixWidget matrix;
  matrix.setMinimum(-100.);
  matrix.setMaximum(100.);

  matrix.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&matrix);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&matrix);
#endif
  //qApp->exec();
}

// ----------------------------------------------------------------------------
void ctkMatrixWidgetTester::testDecimals()
{
  ctkMatrixWidget matrix;
  matrix.setDecimalsOption(
    ctkDoubleSpinBox::DecimalsByKey | ctkDoubleSpinBox::DecimalsByShortcuts );
  matrix.setDecimals(10);
  matrix.setValue(0, 0, 1.000000001);

  matrix.show();

#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&matrix);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&matrix);
#endif
  //qApp->exec();

  QCOMPARE(matrix.value(0, 0), 1.000000001);
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkMatrixWidgetTest)
#include "moc_ctkMatrixWidgetTest.cpp"


