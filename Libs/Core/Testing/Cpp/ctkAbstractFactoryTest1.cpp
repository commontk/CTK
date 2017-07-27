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
#include "ctkAbstractFactory.h"
#include "ctkModelTester.h"

// STD includes
#include <cstdlib>
#include <iostream>

template<class BaseClassType>
class FactoryItem : public ctkAbstractFactoryItem<BaseClassType>
{
public:
  virtual bool load()
    {
    return true;
    }
protected:
  /// Must be reimplemented in subclasses to instanciate a BaseClassType*
  virtual BaseClassType* instanciator()
    {
    return new BaseClassType;
    }
};

template<class BaseClassType>
class Factory: public ctkAbstractFactory<BaseClassType>
{
public:
  virtual void registerItems()
  {
    this->registerItem("item1", QSharedPointer<ctkAbstractFactoryItem<BaseClassType> >(new FactoryItem<BaseClassType>()));
    this->registerItem("item2", QSharedPointer<ctkAbstractFactoryItem<BaseClassType> >(new FactoryItem<BaseClassType>()));
  }
};

struct Item{
};

//-----------------------------------------------------------------------------
int ctkAbstractFactoryTest1(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkAbstractFactory<Item> abstractFactory;
  abstractFactory.printAdditionalInfo();
  
  Item* item = abstractFactory.instantiate("unregistered item");
  if (item != 0)
    {
    std::cerr << "ctkAbstractFactory::instantiate() failed" << std::endl;
    return EXIT_FAILURE;
    }
  abstractFactory.uninstantiate("uninstanced item");
  if (!abstractFactory.path("wrong key").isEmpty())
    {
    std::cerr<< "ctkAbstractFactory::path() failed" << std::endl;
    return EXIT_FAILURE;
    }
  abstractFactory.registerItems();
  if (abstractFactory.itemKeys().count() != 0)
    {
    std::cerr<< "ctkAbstractFactory::keys() failed" << std::endl;
    return EXIT_FAILURE;
    }
  abstractFactory.setVerbose(true);
  if (abstractFactory.verbose() != true)
    {
    std::cerr<< "ctkAbstractFactory::setVerbose() failed" << std::endl;
    return EXIT_FAILURE;
    }

  Factory<Item> factory;
  factory.printAdditionalInfo();
  
  Item* item1 = factory.instantiate("item1");
  if (item != 0)
    {
    std::cerr << "ctkAbstractFactory::instantiate() failed" << std::endl;
    return EXIT_FAILURE;
    }
  factory.uninstantiate("item1");
  if (!factory.path("wrong key").isEmpty())
    {
    std::cerr<< "ctkAbstractFactory::path() failed" << std::endl;
    return EXIT_FAILURE;
    }
  factory.registerItems();
  if (factory.itemKeys().count() != 2 ||
      !factory.itemKeys().contains("item1") ||
      !factory.itemKeys().contains("item2"))
    {
    std::cerr<< "ctkAbstractFactory::itemKeys() failed" << std::endl;
    return EXIT_FAILURE;
    }
  item1 = factory.instantiate("item1");
  if (item1 == 0)
    {
    std::cerr << "ctkAbstractFactory::instantiate() failed" << std::endl;
    return EXIT_FAILURE;
    }
  factory.uninstantiate("item1");

  return EXIT_SUCCESS;
}

