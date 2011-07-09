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

#ifndef __ctkSimplePythonQtDecorators_h
#define __ctkSimplePythonQtDecorators_h

// CTK includes
#include "ctkSimplePythonShellConfigure.h" // For CTK_WRAP_PYTHONQT_LIGHT
#include <ctkAbstractPythonManager.h>
#ifdef CTK_WRAP_PYTHONQT_LIGHT
# include <ctkCorePythonQtDecorators.h>
# include <ctkWidgetsPythonQtDecorators.h>
#endif

// PythonQt includes
#include <PythonQt.h>

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

class ctkSimplePythonQtDecorators : public QObject
{
  Q_OBJECT

public:

  ctkSimplePythonQtDecorators(ctkAbstractPythonManager* pythonManager)
    {
    Q_ASSERT(pythonManager);
#ifdef CTK_WRAP_PYTHONQT_LIGHT
    pythonManager->registerPythonQtDecorator(new ctkCorePythonQtDecorators);
    pythonManager->registerPythonQtDecorator(new ctkWidgetsPythonQtDecorators);
#else
    Q_UNUSED(pythonManager);
#endif
    }

public slots:

  
  
};

#endif

