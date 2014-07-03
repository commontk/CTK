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
 * @ingroup CommandLineModulesCore_API
 *
 * @brief Abstract base class for all back-end command line module
 * implementations.
 *
 * A back-end is responsible for providing the XML module description for a
 * given URL and its "timestamp". It also knows how to actually run a module,
 * using the current parameter values provided by a ctkCmdLineModuleFrontend instance.
 *
 * @see ctkCmdLineModuleBackendLocalProcess
 * @see ctkCmdLineModuleBackendFunctionPointer
 */
struct CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleBackend
{

  virtual ~ctkCmdLineModuleBackend();

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
   * @brief Returns a list of URL schemes this back-end can handle.
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
   * @param timeout The time-out for retrieving the XML parameter description
   * @return The raw XML parameter description.
   *
   * This method may be concurrently called by the ctkCmdLineModuleManager and
   * must be thread-safe. Implementations must not use any caching mechanism,
   * as caching is done by the ctkCmdLineModuleManager itself, checking the
   * return value of timeStamp().
   *
   * Implementations should also throw either a ctkCmdLineModuleTimeoutException
   * object if a time-out occured when retrieving the XML parameter description
   * or a ctkCmdLineModuleRunException for any other error during invocation
   * of the module.
   *
   * @throws ctkCmdLineModuleTimeoutException if a time-out occurred when
   *         retrieving the XML parameter description.
   * @throws ctkCmdLineModuleRunException if a runtime error occurred when
   *         invoking the module to retrieve the XML parameter description.
   */
  virtual QByteArray rawXmlDescription(const QUrl& location, int timeout) = 0;

  /**
   * @brief Get the XML parameter description from the given location.
   * @param location The location URL specifying the module.
   * @return The raw XML parameter description.
   *
   * This method calls rawXmlDescription(const QUrl&, int) with a timeout
   * of 30 seconds.
   *
   * @throws ctkCmdLineModuleTimeoutException if a time-out occurred when
   *         retrieving the XML parameter description.
   * @throws ctkCmdLineModuleRunException if a runtime error occurred when
   *         invoking the module to retrieve the XML parameter description.
   */
  QByteArray rawXmlDescription(const QUrl& location);

  /**
   * @brief returns the number of milliseconds to wait when retrieving xml.
   *
   * The default implementation returns 0, which signals that the global
   * timeout value from the ctkCmdLineModuleManager object with which this
   * backend was registered should be used.
   *
   * @return int Time-out in milliseconds.
   */
  virtual int timeOutForXmlRetrieval() const;

protected:

  friend class ctkCmdLineModuleManager;

  /**
   * @brief The main method to actually execute the back-end process.
   * @param frontend A pointer to a front end implementation.
   *
   * Implementations must execute the actual task of running the module asynchronously
   * and return from this method immediately. After returning from this method,
   * accessing the <code>frontend</code> pointer is not guaranteed to be safe.
   */
  virtual ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend* frontend) = 0;

};

#endif // CTKCMDLINEMODULEBACKEND_H
