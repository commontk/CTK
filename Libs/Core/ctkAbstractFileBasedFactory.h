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

#ifndef __ctkAbstractFileBasedFactory_h
#define __ctkAbstractFileBasedFactory_h

// Qt includes
#include <QFileInfo>
#include <QStringList>

// CTK includes
#include "ctkAbstractFactory.h"

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkAbstractFactoryFileBasedItem : public ctkAbstractFactoryItem<BaseClassType>
{
public:
  explicit ctkAbstractFactoryFileBasedItem(const QString& path);
 
  /// Get path associated with the object identified by \a key
  QString path()const;

private:
  QString               Path;
};

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkAbstractFileBasedFactory : public ctkAbstractFactory<BaseClassType>
{
public:

  /// Constructor
  explicit ctkAbstractFileBasedFactory();
  virtual ~ctkAbstractFileBasedFactory();

  /// Get path associated with the library identified by \a key
  virtual QString path(const QString& key);

private:
  ctkAbstractFileBasedFactory(const ctkAbstractFileBasedFactory &);  /// Not implemented
  void operator=(const ctkAbstractFileBasedFactory&); /// Not implemented
};

#include "ctkAbstractFileBasedFactory.tpp"

#endif
