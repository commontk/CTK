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


#include "ctkPluginFrameworkDebug_p.h"

#include "ctkPluginFrameworkDebugOptions_p.h"
#include "ctkPluginFrameworkProperties_p.h"

static QString CTK_OSGI = "org.commontk.pluginfw";

QString ctkPluginFrameworkDebug::OPTION_DEBUG_GENERAL = CTK_OSGI + "/debug";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_FRAMEWORK = CTK_OSGI + "/debug/framework";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_ERRORS = CTK_OSGI + "/debug/errors";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_HOOKS = CTK_OSGI + "/debug/hooks";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_LAZY_ACTIVATION = CTK_OSGI + "/debug/lazy_activation";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_LDAP = CTK_OSGI + "/debug/ldap";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_SERVICE_REFERENCE = CTK_OSGI + "/debug/service_reference";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_STARTLEVEL = CTK_OSGI + "/debug/startlevel";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_URL = CTK_OSGI + "/debug/url";
QString ctkPluginFrameworkDebug::OPTION_DEBUG_RESOLVE = CTK_OSGI + "/debug/resolve";

//----------------------------------------------------------------------------
ctkPluginFrameworkDebug::ctkPluginFrameworkDebug()
{
  ctkPluginFrameworkDebugOptions* dbgOptions = ctkPluginFrameworkDebugOptions::getDefault();
  if (dbgOptions != NULL)
  {
    enabled = dbgOptions->isDebugEnabled();
    errors = dbgOptions->getBooleanOption(OPTION_DEBUG_ERRORS, false);
    framework = dbgOptions->getBooleanOption(OPTION_DEBUG_FRAMEWORK, false);
    hooks = dbgOptions->getBooleanOption(OPTION_DEBUG_HOOKS, false);
    lazy_activation = dbgOptions->getBooleanOption(OPTION_DEBUG_LAZY_ACTIVATION, false);
    ldap = dbgOptions->getBooleanOption(OPTION_DEBUG_LDAP, false);
    service_reference = dbgOptions->getBooleanOption(OPTION_DEBUG_SERVICE_REFERENCE, false);
    startlevel = dbgOptions->getBooleanOption(OPTION_DEBUG_STARTLEVEL, false);
    url = dbgOptions->getBooleanOption(OPTION_DEBUG_URL, false);
    resolve = dbgOptions->getBooleanOption(OPTION_DEBUG_RESOLVE, false);
  }
}
