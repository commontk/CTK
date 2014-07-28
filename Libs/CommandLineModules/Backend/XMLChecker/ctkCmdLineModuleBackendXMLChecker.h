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

#ifndef CTKCMDLINEMODULEBACKENDXMLCHECKER_H
#define CTKCMDLINEMODULEBACKENDXMLCHECKER_H

#include "ctkCmdLineModuleBackend.h"

#include "ctkCommandLineModulesBackendXMLCheckerExport.h"

#include <QScopedPointer>

struct ctkCmdLineModuleBackendXMLCheckerPrivate;

/**
 * @ingroup CommandLineModulesBackendXMLChecker_API
 *
 * @brief Provides a ctkCmdLineModuleBackend implementation
 * to pretend to run a command line process, but actually
 * it returns a fixed, static piece of XML which can then
 * be validated by the core library.
 */
class CTK_CMDLINEMODULEBACKENDXMLCHECKER_EXPORT ctkCmdLineModuleBackendXMLChecker : public ctkCmdLineModuleBackend
{

public:

  ctkCmdLineModuleBackendXMLChecker();
  ctkCmdLineModuleBackendXMLChecker(const QString &xmlToValidate);
  ~ctkCmdLineModuleBackendXMLChecker();

  virtual QString name() const;
  virtual QString description() const;

  /**
   * @brief This back-end can handle the "xml checker" URL scheme.
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

  void setXML(const QString& xml);
  QString xml() const;

private:

  QScopedPointer<ctkCmdLineModuleBackendXMLCheckerPrivate> d;

};

#endif // CTKCMDLINEMODULEBACKENDLOCALPROCESS_H
