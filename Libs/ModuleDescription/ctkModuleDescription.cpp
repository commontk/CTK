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

#include "ctkModuleDescription.h"
#include <iostream>
#include "QFile"
#include "QTextStream"

//----------------------------------------------------------------------------
void ctkModuleDescription::addParameterGroup( ctkModuleParameterGroup* group )
{
  Q_ASSERT(group);
	this->ParameterGroups.push_back(group);
}

//----------------------------------------------------------------------------
const QVector<ctkModuleParameterGroup*>& ctkModuleDescription::parameterGroups() const
{
	return this->ParameterGroups;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription::hasReturnParameters() const
{
  // iterate over each parameter group
  foreach( const ctkModuleParameterGroup* group, this->ParameterGroups)
    {
    if (group->hasReturnParameters())
      {
      return true;
      }
    }

  return false;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription::setParameterDefaultValue(const QString& name, const QString& value)
{
  ctkModuleParameter* param = this->parameter( name );
  if ( param )
    {
    (*param)[ "Default" ] = value;
    return true;
    }

  return false;
}

//----------------------------------------------------------------------------
ctkModuleParameterGroup* ctkModuleDescription::parameterGroup(const QString& parameterName)const
{
  // iterate over each parameter group
  foreach( ctkModuleParameterGroup* group, this->ParameterGroups)
    {
    ctkModuleParameter* param = group->parameter(parameterName);
    if (param)
      {
      return group;
      }    
    }
  return 0;
}

//----------------------------------------------------------------------------
ctkModuleParameter* ctkModuleDescription::parameter(const QString& name)const
{
  // iterate over each parameter group
  foreach( const ctkModuleParameterGroup* group, this->ParameterGroups)
    {
    ctkModuleParameter* param = group->parameter(name);
    if (param)
      {
      return param;
      }    
    }
  return 0;
}

//----------------------------------------------------------------------------
void ctkModuleDescription ::setIcon(const QIcon& logo)
{
  this->Icon = logo;
}

//----------------------------------------------------------------------------
const QIcon& ctkModuleDescription::icon() const
{
  return this->Icon;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription ::readParameterFile(const QString& filename)
{
  bool modified = false;

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    std::cout << "Parameter file " << filename.toStdString( ) << " could not be opened." << std::endl;
    return false;
    }

  QTextStream in(&file);
  while (!in.atEnd())
    {
    QString line = in.readLine();

    // split the line into key: value
    QString key, value;

    line = line.trimmed();
    QStringList list = line.split( "=" );
    key = list[ 0 ].trimmed();
    if ( list.size() == 1 )
      {
      continue;
      }
    value = list[ 1 ].trimmed();

    
    // std::cout << "key=" << key << ", value=" << value << "!" << endl;

    ctkModuleParameter *param = this->parameter( key );
    if ( param )
      {
      if (value != (*param)["Default"] )
        {
        (*param)["Default"] = value;
        modified = true;

        // multiple="true" may have to be handled differently
        }
      }
    }

  return modified;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription::
writeParameterFile(const QString& filename, bool withHandlesToBulkParameters)const
{
  QFile rtp(filename);

  if (!rtp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
    std::cout << "Parameter file " << filename.toStdString() << " could not be opened for writing." << std::endl;
    return false;
    }

  QTextStream in(&rtp);
  // iterate over each parameter group
  foreach(const ctkModuleParameterGroup* group, this->ParameterGroups)
    {
    group->writeParameterFile(in, withHandlesToBulkParameters);
    }

  return true;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkModuleDescription &module)
{
  os << QHash<QString, QString>(module);
  os << "Icon: " << QBool(!module.icon().isNull()) << '\n';

  os << "ParameterGroups: " << '\n';
  foreach(const ctkModuleParameterGroup* group, module.ParameterGroups)
    {
    os << *group;
    }
  return os;
}

