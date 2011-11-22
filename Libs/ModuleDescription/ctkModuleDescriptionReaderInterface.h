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

#ifndef __ctkModuleDescriptionReaderInterface_h
#define __ctkModuleDescriptionReaderInterface_h

// Qt includes
#include <QObject>
#include <QString>

// CTK includes
#include "ctkModuleDescription.h"
#include "ctkModuleDescriptionExport.h"

/** 
 * \brief Interface of ModuleDescription reader
 */
class CTK_MODULDESC_EXPORT ctkModuleDescriptionReaderInterface : public QObject
{
  Q_OBJECT
public:

  //!
  virtual void update( ) = 0;

  //! Returns the populated module description.
  //! Note: the module description is set after update() is called
  const ctkModuleDescription &moduleDescription() const;

protected:
  ctkModuleDescription ModuleDescription;
};

#endif
