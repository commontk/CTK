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

// Qt includes
#include <QTimer>
#include <QVBoxLayout>
#include <QDebug>

// CTK includes
#include "ctkVTKRenderView.h"
#include "ctkVTKRenderView_p.h"

// VTK includes
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkCamera.h>

// --------------------------------------------------------------------------
// ctkVTKRenderViewPrivate methods

// --------------------------------------------------------------------------
ctkVTKRenderViewPrivate::ctkVTKRenderViewPrivate(ctkVTKRenderView& object)
  :ctkVTKAbstractViewPrivate(object)
{
  qRegisterMetaType<ctkAxesWidget::Axis>("ctkAxesWidget::Axis");
  this->Renderer = vtkSmartPointer<vtkRenderer>::New();
  this->Axes = vtkSmartPointer<vtkAxesActor>::New();
  this->Orientation = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  this->ZoomFactor = 0.05;
  this->PitchRollYawIncrement = 5;
  this->PitchDirection = ctkVTKRenderView::PitchUp;
  this->RollDirection = ctkVTKRenderView::RollRight;
  this->YawDirection = ctkVTKRenderView::YawLeft;
  this->SpinDirection = ctkVTKRenderView::YawRight;
  this->SpinEnabled = false;
  this->AnimationIntervalMs = 5;
  this->SpinIncrement = 2;
  this->RockEnabled = false;
  this->RockIncrement = 0;
  this->RockLength = 200;

  this->Orientation->SetOrientationMarker(this->Axes);
}

// --------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::setupCornerAnnotation()
{
  this->ctkVTKAbstractViewPrivate::setupCornerAnnotation();
  if (!this->Renderer->HasViewProp(this->CornerAnnotation))
    {
    this->Renderer->AddViewProp(this->CornerAnnotation);
    }
}

//---------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::setupRendering()
{
  // Add renderer
  this->RenderWindow->AddRenderer(this->Renderer);
  this->ctkVTKAbstractViewPrivate::setupRendering();
}

//----------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::zoom(double zoomFactor)
{
  Q_ASSERT(this->Renderer->IsActiveCameraCreated());
  vtkCamera * camera = this->Renderer->GetActiveCamera();

  if (camera->GetParallelProjection())
    {
    camera->SetParallelScale(camera->GetParallelScale() / (1 + zoomFactor));
    }
  else
    {
    camera->Dolly(1 + zoomFactor);
    this->Renderer->ResetCameraClippingRange();
    this->Renderer->UpdateLightsGeometryToFollowCamera();
    }
}

//---------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::pitch(double rotateDegrees,
                                    ctkVTKRenderView::RotateDirection pitchDirection)
{
  Q_ASSERT(this->Renderer->IsActiveCameraCreated());
  Q_ASSERT(rotateDegrees >= 0);
  vtkCamera *cam = this->Renderer->GetActiveCamera();
  cam->Elevation(pitchDirection == ctkVTKRenderView::PitchDown ? rotateDegrees : -rotateDegrees);
  cam->OrthogonalizeViewUp();
  this->Renderer->UpdateLightsGeometryToFollowCamera();
}

//---------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::roll(double rotateDegrees,
                                    ctkVTKRenderView::RotateDirection rollDirection)
{
  Q_ASSERT(this->Renderer->IsActiveCameraCreated());
  Q_ASSERT(rotateDegrees >= 0);

  vtkCamera *cam = this->Renderer->GetActiveCamera();
  cam->Roll(rollDirection == ctkVTKRenderView::RollLeft ? rotateDegrees : -rotateDegrees);
  cam->OrthogonalizeViewUp();
  this->Renderer->UpdateLightsGeometryToFollowCamera();
}

//---------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::yaw(double rotateDegrees,
                                    ctkVTKRenderView::RotateDirection yawDirection)
{
  Q_ASSERT(this->Renderer->IsActiveCameraCreated());
  Q_ASSERT(rotateDegrees >= 0);
  vtkCamera *cam = this->Renderer->GetActiveCamera();
  cam->Azimuth(yawDirection == ctkVTKRenderView::YawLeft ? rotateDegrees : -rotateDegrees);
  cam->OrthogonalizeViewUp();
  this->Renderer->UpdateLightsGeometryToFollowCamera();
}

//---------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::doSpin()
{
  Q_Q(ctkVTKRenderView);
  if (!this->SpinEnabled)
    {
    return;
    }

  switch (this->SpinDirection)
    {
    case ctkVTKRenderView::PitchUp:
    case ctkVTKRenderView::PitchDown:
      this->pitch(this->SpinIncrement, this->SpinDirection);
      break;
    case ctkVTKRenderView::RollLeft:
    case ctkVTKRenderView::RollRight:
      this->roll(this->SpinIncrement, this->SpinDirection);
      break;
    case ctkVTKRenderView::YawLeft:
    case ctkVTKRenderView::YawRight:
      this->yaw(this->SpinIncrement, this->SpinDirection);
      break;
    }

  q->forceRender();
  QTimer::singleShot(this->AnimationIntervalMs, this, SLOT(doSpin()));
}

//---------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::doRock()
{
  Q_Q(ctkVTKRenderView);
  Q_ASSERT(this->Renderer->IsActiveCameraCreated());

  if (!this->RockEnabled)
    {
    return;
    }

  vtkCamera *camera = this->Renderer->GetActiveCamera();

  double frac = static_cast<double>(this->RockIncrement) / static_cast<double>(this->RockLength);
  double az = 1.5 * cos (2.0 * 3.1415926 * (frac - floor(frac)));
  this->RockIncrement++;
  this->RockIncrement = this->RockIncrement % this->RockLength;

  // Move the camera
  camera->Azimuth(az);
  camera->OrthogonalizeViewUp();

  //Make the lighting follow the camera to avoid illumination changes
  this->Renderer->UpdateLightsGeometryToFollowCamera();

  q->forceRender();
  QTimer::singleShot(this->AnimationIntervalMs, this, SLOT(doRock()));
}

//---------------------------------------------------------------------------
// ctkVTKRenderView methods

// --------------------------------------------------------------------------
ctkVTKRenderView::ctkVTKRenderView(QWidget* parentWidget)
  : Superclass(new ctkVTKRenderViewPrivate(*this), parentWidget)
{
  Q_D(ctkVTKRenderView);
  d->init();

  // The interactor in RenderWindow exists after the renderwindow is set to
  // the QVTKWidet
  d->Orientation->SetInteractor(d->RenderWindow->GetInteractor());
  d->Orientation->SetEnabled(1);
  d->Orientation->InteractiveOff();
}

//----------------------------------------------------------------------------
ctkVTKRenderView::~ctkVTKRenderView()
{
}
//----------------------------------------------------------------------------
void ctkVTKRenderView::setInteractor(vtkRenderWindowInteractor* newInteractor)
{
  Q_D(ctkVTKRenderView);
  this->Superclass::setInteractor(newInteractor);
  d->Orientation->SetInteractor(newInteractor);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::setOrientationWidgetVisible(bool visible)
{
  Q_D(ctkVTKRenderView);
  d->Orientation->SetEnabled(visible);
}

//----------------------------------------------------------------------------
bool ctkVTKRenderView::orientationWidgetVisible()
{
  Q_D(ctkVTKRenderView);
  return d->Orientation->GetEnabled();
}

//----------------------------------------------------------------------------
vtkCamera* ctkVTKRenderView::activeCamera()
{
  Q_D(ctkVTKRenderView);
  if (d->Renderer->IsActiveCameraCreated())
    {
    return d->Renderer->GetActiveCamera();
    }
  else
    {
    return 0;
    }
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::resetCamera()
{
  Q_D(ctkVTKRenderView);
  d->Renderer->ResetCamera();
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, vtkRenderer*, renderer, Renderer);

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, double, pitchRollYawIncrement, PitchRollYawIncrement);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setPitchRollYawIncrement(double newPitchRollYawIncrement)
{
  Q_D(ctkVTKRenderView);
  d->PitchRollYawIncrement = qAbs(newPitchRollYawIncrement);
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, ctkVTKRenderView::RotateDirection, pitchDirection, PitchDirection);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setPitchDirection(ctkVTKRenderView::RotateDirection newPitchDirection)
{
  Q_D(ctkVTKRenderView);
  if (newPitchDirection != ctkVTKRenderView::PitchUp &&
      newPitchDirection != ctkVTKRenderView::PitchDown)
    {
    return;
    }
  d->PitchDirection = newPitchDirection;
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, ctkVTKRenderView::RotateDirection, rollDirection, RollDirection);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setRollDirection(ctkVTKRenderView::RotateDirection newRollDirection)
{
  Q_D(ctkVTKRenderView);
  if (newRollDirection != ctkVTKRenderView::RollLeft &&
      newRollDirection != ctkVTKRenderView::RollRight)
    {
    return;
    }
  d->RollDirection = newRollDirection;
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, ctkVTKRenderView::RotateDirection, yawDirection, YawDirection);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setYawDirection(ctkVTKRenderView::RotateDirection newYawDirection)
{
  Q_D(ctkVTKRenderView);
  if (newYawDirection != ctkVTKRenderView::YawLeft &&
      newYawDirection != ctkVTKRenderView::YawRight)
    {
    return;
    }
  d->YawDirection = newYawDirection;
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, ctkVTKRenderView::RotateDirection, spinDirection, SpinDirection);
CTK_SET_CPP(ctkVTKRenderView, ctkVTKRenderView::RotateDirection, setSpinDirection, SpinDirection);

//----------------------------------------------------------------------------
void ctkVTKRenderView::pitch()
{
  Q_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  d->pitch(d->PitchRollYawIncrement, d->PitchDirection);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::roll()
{
  Q_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  d->roll(d->PitchRollYawIncrement, d->RollDirection);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::yaw()
{
  Q_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  d->yaw(d->PitchRollYawIncrement, d->YawDirection);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::setSpinEnabled(bool enabled)
{
  Q_D(ctkVTKRenderView);
  if (enabled == d->SpinEnabled)
    {
    return;
    }
  d->SpinEnabled = enabled;
  d->RockEnabled = false;

  QTimer::singleShot(0, d, SLOT(doSpin()));
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, bool, spinEnabled, SpinEnabled);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setSpinIncrement(double newSpinIncrement)
{
  Q_D(ctkVTKRenderView);
  d->SpinIncrement = qAbs(newSpinIncrement);
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, double, spinIncrement, SpinIncrement);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setAnimationIntervalMs(int newAnimationIntervalMs)
{
  Q_D(ctkVTKRenderView);
  d->AnimationIntervalMs = qAbs(newAnimationIntervalMs);
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, int, animationIntervalMs, AnimationIntervalMs);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setRockEnabled(bool enabled)
{
  Q_D(ctkVTKRenderView);
  if (enabled == d->RockEnabled)
    {
    return;
    }
  d->RockEnabled = enabled;
  d->SpinEnabled = false;

  QTimer::singleShot(0, d, SLOT(doRock()));
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, bool, rockEnabled, RockEnabled);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setRockLength(int newRockLength)
{
  Q_D(ctkVTKRenderView);
  d->RockLength = qAbs(newRockLength);
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, int, rockLength, RockLength);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setRockIncrement(int newRockIncrement)
{
  Q_D(ctkVTKRenderView);
  d->RockIncrement = qAbs(newRockIncrement);
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, int, rockIncrement, RockIncrement);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setZoomFactor(double newZoomFactor)
{
  Q_D(ctkVTKRenderView);
  d->ZoomFactor = qBound(0.0, qAbs(newZoomFactor), 1.0);
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKRenderView, double, zoomFactor, ZoomFactor);

//----------------------------------------------------------------------------
void ctkVTKRenderView::zoomIn()
{
  Q_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  d->zoom(d->ZoomFactor);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::zoomOut()
{
  Q_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  d->zoom(-d->ZoomFactor);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::setFocalPoint(double x, double y, double z)
{
  Q_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  vtkCamera * camera = d->Renderer->GetActiveCamera();
  camera->SetFocalPoint(x, y, z);
  camera->ComputeViewPlaneNormal();
  camera->OrthogonalizeViewUp();
  d->Renderer->UpdateLightsGeometryToFollowCamera();
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::resetFocalPoint()
{
  Q_D(ctkVTKRenderView);
  double bounds[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  d->Renderer->ComputeVisiblePropBounds(bounds);
  double x_center = (bounds[1] + bounds[0]) / 2.0;
  double y_center = (bounds[3] + bounds[2]) / 2.0;
  double z_center = (bounds[5] + bounds[4]) / 2.0;
  this->setFocalPoint(x_center, y_center, z_center);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::lookFromAxis(const ctkAxesWidget::Axis& axis, double fov)
{
  Q_D(ctkVTKRenderView);
  Q_ASSERT(d->Renderer);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  vtkCamera * camera = d->Renderer->GetActiveCamera();
  Q_ASSERT(camera);
  double cameraDistance = camera->GetDistance();
  double* focalPoint = camera->GetFocalPoint();
  switch (axis)
    {
    case ctkAxesWidget::Right:
      camera->SetPosition(focalPoint[0]+cameraDistance, focalPoint[1], focalPoint[2]);
      camera->SetViewUp(0, 0, 1);
      break;
    case ctkAxesWidget::Left:
      camera->SetPosition(focalPoint[0]-cameraDistance, focalPoint[1], focalPoint[2]);
      camera->SetViewUp(0, 0, 1);
      break;
    case ctkAxesWidget::Anterior:
      camera->SetPosition(focalPoint[0], focalPoint[1]+cameraDistance, focalPoint[2]);
      camera->SetViewUp(0, 0, 1);
      break;
    case ctkAxesWidget::Posterior:
      camera->SetPosition(focalPoint[0], focalPoint[1]-cameraDistance, focalPoint[2]);
      camera->SetViewUp(0, 0, 1);
      break;
    case ctkAxesWidget::Superior:
      camera->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2]+cameraDistance);
      camera->SetViewUp(0, 1, 0);
      break;
    case ctkAxesWidget::Inferior:
      camera->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2]-cameraDistance);
      camera->SetViewUp(0, 1, 0);
      break;
    case ctkAxesWidget::None:
    default:
      // do nothing
      return;
      break;
    }
  d->Renderer->ResetCameraClippingRange();
  camera->ComputeViewPlaneNormal();
  camera->OrthogonalizeViewUp();
  d->Renderer->UpdateLightsGeometryToFollowCamera();
}
