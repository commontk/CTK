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

#ifndef __ctkCmdLineModuleDirectoryWatcherPrivate_h
#define __ctkCmdLineModuleDirectoryWatcherPrivate_h

#include <QHash>
#include <QString>
#include <QStringList>
#include <QFileInfoList>

#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleDirectoryWatcher.h"

class QFileSystemWatcher;

/**
 * \class ctkCmdLineModuleDirectoryWatcherPrivate
 * \brief Private implementation class implementing directory scanning to load new modules into a ctkCmdLineModuleManager.
 * \ingroup CommandLineModulesCore
 * \author m.clarkson@ucl.ac.uk
 */
class ctkCmdLineModuleDirectoryWatcherPrivate : public QObject
{

  Q_OBJECT

public:

  ctkCmdLineModuleDirectoryWatcherPrivate(ctkCmdLineModuleManager* ModuleManager);
  virtual ~ctkCmdLineModuleDirectoryWatcherPrivate();

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::setDebug
   */
  void setDebug(const bool& debug);

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::setDirectories
   */
  void setDirectories(const QStringList& directories);

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::directories
   */
  QStringList directories();

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::files
   */
  QStringList files();


public Q_SLOTS:

  /**
   * \brief We connect QFileSystemWatcher to here.
   */
  void onFileChanged(const QString& path);

  /**
   * \brief We connect QFileSystemWatcher to here.
   */
  void onDirectoryChanged(const QString &path);

private:

  /**
   * \brief Used to update the QFileSystemWatcher with the right list of directories and files to watch.
   * \param directories list of absolute directory paths
   * \param files list of absolute file paths
   */
  void updateWatchedPaths(const QStringList& directories, const QStringList& files);

  /**
   * \brief Takes a list of directories, and only returns ones that are valid,
   * meaning that the directory name is non-null, non-empty, and the directory exists.
   * \param directories a list of directories, relative or absolute.
   * \return a list of directories, denoted by their absolute path.
   */
  QStringList filterInvalidDirectories(const QStringList& directories);

  /**
   * \brief Uses the MapFileNameToReference to work out a list of valid command line modules in a given directory.
   * \param directory the absolute or relative path of a directory.
   * \return a list of executables, denoted by their absolute path.
   */
  QStringList extractCurrentlyWatchedFilenamesInDirectory(const QString& directory);

  /**
   * \brief Returns a list of executable files (not necessarily valid command line clients) in a directory.
   * \param directory A directory
   * \return QStringList a list of absolute path names to executable files
   */
  QStringList getExecutablesInDirectory(const QString& directory);

  /**
   * \brief Main method to update the current list of watched directories and files.
   * \param directories a list of directories, denoted by their absolute path.
   */
  void setModuleReferences(const QStringList &directories);

  /**
   * \brief Called from the onDirectoryChanged slot to update the current list by calling back to setModuleReferences.
   * \param directory denoted by its absolute path.
   */
  void updateModuleReferences(const QString &directory);

  /**
   * \brief Uses the ctkCmdLineModuleManager to try and add the executable to the list
   * of executables, and if successful it is added to this->MapFileNameToReference.
   * \param pathToExecutable path to an executable file, denoted by its absolute path.
   */
  ctkCmdLineModuleReference loadModule(const QString& pathToExecutable);

  /**
   * \brief Removes the executable from both the ctkCmdLineModuleManager and this->MapFileNameToReference.
   * \param pathToExecutable path to an executable file, denoted by its absolute path.
   */
  void unloadModule(const QString& pathToExecutable);

  QHash<QString, ctkCmdLineModuleReference> MapFileNameToReference;
  ctkCmdLineModuleManager* ModuleManager;
  QFileSystemWatcher* FileSystemWatcher;
  bool Debug;
};

#endif

