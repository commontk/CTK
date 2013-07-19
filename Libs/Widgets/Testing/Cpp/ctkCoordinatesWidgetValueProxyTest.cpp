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
    ctkTest::COMPARE(AcknowledgedSignals, 1);
    AcknowledgedSignals = 0;

    QStringList coordinatesList = coordinatesString.split(',');
    ctkTest::COMPARE(coordinatesList.count(), this->Coordinates.size());
    for (int i = 0; i < this->Coordinates.size(); ++i)
      {
      ctkTest::COMPARE(this->Coordinates[i], coordinatesList[i].toDouble());
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
};

//-----------------------------------------------------------------------------
void ctkCoordinatesWidgetValueProxyTester::testSetValue()
{
  // Setup
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
  QTest::newRow("Offset only") << 1.0 << 42.19 << "0.1,0.2,0.3"
    << "0.1,0.2,0.3";

  QTest::newRow("Offset only: less than min")
    << 1.0 << 42.19 << "-250.0,-900.0,-3000.0" << "-242.19,-242.19,-242.19";
  QTest::newRow("Offset only: less than min but ok with offset")
    << 1.0 << 42.19 << "-240.3,-232.1,-200.01" << "-240.3,-232.1,-200.01";
  QTest::newRow("Offset only: less than min with offset")
    << 1.0 << -42.19 << "-160.15,-199.99,-159.0" << "-157.81,-157.81,-157.81";

  QTest::newRow("Offset only: more than max with offset")
    << 1.0 << 42.19 << "160.0,199.9,163.32" << "157.81,157.81,157.81";
  QTest::newRow("Offset only: more than max")
    << 1.0 << -42.19 << "4830.0,250.01,1e6" << "242.19,242.19,242.19";
  QTest::newRow("Offset only: less than max but ok with offset")
    << 1.0 << -42.19 << "210.3,200.01,241.03" << "210.3,200.01,241.03";

  QTest::newRow("Offset only: max")
    << 1.0 << 42.19
    << coordinatesFromValue(std::numeric_limits<double>::max())
    << "157.81,157.81,157.81";

  QTest::newRow("Offset only:  min")
    << 1.0 << 42.19
    << coordinatesFromValue(- std::numeric_limits<double>::max())
    << "-242.19,-242.19,-242.19";

  QTest::newRow("Offset only: infinity")
    << 1.0 << 42.19
    << coordinatesFromValue(std::numeric_limits<double>::infinity())
    << "157.81,157.81,157.81";

  QTest::newRow("Offset only:  - infinity")
    << 1.0 << 42.19
    << coordinatesFromValue(- std::numeric_limits<double>::infinity())
    << "-242.19,-242.19,-242.19";

  QTest::newRow("Offset only: Nan")
    << 1.0 << 42.19
    << coordinatesFromValue(std::numeric_limits<double>::quiet_NaN())
    << "157.81,157.81,157.81";

  //---------------------------------------------------------------------------
  // Coefficient
  QTest::newRow("Coeff only") << 5.0 << 0.0 << "0.1,0.2,0.3"
    << "0.1,0.2,0.3";

  QTest::newRow("Coeff only: less than min")
    << 5.0 << 0.0 << "-510.08,-2000,-1000000." << "-40,-40,-40";
  QTest::newRow("Coeff only: less than min but ok with coeff")
    << 0.5 << 0.0 << "-250.08,-399.99,-120" << "-250.08,-399.99,-120";
  QTest::newRow("Coeff only: less than min with coeff")
    << 5.0 << 0.0<< "-42.08,-199.99,-40.01" << "-40,-40,-40";

  QTest::newRow("Coeff only: more than max with coeff")
    << 5.0 << 0.0 << "160.08,40.01,199.99" << "40,40,40";
  QTest::newRow("Coeff only: more than max")
    << 5.0 << 0.0 << "510.08,2000,1000000." << "40,40,40";
  QTest::newRow("Offset only: more than max but ok with coeff")
    << 0.5 << 0.0 << "380.08,399.99,200.01" << "380.08,399.99,200.01";

  QTest::newRow("Offset only: max")
    << 5.0 << 0.0
    << coordinatesFromValue(std::numeric_limits<double>::max())
    << "40,40,40";

  QTest::newRow("Offset only:  min")
    << 5.0 << 0.0
    << coordinatesFromValue(- std::numeric_limits<double>::max())
    << "-40,-40,-40";

  QTest::newRow("Offset only: infinity")
    << 5.0 << 0.0
    << coordinatesFromValue(std::numeric_limits<double>::infinity())
    << "40,40,40";

  QTest::newRow("Offset only:  - infinity")
    << 5.0 << 0.0
    << coordinatesFromValue(- std::numeric_limits<double>::infinity())
    << "-40,-40,-40";

  QTest::newRow("Offset only: Nan")
    << 5.0 << 0.0
    << coordinatesFromValue(std::numeric_limits<double>::quiet_NaN())
    << "40,40,40";

  //---------------------------------------------------------------------------
  // Linear
  QTest::newRow("Linear") << 5.0 << 12.0 << "0.1,0.2,0.3"
    << "0.1,0.2,0.3";

  QTest::newRow("Linear: less than min")
    << 5.0 << 12.0 << "-510.08,-2000,-1000000." << "-42.4,-42.4,-42.4";
  QTest::newRow("Linear: less than min but ok with function")
    << 0.5 << 12.0 << "-250.08,-411.99,-120" << "-250.08,-411.99,-120";
  QTest::newRow("Linear: less than min with function")
    << 5.0 << 12.0 << "-64.08,-199.99,-52.01" << "-42.4,-42.4,-42.4";

  QTest::newRow("Linear: more than max with function")
    << 5.0 << 12.0 << "64.08,189.99,37.61" << "37.6,37.6,37.6";
  QTest::newRow("Linear: more than max")
    << 5.0 << 12.0 << "200.01,900000.0,411.99" << "37.6,37.6,37.6";
  QTest::newRow("Offset only: more than max but ok with function")
    << 0.5 << 12.0 << "209.01,356.9,350.9" << "209.01,356.9,350.9";

  QTest::newRow("Linear: max")
    << 5.0 << 12.0
    << coordinatesFromValue(std::numeric_limits<double>::max())
    << "37.6,37.6,37.6";

  QTest::newRow("Offset only:  min")
    << 5.0 << 12.0
    << coordinatesFromValue(- std::numeric_limits<double>::max())
    << "-42.4,-42.4,-42.4";

  QTest::newRow("Offset only: infinity")
    << 5.0 << 12.0
    << coordinatesFromValue(std::numeric_limits<double>::infinity())
    << "37.6,37.6,37.6";

  QTest::newRow("Offset only:  - infinity")
    << 5.0 << 12.0
    << coordinatesFromValue(- std::numeric_limits<double>::infinity())
    << "-42.4,-42.4,-42.4";

  QTest::newRow("Offset only: Nan")
    << 5.0 << 12.0
    << coordinatesFromValue(std::numeric_limits<double>::quiet_NaN())
    << "37.6,37.6,37.6";
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCoordinatesWidgetValueProxyTest)
#include "moc_ctkCoordinatesWidgetValueProxyTest.cpp"
