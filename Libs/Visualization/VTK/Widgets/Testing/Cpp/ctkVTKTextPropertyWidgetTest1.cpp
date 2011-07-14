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
#include "ctkVTKTextPropertyWidget.h"

// VTK includes
#include <vtkTextProperty.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKTextPropertyWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkVTKTextPropertyWidget textPropertyWidget(0);
  
  if (textPropertyWidget.isEnabled())
    {
    std::cerr << "No vtkTextProperty provided, should be disabled."
              << std::endl;
    return EXIT_FAILURE;
    }
  
  vtkTextProperty* textProperty = vtkTextProperty::New();
  
  double color[3];
  textProperty->GetColor(color);
  double opacity = textProperty->GetOpacity();
  QString fontFamily = textProperty->GetFontFamilyAsString();
  bool bold = textProperty->GetBold();
  bool italic = textProperty->GetItalic();
  bool shadow = textProperty->GetShadow();
  
  textPropertyWidget.setTextProperty(textProperty);
  textProperty->Delete();
  
  if (textPropertyWidget.textProperty() != textProperty)
    {
    std::cerr << "ctkVTKTextPropertyWidget::setTextProperty() failed."
              << textPropertyWidget.textProperty() << std::endl;
    return EXIT_FAILURE;
    }
  
  if (textPropertyWidget.color() != QColor::fromRgbF(color[0],color[1],color[2]))
    {
    std::cerr << "Wrong color" << std::endl;
    return EXIT_FAILURE;
    }

  if (textPropertyWidget.opacity() != opacity)
    {
    std::cerr << "Wrong opacity" << textPropertyWidget.opacity() << std::endl;
    return EXIT_FAILURE;
    }

  if (textPropertyWidget.font() != fontFamily)
    {
    std::cerr << "Wrong font" << textPropertyWidget.font().toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  if (textPropertyWidget.isBold() != bold)
    {
    std::cerr << "Wrong bold" << textPropertyWidget.isBold() << std::endl;
    return EXIT_FAILURE;
    }

  if (textPropertyWidget.isItalic() != italic)
    {
    std::cerr << "Wrong italic" << textPropertyWidget.isItalic() << std::endl;
    return EXIT_FAILURE;
    }
  
  if (textPropertyWidget.hasShadow() != shadow)
    {
    std::cerr << "Wrong shadow" << textPropertyWidget.hasShadow() << std::endl;
    return EXIT_FAILURE;
    }

  textProperty->SetColor(0., 0.5, 1.);

  if (textPropertyWidget.color() != QColor::fromRgbF(0., 0.5, 1.))
    {
    std::cerr << "vtkTextProperty::SetColor() failed" << std::endl;
    return EXIT_FAILURE;
    }

  // QColor handles floating points on 16bit integers
  textPropertyWidget.setColor(QColor::fromRgbF(0.333, 0.666, 0.999));
  textProperty->GetColor(color);

  if (color[0] < 0.3329 || color[0] > 0.3331 ||
      color[1] < 0.6659 || color[1] > 0.6661 ||
      color[2] < 0.9989 || color[2] > 0.9991)
    {
    std::cerr << "ctkVTKTextPropertyWidget::setColor() failed: "
              << color[0]<< " " << color[1] << " " << color[2] << std::endl;
    return EXIT_FAILURE;
    }

  textProperty->SetOpacity(0.2);

  if (textPropertyWidget.opacity() != 0.2)
    {
    std::cerr << "vtkTextProperty::SetOpacity() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textPropertyWidget.setOpacity(0.6);

  if (textProperty->GetOpacity() != 0.6)
    {
    std::cerr << "ctkVTKTextPropertyWidget::setOpacity() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textProperty->SetFontFamilyToCourier();

  if (textPropertyWidget.font() != "Courier")
    {
    std::cerr << "vtkTextProperty::SetFontFamily() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textPropertyWidget.setFont("Arial");

  if (QString(textProperty->GetFontFamilyAsString()) != "Arial")
    {
    std::cerr << "ctkVTKTextPropertyWidget::setFont() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textProperty->SetBold(!bold);

  if (textPropertyWidget.isBold() == bold)
    {
    std::cerr << "vtkTextProperty::SetBold() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textPropertyWidget.setBold(bold);

  if ((textProperty->GetBold() != 0) != bold)
    {
    std::cerr << "ctkVTKTextPropertyWidget::setBold() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textProperty->SetItalic(!italic);

  if (textPropertyWidget.isItalic() == italic)
    {
    std::cerr << "vtkTextProperty::SetItalic() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textPropertyWidget.setItalic(italic);

  if ((textProperty->GetItalic() != 0) != italic)
    {
    std::cerr << "ctkVTKTextPropertyWidget::setItalic() failed" << std::endl;
    return EXIT_FAILURE;
    }
  
  textProperty->SetShadow(!shadow);

  if (textPropertyWidget.hasShadow() == shadow)
    {
    std::cerr << "vtkTextProperty::SetShadow() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textPropertyWidget.setShadow(shadow);

  if ((textProperty->GetShadow() != 0) != shadow)
    {
    std::cerr << "ctkVTKTextPropertyWidget::setShadow() failed" << std::endl;
    return EXIT_FAILURE;
    }

  textPropertyWidget.setText("My custom VTK text");

  if (textPropertyWidget.text() != "My custom VTK text")
    {
    std::cerr << "ctkVTKTextPropertyWidget::setText() failed"
              << textPropertyWidget.text().toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  textPropertyWidget.setTextLabel("My text:");

  if (textPropertyWidget.textLabel() != "My text:")
    {
    std::cerr << "ctkVTKTextPropertyWidget::setTextLabel() failed"
              << textPropertyWidget.textLabel().toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  textPropertyWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

