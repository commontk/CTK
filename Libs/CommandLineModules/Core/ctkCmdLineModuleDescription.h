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

#ifndef __ctkCmdLineModuleDescription_h
#define __ctkCmdLineModuleDescription_h

#include <ctkCommandLineModulesCoreExport.h>

#include <QList>
#include <QSharedDataPointer>

class QIcon;
class QIODevice;
class QTextStream;

struct ctkCmdLineModuleDescriptionPrivate;
class ctkCmdLineModuleParameterGroup;
class ctkCmdLineModuleParameter;

/**
 * Description of the parameters of a command line module.
 *
 * The parameters can be used for automated GUI generation or execution
 * of the module.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleDescription
{

public:

  ctkCmdLineModuleDescription(const ctkCmdLineModuleDescription& description);
  ~ctkCmdLineModuleDescription();

  ctkCmdLineModuleDescription& operator=(const ctkCmdLineModuleDescription& other);

  static ctkCmdLineModuleDescription parse(QIODevice* input);

  QString category() const;

  QString title() const;

  QString description() const;

  QString version() const;

  QString documentationURL() const;

  QString license() const;

  QString acknowledgements() const;

  QString contributor() const;

  QIcon logo() const;

  QList<ctkCmdLineModuleParameterGroup> parameterGroups() const;

  bool hasParameter(const QString& name) const;

  ctkCmdLineModuleParameter parameter(const QString& name) const;

  // Does the module have any simple (primitive) return types?
  bool hasReturnParameters() const;

private:

  friend class ctkCmdLineModuleXmlParser;
  friend class ctkCmdLineModuleReferencePrivate;

  ctkCmdLineModuleDescription();

  QSharedDataPointer<ctkCmdLineModuleDescriptionPrivate> d;

};

CTK_CMDLINEMODULECORE_EXPORT QTextStream & operator<<(QTextStream& os, const ctkCmdLineModuleDescription& module);

#endif
