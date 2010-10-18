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


#ifndef CTKPLUGINGENERATORCMAKELISTS_H
#define CTKPLUGINGENERATORCMAKELISTS_H

#include "ctkPluginGeneratorAbstractTemplate.h"

class org_commontk_plugingenerator_core_EXPORT ctkPluginGeneratorCMakeLists
  : public ctkPluginGeneratorAbstractTemplate
{

public:

  static const QString PLUGIN_PROJECT_NAME_MARKER;
  static const QString PLUGIN_SRCS_MARKER;
  static const QString PLUGIN_MOC_SRCS_MARKER;
  static const QString PLUGIN_RESOURCES_MARKER;
  static const QString PLUGIN_UI_FORMS_MARKER;

  ctkPluginGeneratorCMakeLists(ctkPluginGeneratorAbstractTemplate *parent = 0);

  QString generateContent();

  QStringList getMarkers() const;

};

#endif // CTKPLUGINGENERATORCMAKELISTS_H
