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
#include <QDebug>
#include <QResizeEvent>

// CTK includes
#include "ctkVTKSliceView.h"
#include "ctkVTKSliceView_p.h"
#include "ctkLogger.h"

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

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKSliceView");
//--------------------------------------------------------------------------

// --------------------------------------------------------------------------
// ctkVTKSliceViewPrivate methods

// --------------------------------------------------------------------------
ctkVTKSliceViewPrivate::ctkVTKSliceViewPrivate()
{
  this->RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  this->LightBoxRendererManager = vtkSmartPointer<vtkLightBoxRendererManager>::New();
  this->RenderPending = false;
  this->RenderEnabled = false;
}

//---------------------------------------------------------------------------
void ctkVTKSliceViewPrivate::setupRendering()
{
  Q_ASSERT(this->RenderWindow);
  this->RenderWindow->SetAlphaBitPlanes(1);
  this->RenderWindow->SetMultiSamples(0);
  this->RenderWindow->StereoCapableWindowOn();

  this->LightBoxRendererManager->Initialize(this->RenderWindow);

  this->VTKWidget->SetRenderWindow(this->RenderWindow);
}

//---------------------------------------------------------------------------
// ctkVTKSliceView methods

// --------------------------------------------------------------------------
ctkVTKSliceView::ctkVTKSliceView(QWidget* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkVTKSliceView);
  CTK_D(ctkVTKSliceView);
  
  d->VTKWidget = new QVTKWidget(this);
  this->setLayout(new QVBoxLayout);
  this->layout()->setMargin(0);
  this->layout()->setSpacing(0);
  this->layout()->addWidget(d->VTKWidget);

  d->setupRendering();

  d->LightBoxRendererManager->SetRenderWindowLayout(1, 1);
}

// --------------------------------------------------------------------------
ctkVTKSliceView::~ctkVTKSliceView()
{
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::scheduleRender()
{
  CTK_D(ctkVTKSliceView);

  logger.trace("scheduleRender");
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
void ctkVTKSliceView::forceRender()
{
  CTK_D(ctkVTKSliceView);
  if (!d->RenderEnabled)
    {
    return;
    }
  logger.trace("forceRender");
  d->RenderWindow->Render();
  d->RenderPending = false;
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKSliceView, vtkRenderWindow*, renderWindow, RenderWindow);

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKSliceView, vtkLightBoxRendererManager*,
            lightBoxRendererManager, LightBoxRendererManager);

//----------------------------------------------------------------------------
CTK_SET_CXX(ctkVTKSliceView, bool, setRenderEnabled, RenderEnabled);
CTK_GET_CXX(ctkVTKSliceView, bool, renderEnabled, RenderEnabled);

//----------------------------------------------------------------------------
vtkRenderWindowInteractor* ctkVTKSliceView::interactor() const
{
  CTK_D(const ctkVTKSliceView);
  return d->RenderWindow->GetInteractor();
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setInteractor(vtkRenderWindowInteractor* newInteractor)
{
  CTK_D(const ctkVTKSliceView);
  d->RenderWindow->SetInteractor(newInteractor);
}

//----------------------------------------------------------------------------
vtkInteractorObserver* ctkVTKSliceView::interactorStyle()const
{
  CTK_D(const ctkVTKSliceView);
  if (!d->RenderWindow->GetInteractor())
    {
    return 0;
    }
  return d->RenderWindow->GetInteractor()->GetInteractorStyle();
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::resetCamera()
{
  CTK_D(ctkVTKSliceView);
  d->LightBoxRendererManager->ResetCamera();
}


//----------------------------------------------------------------------------
void ctkVTKSliceView::setImageData(vtkImageData* newImageData)
{
  CTK_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetImageData(newImageData);
}

//----------------------------------------------------------------------------
QString ctkVTKSliceView::cornerAnnotationText()const
{
  CTK_D(const ctkVTKSliceView);
  return QString::fromStdString(d->LightBoxRendererManager->GetCornerAnnotationText());
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setCornerAnnotationText(const QString& text)
{
  CTK_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetCornerAnnotationText(text.toStdString());
}

//----------------------------------------------------------------------------
QColor ctkVTKSliceView::backgroundColor()const
{
  CTK_D(const ctkVTKSliceView);
  double* color = d->LightBoxRendererManager->GetBackgroundColor();
  QColor c;
  c.setRgbF(color[0], color[1], color[2]);
  return c;
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setBackgroundColor(const QColor& newBackgroundColor)
{
  CTK_D(ctkVTKSliceView);
  double color[3];
  color[0] = newBackgroundColor.redF();
  color[1] = newBackgroundColor.greenF();
  color[2] = newBackgroundColor.blueF();
  d->LightBoxRendererManager->SetBackgroundColor(color);
}

//----------------------------------------------------------------------------
ctkVTKSliceView::RenderWindowLayoutType ctkVTKSliceView::renderWindowLayoutType()const
{
  CTK_D(const ctkVTKSliceView);
  return static_cast<ctkVTKSliceView::RenderWindowLayoutType>(
      d->LightBoxRendererManager->GetRenderWindowLayoutType());
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setRenderWindowLayoutType(RenderWindowLayoutType layoutType)
{
  CTK_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetRenderWindowLayoutType(layoutType);
}

//----------------------------------------------------------------------------
double ctkVTKSliceView::colorLevel()const
{
  CTK_D(const ctkVTKSliceView);
  return d->LightBoxRendererManager->GetColorLevel();
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setColorLevel(double newColorLevel)
{
  CTK_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetColorLevel(newColorLevel);
}

//----------------------------------------------------------------------------
double ctkVTKSliceView::colorWindow()const
{
  CTK_D(const ctkVTKSliceView);
  return d->LightBoxRendererManager->GetColorWindow();
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setColorWindow(double newColorWindow)
{
  CTK_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetColorWindow(newColorWindow);
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::resizeEvent(QResizeEvent * event)
{
  this->QWidget::resizeEvent(event);
  emit this->resized(event->size(), event->oldSize());
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setLightBoxRendererManagerRowCount(int newRowCount)
{
  CTK_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetRenderWindowRowCount(newRowCount);
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setLightBoxRendererManagerColumnCount(int newColumnCount)
{
  CTK_D(ctkVTKSliceView);
  d->LightBoxRendererManager->SetRenderWindowColumnCount(newColumnCount);
}

