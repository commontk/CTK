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
#include <QDebug>
#include <QStringList>

// CTK includes
#include "ctkUtils.h"

// STD includes
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <string>
#include <vector>

bool test(double value, int expected)
{
  int decimals = ctk::significantDecimals(value);
  if (decimals != expected)
    {
    std::cerr << std::fixed << value << " decimals: " << decimals << " -> " << expected << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
int ctkUtilsOrderOfMagnitudeTest1(int , char *  [] )
{
  std::cout.precision(16);

  if (ctk::orderOfMagnitude(1.) != 0)
    {
    std::cerr << "order of magnitude failed for number:"
              << 1. << ". Found " << ctk::orderOfMagnitude(1.)
              << " instead of 0" << std::endl;
    return EXIT_FAILURE;
    }
  if (ctk::orderOfMagnitude(2.) != 0)
    {
    std::cerr << "order of magnitude failed for number:"
              << 2. << ". Found " << ctk::orderOfMagnitude(2.)
              << " instead of 0" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(10.) != 1)
    {
    std::cerr << "order of magnitude failed for number:"
              << 10. << ". Found " << ctk::orderOfMagnitude(10.)
              << " instead of 1" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(11.) != 1)
    {
    std::cerr << "order of magnitude failed for number:"
              << 11. << ". Found " << ctk::orderOfMagnitude(11.)
              << " instead of 1" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(0.1) != -1)
    {
    std::cerr << "order of magnitude failed for number:"
              << 0.1 << ". Found " << ctk::orderOfMagnitude(0.1)
              << " instead of -1" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(0.11) != -1)
    {
    std::cerr << "order of magnitude failed for number:"
              << 0.11 << ". Found " << ctk::orderOfMagnitude(0.11)
              << " instead of -1" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(0.2) != -1)
    {
    std::cerr << "order of magnitude failed for number:"
              << 0.2 << ". Found " << ctk::orderOfMagnitude(0.2)
              << " instead of -1" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(0.01) != -2)
    {
    std::cerr << "order of magnitude failed for number:"
              << 0.01 << ". Found " << ctk::orderOfMagnitude(0.01)
              << " instead of -2" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(0.0000000001) != -10)
    {
    std::cerr << "order of magnitude failed for number:"
              << 0.0000000001 << ". Found " << ctk::orderOfMagnitude(0.0000000001)
              << " instead of -10" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(10.0001) != 1)
    {
    std::cerr << "order of magnitude failed for number:"
              << 10.0001 << ". Found " << ctk::orderOfMagnitude(10.0001)
              << " instead of 1" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(100000000000.0001) != 11)
    {
    std::cerr << "order of magnitude failed for number:"
              << 100000000000.0001 << ". Found " << ctk::orderOfMagnitude(100000000000.0001)
              << " instead of 11" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(100000000001.0001) != 11)
    {
    std::cerr << "order of magnitude failed for number:"
              << 100000000001.0001 << ". Found " << ctk::orderOfMagnitude(100000000001.0001)
              << " instead of 11" << std::endl;
    return EXIT_FAILURE;
    }

  if (ctk::orderOfMagnitude(0) != std::numeric_limits<int>::min())
    {
    std::cerr << "order of magnitude failed for number:"
              << 0 << ". Found " << ctk::orderOfMagnitude(0)
              << " instead of " << std::numeric_limits<int>::min() << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
