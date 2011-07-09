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
#include <QDebug>
#include <QStringList>
#include <QVariant>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMQuery.h"
#include "ctkDICOMTester.h"

// STD includes
#include <iostream>

void ctkDICOMQueryTest2PrintUsage()
{
  std::cout << " ctkDICOMQueryTest2 images" << std::endl;
}

// Test on a real local database
int ctkDICOMQueryTest2( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkDICOMTester tester;
  tester.startDCMQRSCP();
  
  QStringList arguments = app.arguments();
  arguments.pop_front(); // remove application name
  arguments.pop_front(); // remove test name
  if (!arguments.count())
    {
    ctkDICOMQueryTest2PrintUsage();
    return EXIT_FAILURE;
    }
  tester.storeData(arguments);

  ctkDICOMDatabase database;

  ctkDICOMQuery query;
  query.setCallingAETitle("CTK_AE");
  query.setCalledAETitle("CTK_AE");
  query.setHost("localhost");
  query.setPort(tester.dcmqrscpPort());

  bool res = query.query(database);
  if (!res)
    {
    std::cout << "ctkDICOMQuery::query() failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (query.studyInstanceUIDQueried().count() == 0)
    {
    std::cout << "ctkDICOMQuery::query() failed."
              << "No study instance retrieved" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
