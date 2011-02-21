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
#include <QCoreApplication>
#include <QStringList>
#include <QVariant>

// ctkDICOMCore includes
#include "ctkDICOMRetrieve.h"

// STD includes
#include <cstdlib>
#include <iostream>

// Simple test that check the values are correctly set
int ctkDICOMRetrieveTest1( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkDICOMRetrieve retrieve;

  // check default values
  if (!retrieve.callingAETitle().isEmpty() ||
      !retrieve.calledAETitle().isEmpty() ||
      !retrieve.host().isEmpty() ||
      retrieve.callingPort() != 0 ||
      retrieve.calledPort() != 0 ||
      !retrieve.moveDestinationAETitle().isEmpty())
    {
    std::cerr << "ctkDICOMRetrieve::ctkDICOMRetrieve() failed: "
              << qPrintable(retrieve.callingAETitle()) << " "
              << qPrintable(retrieve.calledAETitle()) << " "
              << qPrintable(retrieve.host()) << " "
              << retrieve.callingPort() << " "
              << retrieve.calledPort() << " "
              << qPrintable(retrieve.moveDestinationAETitle()) << std::endl;
    return EXIT_FAILURE;
    }

  retrieve.setCallingAETitle("CallingAETitle");
  if (retrieve.callingAETitle() != "CallingAETitle")
    {
    std::cerr << "ctkDICOMRetrieve::setCallingAETitle() failed: "
              << qPrintable(retrieve.callingAETitle()) << std::endl;
    return EXIT_FAILURE;
    }

  retrieve.setCalledAETitle("CalledAETitle");
  if (retrieve.calledAETitle() != "CalledAETitle")
    {
    std::cerr << "ctkDICOMRetrieve::setCalledAETitle() failed: "
              << qPrintable(retrieve.calledAETitle()) << std::endl;
    return EXIT_FAILURE;
    }

  retrieve.setHost("host");
  if (retrieve.host() != "host")
    {
    std::cerr << "ctkDICOMRetrieve::setHost() failed: "
              << qPrintable(retrieve.host()) << std::endl;
    return EXIT_FAILURE;
    }

  retrieve.setCallingPort(80);
  if (retrieve.callingPort() != 80)
    {
    std::cerr << "ctkDICOMRetrieve::setCallingPort() failed: "
              << retrieve.callingPort() << std::endl;
    return EXIT_FAILURE;
    }

  retrieve.setCalledPort(80);
  if (retrieve.calledPort() != 80)
    {
    std::cerr << "ctkDICOMRetrieve::setCalledPort() failed: "
              << retrieve.calledPort() << std::endl;
    return EXIT_FAILURE;
    }

  QSharedPointer<ctkDICOMDatabase> dicomDatabase(new ctkDICOMDatabase);
  retrieve.setRetrieveDatabase(dicomDatabase);

  if (retrieve.retrieveDatabase() != dicomDatabase)
    {
    std::cerr << __LINE__ << ": ctkDICOMRetrieve::setRetrieveDatabase() failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  bool res = retrieve.retrieveSeries(QString());
  if (res)
    {
    std::cerr << __LINE__ << ": ctkDICOMRetrieve::retrieveSeries() should fail."
              << std::endl;
    return EXIT_FAILURE;
    }

  res = retrieve.retrieveStudy(QString());
  if (res)
    {
    std::cerr << __LINE__ << ": ctkDICOMRetrieve::retrieveStudy() should fail."
              << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
