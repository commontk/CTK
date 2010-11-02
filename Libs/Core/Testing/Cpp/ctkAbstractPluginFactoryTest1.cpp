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
#include <QApplication>

// CTK includes
#include "ctkAbstractPluginFactory.h"
#include "ctkDummyPlugin.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkAbstractPluginFactoryTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkAbstractPluginFactory< ctkDummyPlugin > pluginFactory;
  pluginFactory.setVerbose(true);
  if (argc <= 1)
    {
    std::cerr << "Missing argument" << std::endl;
    return EXIT_FAILURE;
    }
  QString filePath(argv[1]);
  QFileInfo file(filePath);
  while (filePath.contains("$(OutDir)"))
    {
    QString debugFilePath = filePath;
    debugFilePath.replace("$(OutDir)","Debug");
    if (QFile::exists(QString(debugFilePath)))
      {
      file = QFileInfo(debugFilePath);
      break;
      }
    QString releaseFilePath = filePath;
    releaseFilePath.replace("$(OutDir)","Release");
    if (QFile::exists(QString(releaseFilePath)))
      {
      file = QFileInfo(releaseFilePath);
      break;
      }
    return EXIT_FAILURE;
    }
  
  std::cerr<< "true path: " << file.absoluteFilePath().toStdString() << std::endl;
  pluginFactory.registerLibrary("lib", file);
  if (pluginFactory.keys().count() != 1)
    {
    std::cerr << "ctkAbstractPluginFactory::registerLibrary() failed"
              << pluginFactory.keys().count() << std::endl;
    return EXIT_FAILURE;
    }
  if (QFileInfo(pluginFactory.path("lib")) != file)
    {
    std::cerr << "ctkAbstractPluginFactory::registerLibrary() failed"
              << pluginFactory.path("lib").toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  ctkDummyPlugin* plugin = pluginFactory.instantiate("lib");
  if (plugin == 0)
    {
    std::cerr << "ctkAbstractPluginFactory::instantiate() failed" << std::endl;
    return EXIT_FAILURE;
    }

  pluginFactory.uninstantiate("lib");
  
  return EXIT_SUCCESS;
}

