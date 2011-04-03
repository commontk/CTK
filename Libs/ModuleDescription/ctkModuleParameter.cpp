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

#include "ctkModuleParameter.h"
#include "QStringList"

//----------------------------------------------------------------------------
bool ctkModuleParameter::isReturnParameter() const
{
  // could check for tag == float, int, float-vector, ...
  return (*this)["Channel"] == "output" 
         && !this->isFlagParameter()
         && !this->isIndexParameter();
}

//----------------------------------------------------------------------------
bool ctkModuleParameter::isFlagParameter() const
{
  return ((*this)["Flag"] != "" || (*this)[ "LongFlag" ] != "");
}

//----------------------------------------------------------------------------
bool ctkModuleParameter::isIndexParameter() const
{
  return ((*this)[ "Index" ] != "");
}

//-----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkModuleParameter &parameter)
{ 
  os << "    Parameter" << endl;
  os << "      ";
  os.setFieldWidth(7);
  os.setFieldAlignment(QTextStream::AlignLeft);
  os << QHash<QString, QString>( parameter );
  os.setFieldWidth(0);

  os << "Flag aliases: ";
  os << parameter["FlagAliases"].split( "," );

  os << "      " << "Deprecated Flag aliases: ";
  os << parameter["DeprecatedFlagAliases"].split( "," );

  os << "      " << "LongFlag aliases: ";
  os << parameter["LongFlagAliases"].split( "," );

  os << "      " << "Deprecated LongFlag aliases: ";
  os << parameter["DeprecatedLongFlagAliases"].split( "," );

  os << "      " << "FileExtensions: ";
  os << parameter["FileExtensions"].split( "," );

  return os;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const QStringList &list)
{
  os << list.join(", ") << endl;
  return os;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const QHash<QString, QString> &hash)
{ 
  QHash<QString,QString>::const_iterator itProp;
  for ( itProp = hash.begin( ) ; 
        itProp != hash.end( ) ; 
        itProp++ )
    {
    os << itProp.key( ) << ": " << itProp.value( ) << endl;
    }

  return os;
}
