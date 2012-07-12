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

#ifndef CTKCMDLINEMODULEXSLTRANSFORM_H
#define CTKCMDLINEMODULEXSLTRANSFORM_H

#include "ctkCommandLineModulesCoreExport.h"
#include "ctkCmdLineModuleXmlValidator.h"

#include <QString>

class QIODevice;

/**
 * \ingroup CommandLineModulesCore
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleXslTransform
    : public ctkCmdLineModuleXmlValidator
{

public:

  ctkCmdLineModuleXslTransform(QIODevice* input = 0, QIODevice* output = 0);

  void setOutput(QIODevice* output);
  QIODevice* output() const;

  void setOutputSchema(QIODevice* output);

  /**
   * @brief Transforms an XML input via a XSL transformation.
   *
   * This method assumes that the input set via setInput() or supplied
   * in the constructor is a valid XML fragment.
   *
   * @return
   */
  bool transform();

  void setXslTransformation(QIODevice* transformation);

  void setValidateOutput(bool validate);

  bool error() const;
  QString errorString() const;

private:

  bool validateOutput();

  bool Validate;

  QIODevice* OutputSchema;
  QIODevice* Transformation;

  QIODevice* Output;
  QString ErrorStr;
};

#endif // CTKCMDLINEMODULEXSLTRANSFORM_H
