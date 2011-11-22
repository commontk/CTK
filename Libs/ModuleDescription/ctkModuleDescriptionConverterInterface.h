/*=============================================================================

Library: CTK

Copyright (c) 2010 CISTIB - Universitat Pompeu Fabra

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

#ifndef __ctkModuleDescriptionConverterInterface_h
#define __ctkModuleDescriptionConverterInterface_h

#include <QVariant>

#include "ctkModuleDescriptionExport.h"
#include "ctkModuleDescription.h"

/** 
 * \brief Interface of ModuleDescription converter
 *
 * A converter can be a Automated GUI generator that converts
 * ctkModuleDescription into a Qt window. A converter can also be
 * a command line generator that creates a command line string to 
 * execute an application
 */
class CTK_MODULDESC_EXPORT ctkModuleDescriptionConverterInterface : public QObject
{
    Q_OBJECT
public:
  ctkModuleDescriptionConverterInterface(){};
  ~ctkModuleDescriptionConverterInterface(){};

  //! Set input Module Description
  virtual void setModuleDescription(const ctkModuleDescription &val) = 0;

  //!
  virtual void update( ) = 0;

  //!
  virtual const QVariant GetOutput( ) = 0;
};

#endif
