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

#ifndef CTKPLUGINCONSTANTS_H
#define CTKPLUGINCONSTANTS_H

#include <QString>

#include "ctkPluginFrameworkExport.h"

/**
 * \ingroup PluginFramework
 */
struct CTK_PLUGINFW_EXPORT ctkPluginConstants {

  /**
   * Location identifier of the CTK <i>system plugin</i>, which is defined
   * to be &quot;System Plugin&quot;.
   */
  static const QString SYSTEM_PLUGIN_LOCATION; // = "System Plugin"

  /**
   * Alias for the symbolic name of the CTK <i>system plugin</i>. It is
   * defined to be &quot;system.plugin&quot;.
   *
   */
  static const QString SYSTEM_PLUGIN_SYMBOLICNAME; // = "system.plugin"


  /**
   * Framework environment property identifying the Framework version.
   *
   * <p>
   * The value of this property may be retrieved by calling the
   * <code>ctkPluginContext::getProperty</code> method.
   */
  static const QString FRAMEWORK_VERSION; // = "org.commontk.pluginfw.version"

  /**
   * Framework environment property identifying the Framework implementation
   * vendor.
   *
   * <p>
   * The value of this property may be retrieved by calling the
   * <code>ctkPluginContext::getProperty</code> method.
   */
  static const QString FRAMEWORK_VENDOR; // = "org.commontk.pluginfw.vendor"

  /**
   * Specifies the persistent storage area used by the framework. The value of
   * this property must be a valid file path in the file system to a
   * directory. If the specified directory does not exist then the framework
   * will create the directory. If the specified path exists but is not a
   * directory or if the framework fails to create the storage directory, then
   * framework initialization must fail. The framework is free to use this
   * directory as it sees fit. This area can not be shared with anything else.
   * <p>
   * If this property is not set, the framework should use a reasonable
   * platform default for the persistent storage area.
   */
  static const QString FRAMEWORK_STORAGE; // = "org.commontk.pluginfw.storage"

  /**
   * Specifies if and when the persistent storage area for the framework
   * should be cleaned. If this property is not set, then the framework
   * storage area must not be cleaned.
   *
   * @see #FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT
   */
  static const QString FRAMEWORK_STORAGE_CLEAN; // = "org.commontk.pluginfw.storage.clean";

  /**
   * Specifies that the framework storage area must be cleaned before the
   * framework is initialized for the first time. Subsequent inits, starts or
   * updates of the framework will not result in cleaning the framework
   * storage area.
   */
  static const QString FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT; // = "onFirstInit";

  /**
   * Specifies the hints on how symbols in dynamic shared objects (plug-ins) are
   * resolved. The value of this property must be of type
   * <a href="http://doc.trolltech.com/4.7/qlibrary.html#LoadHint-enum">QLibrary::LoadHints</a>.
   *
   * Setting this property to QLibrary::ExportExternalSymbolsHint may
   * be necessary on some platforms (e.g. ELF platforms with gcc < 4.5) to get
   * RTTI working across DSO boundaries.
   */
  static const QString FRAMEWORK_PLUGIN_LOAD_HINTS; // = "org.commontk.pluginfw.loadhints"

  /**
   * Specifies the set of libraries which should be dynamically opened when starting
   * the framework. The value of this property must be either of type QString
   * or QStringList. The given libraries are loaded with QLibrary::load(), using the
   * load hints specified in FRAMEWORK_PLUGIN_LOAD_HINTS. The library search path is
   * defined by the QLibrary class.
   *
   * Setting this property can improve the initial framework start-up time dramatically if
   * a lot of plug-ins with deeply nested library dependencies are installed. During
   * initial framework start-up (no plug-in meta-data cached yet), the repeated loading
   * and unloading of the installed plug-ins will then only lead to repeated loading
   * and unloading of plug-in dependencies which are not contained in the transitive
   * dependency closure of the given set of pre-loaded libraries.
   */
  static const QString FRAMEWORK_PRELOAD_LIBRARIES; // = "org.commontk.pluginfw.preloadlibs"

  /**
   * Manifest header identifying the plugin's symbolic name.
   *
   * <p>
   * The attribute value may be retrieved from the <code>QHash</code>
   * object returned by the <code>Plugin::getHeaders()</code> method.
   */
  static const QString PLUGIN_SYMBOLICNAME; // = "Plugin-SymbolicName"

  /**
   * Manifest header identifying the plugin's copyright information.
   * <p>
   * The attribute value may be retrieved from the <code>QHash</code>
   * object returned by the <code>ctkPlugin::getHeaders</code> method.
   */
  static const QString PLUGIN_COPYRIGHT; // = "Plugin-Copyright"

  /**
   * Manifest header containing a brief description of the plugin's
   * functionality.
   * <p>
   * The attribute value may be retrieved from the <code>QHash</code>
   * object returned by the <code>ctkPlugin::getHeaders</code> method.
   */
  static const QString PLUGIN_DESCRIPTION; // = "Plugin-Description"

  /**
   * Manifest header identifying the plugin's name.
   * <p>
   * The attribute value may be retrieved from the <code>QHash</code>
   * object returned by the <code>ctkPlugin::getHeaders</code> method.
   */
  static const QString PLUGIN_NAME; // = "Plugin-Name"

  /**
   * Manifest header identifying the plugin's vendor.
   *
   * <p>
   * The header value may be retrieved from the <code>QHash</code> object
   * returned by the <code>ctkPlugin::getHeaders</code> method.
   */
  static const QString PLUGIN_VENDOR; // = "Plugin-Vendor"

  /**
   * Manifest header identifying the base name of the plugin's Qt .qm
   * files.
   *
   * <p>
   * The attribute value may be retrieved from the <code>QHash</code>
   * object returned by the <code>ctkPlugin::getHeaders</code> method.
   *
   * @see #PLUGIN_LOCALIZATION_DEFAULT_BASENAME
   */
  static const QString PLUGIN_LOCALIZATION; // = "Plugin-Localization"

  /**
   * Default value for the <code>Plugin-Localization</code> manifest header.
   *
   * @see #PLUGIN_LOCALIZATION
   */
  static const QString PLUGIN_LOCALIZATION_DEFAULT_BASENAME; // = "CTK-INF/l10n/plugin"

  /**
   * Manifest header identifying the symbolic names of other plugins required
   * by the plugin.
   *
   * <p>
   * The attribute value may be retrieved from the <code>QHash</code>
   * object returned by the <code>ctkPlugin::getHeaders</code> method.
   *
   */
  static const QString REQUIRE_PLUGIN; // = "Require-Plugin"

  /**
   * Manifest header attribute identifying a range of versions for a plugin
   * specified in the <code>Require-Plugin</code> manifest headers.
   * The default value is <code>0.0.0</code>.
   *
   * <p>
   * The attribute value is encoded in the Require-Plugin manifest header
   * like:
   *
   * <pre>
   *     Require-Plugin: com.acme.module.test; plugin-version=&quot;1.1&quot;
   *     Require-Plugin: com.acme.module.test; plugin-version=&quot;[1.0,2.0)&quot;
   * </pre>
   *
   * <p>
   * The plugin-version attribute value uses a mathematical interval notation
   * to specify a range of plugin versions. A plugin-version attribute value
   * specified as a single version means a version range that includes any
   * plugin version greater than or equal to the specified version.
   *
   * @see #REQUIRE_PLUGIN
   */
  static const QString PLUGIN_VERSION_ATTRIBUTE; // = "plugin-version"


  /**
   * Manifest header identifying the plugin's version.
   *
   * <p>
   * The attribute value may be retrieved from the <code>QHash</code>
   * object returned by the <code>ctkPlugin::getHeaders()</code> method.
   */
  static const QString PLUGIN_VERSION; // = "Plugin-Version"

  /**
   * Manifest header identifying the plugin's activation policy.
   * <p>
   * The attribute value may be retrieved from the <code>QHash</code>
   * object returned by the <code>Plugin::getHeaders()</code> method.
   *
   * @see #ACTIVATION_EAGER
   */
  static const QString PLUGIN_ACTIVATIONPOLICY; // = "Plugin-ActivationPolicy"

  /**
   * Manifest header identifying the location from which a new plugin version
   * is obtained during a plugin update operation.
   *
   * <p>
   * The attribute value may be retrieved from the <code>ctkDictionary</code>
   * object returned by the <code>ctkPlugin::getHeaders()</code> method.
   */
  static const QString PLUGIN_UPDATELOCATION; // = "Plugin-UpdateLocation"

  /**
   * Plugin activation policy declaring the plugin must be activated immediately.
   *
   * <p>
   * A plugin with the eager activation policy that is started with the
   * ctkPlugin#START_ACTIVATION_POLICY option will be activating immediately when
   * the framework is started.
   * <p>
   * The activation policy value is specified as in the
   * Plugin-ActivationPolicy manifest header like:
   *
   * <pre>
   *       Plugin-ActivationPolicy: eager
   * </pre>
   *
   * @see #PLUGIN_ACTIVATIONPOLICY
   * @see ctkPlugin#start(int)
   * @see ctkPlugin#START_ACTIVATION_POLICY
   */
  static const QString ACTIVATION_EAGER; // = "eager"

  /**
   * Plugin activation policy declaring the plugin must be activated when the
   * first object is instantiated from the plugin.
   * <p>
   * A plugin with the lazy activation policy that is started with the
   * {@link ctkPlugin#START_ACTIVATION_POLICY START_ACTIVATION_POLICY} option
   * will wait in the ctkPlugin#STARTING state until the first
   * class instantiation from the plugin occurs. The plugin will then be activated
   * before the instance is returned to the requester.
   * <p>
   * The lazy activation policy is the default for every plugin.
   *
   * @see #PLUGIN_ACTIVATIONPOLICY
   * @see ctkPlugin#start(int)
   * @see ctkPlugin#START_ACTIVATION_POLICY
   */
  static const QString ACTIVATION_LAZY; // = "lazy"

  /**
   * Manifest header directive identifying the resolution type in the
   * Require-Plugin manifest header. The default value is
   * {@link #RESOLUTION_MANDATORY mandatory}.
   *
   * <p>
   * The directive value is encoded in the Require-Plugin
   * manifest header like:
   *
   * <pre>
   *     Require-Plugin: com.acme.module.test; resolution:=&quot;optional&quot;
   * </pre>
   *
   * @see #REQUIRE_PLUGIN
   * @see #RESOLUTION_MANDATORY
   * @see #RESOLUTION_OPTIONAL
   */
  static const QString RESOLUTION_DIRECTIVE; // = "resolution"

  /**
   * Manifest header directive value identifying a mandatory resolution type.
   * A mandatory resolution type indicates that the required plugin
   * must be resolved when the plugin is resolved. If such a
   * require plugin cannot be resolved, the module fails to resolve.
   *
   * <p>
   * The directive value is encoded in the Require-Plugin
   * manifest header like:
   *
   * <pre>
   *     Require-Plugin: com.acme.module.test; resolution:=&quot;manditory&quot;
   * </pre>
   *
   * @see #RESOLUTION_DIRECTIVE
   */
  static const QString RESOLUTION_MANDATORY; // = "mandatory"

  /**
   * Manifest header directive value identifying an optional resolution type.
   * An optional resolution type indicates that the require plugin
   * is optional and the plugin may be resolved without the require
   * plugin being resolved. If the require plugin is not resolved
   * when the plugin is resolved, therequire plugin may not be
   * resolved before the plugin is refreshed.
   *
   * <p>
   * The directive value is encoded in the Require-Plugin
   * manifest header like:
   *
   * <pre>
   *     Require-Plugin: com.acme.module.test; resolution:=&quot;optional&quot;
   * </pre>
   *
   * @see #RESOLUTION_DIRECTIVE
   */
  static const QString RESOLUTION_OPTIONAL; // = "optional"

  /**
   * Service property identifying all of the class names under which a service
   * was registered in the Framework. The value of this property must be of
   * type <code>QStringList</code>.
   *
   * <p>
   * This property is set by the Framework when a service is registered.
   */
  static const QString OBJECTCLASS; // = "objectclass"

  /**
   * Service property identifying a service's registration number. The value
   * of this property must be of type <code>qlonglong</code>.
   *
   * <p>
   * The value of this property is assigned by the Framework when a service is
   * registered. The Framework assigns a unique value that is larger than all
   * previously assigned values since the Framework was started. These values
   * are NOT persistent across restarts of the Framework.
   */
  // ATTENTION!!! If the value is changed, change also ctkEventConstants::SERVICE_ID
  static const QString SERVICE_ID; // = "service.id"

  /**
   * Service property identifying a service's persistent identifier.
   *
   * <p>
   * This property may be supplied in the <code>properties</code>
   * <code>ctkDictionary</code> object passed to the
   * <code>ctkPluginContext#registerService</code> method. The value of this
   * property must be of type <code>QString</code> or <code>QStringList</code>.
   *
   * <p>
   * A service's persistent identifier uniquely identifies the service and
   * persists across multiple Framework invocations.
   *
   * <p>
   * By convention, every plugin has its own unique namespace, starting with
   * the plugin's identifier (see ctkPlugin::getPluginId) and followed by
   * a dot (.). A plugin may use this as the prefix of the persistent
   * identifiers for the services it registers.
   */
  // ATTENTION!!! If the value is changed, change also ctkEventConstants::SERVICE_PID
  static const QString SERVICE_PID; // = "service.pid"

  /**
   * Service property identifying a service's ranking number.
   *
   * <p>
   * This property may be supplied in the
   * <code>ctkDictionary</code> object passed to the
   * <code>ctkPluginContext::registerService</code> method. The value of this
   * property must be of type <code>int</code>.
   *
   * <p>
   * The service ranking is used by the Framework to determine the <i>natural
   * order</i> of services, see ctkServiceReference::operator<(const ctkServiceReference&),
   * and the <i>default</i> service to be returned from a call to the
   * {@link ctkPluginContext::getServiceReference} method.
   *
   * <p>
   * The default ranking is zero (0). A service with a ranking of
   * <code>std::numeric_limits<int>::max()</code> is very likely to be returned as the
   * default service, whereas a service with a ranking of
   * <code>std::numeric_limits<int>::min()</code> is very unlikely to be returned.
   *
   * <p>
   * If the supplied property value is not of type <code>int</code>, it is
   * deemed to have a ranking value of zero.
   */
  static const QString SERVICE_RANKING; // = "service.ranking"

  /**
   * Service property identifying a service's vendor.
   *
   * <p>
   * This property may be supplied in the properties <code>ctkDictionary</code> object
   * passed to the <code>ctkPluginContext::registerService()</code> method.
   */
  static const QString SERVICE_VENDOR; // = "service.vendor"

  /**
   * Service property identifying a service's description.
   *
   * <p>
   * This property may be supplied in the properties <code>ctkDictionary</code> object
   * passed to the <code>ctkPluginContext::registerService()</code> method.
   */
  static const QString SERVICE_DESCRIPTION; // = "service.description"

};

#include <QLibrary>
#include <QMetaType>
Q_DECLARE_METATYPE(QLibrary::LoadHints)


#endif // CTKPLUGINCONSTANTS_H
