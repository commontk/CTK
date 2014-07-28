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

#include "ctkCmdLineModuleReferenceResult.h"
#include "ctkCmdLineModuleDirectoryWatcher.h"

class QFileSystemWatcher;

/**
 * \class ctkCmdLineModuleDirectoryWatcherPrivate
 * \brief Private implementation class implementing directory/file watching to
 * load new modules into a ctkCmdLineModuleManager.
 *
 * \ingroup CommandLineModulesCore_API
 * \author m.clarkson@ucl.ac.uk
 */
class ctkCmdLineModuleDirectoryWatcherPrivate : public QObject
{

  Q_OBJECT

public:

  ctkCmdLineModuleDirectoryWatcherPrivate(ctkCmdLineModuleDirectoryWatcher* d,
                                          ctkCmdLineModuleManager* ModuleManager);
  virtual ~ctkCmdLineModuleDirectoryWatcherPrivate();

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::setDebug
   */
  void setDebug(bool debug);

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::setDirectories
   */
  void setDirectories(const QStringList& directories);

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::directories
   */
  QStringList directories() const;

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::setAdditionalModules
   */
  void setAdditionalModules(const QStringList& files);

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::additionalModules
   */
  QStringList additionalModules() const;

  /**
   * \see ctkCmdLineModuleDirectoryWatcher::commandLineModules
   */
  QStringList commandLineModules() const;

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
   * This is the main method, called by others to update what is being watched in terms of both files and directories.
   *
   * \param directories list of absolute directory paths
   * \param files list of absolute file paths
   */
  void updateWatchedPaths(const QStringList& directories, const QStringList& files);

  /**
   * \brief Takes a list of directories, and only returns ones that are valid,
   * meaning that the directory name is non-null, non-empty, and the directory exists.
   *
   * \param directories a list of directories, relative or absolute.
   * \return a list of valid directories, denoted by their absolute path.
   */
  QStringList filterInvalidDirectories(const QStringList& directories) const;

  /**
   * \brief Takes a list of filenames, and only returns ones that are not contained within
   * the current list of directories that are being watched.
   *
   * \param filenames a list of filenames, relative or absolute.
   * \return a list of valid filenames, denoted by absolute path, that are not contained within
   * the current list of directories being scanned.
   */
  QStringList filterFilesNotInCurrentDirectories(const QStringList& filenames) const;

  /**
   * \brief Returns a list of executable files (not necessarily valid command line clients) in a directory.
   *
   * \param directory the absolute or relative path of a directory.
   * \return a list of absolute path names to executable files
   */
  QStringList getExecutablesInDirectory(const QString& directory) const;

  /**
   * \brief Uses the MapFileNameToReference to work out a list of valid command line modules in a given directory.
   *
   * \param directory the absolute or relative path of a directory.
   * \return a list of executables, denoted by their absolute path.
   */
  QStringList extractCurrentlyWatchedFilenamesInDirectory(const QString& directory) const;

  /**
   * \brief Main method to update the current list of watched executables in a given set of directories.
   * \param directories a list of directories, denoted by their absolute path.
   */
  void setModules(const QStringList &directories);

  /**
   * \brief Called from the onDirectoryChanged slot to update the current list of modules
   * by calling back to setModules.
   * \param directory denoted by its absolute path.
   */
  void updateModules(const QString &directory);

  /**
   * \brief Uses the ctkCmdLineModuleManager to try and add the executables to the list
   * of executables, and if successful it is added to this->MapFileNameToReference.
   *
   * \param executables A list of paths to executable files, denoted by an absolute path.
   */
  QList<ctkCmdLineModuleReferenceResult> loadModules(const QStringList& executables);

  /**
   * \brief Removes the executables from both the ctkCmdLineModuleManager and this->MapFileNameToReference.
   *
   * \param executables path to an executable file, denoted by its absolute path.
   */
  void unloadModules(const QStringList& executables);

  ctkCmdLineModuleDirectoryWatcher* q;
  QHash<QString, ctkCmdLineModuleReferenceResult> MapFileNameToReferenceResult;
  ctkCmdLineModuleManager* ModuleManager;
  QFileSystemWatcher* FileSystemWatcher;
  QStringList AdditionalModules;
  bool Debug;
};

#endif

