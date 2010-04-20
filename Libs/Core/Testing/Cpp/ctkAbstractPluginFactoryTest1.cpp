/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// CTK includes
#include "ctkAbstractPluginFactory.h"
#include "ctkModelTester.h"

// Qt includes
#include <QApplication>

// STD includes
#include <cstdlib>
#include <iostream>


int ctkAbstractPluginFactoryTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkAbstractPluginFactory< ctkModelTester > ctkObject;


  return EXIT_SUCCESS;
}

