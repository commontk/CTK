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

namespace {
/// Construct a QLatin1String from a string literal, deducing length at
/// compile time via the array-reference parameter.  Avoids repeating
/// sizeof/static_cast at every definition site.
template<std::size_t N>
constexpr QLatin1String make_constexpr_QLatin1String_(const char (&str)[N])
{
  return QLatin1String{str, static_cast<int>(N - 1)};
}
} // namespace

#define CTK_OSGI "org.commontk.pluginfw"

/**
 * Variables that control debugging of the pluginfw code.
 */
class ctkPluginFrameworkDebug
{

public:
  ctkPluginFrameworkDebug();

  static constexpr QLatin1String OPTION_DEBUG_GENERAL         = make_constexpr_QLatin1String_(CTK_OSGI "/debug");
  bool enabled;

  /**
   * Report error handling events.
   */
  static constexpr QLatin1String OPTION_DEBUG_ERRORS          = make_constexpr_QLatin1String_(CTK_OSGI "/debug/errors");
  bool errors;

  /**
   * Report pluginfw create, init, start, stop
   */
  static constexpr QLatin1String OPTION_DEBUG_FRAMEWORK       = make_constexpr_QLatin1String_(CTK_OSGI "/debug/framework");
  bool framework;

  /**
   * Report hooks handling
   */
  static constexpr QLatin1String OPTION_DEBUG_HOOKS           = make_constexpr_QLatin1String_(CTK_OSGI "/debug/hooks");
  bool hooks;

  /**
   * Report triggering of lazy activation
   */
  static constexpr QLatin1String OPTION_DEBUG_LAZY_ACTIVATION = make_constexpr_QLatin1String_(CTK_OSGI "/debug/lazy_activation");
  bool lazy_activation;

  /**
   * Report LDAP handling
   */
  static constexpr QLatin1String OPTION_DEBUG_LDAP            = make_constexpr_QLatin1String_(CTK_OSGI "/debug/ldap");
  bool ldap;

  /**
   * Print information about service reference lookups
   * and rejections due to missing permissions
   * for calling plug-ins.
   */
  static constexpr QLatin1String OPTION_DEBUG_SERVICE_REFERENCE = make_constexpr_QLatin1String_(CTK_OSGI "/debug/service_reference");
  bool service_reference;

  /**
   * Report startlevel.
   */
  static constexpr QLatin1String OPTION_DEBUG_STARTLEVEL      = make_constexpr_QLatin1String_(CTK_OSGI "/debug/startlevel");
  bool startlevel;

  /**
   * Report url
   */
  static constexpr QLatin1String OPTION_DEBUG_URL             = make_constexpr_QLatin1String_(CTK_OSGI "/debug/url");
  bool url;

  /**
   * Report plug-in resolve progress
   */
  static constexpr QLatin1String OPTION_DEBUG_RESOLVE         = make_constexpr_QLatin1String_(CTK_OSGI "/debug/resolve");
  bool resolve;

};

#undef CTK_OSGI

#endif // CTKPLUGINFRAMEWORKDEBUG_P_H
