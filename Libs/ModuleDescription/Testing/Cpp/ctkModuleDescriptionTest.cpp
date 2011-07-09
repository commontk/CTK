/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

// Qt includes
#include <QTextStream>

// CTK includes
#include "ctkModuleDescription.h"
#include <iostream>

//-----------------------------------------------------------------------------
int ctkModuleDescriptionTest(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);  


  ctkModuleParameter param;
  param[ "Tag" ] = "MyTag";
  param[ "Name" ] = "MyName";
  param[ "Description" ] = "MyDescription";
  param[ "Label" ] = "MyLabel";
  param[ "CPPType" ] = "MyCPPType";
  param[ "Type" ] = "MyType";
  param[ "Reference" ] = "MyReference";
  param[ "Hidden" ] = "false";
  param[ "ArgType" ] = "MyArgType";
  param[ "StringToType" ] = "MyStringToType";
  param[ "Default" ] = "MyDefault";
  param[ "Flag" ] = "MyFlag";
  param[ "LongFlag" ] = "MyLongFlag";
  param[ "Constraints" ] = "MyConstraints";
  param[ "Minimum" ] = "MyMinimum";
  param[ "Maximum" ] = "MyMaximum";
  param[ "Channel" ] = "MyChannel";
  param[ "Index" ] = "MyIndex";
  param[ "Multiple" ] = "false";
  param[ "Aggregate" ] = "false";
  param[ "FileExtensions" ] = ".vtk,.jpg";
  param[ "FlagAliases" ] = "MyFlagAliases";
  param[ "DeprecatedFlagAliases" ] = "MyDeprecatedFlagAliases";
  param[ "LongFlagAliases" ] = "MyLongFlagAliases";
  param[ "DeprecatedLongFlagAliases" ] = "MyDeprecatedLongFlagAliases";
  param[ "CoordinateSystem" ] = "MyCoordinateSystem";


  ctkModuleParameterGroup group;
  group[ "Label" ] = "MyLabel";
  group[ "Description" ] = "MyDescription";
  group[ "Advanced" ] = "MyAdvanced";


  ctkModuleDescription module;
  module[ "Category" ] = "MyCategory";
  module[ "Index" ] = "MyIndex";
  module[ "Title" ] = "MyTitle";
  module[ "Description" ] = "MyDescription";
  module[ "DocumentationURL" ] = "MyDocumentationURL";
  module[ "License" ] = "MyLicense";
  module[ "Acknowledgements" ] = "MyAcknowledgements";
  module[ "Contributor" ] = "MyContributor";
  module[ "Type" ] = "MyType";
  module[ "AlternativeType" ] = "MyAlternativeType";
  module[ "Target" ] = "MyTarget";
  module[ "AlternativeTarget" ] = "MyAlternativeTarget";
  module[ "Location" ] = "MyLocation";

  group.addParameter( new ctkModuleParameter(param) );
  module.addParameterGroup( new ctkModuleParameterGroup(group) );

  QTextStream stream(stdout);
  stream<< module;

  return EXIT_SUCCESS;
}
