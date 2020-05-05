/*=========================================================================

  Library:   CTK

  Copyright (c) [Organization Name]

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDir>
#include <QTemporaryFile>

// CTK includes
#include "ctkScopedCurrentDir.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkScopedCurrentDirTest1(int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  QString savedCurrentPath = QDir::currentPath();

  QTemporaryFile foo(QDir::tempPath() + "/ctkScopedCurrentDirTest1-XXXXXX.txt");
  foo.setAutoRemove(false);
  bool opened = foo.open();
  if(!opened)
    {
    std::cerr << "Line " << __LINE__ << " - Failed to create temporary file !" << std::endl;
    return EXIT_FAILURE;
    }

  {
  ctkScopedCurrentDir scopedCurrentDir(QDir::tempPath());

  QString currentPath = scopedCurrentDir.currentPath();
  QString expectedCurrentPath = QDir::tempPath();
  if (currentPath != expectedCurrentPath)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with ctkScopedCurrentDir\n"
              << "\tcurrentPath:" << qPrintable(currentPath) << "\n"
              << "\texpectedCurrentPath:" << qPrintable(expectedCurrentPath) << std::endl;
    return EXIT_FAILURE;
    }

  if (scopedCurrentDir.savedCurrentPath() != savedCurrentPath)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with ctkScopedCurrentDir\n"
              << "\tsavedCurrentPath:" << qPrintable(scopedCurrentDir.savedCurrentPath()) << "\n"
              << "\texpectedSavedCurrentPath:" << qPrintable(savedCurrentPath) << std::endl;
    return EXIT_FAILURE;
    }

  if (!QFile::exists(foo.fileName()))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with ctkScopedCurrentDir\n"
              << "\tfile [" << qPrintable(foo.fileName()) <<"] doesn't exist "
              << "in directory [" << qPrintable(QDir::currentPath()) << "]!" << std::endl;
    return EXIT_FAILURE;
    }
  }

  QString currentPath = QDir::currentPath();
  if (savedCurrentPath != currentPath)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with ctkScopedCurrentDir\n"
              << "\tsavedCurrentPath [" << qPrintable(savedCurrentPath) << "]\n"
              << "\tcurrentPath [" << qPrintable(currentPath) << "]" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
