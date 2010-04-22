/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// Qt includes
#include <QApplication>

// CTK includes
#include "ctkCollapsibleButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

int ctkCollapsibleButtonTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkCollapsibleButton qctkObject;


  return EXIT_SUCCESS;
}

