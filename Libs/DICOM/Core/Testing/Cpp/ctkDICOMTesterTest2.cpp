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
#include <QStringList>

// ctkDICOMCore includes
#include "ctkDICOMTester.h"

// STD includes
#include <iostream>
#include <cstdlib>

void ctkDICOMTesterTest2PrintUsage()
{
  std::cout << " ctkDICOMTesterTest2 images" << std::endl;
}

int ctkDICOMTesterTest2(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  arguments.pop_front();
  if (!arguments.count())
    {
    ctkDICOMTesterTest2PrintUsage();
    return EXIT_FAILURE;
    }

  ctkDICOMTester tester;
  tester.startDCMQRSCP();

  bool res = tester.storeData(arguments);

  if (!res)
    {
    std::cout << "Can't store data" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

