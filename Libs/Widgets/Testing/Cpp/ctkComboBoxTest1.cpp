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

// CTK includes
#include "ctkComboBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkComboBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkComboBox comboBox(0);
  if (!comboBox.defaultText().isEmpty())
    {
    std::cerr << "non empty default defaultText" << std::endl;
    return EXIT_FAILURE;
    }
  comboBox.setDefaultText("Select...");
  if (comboBox.defaultText() != "Select...")
    {
    std::cerr << "ctkComboBox::setDefaultText() failed"
              << comboBox.defaultText().toStdString() << std::endl;
    return EXIT_FAILURE;
    }
  if (comboBox.currentText() == "Select...")
    {
    std::cerr << "ctkComboBox::setDefaultText() failed" << std::endl;
    return EXIT_FAILURE;
    }
  QIcon icon = comboBox.style()->standardIcon(QStyle::SP_MessageBoxQuestion);
  comboBox.setDefaultIcon(icon);
  if (comboBox.defaultIcon().pixmap(20).toImage() !=
      icon.pixmap(20).toImage())
    {
    std::cerr << "ctkComboBox::setDefaultIcon() failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (comboBox.isDefaultForced())
    {
    std::cerr << "default of ctkComboBox::isDefaultForced() failed" << std::endl;
    return EXIT_FAILURE;
    }
  comboBox.forceDefault(true);
  if (!comboBox.isDefaultForced())
    {
    std::cerr << "ctkComboBox::setDefaultForced() failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (comboBox.elideMode() != Qt::ElideNone)
    {
    std::cerr << "Wrong default elide mode" << std::endl;
    return EXIT_FAILURE;
    }
  comboBox.setElideMode(Qt::ElideRight);
  if (comboBox.elideMode() != Qt::ElideRight)
    {
    std::cerr << "ctkComboBox::setElideMode() failed" << std::endl;
    return EXIT_FAILURE;
    }
  comboBox.addItem("Item Item Item Item Item Item Item Item 1");
  comboBox.addItem("Item Item Item Item Item Item Item Item 2");
  comboBox.addItem("Item Item Item Item Item Item Item Item 3");
  // adding items shouldn't change anything to the combobox current text
  if (comboBox.currentIndex() != 0 || 
      comboBox.currentText() != "Item Item Item Item Item Item Item Item 1")
    {
    std::cerr << "ctkComboBox::addItem failed:"
              << comboBox.currentIndex() << " "
              << comboBox.currentText().toStdString() << std::endl;
    return EXIT_FAILURE;
    }
  comboBox.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

