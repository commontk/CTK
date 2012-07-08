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

#ifndef __ctkModuleParameterGroup_h
#define __ctkModuleParameterGroup_h

#include "ctkModuleParameter.h"

class ctkModuleParameterGroupPrivate;

/** 
*  \brief Group of parameters
*
*/
class CTK_CMDLINEMODULE_EXPORT ctkModuleParameterGroup
{
  Q_DECLARE_PRIVATE(ctkModuleParameterGroup)

public:

  ctkModuleParameterGroup();
  ~ctkModuleParameterGroup();
  
  void setLabel(const QString& label);
  QString label() const;

  void setDescription(const QString& description);
  QString description() const;

  void setAdvanced(bool advanced);
  bool advanced() const;

  void addParameter(ctkModuleParameter* parameter);
  QList<ctkModuleParameter*> parameters() const;

  bool hasParameter(const QString& name) const;

  ctkModuleParameter* parameter(const QString& name) const;

  bool hasReturnParameters() const;

  bool writeParameterFile(QTextStream& in, bool withHandlesToBulkParameters) const;
  
private:

  Q_DISABLE_COPY(ctkModuleParameterGroup)

  ctkModuleParameterGroupPrivate * const d_ptr;
};

CTK_CMDLINEMODULE_EXPORT QTextStream & operator<<(QTextStream &os, const ctkModuleParameterGroup& group);

#endif
