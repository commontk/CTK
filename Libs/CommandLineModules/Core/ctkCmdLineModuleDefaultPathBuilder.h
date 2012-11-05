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

#ifndef __ctkCmdLineModuleDefaultPathBuilder_h
#define __ctkCmdLineModuleDefaultPathBuilder_h

#include "ctkCommandLineModulesCoreExport.h"
#include <QStringList>
#include <QScopedPointer>

struct ctkCmdLineModuleDefaultPathBuilderPrivate;

/**
 * \class ctkCmdLineModuleDefaultPathBuilder
 * \brief Builds up a list of directory paths to search for command line modules.
 * \ingroup CommandLineModulesCore_API
 * \author m.clarkson@ucl.ac.uk
 *
 * Simple class to enable the user to easily add various directories
 * to a list of directories. You create this object, add a load of directories
 * by repeatedly calling add..() functions, and then call getDirectoryList()
 * to get the final StringList of directory locations.
 *
 * The choices are:
 * <pre>
 * 1. The directory or list of directories defined by the CTK_MODULE_LOAD_PATH environment variable.
 * Uses usual PATH semantics such as colon separator on *nix systems and semi-colon on Windows.
 * 2. The directory defined by the users HOME directory, or any sub-directory under this.
 * 3. The directory defined by the current working directory, or any sub-directory under this.
 * 4. The directory defined by the application installation directory or any sub-directory under this.
 * </pre>
 *
 * A strictMode flag exists to decide if this class only returns directories that already exist.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleDefaultPathBuilder
{

public:

  ctkCmdLineModuleDefaultPathBuilder();
  ~ctkCmdLineModuleDefaultPathBuilder();

  /**
   * @brief Clears the current list of directories.
   */
  virtual void clear();

  /**
   * @brief Sets strict mode which checks that all directories already exist.
   * @param strict if true this object will only return existing directories,
   * if false, the return StringList is un-checked.
   */
  virtual void setStrictMode(const bool& strict);

  /**
   * @brief Returns the strict mode flag.
   */
  virtual bool strictMode() const;

  /**
   * @brief Adds the users home directory, or if specified a sub-directory.
   *
   * This depends on QDir::home() existing. If this is not the case,
   * then this method will do nothing and ignore the request.
   */
  virtual void addHomeDir(const QString& subFolder = QString());

  /**
   * @brief Adds the current working directory, or if specified a sub-directory.
   *
   * This depends on QDir::current() existing. If this is not the case,
   * then this method will do nothing and ignore the request.
   */
  virtual void addCurrentDir(const QString& subFolder = QString());

  /**
   * @brief Adds the application installation directory, or if specified a sub-directory.
   */
  virtual void addApplicationDir(const QString& subFolder = QString());

  /**
   * @brief Adds the directories denoted by the environment variable CTK_MODULE_LOAD_PATH.
   *
   * Semi-colon separated lists of directories are allowed.
   */
  virtual void addCtkModuleLoadPath();

  /**
   * @brief Returns the QStringList containing directories.
   * @return QStringList of directories or, if in strict mode, directories that already exist.
   */
  virtual QStringList getDirectoryList() const;

private:

  QScopedPointer<ctkCmdLineModuleDefaultPathBuilderPrivate> d;
  Q_DISABLE_COPY(ctkCmdLineModuleDefaultPathBuilder)
};

#endif
