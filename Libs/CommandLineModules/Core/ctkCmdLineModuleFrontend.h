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

#ifndef CTKCMDLINEMODULEFRONTEND_H
#define CTKCMDLINEMODULEFRONTEND_H

#include "ctkCommandLineModulesCoreExport.h"

#include <QObject>

template<class K, class V> class QHash;
class QUrl;

class ctkCmdLineModuleFuture;
class ctkCmdLineModuleReference;
class ctkCmdLineModuleParameter;
struct ctkCmdLineModuleFrontendPrivate;

/**
 * \class ctkCmdLineModuleFrontend
 * \brief Abstract base class for all front-end command
 * line module implementations.
 * \ingroup CommandLineModulesCore
 * \see ctkCmdLineModuleFrontendQtGui
 * \see ctkCmdLineModuleFrontendQtWebKit
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleFrontend : public QObject
{
  Q_OBJECT

public:

  enum ParameterValueRole {

    /* Data returned using this role must not be of any type not supported by
       QVariant by default. For complex parameter types (like file, image,
       geometry, etc.) the data must be convertible to a QString pointing
       to a local resource.
     */
    LocalResourceRole = 0,

    /* This role can be used in custom frontends to return a QVariant
       containing for example an in-memory representation of a complex object.
       One can then either convert the in-memory representation to a local
       resource before running a module such that arbitrary backends relying on
       the LocalResourceRole can process the data. Or one creates a custom
       backend which knows how to handle QVariants returned by this role.
     */
    UserRole = 8
  };

  enum ParameterFilter {
    Input = 0x01,
    Output = 0x02,
    All = Input | Output
  };
  Q_DECLARE_FLAGS(ParameterFilters, ParameterFilter)

  ~ctkCmdLineModuleFrontend();

  /**
   * @brief Returns the GUI representation, currently supporting only
   * QObject subclasses.
   * @return The GUI that can then be embeded in an applicaiton
   * window for instance.
   */
  virtual QObject* guiHandle() const = 0;

  /**
   * @brief GUIs will need to be able to read parameters,
   * here we retrieve by role.
   * @return QVariant
   * @see ParameterValueRole
   */
  virtual QVariant value(const QString& parameter,
      int role = LocalResourceRole) const = 0;

  /**
   * @brief Set the value of a certain parameters
   * @param parameter The name of the parameter, as defined in the XML.
   * @param value The value for that parameter.
   */
  virtual void setValue(const QString& parameter, const QVariant& value) = 0;

  /**
   * @brief Return the ctkCmdLineModuleFuture, derived from QFuture to
   * provide asynchronous processing.
   * @see ctkCmdLineModuleFuture
   */
  virtual ctkCmdLineModuleFuture future() const;

  /**
   * @brief Returns a QUrl to define the location of the module that is run.
   *
   * For a local process this may be the file location of the command
   * line module. For other implementations, such as a web-service,
   * this could be a web URL.
   *
   * @return QUrl A resource independent URL defining where the module is.
   */
  QUrl location() const;

  /**
   * @brief Returns a ctkCmdLineModuleReference value object that refers
   * and provides access to the module.
   * @return ctkCmdLineModuleReference
   */
  ctkCmdLineModuleReference moduleReference() const;

  /**
   * @brief Returns a list of all valid parameter names.
   */
  virtual QList<QString> parameterNames() const;

  /**
   * @brief Returns a map of parameter names and values.
   */
  virtual QHash<QString,QVariant> values() const;

  /**
   * @brief Enables the parameter values to be set.
   */
  virtual void setValues(const QHash<QString,QVariant>& values);

  /**
   * @brief Indicates if the underlying process is currently active.
   * @return true if running and false otherwise.
   */
  bool isRunning() const;

  /**
   * @brief Indicates if the underlying process is currently paused.
   * @return true if paused and false otherwise.
   */
  bool isPaused() const;

  Q_SIGNAL void valueChanged(const QString& parameter, const QVariant& value);

  // convenience methods

  /**
   * @brief Useful method to return subsets of parameter objects, searhing
   * by type for example "image" and filter for example "input"/"output".
   * @param type The type of parameter, as defined in the XML element.
   * @param filters flag to define whether we want input/output.
   * @return QList of ctkCmdLineModuleParameter depending on type and filters.
   * @see ParameterFilter
   */
  QList<ctkCmdLineModuleParameter> parameters(
      const QString& type = QString(),
      ParameterFilters filters = All);

  void resetValues();

protected:

  /**
   * @brief Constructor.
   */
  ctkCmdLineModuleFrontend(const ctkCmdLineModuleReference& moduleRef);

  /**
   * @brief Sets the ctkCmdLineModuleFuture which effectively
   * contains the backend that is run.
   */
  void setFuture(const ctkCmdLineModuleFuture& future);

private:

  Q_DISABLE_COPY(ctkCmdLineModuleFrontend)

  friend class ctkCmdLineModuleManager;
  friend class ctkCmdLineModulePrivate;

  QScopedPointer<ctkCmdLineModuleFrontendPrivate> d;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkCmdLineModuleFrontend::ParameterFilters)

#endif // CTKCMDLINEMODULEFRONTEND_H
