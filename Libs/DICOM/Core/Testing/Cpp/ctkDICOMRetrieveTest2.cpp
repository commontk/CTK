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
#include "ctkDICOMRetrieve.h"
#include "ctkDICOMTester.h"

// STD includes
#include <iostream>

void ctkDICOMRetrieveTest2PrintUsage()
{
  std::cout << " ctkDICOMRetrieveTest2 images" << std::endl;
}

// Test on a real local database
int ctkDICOMRetrieveTest2( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkDICOMTester tester;
  std::cerr << "ctkDICOMRetrieveTest2: Starting dcmqrscp\n";
  tester.startDCMQRSCP();
  
  QStringList arguments = app.arguments();
  arguments.pop_front(); // remove application name
  arguments.pop_front(); // remove test name
  if (!arguments.count())
    {
    ctkDICOMRetrieveTest2PrintUsage();
    return EXIT_FAILURE;
    }
  std::cerr << "ctkDICOMRetrieveTest2: Storing data to dcmqrscp\n";
  tester.storeData(arguments);

  ctkDICOMDatabase queryDatabase;

  std::cerr << "ctkDICOMRetrieveTest2: Setting up query\n";
  ctkDICOMQuery query;
  query.setCallingAETitle("CTK_AE");
  query.setCalledAETitle("CTK_AE");
  query.setHost("localhost");
  query.setPort(tester.dcmqrscpPort());

  std::cerr << "ctkDICOMRetrieveTest2: Running query\n";
  bool res = query.query(queryDatabase);
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

  std::cerr << "ctkDICOMRetrieveTest2: Setting up retrieve database\n";
  QSharedPointer<ctkDICOMDatabase> retrieveDatabase(new ctkDICOMDatabase);
  retrieveDatabase->openDatabase( "./ctkDICOM.sql" );

  ctkDICOMRetrieve retrieve;
  retrieve.setCallingAETitle("CTK_AE");
  retrieve.setCalledAETitle("CTK_AE");
  retrieve.setPort(tester.dcmqrscpPort());
  retrieve.setHost("localhost");
  retrieve.setMoveDestinationAETitle("CTK_CLIENT_AE");

  retrieve.setDatabase(retrieveDatabase);

  std::cerr << "ctkDICOMRetrieveTest2: Retrieving\n";
  foreach(const QString& study, query.studyInstanceUIDQueried())
    {
    std::cerr << "ctkDICOMRetrieveTest2: Retrieving " << study.toStdString() << "\n";
    bool res = retrieve.moveStudy(study);
    if (!res)
      {
      std::cout << "ctkDICOMRetrieve::retrieveStudy() failed. "
                << "Study " << qPrintable(study) << " can't be retrieved"
                << std::endl;
      return EXIT_FAILURE;
      }
    }

  std::cerr << "ctkDICOMRetrieveTest2: Exit success\n";

  return EXIT_SUCCESS;
}
