/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkAbstractQObjectFactory_h
#define __ctkAbstractQObjectFactory_h

#include "ctkAbstractObjectFactory.h"

#include <QDebug>

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkAbstractQObjectFactory : public ctkAbstractObjectFactory<BaseClassType>
{
public:
  //-----------------------------------------------------------------------------
  /// 
  /// Constructor/Desctructor
  explicit ctkAbstractQObjectFactory();
  virtual ~ctkAbstractQObjectFactory();

  //----------------------------------------------------------------------------
  /// 
  /// Create an instance of the object
  virtual BaseClassType * instantiate(const QString& itemKey);

  //----------------------------------------------------------------------------
  /// 
  /// Uninstanciate the object
  virtual void uninstantiate(const QString& itemKey);

  //-----------------------------------------------------------------------------
  /// 
  /// Register an object in the factory
  /// Note: The parameter 'key' passed by reference will be updated with the associated object name
  template<typename ClassType>
  bool registerQObject(QString& key);

private:
  ctkAbstractQObjectFactory(const ctkAbstractQObjectFactory &);  /// Not implemented
  void operator=(const ctkAbstractQObjectFactory&); /// Not implemented
};

#include "ctkAbstractQObjectFactory.tpp"

#endif
