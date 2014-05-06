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
#include <QProcess>

// STD includes
#include <cstdlib>
#include <iostream>

int ctkDICOMIndexerAppTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  QString database("test.db");

  if (argc < 2)
    {
    std::cerr << "Must specify path to ctkDICOMIndexer on command line\n";
    return EXIT_FAILURE;
    }
  std::cout << "Testing ctkDICOMIndexer: " << argv[1] << "\n";
  QString command = QString(argv[1]);

  QStringList parameters;
  parameters << "--init" << database;
  int res = QProcess::execute(command, parameters);
  if (res != EXIT_SUCCESS)
    {
    std::cerr << '\"' << qPrintable(command + " " + parameters.join(" ")) << '\"'
              << " returned " << res << std::endl;
    return res;
    }
  parameters.clear();
  parameters << "--add" << database << ".";
  res = QProcess::execute(command, parameters);
  if (res != EXIT_SUCCESS)
    {
    std::cerr << '\"' << qPrintable(command + " " + parameters.join(" ")) << '\"'
              << " returned " << res << std::endl;
    return res;
    }
  parameters.clear();
  parameters << "--cleanup" << database;
  res = QProcess::execute(command, parameters);
  if (res != EXIT_SUCCESS)
    {
    std::cerr << '\"' << qPrintable(command + " " + parameters.join(" ")) << '\"'
              << " returned " << res << std::endl;
    return res;
    }
  return res;
}
