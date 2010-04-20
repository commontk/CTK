/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkAbstractLibraryFactory_h
#define __ctkAbstractLibraryFactory_h

// QT includes
#include <QFileInfo>
#include <QLibrary>
#include <QStringList>

// CTK includes
#include "ctkAbstractFactory.h"

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkFactoryLibraryItem : public ctkAbstractFactoryItem<BaseClassType>
{
protected:
  typedef typename QHash<QString, void*>::const_iterator ConstIterator;
  typedef typename QHash<QString, void*>::iterator       Iterator;

public:
  explicit ctkFactoryLibraryItem(const QString& key, const QString& path);
  virtual bool load();
  QString path()const;
  virtual QString loadErrorString()const;

  void setSymbols(const QStringList& symbols);

  //-----------------------------------------------------------------------------
  /// 
  /// Resolve symbols
  void resolve();
  
  //-----------------------------------------------------------------------------
  /// 
  /// Get symbol address
  void* symbolAddress(const QString& symbol)const;

private:
  QLibrary              Library;
  QString               Path;
  QHash<QString, void*> ResolvedSymbols;
  QStringList           Symbols;
};

//----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
class ctkAbstractLibraryFactory : public ctkAbstractFactory<BaseClassType>
{
public:
  //-----------------------------------------------------------------------------
  /// 
  /// Constructor
  explicit ctkAbstractLibraryFactory();
  virtual ~ctkAbstractLibraryFactory();

  //-----------------------------------------------------------------------------
  /// 
  /// Set the list of symbols
  void setSymbols(const QStringList& symbols);

  //-----------------------------------------------------------------------------
  /// 
  /// Register a plugin in the factory
  virtual bool registerLibrary(const QFileInfo& file, QString& key);

private:
  ctkAbstractLibraryFactory(const ctkAbstractLibraryFactory &);  /// Not implemented
  void operator=(const ctkAbstractLibraryFactory&); /// Not implemented

  QStringList Symbols;
};

#include "ctkAbstractLibraryFactory.tpp"

#endif
