/*=========================================================================

  Library:   CTK
 
  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

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

// VTK includes
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>

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
}

// --------------------------------------------------------------------------
void ctkVTKRenderViewPrivate::setupCornerAnnotation()
{
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
  CTK_P(ctkVTKRenderView);
  p->setInteractor(this->RenderWindow->GetInteractor());
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

// --------------------------------------------------------------------------
ctkVTKRenderView::~ctkVTKRenderView()
{
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKRenderView, vtkRenderWindowInteractor*, interactor, CurrentInteractor);

//----------------------------------------------------------------------------
void ctkVTKRenderView::scheduleRender()
{
  CTK_D(ctkVTKRenderView);
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
  d->RenderWindow->Render();
  d->RenderPending = false;
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::setInteractor(vtkRenderWindowInteractor* newInteractor)
{
  Q_ASSERT(newInteractor);
  CTK_D(ctkVTKRenderView);
  d->RenderWindow->SetInteractor(newInteractor);
  d->Orientation->SetOrientationMarker(d->Axes);
  d->Orientation->SetInteractor(newInteractor);
  d->Orientation->SetEnabled(1);
  d->Orientation->InteractiveOff();
  d->CurrentInteractor = newInteractor; 
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::setCornerAnnotationText(const QString& text)
{
  CTK_D(ctkVTKRenderView);
  d->CornerAnnotation->ClearAllTexts();
  d->CornerAnnotation->SetText(2, text.toLatin1());
}

// --------------------------------------------------------------------------
void ctkVTKRenderView::setBackgroundColor(double r, double g, double b)
{
  CTK_D(ctkVTKRenderView);
  double background_color[3] = {r, g, b};
  d->Renderer->SetBackground(background_color);
}

//----------------------------------------------------------------------------
void ctkVTKRenderView::resetCamera()
{
  CTK_D(ctkVTKRenderView);
  d->Renderer->ResetCamera();
}
