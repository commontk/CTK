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


#include "ctkPluginGeneratorTargetLibraries.h"

#include <QTextStream>

const QString ctkPluginGeneratorTargetLibraries::TARGETLIBRARIES_MARKER = "targetlibraries";

ctkPluginGeneratorTargetLibraries::ctkPluginGeneratorTargetLibraries(ctkPluginGeneratorAbstractTemplate* parent)
  : ctkPluginGeneratorAbstractTemplate("target_libraries.cmake", parent)
{

}

QString ctkPluginGeneratorTargetLibraries::generateContent()
{
  QString content;
  QTextStream stream(&content);

  stream << "# See CMake/ctkFunctionGetTargetLibraries.cmake\n"
      << "#\n"
      << "# This file should list the libraries required to build the current CTK plugin.\n"
      << "# For specifying required plugins, see the manifest_headers.cmake file.\n"
      << "#\n\n"
      << "SET(target_libraries\n";

  QStringList libs = getContent(TARGETLIBRARIES_MARKER);
  foreach(QString lib, libs)
  {
    stream << "  " << lib << "\n";
  }

  stream << ")\n";

  return content;
}

QStringList ctkPluginGeneratorTargetLibraries::getMarkers() const
{
  QStringList markers = ctkPluginGeneratorAbstractTemplate::getMarkers();

  markers << TARGETLIBRARIES_MARKER;
  return markers;
}
