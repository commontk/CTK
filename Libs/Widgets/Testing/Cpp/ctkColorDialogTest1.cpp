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
#include <QCheckBox>

// CTK includes
#include "ctkColorDialog.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkColorDialogTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkColorDialog colorDialog;
  QWidget* extraPanel = new QWidget;
  colorDialog.addTab(extraPanel, "Extra");
  if (extraPanel != colorDialog.widget(0))
    {
    return EXIT_FAILURE;
    }
  // the following is only in interactive mode
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    return EXIT_SUCCESS;
    }
 if (!colorDialog.exec())
    {
    return EXIT_FAILURE;
    }
  ctkColorDialog::addDefaultTab(extraPanel, "Extra");
  QColor color = ctkColorDialog::getColor(Qt::black,0 , "", 0);
  return EXIT_SUCCESS;

}
