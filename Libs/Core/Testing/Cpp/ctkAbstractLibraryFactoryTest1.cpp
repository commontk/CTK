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
#include "ctkAbstractLibraryFactory.h"

// STD includes
#include <cstdlib>
#include <iostream>

int ObjectConstructed = 0; 

//-----------------------------------------------------------------------------
class Object
{
public:
  Object()
  {
    ++ObjectConstructed;
  }
  ~Object()
  {
    --ObjectConstructed;
  }
};

//-----------------------------------------------------------------------------
class ObjectFactoryItem : public ctkFactoryLibraryItem<Object>
{
public:
  ObjectFactoryItem(const QString& path)
    :ctkFactoryLibraryItem<Object>(path)
  {
  }
protected:
  virtual Object* instanciator(){return new Object;}
};

//-----------------------------------------------------------------------------
class ObjectFactory : public ctkAbstractLibraryFactory<Object>
{
public:
  virtual void registerItems()
  {
    qDebug() << "Registering items";
  }
  
protected:
  virtual ctkFactoryLibraryItem<Object>* createFactoryLibraryItem(const QFileInfo& file)const
  {
    return new ObjectFactoryItem(file.filePath());
  }
};

//-----------------------------------------------------------------------------
int ctkAbstractLibraryFactoryTest1(int argc, char * argv [])
{
  QApplication app(argc, argv);

  ObjectFactory factory;
  factory.registerItems();

  return EXIT_SUCCESS;
}

