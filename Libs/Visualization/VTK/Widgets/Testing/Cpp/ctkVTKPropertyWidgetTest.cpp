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

// CTK includes
#include "ctkVTKPropertyWidget.h"
#include "ctkTest.h"

// VTK includes
#include <vtkProperty.h>

// ----------------------------------------------------------------------------
class ctkVTKPropertyWidgetTester: public QObject
{
  Q_OBJECT
private slots:
  void testGUIEvents();
  void testVTKProperty();
  void testProperties();
};

// ----------------------------------------------------------------------------
void ctkVTKPropertyWidgetTester::testGUIEvents()
{
  ctkVTKPropertyWidget propertyWidget(0);
  propertyWidget.show();
  qApp->processEvents();
  propertyWidget.resize(100, 100);
  qApp->processEvents();
  propertyWidget.resize(1, 100);
  qApp->processEvents();
  propertyWidget.resize(100, 1);
  qApp->processEvents();
  propertyWidget.hide();
  qApp->processEvents();
  propertyWidget.show();
}

// ----------------------------------------------------------------------------
void ctkVTKPropertyWidgetTester::testVTKProperty()
{
  ctkVTKPropertyWidget propertyWidget(0);
  vtkProperty* property = propertyWidget.property();

  // ctkVTKPropertyWidget -> vtkProperty
  double newOpacity(0.9);
  propertyWidget.setOpacity(newOpacity);
  QCOMPARE(property->GetOpacity(), newOpacity);

  // ctkVTKPropertyWidget <- vtkProperty
  newOpacity = 0.8;
  property->SetOpacity(newOpacity);
  QCOMPARE(propertyWidget.opacity(), newOpacity);

  // new vtkProperty
  property = vtkProperty::New();
  newOpacity = 0.7;
  property->SetOpacity(newOpacity);
  propertyWidget.setProperty(property);
  QCOMPARE(propertyWidget.opacity(), newOpacity);

  // ctkVTKPropertyWidget -> vtkProperty
  newOpacity = 0.6;
  propertyWidget.setOpacity(newOpacity);
  QCOMPARE(property->GetOpacity(), newOpacity);

  // ctkVTKPropertyWidget <- vtkProperty
  newOpacity = 0.5;
  property->SetOpacity(newOpacity);
  QCOMPARE(propertyWidget.opacity(), newOpacity);

  property->Delete();
}

// ----------------------------------------------------------------------------
void ctkVTKPropertyWidgetTester::testProperties()
{
  ctkVTKPropertyWidget propertyWidget(0);

  propertyWidget.setRepresentation(0);
  QCOMPARE(propertyWidget.representation(), 0);
  propertyWidget.setPointSize(3.5);
  QCOMPARE(propertyWidget.pointSize(), 3.5);
  propertyWidget.setLineWidth(10.11);
  QCOMPARE(propertyWidget.lineWidth(), 10.11);
  propertyWidget.setFrontfaceCulling(true);
  QCOMPARE(propertyWidget.frontfaceCulling(), true);
  propertyWidget.setBackfaceCulling(true);
  QCOMPARE(propertyWidget.backfaceCulling(), true);
  propertyWidget.setColor(Qt::red);
  QCOMPARE(propertyWidget.color(), QColor(Qt::red));
  propertyWidget.setOpacity(0.1);
  QCOMPARE(propertyWidget.opacity(), 0.1);
  propertyWidget.setEdgeVisibility(true);
  QCOMPARE(propertyWidget.edgeVisibility(), true);
  propertyWidget.setEdgeColor(Qt::blue);
  QCOMPARE(propertyWidget.edgeColor(), QColor(Qt::blue));
  propertyWidget.setLighting(false);
  QCOMPARE(propertyWidget.lighting(), false);
  propertyWidget.setInterpolation(0);
  QCOMPARE(propertyWidget.interpolation(), 0);
  propertyWidget.setShading(false);
  QCOMPARE(propertyWidget.shading(), false);
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkVTKPropertyWidgetTest)
#include "moc_ctkVTKPropertyWidgetTest.cpp"
