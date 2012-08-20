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

#include "ctkCmdLineModuleBackendFPDescriptionPrivate.h"

//----------------------------------------------------------------------------
QString ctkCmdLineModuleBackendFunctionPointer::DescriptionPrivate::xmlDescription() const
{
  QString xml;
  QTextStream str(&xml);
  str << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  str << "<executable>\n";
  str << "  <category>" + ModuleCategory + "</category>\n";
  str << "  <title>" + ModuleTitle + "</title>\n";
  str << "  <description>" + ModuleDescription + "</description>\n";
  str << "  <contributor>" + ModuleContributor + "</contributor>\n";
  str << "  <parameters>\n";
  str << "    <label>Function Parameters</label>\n";
  str << "    <description>Parameters for calling a function pointer.</description>\n";
  foreach (QString param, paramDescriptions)
  {
    str << param;
  }
  str << "  </parameters>\n";
  str << "</executable>\n";

  return xml;
}
