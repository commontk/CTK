/*=========================================================================

  Library:   CTK

  Copyright (c) University College London

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
#include <QList>
#include <QString>
#include <QDir>
#include <QApplication>
#include <QProcessEnvironment>

// CTK includes
#include "ctkTest.h"
#include "ctkCmdLineModuleDefaultPathBuilder.h"

// Others
#include <iostream>

/**
 * \fn Basic tests for ctkCmdLineModuleDefaultPathBuilderTest.
 */
int ctkCmdLineModuleDefaultPathBuilderTest(int argc, char* argv[])
{
  QApplication myApp(argc, argv);

  if (argc != 2)
  {
    qDebug() << "Usage: ctkCmdLineModuleDefaultPathBuilderTest directoryThatTheApplicationIsIn";
  }
  QString runtimeDirectoryName = argv[1];
  QDir runtimeDirectory(runtimeDirectoryName);

  ctkCmdLineModuleDefaultPathBuilder builder;

  QStringList defaultList = builder.getDirectoryList();
  if (defaultList.size() != 0)
  {
    qDebug() << "The default list should be empty";
    return EXIT_FAILURE;
  }

  builder.addCurrentDir();
  QStringList result = builder.getDirectoryList();

  qDebug() << "1. Built:" << result;

  if (result.size() != 1)
  {
    qDebug() << "Calling addCurrentDir should add one directory to the list of directories.";
    return EXIT_FAILURE;
  }

  builder.addCurrentDir("cli-modules");
  result = builder.getDirectoryList();

  qDebug() << "2. Built:" << result;

  if (result.size() != 2)
  {
    qDebug() << "Calling addCurrentDir(cli-modules) should add one directory to the list of directories.";
    return EXIT_FAILURE;
  }

  builder.clear();

  builder.addApplicationDir();
  result = builder.getDirectoryList();

  qDebug() << "3. Built:" << result;

  if (result.size() != 1)
  {
    qDebug() << "Calling addApplicationDir should return the application installation dir.";
    return EXIT_FAILURE;
  }

  builder.addApplicationDir("cli-modules");
  result = builder.getDirectoryList();

  qDebug() << "4. Built:" << result;

  if (result.size() != 2)
  {
    qDebug() << "Calling addApplicationDir(cli-modules) should return 2 directories, one the sub-directory of the other.";
    return EXIT_FAILURE;
  }

  builder.clear();

  builder.addCtkModuleLoadPathDir();
  result = builder.getDirectoryList();

  qDebug() << "5. Built:" << result;

  // If the environment variable CTK_MODULE_LOAD_PATH exists, it should point to a valid directory.
  // If it does not exist, then the list should not change.
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  qDebug() << "Environment is:" << env.toStringList();

  if (env.contains("CTK_MODULE_LOAD_PATH"))
  {
    QDir loadDir(env.value("CTK_MODULE_LOAD_PATH"));

    qDebug() << "CTK_MODULE_LOAD_PATH does exist, and is set to:" << env.value("CTK_MODULE_LOAD_PATH") << ", and isExists() returns " << loadDir.exists();

    if (loadDir.exists() && result.size() != 1)
    {
      qDebug() << "Environment variable CTK_MODULE_LOAD_PATH did exist and is valid, so there should be 1 entries";
      return EXIT_FAILURE;
    }
    else if (!loadDir.exists() && result.size() != 0)
    {
      qDebug() << "Environment variable CTK_MODULE_LOAD_PATH did exist but is invalid, so there should be 0 entries";
      return EXIT_FAILURE;
    }
  }
  else if (result.size() != 0)
  {
    qDebug() << "Environment variable CTK_MODULE_LOAD_PATH did not exist, so there should still be 0 entries.";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

