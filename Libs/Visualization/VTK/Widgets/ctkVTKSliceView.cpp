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
#include "ctkVTKSliceView.h"
#include "ctkVTKSliceView_p.h"

// VTK includes
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkCamera.h>
#include <vtkImageData.h>

// --------------------------------------------------------------------------
// RenderWindowItem methods

//-----------------------------------------------------------------------------
RenderWindowItem::RenderWindowItem(const QColor& rendererBackgroundColor,
                                   double colorWindow, double colorLevel)
{
  // Instanciate an image mapper
  this->ImageMapper = vtkSmartPointer<vtkImageMapper>::New();
  this->ImageMapper->SetColorWindow(colorWindow);
  this->ImageMapper->SetColorLevel(colorLevel);

  // .. and its corresponding 2D actor
  vtkSmartPointer<vtkActor2D> actor2D =  vtkSmartPointer<vtkActor2D>::New();
  actor2D->SetMapper(this->ImageMapper);
  actor2D->GetProperty()->SetDisplayLocationToBackground();

  // Instanciate a renderer
  this->Renderer = vtkSmartPointer<vtkRenderer>::New();
  this->Renderer->SetBackground(rendererBackgroundColor.redF(),
                                rendererBackgroundColor.greenF(),
                                rendererBackgroundColor.blueF());

  // .. and add it the 2D actor
  this->Renderer->AddActor2D(actor2D);
}

//-----------------------------------------------------------------------------
void RenderWindowItem::setViewport(double xMin, double yMin,
                                   double viewportWidth, double viewportHeight)
{
  Q_ASSERT(this->Renderer);
  this->Renderer->SetViewport( xMin, yMin, (xMin + viewportWidth), (yMin + viewportHeight));
}

// --------------------------------------------------------------------------
// ctkVTKSliceViewPrivate methods

// --------------------------------------------------------------------------
ctkVTKSliceViewPrivate::ctkVTKSliceViewPrivate()
{
  //this->Renderer = vtkSmartPointer<vtkRenderer>::New();
  this->RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  this->Axes = vtkSmartPointer<vtkAxesActor>::New();
  this->RenderPending = false;
  this->RenderEnabled = false;
  this->RenderWindowRowCount = 0;
  this->RenderWindowColumnCount = 0;
  this->RenderWindowLayoutType = ctkVTKSliceView::LeftRightTopBottom;
  this->ColorWindow = 255;
  this->ColorLevel = 127.5;
  this->RendererBackgroundColor = QColor(Qt::black);
  //this->ActorCollection = vtkSmartPointer<vtkActor2DCollection>::New();
  //this->PolyDataCollection = vtkSmartPointer<vtkPolyDataCollection>::New();
}

// --------------------------------------------------------------------------
void ctkVTKSliceViewPrivate::setupCornerAnnotation()
{
  foreach(const QSharedPointer<RenderWindowItem>& item, this->RenderWindowItemList)
    {
    if (!item->Renderer->HasViewProp(this->CornerAnnotation))
      {
      item->Renderer->AddViewProp(this->CornerAnnotation);
      this->CornerAnnotation->SetMaximumLineHeight(0.07);
      vtkTextProperty *tprop = this->CornerAnnotation->GetTextProperty();
      tprop->ShadowOn();
      }
    this->CornerAnnotation->ClearAllTexts();
    }
}

//---------------------------------------------------------------------------
void ctkVTKSliceViewPrivate::setupRendering()
{
  CTK_P(ctkVTKSliceView);

  Q_ASSERT(this->RenderWindow);
  this->RenderWindow->SetAlphaBitPlanes(1);
  this->RenderWindow->SetMultiSamples(0);
  this->RenderWindow->StereoCapableWindowOn();
  
  this->RenderWindow->GetRenderers()->RemoveAllItems();
  
  // Compute the width and height of each RenderWindowItem
  double viewportWidth  = 1.0 / static_cast<double>(this->RenderWindowColumnCount);
  double viewportHeight = 1.0 / static_cast<double>(this->RenderWindowRowCount);

  // Postion of the Top-Left corner of the RenderWindowItem
  float xMin, yMin;

  // Loop through RenderWindowItem and set their respective viewport
  for ( int rowId = 0; rowId < this->RenderWindowRowCount; ++rowId )
    {
    yMin = (this->RenderWindowRowCount - 1 - rowId) * viewportHeight;
    xMin = 0.0;

    for ( int columnId = 0; columnId < this->RenderWindowColumnCount; ++columnId )
      {
      this->RenderWindowItemList.at(p->renderWindowItemId(rowId, columnId))->setViewport(
          xMin, yMin, viewportWidth, viewportHeight);
      }
    }

  // Get active camera
  vtkCamera * activeCamera = p->activeCamera();
  Q_ASSERT(activeCamera);
  activeCamera->ParallelProjectionOn();

  // Add render(s) and set their respective active camera
  foreach(const QSharedPointer<RenderWindowItem>& item, this->RenderWindowItemList)
    {
    this->RenderWindow->AddRenderer(item->Renderer);
    item->Renderer->SetActiveCamera(activeCamera);
    }

  this->VTKWidget->SetRenderWindow(this->RenderWindow);
}

//---------------------------------------------------------------------------
void ctkVTKSliceViewPrivate::setupDefaultInteractor()
{
  CTK_P(ctkVTKSliceView);
  p->setInteractor(this->RenderWindow->GetInteractor());
}

// --------------------------------------------------------------------------
void ctkVTKSliceViewPrivate::updateRenderWindowItemZIndex(
    ctkVTKSliceView::RenderWindowLayoutType layoutType)
{
  CTK_P(ctkVTKSliceView);

  for (int rowId = 0; rowId < this->RenderWindowRowCount; ++rowId)
    {
    for (int columnId = 0; columnId < this->RenderWindowColumnCount; ++columnId)
      {
      int itemId = p->renderWindowItemId(rowId, columnId);
      Q_ASSERT(itemId <= this->RenderWindowItemList.size());

      RenderWindowItem * item = this->RenderWindowItemList.at(itemId).data();
      Q_ASSERT(item->ImageMapper->GetInput());

      // Default to ctkVTKSliceView::LeftRightTopBottom
      int zSliceIndex = rowId * this->RenderWindowColumnCount + columnId;

      if (layoutType == ctkVTKSliceView::LeftRightBottomTop)
        {
        zSliceIndex = (this->RenderWindowRowCount - rowId - 1) *
                      this->RenderWindowColumnCount + columnId;
        }

      item->ImageMapper->SetZSlice(zSliceIndex);
      }
    }
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
  d->setupDefaultInteractor();
}

// --------------------------------------------------------------------------
ctkVTKSliceView::~ctkVTKSliceView()
{
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::scheduleRender()
{
  CTK_D(ctkVTKSliceView);
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
  d->RenderWindow->Render();
  d->RenderPending = false;
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKSliceView, vtkRenderWindow*, renderWindow, RenderWindow);

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKSliceView, vtkRenderWindowInteractor*, interactor, CurrentInteractor);

//----------------------------------------------------------------------------
void ctkVTKSliceView::setInteractor(vtkRenderWindowInteractor* newInteractor)
{
  CTK_D(ctkVTKSliceView);
  d->RenderWindow->SetInteractor(newInteractor);
  d->CurrentInteractor = newInteractor;
}

//----------------------------------------------------------------------------
vtkInteractorObserver* ctkVTKSliceView::interactorStyle()
{
  CTK_D(ctkVTKSliceView);
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
void ctkVTKSliceView::setCornerAnnotationText(const QString& text)
{
  CTK_D(ctkVTKSliceView);
  d->CornerAnnotation->ClearAllTexts();
  d->CornerAnnotation->SetText(2, text.toLatin1());
}

//----------------------------------------------------------------------------
QString ctkVTKSliceView::cornerAnnotationText()const
{
  CTK_D(const ctkVTKSliceView);
  return QLatin1String(d->CornerAnnotation->GetText(2));
}

// --------------------------------------------------------------------------
void ctkVTKSliceView::setBackgroundColor(const QColor& newBackgroundColor)
{
  CTK_D(ctkVTKSliceView);

  foreach(const QSharedPointer<RenderWindowItem>& item, d->RenderWindowItemList)
    {
    item->Renderer->SetBackground(newBackgroundColor.redF(),
                                  newBackgroundColor.greenF(),
                                  newBackgroundColor.blueF());
    }

  d->RendererBackgroundColor = newBackgroundColor;
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKSliceView, QColor, backgroundColor, RendererBackgroundColor);

//----------------------------------------------------------------------------
vtkCamera* ctkVTKSliceView::activeCamera()
{
  CTK_D(ctkVTKSliceView);
  if (d->RenderWindowItemList.size() == 0)
    {
    return 0;
    }

  // Obtain reference of the first renderer
  vtkRenderer * firstRenderer = d->RenderWindowItemList.at(0)->Renderer;
  if (firstRenderer->IsActiveCameraCreated())
    {
    return firstRenderer->GetActiveCamera();
    }
  else
    {
    return 0;
    }
  return 0;
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::resetCamera()
{
  CTK_D(ctkVTKSliceView);
  foreach(const QSharedPointer<RenderWindowItem>& item, d->RenderWindowItemList)
    {
    item->Renderer->ResetCamera();
    }
}

//----------------------------------------------------------------------------
int ctkVTKSliceView::rendererCount()
{
  CTK_D(ctkVTKSliceView);
  return d->RenderWindowItemList.size();
}

//----------------------------------------------------------------------------
vtkRenderer* ctkVTKSliceView::renderer(int id)
{
  CTK_D(ctkVTKSliceView);
  if (id < 0 || id >= d->RenderWindowItemList.size())
    {
    return 0;
    }
  return d->RenderWindowItemList.at(id)->Renderer;
}

//----------------------------------------------------------------------------
CTK_SET_CXX(ctkVTKSliceView, bool, setRenderEnabled, RenderEnabled);
CTK_GET_CXX(ctkVTKSliceView, bool, renderEnabled, RenderEnabled);

//----------------------------------------------------------------------------
int ctkVTKSliceView::renderWindowItemId(int rowId, int columnId)
{
  CTK_D(ctkVTKSliceView);
  return d->RenderWindowColumnCount * rowId + columnId;
}

//----------------------------------------------------------------------------
void ctkVTKSliceView::setRenderWindowLayoutType(ctkVTKSliceView::RenderWindowLayoutType layoutType)
{
  CTK_D(ctkVTKSliceView);
  if (d->RenderWindowLayoutType == layoutType)
    {
    return;
    }

  d->updateRenderWindowItemZIndex(layoutType);

  d->RenderWindowLayoutType = layoutType;
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKSliceView, ctkVTKSliceView::RenderWindowLayoutType,
            renderWindowLayoutType, RenderWindowLayoutType);

//----------------------------------------------------------------------------
void ctkVTKSliceView::setRenderWindowLayout(int rowCount, int columnCount)
{
  CTK_D(ctkVTKSliceView);

  // Sanity checks
  Q_ASSERT(rowCount >= 0 && columnCount >= 0);
  if(!(rowCount >= 0 && columnCount >= 0))
    {
    return;
    }

  if (d->RenderWindowRowCount == rowCount && d->RenderWindowColumnCount == columnCount)
    {
    return;
    }

  int extraItem = (rowCount * columnCount) - d->RenderWindowItemList.size();
  if (extraItem > 0)
    {
    // Create extra RenderWindowItem(s)
    while(extraItem > 0)
      {
      d->RenderWindowItemList.append(QSharedPointer<RenderWindowItem>(
          new RenderWindowItem(d->RendererBackgroundColor, d->ColorWindow, d->ColorLevel)));
      }
    }
  else
    {
    // Remove extra RenderWindowItem(s)
    extraItem = qAbs(extraItem);
    while(extraItem > 0)
      {
      d->RenderWindowItemList.removeLast();
      --extraItem;
      }
    }

  d->RenderWindowRowCount = rowCount;
  d->RenderWindowColumnCount = columnCount;

  d->setupRendering();
}

//----------------------------------------------------------------------------
CTK_GET_CXX(ctkVTKSliceView, double, colorWindow, ColorWindow);
CTK_GET_CXX(ctkVTKSliceView, double, colorLevel, ColorLevel);

//----------------------------------------------------------------------------
void ctkVTKSliceView::setImageData(vtkImageData* imageDataToSet)
{
  CTK_D(ctkVTKSliceView);

  foreach(const QSharedPointer<RenderWindowItem>& item, d->RenderWindowItemList)
    {
    item->ImageMapper->SetInput(imageDataToSet);
    }

  d->updateRenderWindowItemZIndex(d->RenderWindowLayoutType);

  d->ImageData = imageDataToSet;
}

