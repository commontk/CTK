/*=============================================================================

  Library: CTK

  Copyright (c) University College London

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkCmdLineModuleDefaultPathBuilder.h"
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <cstdlib>
#include <QStringList>

//----------------------------------------------------------------------------
struct ctkCmdLineModuleDefaultPathBuilderPrivate
{
public:
  ctkCmdLineModuleDefaultPathBuilderPrivate();
  ~ctkCmdLineModuleDefaultPathBuilderPrivate();
  void clear();
  void setStrictMode(const bool& strict);
  bool strictMode() const;
  void addHomeDir(const QString& subFolder = QString());
  void addCurrentDir(const QString& subFolder = QString());
  void addApplicationDir(const QString& subFolder = QString());
  void addCtkModuleLoadPathDir(const QString& subFolder = QString());
  QStringList getDirectoryList() const;

  bool isStrictMode;
  QStringList directoryList;

private:

  QString addSubFolder(const QString& folder, const QString& subFolder);
};

//-----------------------------------------------------------------------------
// ctkCmdLineModuleDefaultPathBuilderPrivate methods

//-----------------------------------------------------------------------------
ctkCmdLineModuleDefaultPathBuilderPrivate::ctkCmdLineModuleDefaultPathBuilderPrivate()
: isStrictMode(false)
{
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleDefaultPathBuilderPrivate::~ctkCmdLineModuleDefaultPathBuilderPrivate()
{
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilderPrivate::clear()
{
  directoryList.clear();
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilderPrivate::setStrictMode(const bool& strict)
{
  isStrictMode = strict;
}


//-----------------------------------------------------------------------------
bool ctkCmdLineModuleDefaultPathBuilderPrivate::strictMode() const
{
  return isStrictMode;
}


//-----------------------------------------------------------------------------
QString ctkCmdLineModuleDefaultPathBuilderPrivate::addSubFolder(
    const QString& folder, const QString& subFolder)
{
  if (subFolder.length() > 0)
  {
    return folder + QDir::separator() + subFolder;
  }
  else
  {
    return folder;
  }
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilderPrivate::addHomeDir(const QString& subFolder)
{
  if (QDir::home().exists())
  {
    QString result = addSubFolder(QDir::homePath(), subFolder);
    directoryList << result;
  }
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilderPrivate::addCurrentDir(const QString& subFolder)
{
  if (QDir::current().exists())
  {
    QString result = addSubFolder(QDir::currentPath(), subFolder);
    directoryList << result;
  }
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilderPrivate::addApplicationDir(const QString& subFolder)
{
  QString result = addSubFolder(QCoreApplication::applicationDirPath(), subFolder);
  directoryList << result;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilderPrivate::addCtkModuleLoadPathDir(
    const QString& subFolder)
{
  char *ctkModuleLoadPath = getenv("CTK_MODULE_LOAD_PATH");
  if (ctkModuleLoadPath != NULL)
  {
    QDir dir = QDir(QString(ctkModuleLoadPath));
    QString result = addSubFolder(dir.canonicalPath(), subFolder);
    directoryList << result;
  }
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDefaultPathBuilderPrivate::getDirectoryList() const
{
  if (!isStrictMode)
  {
    return directoryList;
  }
  else
  {
    QStringList filteredList;
    foreach (QString directory, directoryList)
    {
      QDir dir(directory);
      if (dir.exists())
      {
        filteredList << directory;
      }
    }

    qDebug() << "Filtered directory list " << directoryList << " to " << filteredList;
    return filteredList;
  }
}



//-----------------------------------------------------------------------------
// ctkCmdLineModuleDefaultPathBuilder methods

//-----------------------------------------------------------------------------
ctkCmdLineModuleDefaultPathBuilder::ctkCmdLineModuleDefaultPathBuilder()
  : d(new ctkCmdLineModuleDefaultPathBuilderPrivate)
{
}

//-----------------------------------------------------------------------------
ctkCmdLineModuleDefaultPathBuilder::~ctkCmdLineModuleDefaultPathBuilder()
{
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilder::clear()
{
  d->clear();
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilder::setStrictMode(const bool& strict)
{
  d->setStrictMode(strict);
}


//-----------------------------------------------------------------------------
bool ctkCmdLineModuleDefaultPathBuilder::strictMode() const
{
  return d->strictMode();
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilder::addHomeDir(const QString& subFolder)
{
  d->addHomeDir(subFolder);
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilder::addCurrentDir(const QString& subFolder)
{
  d->addCurrentDir(subFolder);
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilder::addApplicationDir(const QString& subFolder)
{
  d->addApplicationDir(subFolder);
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDefaultPathBuilder::addCtkModuleLoadPathDir(
    const QString& subFolder)
{
  d->addCtkModuleLoadPathDir(subFolder);
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDefaultPathBuilder::getDirectoryList() const
{
  return d->getDirectoryList();
}
