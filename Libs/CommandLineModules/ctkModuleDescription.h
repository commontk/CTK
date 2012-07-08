/*=============================================================================

  Library: CTK

  Copyright (c) 2010 Brigham and Women's Hospital (BWH) All Rights Reserved.

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

#ifndef __ctkModuleDescription_h
#define __ctkModuleDescription_h

#include <QIcon>

#include "ctkModuleParameterGroup.h"

struct ctkModuleDescriptionPrivate;

/**
* Description of the parameters of a module
*
* The parameters can be used for automated GUI generation or execution
* of the module.
*
* For example:
* - Target: This is the entry point for a shared object module and the full 
* command (with path) for an executable.
* - Location: This is path to the executable for the module
*/
class CTK_CMDLINEMODULE_EXPORT ctkModuleDescription
{
  Q_DECLARE_PRIVATE(ctkModuleDescription)

public:

  ~ctkModuleDescription();

  static ctkModuleDescription* parse(QIODevice* input);

  void setCategory(const QString& cat);
  QString category() const;

  void setTitle(const QString& title);
  QString title() const;

  void setDescription(const QString& description);
  QString description() const;

  void setVersion(const QString& version);
  QString version() const;

  void setDocumentationURL(const QString& documentationURL);
  QString documentationURL() const;

  void setLicense(const QString& license);
  QString license() const;

  void setAcknowledgements(const QString& acknowledgements);
  QString acknowledgements() const;

  void setContributor(const QString& contributor);
  QString contributor() const;

  /// Set the location for the module.  This is path to the file for the module.
  void setLocation(const QString& target);
  /// Get the location for the module.  This is path to the file for the module.
  QString location() const;

  void setLogo(const QIcon& logo);
  QIcon logo() const;

  void addParameterGroup(ctkModuleParameterGroup* group);
  QList<ctkModuleParameterGroup*> parameterGroups() const;
  void setParameterGroups(const QList<ctkModuleParameterGroup*>& groups);

  bool hasParameter(const QString& name) const;

  ctkModuleParameter* parameter(const QString& name) const;

  // Does the module have any simple (primitive) return types?
  bool hasReturnParameters() const;

  bool setParameterDefaultValue(const QString& name,
                                const QString& value);

//  const ModuleProcessInformation* processInformation() const
//  {return &ProcessInformation;}

//  ModuleProcessInformation* processInformation()
//  {return &ProcessInformation;}

  ///
  /// Read a parameter file. Syntax of file is "name: value" for each
  /// parameter. Returns a bool indicating whether any parameter value
  /// was modified.
  bool readParameterFile(const QString& filename);

  ///
  /// Write a parameter file. By default, the method writes out all
  /// the parameters.  The "withHandlesToBulkParameters" parameter
  /// controls whether the handles to the bulk parameters (image,
  /// geometry, etc.) are written to the file.
  bool writeParameterFile(const QString& filename, bool withHandlesToBulkParameters = true) const;

private:

  ctkModuleDescription();

  Q_DISABLE_COPY(ctkModuleDescription)

  ctkModuleDescriptionPrivate * const d_ptr;

};

CTK_CMDLINEMODULE_EXPORT QTextStream & operator<<(QTextStream& os, const ctkModuleDescription& module);

#endif
