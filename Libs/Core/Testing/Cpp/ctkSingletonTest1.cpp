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

// QT includes
#include <QtGlobal>

// CTK includes
#include "ctkSingletonTestHelper.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSingletonTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);
  
  if (!ctkSingletonTestHelper::instance())
    {
    std::cerr << "Problem with ctkSingletonTestHelper::instance()" << std::endl;
    return EXIT_FAILURE;
    }
  ctkSingletonTestHelper::instance()->registerNorthFace();
  ctkSingletonTestHelper::instance()->registerNorthFace();
  if (ctkSingletonTestHelper::instance()->northFaceCount() != 2)
    {
    std::cerr << "Problem with ctkSingletonTestHelper" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

