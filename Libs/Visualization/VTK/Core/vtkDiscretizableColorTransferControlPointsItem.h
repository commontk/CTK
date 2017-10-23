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

#ifndef __vtkDiscretizableColorTransferControlPointsItem_h
#define __vtkDiscretizableColorTransferControlPointsItem_h

#include "ctkVisualizationVTKCoreExport.h"
#include <vtkCompositeControlPointsItem.h>

class vtkContextMouseEvent;

/// \ingroup Visualization_VTK_Core
///
/// Special control points item class that overriding mouse events
/// to add points only when close to the transfer function.
class CTK_VISUALIZATION_VTK_CORE_EXPORT vtkDiscretizableColorTransferControlPointsItem
  : public vtkCompositeControlPointsItem
{
public:
  vtkTypeMacro(vtkDiscretizableColorTransferControlPointsItem,
    vtkCompositeControlPointsItem)
  static vtkDiscretizableColorTransferControlPointsItem* New();

  bool MouseMoveEvent(const vtkContextMouseEvent &mouse) VTK_OVERRIDE;
  bool MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse) VTK_OVERRIDE;
  bool MouseButtonPressEvent(const vtkContextMouseEvent& mouse) VTK_OVERRIDE;

  bool IsProcessing();
  void StartProcessing();
  void EndProcessing();

protected:
  vtkDiscretizableColorTransferControlPointsItem();
  virtual ~vtkDiscretizableColorTransferControlPointsItem();

  /// Utility function to determine whether a position is near the piecewise
  /// function.
  bool PointNearPiecewiseFunction(const double pos[2]);

private:
  vtkDiscretizableColorTransferControlPointsItem(
    const vtkDiscretizableColorTransferControlPointsItem&); // Not implemented.
  void operator=(const vtkDiscretizableColorTransferControlPointsItem&); // Not implemented.
};

#endif
