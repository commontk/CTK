/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QTimer>

// CTK includes
#include "ctkVTKSurfaceMaterialPropertyWidget.h"

// VTK includes
#include <vtkProperty.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKSurfaceMaterialPropertyWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkVTKSurfaceMaterialPropertyWidget propertyWidget(0);
  
  if (propertyWidget.isEnabled())
    {
    std::cerr << "No vtkProperty provided, should be disabled."
              << std::endl;
    return EXIT_FAILURE;
    }
  
  vtkProperty* property = vtkProperty::New();
  
  double ambient = property->GetAmbient();
  double diffuse = property->GetDiffuse();
  double specular = property->GetSpecular();
  double specularPower = property->GetSpecularPower();
  
  propertyWidget.setProperty(property);
  property->Delete();
  
  if (propertyWidget.property() != property)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setProperty() failed."
              << propertyWidget.property() << std::endl;
    return EXIT_FAILURE;
    }
  
  if (propertyWidget.ambient() != ambient)
    {
    std::cerr << "Wrong ambient: " << propertyWidget.ambient() << std::endl;
    return EXIT_FAILURE;
    }

  if (propertyWidget.diffuse() != diffuse)
    {
    std::cerr << "Wrong diffuse: " << propertyWidget.diffuse() << std::endl;
    return EXIT_FAILURE;
    }

  if (propertyWidget.specular() != specular)
    {
    std::cerr << "Wrong specular: " << propertyWidget.specular() << std::endl;
    return EXIT_FAILURE;
    }

  if (propertyWidget.specularPower() != specularPower)
    {
    std::cerr << "Wrong specularPower: " << propertyWidget.specularPower() << std::endl;
    return EXIT_FAILURE;
    }

  property->SetAmbient(0.5);

  if (propertyWidget.ambient() != 0.5)
    {
    std::cerr << "vtkProperty::SetAmbient() failed: " << propertyWidget.ambient() << std::endl;
    return EXIT_FAILURE;
    }

  // QColor handles floating points on 16bit integers
  propertyWidget.setAmbient(0.8);
  
  if (property->GetAmbient() != 0.8)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setAmbient() failed: "
              << property->GetAmbient() << std::endl;
    return EXIT_FAILURE;
    }

  property->SetDiffuse(1.2);

  if (propertyWidget.diffuse() != 1.)
    {
    std::cerr << "vtkProperty::SetDiffuse() failed: " << propertyWidget.diffuse() << std::endl;
    return EXIT_FAILURE;
    }

  // QColor handles floating points on 16bit integers
  propertyWidget.setDiffuse(0.3);
  
  if (property->GetDiffuse() != 0.3)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setDiffuse() failed: "
              << property->GetDiffuse() << std::endl;
    return EXIT_FAILURE;
    }

  property->SetSpecular(0.99);

  if (propertyWidget.specular() != 0.99)
    {
    std::cerr << "vtkProperty::SetSpecular() failed: " << propertyWidget.specular() << std::endl;
    return EXIT_FAILURE;
    }

  // QColor handles floating points on 16bit integers
  propertyWidget.setSpecular(0.01);
  
  if (property->GetSpecular() != 0.01)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setSpecular() failed: "
              << property->GetSpecular() << std::endl;
    return EXIT_FAILURE;
    }

  property->SetSpecularPower(45);

  if (propertyWidget.specularPower() != 45)
    {
    std::cerr << "vtkProperty::SetSpecularPower() failed: " << propertyWidget.specularPower() << std::endl;
    return EXIT_FAILURE;
    }

  // QColor handles floating points on 16bit integers
  propertyWidget.setSpecularPower(60);
  
  if (property->GetSpecularPower() != 50)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setSpecularPower() failed: "
              << property->GetSpecularPower() << std::endl;
    return EXIT_FAILURE;
    }

  propertyWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

