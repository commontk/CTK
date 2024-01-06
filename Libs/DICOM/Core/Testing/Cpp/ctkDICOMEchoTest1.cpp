/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=============================================================================*/

// Qt includes
#include <QCoreApplication>

// ctk includes
#include "ctkCoreTestingMacros.h"

// ctkDICOMCore includes
#include "ctkDICOMEcho.h"
#include "ctkDICOMTester.h"

// STD includes
#include <iostream>

int ctkDICOMEchoTest1(int argc, char * argv []) {

  QCoreApplication app(argc, argv);

  // run tester server
  ctkDICOMTester tester;
  std::cerr << "ctkDICOMEchoTest1: Starting dcmqrscp\n";
  tester.startDCMQRSCP();
  QStringList arguments = app.arguments();
  arguments.pop_front(); // remove test name
  if (!arguments.count())
    {
    return EXIT_FAILURE;
    }
  std::cerr << "ctkDICOMEchoTest1: Storing data to dcmqrscp\n";
  tester.storeData(arguments);

  ctkDICOMEcho echo;

  // Test the default values
  CHECK_QSTRING(echo.connectionName(), "");
  CHECK_QSTRING(echo.callingAETitle(), "");
  CHECK_QSTRING(echo.calledAETitle(), "");
  CHECK_QSTRING(echo.host(), "");
  CHECK_INT(echo.port(), 80);
  CHECK_INT(echo.connectionTimeout(), 3);

  // Test setting and getting
  echo.setConnectionName("connectionName");
  CHECK_QSTRING(echo.connectionName(), "connectionName");
  echo.setCallingAETitle("callingAETitle");
  CHECK_QSTRING(echo.callingAETitle(), "callingAETitle");
  echo.setCalledAETitle("calledAETitle");
  CHECK_QSTRING(echo.calledAETitle(), "calledAETitle");
  echo.setHost("host");
  CHECK_QSTRING(echo.host(), "host");
  echo.setPort(3000);
  CHECK_INT(echo.port(), 3000);
  echo.setConnectionTimeout(30);
  CHECK_INT(echo.connectionTimeout(), 30);

  // Test echo
  // this should print: Failed to establish association
  CHECK_BOOL(echo.echo(), false);

  // this has to be successful
  std::cerr << "ctkDICOMEchoTest1: Setting up echo\n";
  echo.setCallingAETitle("CTK_AE");
  echo.setCalledAETitle("CTK_AE");
  echo.setHost("localhost");
  echo.setPort(tester.dcmqrscpPort());

  std::cerr << "ctkDICOMEchoTest1: Running echo\n";
  CHECK_BOOL(echo.echo(), true);

  return EXIT_SUCCESS;
}

