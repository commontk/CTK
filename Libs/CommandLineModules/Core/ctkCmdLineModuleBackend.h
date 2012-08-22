/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#ifndef CTKCMDLINEMODULEBACKEND_H
#define CTKCMDLINEMODULEBACKEND_H

#include "ctkCommandLineModulesCoreExport.h"

class ctkCmdLineModuleFrontend;
class ctkCmdLineModuleFuture;

template<typename T> class QList;
class QUrl;

/**
 * \class ctkCmdLineModuleBackend
 * \brief Abstract base class for all back-end command line module
 * implementations.
 * \ingroup CommandLineModulesCore
 * \see ctkCmdLineModuleBackendLocalProcess
 * \see ctkCmdLineModuleBackendFunctionPointer
 */
struct CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleBackend
{

  ~ctkCmdLineModuleBackend();

  /**
   * @brief Returns the name of the type of the backend, not the name
   * of the thing or application that is run.
   * @return A QString containing the name.
   */
  virtual QString name() const = 0;

  /**
   * @brief Returns a brief description of the type of the backend.
   * @return A QString containing a description.
   */
  virtual QString description() const = 0;

  /**
   * @brief Returns a list of the capabilities or the types of things
   * that this back-end can run.
   * @return A list of "schemes", meaning the capabilities.
   */
  virtual QList<QString> schemes() const = 0;

  /**
   * @brief Returns a timestap of the backend, which for example in the
   * case of the LocalProcess may be the last modified time of the command line
   * application.
   */
  virtual qint64 timeStamp(const QUrl& location) const = 0;

  /**
   * @brief Get the XML parameter description from the given location.
   * @param location The location URL specifying the module.
   * @return The raw XML parameter description.
   *
   * This method may be concurrently called by the ctkCmdLineModuleManager and
   * must be thread-safe.
   *
   */
  virtual QByteArray rawXmlDescription(const QUrl& location) = 0;

protected:

  friend class ctkCmdLineModuleManager;

  /**
   * @brief The main method to actually execute the back-end process.
   * @param A pointer to a front end implementation.
   */
  virtual ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend* frontend) = 0;

};

#endif // CTKCMDLINEMODULEBACKEND_H
