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
/// \ingroup Core
template<typename BaseClassType>
class ctkAbstractFactoryFileBasedItem
  : public ctkAbstractFactoryItem<BaseClassType>
{
public:
  /// Get path associated with the object identified by \a key
  void setPath(const QString& path);
  QString path()const;

private:
  QString               Path;
};

//----------------------------------------------------------------------------
/// \ingroup Core
template<typename BaseClassType>
class ctkAbstractFileBasedFactory : public ctkAbstractFactory<BaseClassType>
{
public:
  /// Get path associated with the library identified by \a key
  virtual QString path(const QString& key);

  bool registerFileItem(const QFileInfo& file);
  bool registerFileItem(const QString& key, const QFileInfo& file);

protected:
  void registerAllFileItems(const QStringList& directories);

  virtual bool isValidFile(const QFileInfo& file)const;
  virtual ctkAbstractFactoryItem<BaseClassType>* createFactoryFileBasedItem();
  virtual void initItem(ctkAbstractFactoryItem<BaseClassType>* item);

  virtual QString fileNameToKey(const QString& objectName)const;
};

#include "ctkAbstractFileBasedFactory.tpp"

#endif
