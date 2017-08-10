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

#ifndef __ctkVTKRenderView_p_h
#define __ctkVTKRenderView_p_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkPimpl.h>
#include "ctkVTKAbstractView_p.h"
#include <ctkVTKObject.h>
#include "ctkVTKRenderView.h"

// VTK includes
#include <vtkAxesActor.h>
#include <vtkCornerAnnotation.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

class vtkRenderWindowInteractor;

//-----------------------------------------------------------------------------
/// \ingroup Visualization_VTK_Widgets
class ctkVTKRenderViewPrivate : public ctkVTKAbstractViewPrivate
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkVTKRenderView);

public:
  ctkVTKRenderViewPrivate(ctkVTKRenderView& object);

  /// Convenient setup methods
  virtual void setupCornerAnnotation();
  virtual void setupRendering();

  void zoom(double zoomFactor);

  void pitch(double rotateDegrees, ctkVTKRenderView::RotateDirection pitchDirection);
  void roll(double rotateDegrees, ctkVTKRenderView::RotateDirection rollDirection);
  void yaw(double rotateDegrees, ctkVTKRenderView::RotateDirection yawDirection);

public Q_SLOTS:
  void doSpin();
  void doRock();

public:

  vtkSmartPointer<vtkRenderer>                  Renderer;

  vtkSmartPointer<vtkAxesActor>                 Axes;
  vtkSmartPointer<vtkOrientationMarkerWidget>   Orientation;

  double                                        ZoomFactor;
  double                                        PitchRollYawIncrement;
  ctkVTKRenderView::RotateDirection             PitchDirection;
  ctkVTKRenderView::RotateDirection             RollDirection;
  ctkVTKRenderView::RotateDirection             YawDirection;
  ctkVTKRenderView::RotateDirection             SpinDirection;
  bool                                          SpinEnabled;
  int                                           AnimationIntervalMs;
  double                                        SpinIncrement;
  bool                                          RockEnabled;
  int                                           RockIncrement;
  int                                           RockLength;
};

#endif
