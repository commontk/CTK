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
#include <QCoreApplication>
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
  QCoreApplication myApp(argc, argv);

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

  QString tmp1 = result[0];
  QString tmp2 = result[1];
  QString expected = tmp1 + QDir::separator() + "cli-modules";

  if (tmp2 != expected)
  {
    qDebug() << "Expected " << expected << ", but got " << tmp2;
    return EXIT_FAILURE;
  }

  builder.clear();

  builder.addCtkModuleLoadPath();
  result = builder.getDirectoryList();

  qDebug() << "5. Built:" << result;

  // If the environment variable CTK_MODULE_LOAD_PATH exists, it should point to a valid list of directories.
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  qDebug() << "Environment is:" << env.toStringList();

#ifdef Q_OS_WIN32
    QString pathSeparator(";");
#else
    QString pathSeparator(":");
#endif

  if (env.contains("CTK_MODULE_LOAD_PATH"))
  {
    QString loadPath = env.value("CTK_MODULE_LOAD_PATH");
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QStringList loadPaths = loadPath.split(pathSeparator, Qt::SkipEmptyParts);
    #else
    QStringList loadPaths = loadPath.split(pathSeparator, QString::SkipEmptyParts);
    #endif

    foreach (QString path, loadPaths)
    {
      if (!loadPaths.contains(path))
      {
        qDebug() << "Expecte loadPaths to contain path=" << path;
        return EXIT_FAILURE;
      }
    }
  }

  builder.clear();

  builder.addApplicationDir("blah-blah-blah");
  builder.setStrictMode(true);
  result = builder.getDirectoryList();

  qDebug() << "6. Built:" << result;

  if (result.size() != 0)
  {
    qDebug() << "Calling addApplicationDir(blah-blah-blah) should return nothing unless we do have a sub-folder called blah-blah-blah.";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

