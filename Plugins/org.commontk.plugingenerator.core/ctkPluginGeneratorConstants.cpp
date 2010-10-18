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


#include "ctkPluginGeneratorConstants.h"

#include <QStringList>

const QString ctkPluginGeneratorConstants::PLUGIN_LICENSE_MARKER = "plugin_license";
const QString ctkPluginGeneratorConstants::PLUGIN_EXPORTMACRO_MARKER = "plugin_exportmacro";
const QString ctkPluginGeneratorConstants::PLUGIN_NAMESPACE_MARKER = "plugin_namespace";

QStringList ctkPluginGeneratorConstants::getGlobalMarkers()
{
  return QStringList()
      << PLUGIN_LICENSE_MARKER
      << PLUGIN_EXPORTMACRO_MARKER
      << PLUGIN_NAMESPACE_MARKER;
}

const QString ctkPluginGeneratorConstants::TEMPLATE_CMAKELISTS_TXT = "CMakeLists.txt";
const QString ctkPluginGeneratorConstants::TEMPLATE_PLUGINACTIVATOR_H = "PluginActivator.h";
const QString ctkPluginGeneratorConstants::TEMPLATE_PLUGINACTIVATOR_CPP = "PluginActivator.cpp";
