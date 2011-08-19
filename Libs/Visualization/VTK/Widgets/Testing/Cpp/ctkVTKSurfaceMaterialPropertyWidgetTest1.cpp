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
  
  double color[3];
  property->GetColor(color);
  double opacity = property->GetOpacity();
  double ambient = property->GetAmbient();
  double diffuse = property->GetDiffuse();
  double specular = property->GetSpecular();
  double specularPower = property->GetSpecularPower();
  
  bool backfaceCulling = property->GetBackfaceCulling();
  
  propertyWidget.setProperty(property);
  property->Delete();
  
  if (propertyWidget.property() != property)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setProperty() failed."
              << propertyWidget.property() << std::endl;
    return EXIT_FAILURE;
    }

  if (propertyWidget.color() != QColor::fromRgbF(color[0],color[1],color[2]))
    {
    std::cerr << "Wrong color: " << propertyWidget.color().rgb() << std::endl;
    return EXIT_FAILURE;
    }

  if (propertyWidget.opacity() != opacity)
    {
    std::cerr << "Wrong opacity: " << propertyWidget.opacity() << std::endl;
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

  if (propertyWidget.backfaceCulling() != backfaceCulling)
    {
    std::cerr << "Wrong backfaceCulling: " << propertyWidget.backfaceCulling() << std::endl;
    return EXIT_FAILURE;
    }

  property->SetColor(1., 1., 1.);

  if (propertyWidget.color() != QColor::fromRgbF(1., 1., 1.))
    {
    std::cerr << "vtkProperty::SetColor() failed: " << propertyWidget.color().rgb() << std::endl;
    return EXIT_FAILURE;
    }

  propertyWidget.setColor(Qt::red);
  property->GetColor(color);
  if (color[0] != 1. || color[1] != 0. || color[2] !=0)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setColor() failed: "
              << color[0] << " " << color[1] << " " << color[2] << std::endl;
    return EXIT_FAILURE;
    }

  property->SetOpacity(0.111);
  // Only 2 decimals are supported by the widget
  if (propertyWidget.opacity() != 0.11)
    {
    std::cerr << "vtkProperty::SetOpacity() failed: " << propertyWidget.opacity() << std::endl;
    return EXIT_FAILURE;
    }

  propertyWidget.setOpacity(0.999);
  
  if (property->GetOpacity() != 1.00)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setOpacity() failed: "
              << property->GetOpacity() << std::endl;
    return EXIT_FAILURE;
    }


  property->SetAmbient(0.5);

  if (propertyWidget.ambient() != 0.5)
    {
    std::cerr << "vtkProperty::SetAmbient() failed: " << propertyWidget.ambient() << std::endl;
    return EXIT_FAILURE;
    }

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

  propertyWidget.setSpecularPower(60);
  
  if (property->GetSpecularPower() != 50)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setSpecularPower() failed: "
              << property->GetSpecularPower() << std::endl;
    return EXIT_FAILURE;
    }

  property->SetBackfaceCulling(false);

  if (propertyWidget.backfaceCulling() != false)
    {
    std::cerr << "vtkProperty::SetBackfaceCulling() failed: " << propertyWidget.backfaceCulling() << std::endl;
    return EXIT_FAILURE;
    }

  propertyWidget.setBackfaceCulling(true);
  
  if (property->GetBackfaceCulling() != 1)
    {
    std::cerr << "ctkVTKSurfaceMaterialPropertyWidget::setBackfaceCulling() failed: "
              << property->GetBackfaceCulling() << std::endl;
    return EXIT_FAILURE;
    }

  propertyWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

