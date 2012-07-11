/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#ifndef __ctkVTKPythonQtWrapperFactory_h
#define __ctkVTKPythonQtWrapperFactory_h

// PythonQt includes
#include <PythonQtCppWrapperFactory.h>

// CTK includes
#include "ctkVisualizationVTKCoreExport.h"

/// \ingroup Visualization_VTK_Core
/// VTK PythonQt wrapper factory
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKPythonQtWrapperFactory : public PythonQtForeignWrapperFactory
{
public:
  typedef PythonQtForeignWrapperFactory Superclass;
  ctkVTKPythonQtWrapperFactory();
  virtual ~ctkVTKPythonQtWrapperFactory();
  virtual PyObject* wrap(const QByteArray& classname, void *ptr);
  virtual void* unwrap(const QByteArray& classname, PyObject* object);
};

#endif

