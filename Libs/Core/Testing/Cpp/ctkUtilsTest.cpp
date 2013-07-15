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

// CTK includes
#include "ctkUtils.h"
#include "ctkTest.h"

// STD includes
#include <iostream>
#include <limits>

// ----------------------------------------------------------------------------
class ctkUtilsTester: public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();

  void testOrderOfMagnitude();
  void testOrderOfMagnitude_data();

  void testClosestPowerOfTen();
  void testClosestPowerOfTen_data();

  void testSignificantDecimals();
  void testSignificantDecimals_data();
};

// ----------------------------------------------------------------------------
void ctkUtilsTester::initTestCase()
{
  std::cout.precision(16);
  std::cerr.precision(16);
  std::cout << std::fixed;
  std::cerr << std::fixed;
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testOrderOfMagnitude()
{
  QFETCH(double, value);
  QFETCH(int, expectedOrder);
  QCOMPARE(ctk::orderOfMagnitude(value), expectedOrder);
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testOrderOfMagnitude_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<int>("expectedOrder");

  QTest::newRow("1. -> 0") << 1. << 0;
  QTest::newRow("2. -> 0") << 2. << 0;
  QTest::newRow("10. -> 1") << 10. << 1;
  QTest::newRow("11. -> 1") << 11. << 1;
  QTest::newRow("0.1 -> -1") << 0.1 << -1;
  QTest::newRow("0.11 -> -1") << 0.1 << -1;
  QTest::newRow("0.2 -> -1") << 0.2 << -1;
  QTest::newRow("0.01 -> -2") << 0.01 << -2;
  QTest::newRow("0.0000000001 -> -10") << 0.0000000001 << -10;
  QTest::newRow("10.0001 -> 1") << 10.0001 << 1;
  QTest::newRow("100000000001.0001 -> 11") << 100000000001.0001 << 11;
  QTest::newRow("0. -> min") << 0. << std::numeric_limits<int>::min();
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testClosestPowerOfTen()
{
  QFETCH(double, value);
  QFETCH(double, expectedValue);
  QFETCH(bool, compareWithEpsilon);
  const double closestValue = ctk::closestPowerOfTen(value);
  if (compareWithEpsilon)
    {
    const double epsilon = std::numeric_limits<double>::epsilon();
    QVERIFY( closestValue > expectedValue - epsilon );
    QVERIFY( closestValue < expectedValue + epsilon );
    }
  else
    {
    QCOMPARE(closestValue, expectedValue);
    }
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testClosestPowerOfTen_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("expectedValue");
  QTest::addColumn<bool>("compareWithEpsilon");

  QTest::newRow("1. -> 1.") << 1. << 1. << false;
  QTest::newRow("2. -> 1.") << 2. << 1. << false;
  QTest::newRow("10. -> 10.") << 10. << 10. << false;
  QTest::newRow("45. -> 10.") << 45. << 10. << false;
  QTest::newRow("98. -> 100.") << 98. << 100. << false;
  QTest::newRow("50. -> 10.") << 50. << 10. << false;
  QTest::newRow("-1234. -> -1000.") << -1234. << -1000. << false;
  QTest::newRow("0.01 -> 0.01") << 0.01 << 0.01 << true;
  QTest::newRow("0.00000000015 -> 0.0000000001")
    << 0.00000000015 << 0.0000000001 << true;
  QTest::newRow("0.1 -> 0.1") << 0.1 << 0.1 << true;
  QTest::newRow("0. -> 0.") << 0. << 0. << false;
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testSignificantDecimals()
{
  QFETCH(double, value);
  QFETCH(int, expectedDecimals);

  QCOMPARE(ctk::significantDecimals(value), expectedDecimals);
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testSignificantDecimals_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<int>("expectedDecimals");

  QTest::newRow("123456 -> 0") << 123456. << 0;
  QTest::newRow("123456.1 -> 1") << 123456.1 << 1;
  QTest::newRow("123456.12 -> 2") << 123456.12 << 2;
  QTest::newRow("123456.123 -> 3") << 123456.123 << 3;
  QTest::newRow("123456.122 -> 3") << 123456.122 << 3;
  QTest::newRow("123456.1223 -> 4") << 123456.1223 << 4;
  QTest::newRow("123456.1234 -> 4") << 123456.1234 << 4;
  QTest::newRow("123456.0123 -> 4") << 123456.0123 << 4;
  QTest::newRow("123456.0012 -> 4") << 123456.0012 << 4;
  QTest::newRow("123456.001234 -> 6") << 123456.001234 << 6;
  QTest::newRow("123456.000123 -> 6") << 123456.000123 << 6;
  QTest::newRow("123456.0000 -> 0") << 123456.0000 << 0;
  QTest::newRow("123456.0001 -> 4") << 123456.0001 << 4;
  QTest::newRow("123456.3333333 -> 2") << 123456.3333333 << 2;
  QTest::newRow("123456.1333333 -> 3") << 123456.1333333 << 3;
  QTest::newRow("123456.3333334 -> 2") << 123456.3333334 << 2;
  QTest::newRow("123456.00122 -> 5") << 123456.00122 << 5;
  QTest::newRow("123456.00123 -> 5") << 123456.00123 << 5;
  // internally representated as 123456.001109999997425
  QTest::newRow("123456.00111 -> 5") << 123456.00111 << 5;
  // internally representated as 123456.270000000004075
  QTest::newRow("123456.26999999999999996 -> 2")
    << 123456.26999999999999996 << 2;
  QTest::newRow("123456.863899999999987 -> 4") << 123456.863899999999987 << 4;
  QTest::newRow("0.5 -> 1") << 0.5 << 1;
  QTest::newRow("0.25 -> 2") << 0.25 << 2;
  QTest::newRow("0.125 -> 3") << 0.125 << 3;
  QTest::newRow("0.1234567891013151 -> 16") << 0.1234567891013151 << 16;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkUtilsTest)
#include "moc_ctkUtilsTest.cpp"


