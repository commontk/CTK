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

#ifndef CTKCMDLINEMODULEBACKENDLOCALPROCESS_H
#define CTKCMDLINEMODULEBACKENDLOCALPROCESS_H

#include "ctkCmdLineModuleBackend.h"

#include "ctkCommandLineModulesBackendLocalProcessExport.h"

#include <QScopedPointer>

struct ctkCmdLineModuleBackendLocalProcessPrivate;

/**
 * @ingroup CommandLineModulesBackendLocalProcess_API
 *
 * @brief Provides an ctkCmdLineModuleBackend implementation
 * to run a locally installed command line application.
 *
 * Use this back-end if you want to be able to register local executables as command
 * line modules. The back-end handles the "file" URL scheme, allowing you to register
 * modules with the ctkCmdLineModuleManager by using
 * @code
 * ctkCmdLineModuleManager::registerModule(QUrl::fromLocalFile("/path/to/executable"));
 * @endcode
 *
 * The XML description for a module is extracted from the standard output of the
 * executable when calling it with the \c &ndash;&ndash;xml command line argument.
 *
 * The ctkCmdLineModuleFuture returned by run() allows cancelation by killing the running
 * process. On Unix systems, it also allows to pause it.
 */
class CTK_CMDLINEMODULEBACKENDLP_EXPORT ctkCmdLineModuleBackendLocalProcess : public ctkCmdLineModuleBackend
{

public:

  ctkCmdLineModuleBackendLocalProcess();
  ~ctkCmdLineModuleBackendLocalProcess();

  virtual QString name() const;
  virtual QString description() const;

  /**
   * @brief This back-end can handle the "file" URL scheme.
   * @return Returns the schemes this back-end can handle.
   */
  virtual QList<QString> schemes() const;

  /**
   * @brief Returns the last modified time of the module at \c location.
   * @param location The location URL of the module for which to get the timestamp.
   * @return A timestamp.
   */
  virtual qint64 timeStamp(const QUrl &location) const;

  /**
   * @brief Get the raw XML description from the module at \c location.
   * @param location The location URL of the module for which to get the XML description.
   * @return The raw XML description.
   *
   * This method always calls the executable with a \c &ndash;&ndash;xml argument and returns
   * the complete data emitted on the standard output channel.
   */
  virtual QByteArray rawXmlDescription(const QUrl& location, int timeout);

  /**
   * @brief Run a front-end for this module in a local process.
   * @param frontend The front-end to run.
   * @return A future object for communicating with the running process.
   */
  virtual ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend *frontend);

  /**
   * @brief Setter for the number of milliseconds to wait when retrieving xml.
   * @param timeOut in milliseconds.
   */
  void setTimeOutForXMLRetrieval(int timeOut);

  /**
   * @brief Returns the number of milliseconds to wait when retrieving xml.
   * @return Time-out in milliseconds.
   */
  virtual int timeOutForXMLRetrieval() const;

private:

  QScopedPointer<ctkCmdLineModuleBackendLocalProcessPrivate> d;

};

#endif // CTKCMDLINEMODULEBACKENDLOCALPROCESS_H
