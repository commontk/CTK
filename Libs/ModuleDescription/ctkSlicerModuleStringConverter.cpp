/*=============================================================================

Library: CTK

Copyright (c) 2010 CISTIB - Universtitat Pompeu Fabra

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

#include "ctkSlicerModuleStringConverter.h"
#include <QVector>


void ctkSlicerModuleStringConverter::update()
{
  SetTarget( );

  SetAllParameters( );
}

const QVariant ctkSlicerModuleStringConverter::GetOutput()
{
  return this->CommandLineAsString;
}

void ctkSlicerModuleStringConverter::SetAllParameters()
{
  foreach( const ctkModuleParameterGroup* itGroup, this->ModuleDescription.parameterGroups())
  {
    foreach( const ctkModuleParameter* itParam, itGroup->parameters())
    {
      SetParameterValue( *itParam );
    }
  }
}

void ctkSlicerModuleStringConverter::SetParameterValue( const ctkModuleParameter &param )
{
  QString prefix;
  QString flag;
  bool hasFlag = false;

  if ( param["LongFlag"] != "")
  {
    prefix = "--";
    flag = param["LongFlag"];
    hasFlag = true;
  }
  else if (param["Flag"] != "")
  {
    prefix = "-";
    flag = param["Flag"];
    hasFlag = true;
  }


  if (hasFlag)
  {
    if (   param["Tag"] != "boolean"
      && param["Tag"] != "file" 
      && param["Tag"] != "directory"
      && param["Tag"] != "string"
      && param["Tag"] != "integer-vector"
      && param["Tag"] != "float-vector"
      && param["Tag"] != "double-vector"
      && param["Tag"] != "string-vector"
      && param["Tag"] != "image"
      && param["Tag"] != "point"
      && param["Tag"] != "region"
      && param["Tag"] != "transform"
      && param["Tag"] != "geometry"
      && param["Tag"] != "table"
      && param["Tag"] != "measurement")
    {
      // simple parameter, write flag and value
      this->CommandLineAsString.push_back(prefix + flag);
      this->CommandLineAsString.push_back(param["Default"]);
    }
    else if (param["Tag"] == "boolean" && param["Default"] == "true")
    {
      this->CommandLineAsString.push_back(prefix + flag);
    }
    else if (param["Tag"] == "file" 
      || param["Tag"] == "directory"
      || param["Tag"] == "string"
      || param["Tag"] == "integer-vector"
      || param["Tag"] == "float-vector"
      || param["Tag"] == "double-vector"
      || param["Tag"] == "string-vector")
    {
      // Only write out the flag if value is not empty
      if ( param["Default"] != "")
      {
        this->CommandLineAsString.push_back(prefix + flag);
        this->CommandLineAsString.push_back( param["Default"] );
      }
    }
    // data passed as parameter
    else if ( param["Tag"] == "image" 
      || param["Tag"] == "geometry"
      || param["Tag"] == "transform" 
      || param["Tag"] == "table" 
      || param["Tag"] == "measurement" )
    {
      if ( param["Default"] != "")
      {
        this->CommandLineAsString.push_back(prefix + flag);
        this->CommandLineAsString.push_back(param["Default"]);
      }
    }
    else if ( param["Tag"] == "region" )
    {
      this->CommandLineAsString.push_back(prefix + flag);
      this->CommandLineAsString.push_back( param["Default"] );
    }
    else if ( param["Tag"] == "point" )
    {
      QStringList points = param["Default"].split( ";");
      foreach ( const QString &it, points )
      {
        this->CommandLineAsString.push_back(prefix + flag);
        this->CommandLineAsString.push_back( it );
      }
    }

  }

  // If index is not empty -> It's a command line argument arg0, arg1, ... without flag prefix
  if ( param["Index"] != "")
  {
    this->CommandLineAsString.push_back( param["Default"] );
  }
}

void ctkSlicerModuleStringConverter::SetTarget()
{
  this->CommandLineAsString.clear();

  if (!this->ModuleDescription["Location"].isEmpty() && 
    this->ModuleDescription["Location"] != this->ModuleDescription["Target"])
  {
    this->CommandLineAsString.push_back(this->ModuleDescription["Location"]);
  }
  this->CommandLineAsString.push_back( this->ModuleDescription["Target"] );
}


