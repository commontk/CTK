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

#ifndef __ctkCmdLineModuleDirectoryWatcher_h
#define __ctkCmdLineModuleDirectoryWatcher_h

#include <ctkCommandLineModulesCoreExport.h>

#include <QObject>
#include <QScopedPointer>

class ctkCmdLineModuleManager;
class ctkCmdLineModuleDirectoryWatcherPrivate;

/**
 * \class ctkCmdLineModuleDirectoryWatcher
 * \brief Provides directory scanning and file watching via QFileSystemWatcher to
 * automatically load new modules into a ctkCmdLineModuleManager.
 *
 * \ingroup CommandLineModulesCore_API
 * \author m.clarkson@ucl.ac.uk
 *
 * This class can be used in 3 ways.
 *
 * 1. The user can provide a set of directories by calling setDirectories().
 * These directories are scanned for valid command line executables, which
 * are registered with the ctkCmdLineModuleManager. The QFileSystemWatcher
 * then watches for any changes in these directories and files.
 *
 * OR
 *
 * 2. The user can directly provide a list of files, which should be
 * valid command line executables, which are registered with the ctkCmdLineModuleManager
 * and the QFileSystemWatcher then watches for changes in these files.
 *
 * OR
 *
 * 3. Both of the above. In this case, the set of files specified must
 * not be contained within the set of directories specified. For this reason, we have
 * "setDirectories", and then "setAdditionalModules", as the list of files should
 * be considered as being "in addition" to any directories we are watching.
 *
 * If either directories or files are invalid (not existing, not executable etc),
 * they are filtered out and ignored.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleDirectoryWatcher
: public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleDirectoryWatcher(ctkCmdLineModuleManager* moduleManager);
  virtual ~ctkCmdLineModuleDirectoryWatcher();

  /**
   * \brief Set the watcher into debug mode, for more output.
   * \param debug if true, you get more output on the console, otherwise, less output.
   */
  void setDebug(bool debug);

  /**
   * \brief Set the directories to be watched.
   * \param directories a list of directory names. If any of these are
   * invalid, they will be filtered out and ignored.
   */
  void setDirectories(const QStringList& directories);

  /**
   * \brief Returns the list of directories currently being watched.
   */
  QStringList directories() const;

  /**
   * \brief Sets an additional list of command line executables to watch.
   * \param files a list of file names. If any of these file names are
   * not valid command line executables, they will be filtered out and ignored.
   */
  void setAdditionalModules(const QStringList& files);

  /**
   * \brief Gets the list of additional command line executable, where
   * "additional" means "in addition to those directories we are watching".
   */
  QStringList additionalModules() const;

  /**
   * \brief Returns the complete list of files (command line executables)
   * currently being watched.
   */
  QStringList commandLineModules() const;

  /**
   * \brief public method to emit the errorDetected signal.
   */
  void emitErrorDectectedSignal(const QString&);

Q_SIGNALS:

  /**
   * \brief Signals that an error was detected, and the caller should raise an error to the user.
   */
  void errorDetected(const QString&);

private:

  QScopedPointer<ctkCmdLineModuleDirectoryWatcherPrivate> d;
  Q_DISABLE_COPY(ctkCmdLineModuleDirectoryWatcher)
};

#endif // __ctkCmdLineModuleDirectoryWatcher_h
