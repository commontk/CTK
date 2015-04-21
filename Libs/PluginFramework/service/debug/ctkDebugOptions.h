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

#ifndef CTKDEBUGOPTIONS_H
#define CTKDEBUGOPTIONS_H

#include <ctkPluginFrameworkExport.h>

#include <QString>
#include <QHash>
#include <QVariant>

/**
 * Used to get debug options settings.
 */
struct CTK_PLUGINFW_EXPORT ctkDebugOptions
{

  virtual ~ctkDebugOptions() {}

  /**
   * The service property (named &quot;listener.symbolic.name&quot;) which specifies
   * the bundle symbolic name of a {@link DebugOptionsListener} service.
   */
  static const QString LISTENER_SYMBOLICNAME; // = "listener.symbolic.name";

  /**
   * Returns the identified option as a boolean value. The specified
   * defaultValue is returned if no such option is found or if debug is not enabled.
   *
   * <p>
   * Options are specified in the general form <i>&lt;Plugin-SymbolicName&gt;/&lt;option-path&gt;</i>.
   * For example, <code>org.commontk.configadmin/debug</code>
   * </p>
   *
   * @param option the name of the option to lookup
   * @param defaultValue the value to return if no such option is found
   * @return the value of the requested debug option or the
   * defaultValue if no such option is found.
   */
  virtual bool getBooleanOption(const QString& option, bool defaultValue) const = 0;

  /**
   * Returns the identified option.  A null value
   * is returned if no such option is found or if debug is not enabled.
   *
   * <p>
   * Options are specified
   * in the general form <i>&lt;Plugin-SymbolicName&gt;/&lt;option-path&gt;</i>.
   * For example, <code>org.commontk.configadmin/debug</code>
   * </p>
   *
   * @param option the name of the option to lookup
   * @return the value of the requested debug option or <code>null</code>
   */
  virtual QVariant getOption(const QString& option) const = 0;

  /**
   * Returns the identified option. The specified defaultValue is
   * returned if no such option is found or if debug is not enabled.
   *
   * <p>
   * Options are specified
   * in the general form <i>&lt;Plugin-SymbolicName&gt;/&lt;option-path&gt;</i>.
   * For example, <code>org.commontk.configadmin/debug</code>
   * </p>
   *
   * @param option the name of the option to lookup
   * @param defaultValue the value to return if no such option is found
   * @return the value of the requested debug option or the
   * defaultValue if no such option is found.
   */
  virtual QVariant getOption(const QString& option, const QVariant& defaultValue) const = 0;

  /**
   * Returns the identified option as an int value. The specified
   * defaultValue is returned if no such option is found or if an
   * error occurs while converting the option value
   * to an integer or if debug is not enabled.
   *
   * <p>
   * Options are specified
   * in the general form <i>&lt;Plugin-SymbolicName&gt;/&lt;option-path&gt;</i>.
   * For example, <code>org.commontk.configadmin/debug</code>
   * </p>
   *
   * @param option the name of the option to lookup
   * @param defaultValue the value to return if no such option is found
   * @return the value of the requested debug option or the
   * defaultValue if no such option is found.
   */
  virtual int getIntegerOption(const QString& option, int defaultValue) const = 0;

  /**
   * Returns a snapshot of the current options.  If no
   * options are set then an empty map is returned.
   * <p>
   * If debug is not enabled then the snapshot of the current disabled
   * values is returned. See setDebugEnabled(bool).
   * </p>
   * @return a snapshot of the current options.
   */
  virtual QHash<QString, QVariant> getOptions() const = 0;

  /**
   * Sets the identified option to the identified value. If debug is
   * not enabled then the specified option is not changed.
   * @param option the name of the option to set
   * @param value the value of the option to set
   */
  virtual void setOption(const QString& option, const QVariant& value) = 0;

  /**
   * Sets the current option key/value pairs to the specified options.
   * The specified map replaces all keys and values of the current debug options.
   * <p>
   * If debug is not enabled then the specified options are saved as
   * the disabled values and no notifications will be sent.
   * See setDebugEnabled(bool).
   * If debug is enabled then notifications will be sent to the
   * listeners which have options that have been changed, added or removed.
   * </p>
   *
   * @param options the new set of options
   */
  virtual void setOptions(const QHash<QString, QVariant>& ops) = 0;

  /**
   * Removes the identified option.  If debug is not enabled then
   * the specified option is not removed.
   * @param option the name of the option to remove
   */
  virtual void removeOption(const QString& option) = 0;

  /**
   * Returns true if debugging/tracing is currently enabled.
   * @return true if debugging/tracing is currently enabled;  Otherwise false is returned.
   */
  virtual bool isDebugEnabled() const = 0;

  /**
   * Enables or disables debugging/tracing.
   * <p>
   * When debug is disabled all debug options are unset.
   * When disabling debug the current debug option values are
   * stored in memory as disabled values. If debug is re-enabled the
   * disabled values will be set back and enabled.  The disabled values
   * are only stored in memory and if the framework is restarted then
   * the disabled option values will be lost.
   * </p>
   * @param value If <code>true</code>, debug is enabled, otherwise
   * debug is disabled.
   */
  virtual void setDebugEnabled(bool enabled) = 0;

};

Q_DECLARE_INTERFACE(ctkDebugOptions, "org.commontk.service.debug.DebugOptions")

#endif // CTKDEBUGOPTIONS_H
