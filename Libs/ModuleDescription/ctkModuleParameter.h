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

#ifndef __ctkModuleParameter_h
#define __ctkModuleParameter_h

#include "CTKModuleDescriptionExport.h"
#include "QHash"
#include "QTextStream"

/** 
 *  \brief Single parameter to a module, like a threshold of a filter.
 *
 * ctkModuleParameter describes a single parameters to a
 * module. Information on the parameter type, name, flag, label,
 * description, channel, index, default, and constraints can be
 * stored.
 *
 */
class CTK_MODULDESC_EXPORT ctkModuleParameter : public QHash<QString, QString>
{
public:
  ctkModuleParameter();
  ctkModuleParameter(const ctkModuleParameter& parameter);

  /// Simple return types are parameters on output channel with no
  /// flags and without a specified index 
  virtual bool isReturnParameter() const;
  
  /// Has a flag or a long flag?
  virtual bool isFlagParameter() const;

  /// Is an index type?
  virtual bool isIndexParameter() const;

};

CTK_MODULDESC_EXPORT QTextStream & operator<<(QTextStream &os, const ctkModuleParameter &parameter);
CTK_MODULDESC_EXPORT QTextStream & operator<<(QTextStream &os, const QStringList &list);
CTK_MODULDESC_EXPORT QTextStream & operator<<(QTextStream &os, const QHash<QString, QString> &hash);

#endif
