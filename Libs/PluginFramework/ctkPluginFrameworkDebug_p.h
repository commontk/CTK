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
  ctkPluginFrameworkDebug();

  static QString OPTION_DEBUG_GENERAL;
  bool enabled;

  /**
   * Report error handling events.
   */
  static QString OPTION_DEBUG_ERRORS;
  bool errors;

  /**
   * Report pluginfw create, init, start, stop
   */
  static QString OPTION_DEBUG_FRAMEWORK;
  bool framework;

  /**
   * Report hooks handling
   */
  static QString OPTION_DEBUG_HOOKS;
  bool hooks;

  /**
   * Report triggering of lazy activation
   */
  static QString OPTION_DEBUG_LAZY_ACTIVATION;
  bool lazy_activation;

  /**
   * Report LDAP handling
   */
  static QString OPTION_DEBUG_LDAP;
  bool ldap;

  /**
   * Print information about service reference lookups
   * and rejections due to missing permissions
   * for calling plug-ins.
   */
  static QString OPTION_DEBUG_SERVICE_REFERENCE;
  bool service_reference;

  /**
   * Report startlevel.
   */
  static QString OPTION_DEBUG_STARTLEVEL;
  bool startlevel;

  /**
   * Report url
   */
  static QString OPTION_DEBUG_URL;
  bool url;

  /**
   * Report plug-in resolve progress
   */
  static QString OPTION_DEBUG_RESOLVE;
  bool resolve;

};

#endif // CTKPLUGINFRAMEWORKDEBUG_P_H
