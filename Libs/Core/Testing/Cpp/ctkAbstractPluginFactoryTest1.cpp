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
#include <QTimer>

// CTK includes
#include "ctkAbstractPluginFactory.h"
#include "ctkDummyPlugin.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkAbstractPluginFactoryTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);

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
  ctkAbstractPluginFactory< ctkDummyPlugin > pluginFactory;
  pluginFactory.setVerbose(true);

  QString itemKey = pluginFactory.registerFileItem(QFileInfo("foo/bar.txt"));
  if (!itemKey.isEmpty())
    {
    std::cerr << "ctkAbstractPluginFactory::registerLibrary() registered bad file"
              << std::endl;
    return EXIT_FAILURE;
    }

  itemKey = pluginFactory.registerFileItem(file);
  if (itemKey.isEmpty() || pluginFactory.itemKeys().count() != 1)
    {
    std::cerr << __LINE__ << ": ctkAbstractPluginFactory::registerLibrary() failed: "
              << pluginFactory.itemKeys().count() << std::endl;
    return EXIT_FAILURE;
    }
  // register twice must return false
  itemKey = pluginFactory.registerFileItem(file);
  if (itemKey.isEmpty() || pluginFactory.itemKeys().count() != 1)
    {
    std::cerr << __LINE__ << ": ctkAbstractPluginFactory::registerLibrary() failed: "
              << qPrintable(itemKey) << " count: "
              << pluginFactory.itemKeys().count() << std::endl;
    return EXIT_FAILURE;
    }
  if (QFileInfo(pluginFactory.path(itemKey)) != file)
    {
    std::cerr << __LINE__ << ": ctkAbstractPluginFactory::registerLibrary() failed: "
              << pluginFactory.path(itemKey).toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  ctkDummyPlugin* plugin = pluginFactory.instantiate(itemKey);
  if (plugin == 0)
    {
    std::cerr << __LINE__ << ": ctkAbstractPluginFactory::instantiate() failed" << std::endl;
    return EXIT_FAILURE;
    }

  pluginFactory.uninstantiate(itemKey);

  // ctkDummyPlugin is not a QPushButton, it should fail then.
  ctkAbstractPluginFactory< QTimer > buttonPluginFactory;
  buttonPluginFactory.setVerbose(true);
  // it should register but fail while instanciating
  itemKey = buttonPluginFactory.registerFileItem(file);
  if (itemKey.isEmpty() || buttonPluginFactory.itemKeys().count() != 1)
    {
    std::cerr << __LINE__ << ": ctkAbstractPluginFactory::registerLibrary() failed" << std::endl;
    return EXIT_FAILURE;
    }
  QTimer* timer = buttonPluginFactory.instantiate("foo");
  if (timer != 0)
    {
    std::cerr << __LINE__ << ": ctkAbstractPluginFactory::instantiate() failed" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

