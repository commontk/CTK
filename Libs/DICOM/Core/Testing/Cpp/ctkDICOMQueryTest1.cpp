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
#include "ctkDICOMQuery.h"

// STD includes
#include <cstdlib>
#include <iostream>

// Simple test that check the values are correctly set
int ctkDICOMQueryTest1( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkDICOMQuery query;

  // check default values
  if (!query.callingAETitle().isEmpty() ||
      !query.calledAETitle().isEmpty() ||
      !query.host().isEmpty() ||
      query.port() != 0 ||
      !query.filters().isEmpty() ||
      !query.studyInstanceUIDQueried().isEmpty())
    {
    std::cerr << "ctkDICOMQuery::ctkDICOMQuery() failed: "
              << qPrintable(query.callingAETitle()) << " "
              << qPrintable(query.calledAETitle()) << " "
              << qPrintable(query.host()) << " "
              << query.port() << " "
              << std::endl;
    return EXIT_FAILURE;
    }

  query.setCallingAETitle("CallingAETitle");
  if (query.callingAETitle() != "CallingAETitle")
    {
    std::cerr << "ctkDICOMQuery::setCallingAETitle() failed: "
              << qPrintable(query.callingAETitle()) << std::endl;
    return EXIT_FAILURE;
    }

  query.setCalledAETitle("CalledAETitle");
  if (query.calledAETitle() != "CalledAETitle")
    {
    std::cerr << "ctkDICOMQuery::setCalledAETitle() failed: "
              << qPrintable(query.calledAETitle()) << std::endl;
    return EXIT_FAILURE;
    }

  query.setHost("host");
  if (query.host() != "host")
    {
    std::cerr << "ctkDICOMQuery::setHost() failed: "
              << qPrintable(query.host()) << std::endl;
    return EXIT_FAILURE;
    }

  query.setPort(80);
  if (query.port() != 80)
    {
    std::cerr << "ctkDICOMQuery::setPort() failed: "
              << query.port() << std::endl;
    return EXIT_FAILURE;
    }

  QMap<QString,QVariant> filters;
  filters["Name"] = QString("JohnDoe");
  filters["StartDate"] = QString("20090101");
  filters["EndDate"] = QString("20091231");

  query.setFilters(filters);
  if (query.filters() != filters)
    {
    std::cerr << "ctkDICOMDatabase::setFilters() failed: "
              << query.filters().count() << std::endl;
    return EXIT_FAILURE;
    }
  ctkDICOMDatabase database;
  query.query(database);
  
  // Queried studies should be empty because we use an empty database.
  if (!query.studyInstanceUIDQueried().isEmpty())
    {
    std::cerr << "ctkDICOMDatabase::query() failed: "
              << query.studyInstanceUIDQueried().count() << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
