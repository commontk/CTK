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
#include <QCoreApplication>

// CTKVTK includes
#include "ctkVTKObjectTestHelper.h"

// STD includes
#include <cstdlib>
#include <iostream>

int ctkVTKObjectTest1( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkVTKObjectTest objectTest;
  if (!objectTest.test())
  {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
