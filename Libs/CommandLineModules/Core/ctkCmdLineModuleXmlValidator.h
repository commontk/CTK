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

#include <ctkCommandLineModulesCoreExport.h>

#include <QString>

class QIODevice;

/**
 * \ingroup CommandLineModulesCore
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleXmlValidator
{

public:

  ctkCmdLineModuleXmlValidator(QIODevice* input = 0);

  void setInput(QIODevice* input);
  QIODevice* input() const;

  void setInputSchema(QIODevice* input);

  virtual bool validateInput();

  virtual bool error() const;
  virtual QString errorString() const;

private:

  QIODevice* Input;
  QIODevice* InputSchema;

  QString ErrorStr;
};

#endif // CTKCMDLINEMODULEXMLVALIDATOR_H
