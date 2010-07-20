/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include <QTextStream>

const QString ctkPluginGeneratorCMakeLists::PLUGIN_PROJECT_NAME_MARKER = "plugin_project_name";
const QString ctkPluginGeneratorCMakeLists::PLUGIN_EXPORT_DIRECTIVE_MARKER = "plugin_export_directive";
const QString ctkPluginGeneratorCMakeLists::PLUGIN_SRCS_MARKER = "plugin_srcs";
const QString ctkPluginGeneratorCMakeLists::PLUGIN_MOC_SRCS_MARKER = "plugin_moc_srcs";
const QString ctkPluginGeneratorCMakeLists::PLUGIN_RESOURCES_MARKER = "plugin_resources";

ctkPluginGeneratorCMakeLists::ctkPluginGeneratorCMakeLists(QObject *parent) :
    ctkPluginGeneratorAbstractTemplate("CMakeLists.txt", parent)
{
}

QString ctkPluginGeneratorCMakeLists::generateContent()
{
  QString content;
  QTextStream stream(&content);

  stream
    << "PROJECT(" << this->getContent(PLUGIN_PROJECT_NAME_MARKER).front() << ")\n\n"
    << "SET(PLUGIN_export_directive \"" << this->getContent(PLUGIN_EXPORT_DIRECTIVE_MARKER).front() << "\")\n\n"
    << "SET(PLUGIN_SRCS\n";

  for (QStringListIterator it(this->getContent(PLUGIN_SRCS_MARKER)); it.hasNext();)
  {
    stream << "  " << it.next() << "\n";
  }

  stream
    << ")\n\n"
    << "# Files which should be processed my Qts moc\n"
    << "SET(PLUGIN_MOC_SRCS\n";

  for (QStringListIterator it(this->getContent(PLUGIN_MOC_SRCS_MARKER)); it.hasNext();)
  {
    stream << "  " << it.next() << "\n";
  }

  stream
    << ")\n\n"
    << "# QRC Files which should be compiled into the plugin\n"
    << "SET(PLUGIN_resources\n";

  for (QStringListIterator it(this->getContent(PLUGIN_RESOURCES_MARKER)); it.hasNext();)
  {
    stream << "  " << it.next() << "\n";
  }

  stream
    << ")\n\n"
    << "#Compute the library dependencies\n"
    << "ctkMacroGetTargetLibraries(PLUGIN_target_libraries)\n\n"
    << "ctkMacroBuildPlugin(\n"
    << "  NAME ${PROJECT_NAME}\n"
    << "  EXPORT_DIRECTIVE ${PLUGIN_export_directive}\n"
    << "  SRCS ${PLUGIN_SRCS}\n"
    << "  MOC_SRCS ${PLUGIN_MOC_SRCS}\n"
    << "  RESOURCES ${PLUGIN_resources}\n"
    << "  TARGET_LIBRARIES ${PLUGIN_target_libraries}\n"
    << ")\n";

  return content;
}

QStringList ctkPluginGeneratorCMakeLists::getMarkers() const
{
  QStringList markers;
  markers << PLUGIN_PROJECT_NAME_MARKER
      << PLUGIN_EXPORT_DIRECTIVE_MARKER
      << PLUGIN_SRCS_MARKER
      << PLUGIN_MOC_SRCS_MARKER
      << PLUGIN_RESOURCES_MARKER;
  return markers;
}
