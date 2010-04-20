/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkAbstractObjectFactory_h
#define __ctkAbstractObjectFactory_h

// QT includes
#include <QDebug>

// CTK includes
#include "ctkAbstractFactory.h"

//----------------------------------------------------------------------------
namespace{
  /// 
  /// Function in charge of instanciating an object of type: ClassType
  template<typename BaseClassType, typename ClassType>
    BaseClassType *instantiateObject()
  {
    return new ClassType;
  }
}

//----------------------------------------------------------------------------
template<typename BaseClassType, typename ClassType>
class ctkFactoryObjectItem : public ctkAbstractFactoryItem<BaseClassType>
{
protected:
  typedef BaseClassType *(*InstantiateObjectFunc)();
public:
  explicit ctkFactoryObjectItem(const QString& key);
  virtual bool load();
protected:
  virtual BaseClassType* instanciator();
private:
  InstantiateObjectFunc instantiateObjectFunc;
};

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkAbstractObjectFactory : public ctkAbstractFactory<BaseClassType>
{
public:
  //-----------------------------------------------------------------------------
  /// 
  /// Constructor/Desctructor
  explicit ctkAbstractObjectFactory();
  virtual ~ctkAbstractObjectFactory();

  //-----------------------------------------------------------------------------
  /// 
  /// Register an object in the factory
  template<typename ClassType>
  bool registerObject(const QString& key);

private:
  ctkAbstractObjectFactory(const ctkAbstractObjectFactory &);  /// Not implemented
  void operator=(const ctkAbstractObjectFactory&); /// Not implemented
};

#include "ctkAbstractObjectFactory.tpp"

#endif
