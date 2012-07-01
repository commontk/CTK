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

#ifndef CTKPLUGINEXCEPTION_H
#define CTKPLUGINEXCEPTION_H

#include "ctkException.h"

#include <ctkPluginFrameworkExport.h>

/**
 * \ingroup PluginFramework
 *
 * A Plugin Framework exception used to indicate that a plugin lifecycle
 * problem occurred.
 *
 * <p>
 * A <code>ctkPluginException</code> object is created by the Framework to denote
 * an exception condition in the lifecycle of a plugin.
 * <code>ctkPluginException</code>s should not be created by plugin developers.
 * An enum type is used to identify the exception type for future extendability.
 *
 * <p>
 * This exception conforms to the general purpose exception chaining mechanism.
 */
class CTK_PLUGINFW_EXPORT ctkPluginException : public ctkRuntimeException
{
public:

  enum Type {
    /**
     * No exception type is unspecified.
     */
    UNSPECIFIED,
    /**
     * The operation was unsupported.
     */
    UNSUPPORTED_OPERATION,
    /**
     * The operation was invalid.
     */
    INVALID_OPERATION,
    /**
     * The plugin manifest contains errors.
     */
    MANIFEST_ERROR,
    /**
     * The plugin was not resolved.
     */
    RESOLVE_ERROR,
    /**
     * The plugin activator was in error.
     */
    ACTIVATOR_ERROR,
    /**
     * The operation failed due to insufficient permissions.
     */
    SECURITY_ERROR,
    /**
     * The operation failed to complete the requested lifecycle state change.
     */
    STATECHANGE_ERROR,
    /**
     * The install or update operation failed because another
     * already installed plugin has the same symbolic name and version.
     */
    DUPLICATE_PLUGIN_ERROR,
    /**
     * The framework received an error while reading the input stream for a plugin.
     */
    READ_ERROR,
    /**
     * The start transient operation failed because the start level of the
     * plugin is greater than the current framework start level
     */
    START_TRANSIENT_ERROR
  };

  /**
   * Creates a <code>ctkPluginException</code> with the specified message and type.
   *
   * @param msg The associated message.
   * @param type The type for this exception.
   */
  ctkPluginException(const QString& msg, const Type& type = UNSPECIFIED);

  /**
   * Creates a <code>ctkPluginException</code> with the specified message, type
   * and exception cause.
   *
   * @param msg The associated message.
   * @param type The type for this exception.
   * @param cause The cause of this exception.
   */
  ctkPluginException(const QString& msg, const Type& type, const ctkException& cause);

  /**
   * Creates a <code>ctkPluginException</code> with the specified message and
   * exception cause.
   *
   * @param msg The associated message.
   * @param cause The cause of this exception.
   */
  ctkPluginException(const QString& msg, const ctkException& cause);

  ctkPluginException(const ctkPluginException& o);
  ctkPluginException& operator=(const ctkPluginException& o);

  ~ctkPluginException() throw();

  /**
   * @see ctkException::name()
   */
  const char* name() const throw();

  /**
   * @see ctkException::clone()
   */
  ctkPluginException* clone() const;

  /**
   * @see ctkException::rethrow()
   */
  void rethrow() const;

  /**
   * Returns the type for this exception or <code>UNSPECIFIED</code> if the
   * type was unspecified or unknown.
   *
   * @return The type of this exception.
   */
  Type getType() const;

private:

  /**
   * Type of plugin exception.
   */
  Type type;

};

#endif // CTKPLUGINEXCEPTION_H
