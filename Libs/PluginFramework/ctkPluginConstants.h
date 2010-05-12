/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "CTKPluginFrameworkExport.h"

namespace ctk {

  struct CTK_PLUGINFW_EXPORT PluginConstants {

    /**
     * Location identifier of the CTK <i>system plugin</i>, which is defined
     * to be &quot;System Plugin&quot;.
     */
    static const QString	SYSTEM_PLUGIN_LOCATION; // = "System Plugin"

    /**
     * Alias for the symbolic name of the CTK <i>system plugin</i>. It is
     * defined to be &quot;system.plugin&quot;.
     *
     */
    static const QString	SYSTEM_PLUGIN_SYMBOLICNAME; // = "system.plugin"


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
     * Manifest header identifying the plugin's symbolic name.
     *
     * <p>
     * The attribute value may be retrieved from the <code>QHash</code>
     * object returned by the <code>Plugin::getHeaders()</code> method.
     */
    static const QString	PLUGIN_SYMBOLICNAME; // = "Plugin-SymbolicName"

    /**
     * Manifest header identifying the plugin's copyright information.
     * <p>
     * The attribute value may be retrieved from the <code>QHash</code>
     * object returned by the <code>Plugin::getHeaders</code> method.
     */
    static const QString PLUGIN_COPYRIGHT; // = "Plugin-Copyright"

    /**
     * Manifest header containing a brief description of the plugin's
     * functionality.
     * <p>
     * The attribute value may be retrieved from the <code>QHash</code>
     * object returned by the <code>Plugin::getHeaders</code> method.
     */
    static const QString PLUGIN_DESCRIPTION; // = "Plugin-Description"

    /**
     * Manifest header identifying the plugin's name.
     * <p>
     * The attribute value may be retrieved from the <code>QHash</code>
     * object returned by the <code>Plugin::getHeaders</code> method.
     */
    static const QString PLUGIN_NAME; // = "Plugin-Name"


    /**
     * Manifest header identifying the base name of the plugin's Qt .qm
     * files.
     *
     * <p>
     * The attribute value may be retrieved from the <code>QHash</code>
     * object returned by the <code>Plugin::getHeaders</code> method.
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
     * object returned by the <code>Plugin::getHeaders</code> method.
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
     * object returned by the <code>Plugin::getHeaders()</code> method.
     */
    static const QString	PLUGIN_VERSION; // = "Plugin-Version"

    /**
     * Manifest header identifying the plugin's activation policy.
     * <p>
     * The attribute value may be retrieved from the <code>QHash</code>
     * object returned by the <code>Plugin::getHeaders()</code> method.
     *
     * @see #ACTIVATION_EAGER
     */
    static const QString	PLUGIN_ACTIVATIONPOLICY; // = "Plugin-ActivationPolicy"

    /**
     * Plugin activation policy declaring the plugin must be activated when the
     * first object is instantiated from the plugin.
     * <p>
     * A plugin with the lazy (default) activation policy
     * will wait in the Plugin#STARTING state until the first
     * class instantiation from the plugin occurs. The plugin will then be activated
     * before the instance is returned to the requester.
     * <p>
     * A plugin with the eager activation policy that is started with the
     * Plugin#START_ACTIVATION_POLICY option will be activating immediately when
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
     * @see Plugin#start(int)
     * @see Plugin#START_ACTIVATION_POLICY
     */
    static const QString	ACTIVATION_EAGER; // = "eager"

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

  };

}

#endif // CTKPLUGINCONSTANTS_H
