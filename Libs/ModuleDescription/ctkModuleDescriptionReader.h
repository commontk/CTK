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

#ifndef __ctkModuleDescriptionReader_h
#define __ctkModuleDescriptionReader_h

#include "CTKModuleDescriptionExport.h"
#include "ctkModuleDescriptionReaderInterface.h"

class QIODevice;
/** 
 * \brief Base XML Reader of ModuleDescription
 *
 * This specialized ctkModuleDescriptionReaderInterface can initialize
 * a module description from an XML file. update() must be reimplemented
 * in inherited classes.
 */
class CTK_MODULDESC_EXPORT ctkModuleDescriptionReader :
  public ctkModuleDescriptionReaderInterface
{
  Q_OBJECT
public:
  ctkModuleDescriptionReader();

  //! xml is the content of the XML file
  void setInput(QIODevice * device);
  QIODevice* input()const;

protected:
  QIODevice*           Device;
};

#endif
