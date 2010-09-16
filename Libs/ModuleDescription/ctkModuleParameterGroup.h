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
#include "QHash"
#include "QVector"

/** 
*  \brief Group of parameters
*
*/
class CTK_MODULDESC_EXPORT ctkModuleParameterGroup : public QHash<QString, QString>
{
public:
  ctkModuleParameterGroup();
  ctkModuleParameterGroup(const ctkModuleParameterGroup &parameters);

  //void operator=(const ctkModuleParameterGroup &parameters);

  void addParameter(const ctkModuleParameter &parameter);

  const QVector<ctkModuleParameter>& parameters() const;

  QVector<ctkModuleParameter>& parameters();
  
private:
  ///
  QVector<ctkModuleParameter> Parameters;
};

CTK_MODULDESC_EXPORT QTextStream & operator<<(QTextStream &os, const ctkModuleParameterGroup &group);

#endif
