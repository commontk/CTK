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
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkColorPickerButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkColorPickerButtonTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget topLevel;
  ctkColorPickerButton colorPicker1;
  ctkColorPickerButton colorPicker2("Select a color");
  ctkColorPickerButton colorPicker3(Qt::red,"text");
  ctkColorPickerButton colorPicker4(Qt::yellow,"");
  
  QHBoxLayout* subLayout = new QHBoxLayout;
  subLayout->addStretch(1);
  subLayout->addWidget(&colorPicker4);
  subLayout->addStretch(1);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(&colorPicker1);
  layout->addWidget(&colorPicker2);
  layout->addWidget(&colorPicker3);
  layout->addLayout(subLayout);
  topLevel.setLayout(layout);

  if (!colorPicker1.text().isEmpty() ||
       colorPicker2.text() != "Select a color" ||
       colorPicker3.text() != "text" ||
       colorPicker4.text() != "")
    {
    std::cerr << "ctkColorPickerButton::ctkColorPickerButton wrong default text"
              << colorPicker1.text().toStdString() << " "
              << colorPicker2.text().toStdString() << " "
              << colorPicker3.text().toStdString() << " "
              << colorPicker4.text().toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  if (colorPicker1.color() != Qt::black || 
      colorPicker2.color() != Qt::black ||
      colorPicker3.color() != Qt::red)
    {
    std::cerr << "ctkColorPickerButton::ctkColorPickerButton wrong default color"
              << std::endl;
    return EXIT_FAILURE;
    }
  
  colorPicker3.setDisplayColorName(false);
  colorPicker4.setDisplayColorName(false);
  
  if (colorPicker3.displayColorName() ||
      colorPicker3.text() != "text")
    {
    std::cerr << "ctkColorPickerButton::setDisplayColorName failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  colorPicker1.setColor(QColor::fromRgbF(1., 0., 0.5));

  if (colorPicker1.color() != QColor::fromRgbF(1., 0., 0.5))
    {
    std::cerr << "ctkColorPickerButton::setColor failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  colorPicker2.setDialogOptions(ctkColorPickerButton::ShowAlphaChannel | ctkColorPickerButton::UseCTKColorDialog);
  if (!colorPicker2.dialogOptions().testFlag(ctkColorPickerButton::ShowAlphaChannel) ||
      !colorPicker2.dialogOptions().testFlag(ctkColorPickerButton::UseCTKColorDialog))
    {
    std::cerr << "ctkColorPickerButton::setDialogOptions failed" << std::endl;
    return EXIT_FAILURE;
    }

  topLevel.show();
  
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(300, &app, SLOT(quit()));
    }
  // toggle will eventually call :changeColor()
  QTimer::singleShot(100, &colorPicker2, SLOT(toggle()));
  return app.exec();
}

