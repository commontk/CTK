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
#include <string>
#include <vector>

bool testSignificantDecimals(double value, int expected)
{
  int decimals = ctk::significantDecimals(value);
  if (decimals != expected)
    {
    std::cerr << std::fixed << value << " decimals: " << decimals << " -> " << expected << std::endl;
    return true;
    }
  return false;
}

//-----------------------------------------------------------------------------
int ctkUtilsSignificantDecimalsTest1(int , char *  [] )
{
  std::cout.precision(16);
  if (testSignificantDecimals(123456., 0))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.1, 1))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.12, 2))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.123, 3))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.122, 3))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.1223, 4))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.1234, 4))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.0123, 4))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.0012, 4))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.001234, 6))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.000123, 6))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.0000, 0))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.0001, 4))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.3333333, 2))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.1333333, 3))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.3333334, 2))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.00122, 5))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.00123, 5))
    {
    return EXIT_FAILURE;
    }
  // internally representated as 123456.001109999997425
  if (testSignificantDecimals(123456.00111, 5))
    {
    return EXIT_FAILURE;
    }
  // internally representated as 123456.270000000004075
  if (testSignificantDecimals(123456.26999999999999996, 2))
    {
    return EXIT_FAILURE;
    }
  if (testSignificantDecimals(123456.863899999999987, 4))
    {
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
