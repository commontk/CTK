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
/// ctkAbstractFactoryItem is the base class of factory items. They are
/// uniquely defined by a key and are responsible for creating/holding an
/// instance of a BaseClassType object.
template<typename BaseClassType>
class ctkAbstractFactoryItem
{
public:
  explicit ctkAbstractFactoryItem();
  
  virtual QString loadErrorString()const;
  virtual bool load() = 0;
  
  BaseClassType* instantiate();
  bool instantiated()const;
  virtual void uninstantiate();
  
  void setVerbose(bool value);
  bool verbose()const;

protected:
  /// Must be reimplemented in subclasses to instanciate a BaseClassType*
  virtual BaseClassType* instanciator() = 0;
  BaseClassType* Instance;

private:
  bool Verbose;
};

//----------------------------------------------------------------------------
/// \brief ctkAbstractFactory is the base class of all the factory where items need
/// to be registered before being instantiated.
/// \paragraph ctkAbstractFactory contains a collection of ctkAbstractFactoryItems that
/// are uniquely identifyed by a key. Subclasses of ctkAbstractFactory are
/// responsible for populating the list of ctkAbstractFactoryItems.
/// BaseClassType could be any type (most probably a QObject) 
template<typename BaseClassType>
class ctkAbstractFactory
{
public:

  /// Constructor/Desctructor
  explicit ctkAbstractFactory();
  virtual ~ctkAbstractFactory();
  virtual void printAdditionalInfo();

  /// \brief Create an instance of the object.
  /// The item corresponding to the key should have been registered before.
  virtual BaseClassType * instantiate(const QString& itemKey);

  /// \brief Uninstanciate the object.
  /// Do nothing if the item given by the key has not be instantiated nor registered.
  void uninstantiate(const QString& itemKey);

  /// \brief Get path associated with the item identified by \a itemKey
  /// Should be overloaded in subclasse
  virtual QString path(const QString& itemKey){ Q_UNUSED(itemKey); return QString(); }

  /// Get list of all registered item keys.
  QStringList keys() const;

  /// \brief Register items with the factory
  /// Method provided for convenience - Should be overloaded in subclasse
  virtual void registerItems(){}

  /// \brief Enabled verbose output
  /// Warning and error message will be printed to standard outputs
  void setVerbose(bool value);
  bool verbose()const;

protected:

  /// \brief Call the load method associated with the item.
  /// If succesfully loaded, add it to the internal map.
  bool registerItem(const QString& key, const QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > & item);

  /// Get a Factory item given its itemKey. Return 0 if any.
  ctkAbstractFactoryItem<BaseClassType> * item(const QString& itemKey)const;

  typedef typename QHash<QString, QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > >::const_iterator ConstIterator;
  typedef typename QHash<QString, QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > >::iterator       Iterator;

private:
  ctkAbstractFactory(const ctkAbstractFactory &); /// Not implemented
  void operator=(const ctkAbstractFactory&); /// Not implemented

  QHash<QString, QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > > RegisteredItemMap;

  bool Verbose;
};

#include "ctkAbstractFactory.tpp"

#endif
