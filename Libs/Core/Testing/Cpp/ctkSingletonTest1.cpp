/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// QT includes
#include <QApplication>

// CTK includes
#include "ctkSingleton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
class ctkSingletonHelper
{
public:
  CTK_SINGLETON_DECLARE(ctkSingletonHelper);
};

//-----------------------------------------------------------------------------
void ctkSingletonHelper::classInitialize()
{
}

//-----------------------------------------------------------------------------
int ctkSingletonTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkSingletonHelper ctkObject;

  ctkObject.classInitialize();


  return EXIT_SUCCESS;
}

