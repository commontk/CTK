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
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTimer>
#include <QVector3D>

// CTK includes
#include "ctkCoordinatesWidget.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkCoordinatesWidgetTester: public QObject
{
  Q_OBJECT
private slots:
  void testDefaults();

  void testNormalized();
  void testNormalized_data();
};

// ----------------------------------------------------------------------------
void ctkCoordinatesWidgetTester::testDefaults()
{
  ctkCoordinatesWidget coordinatesWidget;
  QCOMPARE(coordinatesWidget.dimension(), 3);
  QCOMPARE(coordinatesWidget.decimals(), 3);
  QCOMPARE(coordinatesWidget.singleStep(), 1.);
  QCOMPARE(coordinatesWidget.minimum(), -100000.);
  QCOMPARE(coordinatesWidget.maximum(), 100000.);
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
CTK_TEST_MAIN(ctkCoordinatesWidgetTest)
#include "moc_ctkCoordinatesWidgetTest.cpp"
