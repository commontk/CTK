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
#include "ctkCoordinatesWidget.h"
#include "ctkLinearValueProxy.h"
#include "ctkTest.h"
#include "ctkValueProxy.h"

namespace
{
class Spy : public QObject
{
  Q_OBJECT

public:
  explicit Spy()
    {
    AcknowledgedSignals = 0;
    }

  void getSpyReport(QString coordinatesString)
    {
    QCOMPARE(AcknowledgedSignals, 1);
    AcknowledgedSignals = 0;

    QStringList coordinatesList = coordinatesString.split(',');
    QCOMPARE(coordinatesList.count(), this->Coordinates.size());
    for (int i = 0; i < this->Coordinates.size(); ++i)
      {
      QCOMPARE(this->Coordinates[i], coordinatesList[i].toDouble());
      }
    this->Coordinates.clear();
    };

public slots:
  void onCoordinatesChanged(double* coordinates)
    {
    ctkCoordinatesWidget* coordWidget =
      qobject_cast<ctkCoordinatesWidget*>(this->sender());
    QVERIFY(coordWidget != 0);
    for (int i = 0; i < coordWidget->dimension(); ++i)
      {
      this->Coordinates.append(coordinates[i]);
      }
    ++AcknowledgedSignals;
    }

public:
  QList<double> Coordinates;
  int AcknowledgedSignals;
};

// ----------------------------------------------------------------------------
QString coordinatesFromValue(double val)
{
  return QString("%1,%1,%1").arg(val);
}

} // end namespace

// ----------------------------------------------------------------------------
class ctkCoordinatesWidgetValueProxyTester: public QObject
{
  Q_OBJECT
private slots:

  void testSetValue();
  void testSetValue_data();

  void testPrecision();
  void testPrecision_data();
};

//-----------------------------------------------------------------------------
void ctkCoordinatesWidgetValueProxyTester::testSetValue()
{
  ctkCoordinatesWidget coordinatesWidget;
  coordinatesWidget.setMinimum(-200);
  coordinatesWidget.setMaximum(200);
  coordinatesWidget.setCoordinatesAsString("13.2, 13.2, 13.2");

  QFETCH(double, coefficient);
  QFETCH(double, offset);
  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  proxy.setOffset(offset);
  coordinatesWidget.setValueProxy(&proxy);

  // Spy
  Spy valueSpy;
  QObject::connect(&coordinatesWidget, SIGNAL(coordinatesChanged(double*)),
                   &valueSpy, SLOT(onCoordinatesChanged(double*)));

  // Test
  QFETCH(QString, coordinates);
  coordinatesWidget.setCoordinatesAsString(coordinates);

  QFETCH(QString, expectedCoordinates);
  valueSpy.getSpyReport(expectedCoordinates);
  QCOMPARE(coordinatesWidget.coordinatesAsString(), expectedCoordinates);
}

//-----------------------------------------------------------------------------
void ctkCoordinatesWidgetValueProxyTester::testSetValue_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::addColumn<double>("offset");
  QTest::addColumn<QString>("coordinates");
  QTest::addColumn<QString>("expectedCoordinates");

  //---------------------------------------------------------------------------
  // Offset
  QTest::newRow("Offset only") << 1. << 42.19 << "0.1,0.2,0.3"
    << "0.1,0.2,0.3";

  QTest::newRow("Offset only: less than min")
    << 1. << 42.19 << "-250.,-900.,-3000." << "-200,-200,-200";
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1. << 42.19 << "-240.3,-232.1,-200.01" << "-200,-200,-200";
  QTest::newRow("Offset only: less than min with offset")
    << 1. << -42.19 << "-160.15,-199.99,-159." << "-160.15,-199.99,-159";

  QTest::newRow("Offset only: more than max with offset")
    << 1. << 42.19 << "160.,199.9,163.32" << "160,199.9,163.32";
  QTest::newRow("Offset only: more than max")
    << 1. << -42.19 << "4830.,250.01,1e6" << "200,200,200";
  QTest::newRow("Offset only: less than max but ok with offset")
    << 1. << -42.19 << "210.3,200.01,241.03" << "200,200,200";


  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << "0.1,0.2,0.3"
    << "0.1,0.2,0.3";

  QTest::newRow("Coeff only: less than min")
    << 5. << 0. << "-510.08,-2000,-1000000." << "-200,-200,-200";
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0. << "-250.08,-399.99,-120" << "-200,-200,-120";
  QTest::newRow("Coeff only: less than min with coeff")
    << 5. << 0.<< "-42.08,-199.99,-40.01" << "-42.08,-199.99,-40.01";

  QTest::newRow("Coeff only: more than max with coeff")
    << 5. << 0. << "160.08,40.01,199.99" << "160.08,40.01,199.99";
  QTest::newRow("Coeff only: more than max")
    << 5. << 0. << "510.08,2000,1000000." << "200,200,200";
  QTest::newRow("Offset only: more than max but ok with coeff")
    << 0.5 << 0. << "380.08,399.99,200.01" << "200,200,200";


  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 12.0 << "0.1,0.2,0.3"
    << "0.1,0.2,0.3";

  QTest::newRow("Linear: less than min")
    << 5.0 << 12.0 << "-510.08,-2000,-1000000." << "-200,-200,-200";
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12.0 << "-250.08,-411.99,-120" << "-200,-200,-120";
  QTest::newRow("Linear: less than min with function")
    << 5.0 << 12.0 << "-64.08,-199.99,-52.01" << "-64.08,-199.99,-52.01";

  QTest::newRow("Linear: more than max with function")
    << 5.0 << 12.0 << "64.08,189.99,37.61" << "64.08,189.99,37.61";
  QTest::newRow("Linear: more than max")
    << 5.0 << 12.0 << "200.01,900000.0,411.99" << "200,200,200";
  QTest::newRow("Linear: more than max but ok with function")
    << 0.5 << 12.0 << "209.01,356.9,350.9" << "200,200,200";
}

//-----------------------------------------------------------------------------
void ctkCoordinatesWidgetValueProxyTester::testPrecision()
{
  ctkCoordinatesWidget coordinatesWidget;
  coordinatesWidget.setDecimalsOption(ctkDoubleSpinBox::DecimalsByValue);
  double coordinates[3] = {0., 0., 0.};
  coordinatesWidget.setCoordinates(coordinates);
  coordinatesWidget.setDecimals(3);
  coordinatesWidget.setSingleStep(0.001);
  coordinatesWidget.setRange(-10000., 10000.);
  coordinatesWidget.setDecimals(3);
  coordinatesWidget.setSingleStep(0.001);


  coordinates[0] = 1.;
  coordinates[1] = 1.;
  coordinates[1] = 1.;
//  coordinatesWidget.setCoordinates(coordinates);

  QFETCH(double, coefficient);

  ctkLinearValueProxy proxy;
  proxy.setCoefficient(coefficient);
  coordinatesWidget.setValueProxy(&proxy);

  coordinatesWidget.setCoordinates(coordinates);
  coordinates[2] = 1.3;
  coordinatesWidget.setCoordinates(coordinates);
  const double* res = coordinatesWidget.coordinates();

  QCOMPARE(coordinates[0], res[0]);
  QCOMPARE(coordinates[1], res[1]);
  QCOMPARE(coordinates[2], res[2]);
}

//-----------------------------------------------------------------------------
void ctkCoordinatesWidgetValueProxyTester::testPrecision_data()
{
  QTest::addColumn<double>("coefficient");
  QTest::newRow("1000000.") << 1000000.;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCoordinatesWidgetValueProxyTest)
#include "moc_ctkCoordinatesWidgetValueProxyTest.cpp"
