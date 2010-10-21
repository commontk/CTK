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

#ifndef __ctkAbstractQObjectFactory_h
#define __ctkAbstractQObjectFactory_h

// Qt includes
#include <QDebug>

// CTK includes
#include "ctkAbstractObjectFactory.h"

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkAbstractQObjectFactory : public ctkAbstractObjectFactory<BaseClassType>
{
public:

  /// Constructor/Desctructor
  explicit ctkAbstractQObjectFactory();
  virtual ~ctkAbstractQObjectFactory();

  /// Create an instance of the object identified by \a itemKey
  virtual BaseClassType * instantiate(const QString& itemKey);

  /// Uninstanciate the object identified by \a itemKey
  virtual void uninstantiate(const QString& itemKey);

  /// \brief Return a name allowing to uniquely identify the QObject
  /// By default, it return \a objectName obtained using staticMetaObject.className()
  virtual QString objectNameToKey(const QString& objectName);
  
  /// \brief Register an object in the factory
  /// The parameter \a key passed by reference will be updated with the
  /// associated object name obtained using ::objectNameToKey()
  template<typename ClassType>
  bool registerQObject(QString& key);

private:
  ctkAbstractQObjectFactory(const ctkAbstractQObjectFactory &);  /// Not implemented
  void operator=(const ctkAbstractQObjectFactory&); /// Not implemented
};

#include "ctkAbstractQObjectFactory.tpp"

#endif
