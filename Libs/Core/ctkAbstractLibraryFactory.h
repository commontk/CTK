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

#ifndef __ctkAbstractLibraryFactory_h
#define __ctkAbstractLibraryFactory_h

// Qt includes
#include <QFileInfo>
#include <QLibrary>
#include <QStringList>

// CTK includes
#include "ctkAbstractFileBasedFactory.h"

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkFactoryLibraryItem : public ctkAbstractFactoryFileBasedItem<BaseClassType>
{
protected:
  typedef typename QHash<QString, void*>::const_iterator ConstIterator;
  typedef typename QHash<QString, void*>::iterator       Iterator;

public:
  explicit ctkFactoryLibraryItem(const QString& path);
 
  virtual bool load();
  virtual QString loadErrorString()const;

  ///
  /// Set list of required symbols
  void setSymbols(const QStringList& symbols);

  /// 
  /// \brief Resolve symbols
  /// \note The function will return False if it fails to resolve one
  /// of the required symbols set using ::setSymbols
  bool resolve();
  
  /// 
  /// Get symbol address
  void* symbolAddress(const QString& symbol)const;

private:
  QLibrary              Library;
  QHash<QString, void*> ResolvedSymbols;
  QStringList           Symbols;
};

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkAbstractLibraryFactory : public ctkAbstractFileBasedFactory<BaseClassType>
{
public:
  /// 
  /// Constructor
  explicit ctkAbstractLibraryFactory();
  virtual ~ctkAbstractLibraryFactory();

  /// Set the list of symbols
  void setSymbols(const QStringList& symbols);
  
  /// \brief Register a plugin in the factory
  /// The parameter \a key must be unique
  bool registerLibrary(const QString& key, const QFileInfo& file);

  /// \brief Utility function to register a QLibrary
  /// The parameter \a key must be unique
  bool registerQLibrary(const QString& key, const QFileInfo& file);

protected:
  virtual ctkFactoryLibraryItem<BaseClassType>* createFactoryLibraryItem(
    const QFileInfo& library)const;

private:
  ctkAbstractLibraryFactory(const ctkAbstractLibraryFactory &);  /// Not implemented
  void operator=(const ctkAbstractLibraryFactory&); /// Not implemented

  QStringList Symbols;
};

#include "ctkAbstractLibraryFactory.tpp"

#endif
