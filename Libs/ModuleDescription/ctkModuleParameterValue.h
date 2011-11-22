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

#ifndef __ctkModuleParameterValue_h
#define __ctkModuleParameterValue_h

#include "CTKModuleDescriptionExport.h"
#include "CTKModuleParameter.h"
#include "QVariant.h"

/** 
 *  \brief Single parameter value to a module, like a threshold of a filter.
 *
 * ctkModuleParameterValue describes a single parameter value to a
 * module. Information on the parameter type, name, flag, label,
 * description, channel, index, default, and constraints can be
 * stored.
 *
 */
class CTK_MODULDESC_EXPORT ctkModuleParameterValue
{
public:
  // constructor
  ctkModuleParameterValue( const ctkModuleParameter& param );
  // set the value
  void setValue( const QVariant& value );
  // set the value to the defaut one
  void setDefaultValue();
  // get the value
  const QVariant& getValue() const;
  // get the associated ctkModuleParameter
  const ctkModuleParameter& parameter() const;
private:
  QVariant value;
  const ctkModuleParameter& Parameter;
};

#endif
