/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkAbstractFactory_h
#define __ctkAbstractFactory_h

// QT includes
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
  
  virtual QString loadErrorString()const;
  virtual bool load() = 0;
  BaseClassType* instantiate();
  bool instantiated();
  QString key();
  virtual void uninstantiate();
protected:
  virtual BaseClassType* instanciator() = 0;
  BaseClassType* Instance;
private:
  QString Key;
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
};

#include "ctkAbstractFactory.tpp"

#endif
