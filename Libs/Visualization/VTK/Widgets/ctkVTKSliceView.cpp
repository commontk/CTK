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
#include <QResizeEvent>

// CTK includes
#include "ctkPimpl.h"
#include "ctkVTKSliceView.h"
#include "ctkVTKSliceView_p.h"

// VTK includes
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>

// --------------------------------------------------------------------------
// ctkVTKSliceViewPrivate methods

// --------------------------------------------------------------------------
ctkVTKSliceViewPrivate::ctkVTKSliceViewPrivate(ctkVTKSliceView& object)
  : ctkVTKAbstractViewPrivate(object)
{
  this->LightBoxRendererManager = vtkSmartPointer<vtkLightBoxRendererManager>::New();
  this->OverlayRenderer = vtkSmartPointer<vtkRenderer>::New();
}

// --------------------------------------------------------------------------
void ctkVTKSliceViewPrivate::setupCornerAnnotation()
{
  this->ctkVTKAbstractViewPrivate::setupCornerAnnotation();
  this->LightBoxRendererManager->SetCornerAnnotation(this->CornerAnnotation);
}

//---------------------------------------------------------------------------
void ctkVTKSliceViewPrivate::setupRendering()
{
  Q_ASSERT(this->RenderWindow);
  this->RenderWindow->SetNumberOfLayers(2);

  // Initialize light box
  this->LightBoxRendererManager->Initialize(this->RenderWindow);
  this->LightBoxRendererManager->SetRenderWindowLayout(1, 1);

  // Setup overlay renderer
  this->OverlayRenderer->SetLayer(1);
  this->RenderWindow->AddRenderer(this->OverlayRenderer);
  // Parallel projection is needed to prevent actors from warping/tilting
  // when they are near the edge of the window.
  vtkCamera* camera = this->OverlayRenderer->GetActiveCamera();
  if (camera)
    {
    camera->ParallelProjectionOn();
    }

  // Create cornerAnnotation and set its default property
  this->OverlayCornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  this->OverlayCornerAnnotation->SetMaximumLineHeight(0.07);
  vtkTextProperty *tprop = this->OverlayCornerAnnotation->GetTextProperty();
  tprop->ShadowOn();
  this->OverlayCornerAnnotation->ClearAllTexts();

  // Add corner annotation to overlay renderer
  this->OverlayRenderer->AddViewProp(this->OverlayCornerAnnotation);

  this->ctkVTKAbstractViewPrivate::setupRendering();
}

//---------------------------------------------------------------------------
// ctkVTKSliceView methods

// --------------------------------------------------------------------------
ctkVTKSliceView::ctkVTKSliceView(QWidget* parentWidget)
  : Superclass(new ctkVTKSliceViewPrivate(*this), parentWidget)
{
  Q_D(ctkVTKSliceView);
  d->init();
  this->VTKWidget()->installEventFilter(this);
}

// --------------------------------------------------------------------------
ctkVTKSliceView::~ctkVTKSliceView()
{
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setActiveCamera(vtkCamera * newActiveCamera)
{
  Q_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetActiveCamera(newActiveCamera);
  d->OverlayRenderer->SetActiveCamera(newActiveCamera);
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKSliceView, vtkLightBoxRendererManager*,
            lightBoxRendererManager, LightBoxRendererManager);

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKSliceView, vtkRenderer*, overlayRenderer, OverlayRenderer);

//----------------------------------------------------------------------------
void ctkVTKSliceView::resetCamera()
{
  Q_D(ctkVTKSliceView);
  d->OverlayRenderer->ResetCamera();
  d->LightBoxRendererManager->ResetCamera();
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setImageDataConnection(vtkAlgorithmOutput* newImageDataPort)
{
  Q_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetImageDataConnection(newImageDataPort);
}

//----------------------------------------------------------------------------
vtkCornerAnnotation * ctkVTKSliceView::overlayCornerAnnotation()const
{
  Q_D(const ctkVTKSliceView);
  return d->OverlayCornerAnnotation;
}

//----------------------------------------------------------------------------
QColor ctkVTKSliceView::backgroundColor()const
{
  Q_D(const ctkVTKSliceView);
  double* color = d->LightBoxRendererManager->GetBackgroundColor();
  QColor c;
  c.setRgbF(color[0], color[1], color[2]);
  return c;
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setBackgroundColor(const QColor& newBackgroundColor)
{
  Q_D(ctkVTKSliceView);
  double color[3];
  color[0] = newBackgroundColor.redF();
  color[1] = newBackgroundColor.greenF();
  color[2] = newBackgroundColor.blueF();
  d->LightBoxRendererManager->SetBackgroundColor(color);
}

//----------------------------------------------------------------------------
QColor ctkVTKSliceView::highlightedBoxColor()const
{
  Q_D(const ctkVTKSliceView);
  double* color = d->LightBoxRendererManager->GetHighlightedBoxColor();
  QColor c;
  c.setRgbF(color[0], color[1], color[2]);
  return c;
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setHighlightedBoxColor(const QColor& newHighlightedBoxColor)
{
  Q_D(ctkVTKSliceView);
  double color[3];
  color[0] = newHighlightedBoxColor.redF();
  color[1] = newHighlightedBoxColor.greenF();
  color[2] = newHighlightedBoxColor.blueF();
  d->LightBoxRendererManager->SetHighlightedBoxColor(color);
}

//----------------------------------------------------------------------------
ctkVTKSliceView::RenderWindowLayoutType ctkVTKSliceView::renderWindowLayoutType()const
{
  Q_D(const ctkVTKSliceView);
  return static_cast<ctkVTKSliceView::RenderWindowLayoutType>(
      d->LightBoxRendererManager->GetRenderWindowLayoutType());
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setRenderWindowLayoutType(RenderWindowLayoutType layoutType)
{
  Q_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetRenderWindowLayoutType(layoutType);
}

//----------------------------------------------------------------------------
double ctkVTKSliceView::colorLevel()const
{
  Q_D(const ctkVTKSliceView);
  return d->LightBoxRendererManager->GetColorLevel();
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setColorLevel(double newColorLevel)
{
  Q_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetColorLevel(newColorLevel);
}

//----------------------------------------------------------------------------
double ctkVTKSliceView::colorWindow()const
{
  Q_D(const ctkVTKSliceView);
  return d->LightBoxRendererManager->GetColorWindow();
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setColorWindow(double newColorWindow)
{
  Q_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetColorWindow(newColorWindow);
}

//----------------------------------------------------------------------------
bool ctkVTKSliceView::eventFilter(QObject *object, QEvent *event)
{
  if (object == this->VTKWidget())
    {
    if (event->type() == QEvent::Resize)
      {
      QResizeEvent * resizeEvent = dynamic_cast<QResizeEvent*>(event);
      object->event(event);
      emit this->resized(resizeEvent->size());
      return true;
      }
    return false;
    }
  else
    {
    return this->Superclass::eventFilter(object, event);
    }
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setLightBoxRendererManagerRowCount(int newRowCount)
{
  Q_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetRenderWindowRowCount(newRowCount);
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setLightBoxRendererManagerColumnCount(int newColumnCount)
{
  Q_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetRenderWindowColumnCount(newColumnCount);
}

