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
class ctkCmdLineModuleResult;
struct ctkCmdLineModuleFrontendPrivate;

/**
 * \class ctkCmdLineModuleFrontend
 * \brief Abstract base class for all front-end command
 * line module implementations.
 * \ingroup CommandLineModulesCore_API
 *
 * A module front-end represents a set of current parameter values for a specific
 * module. A front-end instance is usually associated with a graphical user interface,
 * accessible via guiHandle(). This allows users to interactively change parameter values
 * of the module.
 *
 * \see ctkCmdLineModuleFrontendQtGui
 * \see ctkCmdLineModuleFrontendQtWebKit
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleFrontend : public QObject
{
  Q_OBJECT
  Q_ENUMS(ParamterValueRole)

public:

  enum ParameterValueRole {

    /**
     * Data returned using this role must not be of any type not supported by
     * QVariant by default. For complex parameter types (like file, image,
     * geometry, etc.) the data must be convertible to a QString pointing
     * to a local resource.
     *
     * This role is usually used by backends for retrieving data and is mainly
     * important for data which acts as a handle to the real data (e.g. a
     * backend usually needs to get the absolute path to a local file for the
     * current value of an input image parameter, instead of the image label
     * displayed in a GUI).
     */
    LocalResourceRole = 0,

    /**
     * Describes data suitable for displaying in a GUI. For many parameter types
     * (e.g. scalar and vector parameters) data returned by this role will be
     * the same as returned by the LocalResourceRole role.
     **/
    DisplayRole = 1,

    /**
     * This role can be used in custom frontends to return a QVariant
     * containing for example an in-memory representation of a complex object.
     * One can then either convert the in-memory representation to a local
     * resource before running a module such that arbitrary backends relying on
     * the LocalResourceRole role can process the data. Or one creates a custom
     * backend which knows how to handle QVariants returned by this role.
     */
    UserRole = 8
  };

  enum ParameterFilter {
    /** Parameters with channel = "input" */
    Input = 0x01,
    /** Parameter with channel = "output" */
    Output = 0x02,
    /** A convenience enum value combining Input and Output. */
    All = Input | Output
  };
  Q_DECLARE_FLAGS(ParameterFilters, ParameterFilter)

  virtual ~ctkCmdLineModuleFrontend();

  /**
   * @brief Returns the GUI representation.
   * @return A GUI handle that can then be embeded in an application window for instance.
   *
   * The returned object is a handle to the real GUI toolkit specific object representing
   * the user interface. For Qt based front-ends, the returned object is usually a QWidget
   * instance pointing to the main container widget for the GUI. See the documentation
   * of the front-end sub-class for specific information.
   */
  virtual QObject* guiHandle() const = 0;

  /**
   * @brief GUIs will need to be able to read parameters,
   * here we retrieve by role.
   *
   * @return QVariant
   * @see ParameterValueRole
   */
  virtual QVariant value(const QString& parameter,
                         int role = LocalResourceRole) const = 0;

  /**
   * @brief Set the value of a certain parameter.
   *
   * @param parameter The name of the parameter, as defined in the XML.
   * @param value The value for that parameter.
   * @param role The role for which to set the data.
   *
   * @see ParameterValueRole
   */
  virtual void setValue(const QString& parameter, const QVariant& value,
                        int role = DisplayRole) = 0;

  /**
   * @brief Return the ctkCmdLineModuleFuture, derived from QFuture to
   * provide asynchronous processing and interaction with the running frontend.
   *
   * Note that the future returned by this method will be different after the
   * frontend was started. Either use isRunning() to check wether this frontend
   * is currently running or connect to the started() signal.
   *
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
   * @brief Indicates if the currently associated ctkCmdLineModuleFuture object
   *        is in state "running".
   * @return \c true if running and \c false otherwise.
   */
  bool isRunning() const;

  /**
   * @brief Indicates if the currently associated ctkCmdLineModuleFuture Object
   *        is in state "paused".
   * @return \c true if paused and \c false otherwise.
   */
  bool isPaused() const;

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

Q_SIGNALS:

  /**
   * @brief This signal is emitted whenever a parameter value is changed by using
   *        the ctkCmdLineModuleFrontend class.
   * @param parameter The parameter name.
   * @param value The new parameter value.
   *
   * Please note that this signal is not emitted if a parameter value is
   * changed in the generated GUI.
   */
  void valueChanged(const QString& parameter, const QVariant& value);

  /**
   * @brief This signal is emitted when the frontend is run.
   *
   * You can use this signal to get the ctkCmdLineModuleFuture instance
   * from future() to interact with the running frontend.
   */
  void started();

protected:

  /**
   * @brief Constructor.
   */
  ctkCmdLineModuleFrontend(const ctkCmdLineModuleReference& moduleRef);

private Q_SLOTS:

  /**
   * @brief Provides results as reported by the running module.
   * @param result
   *
   * This method is called when a running module reports a new
   * result. The default implementation updates the current value
   * of the output parameter in the GUI with the reported value.
   */
  virtual void resultReady(const ctkCmdLineModuleResult& result);

private:

  /**
   * @brief Sets the ctkCmdLineModuleFuture which effectively
   * contains the backend that is run.
   */
  void setFuture(const ctkCmdLineModuleFuture& future);

private:

  Q_DISABLE_COPY(ctkCmdLineModuleFrontend)

  friend struct ctkCmdLineModuleFrontendPrivate;
  friend class ctkCmdLineModuleManager;
  friend class ctkCmdLineModulePrivate;

  Q_PRIVATE_SLOT(d, void _q_resultReadyAt(int))

  QScopedPointer<ctkCmdLineModuleFrontendPrivate> d;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkCmdLineModuleFrontend::ParameterFilters)

#endif // CTKCMDLINEMODULEFRONTEND_H
