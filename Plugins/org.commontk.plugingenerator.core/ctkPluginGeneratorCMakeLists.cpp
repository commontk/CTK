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


#include "ctkPluginGeneratorCMakeLists.h"

#include "ctkPluginGeneratorConstants.h"

#include <QTextStream>

const QString ctkPluginGeneratorCMakeLists::PLUGIN_PROJECT_NAME_MARKER = "plugin_project_name";
const QString ctkPluginGeneratorCMakeLists::PLUGIN_SRCS_MARKER = "plugin_srcs";
const QString ctkPluginGeneratorCMakeLists::PLUGIN_MOC_SRCS_MARKER = "plugin_moc_srcs";
const QString ctkPluginGeneratorCMakeLists::PLUGIN_RESOURCES_MARKER = "plugin_resources";
const QString ctkPluginGeneratorCMakeLists::PLUGIN_UI_FORMS_MARKER = "plugin_ui_forms";

ctkPluginGeneratorCMakeLists::ctkPluginGeneratorCMakeLists(ctkPluginGeneratorAbstractTemplate *parent) :
    ctkPluginGeneratorAbstractTemplate("CMakeLists.txt", parent)
{
}

QString ctkPluginGeneratorCMakeLists::generateContent()
{
  QString content;
  QTextStream stream(&content);

  stream
    << "PROJECT(" << this->getContent(PLUGIN_PROJECT_NAME_MARKER).front() << ")\n\n"
    << "SET(PLUGIN_export_directive \"" << this->getContent(ctkPluginGeneratorConstants::PLUGIN_EXPORTMACRO_MARKER).front() << "\")\n\n"
    << "SET(PLUGIN_SRCS\n";

  QStringList markerContent = this->getContent(PLUGIN_SRCS_MARKER);
  markerContent.sort();
  for (QStringListIterator it(markerContent); it.hasNext();)
  {
    stream << "  " << it.next() << "\n";
  }

  stream
    << ")\n\n"
    << "# Files which should be processed by Qts moc\n"
    << "SET(PLUGIN_MOC_SRCS\n";

  markerContent = this->getContent(PLUGIN_MOC_SRCS_MARKER);
  markerContent.sort();
  for (QStringListIterator it(markerContent); it.hasNext();)
  {
    stream << "  " << it.next() << "\n";
  }

  stream
    << ")\n\n"
    << "# Qt Designer files which should be processed by Qts uic\n"
    << "SET(PLUGIN_UI_FORMS\n";

  markerContent = this->getContent(PLUGIN_UI_FORMS_MARKER);
  markerContent.sort();
  for (QStringListIterator it(markerContent); it.hasNext();)
  {
    stream << "  " << it.next() << "\n";
  }

  stream
    << ")\n\n"
    << "# QRC Files which should be compiled into the plugin\n"
    << "SET(PLUGIN_resources\n";

  markerContent = this->getContent(PLUGIN_RESOURCES_MARKER);
  markerContent.sort();
  for (QStringListIterator it(markerContent); it.hasNext();)
  {
    stream << "  " << it.next() << "\n";
  }

  stream
    << ")\n\n"
    << "#Compute the plugin dependencies\n"
    << "ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)\n\n"
    << "ctkMacroBuildPlugin(\n"
    << "  NAME ${PROJECT_NAME}\n"
    << "  EXPORT_DIRECTIVE ${PLUGIN_export_directive}\n"
    << "  SRCS ${PLUGIN_SRCS}\n"
    << "  MOC_SRCS ${PLUGIN_MOC_SRCS}\n"
    << "  UI_FORMS ${PLUGIN_UI_FORMS}\n"
    << "  RESOURCES ${PLUGIN_resources}\n"
    << "  TARGET_LIBRARIES ${PLUGIN_target_libraries}\n"
    << ")\n";

  return content;
}

QStringList ctkPluginGeneratorCMakeLists::getMarkers() const
{
  QStringList markers = ctkPluginGeneratorAbstractTemplate::getMarkers();

  markers << PLUGIN_PROJECT_NAME_MARKER
      << PLUGIN_SRCS_MARKER
      << PLUGIN_MOC_SRCS_MARKER
      << PLUGIN_RESOURCES_MARKER;

  return markers;
}
