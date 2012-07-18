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
#include <QHash>
#include <QStringList>
#include <QScopedPointer>
#include "ctkCmdLineModuleReference.h"

class ctkCmdLineModuleManager;
class ctkCmdLineModuleDirectoryWatcherPrivate;

/**
 * \class ctkCmdLineModuleDirectoryWatcher
 * \brief Provides directory scanning to load new modules into a ctkCmdLineModuleManager.
 * \ingroup CommandLineModulesCore
 * \author m.clarkson@ucl.ac.uk
 *
 * This class provides directory scanning and automatic loading of command line modules.
 * The client should call setDirectories() to set the list of directories, and listen
 * to the signal modulesChanged to know when to re-build the GUI representation.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleDirectoryWatcher : public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleDirectoryWatcher(ctkCmdLineModuleManager* moduleManager);
  virtual ~ctkCmdLineModuleDirectoryWatcher();

  /**
   * \brief Set the watcher into debug mode, for more output.
   * \param debug if true, you get more output, otherwise, less output.
   */
  void setDebug(const bool& debug);

  /**
   * \brief Set the directories to be watched.
   * \param directories a StringList of directory names. If any of these are invalid, they will be filtered out and ignored.
   */
  void setDirectories(const QStringList& directories);

  /**
   * \brief Returns the list of directories currently being watched.
   */
  QStringList directories();

  /**
   * \brief Returns the list of files (command line apps) currently being watched.
   */
  QStringList files();

  /**
   * \brief Retrieves a map of filenames (command line apps) and their command line module reference.
   */
  QHash<QString, ctkCmdLineModuleReference> filenameToReferenceMap() const;

Q_SIGNALS:

  /**
   * \brief Signals that the modules have changed, so GUI's can re-build their menus.
   */
  void modulesChanged();

private Q_SLOTS:

  /**
   * \brief Private slot, so we can connect to the ctkCmdLineModuleDirectoryWatcherPrivate::modulesChanged signal.
   */
  void onModulesChanged();

private:

  QScopedPointer<ctkCmdLineModuleDirectoryWatcherPrivate> d;
  Q_DISABLE_COPY(ctkCmdLineModuleDirectoryWatcher)
};

#endif // __ctkCmdLineModuleDirectoryWatcher_h
