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
#include "ctkCmdLineModuleDirectoryWatcher_p.h"
#include "ctkCmdLineModuleManager.h"
#include "ctkCmdLineModuleConcurrentHelpers.h"
#include "ctkCmdLineModuleUtils.h"
#include "ctkException.h"

#include <QObject>
#include <QFileSystemWatcher>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>
#include <QtConcurrentMap>

#include <iostream>


//-----------------------------------------------------------------------------
// ctkCmdLineModuleDirectoryWatcher methods

//-----------------------------------------------------------------------------
ctkCmdLineModuleDirectoryWatcher::ctkCmdLineModuleDirectoryWatcher(ctkCmdLineModuleManager* moduleManager)
  : d(new ctkCmdLineModuleDirectoryWatcherPrivate(this, moduleManager))
{
  Q_ASSERT(moduleManager);
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleDirectoryWatcher::~ctkCmdLineModuleDirectoryWatcher()
{

}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcher::setDebug(bool debug)
{
  d->setDebug(debug);
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcher::setDirectories(const QStringList& directories)
{
  d->setDirectories(directories);
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcher::directories() const
{
  return d->directories();
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcher::setAdditionalModules(const QStringList& modules)
{
  d->setAdditionalModules(modules);
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcher::additionalModules() const
{
  return d->additionalModules();
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcher::commandLineModules() const
{
  return d->commandLineModules();
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcher::emitErrorDectectedSignal(const QString& msg)
{
  emit errorDetected(msg);
}


//-----------------------------------------------------------------------------
// ctkCmdLineModuleDirectoryWatcherPrivate methods


//-----------------------------------------------------------------------------
ctkCmdLineModuleDirectoryWatcherPrivate::ctkCmdLineModuleDirectoryWatcherPrivate(
    ctkCmdLineModuleDirectoryWatcher* d,
    ctkCmdLineModuleManager* moduleManager)
: q(d)
, ModuleManager(moduleManager)
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
void ctkCmdLineModuleDirectoryWatcherPrivate::setDebug(bool debug)
{
  this->Debug = debug;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::setDirectories(const QStringList& directories)
{
  QStringList validDirectories = this->filterInvalidDirectories(directories);
  this->setModules(validDirectories);
  this->updateWatchedPaths(validDirectories, this->MapFileNameToReferenceResult.keys());
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::directories() const
{
  return this->FileSystemWatcher->directories();
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::commandLineModules() const
{
  // So, the commandLineModules() method returns all files registered with
  // QFileSystemWatcher, which means we must filter out any invalid ones before
  // asking QFileSystemWatcher to watch them.
  return this->FileSystemWatcher->files();
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::additionalModules() const
{
  // So, in comparison to commandLineModules(), we store the list of
  // modules that are watched in addition to the directories.
  return this->AdditionalModules;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::setAdditionalModules(const QStringList& executables)
{
  QStringList filteredFileNames = this->filterFilesNotInCurrentDirectories(executables);
  QStringList filteredAdditionalModules = this->filterFilesNotInCurrentDirectories(this->AdditionalModules);

  this->unloadModules(filteredAdditionalModules);
  QList<ctkCmdLineModuleReferenceResult> refs = this->loadModules(filteredFileNames);

  QStringList validFileNames;

  for (int i = 0; i < refs.size(); ++i)
  {
    if (refs[i].m_Reference)
    {
      validFileNames << refs[i].m_Reference.location().toLocalFile();
    }
  }

  this->AdditionalModules = validFileNames;
  this->updateWatchedPaths(this->directories(), this->MapFileNameToReferenceResult.keys());

  if (this->Debug) qDebug() << "ctkCmdLineModuleDirectoryWatcherPrivate::setAdditionalModules watching:" << this->AdditionalModules;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::updateWatchedPaths(const QStringList& directories, const QStringList& files)
{
  // This method is the main interface to QFileSystemWatcher. The input parameters
  // directories, and files are quite simply what is being watched. So all directories
  // and all files must be valid examples of things to watch.

  QStringList currentDirectories = this->directories();
  QStringList currentCommandLineModules = this->commandLineModules();

  if (currentDirectories.size() > 0)
  {
    this->FileSystemWatcher->removePaths(currentDirectories);
  }
  if (currentCommandLineModules.size() > 0)
  {
    this->FileSystemWatcher->removePaths(currentCommandLineModules);
  }

  if (directories.size() > 0)
  {
    this->FileSystemWatcher->addPaths(directories);
  }
  if (files.size() > 0)
  {
    this->FileSystemWatcher->addPaths(files);
  }

  if (this->Debug)
  {
    qDebug() << "ctkCmdLineModuleDirectoryWatcherPrivate::updateWatchedPaths watching directories:\n" << directories << "\n and files:\n" << files;
  }
}

//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::filterInvalidDirectories(const QStringList& directories) const
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
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::filterFilesNotInCurrentDirectories(const QStringList& filenames) const
{
  QStringList currentDirectories = this->directories();
  QStringList filteredFileNames;

  for (int i = 0; i < filenames.size(); i++)
  {
    QFileInfo fileInfo(filenames[i]);

    if (fileInfo.exists() && !(currentDirectories.contains(fileInfo.absolutePath())))
    {
      filteredFileNames << fileInfo.absoluteFilePath();
    }
  }
  return filteredFileNames;
}


//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::getExecutablesInDirectory(const QString& path) const
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
QStringList ctkCmdLineModuleDirectoryWatcherPrivate::extractCurrentlyWatchedFilenamesInDirectory(const QString& path) const
{
  QStringList result;

  QDir dir(path);
  if (dir.exists())
  {
    QList<QString> keys = this->MapFileNameToReferenceResult.keys();

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
void ctkCmdLineModuleDirectoryWatcherPrivate::setModules(const QStringList &directories)
{
  // Note: This method, is called from setDirectories and updateModules,
  // so the input directories list may be longer or shorter than the currently watched directories.
  // In addition, within those directories, programs may have been added/removed.

  QString path;
  QStringList currentlyWatchedDirectories = this->directories();

  QStringList modulesToUnload;
  QStringList modulesToLoad;

  // First remove modules from current directories that are no longer in the requested "directories" list.
  foreach (path, currentlyWatchedDirectories)
  {
    if (!directories.contains(path))
    {
      QStringList currentlyWatchedFiles = this->extractCurrentlyWatchedFilenamesInDirectory(path);

      QString filename;
      foreach (filename, currentlyWatchedFiles)
      {
        modulesToUnload << filename;
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
          modulesToUnload << executable;
        }
      }

      foreach(executable, executablesInDirectory)
      {
        if (!currentlyWatchedFiles.contains(executable))
        {
          modulesToLoad << executable;
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
        modulesToLoad << executable;
      }
    }
  }

  this->unloadModules(modulesToUnload);
  this->loadModules(modulesToLoad);
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::updateModules(const QString &directory)
{
  // Note: If updateModules is only called from onDirectoryChanged which is only called
  // when an EXISTING directory is updated, then this if clause should never be true.

  QStringList currentlyWatchedDirectories = this->directories();
  if (!currentlyWatchedDirectories.contains(directory))
  {
    currentlyWatchedDirectories << directory;
  }
  this->setModules(currentlyWatchedDirectories);
  this->updateWatchedPaths(currentlyWatchedDirectories, this->MapFileNameToReferenceResult.keys());
}


//-----------------------------------------------------------------------------
QList<ctkCmdLineModuleReferenceResult> ctkCmdLineModuleDirectoryWatcherPrivate::loadModules(const QStringList& executables)
{
  QList<ctkCmdLineModuleReferenceResult> refResults = QtConcurrent::blockingMapped(executables,
                                                                                   ctkCmdLineModuleConcurrentRegister(this->ModuleManager, this->Debug));

  for (int i = 0; i < executables.size(); ++i)
  {
    if (refResults[i].m_Reference)
    {
      this->MapFileNameToReferenceResult[executables[i]] = refResults[i];
    }
  }

  // Broadcast error messages.
  QString errorMessages = ctkCmdLineModuleUtils::errorMessagesFromModuleRegistration(refResults, this->ModuleManager->validationMode());
  q->emitErrorDectectedSignal(errorMessages);

  return refResults;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::unloadModules(const QStringList& executables)
{
  QtConcurrent::blockingMapped(executables, ctkCmdLineModuleConcurrentUnRegister(this->ModuleManager));
  foreach(QString executable, executables)
  {
    this->MapFileNameToReferenceResult.remove(executable);
  }
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleDirectoryWatcherPrivate::onFileChanged(const QString& path)
{
  ctkCmdLineModuleReferenceResult refResult = this->loadModules(QStringList() << path).front();
  if (refResult.m_Reference)
  {
    if (this->Debug) qDebug() << "Reloaded " << path;
  }
  else
  {
    if (this->Debug) qDebug() << "ctkCmdLineModuleDirectoryWatcherPrivate::onFileChanged(" << path << "): failed to load module due to " << refResult.m_RuntimeError;
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
    updateModules(path);

    if (this->Debug) qDebug() << "Reloaded modules in" << path;
  }
  else
  {
    if (this->Debug) qDebug() << "ctkCmdLineModuleDirectoryWatcherPrivate::onDirectoryChanged(" << path << "): failed to load modules, as path invalid.";
  }
}


