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


#ifndef CTKPLUGINGENERATORCONSTANTS_H
#define CTKPLUGINGENERATORCONSTANTS_H

#include <QString>

#include <org_commontk_plugingenerator_core_Export.h>

struct org_commontk_plugingenerator_core_EXPORT ctkPluginGeneratorConstants
{

  // project wide template markers
  static const QString PLUGIN_LICENSE_MARKER;
  static const QString PLUGIN_EXPORTMACRO_MARKER;
  static const QString PLUGIN_NAMESPACE_MARKER;

  static QStringList getGlobalMarkers();

  // template names
  static const QString TEMPLATE_CMAKELISTS_TXT;
  static const QString TEMPLATE_PLUGINACTIVATOR_H;
  static const QString TEMPLATE_PLUGINACTIVATOR_CPP;

};

#endif // CTKPLUGINGENERATORCONSTANTS_H
