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

#ifndef __ctkVTKScalarsToColorsUtils_h
#define __ctkVTKScalarsToColorsUtils_h

// CTK includes
#include "ctkVisualizationVTKCoreExport.h"

// VTK includes
class vtkDiscretizableColorTransferFunction;
class vtkScalarsToColors;

namespace ctk
{

/// \ingroup Visualization_VTK_Core
/// Remap color transfer function into the specified range
void CTK_VISUALIZATION_VTK_CORE_EXPORT remapColorScale(
  vtkDiscretizableColorTransferFunction* colorTransferFunction,
  double minRescale, double maxRescale);

/// \ingroup Visualization_VTK_Core
/// Remap color transfer function into the specified range.
/// \param colorTransferFunction is not modified and the output is copied to
/// into \param rescaledColorTranferFunction
void CTK_VISUALIZATION_VTK_CORE_EXPORT remapColorScale(
  vtkDiscretizableColorTransferFunction* colorTransferFunction,
  vtkDiscretizableColorTransferFunction* rescaledColorTransferFunction,
  double minRescale, double maxRescale);

/// \ingroup Visualization_VTK_Core
/// Reverse color transfer function
void CTK_VISUALIZATION_VTK_CORE_EXPORT reverseColorMap(vtkDiscretizableColorTransferFunction* colorTransferFunction);

/// \ingroup Visualization_VTK_Core
/// Set global opacity of the color transfer function.
/// Transparency is multiplied to each node opacity.
void CTK_VISUALIZATION_VTK_CORE_EXPORT setTransparency(vtkDiscretizableColorTransferFunction* colorTransferFunction, double transparency);

}

#endif
