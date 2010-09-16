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

#ifndef __ctkSlicerModuleStringConverter_h
#define __ctkSlicerModuleStringConverter_h

#include <string>

#include "ctkModuleDescriptionConverter.h"

class ModuleDescription;
class ModuleParameterGroup;


/**
 * Convert Slicer Module description to command line string QStringList
 *
 */
class ctkSlicerModuleStringConverter : public ctkModuleDescriptionConverter
{
  Q_OBJECT
public:
  ctkSlicerModuleStringConverter() {};
  ~ctkSlicerModuleStringConverter() {};

  ///
  void Update( );

  ///
  virtual const QVariant GetOutput( );

protected:

  //!
  void SetAllParameters();

  //!
  void SetParameterValue( const ctkModuleParameter &param );

  ///
  void SetTarget();

protected:

  QStringList CommandLineAsString;
};

#endif
