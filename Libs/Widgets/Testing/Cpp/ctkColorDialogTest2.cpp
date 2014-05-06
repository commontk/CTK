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
#include "ctkColorDialog.h"
#include "ctkColorPickerButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkColorDialogTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(true);

  ctkColorPickerButton extraPanel;
  ctkColorDialog::addDefaultTab(&extraPanel, "Extra", SIGNAL(colorChanged(QColor)));

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    // quit the opened dialog, which doesn't quit the application
    // as app.exec() is not executed yet
    QTimer::singleShot(200, &app, SLOT(quit()));
    QTimer::singleShot(300, &app, SLOT(quit()));
    }

  QColor color = ctkColorDialog::getColor(Qt::black,0 , "", QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    {
    std::cout << "The color dialog should have been quit without a valid color."
              << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
