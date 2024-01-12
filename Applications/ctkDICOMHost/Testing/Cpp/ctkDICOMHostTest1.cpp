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
#include <QStringList>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDICOMHostTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  arguments.pop_front(); // remove "program" name
  if (!arguments.count())
    {
    std::cerr << "Usage: ctkDICOMHostTest1 /path/to/ctkDICOMHost" << std::endl;
    return EXIT_FAILURE;
    }

  QString command = arguments.at(0);
  QProcess process;
  process.start(command, /* arguments= */ QStringList());
  bool res = process.waitForStarted();
  if (!res)
    {
    std::cerr << '\"' << qPrintable(command) << '\"'
              << " didn't start correctly" << std::endl;
    return res ? EXIT_SUCCESS : EXIT_FAILURE;
    }
  process.kill();
  res = process.waitForFinished();
  if (!res)
    {
    std::cerr << '\"' << qPrintable(command) << '\"'
              << " failed to terminate" << std::endl;
    return res ? EXIT_SUCCESS : EXIT_FAILURE;
    }
  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}
