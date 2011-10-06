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


#ifndef CTKPLUGINFRAMEWORKDEBUG_P_H
#define CTKPLUGINFRAMEWORKDEBUG_P_H

#include "ctkPluginFramework_global.h"

/**
 * Variables that control debugging of the pluginfw code.
 */
class ctkPluginFrameworkDebug
{

public:
  ctkPluginFrameworkDebug(ctkProperties& props);

  /**
   * Report error handling events.
   */
  static QString ERRORS_PROP; // = "org.commontk.pluginfw.debug.errors";
  bool errors;

  /**
   * Report pluginfw create, init, start, stop
   */
  static QString FRAMEWORK_PROP; // = "org.commontk.pluginfw.debug.pluginfw";
  bool framework;

  /**
   * Report hooks handling
   */
  static QString HOOKS_PROP; // = "org.commontk.pluginfw.debug.hooks";
  bool hooks;

  /**
   * Report triggering of lazy activation
   */
  static QString LAZY_ACTIVATION_PROP; // = "org.commontk.pluginfw.debug.lazy_activation";
  bool lazy_activation;

  /**
   * Report LDAP handling
   */
  static QString LDAP_PROP; // = "org.commontk.pluginfw.debug.ldap";
  bool ldap;

  /**
   * Print information about service reference lookups
   * and rejections due to missing permissions
   * for calling plug-ins.
   */
  static QString SERVICE_REFERENCE_PROP; // = "org.commontk.pluginfw.debug.service_reference";
  bool service_reference;

  /**
   * Report startlevel.
   */
  static QString STARTLEVEL_PROP; // = "org.commontk.pluginfw.debug.startlevel";
  bool startlevel;

  /**
   * Report url
   */
  static QString URL_PROP; // = "org.commontk.pluginfw.debug.url";
  bool url;

  /**
   * Report plug-in resolve progress
   */
  static QString RESOLVE_PROP; // = "org.commontk.pluginfw.debug.resolve";
  bool resolve;

private:

  void setPropertyIfNotSet(ctkProperties& props, const QString& key, const QVariant& val);
};

#endif // CTKPLUGINFRAMEWORKDEBUG_P_H
