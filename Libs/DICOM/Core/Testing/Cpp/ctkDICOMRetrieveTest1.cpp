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
  std::cerr << "Checking Defaults\n";
  if (retrieve.callingAETitle() != "ANY-SCU" ||
      retrieve.calledAETitle() != "ANY-SCP" ||
      !retrieve.host().isEmpty() ||
      retrieve.port() != 104 ||
      !retrieve.moveDestinationAETitle().isEmpty())
    {
    std::cerr << "ctkDICOMRetrieve::ctkDICOMRetrieve() failed: "
              << "callingAETitle: " << qPrintable(retrieve.callingAETitle()) << " "
              << "calledAETitle: " << qPrintable(retrieve.calledAETitle()) << " "
              << "host: " << qPrintable(retrieve.host()) << " "
              << "port: " << retrieve.port() << " "
              << "moveDestinationAETitle: " << qPrintable(retrieve.moveDestinationAETitle()) << std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "Set Variables\n";
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

  retrieve.setPort(80);
  if (retrieve.port() != 80)
    {
    std::cerr << "ctkDICOMRetrieve::setCalledPort() failed: "
              << retrieve.port() << std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "Set Database\n";
  QSharedPointer<ctkDICOMDatabase> dicomDatabase(new ctkDICOMDatabase);
  retrieve.setDatabase(dicomDatabase);

  if (retrieve.database() != dicomDatabase)
    {
    std::cerr << __LINE__ << ": ctkDICOMRetrieve::setDatabase() failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "Move Series\n";
  bool res = retrieve.moveSeries(QString(), QString());
  if (res)
    {
    std::cerr << __LINE__ << ": ctkDICOMRetrieve::moveSeries() should fail."
              << std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "Move Study\n";
  res = retrieve.moveStudy(QString());
  if (res)
    {
    std::cerr << __LINE__ << ": ctkDICOMRetrieve::moveStudy() should fail."
              << std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "Get Series\n";
  res = retrieve.getSeries(QString(), QString());
  if (res)
    {
    std::cerr << __LINE__ << ": ctkDICOMRetrieve::getSeries() should fail."
              << std::endl;
    return EXIT_FAILURE;
    }

  std::cerr << "Get Study\n";
  res = retrieve.getStudy(QString());
  if (res)
    {
    std::cerr << __LINE__ << ": ctkDICOMRetrieve::getStudy() should fail."
              << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
