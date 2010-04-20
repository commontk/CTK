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
#include "ctkAbstractObjectFactory.h"

// Qt includes
#include <QApplication>

// STD includes
#include <cstdlib>
#include <iostream>

class BaseClassHelper
{
public:
};

int ctkAbstractObjectFactoryTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkAbstractObjectFactory< BaseClassHelper > ctkObject;


  return EXIT_SUCCESS;
}

