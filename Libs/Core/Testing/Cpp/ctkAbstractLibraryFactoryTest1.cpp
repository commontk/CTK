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

// CTK includes
#include "ctkAbstractLibraryFactory.h"
#include "ctkDummyPlugin.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
class ctkDummyLibrary
{
};

//-----------------------------------------------------------------------------
class ctkDummyLibraryItem: public ctkFactoryLibraryItem<ctkDummyLibrary>
{
protected:
  virtual ctkDummyLibrary* instanciator()
  {
    // Using a scoped pointer ensures the memory will be cleaned if instanciator
    // fails before returning the module. See QScopedPointer::take()
    QScopedPointer<ctkDummyLibrary> module(new ctkDummyLibrary());
    foreach(QString symbol, this->Symbols)
      {
      void* res = this->symbolAddress(symbol);
      if (!res)
        {
        }
      }
    return module.take();
  }
};

//-----------------------------------------------------------------------------
class ctkDummyLibraryFactoryItem: public ctkAbstractLibraryFactory<ctkDummyLibrary>
{
protected:
  //-----------------------------------------------------------------------------
  ctkAbstractFactoryItem<ctkDummyLibrary>* createFactoryFileBasedItem()
  {
    return new ctkDummyLibraryItem();
  }
};

//-----------------------------------------------------------------------------
int ctkAbstractLibraryFactoryTest1(int argc, char * argv [])
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
  ctkDummyLibraryFactoryItem libraryFactory;
  libraryFactory.setVerbose(true);

  bool res = libraryFactory.registerFileItem("fail", QFileInfo("foo/bar.txt"));
  if (res)
    {
    std::cerr << "ctkAbstractLibraryFactory::registerLibrary() registered bad file"
              << std::endl;
    return EXIT_FAILURE;
    }

  res = libraryFactory.registerFileItem("lib", file);
  if (!res || libraryFactory.itemKeys().count() != 1)
    {
    std::cerr << "ctkAbstractLibraryFactory::registerLibrary() failed"
              << libraryFactory.itemKeys().count() << std::endl;
    return EXIT_FAILURE;
    }
  // register twice must return false
  res = libraryFactory.registerFileItem("lib", file);
  if (res || libraryFactory.itemKeys().count() != 1)
    {
    std::cerr << "ctkAbstractLibraryFactory::registerLibrary() failed"
              << libraryFactory.itemKeys().count() << std::endl;
    return EXIT_FAILURE;
    }
  if (QFileInfo(libraryFactory.path("lib")) != file)
    {
    std::cerr << "ctkAbstractLibraryFactory::registerLibrary() failed"
              << libraryFactory.path("lib").toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  ctkDummyLibrary* library = libraryFactory.instantiate("lib");
  if (library == 0)
    {
    std::cerr << "ctkAbstractLibraryFactory::instantiate() failed" << std::endl;
    return EXIT_FAILURE;
    }

  libraryFactory.uninstantiate("lib");
  return EXIT_SUCCESS;
}

