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

#ifndef CTKCMDLINEMODULEMANAGER_H
#define CTKCMDLINEMODULEMANAGER_H

#include <ctkCommandLineModulesCoreExport.h>

#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include "ctkCmdLineModuleReference.h"

struct ctkCmdLineModuleBackend;
struct ctkCmdLineModuleFrontendFactory;
class ctkCmdLineModuleFrontend;
class ctkCmdLineModuleFuture;

struct ctkCmdLineModuleManagerPrivate;

/**
 * @ingroup CommandLineModulesCore_API
 *
 * @brief Main manager class to handle loading and unloading of modules.
 *
 * This is the central managing class for CTK "command line modules". To register modules,
 * you need a command line module back-end which is capable of handling the URL scheme
 * under which you want to register your modules.
 *
 * A default back-end for handling "file" URLs which runs local executables is
 * available in ctkCmdLineModuleBackendLocalProcess.
 *
 * To create and run a specific front-end for a given module, you need a concrete
 * ctkCmdLineModuleFrontend implementation. The implementation provided in
 * ctkCmdLineModuleFrontendQtGui creates a default Qt widgets based GUI for a given
 * ctkCmdLineModuleReference instance.
 *
 * Here is an example how this and related classes could be used:
 *
 * First, we create ctkCmdLineModuleManager instance.
 * \snippet ModuleManager/main.cpp instantiate-mm
 *
 * Then we need at least one back-end which can handle registering modules.
 * \snippet ModuleManager/main.cpp register-backend
 *
 * Finally, we can register a module.
 * \snippet ModuleManager/main.cpp register-module
 *
 * @see ctkCmdLineModuleFrontend
 * @see ctkCmdLineModuleBackend
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleManager : public QObject
{
  Q_OBJECT

public:

  enum ValidationMode {
    /** registerModule() will throw an exception if the XML is invalid */
    STRICT_VALIDATION,
    /** no XML schema validation at all */
    SKIP_VALIDATION,
    /**
     * registerModule() will validate the XML description but proceed with
     * registration on validation error.
     */
    WEAK_VALIDATION
  };

  /**
   * @brief Create a module manager instance.
   * @param validationMode The validation mode for the XML description of the module parameters.
   * @param cacheDir The directory where to cache information about registered modules.
   *
   * If the <code>validationMode</code> argument is set to <code>SKIP_VALIDATION</code>, no XML validation
   * takes place and certain front-ends might fail to generate a GUI. If it is set to
   * <code>WEAK_VALIDATION</code>, module registrations will proceed on error but the error status
   * is available via ctkCmdLineModuleReference::xmlValidationErrorString().
   */
  ctkCmdLineModuleManager(ValidationMode validationMode = STRICT_VALIDATION,
                          const QString& cacheDir = QString());

  ~ctkCmdLineModuleManager();

  /**
   * @brief Get the validation mode.
   * @return The validation mode.
   */
  ValidationMode validationMode() const;

  /**
   * @brief Set the validation mode.
   */
  void setValidationMode(const ValidationMode& mode);

  /**
   * @brief Set the timeout for retrieving the XML parameter description from a module.
   *
   * The default time-out is 30 seconds.
   *
   * @param timeout The timeout in milli seconds.
   */
  void setTimeOutForXMLRetrieval(int timeout);

  /**
   * @brief Get the timeout for retrieving the XML parameter description from a module.
   * @return The timeout in milli seconds.
   */
  int timeOutForXMLRetrieval() const;

  /**
   * @brief Registers a new back-end.
   * @param backend The new back-end.
   * @throws ctkInvalidArgumentException if another back-end was already registered handling
   *         one of the URL schemes this back-end claims to handle.
   */
  void registerBackend(ctkCmdLineModuleBackend* backend);

  /**
   * @brief Get the registered backend for a scheme.
   * @param scheme The scheme the backend was registered with
   * @return The backend or NULL, if \c scheme is unknown.
   */
  ctkCmdLineModuleBackend* backend(const QString& scheme) const;

  /**
   * @brief Get a list of all registered backends.
   * @return A list of currently registered backends.
   */
  QList<ctkCmdLineModuleBackend*> backends() const;

  /**
   * @brief Registers a module, identified by the given URL.
   * @param location The URL for the new module.
   * @return A module reference.
   * @throws ctkInvalidArgumentException if no back-end for the given URL scheme was registered
   *         or the XML description for the module is invalid.
   * @throws ctkCmdLineModuleTimeoutException if a time-out occured when retrieving the
   *         XML description from the module.
   * @throws ctkCmdLineModuleRunException if a general error occurred when running the module.
   */
  ctkCmdLineModuleReference registerModule(const QUrl& location);

  /**
   * @brief Unregister a previously registered module.
   * @param moduleRef The reference for the module to unregister.
   *
   * This method does nothing if the module reference <code>moduleRef</code> is invalid or
   * reference an un-registered module.
   */
  void unregisterModule(const ctkCmdLineModuleReference& moduleRef);

  /**
   * @brief Clears the XML/timestamp cache.
   */
  void clearCache();

  /**
   * @brief Returns a ctkCmdLineModuleReference object for the given URL.
   * @param location The location URL for which to get a module reference.
   * @return The module reference for the location or an invalid module reference
   *         if no module was registered under the given location URL.
   */
  ctkCmdLineModuleReference moduleReference(const QUrl& location) const;

  /**
   * @brief Returns module references for all currently registered modules.
   * @return A list of module references.
   */
  QList<ctkCmdLineModuleReference> moduleReferences() const;

  /**
   * @brief Run a module front-end.
   * @param frontend The module front-end to run.
   * @return A ctkCmdLineModuleFuture object which can be used to interact with the
   *         running front-end.
   *
   * This method takes a ctkCmdLineModuleFrontend pointer and uses the registered back-end
   * for this module's location URL scheme to run it asynchronously. The returned future object
   * is the only way to interact with the task started by the specific back-end.
   *
   * @see ctkCmdLineModuleFuture
   * @see ctkCmdLineModuleFutureWatcher
   */
  ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend* frontend);

Q_SIGNALS:

  /**
   * @brief This signal is emitted whenever a module is registered.
   */
  void moduleRegistered(const ctkCmdLineModuleReference&);

  /**
   * @brief This signal is emitted whenever a module is un-registered.
   */
  void moduleUnregistered(const ctkCmdLineModuleReference&);

private:

  QScopedPointer<ctkCmdLineModuleManagerPrivate> d;

  Q_DISABLE_COPY(ctkCmdLineModuleManager)

};

#endif // CTKCMDLINEMODULEMANAGER_H
