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
ctkAbstractFactoryFileBasedItem<BaseClassType>::ctkAbstractFactoryFileBasedItem(const QString& _path)
  :ctkAbstractFactoryItem<BaseClassType>()
  ,Path(_path)
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkAbstractFactoryFileBasedItem<BaseClassType>::path()const
{
  return this->Path;
}

//----------------------------------------------------------------------------
// ctkAbstractFileBasedFactory methods

//-----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFileBasedFactory<BaseClassType>::ctkAbstractFileBasedFactory()
  :ctkAbstractFactory<BaseClassType>()
{
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFileBasedFactory<BaseClassType>::~ctkAbstractFileBasedFactory()
{
}

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
      if (!fileInfo.isFile())
        {
        continue;
        }

      if (this->verbose())
        {
        qDebug() << "Attempt to register command line module:" << fileInfo.fileName();
        }

      QString key = this->fileNameToKey(fileInfo.filePath());
      QSharedPointer<ctkAbstractFactoryItem<BaseClassType> >
        itemToRegister = QSharedPointer<ctkAbstractFactoryItem<BaseClassType> >(
          this->createFactoryFileBasedItem(fileInfo));
      if (itemToRegister.isNull())
        {
        continue;
        }
      itemToRegister->setVerbose(this->verbose());
      if (!this->registerItem(key, itemToRegister))
        {
        if (this->verbose())
          {
          qWarning() << "Failed to register module: " << key;
          }
        continue;
        }
      }
    }
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType>* ctkAbstractFileBasedFactory<BaseClassType>
::createFactoryFileBasedItem(const QFileInfo& file)
{
  Q_UNUSED(file);
  return 0;
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkAbstractFileBasedFactory<BaseClassType>
::fileNameToKey(const QString& fileName)const
{
  return QFileInfo(fileName).baseName().toLower();
}

#endif
