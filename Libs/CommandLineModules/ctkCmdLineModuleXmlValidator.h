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

#ifndef CTKCMDLINEMODULEXMLVALIDATOR_H
#define CTKCMDLINEMODULEXMLVALIDATOR_H

#include <ctkCommandLineModulesExport.h>

#include <QString>

class QIODevice;

class CTK_CMDLINEMODULE_EXPORT ctkCmdLineModuleXmlValidator
{

public:

  ctkCmdLineModuleXmlValidator(QIODevice* input = 0);

  void setInput(QIODevice* input);
  QString output();

  void setInputSchema(QIODevice* input);
  void setOutputSchema(QIODevice* output);

  void setXSLTransformation(QIODevice* transformation);

  bool validate();
  bool validateXMLInput();
  bool validateXSLTOutput();

  bool error() const;
  QString errorString() const;

private:

  QIODevice* _input;
  QIODevice* _inputSchema;
  QIODevice* _outputSchema;
  QIODevice* _transformation;

  QString _output;
  QString _errorStr;
};

#endif // CTKCMDLINEMODULEXMLVALIDATOR_H
