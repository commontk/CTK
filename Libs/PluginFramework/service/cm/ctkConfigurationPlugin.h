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


#ifndef CTKCONFIGURATIONPLUGIN_H
#define CTKCONFIGURATIONPLUGIN_H

#include "ctkServiceReference.h"
#include "ctkDictionary.h"

/**
 * \ingroup ConfigAdmin
 *
 * A service interface for processing configuration dictionary before the
 * update.
 *
 * <p>
 * A plugin registers a <code>ctkConfigurationPlugin</code> object in order to
 * process configuration updates before they reach the Managed Service or
 * Managed Service Factory. The Configuration Admin service will detect
 * registrations of Configuration Plugin services and must call these services
 * every time before it calls the <code>ctkManagedService</code> or
 * <code>ctkManagedServiceFactory</code>
 * <code>updated</code> method. The
 * Configuration Plugin service thus has the opportunity to view and modify the
 * properties before they are passed to the Managed Service or Managed Service
 * Factory.
 *
 * <p>
 * Configuration Plugin (plugin) services have full read/write access to all
 * configuration information. Therefore, plugins using this facility should be
 * trusted. Access to this facility should be limited with
 * <code>ctkServicePermission[ctkConfigurationPlugin,REGISTER]</code>.
 * Implementations of a Configuration Plugin service should assure that they
 * only act on appropriate configurations.
 *
 * <p>
 * The integer <code>service.cmRanking</code> registration
 * property may be specified. Not specifying this registration property, or
 * setting it to something other than an integer, is the same as
 * setting it to the integer zero. The
 * <code>service.cmRanking</code> property determines the order in which
 * plugins are invoked. Lower ranked plugins are called before higher ranked
 * ones. In the event of more than one plugin having the same value of
 * <code>service.cmRanking</code>, then the Configuration Admin service
 * arbitrarily chooses the order in which they are called.
 *
 * <p>
 * By convention, plugins with <code>service.cmRanking&lt; 0</code> or
 * <code>service.cmRanking &gt; 1000</code> should not make modifications to
 * the properties.
 *
 * <p>
 * The Configuration Admin service has the right to hide properties from
 * plugins, or to ignore some or all the changes that they make. This might be
 * done for security reasons. Any such behavior is entirely implementation
 * defined.
 *
 * <p>
 * A plugin may optionally specify a <code>cm.target</code> registration
 * property whose value is the PID of the Managed Service or Managed Service
 * Factory whose configuration updates the plugin is intended to intercept. The
 * plugin will then only be called with configuration updates that are targeted
 * at the Managed Service or Managed Service Factory with the specified PID.
 * Omitting the <code>cm.target</code> registration property means that the
 * plugin is called for all configuration updates.
 *
 */
struct CTK_PLUGINFW_EXPORT ctkConfigurationPlugin
{
  virtual ~ctkConfigurationPlugin() {};

  /**
   * A service property to limit the Managed Service or Managed Service
   * Factory configuration dictionaries a Configuration Plugin service
   * receives.
   *
   * This property contains a <code>QStringList</code> of PIDs. A Configuration
   * Admin service must call a Configuration Plugin service only when this
   * property is not set, or the target service's PID is listed in this
   * property.
   */
  static const QString CM_TARGET; // = "cm.target"

  /**
   * A service property to specify the order in which plugins are invoked.
   *
   * This property contains an integer ranking of the plugin.
   * Not specifying this registration property, or setting it to something
   * other than an integer, is the same as setting it to the
   * integer zero. This property determines the order in which
   * plugins are invoked. Lower ranked plugins are called before higher ranked
   * ones.
   */
  static const QString CM_RANKING; // = "service.cmRanking"

  /**
   * View and possibly modify a set of configuration properties before
   * they are sent to the Managed Service or the Managed Service Factory. The
   * Configuration Plugin services are called in increasing order of their
   * <code>service.cmRanking</code> property. If this property is undefined
   * or is a non-integer type, 0 is used.
   *
   * <p>
   * This method should not modify the properties unless the
   * <code>service.cmRanking</code> of this plugin is in the range
   * <code>0 &lt;= service.cmRanking &lt;= 1000</code>.
   * <p>
   * If this method throws any exception, the Configuration
   * Admin service must catch it and should log it.
   *
   * @param reference reference to the Managed Service or Managed Service
   *        Factory
   * @param properties The configuration properties. This argument must not
   *        contain the "service.pluginLocation" property. The value of this
   *        property may be obtained from the
   *        <code>ctkConfiguration#getPluginLocation</code> method.
   */
  virtual void modifyConfiguration(const ctkServiceReference& reference,
                                   ctkDictionary& properties) = 0;
};

Q_DECLARE_INTERFACE(ctkConfigurationPlugin, "org.commontk.service.cm.ConfigurationPlugin")

#endif // CTKCONFIGURATIONPLUGIN_H
