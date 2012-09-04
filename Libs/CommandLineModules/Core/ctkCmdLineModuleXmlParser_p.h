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

#ifndef CTKCMDLINEMODULEXMLPARSER_P_H
#define CTKCMDLINEMODULEXMLPARSER_P_H

#include <QXmlStreamReader>
#include <QHash>

class ctkCmdLineModuleDescription;
class ctkCmdLineModuleParameter;
struct ctkCmdLineModuleParameterParser;

class QIODevice;

/**
 * \class ctkCmdLineModuleXmlParser
 * \brief Performs XML parsing, loading data into a ctkCmdLineModuleDescription
 * \ingroup CommandLineModulesCore_API
 * \see ctkCmdLineModuleDescription
 */
class ctkCmdLineModuleXmlParser
{

public:

  ctkCmdLineModuleXmlParser(QIODevice* device, ctkCmdLineModuleDescription* md);
  ~ctkCmdLineModuleXmlParser();

  void doParse();

private:

  void handleExecutableElement();
  void handleParametersElement();
  ctkCmdLineModuleParameter handleParameterElement();

  QIODevice* const _device;
  ctkCmdLineModuleDescription* _md;
  QXmlStreamReader _xmlReader;
  QHash<QString, ctkCmdLineModuleParameterParser*> _paramParsers;
};

#endif // CTKCMDLINEMODULEXMLPARSER_P_H
