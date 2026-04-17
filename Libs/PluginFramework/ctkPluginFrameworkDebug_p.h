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

// Qt includes
#include <QLatin1String>

#include "ctkPluginFramework_global.h"

/**
 * Variables that control debugging of the pluginfw code.
 */
class ctkPluginFrameworkDebug
{

public:
  ctkPluginFrameworkDebug();

  static constexpr QLatin1String OPTION_DEBUG_GENERAL{
      "org.commontk.pluginfw/debug",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug") - 1)};
  bool enabled;

  /**
   * Report error handling events.
   */
  static constexpr QLatin1String OPTION_DEBUG_ERRORS{
      "org.commontk.pluginfw/debug/errors",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/errors") - 1)};
  bool errors;

  /**
   * Report pluginfw create, init, start, stop
   */
  static constexpr QLatin1String OPTION_DEBUG_FRAMEWORK{
      "org.commontk.pluginfw/debug/framework",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/framework") - 1)};
  bool framework;

  /**
   * Report hooks handling
   */
  static constexpr QLatin1String OPTION_DEBUG_HOOKS{
      "org.commontk.pluginfw/debug/hooks",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/hooks") - 1)};
  bool hooks;

  /**
   * Report triggering of lazy activation
   */
  static constexpr QLatin1String OPTION_DEBUG_LAZY_ACTIVATION{
      "org.commontk.pluginfw/debug/lazy_activation",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/lazy_activation") - 1)};
  bool lazy_activation;

  /**
   * Report LDAP handling
   */
  static constexpr QLatin1String OPTION_DEBUG_LDAP{
      "org.commontk.pluginfw/debug/ldap",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/ldap") - 1)};
  bool ldap;

  /**
   * Print information about service reference lookups
   * and rejections due to missing permissions
   * for calling plug-ins.
   */
  static constexpr QLatin1String OPTION_DEBUG_SERVICE_REFERENCE{
      "org.commontk.pluginfw/debug/service_reference",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/service_reference") - 1)};
  bool service_reference;

  /**
   * Report startlevel.
   */
  static constexpr QLatin1String OPTION_DEBUG_STARTLEVEL{
      "org.commontk.pluginfw/debug/startlevel",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/startlevel") - 1)};
  bool startlevel;

  /**
   * Report url
   */
  static constexpr QLatin1String OPTION_DEBUG_URL{
      "org.commontk.pluginfw/debug/url",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/url") - 1)};
  bool url;

  /**
   * Report plug-in resolve progress
   */
  static constexpr QLatin1String OPTION_DEBUG_RESOLVE{
      "org.commontk.pluginfw/debug/resolve",
      static_cast<int>(sizeof("org.commontk.pluginfw/debug/resolve") - 1)};
  bool resolve;

};

#endif // CTKPLUGINFRAMEWORKDEBUG_P_H
