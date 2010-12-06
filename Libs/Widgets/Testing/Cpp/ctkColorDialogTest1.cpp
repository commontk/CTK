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
#include <QTimer>

// CTK includes
#include "ctkColorDialog.h"
#include "ctkColorPickerButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkColorDialogTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkColorDialog colorDialog;
  ctkColorDialog colorDialog1(Qt::red);

  ctkColorPickerButton* extraPanel = new ctkColorPickerButton;
  QObject::connect(extraPanel, SIGNAL(colorChanged(QColor)),
                   &colorDialog, SLOT(setColor(QColor)));
  colorDialog.addTab(extraPanel, "Extra");
  int index = colorDialog.indexOf(extraPanel);
  if (index != 0 ||
      extraPanel != colorDialog.widget(index) ||
      colorDialog.widget(-1) != 0)
    {
    std::cerr << "ctkColorDialog::addTab failed" << std::endl;
    return EXIT_FAILURE;
    }

  // fake removeTab
  colorDialog.removeTab(-1);
  index = colorDialog.indexOf(extraPanel);
  if (index != 0 ||
      colorDialog.widget(0) != extraPanel)
    {
    std::cerr << "ctkColorDialog::removeTab failed" << std::endl;
    return EXIT_FAILURE;
    }

  // true removeTab  
  colorDialog.removeTab(index);
  index = colorDialog.indexOf(extraPanel);
  if (index != -1 ||
      colorDialog.widget(0) != 0)
    {
    std::cerr << "ctkColorDialog::removeTab failed" << std::endl;
    return EXIT_FAILURE;
    }

  // Add the panel back
  colorDialog.addTab(extraPanel, "Extra chooser");
  extraPanel->setColor(Qt::darkBlue);

  if (colorDialog.currentColor() != Qt::darkBlue)
    {
    std::cerr << "ctkColorDialog::setColor failed" << std::endl;
    return EXIT_FAILURE;
    }

  colorDialog.open();

  // the following is only in interactive mode
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &colorDialog, SLOT(accept()));
    }

  return app.exec();

  ctkColorDialog::addDefaultTab(extraPanel, "Extra");
  QColor color = ctkColorDialog::getColor(Qt::black,0 , "", 0);
  return EXIT_SUCCESS;

}
