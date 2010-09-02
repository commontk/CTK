/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

// Qt includes
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkVTKRenderView.h"
#include "ctkVTKRenderView_p.h"
#include "ctkLogger.h"

// VTK includes
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkCamera.h>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKRenderView");
//--------------------------------------------------------------------------

// --------------------------------------------------------------------------
// ctkVTKRenderViewPrivate methods

// --------------------------------------------------------------------------
ctkVTKRenderViewPrivate::ctkVTKRenderViewPrivate()
{
  this->Renderer = vtkSmartPointer<vtkRenderer>::New();
  this->RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  this->Axes = vtkSmartPointer<vtkAxesActor>::New();
  this->Orientation = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  this->CornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  this->RenderPending = false;
  this->RenderEnabled = false;
  this->ZoomFactor = 0.05;
  this->RotateDegrees = 5;
  this->PitchDirection = ctkVTKRenderView::PitchUp;
  this->RollDirection = ctkVTKRenderView::RollRight;
  this->YawDirection = ctkVTKRenderView::YawLeft;
}

// --------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::setupCornerAnnotation()
{
  logger.trace("setupCornerAnnotation");
  if (!this->Renderer->HasViewProp(this->CornerAnnotation))
    {
    this->Renderer->AddViewProp(this->CornerAnnotation);
    this->CornerAnnotation->SetMaximumLineHeight(0.07);
    vtkTextProperty *tprop = this->CornerAnnotation->GetTextProperty();
    tprop->ShadowOn();
    }
  this->CornerAnnotation->ClearAllTexts();
}

//---------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::setupRendering()
{
  logger.trace("setupRendering");
  Q_ASSERT(this->RenderWindow);
  this->RenderWindow->SetAlphaBitPlanes(1);
  this->RenderWindow->SetMultiSamples(0);
  this->RenderWindow->StereoCapableWindowOn();
  
  this->RenderWindow->GetRenderers()->RemoveAllItems();
  
  // Add renderer
  this->RenderWindow->AddRenderer(this->Renderer);
  
  // Setup the corner annotation
  this->setupCornerAnnotation();

  this->VTKWidget->SetRenderWindow(this->RenderWindow);
}

//---------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::setupDefaultInteractor()
{
  logger.trace("setupDefaultInteractor");
  CTK_P(ctkVTKRenderView);
  p->setInteractor(this->RenderWindow->GetInteractor());
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
// ctkVTKRenderView methods

// --------------------------------------------------------------------------
ctkVTKRenderView::ctkVTKRenderView(QWidget* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkVTKRenderView);
  CTK_D(ctkVTKRenderView);
  
  d->VTKWidget = new QVTKWidget(this);
  this->setLayout(new QVBoxLayout);
  this->layout()->setMargin(0);
  this->layout()->setSpacing(0);
  this->layout()->addWidget(d->VTKWidget);

  d->setupRendering();
  d->setupDefaultInteractor();
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::scheduleRender()
{
  CTK_D(ctkVTKRenderView);

  logger.trace(QString("scheduleRender - RenderEnabled: %1 - RenderPending: %2").
               arg(d->RenderEnabled).arg(d->RenderPending));

  if (!d->RenderEnabled)
    {
    return;
    }
  if (!d->RenderPending)
    {
    d->RenderPending = true;
    QTimer::singleShot(0, this, SLOT(forceRender()));
    }
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::forceRender()
{
  CTK_D(ctkVTKRenderView);

  logger.trace(QString("forceRender - RenderEnabled: %1").arg(d->RenderEnabled));

  if (!d->RenderEnabled)
    {
    return;
    }
  d->RenderWindow->Render();
  d->RenderPending = false;
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, vtkRenderWindow*, renderWindow, RenderWindow);

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, vtkRenderWindowInteractor*, interactor, CurrentInteractor);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setInteractor(vtkRenderWindowInteractor* newInteractor)
{
  CTK_D(ctkVTKRenderView);

  logger.trace("setInteractor");

  d->RenderWindow->SetInteractor(newInteractor);
  d->Orientation->SetOrientationMarker(d->Axes);
  d->Orientation->SetInteractor(newInteractor);
  d->Orientation->SetEnabled(1);
  d->Orientation->InteractiveOff();
  d->CurrentInteractor = newInteractor; 
}

//----------------------------------------------------------------------------
vtkInteractorObserver* ctkVTKRenderView::interactorStyle()
{
  CTK_D(ctkVTKRenderView);
  if (d->CurrentInteractor)
    {
    return d->CurrentInteractor->GetInteractorStyle();
    }
  else
    {
    return 0;
    }
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::setCornerAnnotationText(const QString& text)
{
  CTK_D(ctkVTKRenderView);
  logger.trace(QString("setCornerAnnotationText: %1").arg(text));
  d->CornerAnnotation->ClearAllTexts();
  d->CornerAnnotation->SetText(2, text.toLatin1());
}

//----------------------------------------------------------------------------
QString ctkVTKRenderView::cornerAnnotationText() const
{
  CTK_D(const ctkVTKRenderView);
  return QLatin1String(d->CornerAnnotation->GetText(2));
}

// --------------------------------------------------------------------------
void ctkVTKRenderView::setBackgroundColor(const QColor& newBackgroundColor)
{
  CTK_D(ctkVTKRenderView);

  logger.trace(QString("setBackgroundColor: %1").arg(newBackgroundColor.name()));

  d->Renderer->SetBackground(newBackgroundColor.redF(),
                             newBackgroundColor.greenF(),
                             newBackgroundColor.blueF());
}

//----------------------------------------------------------------------------
QColor ctkVTKRenderView::backgroundColor() const
{
  CTK_D(const ctkVTKRenderView);
  double color[3] = {0, 0, 0};
  d->Renderer->GetBackground(color);
  return QColor::fromRgbF(color[0], color[1], color[2]);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::setOrientationWidgetVisible(bool visible)
{
  CTK_D(ctkVTKRenderView);
  d->Orientation->SetEnabled(visible);
}

//----------------------------------------------------------------------------
bool ctkVTKRenderView::orientationWidgetVisible()
{
  CTK_D(ctkVTKRenderView);
  return d->Orientation->GetEnabled();
}

//----------------------------------------------------------------------------
vtkCamera* ctkVTKRenderView::activeCamera()
{
  CTK_D(ctkVTKRenderView);
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
  CTK_D(ctkVTKRenderView);
  logger.trace("resetCamera");
  d->Renderer->ResetCamera();
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, vtkRenderer*, renderer, Renderer);

//----------------------------------------------------------------------------
CTK_SET_CXX(ctkVTKRenderView, bool, setRenderEnabled, RenderEnabled);
CTK_GET_CXX(ctkVTKRenderView, bool, renderEnabled, RenderEnabled);

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, int, rotateDegrees, RotateDegrees);

//----------------------------------------------------------------------------
void ctkVTKRenderView::setRotateDegrees(int newRotateDegrees)
{
  CTK_D(ctkVTKRenderView);
  d->RotateDegrees = qAbs(newRotateDegrees);
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, ctkVTKRenderView::PitchDirection, pitchDirection, PitchDirection);
CTK_SET_CXX(ctkVTKRenderView, ctkVTKRenderView::PitchDirection, setPitchDirection, PitchDirection);

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, ctkVTKRenderView::RollDirection, rollDirection, RollDirection);
CTK_SET_CXX(ctkVTKRenderView, ctkVTKRenderView::RollDirection, setRollDirection, RollDirection);

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, ctkVTKRenderView::YawDirection, yawDirection, YawDirection);
CTK_SET_CXX(ctkVTKRenderView, ctkVTKRenderView::YawDirection, setYawDirection, YawDirection);

//----------------------------------------------------------------------------
void ctkVTKRenderView::pitch()
{
  CTK_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  vtkCamera *cam = d->Renderer->GetActiveCamera();
  cam->Elevation(d->PitchDirection == Self::PitchDown ? d->RotateDegrees : -d->RotateDegrees);
  cam->OrthogonalizeViewUp();
  d->Renderer->UpdateLightsGeometryToFollowCamera();
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::roll()
{
  CTK_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  vtkCamera *cam = d->Renderer->GetActiveCamera();
  cam->Roll(d->RollDirection == Self::RollLeft ? d->RotateDegrees : -d->RotateDegrees);
  cam->OrthogonalizeViewUp();
  d->Renderer->UpdateLightsGeometryToFollowCamera();
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::yaw()
{
  CTK_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  vtkCamera *cam = d->Renderer->GetActiveCamera();
  cam->Azimuth(d->YawDirection == Self::YawLeft ? d->RotateDegrees : -d->RotateDegrees);
  cam->OrthogonalizeViewUp();
  d->Renderer->UpdateLightsGeometryToFollowCamera();
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::setZoomFactor(double newZoomFactor)
{
  CTK_D(ctkVTKRenderView);
  d->ZoomFactor = qBound(0.0, qAbs(newZoomFactor), 1.0);
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, double, zoomFactor, ZoomFactor);

//----------------------------------------------------------------------------
void ctkVTKRenderView::zoomIn()
{
  CTK_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  d->zoom(d->ZoomFactor);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::zoomOut()
{
  CTK_D(ctkVTKRenderView);
  if (!d->Renderer->IsActiveCameraCreated())
    {
    return;
    }
  d->zoom(-d->ZoomFactor);
}
