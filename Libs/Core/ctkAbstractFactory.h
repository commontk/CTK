/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

#ifndef __ctkAbstractFactory_h
#define __ctkAbstractFactory_h

// Qt includes
#include <QString>
#include <QHash>
#include <QSharedPointer>
#include <QStringList>

#ifdef _MSC_VER
  /// 4505: 'ctkAbstractFactoryItem<BaseClassType>::loadErrorString' :
  ///       unreferenced local function has been removed
# pragma warning(disable: 4505)
#endif

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkAbstractFactoryItem
{
public:
  explicit ctkAbstractFactoryItem(const QString& key);
  virtual ~ctkAbstractFactoryItem(){}
  
  virtual QString loadErrorString()const;
  virtual bool load() = 0;
  BaseClassType* instantiate();
  bool instantiated();
  QString key();
  virtual void uninstantiate();
  void setVerbose(bool value);
  bool verbose();
protected:
  virtual BaseClassType* instanciator() = 0;
  BaseClassType* Instance;
private:
  QString Key;
  bool Verbose;
};

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkAbstractFactory
{
protected:
  typedef typename QHash<QString, QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > >::const_iterator ConstIterator;
  typedef typename QHash<QString, QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > >::iterator       Iterator;

public:
  /// 
  /// Constructor/Desctructor
  explicit ctkAbstractFactory();
  virtual ~ctkAbstractFactory();
  virtual void printAdditionalInfo();

  /// 
  /// Create an instance of the object
  virtual BaseClassType * instantiate(const QString& itemKey);

  /// 
  /// Uninstanciate the object
  void uninstantiate(const QString& itemKey);

  /// 
  /// Get list of all registered item names
  QStringList names() const;

  /// 
  /// Register items with the factory
  /// Method provided for convenience - Should be overloaded in subclasse
  virtual void registerItems(){}

  /// Enabled verbose output
  /// Warning and error message will be printed to standard outputs
  void setVerbose(bool value);
  bool verbose();

protected:

  /// 
  /// Call the load method associated with the item.
  /// If succesfully loaded, add it to the internal map.
  bool registerItem(const QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > & item);

  /// 
  /// Get a Factory item given its itemKey. Return 0 if any.
  ctkAbstractFactoryItem<BaseClassType> * item(const QString& itemKey)const;

private:
  ctkAbstractFactory(const ctkAbstractFactory &); /// Not implemented
  void operator=(const ctkAbstractFactory&); /// Not implemented

  QHash<QString, QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > > RegisteredItemMap;

  bool Verbose;
};

#include "ctkAbstractFactory.tpp"

#endif
