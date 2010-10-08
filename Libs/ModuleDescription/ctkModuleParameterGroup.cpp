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

#include "ctkModuleParameterGroup.h"

//----------------------------------------------------------------------------
ctkModuleParameterGroup::~ctkModuleParameterGroup()
{
  foreach(ctkModuleParameter* param, this->Parameters)
    {
    delete param;
    }
  this->Parameters.clear();
}

//----------------------------------------------------------------------------
void ctkModuleParameterGroup::addParameter( ctkModuleParameter* parameter )
{
  Q_ASSERT(parameter);
  this->Parameters.push_back(parameter);
}

//----------------------------------------------------------------------------
ctkModuleParameter* ctkModuleParameterGroup::parameter( const QString& parameterName )const
{
  foreach(ctkModuleParameter* param, this->Parameters)
    {
    if ((*param)["Name"] == parameterName)
      {
      return param;
      }
    }
  return 0;
}

//----------------------------------------------------------------------------
bool ctkModuleParameterGroup::hasReturnParameters() const
{
  // iterate over each parameter in this group
  foreach(const ctkModuleParameter* param, this->Parameters)
    {
    if (param->isReturnParameter())
      {
      return true;
      }
    }
  return false;
}

//----------------------------------------------------------------------------
bool ctkModuleParameterGroup::
writeParameterFile(QTextStream& in, bool withHandlesToBulkParameters)const
{
  // iterate over each parameter in this group
  foreach(const ctkModuleParameter* moduleParameter, this->Parameters)
    {
    const ctkModuleParameter& param = *moduleParameter;
    // write out all parameters or just the ones that are not bulk parameters
    if (withHandlesToBulkParameters
        || (!withHandlesToBulkParameters 
            && (param[ "Tag" ] != "image"
            && param[ "Tag" ] != "geometry"
            && param[ "Tag" ] != "transform"
            && param[ "Tag" ] != "table"
            && param[ "Tag" ] != "measurement"
            && param[ "Tag" ] != "point"  // point and region are special
            && param[ "Tag" ] != "region")))
      {
      in << param[ "Name" ] << " = " << param[ "Default" ] << endl;
      // multiple="true" may have to be handled differently
      }
    }
  return true;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkModuleParameterGroup &group)
{ 
  os << QHash<QString, QString>(group);

  os << "  Parameters: " << endl;
  foreach (const ctkModuleParameter* it, group.Parameters)
    {
    os << *it;
    }
  return os;
}
