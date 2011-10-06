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

QString ctkPluginFrameworkDebug::ERRORS_PROP = "org.commontk.pluginfw.debug.errors";
QString ctkPluginFrameworkDebug::FRAMEWORK_PROP = "org.commontk.pluginfw.debug.pluginfw";
QString ctkPluginFrameworkDebug::HOOKS_PROP = "org.commontk.pluginfw.debug.hooks";
QString ctkPluginFrameworkDebug::LAZY_ACTIVATION_PROP = "org.commontk.pluginfw.debug.lazy_activation";
QString ctkPluginFrameworkDebug::LDAP_PROP = "org.commontk.pluginfw.debug.ldap";
QString ctkPluginFrameworkDebug::SERVICE_REFERENCE_PROP = "org.commontk.pluginfw.debug.service_reference";
QString ctkPluginFrameworkDebug::STARTLEVEL_PROP = "org.commontk.pluginfw.debug.startlevel";
QString ctkPluginFrameworkDebug::URL_PROP = "org.commontk.pluginfw.debug.url";
QString ctkPluginFrameworkDebug::RESOLVE_PROP = "org.commontk.pluginfw.debug.resolve";

//----------------------------------------------------------------------------
ctkPluginFrameworkDebug::ctkPluginFrameworkDebug(ctkProperties& props)
{
  setPropertyIfNotSet(props, ERRORS_PROP, false);
  setPropertyIfNotSet(props, FRAMEWORK_PROP, false);
  setPropertyIfNotSet(props, HOOKS_PROP, false);
  setPropertyIfNotSet(props, LAZY_ACTIVATION_PROP, false);
  setPropertyIfNotSet(props, LDAP_PROP, false);
  setPropertyIfNotSet(props, SERVICE_REFERENCE_PROP, false);
  setPropertyIfNotSet(props, STARTLEVEL_PROP, false);
  setPropertyIfNotSet(props, URL_PROP, false);
  setPropertyIfNotSet(props, RESOLVE_PROP, false);
  errors = props.value(ERRORS_PROP).toBool();
  framework = props.value(FRAMEWORK_PROP).toBool();
  hooks = props.value(HOOKS_PROP).toBool();
  lazy_activation = props.value(LAZY_ACTIVATION_PROP).toBool();
  ldap = props.value(LDAP_PROP).toBool();
  service_reference = props.value(SERVICE_REFERENCE_PROP).toBool();
  startlevel = props.value(STARTLEVEL_PROP).toBool();
  url = props.value(URL_PROP).toBool();
  resolve = props.value(RESOLVE_PROP).toBool();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkDebug::setPropertyIfNotSet(ctkProperties& props, const QString& key, const QVariant& val)
{
  if (!props.contains(key))
  {
    props.insert(key, val);
  }
}
