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
ctkModuleDescription::ctkModuleDescription()
{
}

//----------------------------------------------------------------------------
ctkModuleDescription::ctkModuleDescription(const ctkModuleDescription &md)
  : QHash<QString, QString>( QHash<QString, QString>( md ) )
{
  this->ParameterGroups = md.ParameterGroups;
  this->Logo = md.Logo;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::operator=(const ctkModuleDescription &md)
{
 QHash<QString, QString>::operator=(md);
	this->ParameterGroups = md.ParameterGroups;
	this->Logo = md.Logo;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkModuleDescription &module)
{
  os << QHash<QString, QString>(module);
  //os << "Logo: " << module.GetLogo() << endl;

  os << "ParameterGroups: " << endl;
  foreach( const ctkModuleParameterGroup& it, module.parameterGroups())
  { os << it; }
  return os;
}


//----------------------------------------------------------------------------
bool ctkModuleDescription::hasReturnParameters() const
{
  // iterate over each parameter group
  QVector<ctkModuleParameterGroup>::const_iterator pgbeginit
    = this->ParameterGroups.begin();
  QVector<ctkModuleParameterGroup>::const_iterator pgendit
    = this->ParameterGroups.end();
  QVector<ctkModuleParameterGroup>::const_iterator pgit;
  
  for (pgit = pgbeginit; pgit != pgendit; ++pgit)
    {
    // iterate over each parameter in this group
    QVector<ctkModuleParameter>::const_iterator pbeginit
      = (*pgit).parameters().begin();
    QVector<ctkModuleParameter>::const_iterator pendit
      = (*pgit).parameters().end();
    QVector<ctkModuleParameter>::const_iterator pit;

    for (pit = pbeginit; pit != pendit; ++pit)
      {
      if ((*pit).isReturnParameter())
        {
        return true;
        }
      }    
    }

  return false;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription::setParameterDefaultValue(const QString& name, const QString& value)
{
  ctkModuleParameter* param = parameter( name );
  if ( param )
  {
    (*param)[ "Default" ] = value;
    return true;
  }

  return false;
}


//----------------------------------------------------------------------------
ctkModuleParameter* ctkModuleDescription::parameter(const QString& name)
{
  // iterate over each parameter group
  QVector<ctkModuleParameterGroup>::iterator pgbeginit
    = this->ParameterGroups.begin();
  QVector<ctkModuleParameterGroup>::iterator pgendit
    = this->ParameterGroups.end();
  QVector<ctkModuleParameterGroup>::iterator pgit;
  
  for (pgit = pgbeginit; pgit != pgendit; ++pgit)
    {
    // iterate over each parameter in this group
    QVector<ctkModuleParameter>::iterator pbeginit
      = (*pgit).parameters().begin();
    QVector<ctkModuleParameter>::iterator pendit
      = (*pgit).parameters().end();
    QVector<ctkModuleParameter>::iterator pit;

    for (pit = pbeginit; pit != pendit; ++pit)
      {
      if ((*pit)["Name"] == name)
        {
        return &(*pit);
        }
      }    
    }

  return NULL;
}

//----------------------------------------------------------------------------
void ctkModuleDescription ::setLogo(const QIcon& logo)
{
  this->Logo = logo;
}

//----------------------------------------------------------------------------
const QIcon& ctkModuleDescription::logo() const
{
  return this->Logo;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription ::readParameterFile(const QString& filename)
{
  bool modified = false;

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    std::cout << "Parameter file " << filename.toStdString( ) << " could not be opened." << endl;
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
writeParameterFile(const QString& filename, bool withHandlesToBulkParameters)
{
  QFile rtp(filename);

  if (!rtp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
    std::cout << "Parameter file " << filename.toStdString() << " could not be opened for writing." << endl;
    return false;
    }

  QTextStream in(&rtp);
  // iterate over each parameter group
  QVector<ctkModuleParameterGroup>::const_iterator pgbeginit
    = this->ParameterGroups.begin();
  QVector<ctkModuleParameterGroup>::const_iterator pgendit
    = this->ParameterGroups.end();
  QVector<ctkModuleParameterGroup>::const_iterator pgit;
  
  for (pgit = pgbeginit; pgit != pgendit; ++pgit)
    {
    // iterate over each parameter in this group
    QVector<ctkModuleParameter>::const_iterator pbeginit
      = (*pgit).parameters().begin();
    QVector<ctkModuleParameter>::const_iterator pendit
      = (*pgit).parameters().end();
    QVector<ctkModuleParameter>::const_iterator pit;

    for (pit = pbeginit; pit != pendit; ++pit)
      {
      // write out all parameters or just the ones that are not bulk parameters
      if (withHandlesToBulkParameters
          || (!withHandlesToBulkParameters 
              && ((*pit)[ "Tag" ] != "image"
                  && (*pit)[ "Tag" ] != "geometry"
                  && (*pit)[ "Tag" ] != "transform"
                  && (*pit)[ "Tag" ] != "table"
                  && (*pit)[ "Tag" ] != "measurement"
                  && (*pit)[ "Tag" ] != "point"  // point and region are special
                  && (*pit)[ "Tag" ] != "region")))
        {
        in << (*pit)[ "Name" ] << " = " << (*pit)[ "Default" ] << endl;

        // multiple="true" may have to be handled differently
        }
      }
    }

  return true;
}

void ctkModuleDescription::addParameterGroup( const ctkModuleParameterGroup &group )
{
	this->ParameterGroups.push_back(group);
}

const QVector<ctkModuleParameterGroup>& ctkModuleDescription::parameterGroups() const
{
	return this->ParameterGroups;
}

QVector<ctkModuleParameterGroup>& ctkModuleDescription::parameterGroups()
{
	return this->ParameterGroups;
}

void ctkModuleDescription::setParameterGroups( const QVector<ctkModuleParameterGroup>& groups )
{
	this->ParameterGroups = groups;
}
