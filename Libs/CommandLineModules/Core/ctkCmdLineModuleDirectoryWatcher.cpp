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

#include "ctkCmdLineModuleDirectoryWatcher.h"
#include "ctkCmdLineModuleDirectoryWatcherPrivate.h"
#include "ctkCmdLineModuleManager.h"
#include <QObject>
#include <QFileSystemWatcher>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <iostream>

//-----------------------------------------------------------------------------
// ctkCmdLineModuleDirectoryWatcher methods

//-----------------------------------------------------------------------------
ctkCmdLineModuleDirectoryWatcher::ctkCmdLineModuleDirectoryWatcher(ctkCmdLineModuleManager* moduleManager)
  : d(new ctkCmdLineModuleDirectoryWatcherPrivate(moduleManager))
{
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleDirectoryWatcher::~ctkCmdLineModuleDirectoryWatcher()
{

}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcher::setDebug(const bool& debug)
{
  d->setDebug(debug);
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcher::setDirectories(const QStringList& directories)
{
  d->setDirectories(directories);
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcher::directories()
{
  return d->directories();
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcher::files()
{
  return d->files();
}


//-----------------------------------------------------------------------------
// ctkCmdLineModuleDirectoryWatcherPrivate methods


//-----------------------------------------------------------------------------
ctkCmdLineModuleDirectoryWatcherPrivate::ctkCmdLineModuleDirectoryWatcherPrivate(ctkCmdLineModuleManager* moduleManager)
: ModuleManager(moduleManager)
, FileSystemWatcher(NULL)
, Debug(false)
{
  FileSystemWatcher = new QFileSystemWatcher();

  connect(this->FileSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));
  connect(this->FileSystemWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleDirectoryWatcherPrivate::~ctkCmdLineModuleDirectoryWatcherPrivate()
{
  delete this->FileSystemWatcher;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::setDebug(const bool& debug)
{
  this->Debug = debug;
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::directories()
{
  return this->FileSystemWatcher->directories();
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::files()
{
  return this->FileSystemWatcher->files();
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::setDirectories(const QStringList& directories)
{
  QStringList validDirectories = this->filterInvalidDirectories(directories);
  this->setModuleReferences(validDirectories);
  this->updateWatchedPaths(validDirectories, this->MapFileNameToReference.keys());
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::updateWatchedPaths(const QStringList& directories, const QStringList& files)
{
  QStringList currentDirectories = this->directories();
  QStringList currentFiles = this->files();

  if (currentDirectories.size() > 0)
  {
    this->FileSystemWatcher->removePaths(currentDirectories);
  }
  if (currentFiles.size() > 0)
  {
    this->FileSystemWatcher->removePaths(currentFiles);
  }

  if (directories.size() > 0)
  {
    this->FileSystemWatcher->addPaths(directories);
  }
  if (files.size() > 0)
  {
    this->FileSystemWatcher->addPaths(files);
  }
}

//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::filterInvalidDirectories(const QStringList& directories)
{
  QStringList result;

  QString path;
  foreach (path, directories)
  {
    if (!path.isNull() && !path.isEmpty() && !path.trimmed().isEmpty())
    {
      QDir dir = QDir(path);
      if (dir.exists())
      {
        result << dir.absolutePath();
      }
    }
  }

  return result;
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::extractCurrentlyWatchedFilenamesInDirectory(const QString& path)
{
  QStringList result;

  QDir dir(path);
  if (dir.exists())
  {
    QList<QString> keys = this->MapFileNameToReference.keys();

    QString fileName;
    foreach(fileName, keys)
    {
      QFileInfo fileInfo(fileName);
      if (fileInfo.absolutePath() == dir.absolutePath())
      {
        result << fileInfo.absoluteFilePath();
      }
    }
  }

  return result;
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::getExecutablesInDirectory(const QString& path)
{
  QStringList result;

  QString executable;
  QFileInfo executableFileInfo;

  QDir dir = QDir(path);
  if (dir.exists())
  {
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Executable);
    QFileInfoList executablesFileInfoList = dir.entryInfoList();

    foreach (executableFileInfo, executablesFileInfoList)
    {
      executable = executableFileInfo.absoluteFilePath();
      result << executable;
    }
  }

  return result;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::setModuleReferences(const QStringList &directories)
{
  // Note: This method, is called from setDirectories and updateModuleReferences,
  // so the input directories list may be longer or shorter than the currently watched directories.
  // In addition, within those directories, programs may have been added/removed.

  QString path;
  QStringList currentlyWatchedDirectories = this->directories();

  // First remove modules from current directories that are no longer in the requested "directories" list.
  foreach (path, currentlyWatchedDirectories)
  {
    if (!directories.contains(path))
    {
      QStringList currentlyWatchedFiles = this->extractCurrentlyWatchedFilenamesInDirectory(path);

      QString filename;
      foreach (filename, currentlyWatchedFiles)
      {
        this->unloadModule(filename);
      }
    }
  }

  // Now for each requested directory.
  foreach (path, directories)
  {
    // Existing folder.
    if (currentlyWatchedDirectories.contains(path))
    {
      QStringList currentlyWatchedFiles = this->extractCurrentlyWatchedFilenamesInDirectory(path);
      QStringList executablesInDirectory = this->getExecutablesInDirectory(path);

      QString executable;
      foreach (executable, currentlyWatchedFiles)
      {
        if (!executablesInDirectory.contains(executable))
        {
          this->unloadModule(executable);
        }
      }

      foreach(executable, executablesInDirectory)
      {
        if (!currentlyWatchedFiles.contains(executable))
        {
          this->loadModule(executable);
        }
      }
    }
    else
    {
      // New folder
      QStringList executables = this->getExecutablesInDirectory(path);

      QString executable;
      foreach (executable, executables)
      {
        this->loadModule(executable);
      }
    }
  }
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::updateModuleReferences(const QString &directory)
{
  // Note: If updateModuleReferences is only called from onDirectoryChanged which is only called
  // when an EXISTING directory is updated, then this if clause should never be true.

  QStringList currentlyWatchedDirectories = this->directories();
  if (!currentlyWatchedDirectories.contains(directory))
  {
    currentlyWatchedDirectories << directory;
  }
  this->setModuleReferences(currentlyWatchedDirectories);
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleReference ctkCmdLineModuleDirectoryWatcherPrivate::loadModule(const QString& pathToExecutable)
{
  ctkCmdLineModuleReference ref = this->ModuleManager->registerModule(pathToExecutable, !this->Debug);
  if (ref)
  {
    this->MapFileNameToReference[pathToExecutable] = ref;
  }
  return ref;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::unloadModule(const QString& pathToExecutable)
{
  ctkCmdLineModuleReference ref = this->ModuleManager->moduleReference(pathToExecutable);
  if (ref)
  {
    this->ModuleManager->unregisterModule(ref);
    this->MapFileNameToReference.remove(pathToExecutable);
  }
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::onFileChanged(const QString& path)
{
  ctkCmdLineModuleReference ref = this->loadModule(path);
  if (ref)
  {
    if (this->Debug) qDebug() << "Reloaded " << path;
  }
  else
  {
    if (this->Debug) qDebug() << "ctkCmdLineModuleDirectoryWatcherPrivate::onFileChanged(" << path << "): failed to load module";
  }
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::onDirectoryChanged(const QString &path)
{
  QStringList directories;
  directories << path;

  QStringList validDirectories = this->filterInvalidDirectories(directories);

  if (validDirectories.size() > 0)
  {
    updateModuleReferences(path);

    if (this->Debug) qDebug() << "Reloaded modules in" << path;
  }
  else
  {
    if (this->Debug) qDebug() << "ctkCmdLineModuleDirectoryWatcherPrivate::onDirectoryChanged(" << path << "): failed to load modules, as path invalid.";
  }
}


