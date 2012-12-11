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

#ifndef __ctkCmdLineModuleParameterGroup_h
#define __ctkCmdLineModuleParameterGroup_h

#include "ctkCommandLineModulesCoreExport.h"

#include <QList>
#include <QSharedDataPointer>

class QTextStream;

class ctkCmdLineModuleParameter;
struct ctkCmdLineModuleParameterGroupPrivate;

/** 
 * \class ctkCmdLineModuleParameterGroup
 * \brief Group of parameters
 * \ingroup CommandLineModulesCore_API
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleParameterGroup
{

public:

  ctkCmdLineModuleParameterGroup(const ctkCmdLineModuleParameterGroup& other);
  ~ctkCmdLineModuleParameterGroup();

  ctkCmdLineModuleParameterGroup& operator=(const ctkCmdLineModuleParameterGroup& other);

  QString label() const;

  QString description() const;

  bool advanced() const;

  QList<ctkCmdLineModuleParameter> parameters() const;

  bool hasParameter(const QString& name) const;

  ctkCmdLineModuleParameter parameter(const QString& name) const;

  bool hasReturnParameters() const;

private:

  friend class ctkCmdLineModuleXmlParser;

  ctkCmdLineModuleParameterGroup();

  QSharedDataPointer<ctkCmdLineModuleParameterGroupPrivate> d;
};

CTK_CMDLINEMODULECORE_EXPORT QTextStream & operator<<(QTextStream &os, const ctkCmdLineModuleParameterGroup& group);

#endif
