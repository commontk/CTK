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

#ifndef __ctkAbstractFileBasedFactory_tpp
#define __ctkAbstractFileBasedFactory_tpp

// Qt includes
#include <QDirIterator>

// CTK includes
#include "ctkAbstractFileBasedFactory.h"

//----------------------------------------------------------------------------
// ctkFactoryFileBasedItem methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryFileBasedItem<BaseClassType>::setPath(const QString& path)
{
  this->Path = path;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkAbstractFactoryFileBasedItem<BaseClassType>::path()const
{
  return this->Path;
}

//----------------------------------------------------------------------------
// ctkAbstractFileBasedFactory methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkAbstractFileBasedFactory<BaseClassType>::path(const QString& key)
{
  ctkAbstractFactoryFileBasedItem<BaseClassType>* _item =
      dynamic_cast<ctkAbstractFactoryFileBasedItem<BaseClassType>*>(this->item(key));
  Q_ASSERT(_item);
  return _item->path();
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFileBasedFactory<BaseClassType>::registerAllFileItems(const QStringList& directories)
{
  // Process one path at a time
  foreach (QString path, directories)
    {
    QDirIterator it(path);
    while (it.hasNext())
      {
      it.next();
      QFileInfo fileInfo = it.fileInfo();
      if (fileInfo.isSymLink())
        {
        // symLinkTarget() handles links pointing to symlinks.
        // How about a symlink pointing to a symlink ?
        fileInfo = QFileInfo(fileInfo.symLinkTarget());
        }
      // Skip if item isn't a file
      if (!this->isValidFile(fileInfo))
        {
        continue;
        }

      if (this->verbose())
        {
        qDebug() << "Attempt to register command line module:" << fileInfo.fileName();
        }

      this->registerFileItem(fileInfo);
      }
    }
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFileBasedFactory<BaseClassType>
::registerFileItem(const QFileInfo& fileInfo)
{
  QString key = this->fileNameToKey(fileInfo.filePath());
  return this->registerFileItem(key, fileInfo);
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFileBasedFactory<BaseClassType>
::registerFileItem(const QString& key, const QFileInfo& fileInfo)
{
  if (this->item(key))
    {
    return false;
    }
  QSharedPointer<ctkAbstractFactoryItem<BaseClassType> >
    itemToRegister(this->createFactoryFileBasedItem());
  if (itemToRegister.isNull())
    {
    return false;
    }
  dynamic_cast<ctkAbstractFactoryFileBasedItem<BaseClassType>*>(itemToRegister.data())
    ->setPath(fileInfo.filePath());
  this->initItem(itemToRegister.data());
  bool res = this->registerItem(key, itemToRegister);
  if (!res && this->verbose())
    {
    qWarning() << "Failed to register module: " << key;
    }
  return res;
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFileBasedFactory<BaseClassType>
::isValidFile(const QFileInfo& file)const
{
  return file.isFile();
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType>* ctkAbstractFileBasedFactory<BaseClassType>
::createFactoryFileBasedItem()
{
  return 0;
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFileBasedFactory<BaseClassType>::
initItem(ctkAbstractFactoryItem<BaseClassType>* item)
{
  item->setVerbose(this->verbose());
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkAbstractFileBasedFactory<BaseClassType>
::fileNameToKey(const QString& fileName)const
{
  return QFileInfo(fileName).baseName().toLower();
}

#endif
