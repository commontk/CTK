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
#include <QDebug>

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKThumbnailView.h"

// VTK includes
#include <vtkCamera.h>
#include <vtkFollower.h>
#include <vtkInteractorStyle.h>
#include <vtkMath.h>
#include <vtkOutlineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>
#include <vtkWeakPointer.h>

//--------------------------------------------------------------------------
static ctkLogger logger("org.slicer.libs.qmrmlwidgets.ctkVTKThumbnailView");
//--------------------------------------------------------------------------

#define DEGREES2RADIANS 0.0174532925

//-----------------------------------------------------------------------------
class ctkVTKThumbnailViewPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKThumbnailView);
protected:
  ctkVTKThumbnailView* const q_ptr;
public:
  ctkVTKThumbnailViewPrivate(ctkVTKThumbnailView& object);
  ~ctkVTKThumbnailViewPrivate();
  
  void init();
  void initCamera();
  void updateBounds();
  void updateCamera();
  void resetCamera();

  vtkRenderer*                       Renderer;
  vtkWeakPointer<vtkRenderWindowInteractor> Interactor;
  
  vtkOutlineSource*                  FOVBox;
  vtkPolyDataMapper*                 FOVBoxMapper;
  vtkFollower*                       FOVBoxActor;
};

//--------------------------------------------------------------------------
// ctkVTKThumbnailViewPrivate methods

//---------------------------------------------------------------------------
ctkVTKThumbnailViewPrivate::ctkVTKThumbnailViewPrivate(ctkVTKThumbnailView& object)
  : q_ptr(&object)
{
  this->Renderer = 0;
  this->Interactor = 0;

  this->FOVBox = 0;
  this->FOVBoxMapper = 0;
  this->FOVBoxActor = 0;
}

//---------------------------------------------------------------------------
ctkVTKThumbnailViewPrivate::~ctkVTKThumbnailViewPrivate()
{
  this->FOVBox->Delete();
  this->FOVBoxMapper->Delete();
  this->FOVBoxActor->Delete();
}

//---------------------------------------------------------------------------
void ctkVTKThumbnailViewPrivate::init()
{
  Q_Q(ctkVTKThumbnailView);

  this->FOVBox = vtkOutlineSource::New();
  this->FOVBoxMapper = vtkPolyDataMapper::New();
  this->FOVBoxMapper->SetInputConnection( this->FOVBox->GetOutputPort() );
  this->FOVBoxActor = vtkFollower::New();

  this->FOVBoxMapper->Update();
   
  this->FOVBoxActor->SetMapper( this->FOVBoxMapper );
  this->FOVBoxActor->SetPickable(0);
  this->FOVBoxActor->SetDragable(0);
  this->FOVBoxActor->SetVisibility(1);
  this->FOVBoxActor->SetScale(1.0, 1.0, 1.0);
  this->FOVBoxActor->GetProperty()->SetColor( 0.1, 0.45, 0.1 );
  this->FOVBoxActor->GetProperty()->SetLineWidth (2.0);

  q->renderWindow()->GetInteractor()->SetInteractorStyle(0);
  
  // not sure what's for...
  q->qvtkConnect(q->renderWindow(), vtkCommand::AbortCheckEvent,
                 q, SLOT(checkAbort()));
}

//---------------------------------------------------------------------------
void ctkVTKThumbnailViewPrivate::initCamera()
{
  Q_Q(ctkVTKThumbnailView);
  vtkRenderer* ren = this->Renderer;
  if (!ren)
    {
    return;
    }
  vtkCamera *cam = ren->IsActiveCameraCreated() ? ren->GetActiveCamera() : NULL;
  if (!cam)
    {
    return;
    }
  vtkCamera *navcam = q->activeCamera();
  if (!navcam)
    {
    return;
    }
  navcam->SetPosition ( cam->GetPosition() );
  navcam->SetFocalPoint ( cam->GetFocalPoint() );
  navcam->ComputeViewPlaneNormal();
  navcam->SetViewUp( cam->GetViewUp() );
  this->FOVBoxActor->SetCamera (navcam);
  this->FOVBox->SetBoxTypeToOriented ( );
}

//---------------------------------------------------------------------------
void ctkVTKThumbnailViewPrivate::updateBounds()
{
  Q_Q(ctkVTKThumbnailView);
  vtkRenderer* ren = this->Renderer;
  
  vtkActorCollection *mainActors;
  vtkActor *mainActor;
  vtkActor *newActor;
  vtkPolyDataMapper *newMapper;

  // iterate thru the actor collection,
  // remove item, delete actor, delete mapper.
  vtkActorCollection *navActors = q->renderer()->GetActors();
    
  if (!navActors)
    {
    q->renderer()->RemoveAllViewProps();
    navActors->RemoveAllItems();
    }
  if (!ren)
    {
    return;
    }

  double bounds[6];
  double dimension;
  double x,y,z;
  double cutoff = 0.1;
  double cutoffDimension;
  
  ren->ComputeVisiblePropBounds( bounds );
  x = bounds[1] - bounds[0];
  y = bounds[3] - bounds[2];
  z = bounds[5] - bounds[4];
  dimension = x*x + y*y + z*z;
  cutoffDimension = cutoff * dimension;

  // Get actor collection from the main viewer's renderer
  mainActors = ren->GetActors();
  if (!mainActors)
    {
    return;
    }

  // add the little FOV box to NavigationWidget's actors
  q->renderer()->AddViewProp(this->FOVBoxActor);

  for(mainActors->InitTraversal(); (mainActor = mainActors->GetNextActor()); )
    {
    // get the bbox of this actor
    int vis = mainActor->GetVisibility();
    //if (vis)
    //  {
      mainActor->GetBounds ( bounds );
      // check to see if it's big enough to include in the scene...
      x = bounds[1] - bounds[0];
      y = bounds[3] - bounds[2];
      z = bounds[5] - bounds[4];
      dimension = x*x + y*y + z*z;
     // }
    // add a copy of the actor to the renderer
    // only if it's big enough to count (don't bother with tiny
    // and don't bother with invisible stuff)
    if ( dimension > cutoffDimension && vis )
      {
      // ---new: create new actor, mapper, deep copy, add it.
      newMapper = vtkPolyDataMapper::New();
      newMapper->ShallowCopy (mainActor->GetMapper() );
      newMapper->SetInputData ( vtkPolyData::SafeDownCast(mainActor->GetMapper()->GetInput()) );

      newActor = vtkActor::New();
      newActor->ShallowCopy (mainActor );
      newActor->SetMapper ( newMapper );
      newMapper->Delete();
      
      q->renderer()->AddActor( newActor );
      newActor->Delete();
      }
    }
}

//---------------------------------------------------------------------------
void ctkVTKThumbnailViewPrivate::updateCamera()
{
  Q_Q(ctkVTKThumbnailView);
   // Scale the FOVBox actor to show the
  // MainViewer's window on the scene.
  vtkRenderer *ren = this->Renderer;
  vtkCamera *cam = ren ? (ren->IsActiveCameraCreated() ? ren->GetActiveCamera() : NULL) : NULL;
  if (!cam)
    {
    return;
    }
  
  // 3DViewer's camera configuration
  double *focalPoint = cam->GetFocalPoint ( );
  double *camPos= cam->GetPosition ( );
  double *vpn = cam->GetViewPlaneNormal ();
  double thetaV = (cam->GetViewAngle()) / 2.0;

  // camera distance, and distance of FOVBox from focalPoint
  double camDist = cam->GetDistance ();
  double boxDist = camDist * 0.89;

  // configure navcam based on main renderer's camera
  vtkRenderer *navren = q->renderer();
  vtkCamera *navcam = q->activeCamera();

  if ( navcam == 0 )
    {
    return;
    }
  // give navcam the same parameters as MainViewer's ActiveCamera
  navcam->SetPosition ( camPos );
  navcam->SetFocalPoint ( focalPoint );
  navcam->SetViewUp( cam->GetViewUp() );
  navcam->ComputeViewPlaneNormal ( );

  // compute FOVBox height & width to correspond 
  // to the main viewer's size and aspect ratio, in world-coordinates,
  // positioned just behind the near clipping plane, to make sure
  // nothing in the scene occludes it.
  double boxHalfHit;
  if ( cam->GetParallelProjection() )
    {
    boxHalfHit = cam->GetParallelScale();
    }
  else
    {
    boxHalfHit = (camDist) * tan ( thetaV * DEGREES2RADIANS);
    }

  // 3D MainViewer height and width for computing aspect
  int mainViewerWid = ren->GetRenderWindow()->GetSize()[0];
  int mainViewerHit = ren->GetRenderWindow()->GetSize()[1];
  // width of the FOVBox that represents MainViewer window.
  double boxHalfWid = boxHalfHit * static_cast<double>(mainViewerWid)
                       / static_cast<double>(mainViewerHit);

  // configure and position the FOVBox
  double data [24];
  data[0] = -1.0;
  data[1] = -1.0;
  data[2] = 0.0;
  data[3] = 1.0;
  data[4] = -1.0;
  data[5] = 0.0;
  data[6] = -1.0;
  data[7] = 1.0;
  data[8] = 0.0;
  data[9] = 1.0;
  data[10] = 1.0;
  data[11] = 0.0;

  data[12] = -1.0;
  data[13] = -1.0;
  data[14] = 0.0;
  data[15] = 1.0;
  data[16] = -1.0;
  data[17] = 0.0;
  data[18] = -1.0;
  data[19] = 1.0;
  data[20] = 0.0;
  data[21] = 1.0;
  data[22] = 1.0;
  data[23] = 0.0;
  this->FOVBox->SetCorners ( data );
  // Position and scale FOVBox very close to camera,
  // to prevent things in the scene from occluding it.
  this->FOVBoxActor->SetScale ( boxHalfWid, boxHalfHit, 1.0);
  this->FOVBoxActor->SetPosition (focalPoint[0]+ (vpn[0]*boxDist),
                                  focalPoint[1] + (vpn[1]*boxDist),
                                  focalPoint[2] + (vpn[2]*boxDist));
  this->resetCamera();
  // put the focal point back into the center of
  // the scene without the FOVBox included,
  // since ResetNavigationCamera moved it.
  navcam->SetFocalPoint ( focalPoint );
  navren->ResetCameraClippingRange();
  navren->UpdateLightsGeometryToFollowCamera();
}

// ----------------------------------------------------------------------------
void ctkVTKThumbnailViewPrivate::resetCamera()
{
  Q_Q(ctkVTKThumbnailView);
  
  vtkRenderer *ren = q->renderer();
  vtkCamera *cam = q->activeCamera();
  
  if (!ren || !cam)
    {
    logger.error("Trying to reset non-existant camera");
    return;
    }
  
  double bounds[6];
  ren->ComputeVisiblePropBounds( bounds );
  
  if (!vtkMath::AreBoundsInitialized(bounds))
    {
    logger.error("Cannot reset camera!");
    return;
    }
  ren->InvokeEvent(vtkCommand::ResetCameraEvent, ren);

  double vn[3];
  cam->GetViewPlaneNormal(vn);

  double center[3];
  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;

  double w1 = bounds[1] - bounds[0];
  double w2 = bounds[3] - bounds[2];
  double w3 = bounds[5] - bounds[4];
  w1 *= w1;
  w2 *= w2;
  w3 *= w3;
  double radius = w1 + w2 + w3;

  // If we have just a single point, pick a radius of 1.0
  radius = (radius==0)?(1.0):(radius);

  // compute the radius of the enclosing sphere
  radius = sqrt(radius)*0.5;

  // default so that the bounding sphere fits within the view fustrum
  // compute the distance from the intersection of the view frustum with the
  // bounding sphere. Basically in 2D draw a circle representing the bounding
  // sphere in 2D then draw a horizontal line going out from the center of
  // the circle. That is the camera view. Then draw a line from the camera
  // position to the point where it intersects the circle. (it will be tangent
  // to the circle at this point, this is important, only go to the tangent
  // point, do not draw all the way to the view plane). Then draw the radius
  // from the tangent point to the center of the circle. You will note that
  // this forms a right triangle with one side being the radius, another being
  // the target distance for the camera, then just find the target dist using
  // a sin.
  double viewAngle = 20.0;
  double distance = radius/sin(viewAngle*vtkMath::Pi()/360.0);

  // check view-up vector against view plane normal
  double* vup = cam->GetViewUp();
  if ( fabs(vtkMath::Dot(vup,vn)) > 0.999 )
    {
    logger.warn("Resetting view-up since view plane normal is parallel");
    cam->SetViewUp(-vup[2], vup[0], vup[1]);
    }

  // update the camera
  cam->SetFocalPoint(center[0],center[1],center[2]);
  cam->SetPosition(center[0]+distance*vn[0],
                   center[1]+distance*vn[1],
                   center[2]+distance*vn[2]);

  ren->ResetCameraClippingRange( bounds );

  // setup default parallel scale
  cam->SetParallelScale(radius);
}

// --------------------------------------------------------------------------
// ctkVTKThumbnailView methods

// --------------------------------------------------------------------------
ctkVTKThumbnailView::ctkVTKThumbnailView(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkVTKThumbnailViewPrivate(*this))
{
  Q_D(ctkVTKThumbnailView);
  d->init();

  // Hide orientation widget
  this->setOrientationWidgetVisible(false);
}

// --------------------------------------------------------------------------
ctkVTKThumbnailView::~ctkVTKThumbnailView()
{
}

//------------------------------------------------------------------------------
void ctkVTKThumbnailView::setRendererToListen(vtkRenderer* newRenderer)
{
  Q_D(ctkVTKThumbnailView);
  d->Renderer = newRenderer;
  vtkRenderWindow* newRenderWindow = newRenderer ? newRenderer->GetRenderWindow() : 0;
  vtkRenderWindowInteractor* newInteractor = newRenderWindow ? newRenderWindow->GetInteractor() : 0;
  this->qvtkReconnect(d->Interactor, newInteractor,
                      vtkCommand::EndInteractionEvent, this,SLOT(updateCamera()));
  d->Interactor = newInteractor;
  d->initCamera();
  d->updateBounds();
  d->updateCamera();
}

//---------------------------------------------------------------------------
void ctkVTKThumbnailView::checkAbort()
{
  if (this->renderWindow()->GetEventPending())
    {
    this->renderWindow()->SetAbortRender(1);
    }
}

//---------------------------------------------------------------------------
void ctkVTKThumbnailView::updateCamera()
{
  Q_D(ctkVTKThumbnailView);
  d->updateCamera();
  this->scheduleRender();
}

//---------------------------------------------------------------------------
void ctkVTKThumbnailView::updateBounds()
{
  Q_D(ctkVTKThumbnailView);
  d->updateBounds();
  this->scheduleRender();
}
