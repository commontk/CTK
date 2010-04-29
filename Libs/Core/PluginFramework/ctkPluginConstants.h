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

#include "CTKCoreExport.h"

namespace ctk {

  struct CTK_CORE_EXPORT PluginConstants {

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




  };

}

#endif // CTKPLUGINCONSTANTS_H
