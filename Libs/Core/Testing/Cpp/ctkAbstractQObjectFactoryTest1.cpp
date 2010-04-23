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
#include "ctkAbstractQObjectFactory.h"
#include "ctkModelTester.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkAbstractQObjectFactoryTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkAbstractQObjectFactory< ctkModelTester > ctkObject;


  return EXIT_SUCCESS;
}

