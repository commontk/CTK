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

#ifndef CTKCMDLINEMODULEREFERENCE_H
#define CTKCMDLINEMODULEREFERENCE_H

#include <ctkCommandLineModulesCoreExport.h>

#include <QSharedDataPointer>
#include <QMetaType>

struct ctkCmdLineModuleBackend;
class ctkCmdLineModuleDescription;
struct ctkCmdLineModuleReferencePrivate;

/**
 * \class ctkCmdLineModuleReference
 * \brief Defines a reference or handle to a module, including location,
 * XML, description and access to the associated backend.
 * \ingroup CommandLineModulesCore_API
 *
 * Instances of this class reference registered modules and can be used
 * to retrieve information about their parameters or to create a specific
 * front-end.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleReference
{
public:

  /**
   * @brief Creates an invalid module reference.
   */
  ctkCmdLineModuleReference();
  ~ctkCmdLineModuleReference();

  ctkCmdLineModuleReference(const ctkCmdLineModuleReference& ref);
  ctkCmdLineModuleReference& operator=(const ctkCmdLineModuleReference& ref);

  /**
   * @brief Conversion operator to test the validity of this module reference.
   */
  operator bool() const;

  /**
   * @brief Get the module description for the parameters.
   * @return The XML description as a class representation.
   * @throws ctkCmdLineModuleXmlException if the raw XML description cannot be parsed.
   */
  ctkCmdLineModuleDescription description() const;

  /**
   * @brief Get the raw XML description, as supplied by the back-end.
   * @return The raw XML description.
   */
  QByteArray rawXmlDescription() const;

  /**
   * @brief Retrieve a validation error string.
   * @return A non-empty string describing the validation error, if validation
   *         of the XML description was not successful.
   */
  QString xmlValidationErrorString() const;

  /**
   * @brief Get the URL under which the module was registered.
   * @return The module location.
   */
  QUrl location() const;

  /**
   * @brief Get the back-end which was registered to handle this module.
   * @return The back-end handling this module.
   */
  ctkCmdLineModuleBackend* backend() const;

private:

  friend class ctkCmdLineModuleManager;
  friend uint CTK_CMDLINEMODULECORE_EXPORT qHash(const ctkCmdLineModuleReference&);

  QSharedDataPointer<ctkCmdLineModuleReferencePrivate> d;

};

Q_DECLARE_METATYPE(ctkCmdLineModuleReference)

uint CTK_CMDLINEMODULECORE_EXPORT qHash(const ctkCmdLineModuleReference& moduleRef);

#endif // CTKCMDLINEMODULEREFERENCE_H
