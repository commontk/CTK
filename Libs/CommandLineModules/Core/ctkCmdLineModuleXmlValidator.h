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

#include <QScopedPointer>

class ctkCmdLineModuleXmlValidatorPrivate;

class QIODevice;

/**
 * @ingroup CommandLineModulesCore_API
 *
 * @brief Provides validation of an XML document against an XML schema.
 *
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleXmlValidator
{

public:

  ctkCmdLineModuleXmlValidator(QIODevice* input = 0);
  ~ctkCmdLineModuleXmlValidator();

  /**
   * @brief Set the XML input.
   * @param input The Xml input.
   */
  void setInput(QIODevice* input);

  /**
   * @brief Get the XML input device.
   * @return The XML input device.
   */
  QIODevice* input() const;

  /**
   * @brief Set the XML schema to be used during validation.
   * @param input The XML schema.
   */
  void setInputSchema(QIODevice* input);

  /**
   * @brief Validate the XML input against the XML schema set via setInputSchema().
   * @return \c true if validation was successful, \c false otherwise.
   */
  virtual bool validateInput();

  /**
   * @brief Get the error flag.
   * @return \c true if an error occured during validation, \c false otherwise.
   */
  virtual bool error() const;

  /**
   * @brief Get the error string.
   * @return A description of the validation error, if any.
   */
  virtual QString errorString() const;

private:

  QScopedPointer<ctkCmdLineModuleXmlValidatorPrivate> d;

};

#endif // CTKCMDLINEMODULEXMLVALIDATOR_H
