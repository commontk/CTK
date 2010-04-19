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
#include "ctkAbstractLibraryFactory.h"

// QT includes
#include <QApplication>

// STD includes
#include <cstdlib>
#include <iostream>

class BaseClassHelperType
{
public:
};

class FactoryItemHelper
{
public:
  FactoryItemHelper( QString &, QString )
    {
    }
  void setSymbols(const QStringList& )
    {
    }
};

int ctkAbstractLibraryFactoryTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

//  typedef ctkAbstractLibraryFactory < BaseClassHelperType, FactoryItemHelper >  FactoryType;
//  FactoryType  ctkObject;


  return EXIT_SUCCESS;
}

