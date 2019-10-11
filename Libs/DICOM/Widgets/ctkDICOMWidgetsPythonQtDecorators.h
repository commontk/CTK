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

#ifndef __ctkDICOMWidgetsPythonQtDecorators_h
#define __ctkDICOMWidgetsPythonQtDecorators_h

// PythonQt includes
#include <PythonQt.h>

// CTK includes

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

/// \ingroup DICOM_Widgets
class ctkDICOMWidgetsPythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkDICOMWidgetsPythonQtDecorators()
    {
    }

public Q_SLOTS:

  //
  // None yet - refer to other libs for examples
  //

};

//-----------------------------------------------------------------------------
void initCTKDICOMWidgetsPythonQtDecorators()
{
  PythonQt::self()->addDecorators(new ctkDICOMWidgetsPythonQtDecorators);
}

#endif
