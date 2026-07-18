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

// CTK includes
#include <ctkStringLiterals.h>

#include "ctkPluginFramework_global.h"

using namespace ctk::string_literals;

#define CTK_OSGI "org.commontk.pluginfw"

/**
 * Variables that control debugging of the pluginfw code.
 */
class ctkPluginFrameworkDebug
{

public:
  ctkPluginFrameworkDebug();

  static constexpr QLatin1String OPTION_DEBUG_GENERAL         = CTK_OSGI "/debug"_L1;
  bool enabled;

  /**
   * Report error handling events.
   */
  static constexpr QLatin1String OPTION_DEBUG_ERRORS          = CTK_OSGI "/debug/errors"_L1;
  bool errors;

  /**
   * Report pluginfw create, init, start, stop
   */
  static constexpr QLatin1String OPTION_DEBUG_FRAMEWORK       = CTK_OSGI "/debug/framework"_L1;
  bool framework;

  /**
   * Report hooks handling
   */
  static constexpr QLatin1String OPTION_DEBUG_HOOKS           = CTK_OSGI "/debug/hooks"_L1;
  bool hooks;

  /**
   * Report triggering of lazy activation
   */
  static constexpr QLatin1String OPTION_DEBUG_LAZY_ACTIVATION = CTK_OSGI "/debug/lazy_activation"_L1;
  bool lazy_activation;

  /**
   * Report LDAP handling
   */
  static constexpr QLatin1String OPTION_DEBUG_LDAP            = CTK_OSGI "/debug/ldap"_L1;
  bool ldap;

  /**
   * Print information about service reference lookups
   * and rejections due to missing permissions
   * for calling plug-ins.
   */
  static constexpr QLatin1String OPTION_DEBUG_SERVICE_REFERENCE = CTK_OSGI "/debug/service_reference"_L1;
  bool service_reference;

  /**
   * Report startlevel.
   */
  static constexpr QLatin1String OPTION_DEBUG_STARTLEVEL      = CTK_OSGI "/debug/startlevel"_L1;
  bool startlevel;

  /**
   * Report url
   */
  static constexpr QLatin1String OPTION_DEBUG_URL             = CTK_OSGI "/debug/url"_L1;
  bool url;

  /**
   * Report plug-in resolve progress
   */
  static constexpr QLatin1String OPTION_DEBUG_RESOLVE         = CTK_OSGI "/debug/resolve"_L1;
  bool resolve;

};

#undef CTK_OSGI

#endif // CTKPLUGINFRAMEWORKDEBUG_P_H
