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

#include "vtkDiscretizableColorTransferControlPointsItem.h"
#include <vtkContextMouseEvent.h>
#include <vtkObjectFactory.h>
#include <vtkPiecewiseFunction.h>

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkDiscretizableColorTransferControlPointsItem)

// ----------------------------------------------------------------------------
vtkDiscretizableColorTransferControlPointsItem::
  vtkDiscretizableColorTransferControlPointsItem()
{
}

// ----------------------------------------------------------------------------
vtkDiscretizableColorTransferControlPointsItem::
  ~vtkDiscretizableColorTransferControlPointsItem()
{
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferControlPointsItem::MouseButtonPressEvent(
  const vtkContextMouseEvent& mouse)
{
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON)
  {
    vtkVector2f vpos = mouse.GetPos();
    this->TransformScreenToData(vpos, vpos);
    double pos[2];
    pos[0] = vpos.GetX();
    pos[1] = vpos.GetY();

    bool pointOnFunction = this->PointNearPiecewiseFunction(pos);
    if (!pointOnFunction)
    {
      this->SetCurrentPoint(-1);
      return false;
    }
  }

  return this->Superclass::MouseButtonPressEvent(mouse);
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferControlPointsItem::
  MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse)
{
  // If no point is selected, abort event
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON &&
    this->GetCurrentPoint() < 0)
  {
    return false;
  }

  return this->Superclass::MouseButtonReleaseEvent(mouse);
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferControlPointsItem::MouseMoveEvent(
  const vtkContextMouseEvent &mouse)
{
  // If no point is selected, abort event
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON &&
    this->GetCurrentPoint() < 0)
  {
    return false;
  }

   return this->Superclass::MouseMoveEvent(mouse);
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferControlPointsItem::
  PointNearPiecewiseFunction(const double position[2])
{
  double x = position[0];
  double y = 0.0;

  vtkPiecewiseFunction* pwf = this->GetOpacityFunction();
  if (!pwf) {
    return false;
  }

  // Evaluate the piewewise function at the given point and get the y position.
  // If we are within a small distance of the piecewise function, return true.
  // Otherwise, we are too far away from the line, and return false.
  pwf->GetTable(x, x, 1, &y, 1);
  return (fabs(y - position[1]) < 0.05);
}

// ----------------------------------------------------------------------------
bool vtkDiscretizableColorTransferControlPointsItem::IsProcessing()
{
  return this->StartedChanges > 0;
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferControlPointsItem::StartProcessing()
{
  this->StartChanges();
}

// ----------------------------------------------------------------------------
void vtkDiscretizableColorTransferControlPointsItem::EndProcessing()
{
  this->EndChanges();
}
