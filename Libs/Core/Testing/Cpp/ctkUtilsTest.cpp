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
  QTest::newRow("inf -> min") << std::numeric_limits<double>::infinity() << std::numeric_limits<int>::min();
  QTest::newRow("-inf -> min") << -std::numeric_limits<double>::infinity() << std::numeric_limits<int>::min();
  QTest::newRow("nan -> min") << std::numeric_limits<double>::quiet_NaN()  << std::numeric_limits<int>::min();
  QTest::newRow("min -> min") << std::numeric_limits<double>::min() << std::numeric_limits<int>::min();
  QTest::newRow("max -> 308") << std::numeric_limits<double>::max() << 308;
  QTest::newRow("denorm -> min") << std::numeric_limits<double>::denorm_min() << std::numeric_limits<int>::min();
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testClosestPowerOfTen()
{
  QFETCH(double, value);
  QFETCH(double, expectedValue);
  QFETCH(int, compareMode);
  const double closestValue = ctk::closestPowerOfTen(value);
  switch (compareMode)
    {
    default:
    case 0:
      QVERIFY(closestValue == expectedValue);
      break;
    case 1:
      {
      const double epsilon = std::numeric_limits<double>::epsilon();
      QVERIFY( closestValue > expectedValue - epsilon );
      QVERIFY( closestValue < expectedValue + epsilon );
      break;
      }
    case 2:
      QVERIFY( closestValue != closestValue );
      break;
    }
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testClosestPowerOfTen_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("expectedValue");
  /// 0 exact compare
  /// 1 compare with epsilon
  /// 2 isNaN
  QTest::addColumn<int>("compareMode");

  QTest::newRow("1. -> 1.") << 1. << 1. << 0;
  QTest::newRow("2. -> 1.") << 2. << 1. << 0;
  QTest::newRow("10. -> 10.") << 10. << 10. << 0;
  QTest::newRow("45. -> 10.") << 45. << 10. << 0;
  QTest::newRow("98. -> 100.") << 98. << 100. << 0;
  QTest::newRow("50. -> 10.") << 50. << 10. << 0;
  QTest::newRow("-1234. -> -1000.") << -1234. << -1000. << 0;
  QTest::newRow("0.01 -> 0.01") << 0.01 << 0.01 << 1;
  QTest::newRow("0.00000000015 -> 0.0000000001")
    << 0.00000000015 << 0.0000000001 << 1;
  QTest::newRow("0.1 -> 0.1") << 0.1 << 0.1 << 1;
  QTest::newRow("0. -> 0.") << 0. << 0. << 0;
  QTest::newRow("inf -> inf") << std::numeric_limits<double>::infinity()
                              << std::numeric_limits<double>::infinity()
                              << 0;
  QTest::newRow("-inf -> -inf") << -std::numeric_limits<double>::infinity()
                                << -std::numeric_limits<double>::infinity()
                                << 0;
  QTest::newRow("nan -> nan") << std::numeric_limits<double>::quiet_NaN()
                              << std::numeric_limits<double>::quiet_NaN()
                              << 2;
  QTest::newRow("min -> min") << std::numeric_limits<double>::min()
                              << std::numeric_limits<double>::min()
                              << 0;
  //QTest::newRow("max -> max") << std::numeric_limits<double>::max()
  //                            << 1e+308
  //                            << 0;
  QTest::newRow("denorm -> denorm") << std::numeric_limits<double>::denorm_min()
                                    << std::numeric_limits<double>::denorm_min()
                                    << 0;
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testSignificantDecimals()
{
  QFETCH(double, value);
  QFETCH(int, defaultDecimals);
  QFETCH(int, expectedDecimals);

  QCOMPARE(ctk::significantDecimals(value, defaultDecimals), expectedDecimals);
}

// ----------------------------------------------------------------------------
void ctkUtilsTester::testSignificantDecimals_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<int>("defaultDecimals");
  QTest::addColumn<int>("expectedDecimals");

  // Default decimals= -1
  QTest::newRow("123456 -> 0") << 123456. << -1 << 0;
  QTest::newRow("123456.1 -> 1") << 123456.1 << -1 << 1;
  QTest::newRow("123456.12 -> 2") << 123456.12 << -1 << 2;
  QTest::newRow("123456.123 -> 3") << 123456.123 << -1 << 3;
  QTest::newRow("123456.122 -> 3") << 123456.122 << -1 << 3;
  QTest::newRow("123456.1223 -> 4") << 123456.1223 << -1 << 4;
  QTest::newRow("123456.1234 -> 4") << 123456.1234 << -1 << 4;
  QTest::newRow("123456.0123 -> 4") << 123456.0123 << -1 << 4;
  QTest::newRow("123456.0012 -> 4") << 123456.0012 << -1 << 4;
  QTest::newRow("123456.001234 -> 6") << 123456.001234 << -1 << 6;
  QTest::newRow("123456.000123 -> 6") << 123456.000123 << -1 << 6;
  QTest::newRow("123456.0000 -> 0") << 123456.0000 << -1 << 0;
  QTest::newRow("123456.0001 -> 4") << 123456.0001 << -1 << 4;
  QTest::newRow("123456.3333333 -> 2") << 123456.3333333 << -1 << 2;
  QTest::newRow("123456.1333333 -> 3") << 123456.1333333 << -1 << 3;
  QTest::newRow("123456.3333334 -> 2") << 123456.3333334 << -1 << 2;
  QTest::newRow("123456.00122 -> 5") << 123456.00122 << -1 << 5;
  QTest::newRow("123456.00123 -> 5") << 123456.00123 << -1 << 5;
  // internally representated as 123456.001109999997425
  QTest::newRow("123456.00111 -> 5") << 123456.00111 << -1 << 5;
  // internally representated as 123456.270000000004075
  QTest::newRow("123456.26999999999999996 -> 2")
    << 123456.26999999999999996 << -1 << 2;
  QTest::newRow("123456.863899999999987 -> 4") << 123456.863899999999987 << -1 << 4;
  QTest::newRow("0.5 -> 1") << 0.5 << -1 << 1;
  QTest::newRow("0.25 -> 2") << 0.25 << -1 << 2;
  QTest::newRow("0.125 -> 3") << 0.125 << -1 << 3;
  QTest::newRow("0.1234567891013151 -> 16") << 0.1234567891013151 << -1 << 16;
  QTest::newRow("0. -> 0") << 0. << -1 << 0;
  QTest::newRow("inf -> 0") << std::numeric_limits<double>::infinity() << -1 << 0;
  QTest::newRow("-inf -> 0") << -std::numeric_limits<double>::infinity() << -1 << 0;
  QTest::newRow("nan -> -1") << std::numeric_limits<double>::quiet_NaN() << -1 << -1;
  QTest::newRow("min -> 16") << std::numeric_limits<double>::min() << -1 << 16;
  QTest::newRow("max -> 0") << std::numeric_limits<double>::max() << -1 << 0;
  QTest::newRow("denorm -> 16") << std::numeric_limits<double>::denorm_min()
                                << -1 << 16;

  // Default decimals= 3
  QTest::newRow("123456 -> 0") << 123456. << 3 << 0;
  QTest::newRow("123456.1 -> 1") << 123456.1 << 3 << 1;
  QTest::newRow("123456.12 -> 2") << 123456.12 << 3 << 2;
  QTest::newRow("123456.123 -> 3") << 123456.123 << 3 << 3;
  QTest::newRow("123456.122 -> 3") << 123456.122 << 3 << 3;
  QTest::newRow("123456.1223 -> 4") << 123456.1223 << 3 << 4;
  QTest::newRow("123456.1234 -> 4") << 123456.1234 << 3 << 4;
  QTest::newRow("123456.0123 -> 4") << 123456.0123 << 3 << 4;
  QTest::newRow("123456.0012 -> 4") << 123456.0012 << 3 << 4;
  QTest::newRow("123456.001234 -> 6") << 123456.001234 << 3 << 6;
  QTest::newRow("123456.000123 -> 6") << 123456.000123 << 3 << 6;
  QTest::newRow("123456.0000 -> 0") << 123456.0000 << 3 << 0;
  QTest::newRow("123456.0001 -> 4") << 123456.0001 << 3 << 4;
  QTest::newRow("123456.3333333 -> 2") << 123456.3333333 << 3 << 2;
  QTest::newRow("123456.1333333 -> 3") << 123456.1333333 << 3 << 3;
  QTest::newRow("123456.3333334 -> 2") << 123456.3333334 << 3 << 2;
  QTest::newRow("123456.00122 -> 5") << 123456.00122 << 3 << 5;
  QTest::newRow("123456.00123 -> 5") << 123456.00123 << 3 << 5;
  // internally representated as 123456.001109999997425
  QTest::newRow("123456.00111 -> 5") << 123456.00111 << 3 << 5;
  // internally representated as 123456.270000000004075
  QTest::newRow("123456.26999999999999996 -> 2")
    << 123456.26999999999999996 << 3 << 2;
  QTest::newRow("123456.863899999999987 -> 4") << 123456.863899999999987 << 3 << 4;
  QTest::newRow("0.5 -> 1") << 0.5 << 3 << 1;
  QTest::newRow("0.25 -> 2") << 0.25 << 3 << 2;
  QTest::newRow("0.125 -> 3") << 0.125 << 3 << 3;
  QTest::newRow("0.1234567891013151 -> 16") << 0.1234567891013151 << 3 << 3;
  QTest::newRow("0. -> 0") << 0. << 3 << 0;
  QTest::newRow("inf -> 0") << std::numeric_limits<double>::infinity() << 3 << 0;
  QTest::newRow("-inf -> 0") << -std::numeric_limits<double>::infinity() << 3 << 0;
  QTest::newRow("nan -> -1") << std::numeric_limits<double>::quiet_NaN() << 3 << -1;
  QTest::newRow("min -> 3") << std::numeric_limits<double>::min() << 3 << 3;
  QTest::newRow("max -> 0") << std::numeric_limits<double>::max() << 3 << 0;
  QTest::newRow("denorm -> 3") << std::numeric_limits<double>::denorm_min()
                                << 3 << 3;

}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkUtilsTest)
#include "moc_ctkUtilsTest.cpp"


