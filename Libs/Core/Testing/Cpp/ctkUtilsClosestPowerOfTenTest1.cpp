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
#include <QDebug>
#include <QStringList>

// CTK includes
#include "ctkUtils.h"

// STD includes
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------
int ctkUtilsClosestPowerOfTenTest1(int , char *  [] )
{
  std::cout.precision(16);

  if (ctk::closestPowerOfTen(1.) != 1.)
    {
    std::cerr << "closest power of 10 failed for number:"
              << 1. << ". Found " << ctk::closestPowerOfTen(1.)
              << " instead of 1." << std::endl;
    return EXIT_FAILURE;
    }
  if (ctk::closestPowerOfTen(2.) != 1.)
    {
    std::cerr << "closest power of 10 failed for number:"
              << 2. << ". Found " << ctk::closestPowerOfTen(2.)
              << " instead of 1." << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::closestPowerOfTen(10.) != 10.)
    {
    std::cerr << "closest power of 10 failed for number:"
              << 10. << ". Found " << ctk::closestPowerOfTen(10.)
              << " instead of 10." << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::closestPowerOfTen(45.) != 10.)
    {
    std::cerr << "closest power of 10 failed for number:"
              << 45. << ". Found " << ctk::closestPowerOfTen(45.)
              << " instead of 10." << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::closestPowerOfTen(98) != 100.)
    {
    std::cerr << "closest power of 10 failed for number:"
              << 98. << ". Found " << ctk::closestPowerOfTen(98.)
              << " instead of 100." << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::closestPowerOfTen(50.) != 10.)
    {
    std::cerr << "closest power of 10 failed for number:"
              << 50. << ". Found " << ctk::closestPowerOfTen(50.)
              << " instead of 10." << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::closestPowerOfTen(-1234.) != -1000)
    {
    std::cerr << "closest power of 10 failed for number:"
              << -1234 << ". Found " << ctk::closestPowerOfTen(-1234)
              << " instead of -1000" << std::endl;
    return EXIT_FAILURE;
    }
  double closest  = ctk::closestPowerOfTen(0.01);
  if ( closest < 0.01 - std::numeric_limits<double>::epsilon() ||
       closest > 0.01 + std::numeric_limits<double>::epsilon())
    {
    std::cerr << "closest power of 10 failed for number:"
              << 0.01 << ". Found " << closest
              << " instead of 0.01" << std::endl;
    return EXIT_FAILURE;
    }

  closest = ctk::closestPowerOfTen(0.00000000015);
  if ( closest < 0.0000000001 - std::numeric_limits<double>::epsilon() ||
       closest > 0.0000000001 + std::numeric_limits<double>::epsilon())
    {
    std::cerr << "closest power of 10 failed for number:"
              << 0.00000000015 << ". Found " << closest
              << " instead of 0.0000000001" << std::endl;
    return EXIT_FAILURE;
    }

  closest = ctk::closestPowerOfTen(0.1);
  if (closest < 0.1 - std::numeric_limits<double>::epsilon() ||
       closest > 0.1 + std::numeric_limits<double>::epsilon())
    {
    std::cerr << "closest power of 10 failed for number:"
              << 0.1 << ". Found " << closest
              << " instead of 0.1" << std::endl;
    return EXIT_FAILURE;
    }

  closest = ctk::closestPowerOfTen(0.);
  if (closest != 0)
    {
    std::cerr << "closest power of 10 failed for number:"
              << 0. << ". Found " << closest
              << " instead of 0" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
